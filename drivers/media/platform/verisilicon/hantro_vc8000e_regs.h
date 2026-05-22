/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Hantro VPU codec driver
 *
 * Copyright (C) 2025-2026 Paul Kocialkowski <paulk@sys-base.io>
 */

#ifndef HANTRO_VC8000E_REGS_H_
#define HANTRO_VC8000E_REGS_H_

#include <linux/types.h>

#define HANTRO_VC8000E_SWREG_OFFSET(s) \
	offsetof(struct hantro_vc8000e_regs, s)

#define hantro_vc8000e_swreg_read(v, r, s) \
	({ \
		u32 *pointer = (u32 *)&(r)->s; \
		*pointer = vepu_read(v, HANTRO_VC8000E_SWREG_OFFSET(s)); \
	})

#define hantro_vc8000e_swreg_write(v, r, s) \
	({ \
		u32 *pointer = (u32 *)&(r)->s; \
		vepu_write(v, *pointer, HANTRO_VC8000E_SWREG_OFFSET(s)); \
	})

#define HANTRO_VC8000E_SWREG0_MAJOR_NUMBER_V6_0		0x60
#define HANTRO_VC8000E_SWREG0_MAJOR_NUMBER_V6_1		0x61
#define HANTRO_VC8000E_SWREG0_MAJOR_NUMBER_V6_2		0x62
#define HANTRO_VC8000E_SWREG0_PRODUCT_ID_VC8000E	0x8000

#define HANTRO_VC8000E_SWREG4_OUTPUT_STRM_MODE_BYTE_STREAM	0x0
#define HANTRO_VC8000E_SWREG4_OUTPUT_STRM_MODE_NAL_STREAM	0x1
#define HANTRO_VC8000E_SWREG4_TRB_SIZE_4X4			0x0
#define HANTRO_VC8000E_SWREG4_TRB_SIZE_8X8			0x1
#define HANTRO_VC8000E_SWREG4_TRB_SIZE_16X16			0x2
#define HANTRO_VC8000E_SWREG4_TRB_SIZE_32X32			0x3
#define HANTRO_VC8000E_SWREG4_CB_SIZE_8X8			0x0
#define HANTRO_VC8000E_SWREG4_CB_SIZE_16X16			0x1
#define HANTRO_VC8000E_SWREG4_CB_SIZE_32X32			0x2
#define HANTRO_VC8000E_SWREG4_CB_SIZE_64X64			0x3
#define HANTRO_VC8000E_SWREG4_MODE_HEVC				0x1
#define HANTRO_VC8000E_SWREG4_MODE_H264				0x2
#define HANTRO_VC8000E_SWREG4_MODE_JPEG				0x4

#define HANTRO_VC8000E_SWREG5_FRAME_CODING_TYPE_P	0x0
#define HANTRO_VC8000E_SWREG5_FRAME_CODING_TYPE_I	0x1
#define HANTRO_VC8000E_SWREG5_FRAME_CODING_TYPE_B	0x2

#define HANTRO_VC8000E_SWREG36_OUTPUT_BITWIDTH_CHROMA_8_BIT	0x0
#define HANTRO_VC8000E_SWREG36_OUTPUT_BITWIDTH_CHROMA_9_BIT	0x1
#define HANTRO_VC8000E_SWREG36_OUTPUT_BITWIDTH_CHROMA_10_BIT	0x2

#define HANTRO_VC8000E_SWREG38_OUTPUT_BITWIDTH_LUM_8_BIT	0x0
#define HANTRO_VC8000E_SWREG38_OUTPUT_BITWIDTH_LUM_9_BIT	0x1
#define HANTRO_VC8000E_SWREG38_OUTPUT_BITWIDTH_LUM_10_BIT	0x2
#define HANTRO_VC8000E_SWREG38_INPUT_ROTATION_0			0x0
#define HANTRO_VC8000E_SWREG38_INPUT_ROTATION_90		0x1
#define HANTRO_VC8000E_SWREG38_INPUT_ROTATION_270		0x2
#define HANTRO_VC8000E_SWREG38_INPUT_ROTATION_180		0x3
#define HANTRO_VC8000E_SWREG38_INPUT_FORMAT_YUV420P		0x0
#define HANTRO_VC8000E_SWREG38_INPUT_FORMAT_YUV420SP		0x1
#define HANTRO_VC8000E_SWREG38_INPUT_FORMAT_YUYV422		0x2
#define HANTRO_VC8000E_SWREG38_INPUT_FORMAT_UYVY422		0x3
#define HANTRO_VC8000E_SWREG38_INPUT_FORMAT_RGB565		0x4
#define HANTRO_VC8000E_SWREG38_INPUT_FORMAT_RGB555		0x5
#define HANTRO_VC8000E_SWREG38_INPUT_FORMAT_RGB444		0x6
#define HANTRO_VC8000E_SWREG38_INPUT_FORMAT_RGB888		0x7
#define HANTRO_VC8000E_SWREG38_INPUT_FORMAT_RGB101010		0x8
#define HANTRO_VC8000E_SWREG38_INPUT_FORMAT_I010		0x9
#define HANTRO_VC8000E_SWREG38_INPUT_FORMAT_P010		0xa
#define HANTRO_VC8000E_SWREG38_INPUT_FORMAT_PACKED10BITPLANAR	0xb
#define HANTRO_VC8000E_SWREG38_INPUT_FORMAT_Y0L2		0xc
#define HANTRO_VC8000E_SWREG38_INPUT_FORMAT_DAHUAHEVC		0xd
#define HANTRO_VC8000E_SWREG38_INPUT_FORMAT_DAHUAH264		0xe

#define HANTRO_VC8000E_SWREG80_HWBUSWIDTH_32_BIT	0x0
#define HANTRO_VC8000E_SWREG80_HWBUSWIDTH_64_BIT	0x1
#define HANTRO_VC8000E_SWREG80_HWBUSWIDTH_128_BIT	0x2
#define HANTRO_VC8000E_SWREG80_HWBUS_AHB		0x1
#define HANTRO_VC8000E_SWREG80_HWBUS_OCP		0x2
#define HANTRO_VC8000E_SWREG80_HWBUS_AXI		0x3
#define HANTRO_VC8000E_SWREG80_HWBUS_PCI		0x4
#define HANTRO_VC8000E_SWREG80_HWBUS_AXIAHB		0x5
#define HANTRO_VC8000E_SWREG80_HWBUS_AXIAPB		0x6

#define HANTRO_VC8000E_SWREG_ROI_QP_TYPE_DELTA		0x0
#define HANTRO_VC8000E_SWREG_ROI_QP_TYPE_ABSOLUTE	0x1

#define HANTRO_VC8000E_SWREG181_RC_BLOCK_SIZE_64X64	0x0
#define HANTRO_VC8000E_SWREG181_RC_BLOCK_SIZE_32X32	0x1
#define HANTRO_VC8000E_SWREG181_RC_BLOCK_SIZE_16X16	0x2

#define HANTRO_VC8000E_SWREG199_HASH_TYPE_NONE		0x0
#define HANTRO_VC8000E_SWREG199_HASH_TYPE_CRC32		0x1
#define HANTRO_VC8000E_SWREG199_HASH_TYPE_CHECKSUM32	0x2

#define HANTRO_VC8000E_SWREG214_HWROIMAPVERSION_4_BPP	0x0
#define HANTRO_VC8000E_SWREG214_HWROIMAPVERSION_8_BPP	0x1

#define HANTRO_VC8000E_SWREG226_HWINLOOPDSRATIO_1_1			0x0
#define HANTRO_VC8000E_SWREG226_HWINLOOPDSRATIO_1_2			0x1
#define HANTRO_VC8000E_SWREG226_BFRAME_ME4N_HOR_SEARCHRANGE_64		0x0
#define HANTRO_VC8000E_SWREG226_BFRAME_ME4N_HOR_SEARCHRANGE_128		0x1
#define HANTRO_VC8000E_SWREG226_BFRAME_ME4N_HOR_SEARCHRANGE_192		0x2
#define HANTRO_VC8000E_SWREG226_BFRAME_ME4N_HOR_SEARCHRANGE_256		0x3
#define HANTRO_VC8000E_SWREG226_HWP010REFSUPPORT_NORMAL			0x0
#define HANTRO_VC8000E_SWREG226_HWP010REFSUPPORT_TILED_P010		0x1

