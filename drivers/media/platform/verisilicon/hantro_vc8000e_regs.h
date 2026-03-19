/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2024, Pengutronix
 *
 * Author: Marco Felsch <kernel@pengutronxi.de>
 *
 * Based on hantro_g2_regs.h
 */

#ifndef HANTRO_VC8000E_REGS_H_
#define HANTRO_VC8000E_REGS_H_

#define VC8000E_SWREG(nr)	((nr) * 4)

#define VC8000E_REG(b, s, m) \
	((const struct hantro_reg) { \
		.base = VC8000E_SWREG(b), \
		.shift = s, \
		.mask = m, \
	})

#define vc8000e_product_id                                  VC8000E_REG(0, 16, 0xffff)
#define vc8000e_major_number                                VC8000E_REG(0,  8, 0xff)
#define vc8000e_minor_number                                VC8000E_REG(0,  0, 0xff)

#define vc8000e_irq_type_sw_reset                           VC8000E_REG(1, 24, 0x1)
#define vc8000e_irq_type_fuse_error                         VC8000E_REG(1, 23, 0x1)
#define vc8000e_irq_type_buffer_full                        VC8000E_REG(1, 22, 0x1)
#define vc8000e_irq_type_bus_error                          VC8000E_REG(1, 21, 0x1)
#define vc8000e_irq_type_timeout                            VC8000E_REG(1, 20, 0x1)
#define vc8000e_irq_type_strm_segment                       VC8000E_REG(1, 19, 0x1)
#define vc8000e_irq_type_line_buffer                        VC8000E_REG(1, 18, 0x1)
#define vc8000e_irq_type_slice_rdy                          VC8000E_REG(1, 17, 0x1)
#define vc8000e_irq_type_frame_rdy                          VC8000E_REG(1, 16, 0x1)
#define vc8000e_strm_segment_rdy_int                        VC8000E_REG(1, 12, 0x1)
#define vc8000e_timeout_int                                 VC8000E_REG(1, 11, 0x1)
#define vc8000e_irq_fuse_error                              VC8000E_REG(1,  9, 0x1)
#define vc8000e_slice_rdy_status                            VC8000E_REG(1,  8, 0x1)
#define vc8000e_irq_line_buffer                             VC8000E_REG(1,  7, 0x1)
#define vc8000e_timeout                                     VC8000E_REG(1,  6, 0x1)
#define vc8000e_buffer_full                                 VC8000E_REG(1,  5, 0x1)
#define vc8000e_sw_reset                                    VC8000E_REG(1,  4, 0x1)
#define vc8000e_bus_error_status                            VC8000E_REG(1,  3, 0x1)
#define vc8000e_frame_rdy_status                            VC8000E_REG(1,  2, 0x1)
#define vc8000e_irq_dis                                     VC8000E_REG(1,  1, 0x1)
#define vc8000e_irq                                         VC8000E_REG(1,  0, 0x1)

#define vc8000e_axi_write_id                                VC8000E_REG(2, 24, 0xff)
#define vc8000e_axi_read_id                                 VC8000E_REG(2, 16, 0xff)
#define vc8000e_strm_swap                                   VC8000E_REG(2, 12, 0xf)
#define vc8000e_pic_swap                                    VC8000E_REG(2,  8, 0xf)
#define vc8000e_roi_map_qp_delta_map_swap                   VC8000E_REG(2,  4, 0xf)
#define vc8000e_ctb_rc_mem_out_swap                         VC8000E_REG(2,  0, 0xf)

#define vc8000e_clock_gate_encoder_e                        VC8000E_REG(3, 31, 0x1)
#define vc8000e_clock_gate_encoder_h265_e                   VC8000E_REG(3, 30, 0x1)
#define vc8000e_clock_gate_encoder_h264_e                   VC8000E_REG(3, 29, 0x1)
#define vc8000e_clock_gate_inter_e                          VC8000E_REG(3, 28, 0x1)
#define vc8000e_clock_gate_inter_h265_e                     VC8000E_REG(3, 27, 0x1)
#define vc8000e_clock_gate_inter_h264_e                     VC8000E_REG(3, 26, 0x1)
#define vc8000e_axi_wr_id_e                                 VC8000E_REG(3, 25, 0x1)
#define vc8000e_axi_rd_id_e                                 VC8000E_REG(3, 24, 0x1)
#define vc8000e_cu_info_mem_out_swap                        VC8000E_REG(3, 20, 0xf)
#define vc8000e_sram_power_down_disable                     VC8000E_REG(3, 19, 0x1)
#define vc8000e_axi_ar_qos                                  VC8000E_REG(3,  8, 0xf)
#define vc8000e_axi_aw_qos                                  VC8000E_REG(3,  4, 0xf)
#define vc8000e_slice_int                                   VC8000E_REG(3,  3, 0x1)
#define vc8000e_line_buffer_int                             VC8000E_REG(3,  2, 0x1)
#define vc8000e_strm_segment_int                            VC8000E_REG(3,  1, 0x1)

#define vc8000e_mode                                        VC8000E_REG(4, 29, 0x7)
#define     VC8000E_ENC_MODE_H264                           0x02
#define vc8000e_min_cb_size                                 VC8000E_REG(4, 25, 0x3)
#define vc8000e_max_cb_size                                 VC8000E_REG(4, 23, 0x3)
#define vc8000e_min_trb_size                                VC8000E_REG(4, 21, 0x3)
#define vc8000e_max_trb_size                                VC8000E_REG(4, 19, 0x3)
#define vc8000e_output_strm_mode                            VC8000E_REG(4, 18, 0x1)
#define vc8000e_chroma_qp_offset                            VC8000E_REG(4, 13, 0x1f)
#define vc8000e_strong_intra_smoothing_enabled_flag         VC8000E_REG(4, 12, 0x1)
#define vc8000e_bw_linebuf_disable                          VC8000E_REG(4, 11, 0x1)
#define vc8000e_scaling_list_enabled_flag                   VC8000E_REG(4,  8, 0x1)
#define vc8000e_active_override_flag                        VC8000E_REG(4,  7, 0x1)
#define vc8000e_sao_enable                                  VC8000E_REG(4,  6, 0x1)
#define vc8000e_max_trans_hierarchy_depth_intra             VC8000E_REG(4,  3, 0x7)
#define vc8000e_max_trans_hierarchy_depth_inter             VC8000E_REG(4,  0, 0x7)

#define vc8000e_jpeg_pic_width                              VC8000E_REG(5, 20, 0xfff)
#define vc8000e_jpeg_pic_height                             VC8000E_REG(5,  8, 0xfff)
#define vc8000e_pic_width                                   VC8000E_REG(5, 22, 0x3ff)
#define vc8000e_pic_height                                  VC8000E_REG(5, 11, 0x7ff)
#define vc8000e_pps_deblocking_filter_override_enabled_flag VC8000E_REG(5,  9, 0x1)
#define vc8000e_slice_deblocking_filter_override_flag       VC8000E_REG(5,  8, 0x1)
#define vc8000e_output_cu_info_enabled                      VC8000E_REG(5,  6, 0x1)
#define vc8000e_buffer_full_continue                        VC8000E_REG(5,  5, 0x1)
#define vc8000e_ref_frames                                  VC8000E_REG(5,  3, 0x3)
#define vc8000e_frame_coding_type                           VC8000E_REG(5,  1, 0x3)
#define     VC8000E_IFRAME_CODING_TYPE                      0x1
#define     VC8000E_PFRAME_CODING_TYPE                      0x0
#define vc8000e_e                                           VC8000E_REG(5,  0, 0x1)

#define vc8000e_slice_size                                  VC8000E_REG(6, 25, 0x7f)
#define vc8000e_num_short_term_ref_pic_sets                 VC8000E_REG(6, 22, 0x7)
#define vc8000e_num_short_term_ref_pic_sets_v2              VC8000E_REG(6, 20, 0x1f)
#define vc8000e_num_negative_pics                           VC8000E_REG(6, 18, 0x3)
#define vc8000e_num_positive_pics                           VC8000E_REG(6, 16, 0x3)
#define vc8000e_deblocking_filter_ctrl                      VC8000E_REG(6, 15, 0x1)
#define vc8000e_deblocking_tc_offset                        VC8000E_REG(6, 11, 0xf)
#define vc8000e_deblocking_beta_offset                      VC8000E_REG(6,  7, 0xf)
#define vc8000e_rps_id                                      VC8000E_REG(6,  2, 0x7)
#define vc8000e_rps_id_v2                                   VC8000E_REG(6,  2, 0x1f)
#define vc8000e_nal_size_write                              VC8000E_REG(6,  1, 0x1)
#define vc8000e_cu_qp_delta_enabled                         VC8000E_REG(6,  0, 0x1)

#define vc8000e_pic_init_qp                                 VC8000E_REG(7, 26, 0x3f)
#define vc8000e_cabac_init_flag                             VC8000E_REG(7, 25, 0x1)
#define vc8000e_num_slices_ready                            VC8000E_REG(7, 17, 0xff)
#define vc8000e_diff_cu_qp_delta_depth                      VC8000E_REG(7, 14, 0x3)
#define vc8000e_pic_qp                                      VC8000E_REG(7,  8, 0x3f)
#define vc8000e_roi1_delta_qp                               VC8000E_REG(7,  4, 0xf)
#define vc8000e_roi2_delta_qp                               VC8000E_REG(7,  0, 0xf)

#define vc8000e_output_strm_base                            VC8000E_REG(8,  0, 0xffffffff)

#define vc8000e_output_strm_buffer_limit                    VC8000E_REG(9,  0, 0xffffffff)

#define vc8000e_size_tbl_base                               VC8000E_REG(10,  0, 0xffffffff)

#define vc8000e_poc                                         VC8000E_REG(11,  0, 0xffffffff)

#define vc8000e_input_y_base                                VC8000E_REG(12,  0, 0xffffffff)

#define vc8000e_input_cb_base                               VC8000E_REG(13,  0, 0xffffffff)

#define vc8000e_input_cr_base                               VC8000E_REG(14,  0, 0xffffffff)

#define vc8000e_recon_y_base                                VC8000E_REG(15,  0, 0xffffffff)
#define vc8000e_ref_ringbuf_luma_wr_offset                  VC8000E_REG(15,  0, 0xffffffff)

#define vc8000e_recon_chroma_base                           VC8000E_REG(16,  0, 0xffffffff)
#define vc8000e_ref_ringbuf_chroma_wr_offset                VC8000E_REG(16,  0, 0xffffffff)

#define vc8000e_l0_delta_poc0                               VC8000E_REG(17, 22, 0x3ff)
#define vc8000e_l0_long_term_flag0                          VC8000E_REG(17, 21, 0x1)
#define vc8000e_l0_used_by_curr_pic0                        VC8000E_REG(17, 20, 0x1)
#define vc8000e_l0_delta_poc1                               VC8000E_REG(17, 10, 0x3ff)
#define vc8000e_l0_long_term_flag1                          VC8000E_REG(17,  9, 0x1)
#define vc8000e_l0_used_by_curr_pic1                        VC8000E_REG(17,  8, 0x1)
#define vc8000e_active_l0_cnt                               VC8000E_REG(17,  6, 0x3)
#define vc8000e_recon_luma_compressor_enable                VC8000E_REG(17,  5, 0x1)
#define vc8000e_recon_chroma_compressor_enable              VC8000E_REG(17,  4, 0x1)
#define vc8000e_l0_ref0_luma_compressor_enable              VC8000E_REG(17,  3, 0x1)
#define vc8000e_l0_ref0_chroma_compressor_enable            VC8000E_REG(17,  2, 0x1)
#define vc8000e_l0_ref1_luma_compressor_enable              VC8000E_REG(17,  1, 0x1)
#define vc8000e_l0_ref1_chroma_compressor_enable            VC8000E_REG(17,  0, 0x1)

#define vc8000e_refpic_recon_l0_y0                          VC8000E_REG(18,  0, 0xffffffff)
#define vc8000e_ref_ringbuf_luma_rd_offset                  VC8000E_REG(18,  0, 0xffffffff)
#define vc8000e_strm_startoffset                            VC8000E_REG(18, 26, 0x3f)
#define vc8000e_jpeg_slice                                  VC8000E_REG(18, 25, 0x1)
#define vc8000e_jpeg_mode                                   VC8000E_REG(18, 24, 0x1)
#define vc8000e_jpeg_rstint                                 VC8000E_REG(18, 16, 0xff)
#define vc8000e_jpeg_rst                                    VC8000E_REG(18,  0, 0xffff)

#define vc8000e_refpic_recon_l0_chroma0                     VC8000E_REG(19,  0, 0xffffffff)
#define vc8000e_strm_hdrrem1                                VC8000E_REG(19,  0, 0xffffffff)
#define vc8000e_ref_ringbuf_chroma_rd_offset                VC8000E_REG(19,  0, 0xffffffff)

#define vc8000e_refpic_recon_l0_y1                          VC8000E_REG(20,  0, 0xffffffff)
#define vc8000e_jpeg_rowlength                              VC8000E_REG(20, 17, 0x7fff)
#define vc8000e_ljpeg_en                                    VC8000E_REG(20, 16, 0x1)
#define vc8000e_ljpeg_format                                VC8000E_REG(20, 14, 0x3)
#define vc8000e_ljpeg_psv                                   VC8000E_REG(20, 11, 0x7)
#define vc8000e_ljpeg_pt                                    VC8000E_REG(20,  8, 0x7)

#define vc8000e_refpic_recon_l0_chroma1                     VC8000E_REG(21,  0, 0xffffffff)
#define vc8000e_strm_hdrrem2                                VC8000E_REG(21,  0, 0xffffffff)

#define vc8000e_cir_start                                   VC8000E_REG(22, 18, 0x3fff)
#define vc8000e_cir_interval                                VC8000E_REG(22,  4, 0x3fff)
#define vc8000e_rcroi_enable                                VC8000E_REG(22,  0, 0xf)

#define vc8000e_intra_area_left                             VC8000E_REG(23, 24, 0xff)
#define vc8000e_intra_area_right                            VC8000E_REG(23, 16, 0xff)
#define vc8000e_intra_area_top                              VC8000E_REG(23,  8, 0xff)
#define vc8000e_intra_area_bottom                           VC8000E_REG(23,  0, 0xff)

#define vc8000e_roi1                                        VC8000E_REG(24,  0, 0xffffffff)
#define vc8000e_roi1_left                                   VC8000E_REG(24, 24, 0xff)
#define vc8000e_roi1_right                                  VC8000E_REG(24, 16, 0xff)
#define vc8000e_roi1_top                                    VC8000E_REG(24,  8, 0xff)
#define vc8000e_roi1_bottom                                 VC8000E_REG(24,  0, 0xff)

#define vc8000e_roi2                                        VC8000E_REG(25,  0, 0xffffffff)
#define vc8000e_roi2_left                                   VC8000E_REG(25, 24, 0xff)
#define vc8000e_roi2_right                                  VC8000E_REG(25, 16, 0xff)
#define vc8000e_roi2_top                                    VC8000E_REG(25,  8, 0xff)
#define vc8000e_roi2_bottom                                 VC8000E_REG(25,  0, 0xff)

#define vc8000e_intra_penalty_pic4x4                        VC8000E_REG(26, 19, 0x1fff)
#define vc8000e_intra_mpm_penalty_pic1                      VC8000E_REG(26, 10, 0x1ff)
#define vc8000e_intra_mpm_penalty_pic2                      VC8000E_REG(26,  0, 0x3ff)
#define vc8000e_intra_size_factor_0                         VC8000E_REG(26, 22, 0x3ff)
#define vc8000e_intra_size_factor_1                         VC8000E_REG(26, 12, 0x3ff)
#define vc8000e_intra_size_factor_2                         VC8000E_REG(26,  2, 0x3ff)

#define vc8000e_intra_penalty_pic8x8                        VC8000E_REG(27, 19, 0x1fff)
#define vc8000e_intra_penalty_pic16x16                      VC8000E_REG(27,  5, 0x3fff)
#define vc8000e_intra_size_factor_3                         VC8000E_REG(27, 22, 0x3ff)
#define vc8000e_intra_mode_factor_0                         VC8000E_REG(27, 17, 0x1f)
#define vc8000e_intra_mode_factor_1                         VC8000E_REG(27, 11, 0x3f)
#define vc8000e_intra_mode_factor_2                         VC8000E_REG(27,  4, 0x7f)

#define vc8000e_intra_penalty_pic32x32                      VC8000E_REG(28, 18, 0x3fff)
#define vc8000e_intra_mpm_penalty_pic3                      VC8000E_REG(28,  7, 0x7ff)
#define vc8000e_lamda_satd_me_0                             VC8000E_REG(28, 24, 0xff)
#define vc8000e_lamda_satd_me_1                             VC8000E_REG(28, 16, 0xff)
#define vc8000e_lamda_satd_me_2                             VC8000E_REG(28,  8, 0xff)
#define vc8000e_lamda_satd_me_3                             VC8000E_REG(28,  0, 0xff)
#define vc8000e_lamda_satd_me_0_expand5bit                  VC8000E_REG(28, 19, 0x1fff)
#define vc8000e_lamda_satd_me_1_expand5bit                  VC8000E_REG(28,  6, 0x1fff)
#define vc8000e_av1_precarry_buffer_base                    VC8000E_REG(28,  0, 0xffffffff)

#define vc8000e_intra_penalty_roi14x4                       VC8000E_REG(29, 19, 0x1fff)
#define vc8000e_intra_mpm_penalty_roi11                     VC8000E_REG(29, 10, 0x1ff)
#define vc8000e_intra_mpm_penalty_roi12                     VC8000E_REG(29,  0, 0x3ff)
#define vc8000e_lamda_satd_me_4                             VC8000E_REG(29, 24, 0xff)
#define vc8000e_lamda_satd_me_5                             VC8000E_REG(29, 16, 0xff)
#define vc8000e_lamda_satd_me_6                             VC8000E_REG(29,  8, 0xff)
#define vc8000e_lamda_satd_me_7                             VC8000E_REG(29,  0, 0xff)
#define vc8000e_lamda_satd_me_2_expand5bit                  VC8000E_REG(29, 19, 0x1fff)
#define vc8000e_lamda_satd_me_3_expand5bit                  VC8000E_REG(29,  6, 0x1fff)
#define vc8000e_av1_precarry_buffer_base_msb                VC8000E_REG(29,  0, 0xffffffff)

#define vc8000e_intra_penalty_roi18x8                       VC8000E_REG(30, 19, 0x1fff)
#define vc8000e_intra_penalty_roi116x16                     VC8000E_REG(30,  5, 0x3fff)
#define vc8000e_lamda_satd_me_8                             VC8000E_REG(30, 24, 0xff)
#define vc8000e_lamda_satd_me_9                             VC8000E_REG(30, 16, 0xff)
#define vc8000e_lamda_satd_me_10                            VC8000E_REG(30,  8, 0xff)
#define vc8000e_lamda_satd_me_11                            VC8000E_REG(30,  0, 0xff)
#define vc8000e_lamda_satd_me_4_expand5bit                  VC8000E_REG(30, 19, 0x1fff)
#define vc8000e_lamda_satd_me_5_expand5bit                  VC8000E_REG(30,  6, 0x1fff)

#define vc8000e_intra_penalty_roi132x32                     VC8000E_REG(31, 18, 0x3fff)
#define vc8000e_intra_mpm_penalty_roi13                     VC8000E_REG(31,  7, 0x7ff)
#define vc8000e_lamda_satd_me_12                            VC8000E_REG(31, 24, 0xff)
#define vc8000e_lamda_satd_me_13                            VC8000E_REG(31, 16, 0xff)
#define vc8000e_lamda_satd_me_14                            VC8000E_REG(31,  8, 0xff)
#define vc8000e_lamda_satd_me_15                            VC8000E_REG(31,  0, 0xff)
#define vc8000e_lamda_satd_me_6_expand5bit                  VC8000E_REG(31, 19, 0x1fff)
#define vc8000e_lamda_satd_me_7_expand5bit                  VC8000E_REG(31,  6, 0x1fff)
#define vc8000e_dynamic_rdo_cu32_factor                     VC8000E_REG(31, 24, 0xff)
#define vc8000e_dynamic_rdo_cu32_bias                       VC8000E_REG(31, 16, 0xff)
#define vc8000e_dynamic_rdo_cu16_factor                     VC8000E_REG(31,  8, 0xff)
#define vc8000e_dynamic_rdo_cu16_bias                       VC8000E_REG(31,  0, 0xff)

