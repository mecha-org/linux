// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * V4L2 H.264 Encode RBSP
 *
 * Copyright (C) 2025-2026 Paul Kocialkowski <paulk@sys-base.io>
 */

#include <linux/bitops.h>
#include <linux/bits.h>
#include <linux/bug.h>
#include <linux/errno.h>
#include <linux/minmax.h>
#include <linux/string.h>
#include <linux/types.h>
#include <linux/v4l2-controls.h>
#include <media/v4l2-h264.h>
#include <media/v4l2-h264-enc-rbsp.h>

int v4l2_h264_enc_rbsp_init(struct v4l2_h264_enc_rbsp *rbsp, u8 *pointer,
			    unsigned int size)
{
	rbsp->pointer = pointer;
	rbsp->size = size;

	rbsp->bit_offset = 0;
	rbsp->bits_count = 0;
	rbsp->zero_count = 0;

	return 0;
}
EXPORT_SYMBOL_GPL(v4l2_h264_enc_rbsp_init);

unsigned int v4l2_h264_enc_rbsp_bits_count(struct v4l2_h264_enc_rbsp *rbsp)
{
	return rbsp->bits_count;
}
EXPORT_SYMBOL_GPL(v4l2_h264_enc_rbsp_bits_count);

unsigned int v4l2_h264_enc_rbsp_bytes_count(struct v4l2_h264_enc_rbsp *rbsp)
{
	WARN_ON(rbsp->bits_count % 8);

	return rbsp->bits_count / 8;
}
EXPORT_SYMBOL_GPL(v4l2_h264_enc_rbsp_bytes_count);

static int v4l2_h264_enc_rbsp_bits_raw(struct v4l2_h264_enc_rbsp *rbsp,
				       u32 value, unsigned char bits_count)
{
	unsigned int bits_left = bits_count;
	unsigned int bits_chunk;
	unsigned int bit_start;
	unsigned int bit_stop;
	u32 value_extract;
	int ret;

	if (bits_count > 32)
		return -EINVAL;
	else if (!bits_count)
		return 0;

	value &= GENMASK(bits_count - 1, 0);

	/* XXX: Has to manage rbsp->bit_offset and rbsp->bits_count. */
	ret = v4l2_h264_enc_rbsp_op(rbsp, bits_raw, value, bits_count);
	if (!ret)
		return 0;
	else if (ret != -EOPNOTSUPP)
		return ret;

	while (bits_left > 0) {
		if (WARN_ON(rbsp->bit_offset >= 8))
			return -1;

		bits_chunk = min(8u - rbsp->bit_offset, bits_left);

		bit_stop = bits_left - 1;
		bit_start = bit_stop + 1 - bits_chunk;
		value_extract = (value & GENMASK(bit_stop, bit_start)) >>
				bit_start;

		bit_stop = 7u - rbsp->bit_offset;
		bit_start = bit_stop + 1 - bits_chunk;

		*rbsp->pointer &= ~GENMASK(bit_stop, bit_start);
		*rbsp->pointer |= value_extract << bit_start;

		rbsp->bit_offset += bits_chunk;

		if (rbsp->bit_offset == 8) {
			rbsp->pointer++;
			rbsp->bit_offset = 0;
		}

		rbsp->bits_count += bits_chunk;

		if (rbsp->bits_count / 8 >= rbsp->size)
			return -ENOMEM;

		bits_left -= bits_chunk;
	}

	WARN_ON(bits_left);

	return 0;
}

static int eptb_bits(struct v4l2_h264_enc_rbsp *rbsp, unsigned int *bits_left,
		     unsigned int zero_step)
{
	unsigned int zero_discard;
	unsigned int zero_count;
	unsigned int zero_chunk;
	int ret;

	if (!zero_step)
		return 0;

	/* Trailing zeros in a non-zero byte are discarded for EPTB. */
	if (!rbsp->zero_count && rbsp->bit_offset)
		zero_discard = min(8u - rbsp->bit_offset, zero_step);
	else
		zero_discard = 0;

	/* Append discarded zeros before byte alignment. */
	if (zero_discard) {
		ret = v4l2_h264_enc_rbsp_bits_raw(rbsp, 0, zero_discard);
		if (ret)
			return ret;

		*bits_left -= zero_discard;
		zero_step -= zero_discard;
	}

	/* Count relevant zeros for EPTB (starting at byte alignment). */
	zero_count = rbsp->zero_count + zero_step;

	/* Append EPTB as many times as necessary. */
	while (zero_count >= 22) {
		zero_chunk = 22 - rbsp->zero_count;

		ret = v4l2_h264_enc_rbsp_bits_raw(rbsp, 0, zero_chunk);
		if (ret)
			return ret;

		/* Two high bits for 0x3 and 6 other bits for stolen zeros. */
		ret = v4l2_h264_enc_rbsp_bits_raw(rbsp,
						  V4L2_H264_ENC_RBSP_EPTB << 6,
						  8);
		if (ret)
			return ret;

		/* We get 6 zeros after byte alignment. */
		rbsp->zero_count = 6;

		*bits_left -= zero_chunk;
		zero_step -= zero_chunk;
		zero_count = rbsp->zero_count + zero_step;
	}

	/* Append remaining zero bits. */
	if (zero_step) {
		ret = v4l2_h264_enc_rbsp_bits_raw(rbsp, 0, zero_step);
		if (ret)
			return ret;

		*bits_left -= zero_step;
	}

	rbsp->zero_count = zero_count;

	return ret;
}