#define HANTRO_VC8000E_SWREG272_URGENT_THRESHOLD_DISABLE	0xff

#define HANTRO_VC8000E_SWREG281_CHROMA_FORMAT_IDC_400	0x0
#define HANTRO_VC8000E_SWREG281_CHROMA_FORMAT_IDC_420	0x1

struct hantro_vc8000e_regs {
	struct {
		u32 minor_number	: 8;
		u32 major_number	: 8;
		u32 product_id		: 16;
	} swreg0;

	struct {
		u32 irq				: 1;
		u32 irq_dis			: 1;
		u32 frame_rdy_status		: 1;
		u32 bus_error_status		: 1;
		u32 sw_reset			: 1;
		u32 buffer_full			: 1;
		u32 timeout			: 1;
		u32 irq_line_buffer		: 1;
		u32 slice_rdy_status		: 1;
		u32 irq_fuse_error		: 1;
		u32 reserved0			: 1;
		u32 timeout_int			: 1;
		u32 strm_segment_rdy_int	: 1;
		u32 reserved1			: 19;
	} swreg1;

	struct {
		u32 ctb_rc_mem_out_swap		: 4;
		u32 roi_map_qp_delta_map_swap	: 4;
		u32 pic_swap			: 4;
		u32 strm_swap			: 4;
		u32 axi_read_id			: 8;
		u32 axi_write_id		: 8;
	} swreg2;

	struct {
		u32 reserved0			: 1;
		u32 strm_segment_int		: 1;
		u32 line_buffer_int		: 1;
		u32 slice_int			: 1;
		u32 reserved1			: 16;
		u32 cu_info_mem_out_swap	: 4;
		u32 axi_rd_id_e			: 1;
		u32 axi_wr_id_e			: 1;
		u32 clock_gate_inter_h264_e	: 1;
		u32 clock_gate_inter_h265_e	: 1;
		u32 clock_gate_inter_e		: 1;
		u32 clock_gate_encoder_h264_e	: 1;
		u32 clock_gate_encoder_h265_e	: 1;
		u32 clock_gate_encoder_e	: 1;
	} swreg3;

	struct {
		u32 max_trans_hierarchy_depth_inter	: 3;
		u32 max_trans_hierarchy_depth_intra	: 3;
		u32 sao_enable				: 1;
		u32 active_override_flag		: 1;
		u32 scaling_list_enabled_flag		: 1;
		u32 reserved0				: 2;
		u32 bw_linebuf_disable			: 1;
		u32 strong_intra_smoothing_enabled_flag	: 1;
		u32 chroma_qp_offset			: 5;
		u32 output_strm_mode			: 1;
		u32 max_trb_size			: 2;
		u32 min_trb_size			: 2;
		u32 max_cb_size				: 2;
		u32 min_cb_size				: 2;
		u32 reserved1				: 2;
		u32 mode				: 3;
	} swreg4;

	union {
		struct {
			u32 enable					: 1;
			u32 frame_coding_type				: 2;
			u32 ref_frames					: 2;
			u32 buffer_full_continue			: 1;
			u32 output_cu_info_enabled			: 1;
			u32 reserved0					: 1;
			u32 slice_deblocking_filter_override_flag	: 1;
			u32 pps_deblocking_filter_override_enabled_flag	: 1;
			u32 reserved1					: 1;
			u32 pic_height					: 11;
			u32 pic_width					: 10;
		} swreg5;

		struct {
			u32 reserved		: 8;
			u32 jpeg_pic_height	: 12;
			u32 jpeg_pic_width	: 12;
		} swreg5_jpeg;
	};

	struct {
		u32 cu_qp_delta_enabled		: 1;
		u32 nal_size_write		: 1;
		u32 rps_id			: 5;
		u32 deblocking_beta_offset	: 4;
		u32 deblocking_tc_offset	: 4;
		u32 deblocking_filter_dis	: 1;
		u32 num_positive_pics		: 2;
		u32 num_negative_pics		: 2;
		u32 num_short_term_ref_pic_sets	: 5;
		u32 slice_size			: 7;
	} swreg6;

	struct {
		u32 roi2_delta_qp		: 4;
		u32 roi1_delta_qp		: 4;
		u32 pic_qp			: 6;
		u32 diff_cu_qp_delta_depth	: 2;
		u32 reserved			: 1;
		u32 num_slices_ready		: 8;
		u32 cabac_init_flag		: 1;
		u32 pic_init_qp			: 6;
	} swreg7;

	struct {
		u32 output_strm_base	: 32;
	} swreg8;

	struct {
		u32 output_strm_buffer_limit	: 32;
	} swreg9;

	struct {
		u32 size_tbl_base	: 32;
	} swreg10;

	struct {
		u32 poc	: 32;
	} swreg11;

	struct {
		u32 input_y_base	: 32;
	} swreg12;

	struct {
		u32 input_cb_base	: 32;
	} swreg13;

	struct {
		u32 input_cr_base	: 32;
	} swreg14;

	struct {
		u32 recon_y_base	: 32;
	} swreg15;

	struct {
		u32 recon_chroma_base	: 32;
	} swreg16;

	struct {
		u32 l0_ref1_chroma_compressor_enable	: 1;
		u32 l0_ref1_luma_compressor_enable	: 1;
		u32 l0_ref0_chroma_compressor_enable	: 1;
		u32 l0_ref0_luma_compressor_enable	: 1;
		u32 recon_chroma_compressor_enable	: 1;
		u32 recon_luma_compressor_enable	: 1;
		u32 active_l0_cnt			: 2;
		u32 l0_used_by_curr_pic1		: 1;
		u32 l0_long_term_flag1			: 1;
		u32 l0_delta_poc1			: 10;
		u32 l0_used_by_curr_pic0		: 1;
		u32 l0_long_term_flag0			: 1;
		u32 l0_delta_poc0			: 10;
	} swreg17;

	union {
		struct {
			u32 refpic_recon_l0_y0	: 32;
		} swreg18;

		struct {
			u32 jpeg_rst		: 16;
			u32 jpeg_rstint		: 8;
			u32 jpeg_mode		: 1;
			u32 jpeg_slice		: 1;
			u32 strm_startoffset	: 6;
		} swreg18_jpeg;
	};

	union {
		struct {
			u32 refpic_recon_l0_chroma0	: 32;
		} swreg19;

		struct {
			u32 strm_hdrrem1	: 32;
		} swreg19_jpeg;
	};

	union {
		struct {
			u32 refpic_recon_l0_y1	: 32;
		} swreg20;

		struct {
			u32 reserved		: 8;
			u32 ljpeg_pt		: 3;
			u32 ljpeg_psv		: 3;
			u32 ljpeg_format	: 2;
			u32 ljpeg_en		: 1;
			u32 jpeg_rowlength	: 15;
		} swreg20_jpeg;
	};

	union {
		struct {
			u32 refpic_recon_l0_chroma1	: 32;
		} swreg21;

		struct {
			u32 strm_hdrrem2	: 32;
		} swreg21_jpeg;
	};

	struct {
		u32 roi_area_enable	: 1;
		u32 roi_map_enable	: 1;
		u32 qadj_enable		: 1;
		u32 rc_enable		: 1;
		u32 cir_interval	: 14;
		u32 cir_start		: 14;
	} swreg22;

	struct {
		u32 intra_area_bottom	: 8;
		u32 intra_area_top	: 8;
		u32 intra_area_right	: 8;
		u32 intra_area_left	: 8;
	} swreg23;

	struct {
		u32 roi1_bottom	: 8;
		u32 roi1_top	: 8;
		u32 roi1_right	: 8;
		u32 roi1_left	: 8;
	} swreg24;

	struct {
		u32 roi2_bottom	: 8;
		u32 roi2_top	: 8;
		u32 roi2_right	: 8;
		u32 roi2_left	: 8;
	} swreg25;

	struct {
		u32 reserved		: 2;
		u32 intra_size_factor_2	: 10;
		u32 intra_size_factor_1	: 10;
		u32 intra_size_factor_0	: 10;
	} swreg26;

