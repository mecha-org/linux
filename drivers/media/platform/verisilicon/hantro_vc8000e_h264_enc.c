// SPDX-License-Identifier: GPL-2.0
/*
 * Hantro VPU codec driver
 *
 * Copyright (C) 2024 Pengutronix, Marco Felsch <kernel@pengutronix.de>
 * Copyright (C) 2025-2026 Paul Kocialkowski <paulk@sys-base.io>
 */

#include <linux/unaligned.h>
#include <linux/delay.h>
#include <media/v4l2-mem2mem.h>
#include <media/v4l2-h264-enc-rbsp.h>

#include "hantro.h"
#include "hantro_hw.h"
#include "hantro_vc8000e_regs.h"

static int
hantro_vc8000e_h264_enc_state_constrain(struct v4l2_h264_enc *enc,
					struct v4l2_h264_enc_state *state)
{
	struct v4l2_ctrl_h264_sps *sps = &state->sps;
	struct v4l2_ctrl_h264_pps *pps = &state->pps;
	struct v4l2_ctrl_h264_encode_params *encode = &state->encode;

	/* SPS */

	if (V4L2_H264_SPS_HAS_CHROMA_FORMAT(sps)) {
		sps->chroma_format_idc = 1;
		sps->flags &= ~V4L2_H264_SPS_FLAG_SEPARATE_COLOUR_PLANE;
		sps->bit_depth_luma_minus8 = 0;
		sps->bit_depth_chroma_minus8 = 0;
		sps->flags &= ~V4L2_H264_SPS_FLAG_QPPRIME_Y_ZERO_TRANSFORM_BYPASS;
	}

	/* Only one reference frame is currently supported. */
	if (sps->max_num_ref_frames > 1)
		sps->max_num_ref_frames = 1;

	if (!(sps->flags & V4L2_H264_SPS_FLAG_FRAME_MBS_ONLY)) {
		sps->flags |= V4L2_H264_SPS_FLAG_FRAME_MBS_ONLY;
		sps->flags &= ~V4L2_H264_SPS_FLAG_MB_ADAPTIVE_FRAME_FIELD;
	}

	sps->flags |= V4L2_H264_SPS_FLAG_DIRECT_8X8_INFERENCE;

	/* PPS */

	/* Only one reference frame is currently supported. */
	pps->num_ref_idx_l0_default_active_minus1 = 0;
	pps->num_ref_idx_l1_default_active_minus1 = 0;

	/* Encode */

	/* Only a single bit is available for idr_pic_id. */
	if (encode->flags & V4L2_H264_ENCODE_FLAG_IDR_PIC)
		encode->idr_pic_id &= 1;

	/* The hardware doesn't allow overriding the active list numbers. */
	if (encode->flags & V4L2_H264_SLICE_FLAG_NUM_REF_IDX_ACTIVE_OVERRIDE) {
		encode->flags &= ~V4L2_H264_SLICE_FLAG_NUM_REF_IDX_ACTIVE_OVERRIDE;

		encode->num_ref_idx_l0_active_minus1 = 0;
		encode->num_ref_idx_l1_active_minus1 = 0;
	}

	/* Only a single bit is available for cabac_init_idc. */
	if (encode->cabac_init_idc > 1)
		encode->cabac_init_idc = 1;

	/* Only a single bit is available for disable_deblocking_filter_idc. */
	if (encode->disable_deblocking_filter_idc > 1)
		encode->disable_deblocking_filter_idc = 1;

	return 0;
}

static int
hantro_vc8000e_h264_enc_rec_buffer_alloc(struct v4l2_h264_enc *enc,
					 struct v4l2_h264_enc_rec_buffer *buffer)
{
	struct hantro_ctx *ctx = enc->private_data;
	struct device *dev = ctx->dev->dev;
	struct hantro_vc8000e_rec_buf *rec_buf;
	unsigned int width_mbs, height_mbs;

	width_mbs = MB_WIDTH(ctx->src_fmt.width);
	height_mbs = MB_HEIGHT(ctx->src_fmt.height);

	rec_buf = kzalloc(sizeof(*rec_buf), GFP_KERNEL);
	if (!rec_buf)
		goto error;

	rec_buf->luma.size = ctx->src_fmt.width * ctx->src_fmt.height;
	rec_buf->luma.cpu = dma_alloc_coherent(dev, rec_buf->luma.size,
					       &rec_buf->luma.dma, GFP_KERNEL);
	if (!rec_buf->luma.cpu)
		goto error;

	rec_buf->chroma.size = rec_buf->luma.size / 2;
	rec_buf->chroma.cpu = dma_alloc_coherent(dev, rec_buf->chroma.size,
						 &rec_buf->chroma.dma,
						 GFP_KERNEL);
	if (!rec_buf->chroma.cpu)
		goto error;

