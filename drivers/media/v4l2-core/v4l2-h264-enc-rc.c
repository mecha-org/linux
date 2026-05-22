// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * V4L2 H.264 Encode Rate Control
 *
 * Copyright (C) 2025-2026 Paul Kocialkowski <paulk@sys-base.io>
 */

#include <linux/module.h>
#include <media/v4l2-h264-enc.h>

int v4l2_h264_enc_rc_init(struct v4l2_h264_enc_rc *rc)
{
	rc->qp = 0;
	rc->mode = NULL;

	return 0;
}
EXPORT_SYMBOL_GPL(v4l2_h264_enc_rc_init);

void v4l2_h264_enc_rc_exit(struct v4l2_h264_enc_rc *rc)
{
	if (rc->mode) {
		v4l2_h264_enc_rc_op(rc, exit);
		v4l2_h264_enc_rc_mode_op(rc, exit);
		rc->mode = NULL;
	}
}
EXPORT_SYMBOL_GPL(v4l2_h264_enc_rc_exit);

int v4l2_h264_enc_rc_stats_collect(struct v4l2_h264_enc_rc *rc,
				   unsigned long bytesused)
{
	struct v4l2_h264_enc_state *state = rc->state;
	struct v4l2_ctrl_h264_encode_params *encode = &state->encode;
	struct v4l2_fract *timeperframe = &state->timeperframe;
	struct v4l2_h264_enc_rc_stats *stats = &rc->stats;
	struct v4l2_h264_enc_rc_stats_type *type;
	unsigned long bitrate;
	long size_error;
	long size_drift;
	long bitrate_error;
	long bitrate_drift;
	unsigned int index;

	/* Collect individual and total frame sizes. */

	index = stats->index;

	if (stats->count == V4L2_H264_ENC_RC_STATS_SIZE_COUNT)
		stats->size_total -= stats->size[index];

	stats->slice_type[index] = encode->slice_type;
	stats->size[index] = bytesused;
	stats->size_total += bytesused;

	if (stats->count < V4L2_H264_ENC_RC_STATS_SIZE_COUNT)
		stats->count++;

	stats->index = (index + 1) % V4L2_H264_ENC_RC_STATS_SIZE_COUNT;

	bitrate = 8 * stats->size_total * timeperframe->denominator /
		  stats->count / timeperframe->numerator;

	size_error = (long)bytesused - (long)rc->size;
	size_drift = 100 * size_error / (long)rc->size;

	bitrate_error = (long)bitrate - (long)state->bitrate;
	bitrate_drift = 100 * bitrate_error / (long)state->bitrate;

	pr_debug("+ v4l2-h264-rc: stats");
	pr_debug("  size: %lu B, target: %lu B, error: %ld B, drift: %ld pc",
		 bytesused, rc->size, size_error, size_drift);
	pr_debug("  bitrate: %lu b/s, target: %u b/s, error: %ld b/s, drift: %ld pc",
		 bitrate, state->bitrate, bitrate_error, bitrate_drift);

	/* Collect per-type size and qp information. */

	if (encode->slice_type == V4L2_H264_SLICE_TYPE_I)
		type = &stats->intra;
	else if (encode->slice_type == V4L2_H264_SLICE_TYPE_P)
		type = &stats->pred;
	else if (encode->slice_type == V4L2_H264_SLICE_TYPE_B)
		type = &stats->bipred;
	else
		return -EINVAL;

	index = type->index;

	type->target[index] = rc->size;
	type->size[index] = bytesused;
	type->qp[index] = rc->qp;

	if (type->count < V4L2_H264_ENC_RC_STATS_TYPE_COUNT)
		type->count++;

	type->index = (index + 1) % V4L2_H264_ENC_RC_STATS_TYPE_COUNT;

	return 0;
}
EXPORT_SYMBOL_GPL(v4l2_h264_enc_rc_stats_collect);