	struct {
		u32 reserved		: 4;
		u32 intra_mode_factor_2	: 7;
		u32 intra_mode_factor_1	: 6;
		u32 intra_mode_factor_0	: 5;
		u32 intra_size_factor_3	: 10;
	} swreg27;

	struct {
		u32 reserved		: 6;
		u32 lambda_satd_me_1	: 13;
		u32 lambda_satd_me_0	: 13;
	} swreg28;

	struct {
		u32 reserved		: 6;
		u32 lambda_satd_me_3	: 13;
		u32 lambda_satd_me_2	: 13;
	} swreg29;

	struct {
		u32 reserved		: 6;
		u32 lambda_satd_me_5	: 13;
		u32 lambda_satd_me_4	: 13;
	} swreg30;

	struct {
		u32 reserved		: 6;
		u32 lambda_satd_me_7	: 13;
		u32 lambda_satd_me_6	: 13;
	} swreg31;

	struct {
		u32 reserved		: 6;
		u32 lambda_satd_me_9	: 13;
		u32 lambda_satd_me_8	: 13;
	} swreg32;

	struct {
		u32 reserved		: 6;
		u32 lambda_satd_me_11	: 13;
		u32 lambda_satd_me_10	: 13;
	} swreg33;

	struct {
		u32 reserved		: 6;
		u32 lambda_satd_me_13	: 13;
		u32 lambda_satd_me_12	: 13;
	} swreg34;

	struct {
		u32 bits_est_bias_intra_cu_16	: 8;
		u32 bits_est_bias_intra_cu_8	: 7;
		u32 bits_est_tu_split_penalty	: 3;
		u32 lambda_motion_sse		: 14;
	} swreg35;

	struct {
		u32 output_bitwidth_chroma	: 2;
		u32 bits_est_1n_cu_penalty	: 4;
		u32 inter_skip_bias		: 7;
		u32 bits_est_bias_intra_cu_64	: 10;
		u32 bits_est_bias_intra_cu_32	: 9;
	} swreg36;

	struct {
		u32 chroffset		: 4;
		u32 lambda_sao_luma	: 14;
		u32 lambda_sao_chroma	: 14;
	} swreg37;

	struct {
		u32 mirror		: 1;
		u32 yfill		: 3;
		u32 xfill		: 2;
		u32 rowlength		: 14;
		u32 lumoffset		: 4;
		u32 output_bitwidth_lum	: 2;
		u32 input_rotation	: 2;
		u32 input_format	: 4;
	} swreg38;

	struct {
		u32 rgbcoeffb	: 16;
		u32 rgbcoeffa	: 16;
	} swreg39;

	struct {
		u32 rgbcoeffe	: 16;
		u32 rgbcoeffc	: 16;
	} swreg40;

	struct {
		u32 reserved	: 1;
		u32 bmaskmsb	: 5;
		u32 gmaskmsb	: 5;
		u32 rmaskmsb	: 5;
		u32 rgbcoefff	: 16;
	} swreg41;

	struct {
		u32 basescaledoutlum	: 32;
	} swreg42;

	struct {
		u32 scale_mode		: 2;
		u32 scaledoutwidthmsb	: 1;
		u32 scaledoutwidthratio	: 16;
		u32 scaledoutwidth	: 13;
	} swreg43;

	struct {
		u32 input_format_msb		: 2;
		u32 scaledoutheightratio	: 16;
		u32 scaledoutheight		: 14;
	} swreg44;

	struct {
		u32 reserved			: 2;
		u32 scaledout_format		: 1;
		u32 nalunitsize_swap		: 4;
		u32 scaledverticalcopy		: 1;
		u32 scaledhorizontalcopy	: 1;
		u32 vscale_weight_en		: 1;
		u32 scaledskiptoppixelrow	: 2;
		u32 scaledskipleftpixelcolumn	: 2;
		u32 encoded_ctb_number		: 13;
		u32 chroma_swap			: 1;
		u32 scaledout_swap		: 4;
	} swreg45;

	struct {
		u32 compressedcoeff_base	: 32;
	} swreg46;

	struct {
		u32 compressedcoeff_base_msb	: 32;
	} swreg47;

	struct {
		u32 basescaledoutlum_msb	: 32;
	} swreg48;

	struct {
		u32 refpic_recon_l0_y0_msb	: 32;
	} swreg49;

	struct {
		u32 refpic_recon_l0_chroma0_msb	: 32;
	} swreg50;

	struct {
		u32 refpic_recon_l0_y1_msb	: 32;
	} swreg51;

	struct {
		u32 refpic_recon_l0_chroma1_msb	: 32;
	} swreg52;

	struct {
		u32 input_y_base_msb	: 32;
	} swreg53;

	struct {
		u32 input_cb_base_msb	: 32;
	} swreg54;

	struct {
		u32 input_cr_base_msb	: 32;
	} swreg55;

	struct {
		u32 recon_y_base_msb	: 32;
	} swreg56;

	struct {
		u32 recon_chroma_base_msb	: 32;
	} swreg57;

	struct {
		u32 size_tbl_base_msb	: 32;
	} swreg58;

	struct {
		u32 output_strm_base_msb	: 32;
	} swreg59;

	struct {
		u32 recon_luma_compress_table_base	: 32;
	} swreg60;

	struct {
		u32 recon_luma_compress_table_base_msb	: 32;
	} swreg61;

	struct {
		u32 recon_chroma_compress_table_base	: 32;
	} swreg62;

	struct {
		u32 recon_chroma_compress_table_base_msb	: 32;
	} swreg63;

	struct {
		u32 l0_ref0_luma_compress_table_base	: 32;
	} swreg64;

	struct {
		u32 l0_ref0_luma_compress_table_base_msb	: 32;
	} swreg65;

	struct {
		u32 l0_ref0_chroma_compress_table_base	: 32;
	} swreg66;

	struct {
		u32 l0_ref0_chroma_compress_table_base_msb	: 32;
	} swreg67;

	struct {
		u32 l0_ref1_luma_compress_table_base	: 32;
	} swreg68;

	struct {
		u32 l0_ref1_luma_compress_table_base_msb	: 32;
	} swreg69;

	struct {
		u32 l0_ref1_chroma_compress_table_base	: 32;
	} swreg70;

	struct {
		u32 l0_ref1_chroma_compress_table_base_msb	: 32;
	} swreg71;

	struct {
		u32 recon_luma_4n_base	: 32;
	} swreg72;

	struct {
		u32 recon_luma_4n_base_msb	: 32;
	} swreg73;

	struct {
		u32 refpic_recon_l0_4n0_base	: 32;
	} swreg74;

	struct {
		u32 refpic_recon_l0_4n0_base_msb	: 32;
	} swreg75;

	struct {
		u32 refpic_recon_l0_4n1_base	: 32;
	} swreg76;

	struct {
		u32 refpic_recon_l0_4n1_base_msb	: 32;
	} swreg77;

	struct {
		u32 reserved		: 6;
		u32 lambda_satd_me_15	: 13;
		u32 lambda_satd_me_14	: 13;
	} swreg78;

	struct {
		u32 reserved		: 11;
		u32 lambda_sse_me_0	: 21;
	} swreg79;

	struct {
		u32 hwmaxvideowidth	: 13;
		u32 hwbuswidth		: 2;
		u32 hwjpegsupport	: 1;
		u32 hwtu32support	: 1;
		u32 hwrfcsupport	: 1;
		u32 hwprogrdosupport	: 1;
		u32 hwlinebufsupport	: 1;
		u32 hwcavlcsupport	: 1;
		u32 hwbus		: 3;
		u32 hwmain10support	: 1;
		u32 hwdenoisesupport	: 1;
		u32 hwvp9support	: 1;
		u32 hwhevcsupport	: 1;
		u32 hwrgbsupport	: 1;
		u32 hwbframesupport	: 1;
		u32 hwscalingsupport	: 1;
		u32 hwh264support	: 1;
	} swreg80;

	struct {
		u32 timeout_cycles	: 23;
		u32 timeout_override_e	: 1;
		u32 max_burst		: 8;
	} swreg81;

	struct {
		u32 hw_performance	: 32;
	} swreg82;

