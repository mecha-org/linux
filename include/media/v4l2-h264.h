/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Helper functions for H264 codecs.
 *
 * Copyright (c) 2019 Collabora, Ltd.
 *
 * Author: Boris Brezillon <boris.brezillon@collabora.com>
 */

#ifndef _MEDIA_V4L2_H264_H
#define _MEDIA_V4L2_H264_H

#include <media/v4l2-ctrls.h>

#define V4L2_H264_START_CODE_ANNEX_B		0x00000001

#define V4L2_H264_NALU_TYPE_SLICE_NON_IDR	1
#define V4L2_H264_NALU_TYPE_SLICE_IDR		5
#define V4L2_H264_NALU_TYPE_SPS			7
#define V4L2_H264_NALU_TYPE_PPS			8
#define V4L2_H264_NALU_TYPE_AUD			9

#define V4L2_H264_PRIMARY_PIC_TYPE_I		0
#define V4L2_H264_PRIMARY_PIC_TYPE_IP		1
#define V4L2_H264_PRIMARY_PIC_TYPE_IPB		2

#define V4L2_H264_VUI_ASPECT_RATIO_IDC_EXTENDED	255

#define V4L2_H264_VUI_VIDEO_COMPONENT		0
#define V4L2_H264_VUI_VIDEO_PAL			1
#define V4L2_H264_VUI_VIDEO_NTSC		2
#define V4L2_H264_VUI_VIDEO_SECAM		3
#define V4L2_H264_VUI_VIDEO_MAC			4
#define V4L2_H264_VUI_VIDEO_UNSPECIFIED		5

#define V4L2_H264_VUI_COLOUR_BT709		1
#define V4L2_H264_VUI_COLOUR_UNSPECIFIED	2
#define V4L2_H264_VUI_COLOUR_BT470_SYSTEM_M	4
#define V4L2_H264_VUI_COLOUR_BT470_SYSTEM_BG	5
#define V4L2_H264_VUI_COLOUR_SMPTE170M		6
#define V4L2_H264_VUI_COLOUR_SMPTE240M		7
#define V4L2_H264_VUI_COLOUR_BT2020		9

#define V4L2_H264_VUI_TRANSFER_BT709		1
#define V4L2_H264_VUI_TRANSFER_UNSPECIFIED	2
#define V4L2_H264_VUI_TRANSFER_BT470_SYSTEM_M	4
#define V4L2_H264_VUI_TRANSFER_BT470_SYSTEM_BG	5
#define V4L2_H264_VUI_TRANSFER_SMPTE170M	6
#define V4L2_H264_VUI_TRANSFER_SMPTE240M	7
#define V4L2_H264_VUI_TRANSFER_LINEAR		8
#define V4L2_H264_VUI_TRANSFER_SRGB		13

#define V4L2_H264_VUI_MATRIX_IDENTITY		0
#define V4L2_H264_VUI_MATRIX_BT709		1
#define V4L2_H264_VUI_MATRIX_UNSPECIFIED	2
#define V4L2_H264_VUI_MATRIX_BT470_SYSTEM_M	4
#define V4L2_H264_VUI_MATRIX_BT470_SYSTEM_BG	5
#define V4L2_H264_VUI_MATRIX_SMPTE170M		6
#define V4L2_H264_VUI_MATRIX_SMPTE240M		7
#define V4L2_H264_VUI_MATRIX_BT2020		9
#define V4L2_H264_VUI_MATRIX_BT2020_CONST_LUM	10