#define vc8000e_intra_penalty_roi24x4                       VC8000E_REG(32, 19, 0x1fff)
#define vc8000e_intra_mpm_penalty_roi21                     VC8000E_REG(32, 10, 0x1ff)
#define vc8000e_intra_mpm_penalty_roi22                     VC8000E_REG(32,  0, 0x3ff)
#define vc8000e_lamda_sse_me_0                              VC8000E_REG(32, 17, 0x7fff)
#define vc8000e_lamda_sse_me_1                              VC8000E_REG(32,  2, 0x7fff)
#define vc8000e_lamda_satd_me_8_expand5bit                  VC8000E_REG(32, 19, 0x1fff)
#define vc8000e_lamda_satd_me_9_expand5bit                  VC8000E_REG(32,  6, 0x1fff)

#define vc8000e_intra_penalty_roi28x8                       VC8000E_REG(33, 19, 0x1fff)
#define vc8000e_intra_penalty_roi216x16                     VC8000E_REG(33,  5, 0x3fff)
#define vc8000e_lamda_sse_me_2                              VC8000E_REG(33, 17, 0x7fff)
#define vc8000e_lamda_sse_me_3                              VC8000E_REG(33,  2, 0x7fff)
#define vc8000e_lamda_satd_me_10_expand5bit                 VC8000E_REG(33, 19, 0x1fff)
#define vc8000e_lamda_satd_me_11_expand5bit                 VC8000E_REG(33,  6, 0x1fff)

#define vc8000e_intra_penalty_roi232x32                     VC8000E_REG(34, 18, 0x3fff)
#define vc8000e_intra_mpm_penalty_roi23                     VC8000E_REG(34,  7, 0x7ff)
#define vc8000e_lambda_motionsad                            VC8000E_REG(34,  0, 0x7f)
#define vc8000e_lamda_sse_me_4                              VC8000E_REG(34, 17, 0x7fff)
#define vc8000e_lamda_sse_me_5                              VC8000E_REG(34,  2, 0x7fff)
#define vc8000e_lamda_satd_me_12_expand5bit                 VC8000E_REG(34, 19, 0x1fff)
#define vc8000e_lamda_satd_me_13_expand5bit                 VC8000E_REG(34,  6, 0x1fff)

#define vc8000e_lamda_motion_sse                            VC8000E_REG(35, 18, 0x3fff)
#define vc8000e_bits_est_tu_split_penalty                   VC8000E_REG(35, 15, 0x7)
#define vc8000e_bits_est_bias_intra_cu_8                    VC8000E_REG(35,  8, 0x7f)
#define vc8000e_bits_est_bias_intra_cu_16                   VC8000E_REG(35,  0, 0xff)

#define vc8000e_bits_est_bias_intra_cu_32                   VC8000E_REG(36, 23, 0x1ff)
#define vc8000e_bits_est_bias_intra_cu_64                   VC8000E_REG(36, 13, 0x3ff)
#define vc8000e_inter_skip_bias                             VC8000E_REG(36,  6, 0x7f)
#define vc8000e_bits_est_1n_cu_penalty                      VC8000E_REG(36,  2, 0xf)
#define vc8000e_output_bitwidth_chroma                      VC8000E_REG(36,  0, 0x3)

#define vc8000e_lamda_sao_chroma                            VC8000E_REG(37, 18, 0x3fff)
#define vc8000e_lamda_sao_luma                              VC8000E_REG(37,  4, 0x3fff)
#define vc8000e_chroffset                                   VC8000E_REG(37,  0, 0xf)

#define vc8000e_input_format                                VC8000E_REG(38, 28, 0xf)
#define vc8000e_input_rotation                              VC8000E_REG(38, 26, 0x3)
#define     VC8000E_INPUT_ROTATE_OFF                        0
#define     VC8000E_INPUT_ROTATE_90_RIGHT                   1
#define     VC8000E_INPUT_ROTATE_90_LEFT                    2
#define vc8000e_output_bitwidth_lum                         VC8000E_REG(38, 24, 0x3)
#define     VC8000E_OUTPUT_LUMA_8BIT                        0
#define     VC8000E_OUTPUT_LUMA_9BIT                        1
#define     VC8000E_OUTPUT_LUMA_10BIT                       2
#define vc8000e_lumoffset                                   VC8000E_REG(38, 20, 0xf)
#define vc8000e_rowlength                                   VC8000E_REG(38,  6, 0x3fff)
#define vc8000e_xfill                                       VC8000E_REG(38,  4, 0x3)
#define vc8000e_yfill                                       VC8000E_REG(38,  1, 0x7)
#define vc8000e_mirror                                      VC8000E_REG(38,  0, 0x1)

#define vc8000e_rgbcoeffa                                   VC8000E_REG(39, 16, 0xffff)
#define vc8000e_rgbcoeffb                                   VC8000E_REG(39,  0, 0xffff)

#define vc8000e_rgbcoeffc                                   VC8000E_REG(40, 16, 0xffff)
#define vc8000e_rgbcoeffe                                   VC8000E_REG(40,  0, 0xffff)

#define vc8000e_rgbcoefff                                   VC8000E_REG(41, 16, 0xffff)
#define vc8000e_rmaskmsb                                    VC8000E_REG(41, 11, 0x1f)
#define vc8000e_gmaskmsb                                    VC8000E_REG(41,  6, 0x1f)
#define vc8000e_bmaskmsb                                    VC8000E_REG(41,  1, 0x1f)

#define vc8000e_basescaledoutlum                            VC8000E_REG(42,  0, 0xffffffff)

#define vc8000e_scaledoutwidth                              VC8000E_REG(43, 19, 0x1fff)
#define vc8000e_scaledoutwidthratio                         VC8000E_REG(43,  3, 0xffff)
#define vc8000e_scaledoutwidthmsb                           VC8000E_REG(43,  2, 0x1)
#define vc8000e_scale_mode                                  VC8000E_REG(43,  0, 0x3)

#define vc8000e_scaledoutheight                             VC8000E_REG(44, 18, 0x3fff)
#define vc8000e_scaledoutheightratio                        VC8000E_REG(44,  2, 0xffff)
#define vc8000e_input_format_msb                            VC8000E_REG(44,  0, 0x3)

#define vc8000e_scaledout_swap                              VC8000E_REG(45, 28, 0xf)
#define vc8000e_chroma_swap                                 VC8000E_REG(45, 27, 0x1)
#define vc8000e_encoded_ctb_number                          VC8000E_REG(45, 14, 0x1fff)
#define vc8000e_scaledskipleftpixelcolumn                   VC8000E_REG(45, 12, 0x3)
#define vc8000e_scaledskiptoppixelrow                       VC8000E_REG(45, 10, 0x3)
#define vc8000e_vscale_weight_en                            VC8000E_REG(45,  9, 0x1)
#define vc8000e_scaledhorizontalcopy                        VC8000E_REG(45,  8, 0x1)
#define vc8000e_scaledverticalcopy                          VC8000E_REG(45,  7, 0x1)
#define vc8000e_nalunitsize_swap                            VC8000E_REG(45,  3, 0xf)
#define vc8000e_scaledout_format                            VC8000E_REG(45,  2, 0x1)

#define vc8000e_compressedcoeff_base                        VC8000E_REG(46,  0, 0xffffffff)

#define vc8000e_compressedcoeff_base_msb                    VC8000E_REG(47,  0, 0xffffffff)

#define vc8000e_basescaledoutlum_msb                        VC8000E_REG(48,  0, 0xffffffff)

#define vc8000e_refpic_recon_l0_y0_msb                      VC8000E_REG(49,  0, 0xffffffff)

#define vc8000e_refpic_recon_l0_chroma0_msb                 VC8000E_REG(50,  0, 0xffffffff)

#define vc8000e_refpic_recon_l0_y1_msb                      VC8000E_REG(51,  0, 0xffffffff)

#define vc8000e_refpic_recon_l0_chroma1_msb                 VC8000E_REG(52,  0, 0xffffffff)

#define vc8000e_input_y_base_msb                            VC8000E_REG(53,  0, 0xffffffff)

#define vc8000e_input_cb_base_msb                           VC8000E_REG(54,  0, 0xffffffff)

#define vc8000e_input_cr_base_msb                           VC8000E_REG(55,  0, 0xffffffff)

#define vc8000e_recon_y_base_msb                            VC8000E_REG(56,  0, 0xffffffff)
#define vc8000e_ref_ringbuf_luma_size                       VC8000E_REG(56,  0, 0xffffffff)

#define vc8000e_recon_chroma_base_msb                       VC8000E_REG(57,  0, 0xffffffff)
#define vc8000e_ref_ringbuf_chroma_size                     VC8000E_REG(57,  0, 0xffffffff)

#define vc8000e_size_tbl_base_msb                           VC8000E_REG(58,  0, 0xffffffff)

#define vc8000e_output_strm_base_msb                        VC8000E_REG(59,  0, 0xffffffff)

#define vc8000e_recon_luma_compress_table_base              VC8000E_REG(60,  0, 0xffffffff)

#define vc8000e_recon_luma_compress_table_base_msb          VC8000E_REG(61,  0, 0xffffffff)

#define vc8000e_recon_chroma_compress_table_base            VC8000E_REG(62,  0, 0xffffffff)

#define vc8000e_recon_chroma_compress_table_base_msb        VC8000E_REG(63,  0, 0xffffffff)

#define vc8000e_l0_ref0_luma_compress_table_base            VC8000E_REG(64,  0, 0xffffffff)

#define vc8000e_l0_ref0_luma_compress_table_base_msb        VC8000E_REG(65,  0, 0xffffffff)

#define vc8000e_l0_ref0_chroma_compress_table_base          VC8000E_REG(66,  0, 0xffffffff)

#define vc8000e_l0_ref0_chroma_compress_table_base_msb      VC8000E_REG(67,  0, 0xffffffff)

#define vc8000e_l0_ref1_luma_compress_table_base            VC8000E_REG(68,  0, 0xffffffff)

#define vc8000e_l0_ref1_luma_compress_table_base_msb        VC8000E_REG(69,  0, 0xffffffff)

#define vc8000e_l0_ref1_chroma_compress_table_base          VC8000E_REG(70,  0, 0xffffffff)

#define vc8000e_l0_ref1_chroma_compress_table_base_msb      VC8000E_REG(71,  0, 0xffffffff)

#define vc8000e_recon_luma_4n_base                          VC8000E_REG(72,  0, 0xffffffff)
#define vc8000e_ref_ringbuf_luma_4n_wr_offset               VC8000E_REG(72,  0, 0xffffffff)

#define vc8000e_recon_luma_4n_base_msb                      VC8000E_REG(73,  0, 0xffffffff)
#define vc8000e_ref_ringbuf_luma_4n_size                    VC8000E_REG(73,  0, 0xffffffff)

#define vc8000e_refpic_recon_l0_4n0_base                    VC8000E_REG(74,  0, 0xffffffff)
#define vc8000e_ref_ringbuf_luma_4n_rd_offset               VC8000E_REG(74,  0, 0xffffffff)

#define vc8000e_refpic_recon_l0_4n0_base_msb                VC8000E_REG(75,  0, 0xffffffff)

#define vc8000e_refpic_recon_l0_4n1_base                    VC8000E_REG(76,  0, 0xffffffff)

#define vc8000e_refpic_recon_l0_4n1_base_msb                VC8000E_REG(77,  0, 0xffffffff)

#define vc8000e_lamda_motion_sse_roi1                       VC8000E_REG(78, 18, 0x3fff)
#define vc8000e_lamda_motion_sse_roi2                       VC8000E_REG(78,  4, 0x3fff)
#define vc8000e_skip_chroma_dc_threadhold                   VC8000E_REG(78,  0, 0x3)
#define vc8000e_lamda_sse_me_6                              VC8000E_REG(78, 17, 0x7fff)
#define vc8000e_lamda_sse_me_7                              VC8000E_REG(78,  2, 0x7fff)
#define vc8000e_lamda_satd_me_14_expand5bit                 VC8000E_REG(78, 19, 0x1fff)
#define vc8000e_lamda_satd_me_15_expand5bit                 VC8000E_REG(78,  6, 0x1fff)

#define vc8000e_lambda_motionsad_roi1                       VC8000E_REG(79, 25, 0x7f)
#define vc8000e_lambda_motionsad_roi2                       VC8000E_REG(79, 18, 0x7f)
#define vc8000e_lamda_sse_me_8                              VC8000E_REG(79, 17, 0x7fff)
#define vc8000e_lamda_sse_me_9                              VC8000E_REG(79,  2, 0x7fff)
#define vc8000e_lamda_sse_me_0_expand6bit                   VC8000E_REG(79, 11, 0x1fffff)

#define vc8000e_hwh264support                               VC8000E_REG(80, 31, 0x1)
#define vc8000e_hwscalingsupport                            VC8000E_REG(80, 30, 0x1)
#define vc8000e_hwbframesupport                             VC8000E_REG(80, 29, 0x1)
#define vc8000e_hwrgbsupport                                VC8000E_REG(80, 28, 0x1)
#define vc8000e_hwhevcsupport                               VC8000E_REG(80, 27, 0x1)
#define vc8000e_hwvp9support                                VC8000E_REG(80, 26, 0x1)
#define vc8000e_hwdenoisesupport                            VC8000E_REG(80, 25, 0x1)
#define vc8000e_hwmain10support                             VC8000E_REG(80, 24, 0x1)
#define vc8000e_hwbus                                       VC8000E_REG(80, 21, 0x7)
#define vc8000e_hwcavlcsupport                              VC8000E_REG(80, 20, 0x1)
#define vc8000e_hwlinebufsupport                            VC8000E_REG(80, 19, 0x1)
#define vc8000e_hwprogrdosupport                            VC8000E_REG(80, 18, 0x1)
#define vc8000e_hwrfcsupport                                VC8000E_REG(80, 17, 0x1)
#define vc8000e_hwtu32support                               VC8000E_REG(80, 16, 0x1)
#define vc8000e_hwjpegsupport                               VC8000E_REG(80, 15, 0x1)
#define vc8000e_hwbuswidth                                  VC8000E_REG(80, 13, 0x3)
#define vc8000e_hwmaxvideowidth                             VC8000E_REG(80,  0, 0x1fff)
#define vc8000e_hwmaxvideowidthhevc                         VC8000E_REG(80,  0, 0x1fff)

#define vc8000e_max_burst                                   VC8000E_REG(81, 24, 0xff)
#define vc8000e_timeout_override_e                          VC8000E_REG(81, 23, 0x1)
#define vc8000e_timeout_cycles                              VC8000E_REG(81,  0, 0x7fffff)

#define vc8000e_hw_performance                              VC8000E_REG(82,  0, 0xffffffff)

#define vc8000e_refpic_recon_l1_y0                          VC8000E_REG(83,  0, 0xffffffff)
#define vc8000e_ref_ringbuf_base_addr                       VC8000E_REG(83,  0, 0xffffffff)
#define vc8000e_jpeg_quant_table_luma_0                     VC8000E_REG(83, 24, 0xff)
#define vc8000e_jpeg_quant_table_luma_1                     VC8000E_REG(83, 16, 0xff)
#define vc8000e_jpeg_quant_table_luma_2                     VC8000E_REG(83,  8, 0xff)
#define vc8000e_jpeg_quant_table_luma_3                     VC8000E_REG(83,  0, 0xff)

#define vc8000e_refpic_recon_l1_chroma0                     VC8000E_REG(84,  0, 0xffffffff)
#define vc8000e_ref_ringbuf_base_addr_msb                   VC8000E_REG(84,  0, 0xffffffff)
#define vc8000e_jpeg_quant_table_luma_4                     VC8000E_REG(84, 24, 0xff)
#define vc8000e_jpeg_quant_table_luma_5                     VC8000E_REG(84, 16, 0xff)
#define vc8000e_jpeg_quant_table_luma_6                     VC8000E_REG(84,  8, 0xff)
#define vc8000e_jpeg_quant_table_luma_7                     VC8000E_REG(84,  0, 0xff)

#define vc8000e_refpic_recon_l1_y1                          VC8000E_REG(85,  0, 0xffffffff)
#define vc8000e_jpeg_quant_table_luma_8                     VC8000E_REG(85, 24, 0xff)
#define vc8000e_jpeg_quant_table_luma_9                     VC8000E_REG(85, 16, 0xff)
#define vc8000e_jpeg_quant_table_luma_10                    VC8000E_REG(85,  8, 0xff)
#define vc8000e_jpeg_quant_table_luma_11                    VC8000E_REG(85,  0, 0xff)

#define vc8000e_refpic_recon_l1_chroma1                     VC8000E_REG(86,  0, 0xffffffff)
#define vc8000e_jpeg_quant_table_luma_12                    VC8000E_REG(86, 24, 0xff)
#define vc8000e_jpeg_quant_table_luma_13                    VC8000E_REG(86, 16, 0xff)
#define vc8000e_jpeg_quant_table_luma_14                    VC8000E_REG(86,  8, 0xff)
#define vc8000e_jpeg_quant_table_luma_15                    VC8000E_REG(86,  0, 0xff)

#define vc8000e_refpic_recon_l1_y0_msb                      VC8000E_REG(87,  0, 0xffffffff)
#define vc8000e_jpeg_quant_table_luma_16                    VC8000E_REG(87, 24, 0xff)
#define vc8000e_jpeg_quant_table_luma_17                    VC8000E_REG(87, 16, 0xff)
#define vc8000e_jpeg_quant_table_luma_18                    VC8000E_REG(87,  8, 0xff)
#define vc8000e_jpeg_quant_table_luma_19                    VC8000E_REG(87,  0, 0xff)

#define vc8000e_refpic_recon_l1_chroma0_msb                 VC8000E_REG(88,  0, 0xffffffff)
#define vc8000e_jpeg_quant_table_luma_20                    VC8000E_REG(88, 24, 0xff)
#define vc8000e_jpeg_quant_table_luma_21                    VC8000E_REG(88, 16, 0xff)
#define vc8000e_jpeg_quant_table_luma_22                    VC8000E_REG(88,  8, 0xff)
#define vc8000e_jpeg_quant_table_luma_23                    VC8000E_REG(88,  0, 0xff)

#define vc8000e_refpic_recon_l1_y1_msb                      VC8000E_REG(89,  0, 0xffffffff)
#define vc8000e_jpeg_quant_table_luma_24                    VC8000E_REG(89, 24, 0xff)
#define vc8000e_jpeg_quant_table_luma_25                    VC8000E_REG(89, 16, 0xff)
#define vc8000e_jpeg_quant_table_luma_26                    VC8000E_REG(89,  8, 0xff)
#define vc8000e_jpeg_quant_table_luma_27                    VC8000E_REG(89,  0, 0xff)

#define vc8000e_refpic_recon_l1_chroma1_msb                 VC8000E_REG(90,  0, 0xffffffff)
#define vc8000e_jpeg_quant_table_luma_28                    VC8000E_REG(90, 24, 0xff)
#define vc8000e_jpeg_quant_table_luma_29                    VC8000E_REG(90, 16, 0xff)
#define vc8000e_jpeg_quant_table_luma_30                    VC8000E_REG(90,  8, 0xff)
#define vc8000e_jpeg_quant_table_luma_31                    VC8000E_REG(90,  0, 0xff)

#define vc8000e_l1_delta_poc0                               VC8000E_REG(91, 22, 0x3ff)
#define vc8000e_l1_long_term_flag0                          VC8000E_REG(91, 21, 0x1)
#define vc8000e_l1_used_by_curr_pic0                        VC8000E_REG(91, 20, 0x1)
#define vc8000e_l1_delta_poc1                               VC8000E_REG(91, 10, 0x3ff)
#define vc8000e_l1_long_term_flag1                          VC8000E_REG(91,  9, 0x1)
#define vc8000e_l1_used_by_curr_pic1                        VC8000E_REG(91,  8, 0x1)
#define vc8000e_active_l1_cnt                               VC8000E_REG(91,  6, 0x3)
#define vc8000e_long_term_ref_pics_present_flag             VC8000E_REG(91,  4, 0x1)
#define vc8000e_l1_ref0_luma_compressor_enable              VC8000E_REG(91,  3, 0x1)
#define vc8000e_l1_ref0_chroma_compressor_enable            VC8000E_REG(91,  2, 0x1)
#define vc8000e_l1_ref1_luma_compressor_enable              VC8000E_REG(91,  1, 0x1)
#define vc8000e_l1_ref1_chroma_compressor_enable            VC8000E_REG(91,  0, 0x1)
#define vc8000e_jpeg_quant_table_luma_32                    VC8000E_REG(91, 24, 0xff)
#define vc8000e_jpeg_quant_table_luma_33                    VC8000E_REG(91, 16, 0xff)
#define vc8000e_jpeg_quant_table_luma_34                    VC8000E_REG(91,  8, 0xff)
#define vc8000e_jpeg_quant_table_luma_35                    VC8000E_REG(91,  0, 0xff)

