/*
 * i.MX drm driver - Raydium MIPI-DSI panel driver
 *
 * Copyright (C) 2017 NXP
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


#include <drm/drm_crtc.h>
#include <drm/drm_mipi_dsi.h>
#include <drm/drm_panel.h>
#include <linux/gpio/consumer.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/printk.h>
#include <linux/regulator/consumer.h>
#include <video/mipi_display.h>
#include <video/of_videomode.h>
#include <video/videomode.h>
#include <linux/backlight.h>
#include <drm/drm_print.h>
#include <linux/delay.h>
#include <linux/media-bus-format.h>

u8 dcs0[] = {0xFF, 0x77, 0x01, 0x00, 0x00, 0x13 };
u8 dcs1[] = {0xEF, 0x08 };
u8 dcs2[] = {0xFF, 0x77, 0x01, 0x00, 0x00, 0x10 };
u8 dcs3[] = {0xC0, 0x3B, 0x00 };
u8 dcs4[] = {0xC1, 0x12, 0x0A };
u8 dcs5[] = {0xC2, 0x07, 0x03 };
u8 dcs6[] = {0xCC, 0x10 };
u8 dcs7[] = {0xB0, 0x0F, 0x11, 0x17, 0x15, 0x15, 0x09, 0x0C, 0x08, 0x08, 0x26, 0x04, 0x59, 0x16, 0x66, 0x2D, 0x1F };
u8 dcs8[] = {0xB1, 0x0F, 0x11, 0x17, 0x15, 0x15, 0x09, 0x0C, 0x08, 0x08, 0x26, 0x04, 0x59, 0x16, 0x66, 0x2D, 0x1F };
u8 dcs9[] =  {0xFF, 0x77, 0x01, 0x00, 0x00, 0x11 };
u8 dcs10[] = {0xB0, 0x6D };
u8 dcs11[] = {0xB1, 0x38 };
u8 dcs12[] = {0xB2, 0x01 };
u8 dcs13[] = {0xB3, 0x80 };
u8 dcs14[] = {0xB5, 0x4E };
u8 dcs15[] = {0xB7, 0x85 };
u8 dcs16[] = {0xB8, 0x20 };
u8 dcs17[] = {0xC1, 0x78 };
u8 dcs18[] = {0xC2, 0x78 };
u8 dcs19[] = {0xD0, 0x88 };
u8 dcs20[] = {0xE0, 0x00, 0x00, 0x02 };
u8 dcs21[] = {0xE1, 0x07, 0x00, 0x09, 0x00, 0x06, 0x00, 0x08, 0x00, 0x00, 0x33, 0x33 };
u8 dcs22[] = {0xE2, 0x11, 0x11, 0x33, 0x33, 0xF6, 0x00, 0xF6, 0x00, 0xF6, 0x00, 0xF6, 0x00, 0x00 };
u8 dcs23[] = {0xE3, 0x00, 0x00, 0x11, 0x11 };
u8 dcs24[] = {0xE4, 0x44, 0x44 };
u8 dcs25[] = {0xE5, 0x0F, 0xF3, 0x3D, 0xFF, 0x11, 0xF5, 0x3D, 0xFF, 0x0B, 0xEF,0x3D, 0xFF, 0x0D, 0xF1, 0x3D, 0xFF };
u8 dcs26[] = {0xE6, 0x00, 0x00, 0x11, 0x11 };
u8 dcs27[] = {0xE7, 0x44, 0x44 };
u8 dcs28[] = {0xE8, 0x0E, 0xF2, 0x3D, 0xFF, 0x10, 0xF4, 0x3D, 0xFF, 0x0A, 0xEE, 0x3D, 0xFF, 0x0C, 0xF0, 0x3D, 0xFF };
u8 dcs29[] = {0xE9, 0x36, 0x00 };
u8 dcs30[] = {0xEB, 0x00, 0x01, 0xE4, 0xE4, 0x44, 0xAA, 0x10 };
u8 dcs31[] = {0xED, 0xFF, 0x45, 0x67, 0xFA, 0x01, 0x2B, 0xCF, 0xFF, 0xFF, 0xFC, 0xB2, 0x10, 0xAF ,0x76, 0x54, 0xFF };
u8 dcs32[] = {0x29 };

unsigned char *ST7701S_CMD_Group[]={
	dcs0,
	dcs1,
	dcs2,
	dcs3,
	dcs4,
	dcs5,
	dcs6,
	dcs7,
	dcs8,
	dcs9,
	dcs10,
	dcs11,
	dcs12,
	dcs13,
	dcs14,
	dcs15,
	dcs16,
	dcs17,
	dcs18,
	dcs19,
	dcs20,
	dcs21,
	dcs22,
	dcs23,
	dcs24,
	dcs25,
	dcs26,
	dcs27,
	dcs28,
	dcs29,
	dcs30,
	dcs31,
	dcs32,
};
unsigned long int ST7701S_CMD_Group_size[]={
	sizeof(dcs0),
	sizeof(dcs1),
	sizeof(dcs2),
	sizeof(dcs3),
	sizeof(dcs4),
	sizeof(dcs5),
	sizeof(dcs6),
	sizeof(dcs7),
	sizeof(dcs8),
	sizeof(dcs9),
	sizeof(dcs10),
	sizeof(dcs11),
	sizeof(dcs12),
	sizeof(dcs13),
	sizeof(dcs14),
	sizeof(dcs15),
	sizeof(dcs16),
	sizeof(dcs17),
	sizeof(dcs18),
	sizeof(dcs19),
	sizeof(dcs20),
	sizeof(dcs21),
	sizeof(dcs22),
	sizeof(dcs23),
	sizeof(dcs24),
	sizeof(dcs25),
	sizeof(dcs26),
	sizeof(dcs27),
	sizeof(dcs28),
	sizeof(dcs29),
	sizeof(dcs30),
	sizeof(dcs31),
	sizeof(dcs32),
};

static const u32 st7701s_bus_formats[] = {
	MEDIA_BUS_FMT_RGB888_1X24,
	MEDIA_BUS_FMT_RGB666_1X18,
	MEDIA_BUS_FMT_RGB565_1X16,
};

 struct st7701s_panel {
	struct drm_panel base;
	struct mipi_dsi_device *dsi;

	struct gpio_desc *reset;
	struct backlight_device *backlight;

	bool prepared;
	bool enabled;

	struct videomode vm;
	u32 width_mm;
	u32 height_mm;
};

static inline struct st7701s_panel *to_st7701s_panel(struct drm_panel *panel)
{
	return container_of(panel, struct st7701s_panel, base);
}

static int st7701s_panel_push_cmd_list(struct mipi_dsi_device *dsi)
{
	size_t i;
	int ret = 0;
	struct device *dev = &dsi->dev;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	//send init cmds
	for(i=0; i<32; i++)
	{
		ret = mipi_dsi_generic_write(dsi, ST7701S_CMD_Group[i],
						ST7701S_CMD_Group_size[i]);
		if (ret < 0) {
			DRM_DEV_ERROR(dev, "Failed: CMD = 0x%x , length = %ld .\n",
				ST7701S_CMD_Group[i][0], sizeof(ST7701S_CMD_Group[i]));
			return ret;
		}
	}
	mdelay(120);
	mipi_dsi_generic_write(dsi, ST7701S_CMD_Group[32], ST7701S_CMD_Group_size[32] );
	return ret;
};

static int st7701s_panel_prepare(struct drm_panel *panel)
{
	struct st7701s_panel *st7701s = to_st7701s_panel(panel);

	if (st7701s->prepared)
		return 0;

	if (st7701s->reset != NULL) {
		gpiod_set_value(st7701s->reset, 1);
		mdelay(20);
		gpiod_set_value(st7701s->reset, 0);
		mdelay(10);
		gpiod_set_value(st7701s->reset, 1);
		mdelay(120);
	}

	st7701s->prepared = true;

	return 0;
}

static int st7701s_panel_unprepare(struct drm_panel *panel)
{
	struct st7701s_panel *st7701s = to_st7701s_panel(panel);
	struct device *dev = &st7701s->dsi->dev;

	if (!st7701s->prepared)
		return 0;

	if (st7701s->enabled) {
		DRM_DEV_ERROR(dev, "Panel still enabled!\n");
		return -EPERM;
	}

	if (st7701s->reset != NULL) {
		gpiod_set_value(st7701s->reset, 0);
		mdelay(1);
		gpiod_set_value(st7701s->reset, 1);
	}

	st7701s->prepared = false;

	return 0;
}

static int st7701s_panel_enable(struct drm_panel *panel)
{
	struct st7701s_panel *st7701s = to_st7701s_panel(panel);
	struct mipi_dsi_device *dsi = st7701s->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	if (st7701s->enabled)
		return 0;

	if (!st7701s->prepared) {
		DRM_DEV_ERROR(dev, "Panel not prepared!\n");
		return -EPERM;
	}

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	ret = mipi_dsi_dcs_exit_sleep_mode(dsi);
	if (ret < 0) {
		DRM_DEV_ERROR(dev, "Failed to exit sleep mode (%d)\n", ret);
		goto fail;
	}

	mdelay(120);

	ret = st7701s_panel_push_cmd_list(dsi);
	if (ret < 0) {
		DRM_DEV_ERROR(dev, "Failed to send init cmds (%d)\n", ret);
		goto fail;
	}

	mdelay(5);

	ret = mipi_dsi_dcs_set_display_on(dsi);
	if (ret < 0) {
		DRM_DEV_ERROR(dev, "Failed to set display ON (%d)\n", ret);
		goto fail;
	}

	// st7701s->backlight->props.power = FB_BLANK_UNBLANK;
	// backlight_update_status(st7701s->backlight);

	st7701s->enabled = true;

	return 0;

fail:
	if (st7701s->reset != NULL)
		gpiod_set_value(st7701s->reset, 0);

	return ret;
}

static int st7701s_panel_disable(struct drm_panel *panel)
{
	struct st7701s_panel *st7701s = to_st7701s_panel(panel);
	struct mipi_dsi_device *dsi = st7701s->dsi;
	struct device *dev = &dsi->dev;
	int ret;

	if (!st7701s->enabled)
		return 0;

	dsi->mode_flags |= MIPI_DSI_MODE_LPM;

	ret = mipi_dsi_dcs_set_display_off(dsi);
	if (ret < 0) {
		DRM_DEV_ERROR(dev, "Failed to set display OFF (%d)\n", ret);
		return ret;
	}

	usleep_range(5000, 10000);

	ret = mipi_dsi_dcs_enter_sleep_mode(dsi);
	if (ret < 0) {
		DRM_DEV_ERROR(dev, "Failed to enter sleep mode (%d)\n", ret);
		return ret;
	}

	usleep_range(10000, 15000);

	// st7701s->backlight->props.power = FB_BLANK_POWERDOWN;
	// backlight_update_status(st7701s->backlight);

	st7701s->enabled = false;

	return 0;
}

static int st7701s_panel_get_modes(struct drm_panel *panel,
					struct drm_connector *connector)
{
	struct st7701s_panel *st7701s = to_st7701s_panel(panel);
	struct device *dev = &st7701s->dsi->dev;
	connector->connector_type = panel->connector_type;
	struct drm_display_mode *mode;
	u32 *bus_flags = &connector->display_info.bus_flags;
	int ret;

	mode = drm_mode_create(connector->dev);
	if (!mode) {
		DRM_DEV_ERROR(dev, "Failed to create display mode!\n");
		return 0;
	}

	drm_display_mode_from_videomode(&st7701s->vm, mode);
	mode->width_mm = st7701s->width_mm;
	mode->height_mm = st7701s->height_mm;
	connector->display_info.width_mm = st7701s->width_mm;
	connector->display_info.height_mm = st7701s->height_mm;
	mode->type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED;

	if (st7701s->vm.flags & DISPLAY_FLAGS_DE_HIGH)
		*bus_flags |= DRM_BUS_FLAG_DE_HIGH;
	if (st7701s->vm.flags & DISPLAY_FLAGS_DE_LOW)
		*bus_flags |= DRM_BUS_FLAG_DE_LOW;
	if (st7701s->vm.flags & DISPLAY_FLAGS_PIXDATA_NEGEDGE)
		*bus_flags |= DRM_BUS_FLAG_PIXDATA_DRIVE_NEGEDGE;
	if (st7701s->vm.flags & DISPLAY_FLAGS_PIXDATA_POSEDGE)
		*bus_flags |= DRM_BUS_FLAG_PIXDATA_DRIVE_POSEDGE;

	ret = drm_display_info_set_bus_formats(&connector->display_info,
			st7701s_bus_formats, ARRAY_SIZE(st7701s_bus_formats));
	if (ret)
		return ret;

	drm_mode_probed_add(connector, mode);

	return 1;
}

static int st7701s_bl_get_brightness(struct backlight_device *bl)
{
	struct mipi_dsi_device *dsi = bl_get_data(bl);
	struct st7701s_panel *st7701s = mipi_dsi_get_drvdata(dsi);
	struct device *dev = &dsi->dev;
	u16 brightness;
	int ret;

	if (!st7701s->prepared)
		return 0;

	DRM_DEV_DEBUG_DRIVER(dev, "\n");

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	ret = mipi_dsi_dcs_get_display_brightness(dsi, &brightness);
	if (ret < 0)
		return ret;

	bl->props.brightness = brightness;

	return brightness & 0xff;
}

static int st7701s_bl_update_status(struct backlight_device *bl)
{
	struct mipi_dsi_device *dsi = bl_get_data(bl);
	struct st7701s_panel *st7701s = mipi_dsi_get_drvdata(dsi);
	struct device *dev = &dsi->dev;
	int ret = 0;

	if (!st7701s->prepared)
		return 0;

	DRM_DEV_DEBUG_DRIVER(dev, "New brightness: %d\n", bl->props.brightness);

	dsi->mode_flags &= ~MIPI_DSI_MODE_LPM;

	ret = mipi_dsi_dcs_set_display_brightness(dsi, bl->props.brightness);
	if (ret < 0)
		return ret;

	return 0;
}

static const struct backlight_ops st7701s_bl_ops = {
	.update_status = st7701s_bl_update_status,
	.get_brightness = st7701s_bl_get_brightness,
};

static const struct drm_panel_funcs st7701s_panel_funcs = {
	.prepare = st7701s_panel_prepare,
	.unprepare = st7701s_panel_unprepare,
	.enable = st7701s_panel_enable,
	.disable = st7701s_panel_disable,
	.get_modes = st7701s_panel_get_modes,
};

/*
 * The clock might range from 66MHz (30Hz refresh rate)
 * to 132MHz (60Hz refresh rate)
 */