	struct {
		u32 refpic_recon_l1_y0	: 32;
	} swreg83;

	struct {
		u32 refpic_recon_l1_chroma0	: 32;
	} swreg84;

	struct {
		u32 refpic_recon_l1_y1	: 32;
	} swreg85;

	struct {
		u32 refpic_recon_l1_chroma1	: 32;
	} swreg86;

	struct {
		u32 refpic_recon_l1_y0_msb	: 32;
	} swreg87;

	struct {
		u32 refpic_recon_l1_chroma0_msb	: 32;
	} swreg88;

	struct {
		u32 refpic_recon_l1_y1_msb	: 32;
	} swreg89;

	struct {
		u32 refpic_recon_l1_chroma1_msb	: 32;
	} swreg90;

	struct {
		u32 l1_ref1_chroma_compressor_enable	: 1;
		u32 l1_ref1_luma_compressor_enable	: 1;
		u32 l1_ref0_chroma_compressor_enable	: 1;
		u32 l1_ref0_luma_compressor_enable	: 1;
		u32 long_term_ref_pics_present_flag	: 1;
		u32 reserved				: 1;
		u32 active_l1_cnt			: 2;
		u32 l1_used_by_curr_pic1		: 1;
		u32 l1_long_term_flag1			: 1;
		u32 l1_delta_poc1			: 10;
		u32 l1_used_by_curr_pic0		: 1;
		u32 l1_long_term_flag0			: 1;
		u32 l1_delta_poc0			: 10;
	} swreg91;

	struct {
		u32 refpic_recon_l1_4n0_base	: 32;
	} swreg92;

	struct {
		u32 refpic_recon_l1_4n0_base_msb	: 32;
	} swreg93;

	struct {
		u32 refpic_recon_l1_4n1_base	: 32;
	} swreg94;

	struct {
		u32 refpic_recon_l1_4n1_base_msb	: 32;
	} swreg95;

	struct {
		u32 l1_ref0_luma_compress_table_base	: 32;
	} swreg96;

	struct {
		u32 l1_ref0_luma_compress_table_base_msb	: 32;
	} swreg97;

	struct {
		u32 l1_ref0_chroma_compress_table_base	: 32;
	} swreg98;

	struct {
		u32 l1_ref0_chroma_compress_table_base_msb	: 32;
	} swreg99;

	struct {
		u32 l1_ref1_luma_compress_table_base	: 32;
	} swreg100;

	struct {
		u32 l1_ref1_luma_compress_table_base_msb	: 32;
	} swreg101;

	struct {
		u32 l1_ref1_chroma_compress_table_base	: 32;
	} swreg102;

	struct {
		u32 l1_ref1_chroma_compress_table_base_msb	: 32;
	} swreg103;

	struct {
		u32 ref_pic_list_modi_flag_l0	: 1;
		u32 list_entry_l0_pic0		: 4;
		u32 list_entry_l0_pic1		: 4;
		u32 reserved0			: 7;
		u32 ref_pic_list_modi_flag_l1	: 1;
		u32 list_entry_l1_pic0		: 4;
		u32 list_entry_l1_pic1		: 4;
		u32 reserved1			: 4;
		u32 rdo_level			: 2;
		u32 lists_modi_present_flag	: 1;
	} swreg104;

	struct {
		u32 targetpicsize	: 32;
	} swreg105;

	struct {
		u32 minpicsize	: 32;
	} swreg106;

	struct {
		u32 maxpicsize	: 32;
	} swreg107;

	struct {
		u32 nonzerocount	: 24;
		u32 averageqp		: 8;
	} swreg108;

	struct {
		u32 roimapdeltaqpaddr	: 32;
	} swreg109;

	struct {
		u32 roimapdeltaqpaddr_msb	: 32;
	} swreg110;

	struct {
		u32 reserved	: 12;
		u32 intracu8num	: 20;
	} swreg111;

	struct {
		u32 reserved	: 12;
		u32 skipcu8num	: 20;
	} swreg112;

	struct {
		u32 pbframe4nrdcost	: 32;
	} swreg113;

	struct {
		u32 colctbs_store_base	: 32;
	} swreg114;

	struct {
		u32 colctbs_store_base_msb	: 32;
	} swreg115;

	struct {
		u32 colctbs_load_base	: 32;
	} swreg116;

	struct {
		u32 colctbs_load_base_msb	: 32;
	} swreg117;

	struct {
		u32 ctbrcthrdmax	: 16;
		u32 ctbrcthrdmin	: 16;
	} swreg118;

	struct {
		u32 ctbbitsmax	: 16;
		u32 ctbbitsmin	: 16;
	} swreg119;

	struct {
		u32 totallcubits	: 32;
	} swreg120;

	struct {
		u32 bitsratio	: 32;
	} swreg121;

	union {
		struct {
			u32 reserved		: 11;
			u32 lambda_sse_me_1	: 21;
		} swreg122;

		struct {
			u32 av1_enable_order_hint		: 1;
			u32 av1_allow_update_cdf		: 1;
			u32 av1_interp_filter			: 3;
			u32 av1_switchable_motion_mode		: 1;
			u32 av1_cur_frame_force_integer_mv	: 1;
			u32 av1_enable_dual_filter		: 1;
			u32 av1_enable_interintra_compound	: 1;
			u32 av1_list1_ref_frame			: 4;
			u32 av1_list0_ref_frame			: 4;
			u32 av1_reference_mode			: 2;
			u32 av1_skip_mode_flag			: 1;
			u32 av1_allow_high_precision_mv		: 1;
			u32 av1_seg_enable			: 1;
			u32 av1_reduced_tx_set_used		: 1;
			u32 av1_tx_mode				: 2;
			u32 av1_enable_filter_intra		: 1;
			u32 av1_delta_q_res			: 4;
			u32 av1_coded_lossless			: 1;
			u32 av1_allow_intrabc			: 1;
		} swreg122_av1;
	};

	union {
		struct {
			u32 ctbrc_qpdelta_flag_reverse	: 1;
			u32 reserved			: 10;
			u32 lambda_sse_me_2		: 21;
		} swreg123;

		struct {
			u32 reserved			: 1;
			u32 av1_btxtypesearch		: 1;
			u32 av1_primary_ref_frame	: 3;
			u32 av1_sharpness_lvl		: 3;
			u32 av1_db_filter_lvl_v		: 6;
			u32 av1_db_filter_lvl_u		: 6;
			u32 av1_db_filter_lvl1		: 6;
			u32 av1_db_filter_lvl0		: 6;
		} swreg123_av1;
	};

	union {
		struct {
			u32 reserved		: 11;
			u32 lambda_sse_me_3	: 21;
		} swreg124;

		struct {
			u32 reserved			: 15;
			u32 av1_cdef_bits		: 2;
			u32 av1_cdef_uv_strengths	: 6;
			u32 av1_cdef_strengths		: 6;
			u32 av1_cdef_damping		: 3;
		} swreg124_av1;
	};

	union {
		struct {
			u32 reserved		: 4;
			u32 intra_satd_lambda_1	: 14;
			u32 intra_satd_lambda_0	: 14;
		} swreg125;

		struct {
			u32 av1_framectx_base	: 32;
		} swreg125_av1;
	};

	union {
		struct {
			u32 reserved		: 4;
			u32 intra_satd_lambda_3	: 14;
			u32 intra_satd_lambda_2	: 14;
		} swreg126;

		struct {
			u32 av1_framectx_base_msb	: 32;
		} swreg126_av1;
	};

	struct {
		u32 reserved		: 4;
		u32 intra_satd_lambda_5	: 14;
		u32 intra_satd_lambda_4	: 14;
	} swreg127;

	struct {
		u32 reserved		: 4;
		u32 intra_satd_lambda_7	: 14;
		u32 intra_satd_lambda_6	: 14;
	} swreg128;

	struct {
		u32 reserved		: 4;
		u32 intra_satd_lambda_9	: 14;
		u32 intra_satd_lambda_8	: 14;
	} swreg129;

	struct {
		u32 reserved			: 4;
		u32 intra_satd_lambda_11	: 14;
		u32 intra_satd_lambda_10	: 14;
	} swreg130;