#define vc8000e_refpic_recon_l1_4n0_base                    VC8000E_REG(92,  0, 0xffffffff)
#define vc8000e_jpeg_quant_table_luma_36                    VC8000E_REG(92, 24, 0xff)
#define vc8000e_jpeg_quant_table_luma_37                    VC8000E_REG(92, 16, 0xff)
#define vc8000e_jpeg_quant_table_luma_38                    VC8000E_REG(92,  8, 0xff)
#define vc8000e_jpeg_quant_table_luma_39                    VC8000E_REG(92,  0, 0xff)

#define vc8000e_refpic_recon_l1_4n0_base_msb                VC8000E_REG(93,  0, 0xffffffff)
#define vc8000e_jpeg_quant_table_luma_40                    VC8000E_REG(93, 24, 0xff)
#define vc8000e_jpeg_quant_table_luma_41                    VC8000E_REG(93, 16, 0xff)
#define vc8000e_jpeg_quant_table_luma_42                    VC8000E_REG(93,  8, 0xff)
#define vc8000e_jpeg_quant_table_luma_43                    VC8000E_REG(93,  0, 0xff)

#define vc8000e_refpic_recon_l1_4n1_base                    VC8000E_REG(94,  0, 0xffffffff)
#define vc8000e_jpeg_quant_table_luma_44                    VC8000E_REG(94, 24, 0xff)
#define vc8000e_jpeg_quant_table_luma_45                    VC8000E_REG(94, 16, 0xff)
#define vc8000e_jpeg_quant_table_luma_46                    VC8000E_REG(94,  8, 0xff)
#define vc8000e_jpeg_quant_table_luma_47                    VC8000E_REG(94,  0, 0xff)

#define vc8000e_refpic_recon_l1_4n1_base_msb                VC8000E_REG(95,  0, 0xffffffff)
#define vc8000e_jpeg_quant_table_luma_48                    VC8000E_REG(95, 24, 0xff)
#define vc8000e_jpeg_quant_table_luma_49                    VC8000E_REG(95, 16, 0xff)
#define vc8000e_jpeg_quant_table_luma_50                    VC8000E_REG(95,  8, 0xff)
#define vc8000e_jpeg_quant_table_luma_51                    VC8000E_REG(95,  0, 0xff)

#define vc8000e_l1_ref0_luma_compress_table_base            VC8000E_REG(96,  0, 0xffffffff)
#define vc8000e_jpeg_quant_table_luma_52                    VC8000E_REG(96, 24, 0xff)
#define vc8000e_jpeg_quant_table_luma_53                    VC8000E_REG(96, 16, 0xff)
#define vc8000e_jpeg_quant_table_luma_54                    VC8000E_REG(96,  8, 0xff)
#define vc8000e_jpeg_quant_table_luma_55                    VC8000E_REG(96,  0, 0xff)

#define vc8000e_l1_ref0_luma_compress_table_base_msb        VC8000E_REG(97,  0, 0xffffffff)
#define vc8000e_jpeg_quant_table_luma_56                    VC8000E_REG(97, 24, 0xff)
#define vc8000e_jpeg_quant_table_luma_57                    VC8000E_REG(97, 16, 0xff)
#define vc8000e_jpeg_quant_table_luma_58                    VC8000E_REG(97,  8, 0xff)
#define vc8000e_jpeg_quant_table_luma_59                    VC8000E_REG(97,  0, 0xff)

#define vc8000e_l1_ref0_chroma_compress_table_base          VC8000E_REG(98,  0, 0xffffffff)
#define vc8000e_jpeg_quant_table_luma_60                    VC8000E_REG(98, 24, 0xff)
#define vc8000e_jpeg_quant_table_luma_61                    VC8000E_REG(98, 16, 0xff)
#define vc8000e_jpeg_quant_table_luma_62                    VC8000E_REG(98,  8, 0xff)
#define vc8000e_jpeg_quant_table_luma_63                    VC8000E_REG(98,  0, 0xff)

#define vc8000e_l1_ref0_chroma_compress_table_base_msb      VC8000E_REG(99,  0, 0xffffffff)
#define vc8000e_jpeg_quant_table_chroma_0                   VC8000E_REG(99, 24, 0xff)
#define vc8000e_jpeg_quant_table_chroma_1                   VC8000E_REG(99, 16, 0xff)
#define vc8000e_jpeg_quant_table_chroma_2                   VC8000E_REG(99,  8, 0xff)
#define vc8000e_jpeg_quant_table_chroma_3                   VC8000E_REG(99,  0, 0xff)

#define vc8000e_l1_ref1_luma_compress_table_base            VC8000E_REG(100,  0, 0xffffffff)
#define vc8000e_jpeg_quant_table_chroma_4                   VC8000E_REG(100, 24, 0xff)
#define vc8000e_jpeg_quant_table_chroma_5                   VC8000E_REG(100, 16, 0xff)
#define vc8000e_jpeg_quant_table_chroma_6                   VC8000E_REG(100,  8, 0xff)
#define vc8000e_jpeg_quant_table_chroma_7                   VC8000E_REG(100,  0, 0xff)

#define vc8000e_l1_ref1_luma_compress_table_base_msb        VC8000E_REG(101,  0, 0xffffffff)
#define vc8000e_jpeg_quant_table_chroma_8                   VC8000E_REG(101, 24, 0xff)
#define vc8000e_jpeg_quant_table_chroma_9                   VC8000E_REG(101, 16, 0xff)
#define vc8000e_jpeg_quant_table_chroma_10                  VC8000E_REG(101,  8, 0xff)
#define vc8000e_jpeg_quant_table_chroma_11                  VC8000E_REG(101,  0, 0xff)

#define vc8000e_l1_ref1_chroma_compress_table_base          VC8000E_REG(102,  0, 0xffffffff)
#define vc8000e_jpeg_quant_table_chroma_12                  VC8000E_REG(102, 24, 0xff)
#define vc8000e_jpeg_quant_table_chroma_13                  VC8000E_REG(102, 16, 0xff)
#define vc8000e_jpeg_quant_table_chroma_14                  VC8000E_REG(102,  8, 0xff)
#define vc8000e_jpeg_quant_table_chroma_15                  VC8000E_REG(102,  0, 0xff)

#define vc8000e_l1_ref1_chroma_compress_table_base_msb      VC8000E_REG(103,  0, 0xffffffff)
#define vc8000e_jpeg_quant_table_chroma_16                  VC8000E_REG(103, 24, 0xff)
#define vc8000e_jpeg_quant_table_chroma_17                  VC8000E_REG(103, 16, 0xff)
#define vc8000e_jpeg_quant_table_chroma_18                  VC8000E_REG(103,  8, 0xff)
#define vc8000e_jpeg_quant_table_chroma_19                  VC8000E_REG(103,  0, 0xff)

#define vc8000e_lists_modi_present_flag                     VC8000E_REG(104, 31, 0x1)
#define vc8000e_rdo_level                                   VC8000E_REG(104, 29, 0x3)
#define vc8000e_list_entry_l1_pic1                          VC8000E_REG(104, 21, 0xf)
#define vc8000e_list_entry_l1_pic0                          VC8000E_REG(104, 17, 0xf)
#define vc8000e_ref_pic_list_modi_flag_l1                   VC8000E_REG(104, 16, 0x1)
#define vc8000e_list_entry_l0_pic1                          VC8000E_REG(104,  5, 0xf)
#define vc8000e_list_entry_l0_pic0                          VC8000E_REG(104,  1, 0xf)
#define vc8000e_ref_pic_list_modi_flag_l0                   VC8000E_REG(104,  0, 0x1)
#define vc8000e_jpeg_quant_table_chroma_20                  VC8000E_REG(104, 24, 0xff)
#define vc8000e_jpeg_quant_table_chroma_21                  VC8000E_REG(104, 16, 0xff)
#define vc8000e_jpeg_quant_table_chroma_22                  VC8000E_REG(104,  8, 0xff)
#define vc8000e_jpeg_quant_table_chroma_23                  VC8000E_REG(104,  0, 0xff)

#define vc8000e_targetpicsize                               VC8000E_REG(105,  0, 0xffffffff)
#define vc8000e_jpeg_quant_table_chroma_24                  VC8000E_REG(105, 24, 0xff)
#define vc8000e_jpeg_quant_table_chroma_25                  VC8000E_REG(105, 16, 0xff)
#define vc8000e_jpeg_quant_table_chroma_26                  VC8000E_REG(105,  8, 0xff)
#define vc8000e_jpeg_quant_table_chroma_27                  VC8000E_REG(105,  0, 0xff)

#define vc8000e_minpicsize                                  VC8000E_REG(106,  0, 0xffffffff)
#define vc8000e_jpeg_quant_table_chroma_28                  VC8000E_REG(106, 24, 0xff)
#define vc8000e_jpeg_quant_table_chroma_29                  VC8000E_REG(106, 16, 0xff)
#define vc8000e_jpeg_quant_table_chroma_30                  VC8000E_REG(106,  8, 0xff)
#define vc8000e_jpeg_quant_table_chroma_31                  VC8000E_REG(106,  0, 0xff)

#define vc8000e_maxpicsize                                  VC8000E_REG(107,  0, 0xffffffff)
#define vc8000e_jpeg_quant_table_chroma_32                  VC8000E_REG(107, 24, 0xff)
#define vc8000e_jpeg_quant_table_chroma_33                  VC8000E_REG(107, 16, 0xff)
#define vc8000e_jpeg_quant_table_chroma_34                  VC8000E_REG(107,  8, 0xff)
#define vc8000e_jpeg_quant_table_chroma_35                  VC8000E_REG(107,  0, 0xff)

#define vc8000e_averageqp                                   VC8000E_REG(108, 24, 0xff)
#define vc8000e_nonzerocount                                VC8000E_REG(108,  0, 0xffffff)
#define vc8000e_jpeg_quant_table_chroma_36                  VC8000E_REG(108, 24, 0xff)
#define vc8000e_jpeg_quant_table_chroma_37                  VC8000E_REG(108, 16, 0xff)
#define vc8000e_jpeg_quant_table_chroma_38                  VC8000E_REG(108,  8, 0xff)
#define vc8000e_jpeg_quant_table_chroma_39                  VC8000E_REG(108,  0, 0xff)

#define vc8000e_roimapdeltaqpaddr                           VC8000E_REG(109,  0, 0xffffffff)
#define vc8000e_jpeg_quant_table_chroma_40                  VC8000E_REG(109, 24, 0xff)
#define vc8000e_jpeg_quant_table_chroma_41                  VC8000E_REG(109, 16, 0xff)
#define vc8000e_jpeg_quant_table_chroma_42                  VC8000E_REG(109,  8, 0xff)
#define vc8000e_jpeg_quant_table_chroma_43                  VC8000E_REG(109,  0, 0xff)

#define vc8000e_roimapdeltaqpaddr_msb                       VC8000E_REG(110,  0, 0xffffffff)
#define vc8000e_jpeg_quant_table_chroma_44                  VC8000E_REG(110, 24, 0xff)
#define vc8000e_jpeg_quant_table_chroma_45                  VC8000E_REG(110, 16, 0xff)
#define vc8000e_jpeg_quant_table_chroma_46                  VC8000E_REG(110,  8, 0xff)
#define vc8000e_jpeg_quant_table_chroma_47                  VC8000E_REG(110,  0, 0xff)

#define vc8000e_intracu8num                                 VC8000E_REG(111, 12, 0xfffff)
#define vc8000e_jpeg_quant_table_chroma_48                  VC8000E_REG(111, 24, 0xff)
#define vc8000e_jpeg_quant_table_chroma_49                  VC8000E_REG(111, 16, 0xff)
#define vc8000e_jpeg_quant_table_chroma_50                  VC8000E_REG(111,  8, 0xff)
#define vc8000e_jpeg_quant_table_chroma_51                  VC8000E_REG(111,  0, 0xff)

#define vc8000e_skipcu8num                                  VC8000E_REG(112, 12, 0xfffff)
#define vc8000e_jpeg_quant_table_chroma_52                  VC8000E_REG(112, 24, 0xff)
#define vc8000e_jpeg_quant_table_chroma_53                  VC8000E_REG(112, 16, 0xff)
#define vc8000e_jpeg_quant_table_chroma_54                  VC8000E_REG(112,  8, 0xff)
#define vc8000e_jpeg_quant_table_chroma_55                  VC8000E_REG(112,  0, 0xff)

#define vc8000e_pbframe4nrdcost                             VC8000E_REG(113,  0, 0xffffffff)
#define vc8000e_jpeg_quant_table_chroma_56                  VC8000E_REG(113, 24, 0xff)
#define vc8000e_jpeg_quant_table_chroma_57                  VC8000E_REG(113, 16, 0xff)
#define vc8000e_jpeg_quant_table_chroma_58                  VC8000E_REG(113,  8, 0xff)
#define vc8000e_jpeg_quant_table_chroma_59                  VC8000E_REG(113,  0, 0xff)

#define vc8000e_colctbs_store_base                          VC8000E_REG(114,  0, 0xffffffff)
#define vc8000e_jpeg_quant_table_chroma_60                  VC8000E_REG(114, 24, 0xff)
#define vc8000e_jpeg_quant_table_chroma_61                  VC8000E_REG(114, 16, 0xff)
#define vc8000e_jpeg_quant_table_chroma_62                  VC8000E_REG(114,  8, 0xff)
#define vc8000e_jpeg_quant_table_chroma_63                  VC8000E_REG(114,  0, 0xff)

#define vc8000e_colctbs_store_base_msb                      VC8000E_REG(115,  0, 0xffffffff)

#define vc8000e_colctbs_load_base                           VC8000E_REG(116,  0, 0xffffffff)

#define vc8000e_colctbs_load_base_msb                       VC8000E_REG(117,  0, 0xffffffff)

#define vc8000e_ctbrcthrdmin                                VC8000E_REG(118, 16, 0xffff)
#define vc8000e_ctbrcthrdmax                                VC8000E_REG(118,  0, 0xffff)

#define vc8000e_ctbbitsmin                                  VC8000E_REG(119, 16, 0xffff)
#define vc8000e_ctbbitsmax                                  VC8000E_REG(119,  0, 0xffff)

#define vc8000e_totallcubits                                VC8000E_REG(120,  0, 0xffffffff)

#define vc8000e_bitsratio                                   VC8000E_REG(121,  0, 0xffffffff)

#define vc8000e_lamda_sse_me_10                             VC8000E_REG(122, 17, 0x7fff)
#define vc8000e_lamda_sse_me_11                             VC8000E_REG(122,  2, 0x7fff)
#define vc8000e_lamda_sse_me_1_expand6bit                   VC8000E_REG(122, 11, 0x1fffff)
#define vc8000e_av1_allow_intrabc                           VC8000E_REG(122, 31, 0x1)
#define vc8000e_av1_coded_lossless                          VC8000E_REG(122, 30, 0x1)
#define vc8000e_av1_delta_q_res                             VC8000E_REG(122, 26, 0xf)
#define vc8000e_av1_enable_filter_intra                     VC8000E_REG(122, 25, 0x1)
#define vc8000e_av1_tx_mode                                 VC8000E_REG(122, 23, 0x3)
#define vc8000e_av1_reduced_tx_set_used                     VC8000E_REG(122, 22, 0x1)
#define vc8000e_av1_seg_enable                              VC8000E_REG(122, 21, 0x1)
#define vc8000e_av1_allow_high_precision_mv                 VC8000E_REG(122, 20, 0x1)
#define vc8000e_av1_skip_mode_flag                          VC8000E_REG(122, 19, 0x1)
#define vc8000e_av1_reference_mode                          VC8000E_REG(122, 17, 0x3)
#define vc8000e_av1_list0_ref_frame                         VC8000E_REG(122, 13, 0xf)
#define vc8000e_av1_list1_ref_frame                         VC8000E_REG(122,  9, 0xf)
#define vc8000e_av1_enable_interintra_compound              VC8000E_REG(122,  8, 0x1)
#define vc8000e_av1_enable_dual_filter                      VC8000E_REG(122,  7, 0x1)
#define vc8000e_av1_cur_frame_force_integer_mv              VC8000E_REG(122,  6, 0x1)
#define vc8000e_av1_switchable_motion_mode                  VC8000E_REG(122,  5, 0x1)
#define vc8000e_av1_interp_filter                           VC8000E_REG(122,  2, 0x7)
#define vc8000e_av1_allow_update_cdf                        VC8000E_REG(122,  1, 0x1)
#define vc8000e_av1_enable_order_hint                       VC8000E_REG(122,  0, 0x1)

#define vc8000e_lamda_sse_me_12                             VC8000E_REG(123, 17, 0x7fff)
#define vc8000e_lamda_sse_me_13                             VC8000E_REG(123,  2, 0x7fff)
#define vc8000e_lamda_sse_me_2_expand6bit                   VC8000E_REG(123, 11, 0x1fffff)
#define vc8000e_ctbrc_qpdelta_flag_reverse                  VC8000E_REG(123,  0, 0x1)
#define vc8000e_av1_db_filter_lvl0                          VC8000E_REG(123, 26, 0x3f)
#define vc8000e_av1_db_filter_lvl1                          VC8000E_REG(123, 20, 0x3f)
#define vc8000e_av1_db_filter_lvl_u                         VC8000E_REG(123, 14, 0x3f)
#define vc8000e_av1_db_filter_lvl_v                         VC8000E_REG(123,  8, 0x3f)
#define vc8000e_av1_sharpness_lvl                           VC8000E_REG(123,  5, 0x7)
#define vc8000e_av1_primary_ref_frame                       VC8000E_REG(123,  2, 0x7)
#define vc8000e_av1_btxtypesearch                           VC8000E_REG(123,  1, 0x1)

#define vc8000e_lamda_sse_me_14                             VC8000E_REG(124, 17, 0x7fff)
#define vc8000e_lamda_sse_me_15                             VC8000E_REG(124,  2, 0x7fff)
#define vc8000e_lamda_sse_me_3_expand6bit                   VC8000E_REG(124, 11, 0x1fffff)
#define vc8000e_av1_cdef_damping                            VC8000E_REG(124, 29, 0x7)
#define vc8000e_av1_cdef_strengths                          VC8000E_REG(124, 23, 0x3f)
#define vc8000e_av1_cdef_uv_strengths                       VC8000E_REG(124, 17, 0x3f)
#define vc8000e_av1_cdef_bits                               VC8000E_REG(124, 15, 0x3)
#define vc8000e_vp9_refresh_frame_context                   VC8000E_REG(124, 14, 0x1)
#define vc8000e_last_frame_type                             VC8000E_REG(124, 13, 0x1)
#define vc8000e_vp9_segmentation_enable                     VC8000E_REG(124, 12, 0x1)
#define vc8000e_rdoq_map_enable                             VC8000E_REG(124, 11, 0x1)

#define vc8000e_intra_satd_lamda_0                          VC8000E_REG(125, 18, 0x3fff)
#define vc8000e_intra_satd_lamda_1                          VC8000E_REG(125,  4, 0x3fff)
#define vc8000e_av1_framectx_base                           VC8000E_REG(125,  0, 0xffffffff)

#define vc8000e_intra_satd_lamda_2                          VC8000E_REG(126, 18, 0x3fff)
#define vc8000e_intra_satd_lamda_3                          VC8000E_REG(126,  4, 0x3fff)
#define vc8000e_av1_framectx_base_msb                       VC8000E_REG(126,  0, 0xffffffff)

#define vc8000e_intra_satd_lamda_4                          VC8000E_REG(127, 18, 0x3fff)
#define vc8000e_intra_satd_lamda_5                          VC8000E_REG(127,  4, 0x3fff)
#define vc8000e_overlay_input_yaddr1                        VC8000E_REG(127,  0, 0xffffffff)

#define vc8000e_intra_satd_lamda_6                          VC8000E_REG(128, 18, 0x3fff)
#define vc8000e_intra_satd_lamda_7                          VC8000E_REG(128,  4, 0x3fff)
#define vc8000e_overlay_input_yaddr1_msb                    VC8000E_REG(128,  0, 0xffffffff)

#define vc8000e_intra_satd_lamda_8                          VC8000E_REG(129, 18, 0x3fff)
#define vc8000e_intra_satd_lamda_9                          VC8000E_REG(129,  4, 0x3fff)
#define vc8000e_overlay_input_uaddr1                        VC8000E_REG(129,  0, 0xffffffff)

