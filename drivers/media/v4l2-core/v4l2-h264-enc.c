// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * V4L2 H.264 Encode Core
 *
 * Copyright (C) 2025-2026 Paul Kocialkowski <paulk@sys-base.io>
 */

#include <linux/module.h>
#include <linux/v4l2-controls.h>
#include <media/v4l2-h264.h>
#include <media/v4l2-h264-enc.h>
#include <media/v4l2-h264-enc-rbsp.h>
#include <media/v4l2-h264-enc-rc.h>
#include <media/videobuf2-v4l2.h>

static int rec_buffer_alloc(struct v4l2_h264_enc *enc,
			    struct v4l2_h264_enc_rec_buffer *buffer)
{
	int ret;

	ret = v4l2_h264_enc_op(enc, rec_buffer_alloc, buffer);
	if (ret)
		return ret;

	buffer->allocated = true;
	enc->ref.slots_count++;

	return 0;
}

static void rec_buffer_free(struct v4l2_h264_enc *enc,
			    struct v4l2_h264_enc_rec_buffer *buffer)
{
	if (WARN_ON(!enc->ref.slots_count))
		return;

	v4l2_h264_enc_op(enc, rec_buffer_free, buffer);

	buffer->allocated = false;
	enc->ref.slots_count--;
}

static int rec_buffers_alloc(struct v4l2_h264_enc *enc,
			     unsigned int slots_count)
{
	struct v4l2_h264_enc_ref *ref = &enc->ref;
	unsigned int i;
	int ret;

	ret = rec_buffer_alloc(enc, &ref->buffer_current);
	if (ret)
		return ret;

	if (!(enc->flags & (V4L2_H264_ENC_FLAG_INTER_PRED |
			    V4L2_H264_ENC_FLAG_INTER_BIPRED)))
		return 0;

	for (i = 0; i < slots_count; i++) {
		ret = rec_buffer_alloc(enc, &ref->buffers[i]);
		if (ret)
			goto error;
	}

	return 0;

error:
	while (i > 0) {
		i--;
		rec_buffer_free(enc, &ref->buffers[i]);
	}

	rec_buffer_free(enc, &ref->buffer_current);

	return ret;
}

static void rec_buffers_free(struct v4l2_h264_enc *enc)
{
	struct v4l2_h264_enc_ref *ref = &enc->ref;
	unsigned int i;

	rec_buffer_free(enc, &ref->buffer_current);

	if (!(enc->flags & (V4L2_H264_ENC_FLAG_INTER_PRED |
			    V4L2_H264_ENC_FLAG_INTER_BIPRED)))
		return;

	for (i = 0; i < V4L2_H264_NUM_DPB_ENTRIES; i++) {
		if (!ref->buffers[i].allocated)
			continue;

		rec_buffer_free(enc, &ref->buffers[i]);
	}
}

int v4l2_h264_enc_init(struct v4l2_h264_enc *enc)
{
	struct v4l2_h264_enc_rc *rc = &enc->rc;
	struct v4l2_h264_enc_rbsp *rbsp = &enc->rbsp;
	unsigned int slots_count = 0;
	int ret;

	if ((!enc->format && !enc->format_mplane) || !enc->timeperframe ||
	    !enc->ctrl_handler)
		return -EINVAL;

	memset(&enc->state_active, 0, sizeof(enc->state_active));
	memset(&enc->state_next, 0, sizeof(enc->state_next));
	enc->state_serial = 0;

	memset(&enc->ref, 0, sizeof(enc->ref));

	if (enc->flags & (V4L2_H264_ENC_FLAG_INTER_PRED |
			  V4L2_H264_ENC_FLAG_INTER_BIPRED)) {
		if (!enc->ref_slots_count_init)
			slots_count = V4L2_H264_NUM_DPB_ENTRIES / 2;
		else if (WARN_ON(enc->ref_slots_count_init >
				 V4L2_H264_NUM_DPB_ENTRIES))
			slots_count = V4L2_H264_NUM_DPB_ENTRIES;
		else
			slots_count = enc->ref_slots_count_init;
	}

	ret = rec_buffers_alloc(enc, slots_count);
	if (ret)
		return ret;

	rc->ops = enc->rc_ops;
	rc->private_data = enc->private_data;

	ret = v4l2_h264_enc_rc_init(rc);
	if (ret)
		goto error_buffers;

	rbsp->ops = enc->rbsp_ops;
	rbsp->private_data = enc->private_data;

	return 0;

error_buffers:
	rec_buffers_free(enc);

	return ret;
}
EXPORT_SYMBOL_GPL(v4l2_h264_enc_init);

void v4l2_h264_enc_exit(struct v4l2_h264_enc *enc)
{
	struct v4l2_h264_enc_rc *rc = &enc->rc;

	v4l2_h264_enc_rc_exit(rc);
	rec_buffers_free(enc);
}
EXPORT_SYMBOL_GPL(v4l2_h264_enc_exit);