	rec_buf->luma_4n.size = width_mbs * 4 * height_mbs * 4;
	rec_buf->luma_4n.cpu = dma_alloc_coherent(dev, rec_buf->luma_4n.size,
						  &rec_buf->luma_4n.dma,
						  GFP_KERNEL);
	if (!rec_buf->luma_4n.cpu)
		goto error;

	rec_buf->colctbs.size = DIV_ROUND_UP(width_mbs * height_mbs, 2);
	rec_buf->colctbs.cpu = dma_alloc_coherent(dev, rec_buf->colctbs.size,
						  &rec_buf->colctbs.dma,
						  GFP_KERNEL);
	if (!rec_buf->colctbs.cpu)
		goto error;

	buffer->private_data = rec_buf;

	return 0;

error:
	if (rec_buf)
		kfree(rec_buf);

	return -ENOMEM;
}

static int
hantro_vc8000e_h264_enc_rec_buffer_free(struct v4l2_h264_enc *enc,
					struct v4l2_h264_enc_rec_buffer *buffer)
{
	struct hantro_ctx *ctx = enc->private_data;
	struct device *dev = ctx->dev->dev;
	struct hantro_vc8000e_rec_buf *rec_buf = buffer->private_data;

	if (!rec_buf)
		return -EINVAL;

	dma_free_coherent(dev, rec_buf->luma.size, rec_buf->luma.cpu,
			  rec_buf->luma.dma);

	dma_free_coherent(dev, rec_buf->luma_4n.size, rec_buf->luma_4n.cpu,
			  rec_buf->luma_4n.dma);

	dma_free_coherent(dev, rec_buf->chroma.size, rec_buf->chroma.cpu,
			  rec_buf->chroma.dma);

	dma_free_coherent(dev, rec_buf->colctbs.size, rec_buf->colctbs.cpu,
			  rec_buf->colctbs.dma);

	kfree(rec_buf);
	buffer->private_data = NULL;

	return 0;
}

static const struct v4l2_h264_enc_ops hantro_vc8000e_h264_enc_ops = {
	.state_constrain	= hantro_vc8000e_h264_enc_state_constrain,
	.rec_buffer_alloc	= hantro_vc8000e_h264_enc_rec_buffer_alloc,
	.rec_buffer_free	= hantro_vc8000e_h264_enc_rec_buffer_free,
};

int hantro_vc8000e_h264_enc_init(struct hantro_ctx *ctx)
{
	struct hantro_h264_enc_hw_ctx *h264_ctx = &ctx->h264_enc;
	struct hantro_aux_buf *nal_tbl = &h264_ctx->nal_tbl;
	struct v4l2_h264_enc *enc = &h264_ctx->enc;
	struct device *dev = ctx->dev->dev;

	nal_tbl->size = ALIGN(MB_HEIGHT(ctx->src_fmt.height), 8);
	nal_tbl->cpu = dma_alloc_coherent(dev, nal_tbl->size,
					  &nal_tbl->dma, GFP_KERNEL);
	if (!nal_tbl->cpu)
		return -ENOMEM;

	enc->ops = &hantro_vc8000e_h264_enc_ops;
	enc->private_data = ctx;
	enc->format_mplane = &ctx->dst_fmt;
	enc->timeperframe = &ctx->dst_timeperframe;
	enc->ctrl_handler = &ctx->ctrl_handler;
	enc->ref_slots_count_init = 2;
	enc->flags = V4L2_H264_ENC_FLAG_INTER_PRED |
		     V4L2_H264_ENC_FLAG_HW_SLICE_HEADER;

	return v4l2_h264_enc_init(enc);
}

void hantro_vc8000e_h264_enc_exit(struct hantro_ctx *ctx)
{
	struct hantro_h264_enc_hw_ctx *h264_ctx = &ctx->h264_enc;
	struct hantro_aux_buf *nal_tbl = &h264_ctx->nal_tbl;
	struct device *dev = ctx->dev->dev;

	if (nal_tbl->cpu)
		dma_free_coherent(dev, nal_tbl->size, nal_tbl->cpu,
				  nal_tbl->dma);

	v4l2_h264_enc_exit(&h264_ctx->enc);
}