#define vc8000e_intra_satd_lamda_10                         VC8000E_REG(130, 18, 0x3fff)
#define vc8000e_intra_satd_lamda_11                         VC8000E_REG(130,  4, 0x3fff)
#define vc8000e_overlay_input_uaddr1_msb                    VC8000E_REG(130,  0, 0xffffffff)

#define vc8000e_intra_satd_lamda_12                         VC8000E_REG(131, 18, 0x3fff)
#define vc8000e_intra_satd_lamda_13                         VC8000E_REG(131,  4, 0x3fff)
#define vc8000e_overlay_input_vaddr1                        VC8000E_REG(131,  0, 0xffffffff)

#define vc8000e_intra_satd_lamda_14                         VC8000E_REG(132, 18, 0x3fff)
#define vc8000e_intra_satd_lamda_15                         VC8000E_REG(132,  4, 0x3fff)
#define vc8000e_overlay_input_vaddr1_msb                    VC8000E_REG(132,  0, 0xffffffff)

#define vc8000e_sse_div_256                                 VC8000E_REG(133,  0, 0xffffffff)

#define vc8000e_noise_reduction_enable                      VC8000E_REG(134, 30, 0x3)
#define vc8000e_noise_low                                   VC8000E_REG(134, 24, 0x3f)
#define vc8000e_nr_mbnum_invert_reg                         VC8000E_REG(134,  0, 0xffff)

#define vc8000e_sliceqp_prev                                VC8000E_REG(135, 26, 0x3f)
#define vc8000e_thresh_sigma_cur                            VC8000E_REG(135,  5, 0x1fffff)

#define vc8000e_sigma_cur                                   VC8000E_REG(136, 16, 0xffff)
#define vc8000e_frame_sigma_calced                          VC8000E_REG(136,  0, 0xffff)

#define vc8000e_thresh_sigma_calced                         VC8000E_REG(137, 11, 0x1fffff)

#define vc8000e_lamda_sse_me_4_expand6bit                   VC8000E_REG(138, 11, 0x1fffff)
#define vc8000e_overlay_input_yaddr2                        VC8000E_REG(138,  0, 0xffffffff)

#define vc8000e_lamda_sse_me_5_expand6bit                   VC8000E_REG(139, 11, 0x1fffff)
#define vc8000e_overlay_input_yaddr2_msb                    VC8000E_REG(139,  0, 0xffffffff)

#define vc8000e_lamda_sse_me_6_expand6bit                   VC8000E_REG(140, 11, 0x1fffff)
#define vc8000e_overlay_input_uaddr2                        VC8000E_REG(140,  0, 0xffffffff)

#define vc8000e_lamda_sse_me_7_expand6bit                   VC8000E_REG(141, 11, 0x1fffff)
#define vc8000e_overlay_input_uaddr2_msb                    VC8000E_REG(141,  0, 0xffffffff)

#define vc8000e_lamda_sse_me_8_expand6bit                   VC8000E_REG(142, 11, 0x1fffff)
#define vc8000e_overlay_input_vaddr2                        VC8000E_REG(142,  0, 0xffffffff)

#define vc8000e_lamda_sse_me_9_expand6bit                   VC8000E_REG(143, 11, 0x1fffff)
#define vc8000e_overlay_input_vaddr2_msb                    VC8000E_REG(143,  0, 0xffffffff)

#define vc8000e_lamda_sse_me_10_expand6bit                  VC8000E_REG(144, 11, 0x1fffff)
#define vc8000e_overlay_input_yaddr3                        VC8000E_REG(144,  0, 0xffffffff)

#define vc8000e_lamda_sse_me_11_expand6bit                  VC8000E_REG(145, 11, 0x1fffff)
#define vc8000e_overlay_input_yaddr3_msb                    VC8000E_REG(145,  0, 0xffffffff)

#define vc8000e_lamda_sse_me_12_expand6bit                  VC8000E_REG(146, 11, 0x1fffff)
#define vc8000e_overlay_input_uaddr3                        VC8000E_REG(146,  0, 0xffffffff)

#define vc8000e_lamda_sse_me_13_expand6bit                  VC8000E_REG(147, 11, 0x1fffff)
#define vc8000e_overlay_input_uaddr3_msb                    VC8000E_REG(147,  0, 0xffffffff)

#define vc8000e_lamda_sse_me_14_expand6bit                  VC8000E_REG(148, 11, 0x1fffff)
#define vc8000e_overlay_input_vaddr3                        VC8000E_REG(148,  0, 0xffffffff)

#define vc8000e_lamda_sse_me_15_expand6bit                  VC8000E_REG(149, 11, 0x1fffff)
#define vc8000e_overlay_input_vaddr3_msb                    VC8000E_REG(149,  0, 0xffffffff)

#define vc8000e_lamda_satd_me_16                            VC8000E_REG(150, 19, 0x1fff)
#define vc8000e_lamda_satd_me_17                            VC8000E_REG(150,  6, 0x1fff)
#define vc8000e_overlay_input_yaddr4                        VC8000E_REG(150,  0, 0xffffffff)

#define vc8000e_lamda_satd_me_18                            VC8000E_REG(151, 19, 0x1fff)
#define vc8000e_lamda_satd_me_19                            VC8000E_REG(151,  6, 0x1fff)
#define vc8000e_overlay_input_yaddr4_msb                    VC8000E_REG(151,  0, 0xffffffff)

#define vc8000e_lamda_satd_me_20                            VC8000E_REG(152, 19, 0x1fff)
#define vc8000e_lamda_satd_me_21                            VC8000E_REG(152,  6, 0x1fff)
#define vc8000e_overlay_input_uaddr4                        VC8000E_REG(152,  0, 0xffffffff)

#define vc8000e_lamda_satd_me_22                            VC8000E_REG(153, 19, 0x1fff)
#define vc8000e_lamda_satd_me_23                            VC8000E_REG(153,  6, 0x1fff)
#define vc8000e_overlay_input_uaddr4_msb                    VC8000E_REG(153,  0, 0xffffffff)

#define vc8000e_lamda_satd_me_24                            VC8000E_REG(154, 19, 0x1fff)
#define vc8000e_lamda_satd_me_25                            VC8000E_REG(154,  6, 0x1fff)
#define vc8000e_overlay_input_vaddr4                        VC8000E_REG(154,  0, 0xffffffff)

#define vc8000e_lamda_satd_me_26                            VC8000E_REG(155, 19, 0x1fff)
#define vc8000e_lamda_satd_me_27                            VC8000E_REG(155,  6, 0x1fff)
#define vc8000e_overlay_input_vaddr4_msb                    VC8000E_REG(155,  0, 0xffffffff)

#define vc8000e_lamda_satd_me_28                            VC8000E_REG(156, 19, 0x1fff)
#define vc8000e_lamda_satd_me_29                            VC8000E_REG(156,  6, 0x1fff)
#define vc8000e_overlay_input_yaddr5                        VC8000E_REG(156,  0, 0xffffffff)

#define vc8000e_lamda_satd_me_30                            VC8000E_REG(157, 19, 0x1fff)
#define vc8000e_lamda_satd_me_31                            VC8000E_REG(157,  6, 0x1fff)
#define vc8000e_overlay_input_yaddr5_msb                    VC8000E_REG(157,  0, 0xffffffff)

#define vc8000e_lamda_sse_me_16                             VC8000E_REG(158, 11, 0x1fffff)
#define vc8000e_overlay_input_uaddr5                        VC8000E_REG(158,  0, 0xffffffff)

#define vc8000e_lamda_sse_me_17                             VC8000E_REG(159, 11, 0x1fffff)
#define vc8000e_overlay_input_uaddr5_msb                    VC8000E_REG(159,  0, 0xffffffff)

#define vc8000e_lamda_sse_me_18                             VC8000E_REG(160, 11, 0x1fffff)
#define vc8000e_overlay_input_vaddr5                        VC8000E_REG(160,  0, 0xffffffff)

#define vc8000e_lamda_sse_me_19                             VC8000E_REG(161, 11, 0x1fffff)
#define vc8000e_overlay_input_vaddr5_msb                    VC8000E_REG(161,  0, 0xffffffff)

#define vc8000e_lamda_sse_me_20                             VC8000E_REG(162, 11, 0x1fffff)
#define vc8000e_overlay_input_yaddr6                        VC8000E_REG(162,  0, 0xffffffff)

#define vc8000e_lamda_sse_me_21                             VC8000E_REG(163, 11, 0x1fffff)
#define vc8000e_overlay_input_yaddr6_msb                    VC8000E_REG(163,  0, 0xffffffff)

#define vc8000e_lamda_sse_me_22                             VC8000E_REG(164, 11, 0x1fffff)
#define vc8000e_overlay_input_uaddr6                        VC8000E_REG(164,  0, 0xffffffff)

#define vc8000e_lamda_sse_me_23                             VC8000E_REG(165, 11, 0x1fffff)
#define vc8000e_overlay_input_uaddr6_msb                    VC8000E_REG(165,  0, 0xffffffff)

#define vc8000e_lamda_sse_me_24                             VC8000E_REG(166, 11, 0x1fffff)
#define vc8000e_overlay_input_vaddr6                        VC8000E_REG(166,  0, 0xffffffff)

#define vc8000e_lamda_sse_me_25                             VC8000E_REG(167, 11, 0x1fffff)
#define vc8000e_overlay_input_vaddr6_msb                    VC8000E_REG(167,  0, 0xffffffff)

#define vc8000e_lamda_sse_me_26                             VC8000E_REG(168, 11, 0x1fffff)
#define vc8000e_overlay_input_yaddr7                        VC8000E_REG(168,  0, 0xffffffff)

#define vc8000e_lamda_sse_me_27                             VC8000E_REG(169, 11, 0x1fffff)
#define vc8000e_overlay_input_yaddr7_msb                    VC8000E_REG(169,  0, 0xffffffff)

#define vc8000e_lamda_sse_me_28                             VC8000E_REG(170, 11, 0x1fffff)
#define vc8000e_rc_ctbrc_sliceqpoffset                      VC8000E_REG(170,  5, 0x3f)
#define vc8000e_roi1_delta_qp_rc                            VC8000E_REG(170,  0, 0x1f)
#define vc8000e_lambda_depth                                VC8000E_REG(170, 31, 0x1)
#define vc8000e_sse_qp_factor                               VC8000E_REG(170,  8, 0x7fff)
#define vc8000e_roi1_qp_value                               VC8000E_REG(170,  1, 0x7f)
#define vc8000e_roi1_qp_type                                VC8000E_REG(170,  0, 0x1)
#define     VC8000E_ROI_QP_TYPE_ABS                         1

#define vc8000e_lamda_sse_me_29                             VC8000E_REG(171, 11, 0x1fffff)
#define vc8000e_roi2_delta_qp_rc                            VC8000E_REG(171,  0, 0x1f)
#define vc8000e_sad_qp_factor                               VC8000E_REG(171,  8, 0x7fff)
#define vc8000e_roi2_qp_value                               VC8000E_REG(171,  1, 0x7f)
#define vc8000e_roi2_qp_type                                VC8000E_REG(171,  0, 0x1)

#define vc8000e_lamda_sse_me_30                             VC8000E_REG(172, 11, 0x1fffff)
#define vc8000e_qp_min                                      VC8000E_REG(172,  5, 0x3f)
#define vc8000e_complexity_offset                           VC8000E_REG(172,  0, 0x1f)

#define vc8000e_lamda_sse_me_31                             VC8000E_REG(173, 11, 0x1fffff)
#define vc8000e_qp_max                                      VC8000E_REG(173,  5, 0x3f)
#define vc8000e_rc_qpdelta_range                            VC8000E_REG(173,  0, 0xf)

#define vc8000e_intra_satd_lamda_16                         VC8000E_REG(174, 18, 0x3fff)
#define vc8000e_intra_satd_lamda_17                         VC8000E_REG(174,  4, 0x3fff)
#define vc8000e_overlay_input_uaddr7                        VC8000E_REG(174,  0, 0xffffffff)

#define vc8000e_intra_satd_lamda_18                         VC8000E_REG(175, 18, 0x3fff)
#define vc8000e_intra_satd_lamda_19                         VC8000E_REG(175,  4, 0x3fff)
#define vc8000e_overlay_input_uaddr7_msb                    VC8000E_REG(175,  0, 0xffffffff)

#define vc8000e_intra_satd_lamda_20                         VC8000E_REG(176, 18, 0x3fff)
#define vc8000e_intra_satd_lamda_21                         VC8000E_REG(176,  4, 0x3fff)
#define vc8000e_overlay_input_vaddr7                        VC8000E_REG(176,  0, 0xffffffff)

#define vc8000e_intra_satd_lamda_22                         VC8000E_REG(177, 18, 0x3fff)
#define vc8000e_intra_satd_lamda_23                         VC8000E_REG(177,  4, 0x3fff)
#define vc8000e_overlay_input_vaddr7_msb                    VC8000E_REG(177,  0, 0xffffffff)

#define vc8000e_intra_satd_lamda_24                         VC8000E_REG(178, 18, 0x3fff)
#define vc8000e_intra_satd_lamda_25                         VC8000E_REG(178,  4, 0x3fff)
#define vc8000e_overlay_input_yaddr8                        VC8000E_REG(178,  0, 0xffffffff)

#define vc8000e_intra_satd_lamda_26                         VC8000E_REG(179, 18, 0x3fff)
#define vc8000e_intra_satd_lamda_27                         VC8000E_REG(179,  4, 0x3fff)
#define vc8000e_overlay_input_yaddr8_msb                    VC8000E_REG(179,  0, 0xffffffff)

#define vc8000e_intra_satd_lamda_28                         VC8000E_REG(180, 18, 0x3fff)
#define vc8000e_intra_satd_lamda_29                         VC8000E_REG(180,  4, 0x3fff)
#define vc8000e_overlay_input_uaddr8                        VC8000E_REG(180,  0, 0xffffffff)

#define vc8000e_intra_satd_lamda_30                         VC8000E_REG(181, 18, 0x3fff)
#define vc8000e_intra_satd_lamda_31                         VC8000E_REG(181,  4, 0x3fff)
#define vc8000e_av1_plane_rd_mult_intra_lum                 VC8000E_REG(181, 27, 0x1f)
#define vc8000e_av1_plane_rd_mult_intra_chr                 VC8000E_REG(181, 22, 0x1f)
#define vc8000e_av1_plane_rd_mult_inter_lum                 VC8000E_REG(181, 17, 0x1f)
#define vc8000e_av1_plane_rd_mult_inter_chr                 VC8000E_REG(181, 12, 0x1f)
#define vc8000e_rc_block_size                               VC8000E_REG(181,  2, 0x3)

#define vc8000e_qp_fractional                               VC8000E_REG(182, 16, 0xffff)
#define vc8000e_qp_delta_gain                               VC8000E_REG(182,  0, 0xffff)

#define vc8000e_qp_sum                                      VC8000E_REG(183,  6, 0x3ffffff)

#define vc8000e_qp_num                                      VC8000E_REG(184, 12, 0xfffff)

#define vc8000e_pic_complexity                              VC8000E_REG(185,  9, 0x7fffff)
#define vc8000e_timeout_cycles_msb                          VC8000E_REG(185,  0, 0x1ff)

#define vc8000e_cu_information_table_base                   VC8000E_REG(186,  0, 0xffffffff)

#define vc8000e_cu_information_table_base_msb               VC8000E_REG(187,  0, 0xffffffff)

#define vc8000e_cu_information_base                         VC8000E_REG(188,  0, 0xffffffff)

#define vc8000e_cu_information_base_msb                     VC8000E_REG(189,  0, 0xffffffff)

#define vc8000e_num_long_term_pics                          VC8000E_REG(190, 30, 0x3)
#define vc8000e_nal_ref_idc_2bit                            VC8000E_REG(190, 26, 0x3)

#define vc8000e_nal_unit_type                               VC8000E_REG(191, 26, 0x3f)
#define vc8000e_nuh_temporal_id                             VC8000E_REG(191, 23, 0x7)
#define vc8000e_pps_id                                      VC8000E_REG(191, 17, 0x3f)
#define vc8000e_prefixnal_svc_ext                           VC8000E_REG(191, 16, 0x1)
#define vc8000e_obu_extension_flag                          VC8000E_REG(191, 16, 0x1)
#define vc8000e_slice_header_size                           VC8000E_REG(191,  0, 0xffff)

#define vc8000e_framenum                                    VC8000E_REG(192,  0, 0xffffffff)

#define vc8000e_l0_delta_framenum0                          VC8000E_REG(193, 21, 0x7ff)
#define vc8000e_l0_used_by_next_pic0                        VC8000E_REG(193, 20, 0x1)
#define vc8000e_l0_delta_framenum1                          VC8000E_REG(193,  9, 0x7ff)
#define vc8000e_l0_used_by_next_pic1                        VC8000E_REG(193,  8, 0x1)
#define vc8000e_xfill_msb                                   VC8000E_REG(193,  6, 0x3)
#define vc8000e_yfill_msb                                   VC8000E_REG(193,  4, 0x3)
#define vc8000e_nal_ref_idc                                 VC8000E_REG(193,  3, 0x1)
#define vc8000e_idr_pic_id                                  VC8000E_REG(193,  2, 0x1)
#define vc8000e_transform8x8_enable                         VC8000E_REG(193,  1, 0x1)
#define vc8000e_entropy_coding_mode                         VC8000E_REG(193,  0, 0x1)

#define vc8000e_l1_delta_framenum0                          VC8000E_REG(194, 21, 0x7ff)
#define vc8000e_l1_used_by_next_pic0                        VC8000E_REG(194, 20, 0x1)
#define vc8000e_l1_delta_framenum1                          VC8000E_REG(194,  9, 0x7ff)
#define vc8000e_l1_used_by_next_pic1                        VC8000E_REG(194,  8, 0x1)
#define vc8000e_max_longtermidx_plus1                       VC8000E_REG(194,  5, 0x7)
#define vc8000e_cur_longtermidx                             VC8000E_REG(194,  2, 0x7)

#define vc8000e_encoded_ctb_number_msb                      VC8000E_REG(195, 28, 0xf)
#define vc8000e_num_slices_ready_msb                        VC8000E_REG(195, 26, 0x3)
#define vc8000e_slice_size_msb                              VC8000E_REG(195, 24, 0x3)
#define vc8000e_cir_start_msb                               VC8000E_REG(195, 20, 0xf)
#define vc8000e_cir_interval_msb                            VC8000E_REG(195, 16, 0xf)
#define vc8000e_intra_area_left_msb                         VC8000E_REG(195, 15, 0x1)
#define vc8000e_intra_area_right_msb                        VC8000E_REG(195, 14, 0x1)
#define vc8000e_intra_area_top_msb                          VC8000E_REG(195, 13, 0x1)
#define vc8000e_intra_area_bottom_msb                       VC8000E_REG(195, 12, 0x1)
#define vc8000e_roi1_left_msb                               VC8000E_REG(195, 11, 0x1)
#define vc8000e_roi1_right_msb                              VC8000E_REG(195, 10, 0x1)
#define vc8000e_roi1_top_msb                                VC8000E_REG(195,  9, 0x1)
#define vc8000e_roi1_bottom_msb                             VC8000E_REG(195,  8, 0x1)
#define vc8000e_roi2_left_msb                               VC8000E_REG(195,  7, 0x1)
#define vc8000e_roi2_right_msb                              VC8000E_REG(195,  6, 0x1)
#define vc8000e_roi2_top_msb                                VC8000E_REG(195,  5, 0x1)
#define vc8000e_roi2_bottom_msb                             VC8000E_REG(195,  4, 0x1)
#define vc8000e_pic_width_msb                               VC8000E_REG(195,  2, 0x3)
#define vc8000e_ref_ringbuf_en                              VC8000E_REG(195,  1, 0x1)

#define vc8000e_low_latency_hw_sync_en                      VC8000E_REG(196, 31, 0x1)
#define vc8000e_low_latency_en                              VC8000E_REG(196, 30, 0x1)
#define vc8000e_input_buf_loopback_en                       VC8000E_REG(196, 29, 0x1)
#define vc8000e_num_ctb_rows_per_sync                       VC8000E_REG(196, 20, 0x1ff)
#define vc8000e_ctb_row_rd_ptr                              VC8000E_REG(196, 10, 0x3ff)
#define vc8000e_ctb_row_wr_ptr                              VC8000E_REG(196,  0, 0x3ff)