	struct {
		u32 reserved			: 4;
		u32 intra_satd_lambda_13	: 14;
		u32 intra_satd_lambda_12	: 14;
	} swreg131;

	struct {
		u32 reserved			: 4;
		u32 intra_satd_lambda_15	: 14;
		u32 intra_satd_lambda_14	: 14;
	} swreg132;

	struct {
		u32 sse_div_256	: 32;
	} swreg133;

	struct {
		u32 nr_mbnum_invert_reg		: 16;
		u32 reserved			: 8;
		u32 noise_low			: 6;
		u32 noise_reduction_enable	: 2;
	} swreg134;

	struct {
		u32 reserved		: 5;
		u32 thresh_sigma_cur	: 21;
		u32 sliceqp_prev	: 6;
	} swreg135;

	struct {
		u32 frame_sigma_calced	: 16;
		u32 sigma_cur		: 16;
	} swreg136;

	struct {
		u32 reserved		: 11;
		u32 thresh_sigma_calced	: 21;
	} swreg137;

	struct {
		u32 reserved		: 11;
		u32 lambda_sse_me_4	: 21;
	} swreg138;

	struct {
		u32 reserved		: 11;
		u32 lambda_sse_me_5	: 21;
	} swreg139;

	struct {
		u32 reserved		: 11;
		u32 lambda_sse_me_6	: 21;
	} swreg140;

	struct {
		u32 reserved		: 11;
		u32 lambda_sse_me_7	: 21;
	} swreg141;

	struct {
		u32 reserved		: 11;
		u32 lambda_sse_me_8	: 21;
	} swreg142;

	struct {
		u32 reserved		: 11;
		u32 lambda_sse_me_9	: 21;
	} swreg143;

	struct {
		u32 reserved		: 11;
		u32 lambda_sse_me_10	: 21;
	} swreg144;

	struct {
		u32 reserved		: 11;
		u32 lambda_sse_me_11	: 21;
	} swreg145;

	struct {
		u32 reserved		: 11;
		u32 lambda_sse_me_12	: 21;
	} swreg146;

	struct {
		u32 reserved		: 11;
		u32 lambda_sse_me_13	: 21;
	} swreg147;

	struct {
		u32 reserved		: 11;
		u32 lambda_sse_me_14	: 21;
	} swreg148;

	struct {
		u32 reserved		: 11;
		u32 lambda_sse_me_15	: 21;
	} swreg149;

	struct {
		u32 reserved		: 6;
		u32 lambda_satd_me_17	: 13;
		u32 lambda_satd_me_16	: 13;
	} swreg150;

	struct {
		u32 reserved		: 6;
		u32 lambda_satd_me_19	: 13;
		u32 lambda_satd_me_18	: 13;
	} swreg151;

	struct {
		u32 reserved		: 6;
		u32 lambda_satd_me_21	: 13;
		u32 lambda_satd_me_20	: 13;
	} swreg152;

	struct {
		u32 reserved		: 6;
		u32 lambda_satd_me_23	: 13;
		u32 lambda_satd_me_22	: 13;
	} swreg153;

	struct {
		u32 reserved		: 6;
		u32 lambda_satd_me_25	: 13;
		u32 lambda_satd_me_24	: 13;
	} swreg154;

	struct {
		u32 reserved		: 6;
		u32 lambda_satd_me_27	: 13;
		u32 lambda_satd_me_26	: 13;
	} swreg155;

	struct {
		u32 reserved		: 6;
		u32 lambda_satd_me_29	: 13;
		u32 lambda_satd_me_28	: 13;
	} swreg156;

	struct {
		u32 reserved		: 6;
		u32 lambda_satd_me_31	: 13;
		u32 lambda_satd_me_30	: 13;
	} swreg157;

	struct {
		u32 reserved		: 11;
		u32 lambda_sse_me_16	: 21;
	} swreg158;

	struct {
		u32 reserved		: 11;
		u32 lambda_sse_me_17	: 21;
	} swreg159;

	struct {
		u32 reserved		: 11;
		u32 lambda_sse_me_18	: 21;
	} swreg160;

	struct {
		u32 reserved		: 11;
		u32 lambda_sse_me_19	: 21;
	} swreg161;

	struct {
		u32 reserved		: 11;
		u32 lambda_sse_me_20	: 21;
	} swreg162;

	struct {
		u32 reserved		: 11;
		u32 lambda_sse_me_21	: 21;
	} swreg163;

	struct {
		u32 reserved		: 11;
		u32 lambda_sse_me_22	: 21;
	} swreg164;

	struct {
		u32 reserved		: 11;
		u32 lambda_sse_me_23	: 21;
	} swreg165;

	struct {
		u32 reserved		: 11;
		u32 lambda_sse_me_24	: 21;
	} swreg166;

	struct {
		u32 reserved		: 11;
		u32 lambda_sse_me_25	: 21;
	} swreg167;

	struct {
		u32 reserved		: 11;
		u32 lambda_sse_me_26	: 21;
	} swreg168;

	struct {
		u32 reserved		: 11;
		u32 lambda_sse_me_27	: 21;
	} swreg169;

	union {
		struct {
			u32 roi1_delta_qp_rc		: 5;
			u32 rc_ctbrc_sliceqpoffset	: 6;
			u32 lambda_sse_me_28		: 21;
		} swreg170_qp_delta;

		struct {
			u32 roi1_qp_type	: 1;
			u32 roi1_qp_value	: 7;
			u32 sse_qp_factor	: 15;
			u32 reserved		: 8;
			u32 lambda_depth	: 1;
		} swreg170_qp_absolute;
	};

	union {
		struct {
			u32 roi2_delta_qp_rc	: 5;
			u32 reserved		: 6;
			u32 lambda_sse_me_29	: 21;
		} swreg171_qp_delta;

		struct {
			u32 roi2_qp_type	: 1;
			u32 roi2_qp_value	: 7;
			u32 sad_qp_factor	: 15;
			u32 reserved		: 9;
		} swreg171_qp_absolute;
	};

	struct {
		u32 complexity_offset	: 5;
		u32 qp_min		: 6;
		u32 lambda_sse_me_30	: 21;
	} swreg172;

	struct {
		u32 rc_qpdelta_range	: 4;
		u32 reserved		: 1;
		u32 qp_max		: 6;
		u32 lambda_sse_me_31	: 21;
	} swreg173;

	struct {
		u32 reserved			: 4;
		u32 intra_satd_lambda_17	: 14;
		u32 intra_satd_lambda_16	: 14;
	} swreg174;

	struct {
		u32 reserved			: 4;
		u32 intra_satd_lambda_19	: 14;
		u32 intra_satd_lambda_18	: 14;
	} swreg175;

	struct {
		u32 reserved			: 4;
		u32 intra_satd_lambda_21	: 14;
		u32 intra_satd_lambda_20	: 14;
	} swreg176;

	struct {
		u32 reserved			: 4;
		u32 intra_satd_lambda_23	: 14;
		u32 intra_satd_lambda_22	: 14;
	} swreg177;

	struct {
		u32 reserved			: 4;
		u32 intra_satd_lambda_25	: 14;
		u32 intra_satd_lambda_24	: 14;
	} swreg178;

	struct {
		u32 reserved			: 4;
		u32 intra_satd_lambda_27	: 14;
		u32 intra_satd_lambda_26	: 14;
	} swreg179;

	struct {
		u32 reserved			: 4;
		u32 intra_satd_lambda_29	: 14;
		u32 intra_satd_lambda_28	: 14;
	} swreg180;

	struct {
		u32 reserved			: 2;
		u32 rc_block_size		: 2;
		u32 intra_satd_lambda_31	: 14;
		u32 intra_satd_lambda_30	: 14;
	} swreg181;

	struct {
		u32 qp_delta_gain	: 16;
		u32 qp_fractional	: 16;
	} swreg182;

	struct {
		u32 reserved	: 6;
		u32 qp_sum	: 26;
	} swreg183;

	struct {
		u32 reserved	: 12;
		u32 qp_num	: 20;
	} swreg184;

	struct {
		u32 timeout_cycles_msb	: 9;
		u32 pic_complexity	: 23;
	} swreg185;

	struct {
		u32 cu_information_table_base	: 32;
	} swreg186;

