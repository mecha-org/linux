// SPDX-License-Identifier: GPL-2.0-only
/*
 * DDIC CH13726A MIPI-DSI panel driver
 * Copyright (c) 2024, Teguh Sobirin <teguh@sobir.in>.
 * Copyright (c) 2025, Silicon Signals Pvt. Ltd.
 */

#include <linux/backlight.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/regulator/consumer.h>

#include <drm/display/drm_dsc.h>
#include <drm/display/drm_dsc_helper.h>
#include <drm/drm_mipi_dsi.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>

#include <video/mipi_display.h>

struct ch13726a_panel {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct drm_dsc_config dsc;
	struct regulator_bulk_data supplies[4];
	struct gpio_desc *reset_gpio;
	const struct drm_display_mode *display_mode;
	enum drm_panel_orientation orientation;
	bool prepared;
};

static inline struct ch13726a_panel *to_ch13726a_panel(struct drm_panel *panel)
{
	return container_of(panel, struct ch13726a_panel, panel);
}

static void ch13726a_reset(struct ch13726a_panel *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(20000, 21000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(20000, 21000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(20000, 21000);
}

static int ch13726a_on(struct ch13726a_panel *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;
	
	mipi_dsi_dcs_write_seq(dsi, 0x2a, 0x00, 0x00, 0x04, 0x37);
	mipi_dsi_dcs_write_seq(dsi, 0x2b, 0x00, 0x00, 0x04, 0xd7);
	mipi_dsi_dcs_write_seq(dsi, 0x8f, 0x04, 0x88, 0x77);
	mipi_dsi_dcs_write_seq(dsi, 0x51, 0xff);
	mipi_dsi_dcs_write_seq(dsi, 0x53, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x35, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x60, 0x80);
	mipi_dsi_dcs_write_seq(dsi, 0x44, 0x02, 0x6c);
	mipi_dsi_dcs_write_seq(dsi, 0xf0, 0x50);
	mipi_dsi_dcs_write_seq(dsi, 0xb0, 0x0e, 0x0e);
	mipi_dsi_dcs_write_seq(dsi, 0xb2, 0x80);
	mipi_dsi_dcs_write_seq(dsi, 0xb3, 0x00, 0x1e);
	mipi_dsi_dcs_write_seq(dsi, 0xb4, 0x92, 0x18, 0x92);
	mipi_dsi_dcs_write_seq(dsi, 0xb6, 0x00, 0x3f, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xb7, 0x00, 0x00, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xb8, 0x02, 0x62);
	mipi_dsi_dcs_write_seq(dsi, 0xb9, 0x11);
	mipi_dsi_dcs_write_seq(dsi, 0xbb, 0x08, 0x08);
	mipi_dsi_dcs_write_seq(dsi, 0xc0, 0x80);
	mipi_dsi_dcs_write_seq(dsi, 0xc7, 0x40);
	mipi_dsi_dcs_write_seq(dsi, 0xc9, 0x55, 0xf8, 0xf8);
	mipi_dsi_dcs_write_seq(dsi, 0xe8, 0x58);
	mipi_dsi_dcs_write_seq(dsi, 0xec, 0x08);
	mipi_dsi_dcs_write_seq(dsi, 0xf0, 0x51);
	mipi_dsi_dcs_write_seq(dsi, 0xb0, 0x28);
	mipi_dsi_dcs_write_seq(dsi, 0xb1, 0x28);
	mipi_dsi_dcs_write_seq(dsi, 0xb2, 0x1c);
	mipi_dsi_dcs_write_seq(dsi, 0xb3, 0x1c);
	mipi_dsi_dcs_write_seq(dsi, 0xc0, 0x41);
	mipi_dsi_dcs_write_seq(dsi, 0xc1, 0x1e, 0x1e);
	mipi_dsi_dcs_write_seq(dsi, 0xc4, 0x28);
	mipi_dsi_dcs_write_seq(dsi, 0xc9, 0x1b);
	mipi_dsi_dcs_write_seq(dsi, 0xd1, 0x4a, 0x4a, 0x80, 0x4f, 0x69);
	mipi_dsi_dcs_write_seq(dsi, 0xd2, 0x05);
	mipi_dsi_dcs_write_seq(dsi, 0xd3, 0x11, 0x00, 0x00, 0x64);
	mipi_dsi_dcs_write_seq(dsi, 0xd7, 0xa8, 0x5a, 0xaa, 0xaa);
	mipi_dsi_dcs_write_seq(dsi, 0xd8, 0xa8, 0x5a, 0xaa, 0xaa);
	mipi_dsi_dcs_write_seq(dsi, 0xdd, 0xaa, 0xaa, 0xaa, 0xaa);
	mipi_dsi_dcs_write_seq(dsi, 0xf0, 0x52);
	mipi_dsi_dcs_write_seq(dsi, 0xb0, 0x21, 0x94, 0x69);
	mipi_dsi_dcs_write_seq(dsi, 0xba, 0xff, 0x35, 0x43, 0x52, 0x61, 0xff, 0x71, 0x79, 0x7d, 0x80, 0x0f, 0x85, 0x87);
	mipi_dsi_dcs_write_seq(dsi, 0xb9, 0xaa, 0x21, 0x45, 0x67, 0x86, 0xaa, 0x95, 0xa4, 0xc0, 0xde, 0xfe, 0xfa, 0x09, 0x18, 0x26);
	mipi_dsi_dcs_write_seq(dsi, 0xb8, 0x00, 0x00, 0xda, 0xed, 0xf8, 0x55, 0x29, 0x51, 0x6a, 0x7b, 0x55, 0xa1, 0xc2, 0xdf, 0xf7);
	mipi_dsi_dcs_write_seq(dsi, 0xb7, 0xfa, 0xec, 0xfa, 0x07, 0x16, 0xff, 0x24, 0x2b, 0x2e, 0x31, 0x0f, 0x36, 0x37);
	mipi_dsi_dcs_write_seq(dsi, 0xb6, 0xa9, 0xec, 0x0f, 0x2d, 0x4a, 0xaa, 0x58, 0x66, 0x81, 0x9c, 0xaa, 0xb6, 0xc3, 0xd0, 0xde);
	mipi_dsi_dcs_write_seq(dsi, 0xb5, 0x00, 0x00, 0xd8, 0xe3, 0xea, 0x55, 0x03, 0x22, 0x3b, 0x51, 0x55, 0x76, 0x95, 0xaf, 0xc5);
	mipi_dsi_dcs_write_seq(dsi, 0xb4, 0xff, 0x14, 0x23, 0x32, 0x41, 0xff, 0x51, 0x59, 0x5d, 0x61, 0x0f, 0x65, 0x67);
	mipi_dsi_dcs_write_seq(dsi, 0xb3, 0xaa, 0x05, 0x28, 0x48, 0x67, 0xaa, 0x76, 0x84, 0xa0, 0xbc, 0xea, 0xd9, 0xe8, 0xf7, 0x05);
	mipi_dsi_dcs_write_seq(dsi, 0xb2, 0x00, 0x00, 0xc0, 0xcd, 0xcf, 0x55, 0x15, 0x39, 0x52, 0x69, 0x55, 0x8f, 0xad, 0xc6, 0xdd);
	mipi_dsi_dcs_write_seq(dsi, 0xf0, 0x53);
	mipi_dsi_dcs_write_seq(dsi, 0xb0, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xb3, 0xa8, 0x10, 0x3c, 0x0a, 0x03, 0x77, 0x77);
	mipi_dsi_dcs_write_seq(dsi, 0xb4, 0x26, 0x00, 0x00, 0x05, 0x00, 0x77, 0x77);
	mipi_dsi_dcs_write_seq(dsi, 0xb9, 0x24, 0x90, 0x00, 0x77, 0x17, 0x31, 0x22, 0x90, 0x00, 0x77, 0x17, 0x31);
	mipi_dsi_dcs_write_seq(dsi, 0xba, 0x20, 0x90, 0x00, 0x77, 0x03, 0x10, 0x21, 0x90, 0x00, 0x77, 0x03, 0x10);
	mipi_dsi_dcs_write_seq(dsi, 0xbe, 0x20, 0x09, 0x00, 0x00, 0x2c, 0x20, 0x46, 0x00, 0x00, 0x2c);
	mipi_dsi_dcs_write_seq(dsi, 0xc1, 0x13, 0x12, 0x0b, 0x0a, 0x02, 0x09, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xc3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x09, 0x03, 0x0a, 0x0b, 0x12, 0x13);
	mipi_dsi_dcs_write_seq(dsi, 0xf0, 0x55);
	mipi_dsi_dcs_write_seq(dsi, 0xb1, 0x80, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xb3, 0x7f, 0x7f, 0x00, 0x00, 0x77, 0x87, 0x32, 0xaa, 0x21);
	mipi_dsi_dcs_write_seq(dsi, 0xb4, 0x2a, 0xa9, 0x2a, 0x21, 0xaa, 0x32, 0x00, 0x87, 0x77);
	mipi_dsi_dcs_write_seq(dsi, 0xb5, 0x32, 0xaa, 0x21, 0x32, 0xaa, 0x21, 0x77, 0x87, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xb6, 0x00, 0x87, 0x77, 0x87, 0x77, 0x00, 0x21, 0xaa, 0x32);
	mipi_dsi_dcs_write_seq(dsi, 0xc3, 0x7f, 0x7f, 0x00, 0x00, 0x47, 0x51, 0x32, 0xaa, 0x21);
	mipi_dsi_dcs_write_seq(dsi, 0xc4, 0x2a, 0xa9, 0x2a, 0x21, 0x66, 0x1e, 0x00, 0x87, 0x77);
	mipi_dsi_dcs_write_seq(dsi, 0xc5, 0x1e, 0x66, 0x14, 0x32, 0xaa, 0x21, 0x77, 0x87, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xc6, 0x00, 0x51, 0x47, 0x87, 0x77, 0x00, 0x21, 0xaa, 0x32);
	mipi_dsi_dcs_write_seq(dsi, 0xc7, 0x7f, 0x7f, 0x00, 0x00, 0x77, 0x87, 0x32, 0xaa, 0x21);
	mipi_dsi_dcs_write_seq(dsi, 0xc8, 0x32, 0xaa, 0x21, 0x32, 0xaa, 0x21, 0x77, 0x87, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xc4, 0x2a, 0xa9, 0x2a, 0x21, 0xaa, 0x32, 0x00, 0x87, 0x77);
	mipi_dsi_dcs_write_seq(dsi, 0xb6, 0x00, 0x87, 0x77, 0x87, 0x77, 0x00, 0x21, 0xaa, 0x32);
	mipi_dsi_dcs_write_seq(dsi, 0xf0, 0x58);
	mipi_dsi_dcs_write_seq(dsi, 0xc0, 0x83, 0xad, 0x52, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xc1, 0xbf, 0x7f, 0x3f, 0x2f, 0x1f, 0x0f, 0x07, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xc2, 0xff);
	mipi_dsi_dcs_write_seq(dsi, 0xc3, 0x7f, 0x05);
	mipi_dsi_dcs_write_seq(dsi, 0xc4, 0x09);
	mipi_dsi_dcs_write_seq(dsi, 0xc5, 0x00, 0x09, 0x09, 0x09, 0x09, 0x00, 0x09, 0x09, 0x09, 0x09);
	mipi_dsi_dcs_write_seq(dsi, 0xc6, 0xff, 0xff);
	mipi_dsi_dcs_write_seq(dsi, 0xc7, 0x5a);
	mipi_dsi_dcs_write_seq(dsi, 0xc9, 0x7f, 0x3f, 0x2f, 0x1f, 0x0f, 0x07, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xca, 0xff, 0x8f);
	mipi_dsi_dcs_write_seq(dsi, 0xcb, 0x67, 0x44, 0x34, 0x01, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xcc, 0x5a, 0xe9, 0x1f, 0xdc, 0x8b, 0x01, 0x1b, 0xc8, 0xac, 0x9c, 0x00, 0x89, 0x70, 0x52, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0x59, 0x0e);
	mipi_dsi_dcs_write_seq(dsi, 0xf0, 0x59);
	mipi_dsi_dcs_write_seq(dsi, 0xb0, 0x13, 0x04, 0xc4, 0x00, 0x3c, 0x3b);
	mipi_dsi_dcs_write_seq(dsi, 0xb2, 0x07, 0x73, 0xda, 0xfe, 0x13, 0xe8, 0x13, 0xf9, 0x52, 0x3e, 0x90, 0x2f, 0xb0, 0x02, 0x1b, 0x29);
	mipi_dsi_dcs_write_seq(dsi, 0xb3, 0xf5, 0x12, 0x1e, 0x29, 0xf1, 0x81, 0x02, 0x1e, 0x13, 0x17, 0x1a, 0x1d, 0x1e, 0x0f, 0x00, 0x00);
	mipi_dsi_dcs_write_seq(dsi, 0xc2, 0xf1, 0xe1, 0xd1, 0xa1, 0x71, 0x31, 0x02, 0x1e, 0x28, 0xf1, 0x91, 0x12, 0x2e, 0xf5, 0x91, 0x02);
	mipi_dsi_dcs_write_seq(dsi, 0xc3, 0x2b, 0xb0, 0x03, 0xf9, 0x52, 0x3e, 0x91, 0x3f, 0x81, 0x7e, 0x30, 0xa7, 0xed, 0x0f, 0x00, 0x00);
	
	ret = mipi_dsi_dcs_exit_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to exit sleep mode: %d\n", ret);
		return ret;
	}
	
	msleep(128);
	
	ret = mipi_dsi_dcs_set_display_on(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to set display on: %d\n", ret);
		return ret;
	}
	msleep(20);

	return 0;
}

static int ch13726a_disable(struct drm_panel *panel)
{
	struct ch13726a_panel *ctx = to_ch13726a_panel(panel);
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	ret = mipi_dsi_dcs_set_display_off(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to set display off: %d\n", ret);
		return ret;
	}

	ret = mipi_dsi_dcs_enter_sleep_mode(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to enter sleep mode: %d\n", ret);
		return ret;
	}

	msleep(100);

	return 0;
}

static int ch13726a_enable(struct drm_panel *panel)
{
	struct ch13726a_panel *ctx = to_ch13726a_panel(panel);

	struct device *dev = &ctx->dsi->dev;
	int ret;

	if (ctx->prepared)
		return 0;

	ret = regulator_bulk_enable(ARRAY_SIZE(ctx->supplies), ctx->supplies);
	if (ret < 0) {
        	dev_err(dev, "Failed to enable regulators: %d\n", ret);
        	return ret;
	}

	ch13726a_reset(ctx);

	ret = ch13726a_on(ctx);
	if (ret < 0) {
        	dev_err(dev, "Failed to initialize panel: %d\n", ret);
		gpiod_set_value_cansleep(ctx->reset_gpio, 0);
		regulator_bulk_disable(ARRAY_SIZE(ctx->supplies), ctx->supplies);
		return ret;
	}

	msleep(20);

	ctx->prepared = true;
	return ret;

}


static int ch13726a_unprepare(struct drm_panel *panel)
{
	struct ch13726a_panel *ctx = to_ch13726a_panel(panel);

	if (!ctx->prepared)
		return 0;

	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	regulator_bulk_disable(ARRAY_SIZE(ctx->supplies), ctx->supplies);

	ctx->prepared = false;
	return 0;
}

static const struct drm_display_mode rpmini_display_mode = {
	.clock = (960 + 28 + 4 + 36) * (1280 + 16 + 4 + 8) * 60 / 1000,
	.hdisplay = 960,
	.hsync_start = 960 + 28,
	.hsync_end = 960 + 28 + 4,
	.htotal = 960 + 28 + 4 + 36,
	.vdisplay = 1280,
	.vsync_start = 1280 + 16,
	.vsync_end = 1280 + 16 + 4,
	.vtotal = 1280 + 16 + 4 + 8,
	.width_mm = 65,
	.height_mm = 75,
};

//static const struct drm_display_mode rp5_display_mode = {
//	.clock = (1080 + 80 + 80 + 80) * (1240 + 90 + 90) * 60 / 1000,
//	.hdisplay = 1080,
//	.hsync_start = 1080 + 80 + 80,
//	.hsync_end = 1080 + 80,
//	.htotal = 1080 + 80 + 80 + 80,
//	.vdisplay = 1240,
//	.vsync_start = 1240 + 90 + 90,
//	.vsync_end = 1240 + 90,
//	.vtotal = 1240 + 90 + 90 + 90,
//	.width_mm = 65.448,
//	.height_mm = 75.144,
//};

static const struct drm_display_mode rp5_display_mode = {
	.clock = (1080 + 80 + 80 + 80) * (1240 + 90 + 90 + 90) * 60 / 1000,
	.hdisplay = 1080,
	.hsync_start = 1080 + 80,            // front porch
	.hsync_end = 1080 + 80 + 80,         // front + sync
	.htotal = 1080 + 80 + 80 + 80,       // full line
	.vdisplay = 1240,
	.vsync_start = 1240 + 90,            // front porch
	.vsync_end = 1240 + 90 + 90,         // front + sync
	.vtotal = 1240 + 90 + 90 + 90,       // full frame
	.width_mm = 65,                      // or round as needed
	.height_mm = 75,
};

static int ch13726a_get_modes(struct drm_panel *panel,
					struct drm_connector *connector)
{
	struct ch13726a_panel *ctx = to_ch13726a_panel(panel);
	struct drm_display_mode *mode;

	mode = drm_mode_duplicate(connector->dev, ctx->display_mode);
	if (!mode)
		return -ENOMEM;

	drm_mode_set_name(mode);

	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;
	connector->display_info.width_mm = mode->width_mm;
	connector->display_info.height_mm = mode->height_mm;
	drm_mode_probed_add(connector, mode);

	return 1;
}

static enum drm_panel_orientation ch13726a_get_orientation(struct drm_panel *panel)
{
	struct ch13726a_panel *ctx = to_ch13726a_panel(panel);

	return ctx->orientation;
}

static const struct drm_panel_funcs ch13726a_panel_funcs = {
	.unprepare = ch13726a_unprepare,
	.enable = ch13726a_enable,
	.disable = ch13726a_disable,
	.get_modes = ch13726a_get_modes,
	.get_orientation = ch13726a_get_orientation,
};

static int ch13726a_bl_update_status(struct backlight_device *bl)
{
	struct mipi_dsi_device *dsi = bl_get_data(bl);
	u16 brightness = backlight_get_brightness(bl);
	int ret;

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	ret = mipi_dsi_dcs_set_display_brightness(dsi, brightness);
	if (ret < 0)
		return ret;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	return 0;
}

static const struct backlight_ops ch13726a_bl_ops = {
	.update_status = ch13726a_bl_update_status,
};

static struct backlight_device *
ch13726a_create_backlight(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	const struct backlight_properties props = {
		.type = BACKLIGHT_RAW,
		.brightness = 255,
		.max_brightness = 255,
	};

	return devm_backlight_device_register(dev, dev_name(dev), dev, dsi,
					      &ch13726a_bl_ops, &props);
}

static int ch13726a_probe(struct mipi_dsi_device *dsi)
{
	printk(KERN_INFO "DEBUG CH13726A : ch13726a_probe success...");
	struct device *dev = &dsi->dev;
	struct ch13726a_panel *ctx;
	int rotation;
	int ret;

	ctx = devm_kzalloc(dev, sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;
	printk(KERN_INFO " CH13726A : devm_kzalloc success...");

	ctx->display_mode = of_device_get_match_data(dev);

	ctx->supplies[0].supply = "vdd1v2";
	ctx->supplies[1].supply = "vddio";
	ctx->supplies[2].supply = "vdd";
	ctx->supplies[3].supply = "avdd";

	ret = devm_regulator_bulk_get(dev, ARRAY_SIZE(ctx->supplies),
				      ctx->supplies);
	if (ret < 0)
		return dev_err_probe(dev, ret, "Failed to get regulators\n");
	printk(KERN_INFO " CH13726A : devm_regulator_bulk_get success...");

	ctx->reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_OUT_LOW);
	if (IS_ERR(ctx->reset_gpio))
		return dev_err_probe(dev, PTR_ERR(ctx->reset_gpio),
				     "Failed to get reset-gpios\n");
	printk(KERN_INFO " CH13726A : devm_gpiod_get  success...");

	ret = of_property_read_u32(dev->of_node, "rotation", &rotation);
	if (ret == -EINVAL) {
		ctx->orientation = DRM_MODE_PANEL_ORIENTATION_UNKNOWN;
	}

	if (rotation == 0)
		ctx->orientation = DRM_MODE_PANEL_ORIENTATION_NORMAL;
	else if (rotation == 90)
		ctx->orientation = DRM_MODE_PANEL_ORIENTATION_RIGHT_UP;
	else if (rotation == 180)
		ctx->orientation = DRM_MODE_PANEL_ORIENTATION_BOTTOM_UP;
	else if (rotation == 270)
		ctx->orientation = DRM_MODE_PANEL_ORIENTATION_LEFT_UP;

	ctx->dsi = dsi;
	mipi_dsi_set_drvdata(dsi, ctx);

	dsi->lanes = 4;
	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags = MIPI_DSI_MODE_VIDEO |
			  MIPI_DSI_CLOCK_NON_CONTINUOUS;

	drm_panel_init(&ctx->panel, dev, &ch13726a_panel_funcs,
		       DRM_MODE_CONNECTOR_DSI);
	ctx->panel.prepare_prev_first = true;

	ctx->panel.backlight = ch13726a_create_backlight(dsi);
	if (IS_ERR(ctx->panel.backlight))
		return dev_err_probe(dev, PTR_ERR(ctx->panel.backlight),
				     "Failed to create backlight\n");

	drm_panel_add(&ctx->panel);

	ret = mipi_dsi_attach(dsi);
	if (ret < 0) {
		dev_err(dev, "Failed to attach to DSI host: %d\n", ret);
		drm_panel_remove(&ctx->panel);
		return ret;
	}
	printk(KERN_INFO " CH13726A : mipi_dsi_attach  success...");

	return 0;
}

static void ch13726a_remove(struct mipi_dsi_device *dsi)
{
	struct ch13726a_panel *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id ch13726a_of_match[] = {
	{ .compatible = "ch13726a,rpmini", .data = &rpmini_display_mode },
	{ .compatible = "ch13726a,rp5", .data = &rp5_display_mode },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, ch13726a_of_match);

static struct mipi_dsi_driver ch13726a_driver = {
	.probe = ch13726a_probe,
	.remove = ch13726a_remove,
	.driver = {
		.name = "panel-ch13726a-amoled",
		.of_match_table = ch13726a_of_match,
	},
};
module_mipi_dsi_driver(ch13726a_driver);

MODULE_DESCRIPTION("DRM driver for CH13726A DSI panels");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pratham Malaviya <prathamm@mechasystems.com>");