#define vc8000e_l0_delta_poc0_msb                           VC8000E_REG(197, 22, 0x3ff)
#define vc8000e_l0_delta_poc1_msb                           VC8000E_REG(197, 12, 0x3ff)
#define vc8000e_l1_delta_poc0_msb                           VC8000E_REG(197,  2, 0x3ff)
#define vc8000e_ctb_row_wr_ptr_jpeg_msb                     VC8000E_REG(197,  5, 0x1f)
#define vc8000e_ctb_row_rd_ptr_jpeg_msb                     VC8000E_REG(197,  0, 0x1f)

#define vc8000e_l1_delta_poc1_msb                           VC8000E_REG(198, 22, 0x3ff)
#define vc8000e_l0_delta_framenum0_msb                      VC8000E_REG(198, 13, 0x1ff)
#define vc8000e_mark_current_longterm                       VC8000E_REG(198, 12, 0x1)
#define vc8000e_l0_longtermidx0                             VC8000E_REG(198,  9, 0x7)
#define vc8000e_l0_longtermidx1                             VC8000E_REG(198,  6, 0x7)
#define vc8000e_l1_longtermidx0                             VC8000E_REG(198,  3, 0x7)
#define vc8000e_l1_longtermidx1                             VC8000E_REG(198,  0, 0x7)

#define vc8000e_l0_delta_framenum1_msb                      VC8000E_REG(199, 23, 0x1ff)
#define vc8000e_l1_delta_framenum0_msb                      VC8000E_REG(199, 14, 0x1ff)
#define vc8000e_l1_delta_framenum1_msb                      VC8000E_REG(199,  5, 0x1ff)
#define vc8000e_hash_type                                   VC8000E_REG(199,  3, 0x3)
#define vc8000e_hash_offset                                 VC8000E_REG(199,  1, 0x3)
#define vc8000e_osd_alphablend_enable                       VC8000E_REG(199,  0, 0x1)

#define vc8000e_hash_val                                    VC8000E_REG(200,  0, 0xffffffff)

#define vc8000e_mean_thr0                                   VC8000E_REG(201, 24, 0xff)
#define vc8000e_mean_thr1                                   VC8000E_REG(201, 16, 0xff)
#define vc8000e_mean_thr2                                   VC8000E_REG(201,  8, 0xff)
#define vc8000e_mean_thr3                                   VC8000E_REG(201,  0, 0xff)

#define vc8000e_thr_dc_lum_8x8                              VC8000E_REG(202, 16, 0xffff)
#define vc8000e_thr_dc_chroma_8x8                           VC8000E_REG(202,  0, 0xffff)

#define vc8000e_thr_dc_lum_16x16                            VC8000E_REG(203, 16, 0xffff)
#define vc8000e_lum_dc_sum_thr                              VC8000E_REG(203, 24, 0xff)
#define vc8000e_thr_dc_chroma_16x16                         VC8000E_REG(203,  0, 0xffff)
#define vc8000e_cb_dc_sum_thr                               VC8000E_REG(203,  8, 0xff)
#define vc8000e_cr_dc_sum_thr                               VC8000E_REG(203,  0, 0xff)

#define vc8000e_thr_dc_lum_32x32                            VC8000E_REG(204, 16, 0xffff)
#define vc8000e_thr_dc_chroma_32x32                         VC8000E_REG(204,  0, 0xffff)

#define vc8000e_thr_ac_num_lum_8x8                          VC8000E_REG(205, 16, 0xffff)
#define vc8000e_thr_ac_num_chroma_8x8                       VC8000E_REG(205,  0, 0xffff)

#define vc8000e_thr_ac_num_lum_16x16                        VC8000E_REG(206, 16, 0xffff)
#define vc8000e_thr_ac_num_chroma_16x16                     VC8000E_REG(206,  0, 0xffff)

#define vc8000e_thr_ac_num_lum_32x32                        VC8000E_REG(207, 16, 0xffff)
#define vc8000e_thr_ac_num_chroma_32x32                     VC8000E_REG(207,  0, 0xffff)

#define vc8000e_mdqpy                                       VC8000E_REG(208, 26, 0x3f)
#define vc8000e_smart_qp                                    VC8000E_REG(208, 26, 0x3f)
#define vc8000e_mdqpc                                       VC8000E_REG(208, 20, 0x3f)
#define vc8000e_foreground_pixel_thx                        VC8000E_REG(208, 14, 0x3f)
#define vc8000e_enable_smart                                VC8000E_REG(208, 13, 0x1)
#define vc8000e_ipcm1_left                                  VC8000E_REG(208,  4, 0x1ff)
#define vc8000e_skip_map_enable                             VC8000E_REG(208,  3, 0x1)
#define vc8000e_lambda_cost_scale_4n                        VC8000E_REG(208,  0, 0x7)

#define vc8000e_ipcm1_right                                 VC8000E_REG(209, 23, 0x1ff)
#define vc8000e_ipcm1_top                                   VC8000E_REG(209, 14, 0x1ff)
#define vc8000e_ipcm1_bottom                                VC8000E_REG(209,  5, 0x1ff)
#define vc8000e_pcm_filter_disable                          VC8000E_REG(209,  4, 0x1)
#define vc8000e_ipcm_map_enable                             VC8000E_REG(209,  3, 0x1)
#define vc8000e_lambda_cost_scale_2n                        VC8000E_REG(209,  0, 0x7)

#define vc8000e_input_lu_stride                             VC8000E_REG(210, 12, 0xfffff)
#define vc8000e_ipcm2_left                                  VC8000E_REG(210,  3, 0x1ff)
#define vc8000e_lambda_cost_scale_1n                        VC8000E_REG(210,  0, 0x7)

#define vc8000e_input_ch_stride                             VC8000E_REG(211, 12, 0xfffff)
#define vc8000e_ipcm2_right                                 VC8000E_REG(211,  3, 0x1ff)

#define vc8000e_ref_lu_stride                               VC8000E_REG(212, 12, 0xfffff)
#define vc8000e_ipcm2_top                                   VC8000E_REG(212,  3, 0x1ff)

#define vc8000e_ref_ds_lu_stride                            VC8000E_REG(213, 14, 0x3ffff)
#define vc8000e_ipcm2_bottom                                VC8000E_REG(213,  5, 0x1ff)

#define vc8000e_hwljpegsupport                              VC8000E_REG(214, 31, 0x1)
#define vc8000e_hwabsqpsupport                              VC8000E_REG(214, 30, 0x1)
#define vc8000e_hwintratu32support                          VC8000E_REG(214, 29, 0x1)
#define vc8000e_hwroimapversion                             VC8000E_REG(214, 26, 0x7)
#define vc8000e_hwmaxvideowidthh264                         VC8000E_REG(214, 13, 0x1fff)
#define vc8000e_hwmaxvideowidthjpeg                         VC8000E_REG(214,  0, 0x1fff)

#define vc8000e_totalarlen                                  VC8000E_REG(215,  0, 0xffffffff)

#define vc8000e_totalr                                      VC8000E_REG(216,  0, 0xffffffff)

#define vc8000e_totalar                                     VC8000E_REG(217,  0, 0xffffffff)

#define vc8000e_totalrlast                                  VC8000E_REG(218,  0, 0xffffffff)

#define vc8000e_totalawlen                                  VC8000E_REG(219,  0, 0xffffffff)

#define vc8000e_totalw                                      VC8000E_REG(220,  0, 0xffffffff)

#define vc8000e_totalaw                                     VC8000E_REG(221,  0, 0xffffffff)

#define vc8000e_totalwlast                                  VC8000E_REG(222,  0, 0xffffffff)

#define vc8000e_totalb                                      VC8000E_REG(223,  0, 0xffffffff)

#define vc8000e_chroma_const_en                             VC8000E_REG(224, 31, 0x1)
#define vc8000e_psnr_en                                     VC8000E_REG(224, 22, 0x1)
#define vc8000e_ssim_en                                     VC8000E_REG(224, 21, 0x1)
#define vc8000e_skipframe_en                                VC8000E_REG(224, 20, 0x1)
#define vc8000e_cr_const_pixel                              VC8000E_REG(224, 10, 0x3ff)
#define vc8000e_cb_const_pixel                              VC8000E_REG(224,  0, 0x3ff)

#define vc8000e_num_tile_columns                            VC8000E_REG(225, 24, 0xff)
#define vc8000e_num_tile_rows                               VC8000E_REG(225, 16, 0xff)
#define vc8000e_tiles_enabled_flag                          VC8000E_REG(225, 15, 0x1)
#define vc8000e_loop_filter_across_tiles_enabled_flag       VC8000E_REG(225, 14, 0x1)
#define vc8000e_roimap_cuctrl_index_enable                  VC8000E_REG(225, 13, 0x1)
#define vc8000e_roimap_cuctrl_enable                        VC8000E_REG(225, 12, 0x1)
#define vc8000e_roimap_cuctrl_ver                           VC8000E_REG(225,  9, 0x7)
#define vc8000e_roimap_qpdelta_ver                          VC8000E_REG(225,  6, 0x7)

#define vc8000e_hwssimsupport                               VC8000E_REG(226, 31, 0x1)
#define vc8000e_hwp010refsupport                            VC8000E_REG(226, 30, 0x1)
#define vc8000e_hwcuinforversion                            VC8000E_REG(226, 27, 0x7)
#define vc8000e_me_vert_searchrange_hevc                    VC8000E_REG(226, 21, 0x3f)
#define vc8000e_me_vert_searchrange_h264                    VC8000E_REG(226, 15, 0x3f)
#define vc8000e_hwctbrcversion                              VC8000E_REG(226, 12, 0x7)
#define vc8000e_hwjpeg422support                            VC8000E_REG(226, 11, 0x1)
#define vc8000e_hwgmvsupport                                VC8000E_REG(226, 10, 0x1)
#define vc8000e_hwroi8support                               VC8000E_REG(226,  9, 0x1)
#define vc8000e_bframe_me4n_hor_searchrange                 VC8000E_REG(226,  7, 0x3)
#define vc8000e_hwrdoqsupport                               VC8000E_REG(226,  6, 0x1)
#define vc8000e_hwmultipasssupport                          VC8000E_REG(226,  5, 0x1)
#define vc8000e_hwinloopdsratio                             VC8000E_REG(226,  4, 0x1)
#define vc8000e_hwstreambufchain                            VC8000E_REG(226,  3, 0x1)
#define vc8000e_hwstreamsegmentsupport                      VC8000E_REG(226,  2, 0x1)
#define vc8000e_hwiframeonly                                VC8000E_REG(226,  1, 0x1)
#define vc8000e_hwdynamicmaxtusize                          VC8000E_REG(226,  0, 0x1)

#define vc8000e_ssim_y_numerator_lsb                        VC8000E_REG(227,  0, 0xffffffff)

#define vc8000e_ssim_y_numerator_msb                        VC8000E_REG(228,  0, 0xffffffff)

#define vc8000e_ssim_u_numerator_lsb                        VC8000E_REG(229,  0, 0xffffffff)

#define vc8000e_ssim_u_numerator_msb                        VC8000E_REG(230,  0, 0xffffffff)

#define vc8000e_ssim_v_numerator_lsb                        VC8000E_REG(231,  0, 0xffffffff)

#define vc8000e_ssim_v_numerator_msb                        VC8000E_REG(232,  0, 0xffffffff)

#define vc8000e_ssim_y_denominator                          VC8000E_REG(233,  0, 0xffffffff)

#define vc8000e_ssim_uv_denominator                         VC8000E_REG(234,  0, 0xffffffff)

#define vc8000e_rps_delta_poc_0                             VC8000E_REG(235, 22, 0x3ff)
#define vc8000e_rps_delta_poc_1                             VC8000E_REG(235, 12, 0x3ff)
#define vc8000e_rps_delta_poc_2                             VC8000E_REG(235,  2, 0x3ff)
#define vc8000e_rps_used_by_cur_0                           VC8000E_REG(235,  1, 0x1)
#define vc8000e_rps_used_by_cur_1                           VC8000E_REG(235,  0, 0x1)

#define vc8000e_rps_delta_poc_3                             VC8000E_REG(236, 22, 0x3ff)
#define vc8000e_rps_used_by_cur_2                           VC8000E_REG(236, 21, 0x1)
#define vc8000e_rps_used_by_cur_3                           VC8000E_REG(236, 20, 0x1)
#define vc8000e_rps_neg_pic_num                             VC8000E_REG(236, 17, 0x7)
#define vc8000e_rps_pos_pic_num                             VC8000E_REG(236, 14, 0x7)
#define vc8000e_short_term_ref_pic_set_sps_flag             VC8000E_REG(236, 13, 0x1)
#define vc8000e_p010_ref_enable                             VC8000E_REG(236, 12, 0x1)

#define vc8000e_ref_ch_stride                               VC8000E_REG(237, 12, 0xfffff)
#define vc8000e_dummyreaden                                 VC8000E_REG(237, 11, 0x1)
#define vc8000e_prpsbi_id_1                                 VC8000E_REG(237,  6, 0x1f)
#define vc8000e_prpsbi_id_2                                 VC8000E_REG(237,  1, 0x1f)
#define vc8000e_sps_tmvp_enable                             VC8000E_REG(237,  0, 0x1)

#define vc8000e_dummyreadaddr                               VC8000E_REG(238,  0, 0xffffffff)

#define vc8000e_current_ctb_mad_base                        VC8000E_REG(239,  0, 0xffffffff)

#define vc8000e_current_ctb_mad_base_msb                    VC8000E_REG(240,  0, 0xffffffff)

#define vc8000e_previous_ctb_mad_base                       VC8000E_REG(241,  0, 0xffffffff)

#define vc8000e_previous_ctb_mad_base_msb                   VC8000E_REG(242,  0, 0xffffffff)

#define vc8000e_ctb_rc_model_param0                         VC8000E_REG(243, 11, 0x1fffff)
#define vc8000e_tileleftstart                               VC8000E_REG(243,  1, 0x3ff)
#define vc8000e_tilemvconstraint                            VC8000E_REG(243,  0, 0x1)

#define vc8000e_ctb_rc_model_param1                         VC8000E_REG(244, 10, 0x3fffff)
#define vc8000e_roi3_qp_value                               VC8000E_REG(244,  3, 0x7f)
#define vc8000e_roi3_qp_type                                VC8000E_REG(244,  2, 0x1)

#define vc8000e_ctb_rc_model_param_min                      VC8000E_REG(245, 18, 0x3fff)
#define vc8000e_ctb_rc_row_factor                           VC8000E_REG(245,  2, 0xffff)
#define vc8000e_rc_qpdelta_range_msb                        VC8000E_REG(245,  0, 0x3)

#define vc8000e_ctb_rc_qp_step                              VC8000E_REG(246, 14, 0x3ffff)
#define vc8000e_axi_write_outstanding_num                   VC8000E_REG(246,  6, 0xff)
#define vc8000e_ctb_rc_delay                                VC8000E_REG(246,  3, 0x7)

#define vc8000e_prev_pic_lum_mad                            VC8000E_REG(247,  6, 0x3ffffff)
#define vc8000e_tilestrmsizealignmentexp                    VC8000E_REG(247,  2, 0xf)
#define vc8000e_ctb_rc_prev_mad_valid                       VC8000E_REG(247,  1, 0x1)

#define vc8000e_ctb_qp_sum_for_rc                           VC8000E_REG(248,  8, 0xffffff)
#define vc8000e_roi4_qp_value                               VC8000E_REG(248,  1, 0x7f)
#define vc8000e_roi4_qp_type                                VC8000E_REG(248,  0, 0x1)

#define vc8000e_encoded_ctb_number_msb2                     VC8000E_REG(249, 30, 0x3)
#define vc8000e_num_slices_ready_msb2                       VC8000E_REG(249, 29, 0x1)
#define vc8000e_slice_size_msb2                             VC8000E_REG(249, 28, 0x1)
#define vc8000e_cir_start_msb2                              VC8000E_REG(249, 26, 0x3)
#define vc8000e_cir_interval_msb2                           VC8000E_REG(249, 24, 0x3)
#define vc8000e_intra_area_left_msb2                        VC8000E_REG(249, 23, 0x1)
#define vc8000e_intra_area_right_msb2                       VC8000E_REG(249, 22, 0x1)
#define vc8000e_intra_area_top_msb2                         VC8000E_REG(249, 21, 0x1)
#define vc8000e_intra_area_bottom_msb2                      VC8000E_REG(249, 20, 0x1)
#define vc8000e_roi1_left_msb2                              VC8000E_REG(249, 19, 0x1)
#define vc8000e_roi1_right_msb2                             VC8000E_REG(249, 18, 0x1)
#define vc8000e_roi1_top_msb2                               VC8000E_REG(249, 17, 0x1)
#define vc8000e_roi1_bottom_msb2                            VC8000E_REG(249, 16, 0x1)
#define vc8000e_roi2_left_msb2                              VC8000E_REG(249, 15, 0x1)
#define vc8000e_roi2_right_msb2                             VC8000E_REG(249, 14, 0x1)
#define vc8000e_roi2_top_msb2                               VC8000E_REG(249, 13, 0x1)
#define vc8000e_roi2_bottom_msb2                            VC8000E_REG(249, 12, 0x1)
#define vc8000e_pic_width_msb2                              VC8000E_REG(249, 11, 0x1)
#define vc8000e_ipcm1_left_msb                              VC8000E_REG(249, 10, 0x1)
#define vc8000e_ipcm1_right_msb                             VC8000E_REG(249,  9, 0x1)
#define vc8000e_ipcm1_top_msb                               VC8000E_REG(249,  8, 0x1)
#define vc8000e_ipcm1_bottom_msb                            VC8000E_REG(249,  7, 0x1)
#define vc8000e_ipcm2_left_msb                              VC8000E_REG(249,  6, 0x1)
#define vc8000e_ipcm2_right_msb                             VC8000E_REG(249,  5, 0x1)
#define vc8000e_ipcm2_top_msb                               VC8000E_REG(249,  4, 0x1)
#define vc8000e_ipcm2_bottom_msb                            VC8000E_REG(249,  3, 0x1)
#define vc8000e_jpeg_pic_width_msb                          VC8000E_REG(249,  9, 0x3)
#define vc8000e_jpeg_pic_height_msb                         VC8000E_REG(249,  7, 0x3)
#define vc8000e_jpeg_rowlength_msb                          VC8000E_REG(249,  5, 0x3)

#define vc8000e_global_horizontal_mv_l0                     VC8000E_REG(250, 18, 0x3fff)
#define vc8000e_global_vertical_mv_l0                       VC8000E_REG(250,  4, 0x3fff)
#define vc8000e_tilesyncreadalignexp                        VC8000E_REG(250,  0, 0xf)

#define vc8000e_global_horizontal_mv_l1                     VC8000E_REG(251, 18, 0x3fff)
#define vc8000e_global_vertical_mv_l1                       VC8000E_REG(251,  4, 0x3fff)
#define vc8000e_tilesyncwritealignexp                       VC8000E_REG(251,  0, 0xf)

#define vc8000e_roi3_left                                   VC8000E_REG(252, 22, 0x3ff)
#define vc8000e_roi3_top                                    VC8000E_REG(252, 12, 0x3ff)
#define vc8000e_roi3_right                                  VC8000E_REG(252,  2, 0x3ff)

#define vc8000e_roi3_bottom                                 VC8000E_REG(253, 22, 0x3ff)
#define vc8000e_roi4_left                                   VC8000E_REG(253, 12, 0x3ff)
#define vc8000e_roi4_top                                    VC8000E_REG(253,  2, 0x3ff)

#define vc8000e_roi4_right                                  VC8000E_REG(254, 22, 0x3ff)
#define vc8000e_roi4_bottom                                 VC8000E_REG(254, 12, 0x3ff)
#define vc8000e_roi5_left                                   VC8000E_REG(254,  2, 0x3ff)

#define vc8000e_roi5_top                                    VC8000E_REG(255, 22, 0x3ff)
#define vc8000e_roi5_right                                  VC8000E_REG(255, 12, 0x3ff)
#define vc8000e_roi5_bottom                                 VC8000E_REG(255,  2, 0x3ff)

#define vc8000e_roi6_left                                   VC8000E_REG(256, 22, 0x3ff)
#define vc8000e_roi6_top                                    VC8000E_REG(256, 12, 0x3ff)
#define vc8000e_roi6_right                                  VC8000E_REG(256,  2, 0x3ff)

#define vc8000e_roi6_bottom                                 VC8000E_REG(257, 22, 0x3ff)
#define vc8000e_roi7_left                                   VC8000E_REG(257, 12, 0x3ff)
#define vc8000e_roi7_top                                    VC8000E_REG(257,  2, 0x3ff)