static int v4l2_h264_enc_rbsp_bits(struct v4l2_h264_enc_rbsp *rbsp, u32 value,
				   unsigned int bits_count)
{
	unsigned int bits_left = bits_count;
	unsigned int zero_head;
	unsigned int zero_tail;
	int ret;

	if (bits_count > 32)
		return -EINVAL;
	else if (!bits_count)
		return 0;

	ret = v4l2_h264_enc_rbsp_op(rbsp, bits, value, bits_count);
	if (!ret)
		return 0;
	else if (ret != -EOPNOTSUPP)
		return ret;

	value &= GENMASK(bits_count - 1, 0);

	/* Count heading zeros. */
	if (value)
		zero_head = bits_count - __fls(value) - 1;
	else
		zero_head = bits_count;

	/* Append heading zeros with EPTB. */
	ret = eptb_bits(rbsp, &bits_left, zero_head);
	if (ret)
		return ret;

	/* A zero value is entirely handled as heading zeros. */
	if (!bits_left || WARN_ON(!value))
		return 0;

	/* Count trailing zeros. */
	zero_tail = __ffs(value);

	/* Append non-tail bits. */
	ret = v4l2_h264_enc_rbsp_bits_raw(rbsp, value >> zero_tail,
					  bits_left - zero_tail);
	if (ret)
		return ret;

	/* Reset zero count after appending non-zero bits. */
	rbsp->zero_count = 0;

	bits_left = zero_tail;
	if (!bits_left)
		return 0;

	/* Append trailing zeros with EPTB. */
	ret = eptb_bits(rbsp, &bits_left, zero_tail);
	if (ret)
		return ret;

	WARN_ON(bits_left);

	return 0;
}

static int v4l2_h264_enc_rbsp_ue(struct v4l2_h264_enc_rbsp *rbsp, u32 value)
{
	unsigned int bits_count;
	int ret;

	ret = v4l2_h264_enc_rbsp_op(rbsp, ue, value);
	if (!ret)
		return 0;
	else if (ret != -EOPNOTSUPP)
		return ret;

	/*
	 * Exponential-Golomb coding of x stores the value of v + 1.
	 * This takes fls(v + 1) + 1 bits for the non-zero bits and fls(v + 1)
	 * heading zero bits.
	 */
	value += 1;
	bits_count = 2 * __fls(value) + 1;

	return v4l2_h264_enc_rbsp_bits(rbsp, value, bits_count);
}

static int v4l2_h264_enc_rbsp_se(struct v4l2_h264_enc_rbsp *rbsp, s32 value)
{
	u32 value_ue;
	int ret;

	ret = v4l2_h264_enc_rbsp_op(rbsp, se, value);
	if (!ret)
		return 0;
	else if (ret != -EOPNOTSUPP)
		return ret;

	/*
	 * The signed extension represents numbers in Exponential-Golomb
	 * with each positive value followed by its corresponding negative
	 * value in sequence order.
	 */

	if (value > 0)
		value_ue = 2 * value - 1;
	else
		value_ue = -2 * value;

	return v4l2_h264_enc_rbsp_ue(rbsp, value_ue);
}

static int v4l2_h264_enc_rbsp_align(struct v4l2_h264_enc_rbsp *rbsp)
{
	unsigned int zero_count;
	int ret;

	ret = v4l2_h264_enc_rbsp_op(rbsp, align);
	if (!ret)
		return 0;
	else if (ret != -EOPNOTSUPP)
		return ret;

	zero_count = 8 - rbsp->bit_offset;
	if (!zero_count)
		return 0;

	return v4l2_h264_enc_rbsp_bits(rbsp, 0, zero_count);
}

static int v4l2_h264_enc_rbsp_u32(struct v4l2_h264_enc_rbsp *rbsp, u32 value)
{
	return v4l2_h264_enc_rbsp_bits(rbsp, value, 32);
}

static int v4l2_h264_enc_rbsp_u16(struct v4l2_h264_enc_rbsp *rbsp, u16 value)
{
	return v4l2_h264_enc_rbsp_bits(rbsp, value, 16);
}

static int v4l2_h264_enc_rbsp_u8(struct v4l2_h264_enc_rbsp *rbsp, u8 value)
{
	return v4l2_h264_enc_rbsp_bits(rbsp, value, 8);
}

static int v4l2_h264_enc_rbsp_bit(struct v4l2_h264_enc_rbsp *rbsp, u8 value)
{
	return v4l2_h264_enc_rbsp_bits(rbsp, value, 1);
}

static int v4l2_h264_enc_rbsp_flag(struct v4l2_h264_enc_rbsp *rbsp, u32 flags,
				   u32 flag)
{
	return v4l2_h264_enc_rbsp_bit(rbsp, (flags & flag) == flag);
}

int v4l2_h264_enc_rbsp_start_code(struct v4l2_h264_enc_rbsp *rbsp)
{
	/* Start code must be inserted at a byte-aligned position. */
	if (WARN_ON(rbsp->bit_offset))
		return -EINVAL;

	return v4l2_h264_enc_rbsp_bits_raw(rbsp, V4L2_H264_START_CODE_ANNEX_B,
					   32);
}
EXPORT_SYMBOL_GPL(v4l2_h264_enc_rbsp_start_code);

static int v4l2_h264_enc_rbsp_nalu_begin(struct v4l2_h264_enc_rbsp *rbsp,
					 u8 nal_ref_idc, u8 nal_unit_type)
{
	u8 forbidden_zero_bit = 0;
	int ret;

	/* NALU must be inserted at a byte-aligned position. */
	if (WARN_ON(rbsp->bit_offset))
		return -EINVAL;

	/* NALU header bits must not be counted in EPTB. */

	ret = v4l2_h264_enc_rbsp_bits_raw(rbsp, forbidden_zero_bit, 1);
	if (ret)
		return ret;

	ret = v4l2_h264_enc_rbsp_bits_raw(rbsp, nal_ref_idc, 2);
	if (ret)
		return ret;

	return v4l2_h264_enc_rbsp_bits_raw(rbsp, nal_unit_type, 5);
}