#define V4L2_H264_SPS_VIDEO_FLAG_FRAME_CROPPING				BIT(0)
#define V4L2_H264_SPS_VIDEO_FLAG_VUI_PARAMETERS_PRESENT			BIT(1)
#define V4L2_H264_SPS_VIDEO_FLAG_VUI_ASPECT_RATIO_INFO_PRESENT		BIT(2)
#define V4L2_H264_SPS_VIDEO_FLAG_VUI_OVERSCAN_INFO_PRESENT		BIT(3)
#define V4L2_H264_SPS_VIDEO_FLAG_VUI_OVERSCAN_APPROPRIATE		BIT(4)
#define V4L2_H264_SPS_VIDEO_FLAG_VUI_VIDEO_SIGNAL_TYPE_PRESENT		BIT(5)
#define V4L2_H264_SPS_VIDEO_FLAG_VUI_VIDEO_FULL_RANGE			BIT(6)
#define V4L2_H264_SPS_VIDEO_FLAG_VUI_COLOUR_DESCRIPTION_PRESENT		BIT(7)
#define V4L2_H264_SPS_VIDEO_FLAG_VUI_CHROMA_LOC_INFO_PRESENT		BIT(8)
#define V4L2_H264_SPS_VIDEO_FLAG_VUI_TIMING_INFO_PRESENT		BIT(9)
#define V4L2_H264_SPS_VIDEO_FLAG_VUI_FIXED_FRAME_RATE			BIT(10)
#define V4L2_H264_SPS_VIDEO_FLAG_VUI_NAL_HRD_PARAMETERS_PRESENT		BIT(11)
#define V4L2_H264_SPS_VIDEO_FLAG_VUI_VCL_HRD_PARAMETERS_PRESENT		BIT(12)
#define V4L2_H264_SPS_VIDEO_FLAG_VUI_LOW_DELAY_HRD			BIT(13)
#define V4L2_H264_SPS_VIDEO_FLAG_VUI_PIC_STRUCT_PRESENT			BIT(14)
#define V4L2_H264_SPS_VIDEO_FLAG_VUI_BITSTREAM_RESTRICTION		BIT(15)
#define V4L2_H264_SPS_VIDEO_FLAG_VUI_MOTION_VECTORS_OVER_PIC_BOUNDARIES	BIT(16)

struct v4l2_h264_sps_video_hrd {
	u8 cpb_cnt_minus1;
	u8 bit_rate_scale;
	u8 cpb_size_scale;

	u32 bit_rate_value_minus1[32];
	u32 cpb_size_value_minus1[32];
	u8 cbr_flag[32];

	u8 initial_cpb_removal_delay_length_minus1;
	u8 cpb_removal_delay_length_minus1;
	u8 dpb_output_delay_length_minus1;
	u8 time_offset_length;
};

struct v4l2_h264_sps_video {
	u32 frame_crop_left_offset;
	u32 frame_crop_right_offset;
	u32 frame_crop_top_offset;
	u32 frame_crop_bottom_offset;

	u8 aspect_ratio_idc;
	u16 sar_width;
	u16 sar_height;

	u8 video_format;
	u8 colour_primaries;
	u8 transfer_characteristics;
	u8 matrix_coefficients;

	u8 chroma_sample_loc_type_top_field;
	u8 chroma_sample_loc_type_bottom_field;

	u32 num_units_in_tick;
	u32 time_scale;

	struct v4l2_h264_sps_video_hrd nal_hrd;
	struct v4l2_h264_sps_video_hrd vcl_hrd;

	u32 max_bytes_per_pic_denom;
	u32 max_bits_per_mb_denom;
	u32 log2_max_mv_length_horizontal;
	u32 log2_max_mv_length_vertical;
	u32 max_num_reorder_frames;
	u32 max_dec_frame_buffering;

	u32 flags;
};

/**
 * struct v4l2_h264_reflist_builder - Reference list builder object
 *
 * @refs.top_field_order_cnt: top field order count
 * @refs.bottom_field_order_cnt: bottom field order count
 * @refs.frame_num: reference frame number
 * @refs.longterm: set to true for a long term reference
 * @refs: array of references
 * @cur_pic_order_count: picture order count of the frame being decoded
 * @cur_pic_fields: fields present in the frame being decoded
 * @unordered_reflist: unordered list of references. Will be used to generate
 *		       ordered P/B0/B1 lists
 * @num_valid: number of valid references in the refs array
 *
 * This object stores the context of the P/B0/B1 reference list builder.
 * This procedure is described in section '8.2.4 Decoding process for reference
 * picture lists construction' of the H264 spec.
 */