static int direct_estimate(struct v4l2_h264_enc_rc *rc, unsigned int *qp)
{
	struct v4l2_h264_enc_state *state = rc->state;
	struct v4l2_ctrl_h264_encode_params *encode = &state->encode;

	if (encode->slice_type == V4L2_H264_SLICE_TYPE_I)
		*qp = state->qp_i;
	else if (encode->slice_type == V4L2_H264_SLICE_TYPE_P)
		*qp = state->qp_p;
	else if (encode->slice_type == V4L2_H264_SLICE_TYPE_B)
		*qp = state->qp_b;
	else
		return -EINVAL;

	return 0;
}

static const struct v4l2_h264_enc_rc_mode direct_mode = {
	.name		= "direct qp",
	.type		= V4L2_H264_ENC_RC_TYPE_DIRECT,
	.estimate	= direct_estimate,
};

static int cq_estimate(struct v4l2_h264_enc_rc *rc, unsigned int *qp)
{
	struct v4l2_h264_enc_state *state = rc->state;
	struct v4l2_ctrl_h264_encode_params *encode = &state->encode;
	/* TODO: Use some reasonable PSNR to QP mapping. */
	unsigned int qp_range_intra[2] = { 12, 44 };
	unsigned int qp_range_inter[2] = { 16, 48 };
	unsigned int qp_min, qp_max;
	unsigned int qp_diff_range;
	unsigned int quality_diff;
	unsigned int quality_diff_range;

	if (encode->slice_type == V4L2_H264_SLICE_TYPE_I) {
		qp_min = qp_range_intra[0];
		qp_max = qp_range_intra[1];
	} else {
		qp_min = qp_range_inter[0];
		qp_max = qp_range_inter[1];
	}

	qp_diff_range = qp_max - qp_min;

	quality_diff = state->quality - state->quality_min;
	quality_diff_range = state->quality_max - state->quality_min;

	*qp = qp_min + qp_diff_range * quality_diff / quality_diff_range;

	return 0;
}

static const struct v4l2_h264_enc_rc_mode cq_mode = {
	.name		= "constant quality",
	.type		= V4L2_H264_ENC_RC_TYPE_CQ,
	.estimate	= cq_estimate,
};

static int cbr_measure(struct v4l2_h264_enc_rc *rc, unsigned long *size)
{
	struct v4l2_h264_enc_state *state = rc->state;
	struct v4l2_ctrl_h264_encode_params *encode = &state->encode;
	struct v4l2_fract *timeperframe = &state->timeperframe;
	struct v4l2_h264_enc_rc_stats *stats = &rc->stats;
	unsigned long long budget;
	unsigned long target;
	unsigned long target_nominal;

	pr_debug("+ v4l2-h264-enc-rc: cbr measure");

	/* Nominal average size target for exact bitrate. */
	target_nominal = state->bitrate * timeperframe->numerator /
			 timeperframe->denominator / 8;

	/* Total size budget for twice the size window. */
	budget = 2 * V4L2_H264_ENC_RC_STATS_SIZE_COUNT * state->bitrate *
		 timeperframe->numerator / timeperframe->denominator / 8;

	if (budget < stats->size_total) {
		target = target_nominal;
		goto complete;
	}

	/* Remaining size budget after deducing total size in window. */
	budget -= stats->size_total;
	/* Average size target from remaining size budget. */
	target = budget / (2 * V4L2_H264_ENC_RC_STATS_SIZE_COUNT - stats->count);

complete:
	pr_debug("  nominal target: %lu bytes", target_nominal);
	pr_debug("  uniform target: %lu bytes", target);

	/* Bump size for intra frames and reduce for pred frames.
	 * We are better off with good quality initial references.
	 * Maintain average assuming 1 intra frame for 24 inter frames.
	 */
	if (encode->slice_type == V4L2_H264_SLICE_TYPE_I)
		target = target * 6;
	else
		target = target * 80 / 100;

	/* Reduce target size on bitrate overshoot to calm things down. */
	if (9 * stats->size_total > 10 * stats->count * target_nominal) {
		pr_debug("  overshoot, reducing target size");
		target = 80 * target / 100;
	}

	pr_debug("  final target: %lu bytes", target);

	*size = target;

	return 0;
}