static int v4l2_h264_enc_rbsp_nalu_end(struct v4l2_h264_enc_rbsp *rbsp)
{
	u8 rbsp_stop_one_bit = 1;
	int ret;

	ret = v4l2_h264_enc_rbsp_bit(rbsp, rbsp_stop_one_bit);
	if (ret)
		return ret;

	return v4l2_h264_enc_rbsp_align(rbsp);
}

int v4l2_h264_enc_rbsp_aud(struct v4l2_h264_enc_rbsp *rbsp, u8 primary_pic_type)
{
	int ret;

	ret = v4l2_h264_enc_rbsp_nalu_begin(rbsp, 0, V4L2_H264_NALU_TYPE_AUD);
	if (ret)
		return ret;

	ret = v4l2_h264_enc_rbsp_bits(rbsp, primary_pic_type, 3);
	if (ret)
		return ret;

	return v4l2_h264_enc_rbsp_nalu_end(rbsp);
}
EXPORT_SYMBOL_GPL(v4l2_h264_enc_rbsp_aud);

static int v4l2_h264_enc_rbsp_sps_hrd(struct v4l2_h264_enc_rbsp *rbsp,
				      const struct v4l2_h264_sps_video_hrd *hrd)
{
	unsigned int i;
	int ret;

	if (hrd->cpb_cnt_minus1 > 31)
		return -EINVAL;

	ret = v4l2_h264_enc_rbsp_ue(rbsp, hrd->cpb_cnt_minus1);
	if (ret)
		return ret;

	ret = v4l2_h264_enc_rbsp_bits(rbsp, hrd->bit_rate_scale, 4);
	if (ret)
		return ret;

	ret = v4l2_h264_enc_rbsp_bits(rbsp, hrd->cpb_size_scale, 4);
	if (ret)
		return ret;

	for (i = 0; i <= hrd->cpb_cnt_minus1; i++) {
		ret = v4l2_h264_enc_rbsp_ue(rbsp,
					    hrd->bit_rate_value_minus1[i]);
		if (ret)
			return ret;

		ret = v4l2_h264_enc_rbsp_ue(rbsp,
					    hrd->cpb_size_value_minus1[i]);
		if (ret)
			return ret;

		ret = v4l2_h264_enc_rbsp_bit(rbsp, hrd->cbr_flag[i]);
		if (ret)
			return ret;
	}

	ret = v4l2_h264_enc_rbsp_bits(rbsp,
				      hrd->initial_cpb_removal_delay_length_minus1,
				      5);
	if (ret)
		return ret;

	ret = v4l2_h264_enc_rbsp_bits(rbsp,
				      hrd->cpb_removal_delay_length_minus1, 5);
	if (ret)
		return ret;

	ret = v4l2_h264_enc_rbsp_bits(rbsp, hrd->dpb_output_delay_length_minus1,
				      5);
	if (ret)
		return ret;

	ret = v4l2_h264_enc_rbsp_bits(rbsp, hrd->time_offset_length, 5);
	if (ret)
		return ret;

	return 0;
}