	struct {
		u32 cu_information_table_base_msb	: 32;
	} swreg187;

	struct {
		u32 cu_information_base	: 32;
	} swreg188;

	struct {
		u32 cu_information_base_msb	: 32;
	} swreg189;

	struct {
		u32 reserved		: 30;
		u32 num_long_term_pics	: 2;
	} swreg190;

	struct {
		u32 slice_header_size	: 16;
		u32 prefixnal_svc_ext	: 1;
		u32 pps_id		: 6;
		u32 nuh_temporal_id	: 3;
		u32 nal_unit_type	: 6;
	} swreg191;

	struct {
		u32 framenum	: 32;
	} swreg192;

	struct {
		u32 entropy_coding_mode		: 1;
		u32 transform8x8_enable		: 1;
		u32 idr_pic_id			: 1;
		u32 nal_ref_idc			: 1;
		u32 yfill_msb			: 2;
		u32 xfill_msb			: 2;
		u32 l0_used_by_next_pic1	: 1;
		u32 l0_delta_framenum1		: 11;
		u32 l0_used_by_next_pic0	: 1;
		u32 l0_delta_framenum0		: 11;
	} swreg193;

	struct {
		u32 reserved			: 2;
		u32 cur_longtermidx		: 3;
		u32 max_longtermidx_plus1	: 3;
		u32 l1_used_by_next_pic1	: 1;
		u32 l1_delta_framenum1		: 11;
		u32 l1_used_by_next_pic0	: 1;
		u32 l1_delta_framenum0		: 11;
	} swreg194;

	struct {
		u32 reserved			: 2;
		u32 pic_width_msb		: 2;
		u32 roi2_bottom_msb		: 1;
		u32 roi2_top_msb		: 1;
		u32 roi2_right_msb		: 1;
		u32 roi2_left_msb		: 1;
		u32 roi1_bottom_msb		: 1;
		u32 roi1_top_msb		: 1;
		u32 roi1_right_msb		: 1;
		u32 roi1_left_msb		: 1;
		u32 intra_area_bottom_msb	: 1;
		u32 intra_area_top_msb		: 1;
		u32 intra_area_right_msb	: 1;
		u32 intra_area_left_msb		: 1;
		u32 cir_interval_msb		: 4;
		u32 cir_start_msb		: 4;
		u32 slice_size_msb		: 2;
		u32 num_slices_ready_msb	: 2;
		u32 encoded_ctb_number_msb	: 4;
	} swreg195;

	struct {
		u32 ctb_row_wr_ptr		: 10;
		u32 ctb_row_rd_ptr		: 10;
		u32 num_ctb_rows_per_sync	: 9;
		u32 input_buf_loopback_en	: 1;
		u32 low_latency_en		: 1;
		u32 low_latency_hw_sync_en	: 1;
	} swreg196;

	union {
		struct {
			u32 reserved		: 2;
			u32 l1_delta_poc0_msb	: 10;
			u32 l0_delta_poc1_msb	: 10;
			u32 l0_delta_poc0_msb	: 10;
		} swreg197;

		struct {
			u32 ctb_row_rd_ptr_jpeg_msb	: 5;
			u32 ctb_row_wr_ptr_jpeg_msb	: 5;
			u32 reserved			: 22;
		} swreg197_jpeg;
	};

	struct {
		u32 l1_longtermidx1		: 3;
		u32 l1_longtermidx0		: 3;
		u32 l0_longtermidx1		: 3;
		u32 l0_longtermidx0		: 3;
		u32 mark_current_longterm	: 1;
		u32 l0_delta_framenum0_msb	: 9;
		u32 l1_delta_poc1_msb		: 10;
	} swreg198;

	struct {
		u32 osd_alphablend_enable	: 1;
		u32 hash_offset			: 2;
		u32 hash_type			: 2;
		u32 l1_delta_framenum1_msb	: 9;
		u32 l1_delta_framenum0_msb	: 9;
		u32 l0_delta_framenum1_msb	: 9;
	} swreg199;

	struct {
		u32 hash_val	: 32;
	} swreg200;

	struct {
		u32 mean_thr3	: 8;
		u32 mean_thr2	: 8;
		u32 mean_thr1	: 8;
		u32 mean_thr0	: 8;
	} swreg201;

	struct {
		u32 thr_dc_chroma_8x8	: 16;
		u32 thr_dc_lum_8x8	: 16;
	} swreg202;

	union {
		struct {
			u32 cr_dc_sum_thr	: 8;
			u32 cb_dc_sum_thr	: 8;
			u32 reserved		: 8;
			u32 lum_dc_sum_thr	: 8;
		} swreg203_h264;

		struct {
			u32 thr_dc_chroma_16x16	: 16;
			u32 thr_dc_lum_16x16	: 16;
		} swreg203_hevc;
	};

	struct {
		u32 thr_dc_chroma_32x32	: 16;
		u32 thr_dc_lum_32x32	: 16;
	} swreg204;

	struct {
		u32 thr_ac_num_chroma_8x8	: 16;
		u32 thr_ac_num_lum_8x8		: 16;
	} swreg205;

	struct {
		u32 thr_ac_num_chroma_16x16	: 16;
		u32 thr_ac_num_lum_16x16	: 16;
	} swreg206;

	struct {
		u32 thr_ac_num_chroma_32x32	: 16;
		u32 thr_ac_num_lum_32x32	: 16;
	} swreg207;

	union {
		struct {
			u32 reserved0			: 3;
			u32 skip_map_enable		: 1;
			u32 ipcm1_left			: 9;
			u32 enable_smart		: 1;
			u32 foreground_pixel_thx	: 6;
			u32 reserved1			: 6;
			u32 smart_qp			: 6;
		} swreg208_h264;

		struct {
			u32 reserved			: 3;
			u32 skip_map_enable		: 1;
			u32 ipcm1_left			: 9;
			u32 enable_smart		: 1;
			u32 foreground_pixel_thx	: 6;
			u32 mdqpc			: 6;
			u32 mdqpy			: 6;
		} swreg208_hevc;
	};

	struct {
		u32 reserved		: 3;
		u32 ipcm_map_enable	: 1;
		u32 pcm_filter_disable	: 1;
		u32 ipcm1_bottom	: 9;
		u32 ipcm1_top		: 9;
		u32 ipcm1_right		: 9;
	} swreg209;

	struct {
		u32 reserved		: 3;
		u32 ipcm2_left		: 9;
		u32 input_lu_stride	: 20;
	} swreg210;

	struct {
		u32 reserved		: 3;
		u32 ipcm2_right		: 9;
		u32 input_ch_stride	: 20;
	} swreg211;

	struct {
		u32 reserved		: 3;
		u32 ipcm2_top		: 9;
		u32 ref_lu_stride	: 20;
	} swreg212;

	struct {
		u32 reserved		: 5;
		u32 ipcm2_bottom	: 9;
		u32 ref_ds_lu_stride	: 18;
	} swreg213;

	struct {
		u32 hwmaxvideowidthjpeg	: 13;
		u32 hwmaxvideowidthh264	: 13;
		u32 hwroimapversion	: 3;
		u32 hwintratu32support	: 1;
		u32 hwabsqpsupport	: 1;
		u32 hwljpegsupport	: 1;
	} swreg214;

	struct {
		u32 totalarlen	: 32;
	} swreg215;

	struct {
		u32 totalr	: 32;
	} swreg216;

	struct {
		u32 totalar	: 32;
	} swreg217;

	struct {
		u32 totalrlast	: 32;
	} swreg218;

	struct {
		u32 totalawlen	: 32;
	} swreg219;

	struct {
		u32 totalw	: 32;
	} swreg220;

	struct {
		u32 totalaw	: 32;
	} swreg221;

	struct {
		u32 totalwlast	: 32;
	} swreg222;

	struct {
		u32 totalb	: 32;
	} swreg223;

	struct {
		u32 cb_const_pixel	: 10;
		u32 cr_const_pixel	: 10;
		u32 skipframe_en	: 1;
		u32 ssim_en		: 1;
		u32 reserved		: 9;
		u32 chroma_const_en	: 1;
	} swreg224;