/* Intra bit size per macroblock estimation for QP starting at 10. */
static unsigned int cbr_qp_initial_bits_mb[] = {
	437, 411, 376, 355, 318, 303, 291, 271, 252, 238, 218, 204, 188, 171,
	158, 149, 133, 124, 115, 105, 98, 92, 83, 77, 70, 63, 57, 52, 45, 39,
	34, 31, 30, 29, 26, 23, 22, 20, 18, 17, 16, 15
};

static int cbr_qp_initial(struct v4l2_h264_enc_rc *rc, unsigned int *qp)
{
	struct v4l2_h264_enc_state *state = rc->state;
	struct v4l2_ctrl_h264_encode_params *encode = &state->encode;
	unsigned long size_mb;
	unsigned int qp_initial;
	unsigned int threshold;
	unsigned int i;

	size_mb = 8 * rc->size / (state->width_mbs * state->height_mbs);

	pr_debug("  initial estimate from %lu bits/mb", size_mb);

	for (i = 0; i < ARRAY_SIZE(cbr_qp_initial_bits_mb); i++) {
		threshold = cbr_qp_initial_bits_mb[i];

		/* Expect inter frames to be 6 times smaller. */
		if (encode->slice_type != V4L2_H264_SLICE_TYPE_I)
			threshold = cbr_qp_initial_bits_mb[i] / 6;

		if (size_mb >= threshold) {
			qp_initial = 10 + i;
			break;
		}
	}

	/* Fallback to the lowest QP (last in the list). */
	if (i == ARRAY_SIZE(cbr_qp_initial_bits_mb))
		qp_initial = 10 + i - 1;

	*qp = qp_initial;

	return 0;
}

/*
 * This table gives the QP decrease/increase (coded as index, with QP = 0 in
 * the middle) for each size ratio (in base 1000).
 *
 * Intra is a bit agressive since we generally have few intra slices to adapt
 * to a scene change. The QP diff range goes from -4 to +4.
 */
static unsigned long cbr_ratio_qp_diff_steps_intra[] = {
	3000, 2000, 1500, 1250, 800, 666, 500, 333
};

/*
 * Inter is less agressive since we generally have more inter slices to adapt
 * to a scene change and want to give more stability to QP. The QP diff range
 * goes from -3 to +3.
 */
static unsigned long cbr_ratio_qp_diff_steps_inter[] = {
	2000, 1500, 1250, 800, 666, 500
};

static int cbr_ratio_qp_diff(unsigned long ratio, unsigned char slice_type)
{
	unsigned long *steps;
	unsigned int count;
	unsigned int i;

	if (slice_type == V4L2_H264_SLICE_TYPE_I) {
		steps = cbr_ratio_qp_diff_steps_intra;
		count = ARRAY_SIZE(cbr_ratio_qp_diff_steps_intra);
	} else {
		steps = cbr_ratio_qp_diff_steps_inter;
		count = ARRAY_SIZE(cbr_ratio_qp_diff_steps_inter);
	}

	WARN_ON(count % 2);

	for (i = 0; i < count; i++)
		if (ratio > steps[i])
			return -1 * (int)count / 2 + i;

	return count / 2;
}