static int ref_setup(struct hantro_ctx *ctx)
{
	struct hantro_h264_enc_hw_ctx *h264_ctx = &ctx->h264_enc;
	struct hantro_vc8000e_regs *regs = &h264_ctx->vc8000e_regs;
	struct v4l2_h264_enc *enc = &h264_ctx->enc;
	struct v4l2_h264_enc_ref *ref = &enc->ref;
	struct v4l2_h264_enc_state *state = &enc->state_active;
	struct v4l2_ctrl_h264_encode_params *encode = &state->encode;
	struct v4l2_h264_dpb_entry *dpb_entry;
	struct v4l2_h264_enc_rec_buffer *buffer;
	struct hantro_vc8000e_rec_buf *rec_buf;
	bool ltr_present = false;
	unsigned int index;

	regs->swreg193.nal_ref_idc = encode->nal_ref_idc != 0;

	if (encode->flags & V4L2_H264_ENCODE_FLAG_LONG_TERM_REFERENCE) {
		regs->swreg198.mark_current_longterm = 1;
		/* We only get a single long-term reference without MMCO. */
		regs->swreg194.cur_longtermidx = 0;
	}

	regs->swreg193.l0_used_by_next_pic0 = 1;
	regs->swreg193.l0_used_by_next_pic1 = 1;
	regs->swreg194.l1_used_by_next_pic0 = 1;
	regs->swreg194.l1_used_by_next_pic1 = 1;

	regs->swreg17.active_l0_cnt = ref->l0_active_count;
	regs->swreg91.active_l1_cnt = ref->l1_active_count;
	regs->swreg4.active_override_flag = 1;

	if (ref->l0_active_count > 0) {
		index = ref->l0[0].index;
		dpb_entry = &ref->dpb[index];
		buffer = &ref->buffers[index];
		rec_buf = buffer->private_data;

		regs->swreg18.refpic_recon_l0_y0 = rec_buf->luma.dma;
		regs->swreg19.refpic_recon_l0_chroma0 = rec_buf->chroma.dma;
		regs->swreg74.refpic_recon_l0_4n0_base = rec_buf->luma_4n.dma;

		regs->swreg17.l0_used_by_curr_pic0 = 1;
		/* TODO: diff with wrap. */
		regs->swreg17.l0_delta_poc0 = 1;
		regs->swreg193.l0_delta_framenum0 = 1;

		if (dpb_entry->flags & V4L2_H264_DPB_ENTRY_FLAG_LONG_TERM) {
			regs->swreg17.l0_long_term_flag0 = 1;
			regs->swreg198.l0_longtermidx0 = 0;
			ltr_present = true;
		}
	}

	if (ref->l0_active_count > 1) {
		index = ref->l0[1].index;
		dpb_entry = &ref->dpb[index];
		buffer = &ref->buffers[index];
		rec_buf = buffer->private_data;

		regs->swreg20.refpic_recon_l0_y1 = rec_buf->luma.dma;
		regs->swreg21.refpic_recon_l0_chroma1 = rec_buf->chroma.dma;
		regs->swreg76.refpic_recon_l0_4n1_base = rec_buf->luma_4n.dma;

		regs->swreg17.l0_used_by_curr_pic1 = 1;
		/* TODO: diff with wrap. */
		regs->swreg17.l0_delta_poc1 = 1;
		regs->swreg193.l0_delta_framenum1 = 1;

		if (dpb_entry->flags & V4L2_H264_DPB_ENTRY_FLAG_LONG_TERM) {
			regs->swreg17.l0_long_term_flag1 = 1;
			regs->swreg198.l0_longtermidx1 = 0;
			ltr_present = true;
		}
	}

	if (ltr_present)
		regs->swreg91.long_term_ref_pics_present_flag = 1;

	return 0;
}