	struct {
		u32 reserved					: 6;
		u32 roimap_qpdelta_ver				: 3;
		u32 roimap_cuctrl_ver				: 3;
		u32 roimap_cuctrl_enable			: 1;
		u32 roimap_cuctrl_index_enable			: 1;
		u32 loop_filter_across_tiles_enabled_flag	: 1;
		u32 tiles_enabled_flag				: 1;
		u32 num_tile_rows				: 8;
		u32 num_tile_columns				: 8;
	} swreg225;

	struct {
		u32 hwdynamicmaxtusize		: 1;
		u32 hwiframeonly		: 1;
		u32 hwstreamsegmentsupport	: 1;
		u32 hwstreambufchain		: 1;
		u32 hwinloopdsratio		: 1;
		u32 hwmultipasssupport		: 1;
		u32 hwrdoqsupport		: 1;
		u32 bframe_me4n_hor_searchrange	: 2;
		u32 hwroi8support		: 1;
		u32 hwgmvsupport		: 1;
		u32 hwjpeg422support		: 1;
		u32 hwctbrcversion		: 3;
		u32 me_vert_searchrange_h264	: 6;
		u32 me_vert_searchrange_hevc	: 6;
		u32 hwcuinforversion		: 3;
		u32 hwp010refsupport		: 1;
		u32 hwssimsupport		: 1;
	} swreg226;

	struct {
		u32 ssim_y_numerator_lsb	: 32;
	} swreg227;

	struct {
		u32 ssim_y_numerator_msb	: 32;
	} swreg228;

	struct {
		u32 ssim_u_numerator_lsb	: 32;
	} swreg229;

	struct {
		u32 ssim_u_numerator_msb	: 32;
	} swreg230;

	struct {
		u32 ssim_v_numerator_lsb	: 32;
	} swreg231;

	struct {
		u32 ssim_v_numerator_msb	: 32;
	} swreg232;

	struct {
		u32 ssim_y_denominator	: 32;
	} swreg233;

	struct {
		u32 ssim_uv_denominator	: 32;
	} swreg234;

	struct {
		u32 rps_used_by_cur_1	: 1;
		u32 rps_used_by_cur_0	: 1;
		u32 rps_delta_poc_2	: 10;
		u32 rps_delta_poc_1	: 10;
		u32 rps_delta_poc_0	: 10;
	} swreg235;

	struct {
		u32 reserved				: 12;
		u32 p010_ref_enable			: 1;
		u32 short_term_ref_pic_set_sps_flag	: 1;
		u32 rps_pos_pic_num			: 3;
		u32 rps_neg_pic_num			: 3;
		u32 rps_used_by_cur_3			: 1;
		u32 rps_used_by_cur_2			: 1;
		u32 rps_delta_poc_3			: 10;
	} swreg236;

	struct {
		u32 reserved		: 11;
		u32 dummyreaden		: 1;
		u32 ref_ch_stride	: 20;
	} swreg237;

	struct {
		u32 dummyreadaddr	: 32;
	} swreg238;

	struct {
		u32 current_ctb_mad_base	: 32;
	} swreg239;

	struct {
		u32 current_ctb_mad_base_msb	: 32;
	} swreg240;

	struct {
		u32 previous_ctb_mad_base	: 32;
	} swreg241;

	struct {
		u32 previous_ctb_mad_base_msb	: 32;
	} swreg242;

	struct {
		u32 reserved		: 11;
		u32 ctb_rc_model_param0	: 21;
	} swreg243;

	struct {
		u32 reserved		: 2;
		u32 roi3_qp_type	: 1;
		u32 roi3_qp_value	: 7;
		u32 ctb_rc_model_param1	: 22;
	} swreg244;

	struct {
		u32 rc_qpdelta_range_msb	: 2;
		u32 ctb_rc_row_factor		: 16;
		u32 ctb_rc_model_param_min	: 14;
	} swreg245;

	struct {
		u32 reserved			: 3;
		u32 ctb_rc_delay		: 3;
		u32 axi_write_outstanding_num	: 8;
		u32 ctb_rc_qp_step		: 18;
	} swreg246;

	struct {
		u32 reserved0			: 1;
		u32 ctb_rc_prev_mad_valid	: 1;
		u32 reserved1			: 4;
		u32 prev_pic_lum_mad		: 26;
	} swreg247;

	struct {
		u32 roi4_qp_type	: 1;
		u32 roi4_qp_value	: 7;
		u32 ctb_qp_sum_for_rc	: 24;
	} swreg248;

	union {
		struct {
			u32 reserved			: 3;
			u32 ipcm2_bottom_msb		: 1;
			u32 ipcm2_top_msb		: 1;
			u32 ipcm2_right_msb		: 1;
			u32 ipcm2_left_msb		: 1;
			u32 ipcm1_bottom_msb		: 1;
			u32 ipcm1_top_msb		: 1;
			u32 ipcm1_right_msb		: 1;
			u32 ipcm1_left_msb		: 1;
			u32 pic_width_msb2		: 1;
			u32 roi2_bottom_msb2		: 1;
			u32 roi2_top_msb2		: 1;
			u32 roi2_right_msb2		: 1;
			u32 roi2_left_msb2		: 1;
			u32 roi1_bottom_msb2		: 1;
			u32 roi1_top_msb2		: 1;
			u32 roi1_right_msb2		: 1;
			u32 roi1_left_msb2		: 1;
			u32 intra_area_bottom_msb2	: 1;
			u32 intra_area_top_msb2		: 1;
			u32 intra_area_right_msb2	: 1;
			u32 intra_area_left_msb2	: 1;
			u32 cir_interval_msb2		: 2;
			u32 cir_start_msb2		: 2;
			u32 slice_size_msb2		: 1;
			u32 num_slices_ready_msb2	: 1;
			u32 encoded_ctb_number_msb2	: 2;
		} swreg249;

		struct {
			u32 reserved0		: 5;
			u32 jpeg_rowlength_msb	: 2;
			u32 jpeg_pic_height_msb	: 2;
			u32 jpeg_pic_width_msb	: 2;
			u32 reserved1		: 21;
		} swreg249_jpeg;
	};

	struct {
		u32 reserved			: 4;
		u32 global_vertical_mv_l0	: 14;
		u32 global_horizontal_mv_l0	: 14;
	} swreg250;

	struct {
		u32 reserved			: 4;
		u32 global_vertical_mv_l1	: 14;
		u32 global_horizontal_mv_l1	: 14;
	} swreg251;

	struct {
		u32 reserved	: 2;
		u32 roi3_right	: 10;
		u32 roi3_top	: 10;
		u32 roi3_left	: 10;
	} swreg252;

	struct {
		u32 reserved	: 2;
		u32 roi4_top	: 10;
		u32 roi4_left	: 10;
		u32 roi3_bottom	: 10;
	} swreg253;

	struct {
		u32 reserved	: 2;
		u32 roi5_left	: 10;
		u32 roi4_bottom	: 10;
		u32 roi4_right	: 10;
	} swreg254;

	struct {
		u32 reserved	: 2;
		u32 roi5_bottom	: 10;
		u32 roi5_right	: 10;
		u32 roi5_top	: 10;
	} swreg255;

	struct {
		u32 reserved	: 2;
		u32 roi6_right	: 10;
		u32 roi6_top	: 10;
		u32 roi6_left	: 10;
	} swreg256;

	struct {
		u32 reserved	: 2;
		u32 roi7_top	: 10;
		u32 roi7_left	: 10;
		u32 roi6_bottom	: 10;
	} swreg257;

	struct {
		u32 reserved	: 2;
		u32 roi8_left	: 10;
		u32 roi7_bottom	: 10;
		u32 roi7_right	: 10;
	} swreg258;

	struct {
		u32 reserved				: 1;
		u32 current_max_tu_size_decrease	: 1;
		u32 roi8_bottom				: 10;
		u32 roi8_right				: 10;
		u32 roi8_top				: 10;
	} swreg259;

	struct {
		u32 roi5_qp_type	: 1;
		u32 roi5_qp_value	: 7;
		u32 roi6_qp_type	: 1;
		u32 roi6_qp_value	: 7;
		u32 roi7_qp_type	: 1;
		u32 roi7_qp_value	: 7;
		u32 roi8_qp_type	: 1;
		u32 roi8_qp_value	: 7;
	} swreg260;