static int state_prepare_params(struct v4l2_h264_enc *enc)
{
	struct v4l2_h264_enc_state *state = &enc->state_next;
	struct v4l2_ctrl_h264_sps *sps = &state->sps;
	struct v4l2_h264_sps_video *sps_video = &state->sps_video;
	struct v4l2_ctrl_h264_pps *pps = &state->pps;
	struct v4l2_ctrl_h264_encode_params *encode = &state->encode;
	struct v4l2_ctrl_handler *handler = enc->ctrl_handler;
	struct v4l2_fract *timeperframe = enc->timeperframe;
	unsigned int width, height;
	unsigned int colorspace, quantization, xfer_func, ycbcr_enc;
	struct v4l2_ctrl *ctrl;

	/* Time per frame */

	state->timeperframe = *timeperframe;

	/* Format */

	if (enc->format_mplane)
		width = enc->format_mplane->width;
	else
		width = enc->format->width;

	state->width_mbs = DIV_ROUND_UP(width, V4L2_H264_ENC_MB_UNIT);
	state->width_aligned = ALIGN(width, V4L2_H264_ENC_MB_UNIT);

	if (enc->format_mplane)
		height = enc->format_mplane->height;
	else
		height = enc->format->height;

	state->height_mbs = DIV_ROUND_UP(height, V4L2_H264_ENC_MB_UNIT);
	state->height_aligned = ALIGN(height, V4L2_H264_ENC_MB_UNIT);

	/* SPS */

	ctrl = v4l2_ctrl_find(handler, V4L2_CID_STATELESS_H264_SPS);
	if (!ctrl)
		return -EINVAL;

	memcpy(sps, ctrl->p_cur.p_h264_sps, sizeof(*sps));

	sps->pic_width_in_mbs_minus1 = state->width_mbs - 1;
	sps->pic_height_in_map_units_minus1 = state->height_mbs - 1;

	/*
	 * Only pic_order_cnt_type = 0 is currently supported.
	 * Error out since no pic_order_cnt_lsb was provided.
	 */
	if (sps->pic_order_cnt_type)
		return -EINVAL;

	/* SPS Video */

	if (width < state->width_aligned || height < state->height_aligned) {
		sps_video->flags |= V4L2_H264_SPS_VIDEO_FLAG_FRAME_CROPPING;

		sps_video->frame_crop_left_offset = 0;
		sps_video->frame_crop_right_offset = (state->width_aligned -
						      width) / 2;
		sps_video->frame_crop_top_offset = 0;
		sps_video->frame_crop_bottom_offset = (state->height_aligned -
						       height) / 2;
	}

	if (timeperframe->numerator && timeperframe->denominator) {
		sps_video->flags |=
			V4L2_H264_SPS_VIDEO_FLAG_VUI_PARAMETERS_PRESENT |
			V4L2_H264_SPS_VIDEO_FLAG_VUI_TIMING_INFO_PRESENT |
			V4L2_H264_SPS_VIDEO_FLAG_VUI_FIXED_FRAME_RATE;

		/* Timing info is always provided as a field rate. */
		sps_video->num_units_in_tick = timeperframe->numerator;
		sps_video->time_scale = timeperframe->denominator * 2;
	}

	ctrl = v4l2_ctrl_find(handler, V4L2_CID_MPEG_VIDEO_H264_VUI_SAR_ENABLE);
	if (ctrl && ctrl->cur.val) {
		sps_video->flags |=
			V4L2_H264_SPS_VIDEO_FLAG_VUI_ASPECT_RATIO_INFO_PRESENT;

		ctrl = v4l2_ctrl_find(handler,
				      V4L2_CID_MPEG_VIDEO_H264_VUI_SAR_IDC);
		if (!ctrl)
			return -EINVAL;

		if (ctrl->cur.val == V4L2_MPEG_VIDEO_H264_VUI_SAR_IDC_EXTENDED)
			sps_video->aspect_ratio_idc =
				V4L2_H264_VUI_ASPECT_RATIO_IDC_EXTENDED;
		else
			sps_video->aspect_ratio_idc = ctrl->cur.val;

		if (sps_video->aspect_ratio_idc ==
		    V4L2_H264_VUI_ASPECT_RATIO_IDC_EXTENDED) {
			ctrl = v4l2_ctrl_find(handler,
					      V4L2_CID_MPEG_VIDEO_H264_VUI_EXT_SAR_WIDTH);
			if (!ctrl)
				return -EINVAL;

			sps_video->sar_width = ctrl->cur.val;

			ctrl = v4l2_ctrl_find(handler,
					      V4L2_CID_MPEG_VIDEO_H264_VUI_EXT_SAR_HEIGHT);
			if (!ctrl)
				return -EINVAL;

			sps_video->sar_height = ctrl->cur.val;
		}
	}

	if (enc->format_mplane) {
		colorspace = enc->format_mplane->colorspace;
		quantization = enc->format_mplane->quantization;
		xfer_func = enc->format_mplane->xfer_func;
		ycbcr_enc = enc->format_mplane->ycbcr_enc;
	} else {
		colorspace = enc->format->colorspace;
		quantization = enc->format->quantization;
		xfer_func = enc->format->xfer_func;
		ycbcr_enc = enc->format->ycbcr_enc;
	}

	if (colorspace != V4L2_COLORSPACE_DEFAULT ||
	    quantization != V4L2_QUANTIZATION_DEFAULT ||
	    xfer_func != V4L2_XFER_FUNC_DEFAULT ||
	    ycbcr_enc != V4L2_YCBCR_ENC_DEFAULT) {
		sps_video->flags |=
			V4L2_H264_SPS_VIDEO_FLAG_VUI_VIDEO_SIGNAL_TYPE_PRESENT |
			V4L2_H264_SPS_VIDEO_FLAG_VUI_COLOUR_DESCRIPTION_PRESENT;

		sps_video->video_format = V4L2_H264_VUI_VIDEO_UNSPECIFIED;

		switch (colorspace) {
		case V4L2_COLORSPACE_SMPTE170M:
			sps_video->colour_primaries =
				V4L2_H264_VUI_COLOUR_SMPTE170M;
			break;
		case V4L2_COLORSPACE_SMPTE240M:
			sps_video->colour_primaries =
				V4L2_H264_VUI_COLOUR_SMPTE240M;
			break;
		case V4L2_COLORSPACE_REC709:
			sps_video->colour_primaries =
				V4L2_H264_VUI_COLOUR_BT709;
			break;
		case V4L2_COLORSPACE_470_SYSTEM_M:
			sps_video->colour_primaries =
				V4L2_H264_VUI_COLOUR_BT470_SYSTEM_M;
			break;
		case V4L2_COLORSPACE_470_SYSTEM_BG:
			sps_video->colour_primaries =
				V4L2_H264_VUI_COLOUR_BT470_SYSTEM_BG;
			break;
		case V4L2_COLORSPACE_BT2020:
			sps_video->colour_primaries =
				V4L2_H264_VUI_COLOUR_BT2020;
			break;
		case V4L2_COLORSPACE_JPEG:
			sps_video->colour_primaries =
				V4L2_H264_VUI_COLOUR_BT709;
			break;
		case V4L2_COLORSPACE_SRGB:
			sps_video->colour_primaries =
				V4L2_H264_VUI_COLOUR_BT709;
			break;
		default:
			sps_video->colour_primaries =
				V4L2_H264_VUI_COLOUR_UNSPECIFIED;
			break;
		}

		if (quantization == V4L2_QUANTIZATION_FULL_RANGE)
			sps_video->flags |=
				V4L2_H264_SPS_VIDEO_FLAG_VUI_VIDEO_FULL_RANGE;

		switch (xfer_func) {
		case V4L2_XFER_FUNC_709:
			/*
			 * All of these are equivalent but the H.264
			 * specification allows being more specific.
			 */
			switch (colorspace) {
			case V4L2_COLORSPACE_SMPTE170M:
				sps_video->transfer_characteristics =
					V4L2_H264_VUI_TRANSFER_SMPTE170M;
				break;
			case V4L2_COLORSPACE_470_SYSTEM_M:
				sps_video->transfer_characteristics =
					V4L2_H264_VUI_TRANSFER_BT470_SYSTEM_M;
				break;
			case V4L2_COLORSPACE_470_SYSTEM_BG:
				sps_video->transfer_characteristics =
					V4L2_H264_VUI_TRANSFER_BT470_SYSTEM_BG;
				break;
			default:
				sps_video->transfer_characteristics =
					V4L2_H264_VUI_TRANSFER_BT709;
				break;
			}
			break;
		case V4L2_XFER_FUNC_SRGB:
			sps_video->transfer_characteristics =
				V4L2_H264_VUI_TRANSFER_SRGB;
			break;
		case V4L2_XFER_FUNC_SMPTE240M:
			sps_video->transfer_characteristics =
				V4L2_H264_VUI_TRANSFER_SMPTE240M;
			break;
		case V4L2_XFER_FUNC_NONE:
			sps_video->transfer_characteristics =
				V4L2_H264_VUI_TRANSFER_LINEAR;
			break;
		default:
			sps_video->transfer_characteristics =
				V4L2_H264_VUI_TRANSFER_UNSPECIFIED;
			break;
		}

		switch (ycbcr_enc) {
		case V4L2_YCBCR_ENC_601:
			/*
			 * All of these are equivalent but the H.264
			 * specification allows being more specific.
			 */
			switch (colorspace) {
			case V4L2_COLORSPACE_SMPTE170M:
				sps_video->matrix_coefficients =
					V4L2_H264_VUI_MATRIX_SMPTE170M;
				break;
			case V4L2_COLORSPACE_470_SYSTEM_M:
				sps_video->matrix_coefficients =
					V4L2_H264_VUI_MATRIX_BT470_SYSTEM_M;
				break;
			default:
				sps_video->matrix_coefficients =
					V4L2_H264_VUI_MATRIX_BT470_SYSTEM_BG;
				break;
			}
			break;
		case V4L2_YCBCR_ENC_709:
			sps_video->matrix_coefficients =
				V4L2_H264_VUI_MATRIX_BT709;
			break;
		case V4L2_YCBCR_ENC_SMPTE240M:
			sps_video->matrix_coefficients =
				V4L2_H264_VUI_MATRIX_SMPTE240M;
			break;
		case V4L2_YCBCR_ENC_BT2020:
			sps_video->matrix_coefficients =
				V4L2_H264_VUI_MATRIX_BT2020;
			break;
		case V4L2_YCBCR_ENC_BT2020_CONST_LUM:
			sps_video->matrix_coefficients =
				V4L2_H264_VUI_MATRIX_BT2020_CONST_LUM;
			break;
		default:
			sps_video->matrix_coefficients =
				V4L2_H264_VUI_MATRIX_UNSPECIFIED;
			break;
		}
	}

	/* PPS */

	ctrl = v4l2_ctrl_find(handler, V4L2_CID_STATELESS_H264_PPS);
	if (!ctrl)
		return -EINVAL;

	memcpy(pps, ctrl->p_cur.p_h264_pps, sizeof(*pps));

	/* Only single instances of PPS and SPS are supported. */
	if (pps->seq_parameter_set_id != sps->seq_parameter_set_id)
		pps->seq_parameter_set_id = sps->seq_parameter_set_id;

	pps->flags &= ~V4L2_H264_PPS_FLAG_BOTTOM_FIELD_PIC_ORDER_IN_FRAME_PRESENT;

	/* Slice groups are not supported. */
	pps->num_slice_groups_minus1 = 0;

	if (pps->num_ref_idx_l0_default_active_minus1 >
	    (sps->max_num_ref_frames - 1))
		pps->num_ref_idx_l0_default_active_minus1 =
			sps->max_num_ref_frames - 1;

	if (pps->num_ref_idx_l1_default_active_minus1 >
	    (sps->max_num_ref_frames - 1))
		pps->num_ref_idx_l1_default_active_minus1 =
			sps->max_num_ref_frames - 1;

	pps->flags &= ~V4L2_H264_PPS_FLAG_WEIGHTED_PRED;
	pps->weighted_bipred_idc = 0;

	/* Switching slices are not supported. */
	pps->pic_init_qs_minus26 = 0;

	/* Redundant pictures are not supported. */
	pps->flags &= ~V4L2_H264_PPS_FLAG_REDUNDANT_PIC_CNT_PRESENT;

	/* Scaling matrix is not supported. */
	pps->flags &= ~V4L2_H264_PPS_FLAG_SCALING_MATRIX_PRESENT;

	/*
	 * RBSP always writes the optional second_chroma_qp_index_offset,
	 * which has to take the chroma_qp_index_offset value if unsupported.
	 */
	if (!(enc->flags & V4L2_H264_ENC_FLAG_CHROMA_QP_CR_OFFSET))
		pps->second_chroma_qp_index_offset =
			pps->chroma_qp_index_offset;

	/* Encode */

	ctrl = v4l2_ctrl_find(handler, V4L2_CID_STATELESS_H264_ENCODE_PARAMS);
	if (!ctrl)
		return -EINVAL;

	memcpy(encode, ctrl->p_cur.p_h264_encode_params, sizeof(*encode));

	if (encode->slice_type == V4L2_H264_SLICE_TYPE_SI)
		encode->slice_type = V4L2_H264_SLICE_TYPE_I;
	else if (encode->slice_type == V4L2_H264_SLICE_TYPE_SP)
		encode->slice_type = V4L2_H264_SLICE_TYPE_P;

	/* We cannot safely reuse parameters of a B frane for a P frame. */
	if (encode->slice_type == V4L2_H264_SLICE_TYPE_B &&
	    !(enc->flags & V4L2_H264_ENC_FLAG_INTER_BIPRED))
		return -EINVAL;

	/* We can safely reuse parameters of a P frame for an I frame. */
	if (encode->slice_type == V4L2_H264_SLICE_TYPE_P &&
	    !(enc->flags & V4L2_H264_ENC_FLAG_INTER_PRED))
		encode->slice_type = V4L2_H264_SLICE_TYPE_I;

	if (encode->slice_type != V4L2_H264_SLICE_TYPE_I &&
	    !sps->max_num_ref_frames)
		return -EINVAL;

	/* Ensure the stream starts with an I frame. */
	if (!enc->state_serial && encode->slice_type != V4L2_H264_SLICE_TYPE_I)
		return -EINVAL;

	/* Only single instances of PPS and SPS are supported. */
	if (encode->pic_parameter_set_id != pps->pic_parameter_set_id)
		encode->pic_parameter_set_id = pps->pic_parameter_set_id;

	if (encode->flags & V4L2_H264_ENCODE_FLAG_NUM_REF_IDX_ACTIVE_OVERRIDE) {
		if (encode->num_ref_idx_l0_active_minus1 >
		    (sps->max_num_ref_frames - 1))
			encode->num_ref_idx_l0_active_minus1 =
				sps->max_num_ref_frames - 1;

		if (encode->num_ref_idx_l1_active_minus1 >
		    (sps->max_num_ref_frames - 1))
			encode->num_ref_idx_l1_active_minus1 =
				sps->max_num_ref_frames - 1;
	}

	return 0;
}