static int lambda_setup(struct hantro_ctx *ctx)
{
	struct hantro_h264_enc_hw_ctx *h264_ctx = &ctx->h264_enc;
	struct hantro_vc8000e_regs *regs = &h264_ctx->vc8000e_regs;

	/* Intra SATD */

	regs->swreg125.intra_satd_lambda_0 = 0x47;
	regs->swreg125.intra_satd_lambda_1 = 0x3f;
	regs->swreg126.intra_satd_lambda_2 = 0x38;
	regs->swreg126.intra_satd_lambda_3 = 0x32;
	regs->swreg127.intra_satd_lambda_4 = 0;
	regs->swreg127.intra_satd_lambda_5 = 0;
	regs->swreg128.intra_satd_lambda_6 = 0;
	regs->swreg128.intra_satd_lambda_7 = 0;
	regs->swreg129.intra_satd_lambda_8 = 0;
	regs->swreg129.intra_satd_lambda_9 = 0;
	regs->swreg130.intra_satd_lambda_10 = 0;
	regs->swreg130.intra_satd_lambda_11 = 0;
	regs->swreg131.intra_satd_lambda_12 = 0;
	regs->swreg131.intra_satd_lambda_13 = 0;
	regs->swreg132.intra_satd_lambda_14 = 0;
	regs->swreg132.intra_satd_lambda_15 = 0;
	regs->swreg174.intra_satd_lambda_16 = 0x1c4;
	regs->swreg174.intra_satd_lambda_17 = 0x192;
	regs->swreg175.intra_satd_lambda_18 = 0x166;
	regs->swreg175.intra_satd_lambda_19 = 0x13f;
	regs->swreg176.intra_satd_lambda_20 = 0x11c;
	regs->swreg176.intra_satd_lambda_21 = 0xfd;
	regs->swreg177.intra_satd_lambda_22 = 0xe2;
	regs->swreg177.intra_satd_lambda_23 = 0xc9;
	regs->swreg178.intra_satd_lambda_24 = 0xb3;
	regs->swreg178.intra_satd_lambda_25 = 0xa0;
	regs->swreg179.intra_satd_lambda_26 = 0x8e;
	regs->swreg179.intra_satd_lambda_27 = 0x7f;
	regs->swreg180.intra_satd_lambda_28 = 0x71;
	regs->swreg180.intra_satd_lambda_29 = 0x65;
	regs->swreg181.intra_satd_lambda_30 = 0x5a;
	regs->swreg181.intra_satd_lambda_31 = 0x50;

	/* Inter SATD */

	regs->swreg28.lambda_satd_me_0 = 0x24;
	regs->swreg28.lambda_satd_me_1 = 0x20;
	regs->swreg29.lambda_satd_me_2 = 0x1c;
	regs->swreg29.lambda_satd_me_3 = 0x19;
	regs->swreg30.lambda_satd_me_4 = 0x16;
	regs->swreg30.lambda_satd_me_5 = 0x14;
	regs->swreg31.lambda_satd_me_6 = 0x12;
	regs->swreg31.lambda_satd_me_7 = 0x10;
	regs->swreg32.lambda_satd_me_8 = 0xe;
	regs->swreg32.lambda_satd_me_9 = 0xd;
	regs->swreg33.lambda_satd_me_10 = 0xb;
	regs->swreg33.lambda_satd_me_11 = 0xa;
	regs->swreg34.lambda_satd_me_12 = 0x9;
	regs->swreg34.lambda_satd_me_13 = 0x8;
	regs->swreg78.lambda_satd_me_14 = 0x7;
	regs->swreg78.lambda_satd_me_15 = 0x6;
	regs->swreg150.lambda_satd_me_16 = 0;
	regs->swreg150.lambda_satd_me_17 = 0;
	regs->swreg151.lambda_satd_me_18 = 0;
	regs->swreg151.lambda_satd_me_19 = 0;
	regs->swreg152.lambda_satd_me_20 = 0x88;
	regs->swreg152.lambda_satd_me_21 = 0;
	regs->swreg153.lambda_satd_me_22 = 0;
	regs->swreg153.lambda_satd_me_23 = 0;
	regs->swreg154.lambda_satd_me_24 = 0;
	regs->swreg154.lambda_satd_me_25 = 0;
	regs->swreg155.lambda_satd_me_26 = 0;
	regs->swreg155.lambda_satd_me_27 = 0;
	regs->swreg156.lambda_satd_me_28 = 0;
	regs->swreg156.lambda_satd_me_29 = 0;
	regs->swreg157.lambda_satd_me_30 = 0;
	regs->swreg157.lambda_satd_me_31 = 0;

	/* Inter SSE */

	regs->swreg79.lambda_sse_me_0 = 0x4f;
	regs->swreg122.lambda_sse_me_1 = 0x3f;
	regs->swreg123.lambda_sse_me_2 = 0x32;
	regs->swreg124.lambda_sse_me_3 = 0x28;
	regs->swreg138.lambda_sse_me_4 = 0;
	regs->swreg139.lambda_sse_me_5 = 0;
	regs->swreg140.lambda_sse_me_6 = 0;
	regs->swreg141.lambda_sse_me_7 = 0;
	regs->swreg142.lambda_sse_me_8 = 0;
	regs->swreg143.lambda_sse_me_9 = 0;
	regs->swreg144.lambda_sse_me_10 = 0;
	regs->swreg145.lambda_sse_me_11 = 0;
	regs->swreg146.lambda_sse_me_12 = 0;
	regs->swreg147.lambda_sse_me_13 = 0;
	regs->swreg148.lambda_sse_me_14 = 0;
	regs->swreg149.lambda_sse_me_15 = 0;
	regs->swreg158.lambda_sse_me_16 = 0;
	regs->swreg159.lambda_sse_me_17 = 0x800;
	regs->swreg160.lambda_sse_me_18 = 0;
	regs->swreg161.lambda_sse_me_19 = 0;
	regs->swreg162.lambda_sse_me_20 = 0;
	regs->swreg163.lambda_sse_me_21 = 0;
	regs->swreg164.lambda_sse_me_22 = 0;
	regs->swreg165.lambda_sse_me_23 = 0;
	regs->swreg166.lambda_sse_me_24 = 0;
	regs->swreg167.lambda_sse_me_25 = 0;
	regs->swreg168.lambda_sse_me_26 = 0x13c;
	regs->swreg169.lambda_sse_me_27 = 0xfb;
	regs->swreg172.lambda_sse_me_30 = 0x7d;
	regs->swreg173.lambda_sse_me_31 = 0x64;

	regs->swreg35.lambda_motion_sse = 0;

	if (regs->swreg214.hwabsqpsupport) {
		/* Used for lambda calculation, different intra/inter value. */
		regs->swreg170_qp_absolute.sse_qp_factor = 0x1f5c;
		regs->swreg171_qp_absolute.sad_qp_factor = 0x2ccd;
	}

	return 0;
}