#define vc8000e_roi7_right                                  VC8000E_REG(258, 22, 0x3ff)
#define vc8000e_roi7_bottom                                 VC8000E_REG(258, 12, 0x3ff)
#define vc8000e_roi8_left                                   VC8000E_REG(258,  2, 0x3ff)

#define vc8000e_roi8_top                                    VC8000E_REG(259, 22, 0x3ff)
#define vc8000e_roi8_right                                  VC8000E_REG(259, 12, 0x3ff)
#define vc8000e_roi8_bottom                                 VC8000E_REG(259,  2, 0x3ff)
#define vc8000e_current_max_tu_size_decrease                VC8000E_REG(259,  1, 0x1)

#define vc8000e_roi8_qp_value                               VC8000E_REG(260, 25, 0x7f)
#define vc8000e_roi8_qp_type                                VC8000E_REG(260, 24, 0x1)
#define vc8000e_roi7_qp_value                               VC8000E_REG(260, 17, 0x7f)
#define vc8000e_roi7_qp_type                                VC8000E_REG(260, 16, 0x1)
#define vc8000e_roi6_qp_value                               VC8000E_REG(260,  9, 0x7f)
#define vc8000e_roi6_qp_type                                VC8000E_REG(260,  8, 0x1)
#define vc8000e_roi5_qp_value                               VC8000E_REG(260,  1, 0x7f)
#define vc8000e_roi5_qp_type                                VC8000E_REG(260,  0, 0x1)

#define vc8000e_tilewidthin8                                VC8000E_REG(261, 19, 0x1fff)
#define vc8000e_dynamic_rdo_enable                          VC8000E_REG(261, 18, 0x1)
#define vc8000e_rgblumaoffset                               VC8000E_REG(261, 13, 0x1f)
#define vc8000e_prp_in_loop_ds_ratio                        VC8000E_REG(261, 12, 0x1)
#define vc8000e_axi_read_outstanding_num                    VC8000E_REG(261,  4, 0xff)
#define vc8000e_multi_core_en                               VC8000E_REG(261,  3, 0x1)
#define vc8000e_rdoq_enable                                 VC8000E_REG(261,  2, 0x1)
#define vc8000e_pass1_skip_cabac                            VC8000E_REG(261,  1, 0x1)
#define vc8000e_motion_score_enable                         VC8000E_REG(261,  0, 0x1)

#define vc8000e_lum_sse_div_256                             VC8000E_REG(262,  0, 0xffffffff)

#define vc8000e_cb_sse_div_64                               VC8000E_REG(263,  0, 0xffffffff)

#define vc8000e_cr_sse_div_64                               VC8000E_REG(264,  0, 0xffffffff)

#define vc8000e_ref_ready_threshold                         VC8000E_REG(265, 16, 0xffff)
#define vc8000e_ddr_polling_interval                        VC8000E_REG(265,  0, 0xffff)

#define vc8000e_multicore_sync_l0_addr                      VC8000E_REG(266,  0, 0xffffffff)

#define vc8000e_multicore_sync_l0_addr_msb                  VC8000E_REG(267,  0, 0xffffffff)

#define vc8000e_multicore_sync_l1_addr                      VC8000E_REG(268,  0, 0xffffffff)

#define vc8000e_multicore_sync_l1_addr_msb                  VC8000E_REG(269,  0, 0xffffffff)

#define vc8000e_multicore_sync_rec_addr                     VC8000E_REG(270,  0, 0xffffffff)

#define vc8000e_multicore_sync_rec_addr_msb                 VC8000E_REG(271,  0, 0xffffffff)

#define vc8000e_rd_urgent_enable_threshold                  VC8000E_REG(272, 24, 0xff)
#define vc8000e_rd_urgent_disable_threshold                 VC8000E_REG(272, 16, 0xff)
#define vc8000e_wr_urgent_enable_threshold                  VC8000E_REG(272,  8, 0xff)
#define vc8000e_wr_urgent_disable_threshold                 VC8000E_REG(272,  0, 0xff)
#define     VC8000E_URGENT_THR_DISABLE                      255

#define vc8000e_roimap_cuctrl_index_addr                    VC8000E_REG(273,  0, 0xffffffff)

#define vc8000e_roimap_cuctrl_index_addr_msb                VC8000E_REG(274,  0, 0xffffffff)

#define vc8000e_roimap_cuctrl_addr                          VC8000E_REG(275,  0, 0xffffffff)

#define vc8000e_roimap_cuctrl_addr_msb                      VC8000E_REG(276,  0, 0xffffffff)

#define vc8000e_log2_max_pic_order_cnt_lsb                  VC8000E_REG(277, 27, 0x1f)
#define vc8000e_log2_max_frame_num                          VC8000E_REG(277, 22, 0x1f)
#define vc8000e_pic_order_cnt_type                          VC8000E_REG(277, 20, 0x3)
#define vc8000e_syn_amount_per_loopback                     VC8000E_REG(277,  5, 0x7fff)
#define vc8000e_prpsbi_id_0                                 VC8000E_REG(277,  0, 0x1f)

#define vc8000e_output_strm_buf1_base                       VC8000E_REG(278,  0, 0xffffffff)

#define vc8000e_output_strm_buf1_base_msb                   VC8000E_REG(279,  0, 0xffffffff)

#define vc8000e_output_strm_buffer1_limit                   VC8000E_REG(280,  0, 0xffffffff)

#define vc8000e_strm_segment_sw_sync_en                     VC8000E_REG(281, 31, 0x1)
#define vc8000e_strm_segment_en                             VC8000E_REG(281, 30, 0x1)
#define vc8000e_strm_segment_rd_ptr                         VC8000E_REG(281, 20, 0x3ff)
#define vc8000e_strm_segment_wr_ptr                         VC8000E_REG(281, 10, 0x3ff)
#define vc8000e_num_ctb_rows_per_sync_msb                   VC8000E_REG(281,  4, 0x3f)
#define vc8000e_chroma_format_idc                           VC8000E_REG(281,  2, 0x3)
#define     VC8000E_CHROMA_FORMAT_IDC_420		    1
#define vc8000e_write_rec_to_ddr                            VC8000E_REG(281,  1, 0x1)

#define vc8000e_strm_segment_size                           VC8000E_REG(282,  0, 0xffffffff)

#define vc8000e_motion_score_l0_0                           VC8000E_REG(283,  0, 0xffffffff)

#define vc8000e_motion_score_l0_1                           VC8000E_REG(284,  0, 0xffffffff)

#define vc8000e_motion_score_l1_0                           VC8000E_REG(285,  0, 0xffffffff)

#define vc8000e_motion_score_l1_1                           VC8000E_REG(286,  0, 0xffffffff)

#define vc8000e_hwvideoheightext                            VC8000E_REG(287, 31, 0x1)
#define vc8000e_hwcscextensionsupport                       VC8000E_REG(287, 30, 0x1)
#define vc8000e_hwscaler420support                          VC8000E_REG(287, 29, 0x1)
#define vc8000e_hwcutreesupport                             VC8000E_REG(287, 28, 0x1)
#define vc8000e_hwctbrcmoremode                             VC8000E_REG(287, 23, 0x1)
#define vc8000e_hwmevertrangeprogramable                    VC8000E_REG(287, 22, 0x1)
#define vc8000e_hwmonochromesupport                         VC8000E_REG(287, 21, 0x1)
#define vc8000e_extsramsupport                              VC8000E_REG(287, 20, 0x1)
#define vc8000e_hwvideostabsupport                          VC8000E_REG(287, 19, 0x1)
#define vc8000e_hwh264rdoqsupport                           VC8000E_REG(287, 18, 0x1)
#define vc8000e_hwdisablerecwtsupport                       VC8000E_REG(287, 17, 0x1)
#define vc8000e_hwosdsupport                                VC8000E_REG(287, 16, 0x1)
#define vc8000e_hwh264nalrefidc_2bit                        VC8000E_REG(287, 15, 0x1)
#define vc8000e_hwdynamicrdosupport                         VC8000E_REG(287, 14, 0x1)
#define vc8000e_hwav1support                                VC8000E_REG(287, 13, 0x1)
#define vc8000e_hwmaxvideowidthav1                          VC8000E_REG(287,  0, 0x1fff)

#define vc8000e_intracu8num_msb                             VC8000E_REG(288, 29, 0x7)
#define vc8000e_skipcu8num_msb                              VC8000E_REG(288, 26, 0x7)
#define vc8000e_qp_sum_msb                                  VC8000E_REG(288, 24, 0x3)
#define vc8000e_qp_num_msb                                  VC8000E_REG(288, 21, 0x7)
#define vc8000e_pic_complexity_msb                          VC8000E_REG(288, 17, 0xf)
#define vc8000e_ctb_qp_sum_for_rc_msb                       VC8000E_REG(288, 15, 0x3)
#define vc8000e_cuinfoversion                               VC8000E_REG(288, 12, 0x7)
#define vc8000e_prpsbi_wrseginitnum                         VC8000E_REG(288,  0, 0x3ff)

#define vc8000e_rgbcoeffg                                   VC8000E_REG(289, 16, 0xffff)
#define vc8000e_rgbcoeffh                                   VC8000E_REG(289,  0, 0xffff)

#define vc8000e_totalarlen2                                 VC8000E_REG(290,  0, 0xffffffff)

#define vc8000e_totalr2                                     VC8000E_REG(291,  0, 0xffffffff)

#define vc8000e_totalar2                                    VC8000E_REG(292,  0, 0xffffffff)

#define vc8000e_totalrlast2                                 VC8000E_REG(293,  0, 0xffffffff)

#define vc8000e_totalawlen2                                 VC8000E_REG(294,  0, 0xffffffff)

#define vc8000e_totalw2                                     VC8000E_REG(295,  0, 0xffffffff)

#define vc8000e_totalaw2                                    VC8000E_REG(296,  0, 0xffffffff)

#define vc8000e_totalwlast2                                 VC8000E_REG(297,  0, 0xffffffff)

#define vc8000e_totalb2                                     VC8000E_REG(298,  0, 0xffffffff)

#define vc8000e_ext_sram_lum_fwd_base                       VC8000E_REG(299,  0, 0xffffffff)

#define vc8000e_ext_sram_lum_fwd_base_msb                   VC8000E_REG(300,  0, 0xffffffff)

#define vc8000e_ext_sram_lum_bwd_base                       VC8000E_REG(301,  0, 0xffffffff)

#define vc8000e_ext_sram_lum_bwd_base_msb                   VC8000E_REG(302,  0, 0xffffffff)

#define vc8000e_ext_sram_chr_fwd_base                       VC8000E_REG(303,  0, 0xffffffff)

#define vc8000e_ext_sram_chr_fwd_base_msb                   VC8000E_REG(304,  0, 0xffffffff)

#define vc8000e_ext_sram_chr_bwd_base                       VC8000E_REG(305,  0, 0xffffffff)

#define vc8000e_ext_sram_chr_bwd_base_msb                   VC8000E_REG(306,  0, 0xffffffff)

#define vc8000e_extlinebuffer_linecnt_lum_fwd               VC8000E_REG(307, 24, 0xff)
#define vc8000e_extlinebuffer_linecnt_lum_bwd               VC8000E_REG(307, 16, 0xff)
#define vc8000e_extlinebuffer_linecnt_chr_fwd               VC8000E_REG(307,  8, 0xff)
#define vc8000e_extlinebuffer_linecnt_chr_bwd               VC8000E_REG(307,  0, 0xff)

#define vc8000e_axi_strm_write_pending                      VC8000E_REG(308,  0, 0xffffffff)

#define vc8000e_axi_recon_write_pending                     VC8000E_REG(309,  0, 0xffffffff)

#define vc8000e_axi_rec4n_write_pending                     VC8000E_REG(310,  0, 0xffffffff)

#define vc8000e_axi_prp_read_pending                        VC8000E_REG(311,  0, 0xffffffff)

#define vc8000e_axi_ref_read_pending                        VC8000E_REG(312,  0, 0xffffffff)

#define vc8000e_axi_ref4n_read_pending                      VC8000E_REG(313,  0, 0xffffffff)

#define vc8000e_axi_rcroi_read_pending                      VC8000E_REG(314,  0, 0xffffffff)

#define vc8000e_axi_read_channel_pending                    VC8000E_REG(315,  0, 0xffffffff)

#define vc8000e_axi_write_channel_pending                   VC8000E_REG(316,  0, 0xffffffff)

#define vc8000e_axi_total_pending                           VC8000E_REG(317,  0, 0xffffffff)

#define vc8000e_hw_debug                                    VC8000E_REG(318,  0, 0xffffffff)

#define vc8000e_axi_burst_align_fuse_wr_common              VC8000E_REG(319, 28, 0xf)
#define vc8000e_axi_burst_align_fuse_wr_stream              VC8000E_REG(319, 24, 0xf)
#define vc8000e_axi_burst_align_fuse_wr_chroma_ref          VC8000E_REG(319, 20, 0xf)
#define vc8000e_axi_burst_align_fuse_wr_luma_ref            VC8000E_REG(319, 16, 0xf)
#define vc8000e_axi_burst_align_fuse_rd_common              VC8000E_REG(319, 12, 0xf)
#define vc8000e_axi_burst_align_fuse_rd_prp                 VC8000E_REG(319,  8, 0xf)
#define vc8000e_axi_burst_align_fuse_rd_ch_ref_prefetch     VC8000E_REG(319,  4, 0xf)
#define vc8000e_axi_burst_align_fuse_rd_lu_ref_prefetch     VC8000E_REG(319,  0, 0xf)

#define vc8000e_axi_burst_align_wr_common                   VC8000E_REG(320, 28, 0xf)
#define vc8000e_axi_burst_align_wr_stream                   VC8000E_REG(320, 24, 0xf)
#define vc8000e_axi_burst_align_wr_chroma_ref               VC8000E_REG(320, 20, 0xf)
#define vc8000e_axi_burst_align_wr_luma_ref                 VC8000E_REG(320, 16, 0xf)
#define vc8000e_axi_burst_align_rd_common                   VC8000E_REG(320, 12, 0xf)
#define vc8000e_axi_burst_align_rd_prp                      VC8000E_REG(320,  8, 0xf)
#define vc8000e_axi_burst_align_rd_ch_ref_prefetch          VC8000E_REG(320,  4, 0xf)
#define vc8000e_axi_burst_align_rd_lu_ref_prefetch          VC8000E_REG(320,  0, 0xf)

#define vc8000e_me_assigned_vert_search_range               VC8000E_REG(321, 26, 0x3f)

#define vc8000e_overlay_input_uaddr8_msb                    VC8000E_REG(322,  0, 0xffffffff)

#define vc8000e_overlay_input_vaddr8                        VC8000E_REG(323,  0, 0xffffffff)

#define vc8000e_overlay_input_vaddr8_msb                    VC8000E_REG(324,  0, 0xffffffff)

#define vc8000e_overlay_yoffset1                            VC8000E_REG(325, 16, 0xffff)
#define vc8000e_overlay_xoffset1                            VC8000E_REG(325,  0, 0xffff)

#define vc8000e_overlay_yoffset2                            VC8000E_REG(326, 16, 0xffff)
#define vc8000e_overlay_xoffset2                            VC8000E_REG(326,  0, 0xffff)

#define vc8000e_overlay_yoffset3                            VC8000E_REG(327, 16, 0xffff)
#define vc8000e_overlay_xoffset3                            VC8000E_REG(327,  0, 0xffff)

#define vc8000e_overlay_yoffset4                            VC8000E_REG(328, 16, 0xffff)
#define vc8000e_overlay_xoffset4                            VC8000E_REG(328,  0, 0xffff)

#define vc8000e_overlay_yoffset5                            VC8000E_REG(329, 16, 0xffff)
#define vc8000e_overlay_xoffset5                            VC8000E_REG(329,  0, 0xffff)

#define vc8000e_overlay_yoffset6                            VC8000E_REG(330, 16, 0xffff)
#define vc8000e_overlay_xoffset6                            VC8000E_REG(330,  0, 0xffff)

#define vc8000e_overlay_yoffset7                            VC8000E_REG(331, 16, 0xffff)
#define vc8000e_overlay_xoffset7                            VC8000E_REG(331,  0, 0xffff)

#define vc8000e_overlay_yoffset8                            VC8000E_REG(332, 16, 0xffff)
#define vc8000e_overlay_xoffset8                            VC8000E_REG(332,  0, 0xffff)

#define vc8000e_overlay_height1                             VC8000E_REG(333, 16, 0xffff)
#define vc8000e_overlay_width1                              VC8000E_REG(333,  0, 0xffff)

#define vc8000e_overlay_height2                             VC8000E_REG(334, 16, 0xffff)
#define vc8000e_overlay_width2                              VC8000E_REG(334,  0, 0xffff)

#define vc8000e_overlay_height3                             VC8000E_REG(335, 16, 0xffff)
#define vc8000e_overlay_width3                              VC8000E_REG(335,  0, 0xffff)

#define vc8000e_overlay_height4                             VC8000E_REG(336, 16, 0xffff)
#define vc8000e_overlay_width4                              VC8000E_REG(336,  0, 0xffff)

#define vc8000e_overlay_height5                             VC8000E_REG(337, 16, 0xffff)
#define vc8000e_overlay_width5                              VC8000E_REG(337,  0, 0xffff)

#define vc8000e_overlay_height6                             VC8000E_REG(338, 16, 0xffff)
#define vc8000e_overlay_width6                              VC8000E_REG(338,  0, 0xffff)

#define vc8000e_overlay_height7                             VC8000E_REG(339, 16, 0xffff)
#define vc8000e_overlay_width7                              VC8000E_REG(339,  0, 0xffff)

#define vc8000e_overlay_height8                             VC8000E_REG(340, 16, 0xffff)
#define vc8000e_overlay_width8                              VC8000E_REG(340,  0, 0xffff)

#define vc8000e_vp9_segment1_skip                           VC8000E_REG(341, 31, 0x1)
#define vc8000e_overlay_ystride1                            VC8000E_REG(341, 11, 0xfffff)
#define vc8000e_overlay_alpha1                              VC8000E_REG(341,  3, 0xff)
#define vc8000e_overlay_format1                             VC8000E_REG(341,  1, 0x3)
#define vc8000e_overlay_enable1                             VC8000E_REG(341,  0, 0x1)

#define vc8000e_vp9_segment2_skip                           VC8000E_REG(342, 31, 0x1)
#define vc8000e_overlay_ystride2                            VC8000E_REG(342, 11, 0xfffff)
#define vc8000e_overlay_alpha2                              VC8000E_REG(342,  3, 0xff)
#define vc8000e_overlay_format2                             VC8000E_REG(342,  1, 0x3)
#define vc8000e_overlay_enable2                             VC8000E_REG(342,  0, 0x1)

#define vc8000e_vp9_segment3_skip                           VC8000E_REG(343, 31, 0x1)
#define vc8000e_overlay_ystride3                            VC8000E_REG(343, 11, 0xfffff)
#define vc8000e_overlay_alpha3                              VC8000E_REG(343,  3, 0xff)
#define vc8000e_overlay_format3                             VC8000E_REG(343,  1, 0x3)
#define vc8000e_overlay_enable3                             VC8000E_REG(343,  0, 0x1)

#define vc8000e_vp9_segment4_skip                           VC8000E_REG(344, 31, 0x1)
#define vc8000e_overlay_ystride4                            VC8000E_REG(344, 11, 0xfffff)
#define vc8000e_overlay_alpha4                              VC8000E_REG(344,  3, 0xff)
#define vc8000e_overlay_format4                             VC8000E_REG(344,  1, 0x3)
#define vc8000e_overlay_enable4                             VC8000E_REG(344,  0, 0x1)

#define vc8000e_vp9_segment5_skip                           VC8000E_REG(345, 31, 0x1)
#define vc8000e_overlay_ystride5                            VC8000E_REG(345, 11, 0xfffff)
#define vc8000e_overlay_alpha5                              VC8000E_REG(345,  3, 0xff)
#define vc8000e_overlay_format5                             VC8000E_REG(345,  1, 0x3)
#define vc8000e_overlay_enable5                             VC8000E_REG(345,  0, 0x1)

#define vc8000e_vp9_segment6_skip                           VC8000E_REG(346, 31, 0x1)
#define vc8000e_overlay_ystride6                            VC8000E_REG(346, 11, 0xfffff)
#define vc8000e_overlay_alpha6                              VC8000E_REG(346,  3, 0xff)
#define vc8000e_overlay_format6                             VC8000E_REG(346,  1, 0x3)
#define vc8000e_overlay_enable6                             VC8000E_REG(346,  0, 0x1)

