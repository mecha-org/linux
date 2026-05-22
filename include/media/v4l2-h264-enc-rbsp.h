/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * V4L2 H.264 Encode RBSP
 *
 * Copyright (C) 2025-2026 Paul Kocialkowski <paulk@sys-base.io>
 */

#ifndef _MEDIA_V4L2_H264_ENC_RBSP_H
#define _MEDIA_V4L2_H264_ENC_RBSP_H

#include <linux/v4l2-controls.h>
#include <media/v4l2-h264.h>

#define V4L2_H264_ENC_RBSP_EPTB		0x3

#define V4L2_H264_ENC_RBSP_UPDATE_START_CODE	0x1
#define V4L2_H264_ENC_RBSP_UPDATE_AUD		0x2
#define V4L2_H264_ENC_RBSP_UPDATE_SPS		0x4
#define V4L2_H264_ENC_RBSP_UPDATE_PPS		0x8
#define V4L2_H264_ENC_RBSP_UPDATE_SLICE_HEADER	0x10

#define v4l2_h264_enc_rbsp_op(r, o, a...) \
	({ \
		int ret; \
		if ((r)->ops && (r)->ops->o) \
			ret = (r)->ops->o(r, ##a); \
		else \
			ret = -EOPNOTSUPP; \
		ret; \
	})

struct v4l2_h264_enc_rbsp;

struct v4l2_h264_enc_rbsp_ops {
	int (*bits_raw)(struct v4l2_h264_enc_rbsp *rbsp, u32 value,
			unsigned char bits_count);
	int (*bits)(struct v4l2_h264_enc_rbsp *rbsp, u32 value,
		    unsigned char bits_count);
	int (*ue)(struct v4l2_h264_enc_rbsp *rbsp, u32 value);
	int (*se)(struct v4l2_h264_enc_rbsp *rbsp, s32 value);
	int (*align)(struct v4l2_h264_enc_rbsp *rbsp);
};

struct v4l2_h264_enc_rbsp {
	const struct v4l2_h264_enc_rbsp_ops *ops;
	void *private_data;

	u8 *pointer;
	unsigned int size;
	unsigned char bit_offset;
	unsigned int bits_count;
	unsigned int zero_count;
};

int v4l2_h264_enc_rbsp_init(struct v4l2_h264_enc_rbsp *rbsp, u8 *pointer,
			    unsigned int size);
unsigned int v4l2_h264_enc_rbsp_bits_count(struct v4l2_h264_enc_rbsp *rbsp);
unsigned int v4l2_h264_enc_rbsp_bytes_count(struct v4l2_h264_enc_rbsp *rbsp);
int v4l2_h264_enc_rbsp_start_code(struct v4l2_h264_enc_rbsp *rbsp);
int v4l2_h264_enc_rbsp_aud(struct v4l2_h264_enc_rbsp *rbsp,
			   u8 primary_pic_type);
int v4l2_h264_enc_rbsp_sps(struct v4l2_h264_enc_rbsp *rbsp,
			   const struct v4l2_ctrl_h264_sps *sps,
			   const struct v4l2_h264_sps_video *sps_video);
int v4l2_h264_enc_rbsp_pps(struct v4l2_h264_enc_rbsp *rbsp,
			   const struct v4l2_ctrl_h264_pps *pps);
int v4l2_h264_enc_rbsp_slice_header(struct v4l2_h264_enc_rbsp *rbsp,
				    const struct v4l2_ctrl_h264_sps *sps,
				    const struct v4l2_ctrl_h264_pps *pps,
				    const struct v4l2_ctrl_h264_encode_params *encode);

#endif
