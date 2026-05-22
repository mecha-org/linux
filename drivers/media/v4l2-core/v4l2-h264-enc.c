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
#include <media/videobuf2-v4l2.h>

int v4l2_h264_enc_init(struct v4l2_h264_enc *enc)
{
	struct v4l2_h264_enc_rbsp *rbsp = &enc->rbsp;
	int ret;

	if ((!enc->format && !enc->format_mplane) || !enc->timeperframe ||
	    !enc->ctrl_handler)
		return -EINVAL;

	memset(&enc->state_active, 0, sizeof(enc->state_active));
	memset(&enc->state_next, 0, sizeof(enc->state_next));
	enc->state_serial = 0;

	rbsp->ops = enc->rbsp_ops;
	rbsp->private_data = enc->private_data;

	return 0;
}
EXPORT_SYMBOL_GPL(v4l2_h264_enc_init);

void v4l2_h264_enc_exit(struct v4l2_h264_enc *enc)
{
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

static int state_prepare(struct v4l2_h264_enc *enc)
{
	struct v4l2_h264_enc_state *state = &enc->state_next;
	int ret;

	memset(state, 0, sizeof(*state));

	ret = state_prepare_params(enc);
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

	ret = state_commit(enc);
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

	return 0;
}
EXPORT_SYMBOL_GPL(v4l2_h264_enc_complete);

MODULE_DESCRIPTION("V4L2 H.264 Encode Core");
MODULE_AUTHOR("Paul Kocialkowski <paulk@sys-base.io>");
MODULE_LICENSE("GPL");