struct v4l2_h264_reflist_builder {
	struct {
		s32 top_field_order_cnt;
		s32 bottom_field_order_cnt;
		int frame_num;
		u16 longterm : 1;
	} refs[V4L2_H264_NUM_DPB_ENTRIES];

	s32 cur_pic_order_count;
	u8 cur_pic_fields;

	struct v4l2_h264_reference unordered_reflist[V4L2_H264_REF_LIST_LEN];
	/* FIXME: confusing with valid flag (active is checked). Proper terminology is "used" */
	u8 num_valid;
};

static inline char v4l2_h264_slice_type_char(unsigned char slice_type)
{
	if (slice_type == V4L2_H264_SLICE_TYPE_I)
		return 'I';
	else if (slice_type == V4L2_H264_SLICE_TYPE_P)
		return 'P';
	else if (slice_type == V4L2_H264_SLICE_TYPE_B)
		return 'B';
	else
		return 'X';
}

static inline const char *v4l2_h264_slice_type_name(unsigned char slice_type)
{
	if (slice_type == V4L2_H264_SLICE_TYPE_I)
		return "intra";
	else if (slice_type == V4L2_H264_SLICE_TYPE_P)
		return "inter-pred";
	else if (slice_type == V4L2_H264_SLICE_TYPE_B)
		return "inter-bipred";
	else
		return "invalid";
}

void
v4l2_h264_init_reflist_builder(struct v4l2_h264_reflist_builder *b,
		const struct v4l2_ctrl_h264_decode_params *dec_params,
		const struct v4l2_ctrl_h264_sps *sps,
		const struct v4l2_h264_dpb_entry dpb[V4L2_H264_NUM_DPB_ENTRIES]);

void
v4l2_h264_init_reflist_builder_gen(struct v4l2_h264_reflist_builder *b,
		const struct v4l2_ctrl_h264_sps *sps,
		const struct v4l2_h264_dpb_entry dpb[V4L2_H264_NUM_DPB_ENTRIES],
		unsigned int pic_order_count, unsigned int frame_num, unsigned int fields);

/**
 * v4l2_h264_build_b_ref_lists() - Build the B0/B1 reference lists
 *
 * @builder: reference list builder context
 * @b0_reflist: 32 sized array used to store the B0 reference list. Each entry
 *		is a v4l2_h264_reference structure
 * @b1_reflist: 32 sized array used to store the B1 reference list. Each entry
 *		is a v4l2_h264_reference structure
 *
 * This functions builds the B0/B1 reference lists. This procedure is described
 * in section '8.2.4 Decoding process for reference picture lists construction'
 * of the H264 spec. This function can be used by H264 decoder drivers that
 * need to pass B0/B1 reference lists to the hardware.
 */
void
v4l2_h264_build_b_ref_lists(const struct v4l2_h264_reflist_builder *builder,
			    struct v4l2_h264_reference *b0_reflist,
			    struct v4l2_h264_reference *b1_reflist);

/**
 * v4l2_h264_build_p_ref_list() - Build the P reference list
 *
 * @builder: reference list builder context
 * @reflist: 32 sized array used to store the P reference list. Each entry
 *	     is a v4l2_h264_reference structure
 *
 * This functions builds the P reference lists. This procedure is describe in
 * section '8.2.4 Decoding process for reference picture lists construction'
 * of the H264 spec. This function can be used by H264 decoder drivers that
 * need to pass a P reference list to the hardware.
 */
void
v4l2_h264_build_p_ref_list(const struct v4l2_h264_reflist_builder *builder,
			   struct v4l2_h264_reference *reflist);

#endif /* _MEDIA_V4L2_H264_H */