static int v4l2_h264_enc_rbsp_sps_vui(struct v4l2_h264_enc_rbsp *rbsp,
				      const struct v4l2_h264_sps_video *sps_video)
{
	int ret;

	ret = v4l2_h264_enc_rbsp_flag(rbsp, sps_video->flags,
				      V4L2_H264_SPS_VIDEO_FLAG_VUI_ASPECT_RATIO_INFO_PRESENT);
	if (ret)
		return ret;

	if (sps_video->flags & V4L2_H264_SPS_VIDEO_FLAG_VUI_ASPECT_RATIO_INFO_PRESENT) {
		ret = v4l2_h264_enc_rbsp_u8(rbsp, sps_video->aspect_ratio_idc);
		if (ret)
			return ret;

		if (sps_video->aspect_ratio_idc ==
		    V4L2_H264_VUI_ASPECT_RATIO_IDC_EXTENDED) {
			ret = v4l2_h264_enc_rbsp_u16(rbsp,
						     sps_video->sar_width);
			if (ret)
				return ret;

			ret = v4l2_h264_enc_rbsp_u16(rbsp,
						     sps_video->sar_height);
			if (ret)
				return ret;
		}
	}

	ret = v4l2_h264_enc_rbsp_flag(rbsp, sps_video->flags,
				      V4L2_H264_SPS_VIDEO_FLAG_VUI_OVERSCAN_INFO_PRESENT);
	if (ret)
		return ret;

	if (sps_video->flags & V4L2_H264_SPS_VIDEO_FLAG_VUI_OVERSCAN_INFO_PRESENT) {
		ret = v4l2_h264_enc_rbsp_flag(rbsp, sps_video->flags,
					      V4L2_H264_SPS_VIDEO_FLAG_VUI_OVERSCAN_APPROPRIATE);
		if (ret)
			return ret;
	}

	ret = v4l2_h264_enc_rbsp_flag(rbsp, sps_video->flags,
				      V4L2_H264_SPS_VIDEO_FLAG_VUI_VIDEO_SIGNAL_TYPE_PRESENT);
	if (ret)
		return ret;

	if (sps_video->flags & V4L2_H264_SPS_VIDEO_FLAG_VUI_VIDEO_SIGNAL_TYPE_PRESENT) {
		ret = v4l2_h264_enc_rbsp_bits(rbsp, sps_video->video_format, 3);
		if (ret)
			return ret;

		ret = v4l2_h264_enc_rbsp_flag(rbsp, sps_video->flags,
					      V4L2_H264_SPS_VIDEO_FLAG_VUI_VIDEO_FULL_RANGE);
		if (ret)
			return ret;

		ret = v4l2_h264_enc_rbsp_flag(rbsp, sps_video->flags,
					      V4L2_H264_SPS_VIDEO_FLAG_VUI_COLOUR_DESCRIPTION_PRESENT);
		if (ret)
			return ret;

		if (sps_video->flags & V4L2_H264_SPS_VIDEO_FLAG_VUI_COLOUR_DESCRIPTION_PRESENT) {
			ret = v4l2_h264_enc_rbsp_u8(rbsp,
						    sps_video->colour_primaries);
			if (ret)
				return ret;

			ret = v4l2_h264_enc_rbsp_u8(rbsp,
						    sps_video->transfer_characteristics);
			if (ret)
				return ret;

			ret = v4l2_h264_enc_rbsp_u8(rbsp,
						    sps_video->matrix_coefficients);
			if (ret)
				return ret;
		}
	}

	ret = v4l2_h264_enc_rbsp_flag(rbsp, sps_video->flags,
				      V4L2_H264_SPS_VIDEO_FLAG_VUI_CHROMA_LOC_INFO_PRESENT);
	if (ret)
		return ret;

	if (sps_video->flags & V4L2_H264_SPS_VIDEO_FLAG_VUI_CHROMA_LOC_INFO_PRESENT) {
		ret = v4l2_h264_enc_rbsp_ue(rbsp,
					    sps_video->chroma_sample_loc_type_top_field);
		if (ret)
			return ret;

		ret = v4l2_h264_enc_rbsp_ue(rbsp,
					    sps_video->chroma_sample_loc_type_bottom_field);
		if (ret)
			return ret;
	}

	ret = v4l2_h264_enc_rbsp_flag(rbsp, sps_video->flags,
				      V4L2_H264_SPS_VIDEO_FLAG_VUI_TIMING_INFO_PRESENT);
	if (ret)
		return ret;

	if (sps_video->flags & V4L2_H264_SPS_VIDEO_FLAG_VUI_TIMING_INFO_PRESENT) {
		ret = v4l2_h264_enc_rbsp_u32(rbsp,
					     sps_video->num_units_in_tick);
		if (ret)
			return ret;

		ret = v4l2_h264_enc_rbsp_u32(rbsp,
					     sps_video->time_scale);
		if (ret)
			return ret;

		ret = v4l2_h264_enc_rbsp_flag(rbsp, sps_video->flags,
					      V4L2_H264_SPS_VIDEO_FLAG_VUI_FIXED_FRAME_RATE);
		if (ret)
			return ret;
	}

	ret = v4l2_h264_enc_rbsp_flag(rbsp, sps_video->flags,
				      V4L2_H264_SPS_VIDEO_FLAG_VUI_NAL_HRD_PARAMETERS_PRESENT);
	if (ret)
		return ret;

	if (sps_video->flags & V4L2_H264_SPS_VIDEO_FLAG_VUI_NAL_HRD_PARAMETERS_PRESENT) {
		ret = v4l2_h264_enc_rbsp_sps_hrd(rbsp, &sps_video->nal_hrd);
		if (ret)
			return ret;
	}

	ret = v4l2_h264_enc_rbsp_flag(rbsp, sps_video->flags,
				      V4L2_H264_SPS_VIDEO_FLAG_VUI_VCL_HRD_PARAMETERS_PRESENT);
	if (ret)
		return ret;

	if (sps_video->flags & V4L2_H264_SPS_VIDEO_FLAG_VUI_VCL_HRD_PARAMETERS_PRESENT) {
		ret = v4l2_h264_enc_rbsp_sps_hrd(rbsp, &sps_video->vcl_hrd);
		if (ret)
			return ret;
	}

	if (sps_video->flags & V4L2_H264_SPS_VIDEO_FLAG_VUI_NAL_HRD_PARAMETERS_PRESENT ||
	    sps_video->flags & V4L2_H264_SPS_VIDEO_FLAG_VUI_VCL_HRD_PARAMETERS_PRESENT) {
		ret = v4l2_h264_enc_rbsp_flag(rbsp, sps_video->flags,
					      V4L2_H264_SPS_VIDEO_FLAG_VUI_LOW_DELAY_HRD);
		if (ret)
			return ret;
	}

	ret = v4l2_h264_enc_rbsp_flag(rbsp, sps_video->flags,
				      V4L2_H264_SPS_VIDEO_FLAG_VUI_PIC_STRUCT_PRESENT);
	if (ret)
		return ret;

	ret = v4l2_h264_enc_rbsp_flag(rbsp, sps_video->flags,
				      V4L2_H264_SPS_VIDEO_FLAG_VUI_BITSTREAM_RESTRICTION);
	if (ret)
		return ret;

	if (sps_video->flags & V4L2_H264_SPS_VIDEO_FLAG_VUI_BITSTREAM_RESTRICTION) {
		ret = v4l2_h264_enc_rbsp_flag(rbsp, sps_video->flags,
					      V4L2_H264_SPS_VIDEO_FLAG_VUI_MOTION_VECTORS_OVER_PIC_BOUNDARIES);
		if (ret)
			return ret;

		ret = v4l2_h264_enc_rbsp_ue(rbsp,
					    sps_video->max_bytes_per_pic_denom);
		if (ret)
			return ret;

		ret = v4l2_h264_enc_rbsp_ue(rbsp,
					    sps_video->max_bits_per_mb_denom);
		if (ret)
			return ret;

		ret = v4l2_h264_enc_rbsp_ue(rbsp,
					    sps_video->log2_max_mv_length_horizontal);
		if (ret)
			return ret;

		ret = v4l2_h264_enc_rbsp_ue(rbsp,
					    sps_video->log2_max_mv_length_vertical);
		if (ret)
			return ret;

		ret = v4l2_h264_enc_rbsp_ue(rbsp,
					    sps_video->max_num_reorder_frames);
		if (ret)
			return ret;

		ret = v4l2_h264_enc_rbsp_ue(rbsp,
					    sps_video->max_dec_frame_buffering);
		if (ret)
			return ret;
	}

	return 0;
}