static int state_prepare_rc(struct v4l2_h264_enc *enc)
{
	struct v4l2_h264_enc_state *state = &enc->state_next;
	struct v4l2_ctrl_handler *handler = enc->ctrl_handler;
	struct v4l2_ctrl *ctrl;

	ctrl = v4l2_ctrl_find(handler, V4L2_CID_MPEG_VIDEO_FRAME_RC_ENABLE);
	if (ctrl && ctrl->cur.val)
		state->frame_rc_enable = true;
	else
		state->frame_rc_enable = false;

	ctrl = v4l2_ctrl_find(handler, V4L2_CID_MPEG_VIDEO_H264_MIN_QP);
	if (ctrl)
		state->qp_min = ctrl->cur.val;
	else
		state->qp_min = 0;

	ctrl = v4l2_ctrl_find(handler, V4L2_CID_MPEG_VIDEO_H264_MAX_QP);
	if (ctrl)
		state->qp_max = ctrl->cur.val;
	else
		state->qp_max = 51;

	if (!state->frame_rc_enable) {
		ctrl = v4l2_ctrl_find(handler,
				      V4L2_CID_MPEG_VIDEO_H264_I_FRAME_QP);
		if (!ctrl)
			return -EINVAL;

		state->qp_i = ctrl->cur.val;
	}

	if (!state->frame_rc_enable &&
	    (enc->flags & V4L2_H264_ENC_FLAG_INTER_PRED)) {
		ctrl = v4l2_ctrl_find(handler,
				      V4L2_CID_MPEG_VIDEO_H264_P_FRAME_QP);
		if (!ctrl)
			return -EINVAL;

		state->qp_p = ctrl->cur.val;
	}

	if (!state->frame_rc_enable &&
	    (enc->flags & V4L2_H264_ENC_FLAG_INTER_BIPRED)) {
		ctrl = v4l2_ctrl_find(handler,
				      V4L2_CID_MPEG_VIDEO_H264_B_FRAME_QP);
		if (!ctrl)
			return -EINVAL;

		state->qp_b = ctrl->cur.val;
	}

	if (!state->frame_rc_enable)
		return 0;

	ctrl = v4l2_ctrl_find(handler, V4L2_CID_MPEG_VIDEO_BITRATE_MODE);
	if (!ctrl)
		return -EINVAL;

	state->bitrate_mode = ctrl->cur.val;

	if (state->bitrate_mode == V4L2_MPEG_VIDEO_BITRATE_MODE_CQ) {
		ctrl = v4l2_ctrl_find(handler,
				      V4L2_CID_MPEG_VIDEO_CONSTANT_QUALITY);
		if (!ctrl)
			return -EINVAL;

		state->quality = ctrl->cur.val;
		state->quality_min = ctrl->minimum;
		state->quality_max = ctrl->maximum;
	}

	if (state->bitrate_mode == V4L2_MPEG_VIDEO_BITRATE_MODE_CBR ||
	    state->bitrate_mode == V4L2_MPEG_VIDEO_BITRATE_MODE_VBR) {
		ctrl = v4l2_ctrl_find(handler, V4L2_CID_MPEG_VIDEO_BITRATE);
		if (!ctrl)
			return -EINVAL;

		state->bitrate = ctrl->cur.val;
	}

	if (state->bitrate_mode == V4L2_MPEG_VIDEO_BITRATE_MODE_VBR) {
		ctrl = v4l2_ctrl_find(handler,
				      V4L2_CID_MPEG_VIDEO_BITRATE_PEAK);
		if (!ctrl)
			return -EINVAL;

		state->bitrate_peak = ctrl->cur.val;
	}

	return 0;
}

