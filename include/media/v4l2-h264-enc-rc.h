/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * V4L2 H.264 Encode Rate Control
 *
 * Copyright (C) 2025-2026 Paul Kocialkowski <paulk@sys-base.io>
 */

#ifndef _MEDIA_V4L2_H264_ENC_RC_H
#define _MEDIA_V4L2_H264_ENC_RC_H

#include <linux/v4l2-controls.h>
#include <media/v4l2-h264.h>

#define V4L2_H264_ENC_RC_STATS_TYPE_COUNT	10
#define V4L2_H264_ENC_RC_STATS_SIZE_COUNT	25

#define v4l2_h264_enc_rc_op(r, o, a...) \
	({ \
		int ret; \
		if ((r)->ops && (r)->ops->o) \
			ret = (r)->ops->o(r, ##a); \
		else \
			ret = -EOPNOTSUPP; \
		ret; \
	})

#define v4l2_h264_enc_rc_mode_op(r, o, a...) \
	({ \
		int ret; \
		if ((r)->mode && (r)->mode->o) \
			ret = (r)->mode->o(r, ##a); \
		else \
			ret = -EOPNOTSUPP; \
		ret; \
	})

struct v4l2_h264_enc_rc;
struct v4l2_h264_enc_state;
struct vb2_v4l2_buffer;

enum v4l2_h264_enc_rc_type {
	V4L2_H264_ENC_RC_TYPE_DIRECT,
	V4L2_H264_ENC_RC_TYPE_CQ,
	V4L2_H264_ENC_RC_TYPE_CBR,
	V4L2_H264_ENC_RC_TYPE_VBR,
};

struct v4l2_h264_enc_rc_mode {
	char name[32];
	int type;
	int (*init)(struct v4l2_h264_enc_rc *rc);
	int (*exit)(struct v4l2_h264_enc_rc *rc);
	int (*measure)(struct v4l2_h264_enc_rc *rc, unsigned long *size);
	int (*estimate)(struct v4l2_h264_enc_rc *rc, unsigned int *qp);
	int (*complete)(struct v4l2_h264_enc_rc *rc, unsigned long bytesused);
};

struct v4l2_h264_enc_rc_ops {
	int (*init)(struct v4l2_h264_enc_rc *rc);
	int (*exit)(struct v4l2_h264_enc_rc *rc);
	int (*estimate)(struct v4l2_h264_enc_rc *rc, unsigned int *qp);
	int (*complete)(struct v4l2_h264_enc_rc *rc, unsigned long bytesused);
};

struct v4l2_h264_enc_rc_stats_type {
	unsigned long target[V4L2_H264_ENC_RC_STATS_TYPE_COUNT];
	unsigned long size[V4L2_H264_ENC_RC_STATS_TYPE_COUNT];
	unsigned int qp[V4L2_H264_ENC_RC_STATS_TYPE_COUNT];
	unsigned int count;
	unsigned int index;
};

struct v4l2_h264_enc_rc_stats {
	struct v4l2_h264_enc_rc_stats_type intra;
	struct v4l2_h264_enc_rc_stats_type pred;
	struct v4l2_h264_enc_rc_stats_type bipred;

	unsigned char slice_type[V4L2_H264_ENC_RC_STATS_SIZE_COUNT];
	unsigned long size[V4L2_H264_ENC_RC_STATS_SIZE_COUNT];
	unsigned long long size_total;
	unsigned int count;
	unsigned int index;
};

struct v4l2_h264_enc_rc {
	const struct v4l2_h264_enc_rc_ops *ops;
	void *private_data;

	struct v4l2_h264_enc_state *state;
	const struct v4l2_h264_enc_rc_mode *mode;
	void *mode_data;

	struct v4l2_h264_enc_rc_stats stats;
	unsigned long size;
	unsigned int qp;
};

int v4l2_h264_enc_rc_init(struct v4l2_h264_enc_rc *rc);
void v4l2_h264_enc_rc_exit(struct v4l2_h264_enc_rc *rc);
int v4l2_h264_enc_rc_stats_collect(struct v4l2_h264_enc_rc *rc,
				   unsigned long bytesused);
int v4l2_h264_enc_rc_mode_update(struct v4l2_h264_enc_rc *rc);
int v4l2_h264_enc_rc_step(struct v4l2_h264_enc_rc *rc,
			  struct v4l2_h264_enc_state *state);
int v4l2_h264_enc_rc_complete(struct v4l2_h264_enc_rc *rc,
			      unsigned long bytesused);

#endif