static int areas_setup(struct hantro_ctx *ctx)
{
	struct hantro_h264_enc_hw_ctx *h264_ctx = &ctx->h264_enc;
	struct hantro_vc8000e_regs *regs = &h264_ctx->vc8000e_regs;

	/* Intra */

	regs->swreg23.intra_area_left = 0xff;
	regs->swreg195.intra_area_left_msb = 1;
	regs->swreg249.intra_area_left_msb2 = 1;

	regs->swreg23.intra_area_right = 0xff;
	regs->swreg195.intra_area_right_msb = 1;
	regs->swreg249.intra_area_right_msb2 = 1;

	regs->swreg23.intra_area_top = 0xff;
	regs->swreg195.intra_area_top_msb = 1;
	regs->swreg249.intra_area_top_msb2 = 1;

	regs->swreg23.intra_area_bottom = 0xff;
	regs->swreg195.intra_area_bottom_msb = 1;
	regs->swreg249.intra_area_bottom_msb2 = 1;

	/* IPCM1 */

	regs->swreg208_h264.ipcm1_left = 0x1ff;
	regs->swreg249.ipcm1_left_msb = 1;

	regs->swreg209.ipcm1_right = 0x1ff;
	regs->swreg249.ipcm1_right_msb = 1;

	regs->swreg209.ipcm1_top = 0x1ff;
	regs->swreg209.ipcm1_bottom = 0x1ff;

	regs->swreg249.ipcm1_top_msb = 1;
	regs->swreg249.ipcm1_bottom_msb = 1;

	/* IPCM2 */

	regs->swreg210.ipcm2_left = 0x1ff;
	regs->swreg249.ipcm2_left_msb = 1;

	regs->swreg211.ipcm2_right = 0x1ff;
	regs->swreg249.ipcm2_right_msb = 1;

	regs->swreg212.ipcm2_top = 0x1ff;
	regs->swreg249.ipcm2_top_msb = 1;

	regs->swreg213.ipcm2_bottom = 0x1ff;
	regs->swreg249.ipcm2_bottom_msb = 1;

	/* ROI1 */

	regs->swreg24.roi1_left = 0xff;
	regs->swreg195.roi1_left_msb = 1;
	regs->swreg249.roi1_left_msb2 = 1;

	regs->swreg24.roi1_right = 0xff;
	regs->swreg195.roi1_right_msb = 1;
	regs->swreg249.roi1_right_msb2 = 1;

	regs->swreg24.roi1_top = 0xff;
	regs->swreg195.roi1_top_msb = 1;
	regs->swreg249.roi1_top_msb2 = 1;

	regs->swreg24.roi1_bottom = 0xff;
	regs->swreg195.roi1_bottom_msb = 1;
	regs->swreg249.roi1_bottom_msb2 = 1;

	/* ROI2 */

	regs->swreg25.roi2_left = 0xff;
	regs->swreg195.roi2_left_msb = 1;
	regs->swreg249.roi2_left_msb2 = 1;

	regs->swreg25.roi2_right = 0xff;
	regs->swreg195.roi2_right_msb = 1;
	regs->swreg249.roi2_right_msb2 = 1;

	regs->swreg25.roi2_top = 0xff;
	regs->swreg195.roi2_top_msb = 1;
	regs->swreg249.roi2_top_msb2 = 1;

	regs->swreg25.roi2_bottom = 0xff;
	regs->swreg195.roi2_bottom_msb = 1;
	regs->swreg249.roi2_bottom_msb2 = 1;

	if (regs->swreg226.hwroi8support) {
		/* ROI3 */

		regs->swreg252.roi3_left = 0x3ff;
		regs->swreg252.roi3_right = 0x3ff;
		regs->swreg252.roi3_top = 0x3ff;
		regs->swreg253.roi3_bottom = 0x3ff;

		/* ROI4 */

		regs->swreg253.roi4_left = 0x3ff;
		regs->swreg254.roi4_right = 0x3ff;
		regs->swreg253.roi4_top = 0x3ff;
		regs->swreg254.roi4_bottom = 0x3ff;

		/* ROI5 */

		regs->swreg254.roi5_left = 0x3ff;
		regs->swreg255.roi5_right = 0x3ff;
		regs->swreg255.roi5_top = 0x3ff;
		regs->swreg255.roi5_bottom = 0x3ff;

		/* ROI6 */

		regs->swreg256.roi6_left = 0x3ff;
		regs->swreg256.roi6_right = 0x3ff;
		regs->swreg256.roi6_top = 0x3ff;
		regs->swreg257.roi6_bottom = 0x3ff;

		/* ROI7 */

		regs->swreg257.roi7_left = 0x3ff;
		regs->swreg258.roi7_right = 0x3ff;
		regs->swreg257.roi7_top = 0x3ff;
		regs->swreg258.roi7_bottom = 0x3ff;

		/* ROI8 */

		regs->swreg258.roi8_left = 0x3ff;
		regs->swreg259.roi8_right = 0x3ff;
		regs->swreg259.roi8_top = 0x3ff;
		regs->swreg259.roi8_bottom = 0x3ff;
	}

	return 0;
}