static int cbr_estimate(struct v4l2_h264_enc_rc *rc, unsigned int *qp)
{
	struct v4l2_h264_enc_state *state = rc->state;
	struct v4l2_ctrl_h264_encode_params *encode = &state->encode;
	struct v4l2_h264_enc_rc_stats *stats = &rc->stats;
	struct v4l2_h264_enc_rc_stats_type *type;
	unsigned int type_index[3];
	unsigned int ratio_qp[3];
	unsigned int penalty[3];
	unsigned int penalty_total;
	unsigned int weight[3];
	unsigned int weight_total;
	int ratio_qp_diff;
	unsigned long diff;
	unsigned long diff_closest;
	unsigned int slot_closest;
	unsigned int index;
	unsigned int count;
	unsigned int total;
	unsigned long ratio;
	unsigned int i;

	if (!stats->count)
		return cbr_qp_initial(rc, qp);

	if (encode->slice_type == V4L2_H264_SLICE_TYPE_I)
		type = &stats->intra;
	else if (encode->slice_type == V4L2_H264_SLICE_TYPE_P)
		type = &stats->pred;
	else if (encode->slice_type == V4L2_H264_SLICE_TYPE_B)
		type = &stats->bipred;
	else
		return -EINVAL;

	pr_debug("+ v4l2-h264-enc-rc: cbr estimate");

	if (!type->count)
		return cbr_qp_initial(rc, qp);

	for (i = 0; i < 3; i++) {
		if (i == type->count)
			break;

		index = type->index;
		if (index <= i)
			index = type->count + index - (i + 1);
		else
			index -= i + 1;

		/* Set initial penalty based on frame age. */
		penalty[i] = 4 * i;
		type_index[i] = index;

		/* Track frame with size closest to target. */
		diff = abs((long)rc->size - type->size[index]);

		if (!i || diff < diff_closest) {
			diff_closest = diff;
			slot_closest = i;
		}
	}

	count = i;
	penalty_total = 0;

	for (i = 0; i < count; i++) {
		/* Add penalty for frames with larger size difference. */
		if (i != slot_closest)
			penalty[i] += 10;

		penalty_total += penalty[i];

		index = type_index[i];

		/* Calculate QP from target to observed size ratio. */
		ratio = 1000UL * rc->size / type->size[index];

		ratio_qp_diff = cbr_ratio_qp_diff(ratio, encode->slice_type);

		if ((int)type->qp[index] + ratio_qp_diff < 0)
			ratio_qp[i] = 0;
		else if ((int)type->qp[index] + ratio_qp_diff > 51)
			ratio_qp[i] = 51;
		else
			ratio_qp[i] = type->qp[index] + ratio_qp_diff;

		diff = abs((long)rc->size - type->size[index]);

		pr_debug("  - backlog %d size: %lu (diff: %lu), qp: %u, penalty %u, ratio qp: %u",
			 -(i + 1), type->size[index], diff, type->qp[index],
			 penalty[i], ratio_qp[i]);
	}

	total = 0;
	weight_total = 0;

	for (i = 0; i < count; i++) {
		/* Weight each calculated QP using the penalty ratio. */
		if (penalty_total)
			weight[i] = 1000 * (penalty_total - penalty[i]) /
				    penalty_total;
		else
			weight[i] = 1000;

		/* Give non-zero low weight to full penalty cases. */
		if (weight[i] < 200)
			weight[i] = 200;

		total += ratio_qp[i] * weight[i];
		weight_total += weight[i];

		pr_debug("  - backlog %d weight: %u", -(i + 1),
			 weight[i]);
	}

	*qp = total / weight_total;

	return 0;
}

static int cbr_complete(struct v4l2_h264_enc_rc *rc, unsigned long bytesused)
{
	return v4l2_h264_enc_rc_stats_collect(rc, bytesused);
}

static const struct v4l2_h264_enc_rc_mode cbr_mode = {
	.name		= "constant bitrate",
	.type		= V4L2_H264_ENC_RC_TYPE_CBR,
	.measure	= cbr_measure,
	.estimate	= cbr_estimate,
	.complete	= cbr_complete,
};

static const struct v4l2_h264_enc_rc_mode *modes[] = {
	&direct_mode,
	&cq_mode,
	&cbr_mode,
};