int v4l2_h264_enc_rbsp_sps(struct v4l2_h264_enc_rbsp *rbsp,
			   const struct v4l2_ctrl_h264_sps *sps,
			   const struct v4l2_h264_sps_video *sps_video)
{
	u8 constraint_set_flags = 0;
	u8 seq_scaling_matrix_present_flag = 0;
	unsigned int i;
	int ret;

	ret = v4l2_h264_enc_rbsp_nalu_begin(rbsp, 0, V4L2_H264_NALU_TYPE_SPS);
	if (ret)
		return ret;

	ret = v4l2_h264_enc_rbsp_u8(rbsp, sps->profile_idc);
	if (ret)
		return ret;

	if (sps->constraint_set_flags & V4L2_H264_SPS_CONSTRAINT_SET0_FLAG)
		constraint_set_flags |= BIT(7);
	if (sps->constraint_set_flags & V4L2_H264_SPS_CONSTRAINT_SET1_FLAG)
		constraint_set_flags |= BIT(6);
	if (sps->constraint_set_flags & V4L2_H264_SPS_CONSTRAINT_SET2_FLAG)
		constraint_set_flags |= BIT(5);
	if (sps->constraint_set_flags & V4L2_H264_SPS_CONSTRAINT_SET3_FLAG)
		constraint_set_flags |= BIT(4);
	if (sps->constraint_set_flags & V4L2_H264_SPS_CONSTRAINT_SET4_FLAG)
		constraint_set_flags |= BIT(3);
	if (sps->constraint_set_flags & V4L2_H264_SPS_CONSTRAINT_SET5_FLAG)
		constraint_set_flags |= BIT(2);

	ret = v4l2_h264_enc_rbsp_u8(rbsp, constraint_set_flags);
	if (ret)
		return ret;

	ret = v4l2_h264_enc_rbsp_u8(rbsp, sps->level_idc);
	if (ret)
		return ret;

	ret = v4l2_h264_enc_rbsp_ue(rbsp, sps->seq_parameter_set_id);
	if (ret)
		return ret;

	if (V4L2_H264_SPS_HAS_CHROMA_FORMAT(sps)) {
		ret = v4l2_h264_enc_rbsp_ue(rbsp, sps->chroma_format_idc);
		if (ret)
			return ret;

		if (sps->chroma_format_idc == 3) {
			ret = v4l2_h264_enc_rbsp_flag(rbsp, sps->flags,
						      V4L2_H264_SPS_FLAG_SEPARATE_COLOUR_PLANE);
			if (ret)
				return ret;
		}

		ret = v4l2_h264_enc_rbsp_ue(rbsp, sps->bit_depth_luma_minus8);
		if (ret)
			return ret;

		ret = v4l2_h264_enc_rbsp_ue(rbsp, sps->bit_depth_chroma_minus8);
		if (ret)
			return ret;

		ret = v4l2_h264_enc_rbsp_flag(rbsp, sps->flags,
					      V4L2_H264_SPS_FLAG_QPPRIME_Y_ZERO_TRANSFORM_BYPASS);
		if (ret)
			return ret;

		/* Scaling matrix is not supported. */
		ret = v4l2_h264_enc_rbsp_bit(rbsp,
					     seq_scaling_matrix_present_flag);
		if (ret)
			return ret;
	}

	ret = v4l2_h264_enc_rbsp_ue(rbsp, sps->log2_max_frame_num_minus4);
	if (ret)
		return ret;

	ret = v4l2_h264_enc_rbsp_ue(rbsp, sps->pic_order_cnt_type);
	if (ret)
		return ret;

	if (!sps->pic_order_cnt_type) {
		ret = v4l2_h264_enc_rbsp_ue(rbsp,
					    sps->log2_max_pic_order_cnt_lsb_minus4);
		if (ret)
			return ret;
	} else if (sps->pic_order_cnt_type == 1) {
		ret = v4l2_h264_enc_rbsp_flag(rbsp, sps->flags,
					      V4L2_H264_SPS_FLAG_DELTA_PIC_ORDER_ALWAYS_ZERO);
		if (ret)
			return ret;

		ret = v4l2_h264_enc_rbsp_se(rbsp, sps->offset_for_non_ref_pic);
		if (ret)
			return ret;

		ret = v4l2_h264_enc_rbsp_se(rbsp,
					    sps->offset_for_top_to_bottom_field);
		if (ret)
			return ret;

		ret = v4l2_h264_enc_rbsp_ue(rbsp,
					    sps->num_ref_frames_in_pic_order_cnt_cycle);
		if (ret)
			return ret;

		for (i = 0; i < sps->num_ref_frames_in_pic_order_cnt_cycle; i++) {
			ret = v4l2_h264_enc_rbsp_se(rbsp,
						    sps->offset_for_ref_frame[i]);
			if (ret)
				return ret;
		}
	} else {
		return -EINVAL;
	}

	ret = v4l2_h264_enc_rbsp_ue(rbsp, sps->max_num_ref_frames);
	if (ret)
		return ret;

	ret = v4l2_h264_enc_rbsp_flag(rbsp, sps->flags,
				      V4L2_H264_SPS_FLAG_GAPS_IN_FRAME_NUM_VALUE_ALLOWED);
	if (ret)
		return ret;

	ret = v4l2_h264_enc_rbsp_ue(rbsp, sps->pic_width_in_mbs_minus1);
	if (ret)
		return ret;

	ret = v4l2_h264_enc_rbsp_ue(rbsp, sps->pic_height_in_map_units_minus1);
	if (ret)
		return ret;

	ret = v4l2_h264_enc_rbsp_flag(rbsp, sps->flags,
				      V4L2_H264_SPS_FLAG_FRAME_MBS_ONLY);
	if (ret)
		return ret;

	if (!(sps->flags & V4L2_H264_SPS_FLAG_FRAME_MBS_ONLY)) {
		ret = v4l2_h264_enc_rbsp_flag(rbsp, sps->flags,
					      V4L2_H264_SPS_FLAG_MB_ADAPTIVE_FRAME_FIELD);
		if (ret)
			return ret;
	}

	ret = v4l2_h264_enc_rbsp_flag(rbsp, sps->flags,
				      V4L2_H264_SPS_FLAG_DIRECT_8X8_INFERENCE);
	if (ret)
		return ret;

	ret = v4l2_h264_enc_rbsp_flag(rbsp, sps_video->flags,
				      V4L2_H264_SPS_VIDEO_FLAG_FRAME_CROPPING);
	if (ret)
		return ret;

	if (sps_video->flags & V4L2_H264_SPS_VIDEO_FLAG_FRAME_CROPPING) {
		ret = v4l2_h264_enc_rbsp_ue(rbsp,
					    sps_video->frame_crop_left_offset);
		if (ret)
			return ret;

		ret = v4l2_h264_enc_rbsp_ue(rbsp,
					    sps_video->frame_crop_right_offset);
		if (ret)
			return ret;

		ret = v4l2_h264_enc_rbsp_ue(rbsp,
					    sps_video->frame_crop_top_offset);
		if (ret)
			return ret;

		ret = v4l2_h264_enc_rbsp_ue(rbsp,
					    sps_video->frame_crop_bottom_offset);
		if (ret)
			return ret;
	}

	ret = v4l2_h264_enc_rbsp_flag(rbsp, sps_video->flags,
				      V4L2_H264_SPS_VIDEO_FLAG_VUI_PARAMETERS_PRESENT);
	if (ret)
		return ret;

	if (sps_video->flags & V4L2_H264_SPS_VIDEO_FLAG_VUI_PARAMETERS_PRESENT) {
		ret = v4l2_h264_enc_rbsp_sps_vui(rbsp, sps_video);
		if (ret)
			return ret;
	}

	return v4l2_h264_enc_rbsp_nalu_end(rbsp);
}
EXPORT_SYMBOL_GPL(v4l2_h264_enc_rbsp_sps);