#define vc8000e_vp9_segment7_skip                           VC8000E_REG(347, 31, 0x1)
#define vc8000e_overlay_ystride7                            VC8000E_REG(347, 11, 0xfffff)
#define vc8000e_overlay_alpha7                              VC8000E_REG(347,  3, 0xff)
#define vc8000e_overlay_format7                             VC8000E_REG(347,  1, 0x3)
#define vc8000e_overlay_enable7                             VC8000E_REG(347,  0, 0x1)

#define vc8000e_vp9_segment8_skip                           VC8000E_REG(348, 31, 0x1)
#define vc8000e_overlay_ystride8                            VC8000E_REG(348, 11, 0xfffff)
#define vc8000e_overlay_alpha8                              VC8000E_REG(348,  3, 0xff)
#define vc8000e_overlay_format8                             VC8000E_REG(348,  1, 0x3)
#define vc8000e_overlay_enable8                             VC8000E_REG(348,  0, 0x1)

#define vc8000e_sbi_wait_frame_start                        VC8000E_REG(349, 31, 0x1)
#define vc8000e_sbi_out_of_sync                             VC8000E_REG(349, 30, 0x1)
#define vc8000e_sbi_timeout                                 VC8000E_REG(349, 29, 0x1)
#define vc8000e_write_tmv_info_ddr                          VC8000E_REG(349, 28, 0x1)
#define vc8000e_tmvp_enable                                 VC8000E_REG(349, 27, 0x1)
#define vc8000e_vp9_segment1_qp                             VC8000E_REG(349, 20, 0x7f)
#define vc8000e_overlay_uvstride1                           VC8000E_REG(349,  0, 0xfffff)

#define vc8000e_starttileidx                                VC8000E_REG(350, 27, 0x1f)
#define vc8000e_vp9_segment2_qp                             VC8000E_REG(350, 20, 0x7f)
#define vc8000e_overlay_uvstride2                           VC8000E_REG(350,  0, 0xfffff)

#define vc8000e_vp9_segment3_qp                             VC8000E_REG(351, 20, 0x7f)
#define vc8000e_overlay_uvstride3                           VC8000E_REG(351,  0, 0xfffff)

#define vc8000e_vp9_segment4_qp                             VC8000E_REG(352, 20, 0x7f)
#define vc8000e_overlay_uvstride4                           VC8000E_REG(352,  0, 0xfffff)

#define vc8000e_vp9_segment5_qp                             VC8000E_REG(353, 20, 0x7f)
#define vc8000e_overlay_uvstride5                           VC8000E_REG(353,  0, 0xfffff)

#define vc8000e_vp9_segment6_qp                             VC8000E_REG(354, 20, 0x7f)
#define vc8000e_overlay_uvstride6                           VC8000E_REG(354,  0, 0xfffff)

#define vc8000e_vp9_segment7_qp                             VC8000E_REG(355, 20, 0x7f)
#define vc8000e_overlay_uvstride7                           VC8000E_REG(355,  0, 0xfffff)

#define vc8000e_vp9_segment8_qp                             VC8000E_REG(356, 23, 0x7f)
#define vc8000e_vp9_segmentation_abs                        VC8000E_REG(356, 22, 0x1)
#define vc8000e_overlay_supertile                           VC8000E_REG(356, 20, 0x3)
#define vc8000e_overlay_uvstride8                           VC8000E_REG(356,  0, 0xfffff)

#define vc8000e_rdo_check_chroma_zero_tu                    VC8000E_REG(357, 31, 0x1)
#define vc8000e_vp9_segment1_lf                             VC8000E_REG(357, 24, 0x7f)
#define vc8000e_overlay_bitmapy1                            VC8000E_REG(357, 16, 0xff)
#define vc8000e_overlay_bitmapu1                            VC8000E_REG(357,  8, 0xff)
#define vc8000e_overlay_bitmapv1                            VC8000E_REG(357,  0, 0xff)

#define vc8000e_vp9_segment2_lf                             VC8000E_REG(358, 24, 0x7f)
#define vc8000e_overlay_bitmapy2                            VC8000E_REG(358, 16, 0xff)
#define vc8000e_overlay_bitmapu2                            VC8000E_REG(358,  8, 0xff)
#define vc8000e_overlay_bitmapv2                            VC8000E_REG(358,  0, 0xff)

#define vc8000e_vp9_segment3_lf                             VC8000E_REG(359, 24, 0x7f)
#define vc8000e_overlay_bitmapy3                            VC8000E_REG(359, 16, 0xff)
#define vc8000e_overlay_bitmapu3                            VC8000E_REG(359,  8, 0xff)
#define vc8000e_overlay_bitmapv3                            VC8000E_REG(359,  0, 0xff)

#define vc8000e_vp9_segment4_lf                             VC8000E_REG(360, 24, 0x7f)
#define vc8000e_overlay_bitmapy4                            VC8000E_REG(360, 16, 0xff)
#define vc8000e_overlay_bitmapu4                            VC8000E_REG(360,  8, 0xff)
#define vc8000e_overlay_bitmapv4                            VC8000E_REG(360,  0, 0xff)

#define vc8000e_vp9_segment5_lf                             VC8000E_REG(361, 24, 0x7f)
#define vc8000e_overlay_bitmapy5                            VC8000E_REG(361, 16, 0xff)
#define vc8000e_overlay_bitmapu5                            VC8000E_REG(361,  8, 0xff)
#define vc8000e_overlay_bitmapv5                            VC8000E_REG(361,  0, 0xff)

#define vc8000e_vp9_segment6_lf                             VC8000E_REG(362, 24, 0x7f)
#define vc8000e_overlay_bitmapy6                            VC8000E_REG(362, 16, 0xff)
#define vc8000e_overlay_bitmapu6                            VC8000E_REG(362,  8, 0xff)
#define vc8000e_overlay_bitmapv6                            VC8000E_REG(362,  0, 0xff)

#define vc8000e_vp9_segment7_lf                             VC8000E_REG(363, 24, 0x7f)
#define vc8000e_overlay_bitmapy7                            VC8000E_REG(363, 16, 0xff)
#define vc8000e_overlay_bitmapu7                            VC8000E_REG(363,  8, 0xff)
#define vc8000e_overlay_bitmapv7                            VC8000E_REG(363,  0, 0xff)

#define vc8000e_vp9_segment8_lf                             VC8000E_REG(364, 24, 0x7f)
#define vc8000e_overlay_bitmapy8                            VC8000E_REG(364, 16, 0xff)
#define vc8000e_overlay_bitmapu8                            VC8000E_REG(364,  8, 0xff)
#define vc8000e_overlay_bitmapv8                            VC8000E_REG(364,  0, 0xff)

#define vc8000e_stab_next_luma_base                         VC8000E_REG(365,  0, 0xffffffff)

#define vc8000e_stab_next_luma_base_msb                     VC8000E_REG(366,  0, 0xffffffff)

#define vc8000e_stab_mode                                   VC8000E_REG(367, 30, 0x3)
#define vc8000e_stab_minimum                                VC8000E_REG(367,  0, 0x3ffffff)

#define vc8000e_stab_motion_sum                             VC8000E_REG(368,  0, 0xffffffff)

#define vc8000e_stab_gmvx                                   VC8000E_REG(369, 26, 0x3f)
#define vc8000e_stab_matrix1                                VC8000E_REG(369,  0, 0x3ffffff)

#define vc8000e_stab_gmvy                                   VC8000E_REG(370, 26, 0x3f)
#define vc8000e_stab_matrix2                                VC8000E_REG(370,  0, 0x3ffffff)

#define vc8000e_stab_matrix3                                VC8000E_REG(371,  0, 0x3ffffff)

#define vc8000e_stab_matrix4                                VC8000E_REG(372,  0, 0x3ffffff)

#define vc8000e_stab_matrix5                                VC8000E_REG(373,  0, 0x3ffffff)

#define vc8000e_stab_matrix6                                VC8000E_REG(374,  0, 0x3ffffff)

#define vc8000e_stab_matrix7                                VC8000E_REG(375,  0, 0x3ffffff)

#define vc8000e_stab_matrix8                                VC8000E_REG(376,  0, 0x3ffffff)

#define vc8000e_axi_burst_align_wr_cuinfo                   VC8000E_REG(377, 28, 0xf)
#define vc8000e_stab_matrix9                                VC8000E_REG(377,  0, 0x3ffffff)

#define vc8000e_ddrpollinginterval                          VC8000E_REG(378, 16, 0xffff)
#define vc8000e_cuinfo_stride                               VC8000E_REG(378,  0, 0xffff)

#define vc8000e_av1_cdef_strengths6                         VC8000E_REG(379, 24, 0x3f)
#define vc8000e_av1_cdef_strengths5                         VC8000E_REG(379, 18, 0x3f)
#define vc8000e_av1_cdef_strengths4                         VC8000E_REG(379, 12, 0x3f)
#define vc8000e_av1_cdef_strengths3                         VC8000E_REG(379,  6, 0x3f)
#define vc8000e_av1_cdef_strengths2                         VC8000E_REG(379,  0, 0x3f)

#define vc8000e_av1_cdef_uv_strengths4                      VC8000E_REG(380, 24, 0x3f)
#define vc8000e_av1_cdef_uv_strengths3                      VC8000E_REG(380, 18, 0x3f)
#define vc8000e_av1_cdef_uv_strengths2                      VC8000E_REG(380, 12, 0x3f)
#define vc8000e_av1_cdef_strengths8                         VC8000E_REG(380,  6, 0x3f)
#define vc8000e_av1_cdef_strengths7                         VC8000E_REG(380,  0, 0x3f)

#define vc8000e_av1_cdef_uv_strengths8                      VC8000E_REG(381, 18, 0x3f)
#define vc8000e_av1_cdef_uv_strengths7                      VC8000E_REG(381, 12, 0x3f)
#define vc8000e_av1_cdef_uv_strengths6                      VC8000E_REG(381,  6, 0x3f)
#define vc8000e_av1_cdef_uv_strengths5                      VC8000E_REG(381,  0, 0x3f)

#define vc8000e_av1_qp2qindex0                              VC8000E_REG(382, 24, 0xff)
#define vc8000e_av1_qp2qindex1                              VC8000E_REG(382, 16, 0xff)
#define vc8000e_av1_qp2qindex2                              VC8000E_REG(382,  8, 0xff)
#define vc8000e_av1_qp2qindex3                              VC8000E_REG(382,  0, 0xff)

#define vc8000e_av1_qp2qindex4                              VC8000E_REG(383, 24, 0xff)
#define vc8000e_av1_qp2qindex5                              VC8000E_REG(383, 16, 0xff)
#define vc8000e_av1_qp2qindex6                              VC8000E_REG(383,  8, 0xff)
#define vc8000e_av1_qp2qindex7                              VC8000E_REG(383,  0, 0xff)

#define vc8000e_av1_qp2qindex8                              VC8000E_REG(384, 24, 0xff)
#define vc8000e_av1_qp2qindex9                              VC8000E_REG(384, 16, 0xff)
#define vc8000e_av1_qp2qindex10                             VC8000E_REG(384,  8, 0xff)
#define vc8000e_av1_qp2qindex11                             VC8000E_REG(384,  0, 0xff)

#define vc8000e_av1_qp2qindex12                             VC8000E_REG(385, 24, 0xff)
#define vc8000e_av1_qp2qindex13                             VC8000E_REG(385, 16, 0xff)
#define vc8000e_av1_qp2qindex14                             VC8000E_REG(385,  8, 0xff)
#define vc8000e_av1_qp2qindex15                             VC8000E_REG(385,  0, 0xff)

#define vc8000e_av1_qp2qindex16                             VC8000E_REG(386, 24, 0xff)
#define vc8000e_av1_qp2qindex17                             VC8000E_REG(386, 16, 0xff)
#define vc8000e_av1_qp2qindex18                             VC8000E_REG(386,  8, 0xff)
#define vc8000e_av1_qp2qindex19                             VC8000E_REG(386,  0, 0xff)

#define vc8000e_av1_qp2qindex20                             VC8000E_REG(387, 24, 0xff)
#define vc8000e_av1_qp2qindex21                             VC8000E_REG(387, 16, 0xff)
#define vc8000e_av1_qp2qindex22                             VC8000E_REG(387,  8, 0xff)
#define vc8000e_av1_qp2qindex23                             VC8000E_REG(387,  0, 0xff)

#define vc8000e_av1_qp2qindex24                             VC8000E_REG(388, 24, 0xff)
#define vc8000e_av1_qp2qindex25                             VC8000E_REG(388, 16, 0xff)
#define vc8000e_av1_qp2qindex26                             VC8000E_REG(388,  8, 0xff)
#define vc8000e_av1_qp2qindex27                             VC8000E_REG(388,  0, 0xff)

#define vc8000e_av1_qp2qindex28                             VC8000E_REG(389, 24, 0xff)
#define vc8000e_av1_qp2qindex29                             VC8000E_REG(389, 16, 0xff)
#define vc8000e_av1_qp2qindex30                             VC8000E_REG(389,  8, 0xff)
#define vc8000e_av1_qp2qindex31                             VC8000E_REG(389,  0, 0xff)

#define vc8000e_av1_qp2qindex32                             VC8000E_REG(390, 24, 0xff)
#define vc8000e_av1_qp2qindex33                             VC8000E_REG(390, 16, 0xff)
#define vc8000e_av1_qp2qindex34                             VC8000E_REG(390,  8, 0xff)
#define vc8000e_av1_qp2qindex35                             VC8000E_REG(390,  0, 0xff)

#define vc8000e_av1_qp2qindex36                             VC8000E_REG(391, 24, 0xff)
#define vc8000e_av1_qp2qindex37                             VC8000E_REG(391, 16, 0xff)
#define vc8000e_av1_qp2qindex38                             VC8000E_REG(391,  8, 0xff)
#define vc8000e_av1_qp2qindex39                             VC8000E_REG(391,  0, 0xff)

#define vc8000e_av1_qp2qindex40                             VC8000E_REG(392, 24, 0xff)
#define vc8000e_av1_qp2qindex41                             VC8000E_REG(392, 16, 0xff)
#define vc8000e_av1_qp2qindex42                             VC8000E_REG(392,  8, 0xff)
#define vc8000e_av1_qp2qindex43                             VC8000E_REG(392,  0, 0xff)

#define vc8000e_av1_qp2qindex44                             VC8000E_REG(393, 24, 0xff)
#define vc8000e_av1_qp2qindex45                             VC8000E_REG(393, 16, 0xff)
#define vc8000e_av1_qp2qindex46                             VC8000E_REG(393,  8, 0xff)
#define vc8000e_av1_qp2qindex47                             VC8000E_REG(393,  0, 0xff)

#define vc8000e_av1_qp2qindex48                             VC8000E_REG(394, 24, 0xff)
#define vc8000e_av1_qp2qindex49                             VC8000E_REG(394, 16, 0xff)
#define vc8000e_av1_qp2qindex50                             VC8000E_REG(394,  8, 0xff)
#define vc8000e_av1_qp2qindex51                             VC8000E_REG(394,  0, 0xff)

#define vc8000e_jpeg_roimap_addr                            VC8000E_REG(395,  0, 0xffffffff)

#define vc8000e_jpeg_roimap_addr_msb                        VC8000E_REG(396,  0, 0xffffffff)

#define vc8000e_jpeg_roi_enable                             VC8000E_REG(397, 31, 0x1)

#define vc8000e_jpeg_roimap_qptablenonroi_value0            VC8000E_REG(398, 24, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value1            VC8000E_REG(398, 16, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value2            VC8000E_REG(398,  8, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value3            VC8000E_REG(398,  0, 0xff)
#define vc8000e_ipcm3_left                                  VC8000E_REG(398, 22, 0x3ff)
#define vc8000e_ipcm3_right                                 VC8000E_REG(398,  6, 0x3ff)

#define vc8000e_jpeg_roimap_qptablenonroi_value4            VC8000E_REG(399, 24, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value5            VC8000E_REG(399, 16, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value6            VC8000E_REG(399,  8, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value7            VC8000E_REG(399,  0, 0xff)
#define vc8000e_ipcm3_top                                   VC8000E_REG(399, 22, 0x3ff)
#define vc8000e_ipcm3_bottom                                VC8000E_REG(399,  6, 0x3ff)

#define vc8000e_jpeg_roimap_qptablenonroi_value8            VC8000E_REG(400, 24, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value9            VC8000E_REG(400, 16, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value10           VC8000E_REG(400,  8, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value11           VC8000E_REG(400,  0, 0xff)
#define vc8000e_ipcm4_left                                  VC8000E_REG(400, 22, 0x3ff)
#define vc8000e_ipcm4_right                                 VC8000E_REG(400,  6, 0x3ff)

#define vc8000e_jpeg_roimap_qptablenonroi_value12           VC8000E_REG(401, 24, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value13           VC8000E_REG(401, 16, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value14           VC8000E_REG(401,  8, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value15           VC8000E_REG(401,  0, 0xff)
#define vc8000e_ipcm4_top                                   VC8000E_REG(401, 22, 0x3ff)
#define vc8000e_ipcm4_bottom                                VC8000E_REG(401,  6, 0x3ff)

#define vc8000e_jpeg_roimap_qptablenonroi_value16           VC8000E_REG(402, 24, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value17           VC8000E_REG(402, 16, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value18           VC8000E_REG(402,  8, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value19           VC8000E_REG(402,  0, 0xff)
#define vc8000e_ipcm5_left                                  VC8000E_REG(402, 22, 0x3ff)
#define vc8000e_ipcm5_right                                 VC8000E_REG(402,  6, 0x3ff)

#define vc8000e_jpeg_roimap_qptablenonroi_value20           VC8000E_REG(403, 24, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value21           VC8000E_REG(403, 16, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value22           VC8000E_REG(403,  8, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value23           VC8000E_REG(403,  0, 0xff)
#define vc8000e_ipcm5_top                                   VC8000E_REG(403, 22, 0x3ff)
#define vc8000e_ipcm5_bottom                                VC8000E_REG(403,  6, 0x3ff)

#define vc8000e_jpeg_roimap_qptablenonroi_value24           VC8000E_REG(404, 24, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value25           VC8000E_REG(404, 16, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value26           VC8000E_REG(404,  8, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value27           VC8000E_REG(404,  0, 0xff)
#define vc8000e_ipcm6_left                                  VC8000E_REG(404, 22, 0x3ff)
#define vc8000e_ipcm6_right                                 VC8000E_REG(404,  6, 0x3ff)

#define vc8000e_jpeg_roimap_qptablenonroi_value28           VC8000E_REG(405, 24, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value29           VC8000E_REG(405, 16, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value30           VC8000E_REG(405,  8, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value31           VC8000E_REG(405,  0, 0xff)
#define vc8000e_ipcm6_top                                   VC8000E_REG(405, 22, 0x3ff)
#define vc8000e_ipcm6_bottom                                VC8000E_REG(405,  6, 0x3ff)

#define vc8000e_jpeg_roimap_qptablenonroi_value32           VC8000E_REG(406, 24, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value33           VC8000E_REG(406, 16, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value34           VC8000E_REG(406,  8, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value35           VC8000E_REG(406,  0, 0xff)
#define vc8000e_ipcm7_left                                  VC8000E_REG(406, 22, 0x3ff)
#define vc8000e_ipcm7_right                                 VC8000E_REG(406,  6, 0x3ff)

#define vc8000e_jpeg_roimap_qptablenonroi_value36           VC8000E_REG(407, 24, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value37           VC8000E_REG(407, 16, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value38           VC8000E_REG(407,  8, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value39           VC8000E_REG(407,  0, 0xff)
#define vc8000e_ipcm7_top                                   VC8000E_REG(407, 22, 0x3ff)
#define vc8000e_ipcm7_bottom                                VC8000E_REG(407,  6, 0x3ff)

#define vc8000e_jpeg_roimap_qptablenonroi_value40           VC8000E_REG(408, 24, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value41           VC8000E_REG(408, 16, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value42           VC8000E_REG(408,  8, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value43           VC8000E_REG(408,  0, 0xff)
#define vc8000e_ipcm8_left                                  VC8000E_REG(408, 22, 0x3ff)
#define vc8000e_ipcm8_right                                 VC8000E_REG(408,  6, 0x3ff)

#define vc8000e_jpeg_roimap_qptablenonroi_value44           VC8000E_REG(409, 24, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value45           VC8000E_REG(409, 16, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value46           VC8000E_REG(409,  8, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value47           VC8000E_REG(409,  0, 0xff)
#define vc8000e_ipcm8_top                                   VC8000E_REG(409, 22, 0x3ff)
#define vc8000e_ipcm8_bottom                                VC8000E_REG(409,  6, 0x3ff)