static int state_prepare(struct v4l2_h264_enc *enc)
{
	struct v4l2_h264_enc_state *state = &enc->state_next;
	int ret;

	memset(state, 0, sizeof(*state));

	ret = state_prepare_params(enc);
	if (ret)
		return ret;

	ret = state_prepare_rc(enc);
	if (ret)
		return ret;

	ret = v4l2_h264_enc_op(enc, state_constrain, state);
	if (ret && ret != -EOPNOTSUPP)
		return ret;

	return 0;
}

static void state_debug(struct v4l2_h264_enc *enc)
{
	struct v4l2_h264_enc_state *state = &enc->state_active;
	struct v4l2_ctrl_h264_sps *sps = &state->sps;
	struct v4l2_ctrl_h264_pps *pps = &state->pps;
	struct v4l2_ctrl_h264_encode_params *encode = &state->encode;

	pr_debug("+ v4l2-h264-enc: state");

	pr_debug("  type: %c%s, ref: %s%s",
		 v4l2_h264_slice_type_char(encode->slice_type),
		 encode->flags & V4L2_H264_ENCODE_FLAG_IDR_PIC ? " (IDR)" : "",
		 encode->nal_ref_idc ? "marked" : "unmarked",
		 encode->flags & V4L2_H264_ENCODE_FLAG_LONG_TERM_REFERENCE ?
		 " (long-term)" : "");
	pr_debug("  width: %u mbs, height: %u mbs",
		 sps->pic_width_in_mbs_minus1,
		 sps->pic_height_in_map_units_minus1);
	pr_debug("  profile: %u, level: %u", sps->profile_idc,
		 sps->level_idc);

	pr_debug("  entropy coding: %s",
		 pps->flags & V4L2_H264_PPS_FLAG_ENTROPY_CODING_MODE ? "cabac" :
								       "cavlc");

	if (pps->flags & (V4L2_H264_PPS_FLAG_WEIGHTED_PRED |
			  V4L2_H264_PPS_FLAG_CONSTRAINED_INTRA_PRED |
			  V4L2_H264_PPS_FLAG_TRANSFORM_8X8_MODE |
			  V4L2_H264_PPS_FLAG_SCALING_MATRIX_PRESENT) ||
	    encode->flags & V4L2_H264_ENCODE_FLAG_DIRECT_SPATIAL_MV_PRED)
		pr_debug("  coding features:");

	if (pps->flags & V4L2_H264_PPS_FLAG_WEIGHTED_PRED)
		pr_debug("  - weighted-pred");
	if (pps->flags & V4L2_H264_PPS_FLAG_CONSTRAINED_INTRA_PRED)
		pr_debug("  - constrained-intra-pred");
	if (pps->flags & V4L2_H264_PPS_FLAG_TRANSFORM_8X8_MODE)
		pr_debug("  - transform-8x8");
	if (pps->flags & V4L2_H264_PPS_FLAG_SCALING_MATRIX_PRESENT)
		pr_debug("  - scaling-matrix");
	if (encode->flags & V4L2_H264_ENCODE_FLAG_DIRECT_SPATIAL_MV_PRED)
		pr_debug("  - direct-spatial-mv-pred");
}