	struct {
		u32 motion_score_enable		: 1;
		u32 pass1_skip_cabac		: 1;
		u32 rdoq_enable			: 1;
		u32 multi_core_en		: 1;
		u32 axi_read_outstanding_num	: 8;
		u32 prp_in_loop_ds_ratio	: 1;
		u32 rgblumaoffset		: 5;
		u32 reserved			: 14;
	} swreg261;

	struct {
		u32 lum_sse_div_256	: 32;
	} swreg262;

	struct {
		u32 cb_sse_div_64	: 32;
	} swreg263;

	struct {
		u32 cr_sse_div_64	: 32;
	} swreg264;

	struct {
		u32 ddr_polling_interval	: 16;
		u32 ref_ready_threshold		: 16;
	} swreg265;

	struct {
		u32 multicore_sync_l0_addr	: 32;
	} swreg266;

	struct {
		u32 multicore_sync_l0_addr_msb	: 32;
	} swreg267;

	struct {
		u32 multicore_sync_l1_addr	: 32;
	} swreg268;

	struct {
		u32 multicore_sync_l1_addr_msb	: 32;
	} swreg269;

	struct {
		u32 multicore_sync_rec_addr	: 32;
	} swreg270;

	struct {
		u32 multicore_sync_rec_addr_msb	: 32;
	} swreg271;

	struct {
		u32 wr_urgent_disable_threshold	: 8;
		u32 wr_urgent_enable_threshold	: 8;
		u32 rd_urgent_disable_threshold	: 8;
		u32 rd_urgent_enable_threshold	: 8;
	} swreg272;

	struct {
		u32 roimap_cuctrl_index_addr	: 32;
	} swreg273;

	struct {
		u32 roimap_cuctrl_index_addr_msb	: 32;
	} swreg274;

	struct {
		u32 roimap_cuctrl_addr	: 32;
	} swreg275;

	struct {
		u32 roimap_cuctrl_addr_msb	: 32;
	} swreg276;

	struct {
		u32 reserved			: 5;
		u32 syn_amount_per_loopback	: 15;
		u32 pic_order_cnt_type		: 2;
		u32 log2_max_frame_num		: 5;
		u32 log2_max_pic_order_cnt_lsb	: 5;
	} swreg277;

	struct {
		u32 output_strm_buf1_base	: 32;
	} swreg278;

	struct {
		u32 output_strm_buf1_base_msb	: 32;
	} swreg279;

	struct {
		u32 output_strm_buffer1_limit	: 32;
	} swreg280;

	struct {
		u32 reserved			: 2;
		u32 chroma_format_idc		: 2;
		u32 num_ctb_rows_per_sync_msb	: 6;
		u32 strm_segment_wr_ptr		: 10;
		u32 strm_segment_rd_ptr		: 10;
		u32 strm_segment_en		: 1;
		u32 strm_segment_sw_sync_en	: 1;
	} swreg281;

	struct {
		u32 strm_segment_size	: 32;
	} swreg282;

	struct {
		u32 motion_score_l0_0	: 32;
	} swreg283;

	struct {
		u32 motion_score_l0_1	: 32;
	} swreg284;

	struct {
		u32 motion_score_l1_0	: 32;
	} swreg285;

	struct {
		u32 motion_score_l1_1	: 32;
	} swreg286;

	struct {
		u32 reserved0			: 21;
		u32 hwmonochromesupport		: 1;
		u32 hwmevertrangeprogramable	: 1;
		u32 hwctbrcmoremode		: 1;
		u32 reserved1			: 4;
		u32 hwcutreesupport		: 1;
		u32 hwscaler420support		: 1;
		u32 hwcscextensionsupport	: 1;
		u32 hwvideoheightext		: 1;
	} swreg287;

	struct {
		u32 reserved			: 12;
		u32 cuinfoversion		: 3;
		u32 ctb_qp_sum_for_rc_msb	: 2;
		u32 pic_complexity_msb		: 4;
		u32 qp_num_msb			: 3;
		u32 qp_sum_msb			: 2;
		u32 skipcu8num_msb		: 3;
		u32 intracu8num_msb		: 3;
	} swreg288;

	struct {
		u32 rgbcoeffh	: 16;
		u32 rgbcoeffg	: 16;
	} swreg289;

	struct {
		u32 totalarlen2	: 32;
	} swreg290;

	struct {
		u32 totalr2	: 32;
	} swreg291;

	struct {
		u32 totalar2	: 32;
	} swreg292;

	struct {
		u32 totalrlast2	: 32;
	} swreg293;

	struct {
		u32 totalawlen2	: 32;
	} swreg294;

	struct {
		u32 totalw2	: 32;
	} swreg295;

	struct {
		u32 totalaw2	: 32;
	} swreg296;

	struct {
		u32 totalwlast2	: 32;
	} swreg297;

	struct {
		u32 totalb2	: 32;
	} swreg298;

	struct {
		u32 ext_sram_lum_fwd_base	: 32;
	} swreg299;

	struct {
		u32 ext_sram_lum_fwd_base_msb	: 32;
	} swreg300;

	struct {
		u32 ext_sram_lum_bwd_base	: 32;
	} swreg301;

	struct {
		u32 ext_sram_lum_bwd_base_msb	: 32;
	} swreg302;

	struct {
		u32 ext_sram_chr_fwd_base	: 32;
	} swreg303;

	struct {
		u32 ext_sram_chr_fwd_base_msb	: 32;
	} swreg304;

	struct {
		u32 ext_sram_chr_bwd_base	: 32;
	} swreg305;

	struct {
		u32 ext_sram_chr_bwd_base_msb	: 32;
	} swreg306;

	struct {
		u32 extlinebuffer_linecnt_chr_bwd	: 8;
		u32 extlinebuffer_linecnt_chr_fwd	: 8;
		u32 extlinebuffer_linecnt_lum_bwd	: 8;
		u32 extlinebuffer_linecnt_lum_fwd	: 8;
	} swreg307;

	struct {
		u32 axi_strm_write_pending	: 32;
	} swreg308;

	struct {
		u32 axi_recon_write_pending	: 32;
	} swreg309;

	struct {
		u32 axi_rec4n_write_pending	: 32;
	} swreg310;

	struct {
		u32 axi_prp_read_pending	: 32;
	} swreg311;

	struct {
		u32 axi_ref_read_pending	: 32;
	} swreg312;

	struct {
		u32 axi_ref4n_read_pending	: 32;
	} swreg313;

	struct {
		u32 axi_rcroi_read_pending	: 32;
	} swreg314;

	struct {
		u32 axi_read_channel_pending	: 32;
	} swreg315;

	struct {
		u32 axi_write_channel_pending	: 32;
	} swreg316;

	struct {
		u32 axi_total_pending	: 32;
	} swreg317;

	struct {
		u32 hw_debug	: 32;
	} swreg318;

	struct {
		u32 axi_burst_align_fuse_rd_lu_ref_prefetch	: 4;
		u32 axi_burst_align_fuse_rd_ch_ref_prefetch	: 4;
		u32 axi_burst_align_fuse_rd_prp			: 4;
		u32 axi_burst_align_fuse_rd_common		: 4;
		u32 axi_burst_align_fuse_wr_luma_ref		: 4;
		u32 axi_burst_align_fuse_wr_chroma_ref		: 4;
		u32 axi_burst_align_fuse_wr_stream		: 4;
		u32 axi_burst_align_fuse_wr_common		: 4;
	} swreg319;

	struct {
		u32 axi_burst_align_rd_lu_ref_prefetch	: 4;
		u32 axi_burst_align_rd_ch_ref_prefetch	: 4;
		u32 axi_burst_align_rd_prp		: 4;
		u32 axi_burst_align_rd_common		: 4;
		u32 axi_burst_align_wr_luma_ref		: 4;
		u32 axi_burst_align_wr_chroma_ref	: 4;
		u32 axi_burst_align_wr_stream		: 4;
		u32 axi_burst_align_wr_common		: 4;
	} swreg320;

	struct {
		u32 reserved				: 26;
		u32 me_assigned_vert_search_range	: 6;
	} swreg321;
} __packed;

#endif /* HANTRO_VC8000E_REGS_H_ */
