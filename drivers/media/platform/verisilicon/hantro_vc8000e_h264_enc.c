// SPDX-License-Identifier: GPL-2.0
/*
 * Hantro VPU codec driver
 *
 * Copyright (C) 2024 Pengutronix, Marco Felsch <kernel@pengutronix.de>
 */

#include <linux/unaligned.h>
#include <media/v4l2-mem2mem.h>

#include "hantro.h"
#include "hantro_hw.h"
#include "hantro_vc8000e_regs.h"

void hantro_vc8000e_h264_enc_done(struct hantro_ctx *ctx)
{
	struct hantro_dev *vpu = ctx->dev;
	u32 bytesused = vepu_read(vpu, 0x24);
	struct vb2_v4l2_buffer *dst_buf = hantro_get_dst_buf(ctx);

	vb2_set_plane_payload(&dst_buf->vb2_buf, 0, bytesused);

	/* TODO: Need to implement feedback param read for userspace */
}

static u32 vc8000e_get_ref_frame_stride(unsigned int width)
{
	/*
	 * Reg stride size value is specified in term of u32 (4bytes).
	 *
	 * TODO: Check if this is correct since we have no links in the i.MX8MP
	 * TRM
	 */
	return width * 4;
}

int hantro_vc8000e_h264_enc_run(struct hantro_ctx *ctx)
{
	struct hantro_h264_enc_hw_ctx *h264_ctx = &ctx->h264_enc;
	struct hantro_h264_enc_ctrls *ctrls = &h264_ctx->ctrls;
	const struct v4l2_ctrl_h264_encode_params *encode_params;
	const struct v4l2_ctrl_h264_encode_rc *encode_rc;
	const struct v4l2_ctrl_h264_sps *sps;
	const struct v4l2_ctrl_h264_pps *pps;
	struct v4l2_pix_format_mplane *src_fmt = &ctx->src_fmt;
	struct vb2_v4l2_buffer *src_buf, *dst_buf;
	struct hantro_dev *vpu = ctx->dev;
	struct hantro_h264_enc_buf *buf;
	u32 mbs_in_col, mbs_in_row;
	u32 val;

	src_buf = hantro_get_src_buf(ctx);
	dst_buf = hantro_get_dst_buf(ctx);

	/* Prepare the H264 encoder context. */
	if (hantro_h264_enc_prepare_run(ctx))
		return -EINVAL;

	encode_params = ctrls->encode;
	encode_rc = ctrls->rc;
	sps = ctrls->sps;
	pps = ctrls->pps;

	mbs_in_row = MB_WIDTH(src_fmt->width);
	mbs_in_col = MB_HEIGHT(src_fmt->height);

	/* Select encoder before writing registers. */
	hantro_reg_write_relaxed(vpu, &vc8000e_mode, VC8000E_ENC_MODE_H264);

	/* AXI bus control */
	hantro_reg_write_relaxed(vpu, &vc8000e_axi_write_id, 0);
	hantro_reg_write_relaxed(vpu, &vc8000e_axi_read_id, 0);
	/*
	 * max. burst size > 16byte seem to hang the system in case of larger
	 * frame sizes. TODO: Gather more information and since the i.MX8MP TRM
	 * specifies a default value of 32byte.
	 */
	hantro_reg_write_relaxed(vpu, &vc8000e_max_burst, 16);
	hantro_reg_write_relaxed(vpu, &vc8000e_axi_read_outstanding_num, 64);
	hantro_reg_write_relaxed(vpu, &vc8000e_axi_write_outstanding_num, 64);
	hantro_reg_write_relaxed(vpu, &vc8000e_rd_urgent_enable_threshold,
				 VC8000E_URGENT_THR_DISABLE);
	hantro_reg_write_relaxed(vpu, &vc8000e_wr_urgent_enable_threshold,
				 VC8000E_URGENT_THR_DISABLE);
	hantro_reg_write_relaxed(vpu, &vc8000e_rd_urgent_disable_threshold,
				 VC8000E_URGENT_THR_DISABLE);
	hantro_reg_write_relaxed(vpu, &vc8000e_wr_urgent_disable_threshold,
				 VC8000E_URGENT_THR_DISABLE);

	/* Endianness */
	/* TODO: Copied from NXP downstream, check if correct */
	hantro_reg_write_relaxed(vpu, &vc8000e_strm_swap, 0);
	hantro_reg_write_relaxed(vpu, &vc8000e_pic_swap, 0);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi_map_qp_delta_map_swap, 0xf);
	hantro_reg_write_relaxed(vpu, &vc8000e_ctb_rc_mem_out_swap, 0);

	/* Input */
	/*
	 * TODO:
	 * Add proper format handling to allow more formats (incl. support for
	 * pre-processor).
	 */
	hantro_reg_write_relaxed(vpu, &vc8000e_input_rotation,
				 VC8000E_INPUT_ROTATE_OFF);
	hantro_reg_write_relaxed(vpu, &vc8000e_input_format,
				 ctx->vpu_src_fmt->enc_fmt);
	hantro_reg_write_relaxed(vpu, &vc8000e_output_bitwidth_lum,
				 VC8000E_OUTPUT_LUMA_8BIT);
	hantro_reg_write_relaxed(vpu, &vc8000e_lumoffset, 0);
	hantro_reg_write_relaxed(vpu, &vc8000e_rowlength, src_fmt->width);
	/* TODO: Copied from downstream */
	hantro_reg_write_relaxed(vpu, &vc8000e_num_ctb_rows_per_sync, 0x1);

	/* TODO: Add CSC */
	hantro_reg_write_relaxed(vpu, &vc8000e_rgbcoeffa, 0);
	hantro_reg_write_relaxed(vpu, &vc8000e_rgbcoeffb, 0);
	hantro_reg_write_relaxed(vpu, &vc8000e_rgbcoeffc, 0);
	hantro_reg_write_relaxed(vpu, &vc8000e_rgbcoeffe, 0);
	hantro_reg_write_relaxed(vpu, &vc8000e_rgbcoefff, 0);
	hantro_reg_write_relaxed(vpu, &vc8000e_rgbcoeffg, 0);
	hantro_reg_write_relaxed(vpu, &vc8000e_rgbcoeffh, 0);
	hantro_reg_write_relaxed(vpu, &vc8000e_rmaskmsb, 0);
	hantro_reg_write_relaxed(vpu, &vc8000e_gmaskmsb, 0);
	hantro_reg_write_relaxed(vpu, &vc8000e_bmaskmsb, 0);

	val = vb2_dma_contig_plane_dma_addr(&src_buf->vb2_buf, 0);
	hantro_reg_write_relaxed(vpu, &vc8000e_input_y_base, val);

	if (src_fmt->num_planes > 1) {
		val = vb2_dma_contig_plane_dma_addr(&src_buf->vb2_buf, 1);
		hantro_reg_write_relaxed(vpu, &vc8000e_input_cb_base, val);
	}

	if (src_fmt->num_planes > 2) {
		val = vb2_dma_contig_plane_dma_addr(&src_buf->vb2_buf, 2);
		hantro_reg_write_relaxed(vpu, &vc8000e_input_cr_base, val);
	}

	/* Reconstruction */
	/* nal_ref_idc != 0 -> store picture to be used as reference later */
	buf = hantro_h264_enc_get_rec_buf(ctx, &src_buf->vb2_buf,
					  encode_params->nal_ref_idc != 0,
					  encode_params->nal_unit_type ==
					  V4L2_H264_NAL_CODED_SLICE_IDR_PIC);
	if (!buf)
		return -EINVAL;
	hantro_reg_write_relaxed(vpu, &vc8000e_recon_y_base, buf->luma.dma);
	hantro_reg_write_relaxed(vpu, &vc8000e_recon_luma_4n_base,
				 buf->luma_4n.dma);
	hantro_reg_write_relaxed(vpu, &vc8000e_recon_chroma_base,
				 buf->chroma.dma);
	/* rate control (must be set?!) */
	hantro_reg_write_relaxed(vpu, &vc8000e_colctbs_store_base,
				 buf->ctb_rc.dma);

	/* Reference */
	hantro_reg_write_relaxed(vpu, &vc8000e_refpic_recon_l0_y0, 0);
	hantro_reg_write_relaxed(vpu, &vc8000e_refpic_recon_l0_chroma0, 0);
	if (encode_params->slice_type == V4L2_H264_SLICE_TYPE_P) {
		buf = hantro_h264_enc_get_ref_buf(ctx,
						  encode_params->reference_ts);
		if (!buf)
			return -EINVAL;
		hantro_reg_write_relaxed(vpu, &vc8000e_refpic_recon_l0_y0,
					 buf->luma.dma);
		hantro_reg_write_relaxed(vpu, &vc8000e_refpic_recon_l0_chroma0,
					 buf->chroma.dma);
	}

	/* Strides */
	hantro_reg_write_relaxed(vpu, &vc8000e_ref_ch_stride,
				 vc8000e_get_ref_frame_stride(src_fmt->width));
	hantro_reg_write_relaxed(vpu, &vc8000e_ref_lu_stride,
				 vc8000e_get_ref_frame_stride(src_fmt->width));
	hantro_reg_write_relaxed(vpu, &vc8000e_input_lu_stride, src_fmt->width);
	hantro_reg_write_relaxed(vpu, &vc8000e_input_ch_stride, src_fmt->width / 2);

	/* Output */
	val = vb2_plane_size(&dst_buf->vb2_buf, 0);
	hantro_reg_write_relaxed(vpu, &vc8000e_output_strm_buffer_limit, val);

	val = vb2_dma_contig_plane_dma_addr(&dst_buf->vb2_buf, 0);
	hantro_reg_write_relaxed(vpu, &vc8000e_output_strm_base, val);

	hantro_reg_write_relaxed(vpu, &vc8000e_size_tbl_base,
				 h264_ctx->nal_tbl.dma);
	hantro_reg_write_relaxed(vpu, &vc8000e_nal_size_write, 1);

	/* Intra coding */
	/*
	 * TODO:
	 * For now use the values from downstream but keep the correct
	 * calls as comment
	 */
	//hantro_reg_write_relaxed(vpu, &vc8000e_intra_area_left, mbs_in_row);
	hantro_reg_write_relaxed(vpu, &vc8000e_intra_area_left, 0xff);
	hantro_reg_write_relaxed(vpu, &vc8000e_intra_area_left_msb, 0x1);
	hantro_reg_write_relaxed(vpu, &vc8000e_intra_area_left_msb2, 0x1);
	//hantro_reg_write_relaxed(vpu, &vc8000e_intra_area_right, mbs_in_row);
	hantro_reg_write_relaxed(vpu, &vc8000e_intra_area_right, 0xff);
	hantro_reg_write_relaxed(vpu, &vc8000e_intra_area_right_msb, 0x1);
	hantro_reg_write_relaxed(vpu, &vc8000e_intra_area_right_msb2, 0x1);
	//hantro_reg_write_relaxed(vpu, &vc8000e_intra_area_top, mbs_in_col);
	hantro_reg_write_relaxed(vpu, &vc8000e_intra_area_top, 0xff);
	hantro_reg_write_relaxed(vpu, &vc8000e_intra_area_top_msb, 0x1);
	hantro_reg_write_relaxed(vpu, &vc8000e_intra_area_top_msb2, 0x1);
	//hantro_reg_write_relaxed(vpu, &vc8000e_intra_area_bottom, mbs_in_col);
	hantro_reg_write_relaxed(vpu, &vc8000e_intra_area_bottom, 0xff);
	hantro_reg_write_relaxed(vpu, &vc8000e_intra_area_bottom_msb, 0x1);
	hantro_reg_write_relaxed(vpu, &vc8000e_intra_area_bottom_msb2, 0x1);

	/* Encoding control */
	hantro_reg_write_relaxed(vpu, &vc8000e_chroma_qp_offset,
				 encode_params->chroma_qp_index_offset);
	hantro_reg_write_relaxed(vpu, &vc8000e_max_trb_size, 2); /* From downstream */
	hantro_reg_write_relaxed(vpu, &vc8000e_max_cb_size, 1); /* From downstream */
	hantro_reg_write_relaxed(vpu, &vc8000e_max_trans_hierarchy_depth_intra, 1);
	hantro_reg_write_relaxed(vpu, &vc8000e_max_trans_hierarchy_depth_inter, 2);
	/* TODO: add flag handling */
	hantro_reg_write_relaxed(vpu, &vc8000e_short_term_ref_pic_set_sps_flag, 1);

	hantro_reg_write_relaxed(vpu, &vc8000e_slice_size, encode_params->slice_size_mb_rows);
	hantro_reg_write_relaxed(vpu, &vc8000e_deblocking_filter_ctrl,
				 encode_params->disable_deblocking_filter_idc);
	hantro_reg_write_relaxed(vpu, &vc8000e_deblocking_tc_offset,
				 encode_params->slice_alpha_c0_offset_div2 * 2);
	hantro_reg_write_relaxed(vpu, &vc8000e_deblocking_beta_offset,
				 encode_params->slice_beta_offset_div2 * 2);
	/* Must be set else the deblocking filter won't work correctly  */
	hantro_reg_write_relaxed(vpu,
				 &vc8000e_slice_deblocking_filter_override_flag,
				 !encode_params->disable_deblocking_filter_idc);

	/* Reference picture list0 config */
	/* TODO: add support for 2nd reference picture */
	if (encode_params->slice_type == V4L2_H264_SLICE_TYPE_I) {
		hantro_reg_write_relaxed(vpu, &vc8000e_active_override_flag, 0);
	} else {
		hantro_reg_write_relaxed(vpu, &vc8000e_active_l0_cnt, 1);
		if (pps->num_ref_idx_l0_default_active_minus1 + 1 != 1)
			hantro_reg_write_relaxed(vpu, &vc8000e_active_override_flag, 1);
	}
	/*
	 * Delta between frame-to-be-encoded and the reference frame in list0.
	 * Since we only support one reference frame and the last encoded frame
	 * is the used as reference for the frame-to-be-encoded, we can set it
	 * to 1. Only exception is frame0 which doesn't have any delta.
	 */
	if (encode_params->frame_num)
		hantro_reg_write_relaxed(vpu, &vc8000e_l0_delta_framenum0, 1);

	hantro_reg_write_relaxed(vpu, &vc8000e_l0_used_by_next_pic0, 0x1); /* TODO: dyn. params */
	hantro_reg_write_relaxed(vpu, &vc8000e_l0_used_by_next_pic1, 0x1); /* TODO: dyn. params */
	hantro_reg_write_relaxed(vpu, &vc8000e_l1_used_by_next_pic0, 0x1); /* TODO: dyn. params */
	hantro_reg_write_relaxed(vpu, &vc8000e_l1_used_by_next_pic1, 0x1); /* TODO: dyn. params */
	hantro_reg_write_relaxed(vpu, &vc8000e_cur_longtermidx, 0x7); /* TODO: dyn. params */

	hantro_reg_write_relaxed(vpu, &vc8000e_idr_pic_id, encode_params->idr_pic_id);
	hantro_reg_write_relaxed(vpu, &vc8000e_nal_ref_idc, encode_params->nal_ref_idc != 0);
	hantro_reg_write_relaxed(vpu, &vc8000e_transform8x8_enable, 0); /* TODO: dyn. params */
	hantro_reg_write_relaxed(vpu, &vc8000e_entropy_coding_mode,
				 pps->flags & V4L2_H264_PPS_FLAG_ENTROPY_CODING_MODE);
	hantro_reg_write_relaxed(vpu, &vc8000e_pps_id, encode_params->pic_parameter_set_id);
	/*
	 * Only I/P frames supported, therefore the frame_num can be used for both
	 * decoding and displaying order.
	 */
	hantro_reg_write_relaxed(vpu, &vc8000e_framenum, encode_params->frame_num);
	hantro_reg_write_relaxed(vpu, &vc8000e_poc , encode_params->frame_num);
	hantro_reg_write_relaxed(vpu, &vc8000e_log2_max_frame_num,
				 sps->log2_max_frame_num_minus4 + 4);
	if (sps->pic_order_cnt_type == 0)
		hantro_reg_write_relaxed(vpu, &vc8000e_log2_max_pic_order_cnt_lsb,
					 sps->log2_max_pic_order_cnt_lsb_minus4 + 4);

	hantro_reg_write_relaxed(vpu, &vc8000e_pic_init_qp,
				 encode_params->pic_init_qp_minus26 + 26);

	hantro_reg_write_relaxed(vpu, &vc8000e_chroma_format_idc,
				 VC8000E_CHROMA_FORMAT_IDC_420);

	/* I(ntra)PCM params copied from downstream. TODO: handle it correctly */
	hantro_reg_write_relaxed(vpu, &vc8000e_ipcm1_left, 0x1ff);
	hantro_reg_write_relaxed(vpu, &vc8000e_ipcm1_left_msb, 0x1);
	hantro_reg_write_relaxed(vpu, &vc8000e_ipcm1_right, 0x1ff);
	hantro_reg_write_relaxed(vpu, &vc8000e_ipcm1_right_msb, 0x1);
	hantro_reg_write_relaxed(vpu, &vc8000e_ipcm1_top, 0x1ff);
	hantro_reg_write_relaxed(vpu, &vc8000e_ipcm1_top_msb, 0x1);
	hantro_reg_write_relaxed(vpu, &vc8000e_ipcm1_bottom, 0x1ff);
	hantro_reg_write_relaxed(vpu, &vc8000e_ipcm1_bottom_msb, 0x1);
	hantro_reg_write_relaxed(vpu, &vc8000e_ipcm2_left, 0x1ff);
	hantro_reg_write_relaxed(vpu, &vc8000e_ipcm2_left_msb, 0x1);
	hantro_reg_write_relaxed(vpu, &vc8000e_ipcm2_right, 0x1ff);
	hantro_reg_write_relaxed(vpu, &vc8000e_ipcm2_right_msb, 0x1);
	hantro_reg_write_relaxed(vpu, &vc8000e_ipcm2_top, 0x1ff);
	hantro_reg_write_relaxed(vpu, &vc8000e_ipcm2_top_msb, 0x1);
	hantro_reg_write_relaxed(vpu, &vc8000e_ipcm2_bottom, 0x1ff);
	hantro_reg_write_relaxed(vpu, &vc8000e_ipcm2_bottom_msb, 0x1);

	/* TODO: At the moment only CAVLC is supported */
	hantro_reg_write_relaxed(vpu, &vc8000e_cabac_init_flag,
				 encode_params->cabac_init_idc);

	/* Intra/inter modes */
	/* TODO: check if correct (taken from downstream) or if we need more user-control */
	hantro_reg_write_relaxed(vpu, &vc8000e_intra_size_factor_0, 506);
	hantro_reg_write_relaxed(vpu, &vc8000e_intra_size_factor_1, 506);
	hantro_reg_write_relaxed(vpu, &vc8000e_intra_size_factor_2, 709);
	hantro_reg_write_relaxed(vpu, &vc8000e_intra_size_factor_3, 709);
	hantro_reg_write_relaxed(vpu, &vc8000e_intra_mode_factor_0, 24);
	hantro_reg_write_relaxed(vpu, &vc8000e_intra_mode_factor_1, 12);
	hantro_reg_write_relaxed(vpu, &vc8000e_intra_mode_factor_2, 48);

	/* QP control */
	/* TODO: must be dynamic?! */
	hantro_reg_write_relaxed(vpu, &vc8000e_rc_qpdelta_range, 10);
	hantro_reg_write_relaxed(vpu, &vc8000e_pic_qp, encode_rc->qp);
	hantro_reg_write_relaxed(vpu, &vc8000e_qp_max, encode_rc->qp_max);
	hantro_reg_write_relaxed(vpu, &vc8000e_qp_min, encode_rc->qp_min);
	hantro_reg_write_relaxed(vpu, &vc8000e_smart_qp, 0x1e);
	hantro_reg_write_relaxed(vpu, &vc8000e_cr_dc_sum_thr, 0x1);
	hantro_reg_write_relaxed(vpu, &vc8000e_cb_dc_sum_thr, 0x1);
	hantro_reg_write_relaxed(vpu, &vc8000e_lum_dc_sum_thr, 0x5);
	hantro_reg_write_relaxed(vpu, &vc8000e_mean_thr0, 0x5);
	hantro_reg_write_relaxed(vpu, &vc8000e_mean_thr1, 0x5);
	hantro_reg_write_relaxed(vpu, &vc8000e_mean_thr2, 0x5);
	hantro_reg_write_relaxed(vpu, &vc8000e_mean_thr3, 0x5);

	/* Regions of interest */
	/* TODO: requires user input */
	hantro_reg_write_relaxed(vpu, &vc8000e_roi1, 0xffffffff);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi1_left_msb, 0x1);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi1_left_msb2, 0x1);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi1_right_msb, 0x1);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi1_right_msb2, 0x1);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi1_top_msb, 0x1);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi1_top_msb2, 0x1);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi1_bottom_msb, 0x1);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi1_bottom_msb2, 0x1);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi1_qp_type,
				 VC8000E_ROI_QP_TYPE_ABS);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi2, 0xffffffff);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi2_left_msb, 0x1);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi2_left_msb2, 0x1);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi2_right_msb, 0x1);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi2_right_msb2, 0x1);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi2_top_msb, 0x1);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi2_top_msb2, 0x1);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi2_bottom_msb, 0x1);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi2_bottom_msb2, 0x1);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi2_qp_type,
				 VC8000E_ROI_QP_TYPE_ABS);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi3_left, 0x3ff);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi3_top, 0x3ff);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi3_right, 0x3ff);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi3_bottom, 0x3ff);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi3_qp_type,
				 VC8000E_ROI_QP_TYPE_ABS);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi4_left, 0x3ff);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi4_top, 0x3ff);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi4_right, 0x3ff);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi4_bottom, 0x3ff);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi4_qp_type,
				 VC8000E_ROI_QP_TYPE_ABS);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi5_left, 0x3ff);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi5_top, 0x3ff);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi5_right, 0x3ff);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi5_bottom, 0x3ff);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi5_qp_type,
				 VC8000E_ROI_QP_TYPE_ABS);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi6_left, 0x3ff);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi6_top, 0x3ff);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi6_right, 0x3ff);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi6_bottom, 0x3ff);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi6_qp_type,
				 VC8000E_ROI_QP_TYPE_ABS);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi7_left, 0x3ff);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi7_top, 0x3ff);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi7_right, 0x3ff);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi7_bottom, 0x3ff);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi7_qp_type,
				 VC8000E_ROI_QP_TYPE_ABS);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi8_left, 0x3ff);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi8_top, 0x3ff);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi8_right, 0x3ff);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi8_bottom, 0x3ff);
	hantro_reg_write_relaxed(vpu, &vc8000e_roi8_qp_type,
				 VC8000E_ROI_QP_TYPE_ABS);

	/* TODO: add correct motion vector, rate-control, QP handling */