static int state_commit(struct v4l2_h264_enc *enc)
{
	struct v4l2_h264_enc_state *state = &enc->state_next;
	struct v4l2_ctrl_handler *handler = enc->ctrl_handler;
	struct v4l2_ctrl *ctrl;
	int ret;

	/* The presence of required controls was checked already. */
	/* TODO: Attach to media request. */

	/* SPS */

	ctrl = v4l2_ctrl_find(handler, V4L2_CID_STATELESS_H264_SPS);
	ret = v4l2_ctrl_s_ctrl_compound(ctrl, V4L2_CTRL_TYPE_H264_SPS,
					&state->sps);
	if (ret)
		return ret;

	/* PPS */

	ctrl = v4l2_ctrl_find(handler, V4L2_CID_STATELESS_H264_PPS);
	ret = v4l2_ctrl_s_ctrl_compound(ctrl, V4L2_CTRL_TYPE_H264_PPS,
					&state->pps);
	if (ret)
		return ret;

	/* Encode */

	ctrl = v4l2_ctrl_find(handler, V4L2_CID_STATELESS_H264_ENCODE_PARAMS);
	ret = v4l2_ctrl_s_ctrl_compound(ctrl, V4L2_CTRL_TYPE_H264_ENCODE_PARAMS,
					&state->encode);
	if (ret)
		return ret;

	/* State */

	memcpy(&enc->state_active, &enc->state_next, sizeof(enc->state_active));

	state_debug(enc);

	return 0;
}

static int state_complete(struct v4l2_h264_enc *enc,
			  struct vb2_v4l2_buffer *buffer)
{

	struct v4l2_h264_enc_state *state = &enc->state_active;
	struct v4l2_ctrl_h264_encode_params *encode = &state->encode;

	if (encode->slice_type == V4L2_H264_SLICE_TYPE_I)
		buffer->flags |= V4L2_BUF_FLAG_KEYFRAME;
	else if (encode->slice_type == V4L2_H264_SLICE_TYPE_P)
		buffer->flags |= V4L2_BUF_FLAG_PFRAME;
	else if (encode->slice_type == V4L2_H264_SLICE_TYPE_B)
		buffer->flags |= V4L2_BUF_FLAG_BFRAME;

	/*
	 * This is only incremented after a successful encode so we can still
	 * detect a stream start case after the first frame failed to encode.
	 */
	enc->state_serial++;

	return 0;
}

static int rbsp_update(struct v4l2_h264_enc *enc)
{
	struct v4l2_h264_enc_state *state = &enc->state_next;
	struct v4l2_h264_enc_state *state_active = &enc->state_active;
	struct v4l2_ctrl_h264_encode_params *encode = &state->encode;
	struct v4l2_ctrl_handler *handler = enc->ctrl_handler;
	struct v4l2_ctrl *ctrl;

	enc->rbsp_update = 0;

	/* Start Code */

	ctrl = v4l2_ctrl_find(handler, V4L2_CID_STATELESS_H264_START_CODE);
	if ((ctrl && ctrl->cur.val == V4L2_STATELESS_H264_START_CODE_ANNEX_B) ||
	    !ctrl)
		enc->rbsp_update |= V4L2_H264_ENC_RBSP_UPDATE_START_CODE;

	/* AUD */

	ctrl = v4l2_ctrl_find(handler, V4L2_CID_MPEG_VIDEO_AU_DELIMITER);
	if (ctrl && ctrl->cur.val)
		enc->rbsp_update |= V4L2_H264_ENC_RBSP_UPDATE_AUD;

	/* SPS */

	if (!enc->state_serial) {
		if (memcmp(&state_active->sps, &state->sps,
			   sizeof(state_active->sps)) ||
		    memcmp(&state_active->sps_video, &state->sps_video,
			   sizeof(state_active->sps_video)))
			enc->rbsp_update |= V4L2_H264_ENC_RBSP_UPDATE_SPS;
	} else {
		enc->rbsp_update |= V4L2_H264_ENC_RBSP_UPDATE_SPS;
	}

	/* PPS */

	if (!enc->state_serial) {
		if (memcmp(&state_active->pps, &state->pps,
			   sizeof(state_active->pps)))
			enc->rbsp_update |= V4L2_H264_ENC_RBSP_UPDATE_PPS;
	} else {
		enc->rbsp_update |= V4L2_H264_ENC_RBSP_UPDATE_PPS;
	}

	/* IDR Prepend */

	ctrl = v4l2_ctrl_find(handler,
			      V4L2_CID_MPEG_VIDEO_PREPEND_SPSPPS_TO_IDR);
	if (ctrl && ctrl->cur.val &&
	    encode->flags & V4L2_H264_ENCODE_FLAG_IDR_PIC)
		enc->rbsp_update |= V4L2_H264_ENC_RBSP_UPDATE_SPS |
				    V4L2_H264_ENC_RBSP_UPDATE_PPS;

	/* Slice */

	enc->rbsp_update |= V4L2_H264_ENC_RBSP_UPDATE_SLICE_HEADER;

	return 0;
}