int v4l2_h264_enc_rbsp_pps(struct v4l2_h264_enc_rbsp *rbsp,
			   const struct v4l2_ctrl_h264_pps *pps)
{
	u8 pic_scaling_matrix_present_flag = 0;
	int ret;

	ret = v4l2_h264_enc_rbsp_nalu_begin(rbsp, 0, V4L2_H264_NALU_TYPE_PPS);
	if (ret)
		return ret;

	ret = v4l2_h264_enc_rbsp_ue(rbsp, pps->pic_parameter_set_id);
	if (ret)
		return ret;

	ret = v4l2_h264_enc_rbsp_ue(rbsp, pps->seq_parameter_set_id);
	if (ret)
		return ret;

	ret = v4l2_h264_enc_rbsp_flag(rbsp, pps->flags,
				      V4L2_H264_PPS_FLAG_ENTROPY_CODING_MODE);
	if (ret)
		return ret;

	ret = v4l2_h264_enc_rbsp_flag(rbsp, pps->flags,
				      V4L2_H264_PPS_FLAG_BOTTOM_FIELD_PIC_ORDER_IN_FRAME_PRESENT);
	if (ret)
		return ret;

	ret = v4l2_h264_enc_rbsp_ue(rbsp, pps->num_slice_groups_minus1);
	if (ret)
		return ret;

	/* Multiple slice groups are not supported. */
	if (pps->num_slice_groups_minus1 > 1)
		return -EINVAL;

	ret = v4l2_h264_enc_rbsp_ue(rbsp,
				    pps->num_ref_idx_l0_default_active_minus1);
	if (ret)
		return ret;

	ret = v4l2_h264_enc_rbsp_ue(rbsp,
				    pps->num_ref_idx_l1_default_active_minus1);
	if (ret)
		return ret;

	ret = v4l2_h264_enc_rbsp_flag(rbsp, pps->flags,
				      V4L2_H264_PPS_FLAG_WEIGHTED_PRED);
	if (ret)
		return ret;

	ret = v4l2_h264_enc_rbsp_bits(rbsp, pps->weighted_bipred_idc, 2);
	if (ret)
		return ret;

	ret = v4l2_h264_enc_rbsp_se(rbsp, pps->pic_init_qp_minus26);
	if (ret)
		return ret;

	ret = v4l2_h264_enc_rbsp_se(rbsp, pps->pic_init_qs_minus26);
	if (ret)
		return ret;

	ret = v4l2_h264_enc_rbsp_se(rbsp, pps->chroma_qp_index_offset);
	if (ret)
		return ret;

	ret = v4l2_h264_enc_rbsp_flag(rbsp, pps->flags,
				      V4L2_H264_PPS_FLAG_DEBLOCKING_FILTER_CONTROL_PRESENT);
	if (ret)
		return ret;

	ret = v4l2_h264_enc_rbsp_flag(rbsp, pps->flags,
				      V4L2_H264_PPS_FLAG_CONSTRAINED_INTRA_PRED);
	if (ret)
		return ret;

	ret = v4l2_h264_enc_rbsp_flag(rbsp, pps->flags,
				      V4L2_H264_PPS_FLAG_REDUNDANT_PIC_CNT_PRESENT);
	if (ret)
		return ret;

	ret = v4l2_h264_enc_rbsp_flag(rbsp, pps->flags,
				      V4L2_H264_PPS_FLAG_TRANSFORM_8X8_MODE);
	if (ret)
		return ret;

	/* Scaling matrix is not supported. */
	ret = v4l2_h264_enc_rbsp_bit(rbsp, pic_scaling_matrix_present_flag);
	if (ret)
		return ret;

	ret = v4l2_h264_enc_rbsp_se(rbsp, pps->second_chroma_qp_index_offset);
	if (ret)
		return ret;

	return v4l2_h264_enc_rbsp_nalu_end(rbsp);
}
EXPORT_SYMBOL_GPL(v4l2_h264_enc_rbsp_pps);