#if 0
	/* Motion vector settings */
	vepu_write(vpu, 0x03181600, 4 * 28);
	vepu_write(vpu, 0x02781180, 4 * 29);
	vepu_write(vpu, 0x01f00e00, 4 * 30);
	vepu_write(vpu, 0x01900b00, 4 * 31);
	vepu_write(vpu, 0x013808c0, 4 * 32);
	vepu_write(vpu, 0x00f80700, 4 * 33);
	vepu_write(vpu, 0x00c80580, 4 * 34);
	vepu_write(vpu, 0x00011628, 4 * 35);
	vepu_write(vpu, 0x2b261f3c, 4 * 36);
	vepu_write(vpu, 0x001c0090, 4 * 37);
	vepu_write(vpu, 0x00a00480, 4 * 78);
	vepu_write(vpu, 0x00133800, 4 * 79);

	/* Rate control */
	vepu_write(vpu, 0x000112e3, 4 * 105);

	/* QP/Motion vector settings */
	vepu_write(vpu, 0x000f4000, 4 * 122);
	vepu_write(vpu, 0x000c1800, 4 * 123);
	vepu_write(vpu, 0x00099800, 4 * 124);
	vepu_write(vpu, 0x03180b10, 4 * 125);
	vepu_write(vpu, 0x027408c0, 4 * 126);
	vepu_write(vpu, 0x0a000ccc, 4 * 134);
	vepu_write(vpu, 0x2c000000, 4 * 136);
	vepu_write(vpu, 0x02400000, 4 * 159);
	vepu_write(vpu, 0x004cd000, 4 * 168);
	vepu_write(vpu, 0x003cf800, 4 * 169);
	vepu_write(vpu, 0x00099a01, 4 * 170);
	vepu_write(vpu, 0x0018c901, 4 * 171);
	vepu_write(vpu, 0x001e780f, 4 * 172);
	vepu_write(vpu, 0x0018366a, 4 * 173);
	vepu_write(vpu, 0x13ac4620, 4 * 174);
	vepu_write(vpu, 0x0f9c37a0, 4 * 175);
	vepu_write(vpu, 0x0c642c30, 4 * 176);
	vepu_write(vpu, 0x09d82310, 4 * 177);
	vepu_write(vpu, 0x07d01bd0, 4 * 178);
	vepu_write(vpu, 0x06341610, 4 * 179);
	vepu_write(vpu, 0x04ec1180, 4 * 180);
	vepu_write(vpu, 0x03e80df0, 4 * 181);
	vepu_write(vpu, 0x00000139, 4 * 182);
	vepu_write(vpu, 0x00232800, 4 * 183);
	vepu_write(vpu, 0x004b0000, 4 * 184);
#endif

	/* Interrupt */
	hantro_reg_write_relaxed(vpu, &vc8000e_timeout_int, 1);

	/* Start the hardware. */
	hantro_reg_write_relaxed(vpu, &vc8000e_pic_width, src_fmt->width / 8);
	hantro_reg_write_relaxed(vpu, &vc8000e_pic_height, src_fmt->height / 8);

	if (encode_params->slice_type == V4L2_H264_SLICE_TYPE_I)
		hantro_reg_write_relaxed(vpu, &vc8000e_frame_coding_type,
					 VC8000E_IFRAME_CODING_TYPE);
	else
		hantro_reg_write_relaxed(vpu, &vc8000e_frame_coding_type,
					 VC8000E_PFRAME_CODING_TYPE);

	hantro_reg_write_relaxed(vpu, &vc8000e_nal_unit_type,
				 encode_params->nal_unit_type);

	if (encode_params->nal_unit_type == V4L2_H264_NAL_CODED_SLICE_IDR_PIC)
		dst_buf->flags |= V4L2_BUF_FLAG_KEYFRAME;

	hantro_end_prepare_run(ctx);

	hantro_reg_write(vpu, &vc8000e_e, 1);

	return 0;
}