static int rbsp_step_unit(struct v4l2_h264_enc *enc,
			      unsigned int rbsp_update, unsigned int hw_flag)
{
	struct v4l2_h264_enc_state *state = &enc->state_active;
	struct v4l2_ctrl_h264_sps *sps = &state->sps;
	struct v4l2_h264_sps_video *sps_video = &state->sps_video;
	struct v4l2_ctrl_h264_pps *pps = &state->pps;
	struct v4l2_ctrl_h264_encode_params *encode = &state->encode;
	struct v4l2_h264_enc_rbsp *rbsp = &enc->rbsp;
	u8 primary_pic_type;
	int ret;

	/* Return if no update is needed or if hardware generates the unit. */
	if (!(enc->rbsp_update & rbsp_update) || enc->flags & hw_flag)
		return 0;

	if (enc->rbsp_update & V4L2_H264_ENC_RBSP_UPDATE_START_CODE) {
		ret = v4l2_h264_enc_rbsp_start_code(rbsp);
		if (ret)
			return ret;
	}

	if (rbsp_update == V4L2_H264_ENC_RBSP_UPDATE_AUD) {
		if (enc->flags & V4L2_H264_ENC_FLAG_INTER_BIPRED &&
		    enc->flags & V4L2_H264_ENC_FLAG_INTER_PRED)
			primary_pic_type = V4L2_H264_PRIMARY_PIC_TYPE_IPB;
		else if (enc->flags & V4L2_H264_ENC_FLAG_INTER_PRED)
			primary_pic_type = V4L2_H264_PRIMARY_PIC_TYPE_IP;
		else
			primary_pic_type = V4L2_H264_PRIMARY_PIC_TYPE_I;
	}

	if (rbsp_update == V4L2_H264_ENC_RBSP_UPDATE_AUD)
		return v4l2_h264_enc_rbsp_aud(rbsp, primary_pic_type);
	else if (rbsp_update == V4L2_H264_ENC_RBSP_UPDATE_SPS)
		return v4l2_h264_enc_rbsp_sps(rbsp, sps, sps_video);
	else if (rbsp_update == V4L2_H264_ENC_RBSP_UPDATE_PPS)
		return v4l2_h264_enc_rbsp_pps(rbsp, pps);
	else if (rbsp_update == V4L2_H264_ENC_RBSP_UPDATE_SLICE_HEADER)
		return v4l2_h264_enc_rbsp_slice_header(rbsp, sps, pps, encode);

	return -EINVAL;
}

static int rbsp_step(struct v4l2_h264_enc *enc,
			 struct vb2_v4l2_buffer *buffer)
{
	struct v4l2_h264_enc_rbsp *rbsp = &enc->rbsp;
	void *pointer = vb2_plane_vaddr(&buffer->vb2_buf, 0);
	unsigned int size = vb2_plane_size(&buffer->vb2_buf, 0);
	int ret;

	ret = v4l2_h264_enc_rbsp_init(rbsp, pointer, size);
	if (ret)
		return ret;

	ret = rbsp_step_unit(enc, V4L2_H264_ENC_RBSP_UPDATE_AUD,
			     V4L2_H264_ENC_FLAG_HW_AUD);
	if (ret)
		return ret;

	ret = rbsp_step_unit(enc, V4L2_H264_ENC_RBSP_UPDATE_SPS,
			     V4L2_H264_ENC_FLAG_HW_SPS);
	if (ret)
		return ret;

	ret = rbsp_step_unit(enc, V4L2_H264_ENC_RBSP_UPDATE_PPS,
			     V4L2_H264_ENC_FLAG_HW_PPS);
	if (ret)
		return ret;

	ret = rbsp_step_unit(enc, V4L2_H264_ENC_RBSP_UPDATE_SLICE_HEADER,
			     V4L2_H264_ENC_FLAG_HW_SLICE_HEADER);
	if (ret)
		return ret;

	return 0;
}

static struct v4l2_h264_enc_rec_buffer *
ref_step_buffers_slot_find(struct v4l2_h264_enc *enc)
{
	struct v4l2_h264_enc_rec_buffer *buffer;
	unsigned int i;

	for (i = 0; i < V4L2_H264_NUM_DPB_ENTRIES; i++) {
		buffer = &enc->ref.buffers[i];

		if (buffer->allocated)
			continue;

		return buffer;
	}

	return NULL;
}

static int ref_step_buffers(struct v4l2_h264_enc *enc)
{
	struct v4l2_h264_enc_state *state = &enc->state_active;
	struct v4l2_ctrl_h264_sps *sps = &state->sps;
	struct v4l2_h264_enc_ref *ref = &enc->ref;
	struct v4l2_h264_enc_rec_buffer *buffer;
	unsigned int count;
	unsigned int i;
	int ret;

	/*
	 * Only increase the number of slots. It avoids the cost of free
	 * (including on the first frame) and the cost of possible future
	 * allocations, at the expense of memory.
	 */
	if (sps->max_num_ref_frames <= ref->slots_count)
		return 0;

	count = sps->max_num_ref_frames - ref->slots_count;

	for (i = 0; i < count; i++) {
		buffer = ref_step_buffers_slot_find(enc);
		if (WARN_ON(!buffer))
			return -ENOMEM;

		ret = rec_buffer_alloc(enc, buffer);
		if (ret)
			return ret;
	}

	return 0;
}