int v4l2_h264_enc_rbsp_slice_header(struct v4l2_h264_enc_rbsp *rbsp,
				    const struct v4l2_ctrl_h264_sps *sps,
				    const struct v4l2_ctrl_h264_pps *pps,
				    const struct v4l2_ctrl_h264_encode_params *encode)
{
	u8 ref_pic_list_modification_flag_l0 = 0;
	u8 ref_pic_list_modification_flag_l1 = 0;
	u8 adaptive_ref_pic_marking_mode_flag = 0;
	u32 first_mb_in_slice = 0;
	u32 redundant_pic_cnt = 0;
	u8 sp_for_switch_flag = 0;
	s32 slice_qs_delta = 0;
	u8 nal_unit_type;
	int ret;

	if (encode->flags & V4L2_H264_ENCODE_FLAG_IDR_PIC)
		nal_unit_type = V4L2_H264_NALU_TYPE_SLICE_IDR;
	else
		nal_unit_type = V4L2_H264_NALU_TYPE_SLICE_NON_IDR;

	ret = v4l2_h264_enc_rbsp_nalu_begin(rbsp, encode->nal_ref_idc,
					    nal_unit_type);
	if (ret)
		return ret;

	/* Multiple slices are not supported. */
	ret = v4l2_h264_enc_rbsp_ue(rbsp, first_mb_in_slice);
	if (ret)
		return ret;

	ret = v4l2_h264_enc_rbsp_ue(rbsp, encode->slice_type);
	if (ret)
		return ret;

	ret = v4l2_h264_enc_rbsp_ue(rbsp, encode->pic_parameter_set_id);
	if (ret)
		return ret;

	if (sps->flags & V4L2_H264_SPS_FLAG_SEPARATE_COLOUR_PLANE) {
		ret = v4l2_h264_enc_rbsp_bits(rbsp, encode->colour_plane_id, 2);
		if (ret)
			return ret;
	}

	ret = v4l2_h264_enc_rbsp_bits(rbsp, encode->frame_num,
				      sps->log2_max_frame_num_minus4 + 4);
	if (ret)
		return ret;

	if (!(sps->flags & V4L2_H264_SPS_FLAG_FRAME_MBS_ONLY)) {
		ret = v4l2_h264_enc_rbsp_flag(rbsp, encode->flags,
					      V4L2_H264_ENCODE_FLAG_FIELD_PIC);
		if (ret)
			return ret;

		if (encode->flags & V4L2_H264_ENCODE_FLAG_FIELD_PIC) {
			ret = v4l2_h264_enc_rbsp_flag(rbsp, encode->flags,
						      V4L2_H264_ENCODE_FLAG_BOTTOM_FIELD);
			if (ret)
				return ret;
		}
	}

	if (encode->flags & V4L2_H264_ENCODE_FLAG_IDR_PIC) {
		ret = v4l2_h264_enc_rbsp_ue(rbsp, encode->idr_pic_id);
		if (ret)
			return ret;
	}

	if (!sps->pic_order_cnt_type) {
		ret = v4l2_h264_enc_rbsp_bits(rbsp, encode->pic_order_cnt_lsb,
					      sps->log2_max_pic_order_cnt_lsb_minus4 + 4);
		if (ret)
			return ret;

		if (pps->flags & V4L2_H264_PPS_FLAG_BOTTOM_FIELD_PIC_ORDER_IN_FRAME_PRESENT &&
		    !(encode->flags & V4L2_H264_ENCODE_FLAG_FIELD_PIC)) {
			ret = v4l2_h264_enc_rbsp_se(rbsp,
						    encode->delta_pic_order_cnt_bottom);
			if (ret)
				return ret;
		}
	}

	if (sps->pic_order_cnt_type == 1 &&
	    !(sps->flags & V4L2_H264_SPS_FLAG_DELTA_PIC_ORDER_ALWAYS_ZERO)) {
		ret = v4l2_h264_enc_rbsp_se(rbsp,
					    encode->delta_pic_order_cnt0);
		if (ret)
			return ret;

		if (pps->flags & V4L2_H264_PPS_FLAG_BOTTOM_FIELD_PIC_ORDER_IN_FRAME_PRESENT &&
		    !(encode->flags & V4L2_H264_ENCODE_FLAG_FIELD_PIC)) {
			ret = v4l2_h264_enc_rbsp_se(rbsp,
						    encode->delta_pic_order_cnt1);
			if (ret)
				return ret;
		}
	}

	if (pps->flags & V4L2_H264_PPS_FLAG_REDUNDANT_PIC_CNT_PRESENT) {
		/* Redundant pictures are not supported. */
		ret = v4l2_h264_enc_rbsp_ue(rbsp, redundant_pic_cnt);
		if (ret)
			return ret;
	}

	if (encode->slice_type == V4L2_H264_SLICE_TYPE_B) {
		ret = v4l2_h264_enc_rbsp_flag(rbsp, encode->flags,
					      V4L2_H264_ENCODE_FLAG_DIRECT_SPATIAL_MV_PRED);
		if (ret)
			return ret;
	}

	if (encode->slice_type == V4L2_H264_SLICE_TYPE_P ||
	    encode->slice_type == V4L2_H264_SLICE_TYPE_SP ||
	    encode->slice_type == V4L2_H264_SLICE_TYPE_B) {
		ret = v4l2_h264_enc_rbsp_flag(rbsp, encode->flags,
					      V4L2_H264_ENCODE_FLAG_NUM_REF_IDX_ACTIVE_OVERRIDE);
		if (ret)
			return ret;

		if (encode->flags & V4L2_H264_ENCODE_FLAG_NUM_REF_IDX_ACTIVE_OVERRIDE) {
			ret = v4l2_h264_enc_rbsp_ue(rbsp,
						    encode->num_ref_idx_l0_active_minus1);
			if (ret)
				return ret;
		}

		if (encode->flags & V4L2_H264_ENCODE_FLAG_NUM_REF_IDX_ACTIVE_OVERRIDE &&
		    encode->slice_type == V4L2_H264_SLICE_TYPE_B) {
			ret = v4l2_h264_enc_rbsp_ue(rbsp,
						    encode->num_ref_idx_l1_active_minus1);
			if (ret)
				return ret;
		}
	}

	if (encode->slice_type != V4L2_H264_SLICE_TYPE_I &&
	    encode->slice_type != V4L2_H264_SLICE_TYPE_SI) {
		/* Ref pic list modification is not supported. */
		ret = v4l2_h264_enc_rbsp_bit(rbsp,
					     ref_pic_list_modification_flag_l0);
		if (ret)
			return ret;
	}

	if (encode->slice_type == V4L2_H264_SLICE_TYPE_B) {
		/* Ref pic list modification is not supported. */
		ret = v4l2_h264_enc_rbsp_bit(rbsp,
					     ref_pic_list_modification_flag_l1);
		if (ret)
			return ret;
	}

	/* Prediction weights are not supported. */
	if (V4L2_H264_CTRL_PRED_WEIGHTS_REQUIRED(pps, encode))
		return -EINVAL;

	if (encode->nal_ref_idc) {
		if (encode->flags & V4L2_H264_ENCODE_FLAG_IDR_PIC) {
			ret = v4l2_h264_enc_rbsp_flag(rbsp, encode->flags,
						      V4L2_H264_ENCODE_FLAG_NO_OUTPUT_OF_PRIOR_PICS);
			if (ret)
				return ret;

			ret = v4l2_h264_enc_rbsp_flag(rbsp, encode->flags,
						      V4L2_H264_ENCODE_FLAG_LONG_TERM_REFERENCE);
			if (ret)
				return ret;
		} else {
			/* Adaptive ref pic marking mode is not supported. */
			ret = v4l2_h264_enc_rbsp_bit(rbsp,
						     adaptive_ref_pic_marking_mode_flag);
			if (ret)
				return ret;
		}
	}

	if (pps->flags & V4L2_H264_PPS_FLAG_ENTROPY_CODING_MODE &&
	    encode->slice_type != V4L2_H264_SLICE_TYPE_I &&
	    encode->slice_type != V4L2_H264_SLICE_TYPE_SI) {
		ret = v4l2_h264_enc_rbsp_ue(rbsp, encode->cabac_init_idc);
		if (ret)
			return ret;
	}

	ret = v4l2_h264_enc_rbsp_se(rbsp, encode->slice_qp_delta);
	if (ret)
		return ret;

	if (encode->slice_type == V4L2_H264_SLICE_TYPE_SP) {
		/* Switching slices are not supported. */
		ret = v4l2_h264_enc_rbsp_bit(rbsp, sp_for_switch_flag);
		if (ret)
			return ret;
	}

	if (encode->slice_type == V4L2_H264_SLICE_TYPE_SP ||
	    encode->slice_type == V4L2_H264_SLICE_TYPE_SI) {
		ret = v4l2_h264_enc_rbsp_se(rbsp, slice_qs_delta);
		if (ret)
			return ret;
	}

	if (pps->flags & V4L2_H264_PPS_FLAG_DEBLOCKING_FILTER_CONTROL_PRESENT) {
		ret = v4l2_h264_enc_rbsp_ue(rbsp,
					    encode->disable_deblocking_filter_idc);
		if (ret)
			return ret;

		if (encode->disable_deblocking_filter_idc != 1) {
			ret = v4l2_h264_enc_rbsp_se(rbsp,
						    encode->slice_alpha_c0_offset_div2);
			if (ret)
				return ret;

			ret = v4l2_h264_enc_rbsp_se(rbsp,
						    encode->slice_beta_offset_div2);
			if (ret)
				return ret;
		}
	}

	/* The slice NALU is not finished, the hardware has to write the rest! */

	return 0;
}
EXPORT_SYMBOL_GPL(v4l2_h264_enc_rbsp_slice_header);

MODULE_DESCRIPTION("V4L2 H.264 Encode RBSP");
MODULE_AUTHOR("Paul Kocialkowski <paulk@sys-base.io>");
MODULE_LICENSE("GPL");
