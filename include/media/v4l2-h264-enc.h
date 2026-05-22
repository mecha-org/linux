/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * V4L2 H.264 Encode Core
 *
 * Copyright (C) 2025-2026 Paul Kocialkowski <paulk@sys-base.io>
 */

#ifndef _MEDIA_V4L2_H264_ENC_H
#define _MEDIA_V4L2_H264_ENC_H

#include <linux/v4l2-controls.h>
#include <linux/videodev2.h>
#include <media/videobuf2-v4l2.h>

#define V4L2_H264_ENC_MB_UNIT	16

#define V4L2_H264_ENC_FLAG_INTER_PRED		0x1
#define V4L2_H264_ENC_FLAG_INTER_BIPRED		0x2
#define V4L2_H264_ENC_FLAG_HW_AUD		0x4
#define V4L2_H264_ENC_FLAG_HW_SPS		0x8
#define V4L2_H264_ENC_FLAG_HW_PPS		0x10
#define V4L2_H264_ENC_FLAG_HW_SLICE_HEADER	0x20
#define V4L2_H264_ENC_FLAG_CHROMA_QP_CR_OFFSET	0x40

#define v4l2_h264_enc_op(e, o, a...) \
	({ \
		int ret; \
		if ((e)->ops && (e)->ops->o) \
			ret = (e)->ops->o(e, ##a); \
		else \
			ret = -EOPNOTSUPP; \
		ret; \
	})

struct v4l2_h264_enc;

struct v4l2_h264_enc_state {
	struct v4l2_ctrl_h264_sps sps;
	struct v4l2_h264_sps_video sps_video;
	struct v4l2_ctrl_h264_pps pps;
	struct v4l2_ctrl_h264_encode_params encode;

	struct v4l2_fract timeperframe;

	unsigned int width_mbs;
	unsigned int width_aligned;
	unsigned int height_mbs;
	unsigned int height_aligned;
};

struct v4l2_h264_enc_ops {
	int (*state_constrain)(struct v4l2_h264_enc *enc,
			       struct v4l2_h264_enc_state *state);
};

struct v4l2_h264_enc {
	const struct v4l2_h264_enc_ops *ops;
	void *private_data;

	struct v4l2_pix_format *format;
	struct v4l2_pix_format_mplane *format_mplane;
	struct v4l2_fract *timeperframe;
	struct v4l2_ctrl_handler *ctrl_handler;

	struct v4l2_h264_enc_state state_active;
	struct v4l2_h264_enc_state state_next;
	unsigned int state_serial;

	unsigned int flags;
};

int v4l2_h264_enc_init(struct v4l2_h264_enc *enc);
void v4l2_h264_enc_exit(struct v4l2_h264_enc *enc);
int v4l2_h264_enc_step(struct v4l2_h264_enc *enc,
		       struct vb2_v4l2_buffer *buffer);
int v4l2_h264_enc_complete(struct v4l2_h264_enc *enc,
			   struct vb2_v4l2_buffer *buffer);

#endif