static int ref_step_poc(struct v4l2_h264_enc *enc)
{
	struct v4l2_h264_enc_state *state = &enc->state_active;
	struct v4l2_ctrl_h264_sps *sps = &state->sps;
	struct v4l2_ctrl_h264_pps *pps = &state->pps;
	struct v4l2_ctrl_h264_encode_params *encode = &state->encode;
	struct v4l2_h264_enc_ref *ref = &enc->ref;
	unsigned int pic_order_cnt_lsb;
	unsigned int pic_order_cnt_msb;
	unsigned int prev_pic_order_cnt_msb;
	unsigned int prev_pic_order_cnt_lsb;
	unsigned int max_pic_order_cnt_lsb;
	unsigned int delta_pic_order_cnt_bottom;
	unsigned int top_field_order_cnt;
	unsigned int bottom_field_order_cnt;

	/* Only pic_order_cnt_type = 0 is currently supported. */
	if (sps->pic_order_cnt_type)
		return -EINVAL;

	max_pic_order_cnt_lsb = BIT(sps->log2_max_pic_order_cnt_lsb_minus4 + 4);
	pic_order_cnt_lsb = encode->pic_order_cnt_lsb;

	if (encode->flags & V4L2_H264_ENCODE_FLAG_IDR_PIC) {
		prev_pic_order_cnt_msb = 0;
		prev_pic_order_cnt_lsb = 0;
	} else {
		prev_pic_order_cnt_msb = ref->prev_pic_order_cnt_msb;
		prev_pic_order_cnt_lsb = ref->prev_pic_order_cnt_lsb;
	}

	if ((pic_order_cnt_lsb < prev_pic_order_cnt_lsb) &&
	    ((prev_pic_order_cnt_lsb - pic_order_cnt_lsb) >=
	     (max_pic_order_cnt_lsb / 2)))
		pic_order_cnt_msb = prev_pic_order_cnt_msb +
				    max_pic_order_cnt_lsb;
	else if ((pic_order_cnt_lsb > prev_pic_order_cnt_lsb) &&
		 ((pic_order_cnt_lsb - prev_pic_order_cnt_lsb) >
		  (max_pic_order_cnt_lsb / 2)))
		pic_order_cnt_msb = prev_pic_order_cnt_msb -
				    max_pic_order_cnt_lsb;
	else
		pic_order_cnt_msb = prev_pic_order_cnt_msb;

	top_field_order_cnt = pic_order_cnt_msb + pic_order_cnt_lsb;

	if (pps->flags & V4L2_H264_PPS_FLAG_BOTTOM_FIELD_PIC_ORDER_IN_FRAME_PRESENT)
		delta_pic_order_cnt_bottom = encode->delta_pic_order_cnt_bottom;
	else
		delta_pic_order_cnt_bottom = 0;

	if (!(encode->flags & V4L2_H264_ENCODE_FLAG_FIELD_PIC))
		bottom_field_order_cnt = top_field_order_cnt +
					 delta_pic_order_cnt_bottom;
	else
		bottom_field_order_cnt = top_field_order_cnt;

	ref->pic_order_cnt_msb = pic_order_cnt_msb;
	ref->pic_order_cnt_lsb = pic_order_cnt_lsb;
	ref->top_field_order_cnt = top_field_order_cnt;
	ref->bottom_field_order_cnt = bottom_field_order_cnt;
	ref->pic_order_cnt = min(top_field_order_cnt, bottom_field_order_cnt);

	return 0;
}

static int ref_step(struct v4l2_h264_enc *enc)
{
	struct v4l2_h264_enc_state *state = &enc->state_active;
	struct v4l2_ctrl_h264_sps *sps = &state->sps;
	struct v4l2_ctrl_h264_pps *pps = &state->pps;
	struct v4l2_ctrl_h264_encode_params *encode = &state->encode;
	struct v4l2_h264_enc_ref *ref = &enc->ref;
	unsigned int l0_active_count_max;
	unsigned int l1_active_count_max;
	int ret;

	ret = ref_step_buffers(enc);
	if (ret)
		return ret;

	ret = ref_step_poc(enc);
	if (ret)
		return ret;

	ref->l0_active_count = 0;
	ref->l1_active_count = 0;

	if (encode->slice_type == V4L2_H264_SLICE_TYPE_I) {
		/* Flush the DPB on IDR pictures. */
		if (encode->flags & V4L2_H264_ENCODE_FLAG_IDR_PIC)
			memset(ref->dpb, 0, sizeof(ref->dpb));

		return 0;
	}

	/* Generate reference lists. */

	v4l2_h264_init_reflist_builder_gen(&ref->builder, sps, ref->dpb,
					   ref->pic_order_cnt,
					   encode->frame_num,
					   V4L2_H264_FRAME_REF);

	if (encode->flags & V4L2_H264_ENCODE_FLAG_NUM_REF_IDX_ACTIVE_OVERRIDE) {
		l0_active_count_max = encode->num_ref_idx_l0_active_minus1 + 1;
		l1_active_count_max = encode->num_ref_idx_l1_active_minus1 + 1;
	} else {
		l0_active_count_max =
			pps->num_ref_idx_l0_default_active_minus1 + 1;
		l1_active_count_max =
			pps->num_ref_idx_l1_default_active_minus1 + 1;
	}

	switch (encode->slice_type) {
	case V4L2_H264_SLICE_TYPE_P:
		v4l2_h264_build_p_ref_list(&ref->builder, ref->l0);

		if (ref->builder.num_valid > l0_active_count_max)
			ref->l0_active_count = l0_active_count_max;
		else
			ref->l0_active_count = ref->builder.num_valid;

		WARN_ON(!ref->l0_active_count);

		break;
	case V4L2_H264_SLICE_TYPE_B:
		v4l2_h264_build_b_ref_lists(&ref->builder, ref->l0, ref->l1);

		if (ref->builder.num_valid > l0_active_count_max)
			ref->l0_active_count = l0_active_count_max;
		else
			ref->l0_active_count = ref->builder.num_valid;

		WARN_ON(!ref->l0_active_count);

		if (ref->builder.num_valid > l1_active_count_max)
			ref->l1_active_count = l1_active_count_max;
		else
			ref->l1_active_count = ref->builder.num_valid;

		WARN_ON(!ref->l1_active_count);

		break;
	}

	pr_debug("+ v4l2-h264-enc: ref");
	pr_debug("  ref active l0: %u, l1: %u", ref->l0_active_count,
		 ref->l1_active_count);

	return 0;
}

static int ref_complete_slot_find(struct v4l2_h264_enc *enc,
				  unsigned int *index)
{
	struct v4l2_h264_enc_state *state = &enc->state_active;
	struct v4l2_ctrl_h264_sps *sps = &state->sps;
	struct v4l2_ctrl_h264_encode_params *encode = &state->encode;
	struct v4l2_h264_enc_ref *ref = &enc->ref;
	unsigned int max_frame_num = BIT(sps->log2_max_frame_num_minus4 + 4);
	unsigned int frame_num_wrap_smallest = encode->frame_num;
	unsigned int frame_num_wrap_smallest_index;
	unsigned int frame_num_wrap;
	struct v4l2_h264_enc_rec_buffer *buffer;
	struct v4l2_h264_dpb_entry *dpb_entry;
	unsigned int i;

	for (i = 0; i < V4L2_H264_NUM_DPB_ENTRIES; i++) {
		buffer = &ref->buffers[i];
		dpb_entry = &ref->dpb[i];

		if (!buffer->allocated)
			continue;

		/* Return an unused slot. */
		if (!(dpb_entry->flags & V4L2_H264_DPB_ENTRY_FLAG_ACTIVE)) {
			*index = i;
			return 0;
		}

		/* Track the smallest FrameNumWrap value. */
		if (dpb_entry->frame_num > encode->frame_num)
			frame_num_wrap = dpb_entry->frame_num - max_frame_num;
		else
			frame_num_wrap = dpb_entry->frame_num;

		if (frame_num_wrap < frame_num_wrap_smallest) {
			frame_num_wrap_smallest = frame_num_wrap;
			frame_num_wrap_smallest_index = i;
		}
	}

	/* Clear the evicted DPB entry. */
	dpb_entry = &ref->dpb[frame_num_wrap_smallest_index];
	memset(dpb_entry, 0, sizeof(*dpb_entry));

	*index = frame_num_wrap_smallest_index;

	return 0;
}