static const struct display_timing st7701s_default_timing = {
	.pixelclock = {17500000, 19000000, 16632000}, //Hz : htotal*vtotal*60 = 23 908 800 Hz
	.hactive = { 480, 480, 480 },
	.hfront_porch = { 16, 48, 8 },
	.hsync_len = { 40, 32, 32 },
	.hback_porch = { 56, 80, 40 },
	.vactive = { 480, 480, 480 },
	.vfront_porch = { 3, 3, 1 },
	.vsync_len = { 10, 10, 8 },
	.vback_porch = { 7, 6, 6 },
	.flags = DISPLAY_FLAGS_HSYNC_LOW |
		 DISPLAY_FLAGS_VSYNC_LOW |
		 DISPLAY_FLAGS_DE_LOW|
		 DISPLAY_FLAGS_PIXDATA_NEGEDGE,
};

static int st7701s_panel_probe(struct mipi_dsi_device *dsi)
{

	struct device *dev = &dsi->dev;
	struct device_node *np = dev->of_node;
	struct device_node *timings;
	struct st7701s_panel *panel;
	// struct backlight_properties bl_props;
	int ret;
	u32 video_mode;
	panel = devm_kzalloc(&dsi->dev, sizeof(*panel), GFP_KERNEL);
	if (!panel)
		return -ENOMEM;

	mipi_dsi_set_drvdata(dsi, panel);

	panel->dsi = dsi;

	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags =  MIPI_DSI_MODE_VIDEO_HSE | MIPI_DSI_MODE_VIDEO |
			   MIPI_DSI_CLOCK_NON_CONTINUOUS;

	ret = of_property_read_u32(np, "video-mode", &video_mode);
	if (!ret) {
		switch (video_mode) {
		case 0:
			/* burst mode */
			dsi->mode_flags |= MIPI_DSI_MODE_VIDEO_BURST;
			break;
		case 1:
			/* non-burst mode with sync event */
			break;
		case 2:
			/* non-burst mode with sync pulse */
			dsi->mode_flags |= MIPI_DSI_MODE_VIDEO_SYNC_PULSE;
			break;
		default:
			dev_warn(dev, "invalid video mode %d\n", video_mode);
			break;

		}
	}

	ret = of_property_read_u32(np, "dsi-lanes", &dsi->lanes);
	if (ret < 0) {
		dev_err(dev, "Failed to get dsi-lanes property (%d)\n", ret);
		return ret;
	}

	/*
	 * 'display-timings' is optional, so verify if the node is present
	 * before calling of_get_videomode so we won't get console error
	 * messages
	 */
	timings = of_get_child_by_name(np, "display-timings");
	if (timings) {
		of_node_put(timings);
		ret = of_get_videomode(np, &panel->vm, 0);
	} else {
		videomode_from_timing(&st7701s_default_timing, &panel->vm);
	}
	if (ret < 0)
		return ret;

	of_property_read_u32(np, "panel-width-mm", &panel->width_mm);
	of_property_read_u32(np, "panel-height-mm", &panel->height_mm);

	panel->reset = devm_gpiod_get(dev, "reset", GPIOD_OUT_HIGH);

	if (IS_ERR(panel->reset)) {
		panel->reset = NULL;
	}
	else
		gpiod_set_value(panel->reset, 0);


	// memset(&bl_props, 0, sizeof(bl_props));
	// bl_props.type = BACKLIGHT_RAW;
	// bl_props.brightness = 255;
	// bl_props.max_brightness = 255;

	// panel->backlight = devm_backlight_device_register(
	// 			dev, dev_name(dev),
	// 			dev, dsi,
	// 			&st7701s_bl_ops, &bl_props);
	// if (IS_ERR(panel->backlight)) {
	// 	ret = PTR_ERR(panel->backlight);
	// 	dev_err(dev, "Failed to register backlight (%d)\n", ret);
	// 	return ret;
	// }

	drm_panel_init(&panel->base, &dsi->dev, &st7701s_panel_funcs,
			DRM_MODE_CONNECTOR_DSI);
	panel->base.funcs = &st7701s_panel_funcs;
	panel->base.dev = dev;

	drm_panel_add(&panel->base);

	ret = mipi_dsi_attach(dsi);
	if (ret < 0)
	{
		drm_panel_remove(&panel->base);
	}
	return ret;
}