int hantro_vc8000e_h264_enc_run(struct hantro_ctx *ctx)
{
	struct hantro_dev *vpu = ctx->dev;
	struct v4l2_pix_format_mplane *src_fmt = &ctx->src_fmt;
	struct hantro_h264_enc_hw_ctx *h264_ctx = &ctx->h264_enc;
	struct hantro_vc8000e_regs *regs = &h264_ctx->vc8000e_regs;
	struct v4l2_h264_enc *enc = &h264_ctx->enc;
	struct v4l2_h264_enc_state *state = &enc->state_active;
	const struct v4l2_ctrl_h264_sps *sps;
	const struct v4l2_ctrl_h264_pps *pps;
	const struct v4l2_ctrl_h264_encode_params *encode;
	struct vb2_v4l2_buffer *src_buf, *dst_buf;
	struct hantro_vc8000e_rec_buf *rec_buf;
	const struct v4l2_format_info *info;
	unsigned int luma_stride;
	unsigned int chroma_stride;
	int ret;

	hantro_start_prepare_run(ctx);

	info = v4l2_format_info(src_fmt->pixelformat);
	if (!info)
		return -EINVAL;

	src_buf = hantro_get_src_buf(ctx);
	dst_buf = hantro_get_dst_buf(ctx);

	ret = v4l2_h264_enc_step(enc, dst_buf);
	if (ret)
		return ret;

	sps = &state->sps;
	pps = &state->pps;
	encode = &state->encode;

	memset(regs, 0, sizeof(*regs));

	/* Read relevant read-only registers. */
	hantro_vc8000e_swreg_read(vpu, regs, swreg0);
	hantro_vc8000e_swreg_read(vpu, regs, swreg80);
	hantro_vc8000e_swreg_read(vpu, regs, swreg214);
	hantro_vc8000e_swreg_read(vpu, regs, swreg226);
	hantro_vc8000e_swreg_read(vpu, regs, swreg287);

	/* Mode */

	if (!regs->swreg80.hwh264support)
		return -ENODEV;

	regs->swreg4.mode = HANTRO_VC8000E_SWREG4_MODE_H264;

	/* Input */

	regs->swreg38.input_format = ctx->vpu_src_fmt->enc_fmt;
	regs->swreg38.input_rotation = HANTRO_VC8000E_SWREG38_INPUT_ROTATION_0;

	luma_stride = src_fmt->plane_fmt[0].bytesperline;

	/*
	 * The hardware seems to expect the luma stride to represent pixels per
	 * line for packed cases, instead of the usual bytes per line.
	 */
	if (info->comp_planes == 1)
		luma_stride /= info->bpp[0];

	regs->swreg210.input_lu_stride = luma_stride;

	if (info->comp_planes > 1) {
		if (src_fmt->num_planes > 1)
			chroma_stride = src_fmt->plane_fmt[1].bytesperline;
		else if (info->comp_planes > 2)
			chroma_stride = luma_stride / 2;
		else
			chroma_stride = luma_stride;

		regs->swreg211.input_ch_stride = chroma_stride;
	}

	regs->swreg12.input_y_base =
		vb2_dma_contig_plane_dma_addr(&src_buf->vb2_buf, 0);

	if (info->comp_planes > 1) {
		if (src_fmt->num_planes > 1)
			regs->swreg13.input_cb_base =
				vb2_dma_contig_plane_dma_addr(&src_buf->vb2_buf,
							      1);
		else
			regs->swreg13.input_cb_base =
				vb2_dma_contig_plane_dma_addr(&src_buf->vb2_buf,
							      0) +
				luma_stride * src_fmt->height;
	}

	if (info->comp_planes > 2) {
		if (src_fmt->num_planes > 1)
			regs->swreg14.input_cr_base =
				vb2_dma_contig_plane_dma_addr(&src_buf->vb2_buf,
							      2);
		else
			regs->swreg14.input_cr_base =
				vb2_dma_contig_plane_dma_addr(&src_buf->vb2_buf,
							      0) +
				luma_stride * src_fmt->height +
				chroma_stride * src_fmt->height;
	}

	/* Output */

	if (enc->rbsp_update & V4L2_H264_ENC_RBSP_UPDATE_START_CODE)
		regs->swreg4.output_strm_mode =
			HANTRO_VC8000E_SWREG4_OUTPUT_STRM_MODE_BYTE_STREAM;
	else
		regs->swreg4.output_strm_mode =
			HANTRO_VC8000E_SWREG4_OUTPUT_STRM_MODE_NAL_STREAM;

	regs->swreg8.output_strm_base =
		vb2_dma_contig_plane_dma_addr(&dst_buf->vb2_buf, 0) +
		v4l2_h264_enc_rbsp_bytes_count(&enc->rbsp);
	regs->swreg9.output_strm_buffer_limit =
		vb2_plane_size(&dst_buf->vb2_buf, 0) -
		v4l2_h264_enc_rbsp_bytes_count(&enc->rbsp);

	if (!regs->swreg9.output_strm_buffer_limit)
		return -ENOMEM;

	regs->swreg10.size_tbl_base = h264_ctx->nal_tbl.dma;
	regs->swreg6.nal_size_write = 1;

	regs->swreg196.num_ctb_rows_per_sync = 1;

	regs->swreg199.hash_type = HANTRO_VC8000E_SWREG199_HASH_TYPE_NONE;

	/* Picture */

	regs->swreg5.pic_width = src_fmt->width / 8;
	regs->swreg5.pic_height = src_fmt->height / 8;
	regs->swreg38.rowlength = src_fmt->width;

	regs->swreg281.chroma_format_idc = sps->chroma_format_idc;
	regs->swreg38.output_bitwidth_lum =
		HANTRO_VC8000E_SWREG38_OUTPUT_BITWIDTH_LUM_8_BIT;

	regs->swreg11.poc = enc->ref.pic_order_cnt;
	regs->swreg277.pic_order_cnt_type = sps->pic_order_cnt_type;
	if (!sps->pic_order_cnt_type)
		regs->swreg277.log2_max_pic_order_cnt_lsb =
			sps->log2_max_pic_order_cnt_lsb_minus4 + 4;

	regs->swreg192.framenum = encode->frame_num;
	regs->swreg277.log2_max_frame_num = sps->log2_max_frame_num_minus4 + 4;

	/* Reconstruction */

	regs->swreg212.ref_lu_stride = src_fmt->width * 4;
	regs->swreg237.ref_ch_stride = src_fmt->width * 4;
	regs->swreg213.ref_ds_lu_stride = src_fmt->width;

	rec_buf = enc->ref.buffer_current.private_data;

	regs->swreg15.recon_y_base = rec_buf->luma.dma;
	regs->swreg16.recon_chroma_base = rec_buf->chroma.dma;
	regs->swreg72.recon_luma_4n_base = rec_buf->luma_4n.dma;
	regs->swreg114.colctbs_store_base = rec_buf->colctbs.dma;

	/* Reference */

	ret = ref_setup(ctx);
	if (ret)
		return ret;

	/* Slice */

	if (encode->slice_type == V4L2_H264_SLICE_TYPE_I)
		regs->swreg5.frame_coding_type =
			HANTRO_VC8000E_SWREG5_FRAME_CODING_TYPE_I;
	else if (encode->slice_type == V4L2_H264_SLICE_TYPE_P)
		regs->swreg5.frame_coding_type =
			HANTRO_VC8000E_SWREG5_FRAME_CODING_TYPE_P;
	else
		return -EINVAL;

	if (encode->flags & V4L2_H264_ENCODE_FLAG_IDR_PIC) {
		regs->swreg191.nal_unit_type = V4L2_H264_NALU_TYPE_SLICE_IDR;
		regs->swreg193.idr_pic_id = encode->idr_pic_id;
	} else {
		regs->swreg191.nal_unit_type =
			V4L2_H264_NALU_TYPE_SLICE_NON_IDR;
	}

	regs->swreg191.pps_id = encode->pic_parameter_set_id;

	/* Quantization */

	regs->swreg172.qp_min = state->qp_min;
	regs->swreg173.qp_max = state->qp_max;
	regs->swreg7.pic_init_qp = pps->pic_init_qp_minus26 + 26;
	regs->swreg7.pic_qp = enc->rc.qp;
	regs->swreg4.chroma_qp_offset = pps->chroma_qp_index_offset;

	/* Coding */

	if (pps->flags & V4L2_H264_PPS_FLAG_ENTROPY_CODING_MODE) {
		regs->swreg193.entropy_coding_mode = 1;
		regs->swreg7.cabac_init_flag = encode->cabac_init_idc;
	}

	if (pps->flags & V4L2_H264_PPS_FLAG_TRANSFORM_8X8_MODE)
		regs->swreg193.transform8x8_enable = 1;

	if (pps->flags & V4L2_H264_PPS_FLAG_DEBLOCKING_FILTER_CONTROL_PRESENT) {
		regs->swreg5.pps_deblocking_filter_override_enabled_flag = 1;
		regs->swreg5.slice_deblocking_filter_override_flag = 1;

		regs->swreg6.deblocking_filter_dis =
			encode->disable_deblocking_filter_idc;
		regs->swreg6.deblocking_tc_offset =
			encode->slice_alpha_c0_offset_div2;
		regs->swreg6.deblocking_beta_offset =
			encode->slice_beta_offset_div2;
	}

	regs->swreg4.min_trb_size = HANTRO_VC8000E_SWREG4_TRB_SIZE_4X4;
	regs->swreg4.max_trb_size = HANTRO_VC8000E_SWREG4_TRB_SIZE_16X16;
	regs->swreg4.min_cb_size = HANTRO_VC8000E_SWREG4_CB_SIZE_8X8;
	regs->swreg4.max_cb_size = HANTRO_VC8000E_SWREG4_CB_SIZE_16X16;
	regs->swreg4.max_trans_hierarchy_depth_inter = 2;
	regs->swreg4.max_trans_hierarchy_depth_intra = 1;

	regs->swreg36.bits_est_1n_cu_penalty = 15;
	regs->swreg35.bits_est_tu_split_penalty = 3;

	regs->swreg35.bits_est_bias_intra_cu_8 = 22;
	regs->swreg35.bits_est_bias_intra_cu_16 = 40;
	regs->swreg36.bits_est_bias_intra_cu_32 = 86;
	regs->swreg36.bits_est_bias_intra_cu_64 = 304;
	regs->swreg36.inter_skip_bias = 124;

	regs->swreg201.mean_thr0 = 5;
	regs->swreg201.mean_thr1 = 5;
	regs->swreg201.mean_thr2 = 5;
	regs->swreg201.mean_thr3 = 5;

	regs->swreg203_h264.lum_dc_sum_thr = 5;
	regs->swreg203_h264.cb_dc_sum_thr = 1;
	regs->swreg203_h264.cr_dc_sum_thr = 1;

	regs->swreg26.intra_size_factor_0 = 506;
	regs->swreg26.intra_size_factor_1 = 506;
	regs->swreg26.intra_size_factor_2 = 709;
	regs->swreg27.intra_size_factor_3 = 709;

	regs->swreg27.intra_mode_factor_0 = 24;
	regs->swreg27.intra_mode_factor_1 = 12;
	regs->swreg27.intra_mode_factor_2 = 48;

	regs->swreg182.qp_delta_gain = 313;

	/* Lambda */

	ret = lambda_setup(ctx);
	if (ret)
		return ret;

	/* Areas */

	ret = areas_setup(ctx);
	if (ret)
		return ret;

	/* Urgent thresholds */

	regs->swreg272.wr_urgent_disable_threshold =
		HANTRO_VC8000E_SWREG272_URGENT_THRESHOLD_DISABLE;
	regs->swreg272.wr_urgent_enable_threshold =
		HANTRO_VC8000E_SWREG272_URGENT_THRESHOLD_DISABLE;
	regs->swreg272.rd_urgent_disable_threshold =
		HANTRO_VC8000E_SWREG272_URGENT_THRESHOLD_DISABLE;
	regs->swreg272.rd_urgent_enable_threshold =
		HANTRO_VC8000E_SWREG272_URGENT_THRESHOLD_DISABLE;

	/* AXI bus */

	regs->swreg81.max_burst = 16;
	regs->swreg261.axi_read_outstanding_num = 64;
	regs->swreg246.axi_write_outstanding_num = 64;
	regs->swreg320.axi_burst_align_rd_lu_ref_prefetch = 1;

	/* Automatic clock gating */

	regs->swreg3.clock_gate_inter_h264_e = 1;
	regs->swreg3.clock_gate_inter_h265_e = 1;
	regs->swreg3.clock_gate_inter_e = 1;
	regs->swreg3.clock_gate_encoder_h264_e = 1;
	regs->swreg3.clock_gate_encoder_h265_e = 1;
	regs->swreg3.clock_gate_encoder_e = 1;

	/* IRQ status */

	regs->swreg1.irq = 1;
	regs->swreg1.frame_rdy_status = 1;
	regs->swreg1.bus_error_status = 1;
	regs->swreg1.sw_reset = 1;
	regs->swreg1.buffer_full = 1;
	regs->swreg1.timeout = 1;
	regs->swreg1.irq_line_buffer = 1;
	regs->swreg1.slice_rdy_status = 1;

	/* IRQ */

	regs->swreg1.irq_dis = 0;
	regs->swreg1.timeout_int = 1;

	/* Register write */

	hantro_io_copy(vpu->enc_base, regs, sizeof(*regs));

	hantro_end_prepare_run(ctx);

	/* Enable */

	regs->swreg5.enable = 1;

	hantro_vc8000e_swreg_write(vpu, regs, swreg5);

	return 0;
}

void hantro_vc8000e_h264_enc_done(struct hantro_ctx *ctx)
{
	struct hantro_dev *vpu = ctx->dev;
	struct hantro_h264_enc_hw_ctx *h264_ctx = &ctx->h264_enc;
	struct hantro_vc8000e_regs *regs = &h264_ctx->vc8000e_regs;
	struct v4l2_h264_enc *enc = &h264_ctx->enc;
	struct vb2_v4l2_buffer *dst_buf = hantro_get_dst_buf(ctx);
	u32 bytesused;

	hantro_vc8000e_swreg_read(vpu, regs, swreg9);

	bytesused = regs->swreg9.output_strm_buffer_limit +
		    v4l2_h264_enc_rbsp_bytes_count(&enc->rbsp);

	vb2_set_plane_payload(&dst_buf->vb2_buf, 0, bytesused);

	v4l2_h264_enc_complete(enc, dst_buf);
}