static int mode_prepare(struct v4l2_h264_enc_rc *rc)
{
	struct v4l2_h264_enc_state *state = rc->state;
	const struct v4l2_h264_enc_rc_mode *mode = NULL;
	unsigned int i;
	int type;
	int ret;

	if (!state->frame_rc_enable)
		type = V4L2_H264_ENC_RC_TYPE_DIRECT;
	else if (state->bitrate_mode == V4L2_MPEG_VIDEO_BITRATE_MODE_CQ)
		type = V4L2_H264_ENC_RC_TYPE_CQ;
	else if (state->bitrate_mode == V4L2_MPEG_VIDEO_BITRATE_MODE_CBR)
		type = V4L2_H264_ENC_RC_TYPE_CBR;
	else if (state->bitrate_mode == V4L2_MPEG_VIDEO_BITRATE_MODE_VBR)
		type = V4L2_H264_ENC_RC_TYPE_VBR;
	else
		return -EINVAL;

	for (i = 0; i < ARRAY_SIZE(modes); i++) {
		if (modes[i] && modes[i]->type == type) {
			mode = modes[i];
			break;
		}
	}

	if (!mode)
		return -EINVAL;

	memset(&rc->stats, 0, sizeof(rc->stats));

	rc->mode = mode;

	ret = v4l2_h264_enc_rc_op(rc, init);
	if (ret && ret != -EOPNOTSUPP)
		return ret;

	ret = v4l2_h264_enc_rc_mode_op(rc, init);
	if (ret && ret != -EOPNOTSUPP)
		return ret;

	return 0;
}

int v4l2_h264_enc_rc_mode_update(struct v4l2_h264_enc_rc *rc)
{
	if (!rc->mode)
		return 0;

	v4l2_h264_enc_rc_op(rc, exit);
	v4l2_h264_enc_rc_mode_op(rc, exit);

	rc->mode = NULL;

	return 0;
}
EXPORT_SYMBOL_GPL(v4l2_h264_enc_rc_mode_update);

int v4l2_h264_enc_rc_step(struct v4l2_h264_enc_rc *rc,
			  struct v4l2_h264_enc_state *state)
{
	unsigned int qp;
	int ret;

	rc->state = state;

	if (!rc->mode) {
		ret = mode_prepare(rc);
		if (ret)
			return ret;
	}

	ret = v4l2_h264_enc_rc_mode_op(rc, measure, &rc->size);
	if (ret && ret != -EOPNOTSUPP)
		return ret;

	/* Estimate using driver implementation first. */
	ret = v4l2_h264_enc_rc_op(rc, estimate, &qp);
	if (ret && ret != -EOPNOTSUPP)
		return ret;

	/* Fallback to common implementation otherwise. */
	if (ret == -EOPNOTSUPP) {
		ret = v4l2_h264_enc_rc_mode_op(rc, estimate, &qp);
		if (ret)
			return ret;
	}

	rc->qp = clamp(qp, state->qp_min, state->qp_max);

	return 0;

}
EXPORT_SYMBOL_GPL(v4l2_h264_enc_rc_step);

int v4l2_h264_enc_rc_complete(struct v4l2_h264_enc_rc *rc,
			      unsigned long bytesused)
{
	int ret;

	if (!rc->mode)
		return -EINVAL;

	ret = v4l2_h264_enc_rc_op(rc, complete, bytesused);
	if (ret && ret != -EOPNOTSUPP)
		return ret;

	ret = v4l2_h264_enc_rc_mode_op(rc, complete, bytesused);
	if (ret && ret != -EOPNOTSUPP)
		return ret;

	return 0;
}
EXPORT_SYMBOL_GPL(v4l2_h264_enc_rc_complete);

MODULE_DESCRIPTION("V4L2 H.264 Encode Rate Control");
MODULE_AUTHOR("Paul Kocialkowski <paulk@sys-base.io>");
MODULE_LICENSE("GPL");