static void st7701s_panel_remove(struct mipi_dsi_device *dsi)
{
	struct st7701s_panel *st7701s = mipi_dsi_get_drvdata(dsi);
	struct device *dev = &dsi->dev;
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		DRM_DEV_ERROR(dev, "Failed to detach from host (%d)\n",
			ret);

	drm_panel_disable(&st7701s->base);

	if (st7701s->base.dev)
		drm_panel_remove(&st7701s->base);

//	return 0;
}

static void st7701s_panel_shutdown(struct mipi_dsi_device *dsi)
{
	struct st7701s_panel *st7701s= mipi_dsi_get_drvdata(dsi);

	st7701s_panel_disable(&st7701s->base);
	st7701s_panel_unprepare(&st7701s->base);//some panel do not need;
}

static const struct of_device_id st7701s_of_match[] = {
	{ .compatible = "sitronix,st7701s", },
	{ }
};
MODULE_DEVICE_TABLE(of, st7701s_of_match);

static struct mipi_dsi_driver st7701s_panel_driver = {
	.driver = {
		.name = "panel-sirtonix-st7701s",
		.of_match_table = st7701s_of_match,
	},
	.probe = st7701s_panel_probe,
	.remove = st7701s_panel_remove,
	.shutdown = st7701s_panel_shutdown,
};
module_mipi_dsi_driver(st7701s_panel_driver);

MODULE_AUTHOR("Fangkai <fangkai@kyee.com.cn>");
MODULE_DESCRIPTION("sitronix st7701s lcd driver");
MODULE_LICENSE("GPL v2");