#define vc8000e_jpeg_roimap_qptablenonroi_value48           VC8000E_REG(410, 24, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value49           VC8000E_REG(410, 16, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value50           VC8000E_REG(410,  8, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value51           VC8000E_REG(410,  0, 0xff)

#define vc8000e_jpeg_roimap_qptablenonroi_value52           VC8000E_REG(411, 24, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value53           VC8000E_REG(411, 16, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value54           VC8000E_REG(411,  8, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value55           VC8000E_REG(411,  0, 0xff)

#define vc8000e_jpeg_roimap_qptablenonroi_value56           VC8000E_REG(412, 24, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value57           VC8000E_REG(412, 16, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value58           VC8000E_REG(412,  8, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value59           VC8000E_REG(412,  0, 0xff)

#define vc8000e_jpeg_roimap_qptablenonroi_value60           VC8000E_REG(413, 24, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value61           VC8000E_REG(413, 16, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value62           VC8000E_REG(413,  8, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value63           VC8000E_REG(413,  0, 0xff)

#define vc8000e_jpeg_roimap_qptablenonroi_value64           VC8000E_REG(414, 24, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value65           VC8000E_REG(414, 16, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value66           VC8000E_REG(414,  8, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value67           VC8000E_REG(414,  0, 0xff)

#define vc8000e_jpeg_roimap_qptablenonroi_value68           VC8000E_REG(415, 24, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value69           VC8000E_REG(415, 16, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value70           VC8000E_REG(415,  8, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value71           VC8000E_REG(415,  0, 0xff)

#define vc8000e_jpeg_roimap_qptablenonroi_value72           VC8000E_REG(416, 24, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value73           VC8000E_REG(416, 16, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value74           VC8000E_REG(416,  8, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value75           VC8000E_REG(416,  0, 0xff)

#define vc8000e_jpeg_roimap_qptablenonroi_value76           VC8000E_REG(417, 24, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value77           VC8000E_REG(417, 16, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value78           VC8000E_REG(417,  8, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value79           VC8000E_REG(417,  0, 0xff)

#define vc8000e_jpeg_roimap_qptablenonroi_value80           VC8000E_REG(418, 24, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value81           VC8000E_REG(418, 16, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value82           VC8000E_REG(418,  8, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value83           VC8000E_REG(418,  0, 0xff)

#define vc8000e_jpeg_roimap_qptablenonroi_value84           VC8000E_REG(419, 24, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value85           VC8000E_REG(419, 16, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value86           VC8000E_REG(419,  8, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value87           VC8000E_REG(419,  0, 0xff)

#define vc8000e_jpeg_roimap_qptablenonroi_value88           VC8000E_REG(420, 24, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value89           VC8000E_REG(420, 16, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value90           VC8000E_REG(420,  8, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value91           VC8000E_REG(420,  0, 0xff)

#define vc8000e_jpeg_roimap_qptablenonroi_value92           VC8000E_REG(421, 24, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value93           VC8000E_REG(421, 16, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value94           VC8000E_REG(421,  8, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value95           VC8000E_REG(421,  0, 0xff)

#define vc8000e_jpeg_roimap_qptablenonroi_value96           VC8000E_REG(422, 24, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value97           VC8000E_REG(422, 16, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value98           VC8000E_REG(422,  8, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value99           VC8000E_REG(422,  0, 0xff)

#define vc8000e_jpeg_roimap_qptablenonroi_value100          VC8000E_REG(423, 24, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value101          VC8000E_REG(423, 16, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value102          VC8000E_REG(423,  8, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value103          VC8000E_REG(423,  0, 0xff)

#define vc8000e_jpeg_roimap_qptablenonroi_value104          VC8000E_REG(424, 24, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value105          VC8000E_REG(424, 16, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value106          VC8000E_REG(424,  8, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value107          VC8000E_REG(424,  0, 0xff)

#define vc8000e_jpeg_roimap_qptablenonroi_value108          VC8000E_REG(425, 24, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value109          VC8000E_REG(425, 16, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value110          VC8000E_REG(425,  8, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value111          VC8000E_REG(425,  0, 0xff)

#define vc8000e_jpeg_roimap_qptablenonroi_value112          VC8000E_REG(426, 24, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value113          VC8000E_REG(426, 16, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value114          VC8000E_REG(426,  8, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value115          VC8000E_REG(426,  0, 0xff)

#define vc8000e_jpeg_roimap_qptablenonroi_value116          VC8000E_REG(427, 24, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value117          VC8000E_REG(427, 16, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value118          VC8000E_REG(427,  8, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value119          VC8000E_REG(427,  0, 0xff)

#define vc8000e_jpeg_roimap_qptablenonroi_value120          VC8000E_REG(428, 24, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value121          VC8000E_REG(428, 16, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value122          VC8000E_REG(428,  8, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value123          VC8000E_REG(428,  0, 0xff)

#define vc8000e_jpeg_roimap_qptablenonroi_value124          VC8000E_REG(429, 24, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value125          VC8000E_REG(429, 16, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value126          VC8000E_REG(429,  8, 0xff)
#define vc8000e_jpeg_roimap_qptablenonroi_value127          VC8000E_REG(429,  0, 0xff)

#define vc8000e_hwav1rdoqsupport                            VC8000E_REG(430, 31, 0x1)
#define vc8000e_hwav1interpfilterswitchable                 VC8000E_REG(430, 30, 0x1)
#define vc8000e_hwjpegroimapsupport                         VC8000E_REG(430, 29, 0x1)
#define vc8000e_hwbackgrounddetsupport                      VC8000E_REG(430, 28, 0x1)
#define vc8000e_hwvp9rdoqsupport                            VC8000E_REG(430, 27, 0x1)
#define vc8000e_hwctbbitsoutsupport                         VC8000E_REG(430, 26, 0x1)
#define vc8000e_hwvisualtunesupport                         VC8000E_REG(430, 25, 0x1)
#define vc8000e_hwpsytunesupport                            VC8000E_REG(430, 24, 0x1)
#define vc8000e_hwnonrotationsupport                        VC8000E_REG(430, 23, 0x1)
#define vc8000e_hwnvformatonlysupport                       VC8000E_REG(430, 22, 0x1)
#define vc8000e_hwmosaicsupport                             VC8000E_REG(430, 21, 0x1)
#define vc8000e_hwipcm8support                              VC8000E_REG(430, 20, 0x1)
#define vc8000e_hwaqinformationoutputsupport                VC8000E_REG(430, 19, 0x1)
#define vc8000e_hwpsnrsupport                               VC8000E_REG(430, 18, 0x1)
#define vc8000e_hwprpsbisupport                             VC8000E_REG(430, 17, 0x1)

#define vc8000e_hwav1precarrybufferlimit                    VC8000E_REG(431,  0, 0xffffffff)

#define vc8000e_overlay_vstep                               VC8000E_REG(432, 16, 0xffff)
#define vc8000e_overlay_hstep                               VC8000E_REG(432,  0, 0xffff)

#define vc8000e_overlay_scale_width                         VC8000E_REG(433, 16, 0xffff)
#define vc8000e_overlay_scale_height                        VC8000E_REG(433,  0, 0xffff)

#define vc8000e_vp9_seg_tree_prob1                          VC8000E_REG(434, 24, 0xff)
#define vc8000e_vp9_seg_tree_prob2                          VC8000E_REG(434, 16, 0xff)
#define vc8000e_vp9_seg_tree_prob3                          VC8000E_REG(434,  8, 0xff)
#define vc8000e_vp9_seg_tree_prob4                          VC8000E_REG(434,  0, 0xff)

#define vc8000e_vp9_seg_tree_prob5                          VC8000E_REG(435, 24, 0xff)
#define vc8000e_vp9_seg_tree_prob6                          VC8000E_REG(435, 16, 0xff)
#define vc8000e_vp9_seg_tree_prob7                          VC8000E_REG(435,  8, 0xff)
#define vc8000e_ctb_bits_out_enable                         VC8000E_REG(435,  7, 0x1)

#define vc8000e_ctb_bits_out_addr                           VC8000E_REG(436,  0, 0xffffffff)

#define vc8000e_ctb_bits_out_addr_msb                       VC8000E_REG(437,  0, 0xffffffff)

#define vc8000e_h264_intramode4x4_disable                   VC8000E_REG(438, 31, 0x1)
#define vc8000e_h264_intramode8x8_disable                   VC8000E_REG(438, 30, 0x1)
#define vc8000e_reference_use_input_enable                  VC8000E_REG(438, 29, 0x1)
#define vc8000e_me_lambda_qp                                VC8000E_REG(438, 23, 0x3f)
#define vc8000e_bi_mv_in_ldb_disable                        VC8000E_REG(438, 22, 0x1)
#define vc8000e_inloop_ds_bilinear_enable                   VC8000E_REG(438, 21, 0x1)
#define vc8000e_pred_mode_by_satd_enable                    VC8000E_REG(438, 20, 0x1)
#define vc8000e_psy_factor                                  VC8000E_REG(438,  9, 0x7ff)
#define vc8000e_hevc_simple_rdo_assign                      VC8000E_REG(438,  7, 0x3)
#define vc8000e_overlay_enable9                             VC8000E_REG(438,  6, 0x1)
#define vc8000e_overlay_enable10                            VC8000E_REG(438,  5, 0x1)
#define vc8000e_overlay_enable11                            VC8000E_REG(438,  4, 0x1)
#define vc8000e_overlay_enable12                            VC8000E_REG(438,  3, 0x1)

#define vc8000e_overlay_height9                             VC8000E_REG(439, 16, 0xffff)
#define vc8000e_overlay_width9                              VC8000E_REG(439,  0, 0xffff)

#define vc8000e_overlay_yoffset9                            VC8000E_REG(440, 16, 0xffff)
#define vc8000e_overlay_xoffset9                            VC8000E_REG(440,  0, 0xffff)

#define vc8000e_overlay_height10                            VC8000E_REG(441, 16, 0xffff)
#define vc8000e_overlay_width10                             VC8000E_REG(441,  0, 0xffff)

#define vc8000e_overlay_yoffset10                           VC8000E_REG(442, 16, 0xffff)
#define vc8000e_overlay_xoffset10                           VC8000E_REG(442,  0, 0xffff)

#define vc8000e_overlay_height11                            VC8000E_REG(443, 16, 0xffff)
#define vc8000e_overlay_width11                             VC8000E_REG(443,  0, 0xffff)

#define vc8000e_overlay_yoffset11                           VC8000E_REG(444, 16, 0xffff)
#define vc8000e_overlay_xoffset11                           VC8000E_REG(444,  0, 0xffff)

#define vc8000e_overlay_height12                            VC8000E_REG(445, 16, 0xffff)
#define vc8000e_overlay_width12                             VC8000E_REG(445,  0, 0xffff)

#define vc8000e_overlay_yoffset12                           VC8000E_REG(446, 16, 0xffff)
#define vc8000e_overlay_xoffset12                           VC8000E_REG(446,  0, 0xffff)

#define vc8000e_rdoq_lambda_adjust_intra                    VC8000E_REG(447, 16, 0xffff)
#define vc8000e_rdoq_lambda_adjust_inter                    VC8000E_REG(447,  0, 0xffff)

#define vc8000e_aq_information_output_mode                  VC8000E_REG(448, 29, 0x7)
#define vc8000e_aq_strength                                 VC8000E_REG(448, 20, 0x1ff)
#define vc8000e_aq_information_output_stride                VC8000E_REG(448,  0, 0xfffff)

#define vc8000e_aq_information_output_base                  VC8000E_REG(449,  0, 0xffffffff)

#define vc8000e_aq_information_output_base_msb              VC8000E_REG(450,  0, 0xffffffff)

#define vc8000e_tmvp_mvinfo_addr                            VC8000E_REG(451,  0, 0xffffffff)

#define vc8000e_tmvp_mvinfo_addr_msb                        VC8000E_REG(452,  0, 0xffffffff)

#define vc8000e_tmvp_ref_mvinfo_l0_addr                     VC8000E_REG(453,  0, 0xffffffff)

#define vc8000e_tmvp_ref_mvinfo_l0_addr_msb                 VC8000E_REG(454,  0, 0xffffffff)

#define vc8000e_tmvp_ref_mvinfo_l1_addr                     VC8000E_REG(455,  0, 0xffffffff)

#define vc8000e_tmvp_ref_mvinfo_l1_addr_msb                 VC8000E_REG(456,  0, 0xffffffff)

#define vc8000e_overlay_input_yaddr9                        VC8000E_REG(457,  0, 0xffffffff)

#define vc8000e_overlay_input_yaddr9_msb                    VC8000E_REG(458,  0, 0xffffffff)

#define vc8000e_overlay_input_uaddr9                        VC8000E_REG(459,  0, 0xffffffff)

#define vc8000e_overlay_input_uaddr9_msb                    VC8000E_REG(460,  0, 0xffffffff)

#define vc8000e_overlay_input_vaddr9                        VC8000E_REG(461,  0, 0xffffffff)

#define vc8000e_overlay_input_vaddr9_msb                    VC8000E_REG(462,  0, 0xffffffff)

#define vc8000e_overlay_input_yaddr10                       VC8000E_REG(463,  0, 0xffffffff)

#define vc8000e_overlay_input_yaddr10_msb                   VC8000E_REG(464,  0, 0xffffffff)

#define vc8000e_overlay_input_uaddr10                       VC8000E_REG(465,  0, 0xffffffff)

#define vc8000e_overlay_input_uaddr10_msb                   VC8000E_REG(466,  0, 0xffffffff)

#define vc8000e_overlay_input_vaddr10                       VC8000E_REG(467,  0, 0xffffffff)

#define vc8000e_overlay_input_vaddr10_msb                   VC8000E_REG(468,  0, 0xffffffff)

#define vc8000e_overlay_input_yaddr11                       VC8000E_REG(469,  0, 0xffffffff)

#define vc8000e_overlay_input_yaddr11_msb                   VC8000E_REG(470,  0, 0xffffffff)

#define vc8000e_overlay_input_uaddr11                       VC8000E_REG(471,  0, 0xffffffff)

#define vc8000e_overlay_input_uaddr11_msb                   VC8000E_REG(472,  0, 0xffffffff)

#define vc8000e_overlay_input_vaddr11                       VC8000E_REG(473,  0, 0xffffffff)

#define vc8000e_overlay_input_vaddr11_msb                   VC8000E_REG(474,  0, 0xffffffff)

#define vc8000e_overlay_input_yaddr12                       VC8000E_REG(475,  0, 0xffffffff)

#define vc8000e_overlay_input_yaddr12_msb                   VC8000E_REG(476,  0, 0xffffffff)

#define vc8000e_overlay_input_uaddr12                       VC8000E_REG(477,  0, 0xffffffff)

#define vc8000e_overlay_input_uaddr12_msb                   VC8000E_REG(478,  0, 0xffffffff)

#define vc8000e_overlay_input_vaddr12                       VC8000E_REG(479,  0, 0xffffffff)

#define vc8000e_overlay_input_vaddr12_msb                   VC8000E_REG(480,  0, 0xffffffff)

#define vc8000e_overlay_alpha9                              VC8000E_REG(481, 24, 0xff)
#define vc8000e_overlay_bitmapy9                            VC8000E_REG(481, 16, 0xff)
#define vc8000e_overlay_bitmapu9                            VC8000E_REG(481,  8, 0xff)
#define vc8000e_overlay_bitmapv9                            VC8000E_REG(481,  0, 0xff)

#define vc8000e_overlay_alpha10                             VC8000E_REG(482, 24, 0xff)
#define vc8000e_overlay_bitmapy10                           VC8000E_REG(482, 16, 0xff)
#define vc8000e_overlay_bitmapu10                           VC8000E_REG(482,  8, 0xff)
#define vc8000e_overlay_bitmapv10                           VC8000E_REG(482,  0, 0xff)

#define vc8000e_overlay_alpha11                             VC8000E_REG(483, 24, 0xff)
#define vc8000e_overlay_bitmapy11                           VC8000E_REG(483, 16, 0xff)
#define vc8000e_overlay_bitmapu11                           VC8000E_REG(483,  8, 0xff)
#define vc8000e_overlay_bitmapv11                           VC8000E_REG(483,  0, 0xff)

#define vc8000e_overlay_alpha12                             VC8000E_REG(484, 24, 0xff)
#define vc8000e_overlay_bitmapy12                           VC8000E_REG(484, 16, 0xff)
#define vc8000e_overlay_bitmapu12                           VC8000E_REG(484,  8, 0xff)
#define vc8000e_overlay_bitmapv12                           VC8000E_REG(484,  0, 0xff)

#define vc8000e_overlay_ystride9                            VC8000E_REG(485, 12, 0xfffff)
#define vc8000e_overlay_format9                             VC8000E_REG(485, 10, 0x3)
#define vc8000e_overlay_format10                            VC8000E_REG(485,  8, 0x3)
#define vc8000e_overlay_format11                            VC8000E_REG(485,  6, 0x3)
#define vc8000e_overlay_format12                            VC8000E_REG(485,  4, 0x3)

#define vc8000e_overlay_uvstride9                           VC8000E_REG(486, 12, 0xfffff)

#define vc8000e_overlay_ystride10                           VC8000E_REG(487, 12, 0xfffff)

#define vc8000e_overlay_uvstride10                          VC8000E_REG(488, 12, 0xfffff)

#define vc8000e_overlay_ystride11                           VC8000E_REG(489, 12, 0xfffff)

#define vc8000e_overlay_uvstride11                          VC8000E_REG(490, 12, 0xfffff)

#define vc8000e_overlay_ystride12                           VC8000E_REG(491, 12, 0xfffff)

#define vc8000e_overlay_uvstride12                          VC8000E_REG(492, 12, 0xfffff)

#define vc8000e_ctrl_clock                                  VC8000E_REG(493, 16, 0x1)
#define vc8000e_enable_clock                                VC8000E_REG(493,  8, 0x1)
#define vc8000e_assert_reset                                VC8000E_REG(493,  0, 0x1)

#define vc8000e_tilesyncreadbase                            VC8000E_REG(494,  0, 0xffffffff)

#define vc8000e_tilesyncreadbase_msb                        VC8000E_REG(495,  0, 0xffffffff)

#define vc8000e_tilesyncwritebase                           VC8000E_REG(496,  0, 0xffffffff)

#define vc8000e_tilesyncwritebase_msb                       VC8000E_REG(497,  0, 0xffffffff)

#define vc8000e_tileheightsbase                             VC8000E_REG(498,  0, 0xffffffff)

#define vc8000e_tileheightsbase_msb                         VC8000E_REG(499,  0, 0xffffffff)

#define vc8000e_request_seg_num_2                           VC8000E_REG(500, 20, 0x3ff)
#define vc8000e_request_seg_num_1                           VC8000E_REG(500, 10, 0x3ff)
#define vc8000e_request_seg_num_0                           VC8000E_REG(500,  0, 0x3ff)

#define vc8000e_collocate_refidx                            VC8000E_REG(501, 31, 0x1)
#define vc8000e_collocate_from_l0                           VC8000E_REG(501, 30, 0x1)
#define vc8000e_update_seg_num_2                            VC8000E_REG(501, 20, 0x3ff)
#define vc8000e_update_seg_num_1                            VC8000E_REG(501, 10, 0x3ff)
#define vc8000e_update_seg_num_0                            VC8000E_REG(501,  0, 0x3ff)

#define vc8000e_rpll0_deltapoc_l0_idx1                      VC8000E_REG(502, 16, 0xffff)
#define vc8000e_rpll0_deltapoc_l0_idx0                      VC8000E_REG(502,  0, 0xffff)

#define vc8000e_rpll0_deltapoc_l1_idx1                      VC8000E_REG(503, 16, 0xffff)
#define vc8000e_rpll0_deltapoc_l1_idx0                      VC8000E_REG(503,  0, 0xffff)

#define vc8000e_rpll1_deltapoc_l0_idx1                      VC8000E_REG(504, 16, 0xffff)
#define vc8000e_rpll1_deltapoc_l0_idx0                      VC8000E_REG(504,  0, 0xffff)

#define vc8000e_rpll1_deltapoc_l1_idx1                      VC8000E_REG(505, 16, 0xffff)
#define vc8000e_rpll1_deltapoc_l1_idx0                      VC8000E_REG(505,  0, 0xffff)

#define vc8000e_av1_lastalt_orderhint                       VC8000E_REG(506,  0, 0xffffffff)

#endif /* HANTRO_VC8000E_REGS_H_ */