static int ref_complete_swap(struct v4l2_h264_enc *enc, unsigned int index,
			     u64 ts)
{
	struct v4l2_h264_enc_state *state = &enc->state_active;
	struct v4l2_ctrl_h264_encode_params *encode = &state->encode;
	struct v4l2_h264_enc_ref *ref = &enc->ref;
	struct v4l2_h264_dpb_entry *dpb_entry = &ref->dpb[index];
	struct v4l2_h264_enc_rec_buffer *buffer = &ref->buffers[index];
	struct v4l2_h264_enc_rec_buffer *buffer_current =
		&ref->buffer_current;

	/* Set the DPB entry of the available slot. */
	memset(dpb_entry, 0, sizeof(*dpb_entry));
	dpb_entry->reference_ts = ts;
	dpb_entry->pic_num = encode->frame_num;
	dpb_entry->frame_num = encode->frame_num;
	dpb_entry->fields = V4L2_H264_FRAME_REF;
	dpb_entry->top_field_order_cnt = ref->top_field_order_cnt;
	dpb_entry->bottom_field_order_cnt = ref->bottom_field_order_cnt;
	dpb_entry->flags = V4L2_H264_DPB_ENTRY_FLAG_VALID |
			   V4L2_H264_DPB_ENTRY_FLAG_ACTIVE;

	if (encode->flags & V4L2_H264_ENCODE_FLAG_LONG_TERM_REFERENCE)
		dpb_entry->flags |= V4L2_H264_DPB_ENTRY_FLAG_LONG_TERM;

	/* Swap the current buffer with the available slot. */
	swap(*buffer_current, *buffer);

	return 0;
}

static int ref_complete(struct v4l2_h264_enc *enc,
			struct vb2_v4l2_buffer *buffer)
{
	struct v4l2_h264_enc_state *state = &enc->state_active;
	struct v4l2_ctrl_h264_encode_params *encode = &state->encode;
	struct v4l2_h264_enc_ref *ref = &enc->ref;
	unsigned int index;
	int ret;

	if (!encode->nal_ref_idc)
		return 0;

	/* Keep the POC as last reference. */
	ref->prev_pic_order_cnt_msb = ref->pic_order_cnt_msb;
	ref->prev_pic_order_cnt_lsb = ref->pic_order_cnt_lsb;

	ret = ref_complete_slot_find(enc, &index);
	if (ret)
		return ret;

	/* Move our current picture to the DPB. */
	ret = ref_complete_swap(enc, index, buffer->vb2_buf.timestamp);
	if (ret)
		return ret;

	return 0;
}

static int rc_update(struct v4l2_h264_enc *enc)
{
	struct v4l2_h264_enc_state *state = &enc->state_next;
	struct v4l2_h264_enc_state *state_active = &enc->state_active;
	int ret;

	if (!enc->state_serial ||
	    state_active->frame_rc_enable != state->frame_rc_enable ||
	    (state_active->frame_rc_enable &&
	     state_active->bitrate_mode != state->bitrate_mode)) {
		ret = v4l2_h264_enc_rc_mode_update(&enc->rc);
		if (ret)
			return ret;
	}

	return 0;
}

static int rc_step(struct v4l2_h264_enc *enc)
{
	struct v4l2_h264_enc_state *state = &enc->state_active;
	struct v4l2_ctrl_h264_pps *pps = &state->pps;
	struct v4l2_ctrl_h264_encode_params *encode = &state->encode;
	struct v4l2_h264_enc_rc *rc = &enc->rc;
	int ret;

	ret = v4l2_h264_enc_rc_step(rc, state);
	if (ret)
		return ret;

	encode->slice_qp_delta = rc->qp - (pps->pic_init_qp_minus26 + 26);

	pr_debug("+ v4l2-h264-enc: rc");

	if (rc->mode) {
		pr_debug("  rc mode: %s", rc->mode->name);

		if (rc->mode->type == V4L2_H264_ENC_RC_TYPE_CBR) {
			pr_debug("  rc bitrate: %u bits/s", state->bitrate);
			pr_debug("  rc target: %lu bytes", rc->size);
		}
	}

	pr_debug("  rc qp: %u", rc->qp);

	return 0;
}

static int rc_complete(struct v4l2_h264_enc *enc,
		       struct vb2_v4l2_buffer *buffer)
{
	struct v4l2_h264_enc_rc *rc = &enc->rc;
	unsigned long bytesused;
	int ret;

	bytesused = vb2_get_plane_payload(&buffer->vb2_buf, 0);
	if (WARN_ON(!bytesused))
		return -EINVAL;

	ret = v4l2_h264_enc_rc_complete(rc, bytesused);
	if (ret)
		return ret;

	pr_debug("+ v4l2-h264-enc: complete");
	pr_debug("  size: %lu bytes", bytesused);

	return 0;
}

int v4l2_h264_enc_step(struct v4l2_h264_enc *enc,
		       struct vb2_v4l2_buffer *buffer)
{
	int ret;

	ret = state_prepare(enc);
	if (ret)
		return ret;

	ret = rbsp_update(enc);
	if (ret)
		return ret;

	ret = rc_update(enc);
	if (ret)
		return ret;

	ret = state_commit(enc);
	if (ret)
		return ret;

	ret = ref_step(enc);
	if (ret)
		return ret;

	ret = rc_step(enc);
	if (ret)
		return ret;

	ret = rbsp_step(enc, buffer);
	if (ret)
		return ret;

	return 0;
}
EXPORT_SYMBOL_GPL(v4l2_h264_enc_step);

int v4l2_h264_enc_complete(struct v4l2_h264_enc *enc,
			   struct vb2_v4l2_buffer *buffer)
{
	int ret;

	ret = state_complete(enc, buffer);
	if (ret)
		return ret;

	ret = ref_complete(enc, buffer);
	if (ret)
		return ret;

	ret = rc_complete(enc, buffer);
	if (ret)
		return ret;

	return 0;
}
EXPORT_SYMBOL_GPL(v4l2_h264_enc_complete);

MODULE_DESCRIPTION("V4L2 H.264 Encode Core");
MODULE_AUTHOR("Paul Kocialkowski <paulk@sys-base.io>");
MODULE_LICENSE("GPL");
