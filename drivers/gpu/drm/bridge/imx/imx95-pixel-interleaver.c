// SPDX-License-Identifier: GPL-2.0+

/*
 * Copyright 2023 NXP
 */

#include <linux/bitfield.h>
#include <linux/bits.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/media-bus-format.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_graph.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>

#include <drm/drm_atomic_state_helper.h>
#include <drm/drm_bridge.h>

#define PIXEL_INTERLEAVER_CTRL	0x4
#define  DISP_IN_SEL		BIT(1)
#define  MODE			BIT(0)

#define CTRL			0x0
#define  VSYNC_POLARITY		BIT(10)
#define  HSYNC_POLARITY		BIT(9)

#define SWRST			0x20
#define  SW_RST			BIT(1)

#define IE			0x30

#define DRIVER_NAME		"imx95-pixel-interleaver"

struct imx95_pixel_interleaver_bridge {
	struct drm_bridge bridge;
	struct drm_bridge *next_bridge;
	struct device *dev;
	void __iomem *regs;
	struct regmap *regmap;
	struct clk *clk_bus;
};

static void
imx95_pixel_interleaver_bridge_sw_reset(struct imx95_pixel_interleaver_bridge *pi)
{
	clk_prepare_enable(pi->clk_bus);

	writel(SW_RST, pi->regs + SWRST);
	usleep_range(10, 20);
	writel(0, pi->regs + SWRST);

	clk_disable_unprepare(pi->clk_bus);
}

static int
imx95_pixel_interleaver_bridge_attach(struct drm_bridge *bridge,
					     struct drm_encoder *encoder,
					     enum drm_bridge_attach_flags flags)
{
	struct imx95_pixel_interleaver_bridge *pi = bridge->driver_private;

	if (!(flags & DRM_BRIDGE_ATTACH_NO_CONNECTOR)) {
		dev_err(pi->dev, "do not support creating a drm_connector\n");
		return -EINVAL;
	}

	return drm_bridge_attach(encoder, pi->next_bridge, bridge,
				 DRM_BRIDGE_ATTACH_NO_CONNECTOR);
}

static void
imx95_pixel_interleaver_bridge_mode_set(struct drm_bridge *bridge,
					       const struct drm_display_mode *mode,
					       const struct drm_display_mode *adjusted_mode)
{
	struct imx95_pixel_interleaver_bridge *pi = bridge->driver_private;

	imx95_pixel_interleaver_bridge_sw_reset(pi);

	clk_prepare_enable(pi->clk_bus);

	/* HSYNC and VSYNC are active low. Data Enable is active high */
	writel(HSYNC_POLARITY | VSYNC_POLARITY, pi->regs + CTRL);

	/* Disable interrupts */
	writel(0, pi->regs + IE);

	clk_disable_unprepare(pi->clk_bus);
}

static void
imx95_pixel_interleaver_bridge_enable(struct drm_bridge *bridge)
{
	struct imx95_pixel_interleaver_bridge *pi = bridge->driver_private;

	regmap_write(pi->regmap, PIXEL_INTERLEAVER_CTRL, 0);
}

static u32 *
imx95_pixel_interleaver_bridge_atomic_get_input_bus_fmts(struct drm_bridge *bridge,
							 struct drm_bridge_state *bridge_state,
							 struct drm_crtc_state *crtc_state,
							 struct drm_connector_state *conn_state,
							 u32 output_fmt,
							 unsigned int *num_input_fmts)
{
	u32 *input_fmts;

	if (output_fmt != MEDIA_BUS_FMT_RGB888_1X24)
		return NULL;

	*num_input_fmts = 1;

	input_fmts = kmalloc(sizeof(*input_fmts), GFP_KERNEL);
	if (!input_fmts)
		return NULL;

	input_fmts[0] = MEDIA_BUS_FMT_RGB888_1X24;

	return input_fmts;
}

static const struct drm_bridge_funcs imx95_pixel_interleaver_bridge_funcs = {
	.atomic_duplicate_state	= drm_atomic_helper_bridge_duplicate_state,
	.atomic_destroy_state	= drm_atomic_helper_bridge_destroy_state,
	.atomic_reset		= drm_atomic_helper_bridge_reset,
	.attach			= imx95_pixel_interleaver_bridge_attach,
	.mode_set		= imx95_pixel_interleaver_bridge_mode_set,
	.enable			= imx95_pixel_interleaver_bridge_enable,
	.atomic_get_input_bus_fmts =
				imx95_pixel_interleaver_bridge_atomic_get_input_bus_fmts,
};

static int imx95_pixel_interleaver_bridge_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *remote, *remote_port, *np = dev->of_node;
	struct imx95_pixel_interleaver_bridge *pi;

	pi = devm_drm_bridge_alloc(dev, struct imx95_pixel_interleaver_bridge, bridge,
				   &imx95_pixel_interleaver_bridge_funcs);
	if (IS_ERR(pi))
		return PTR_ERR(pi);

	pi->dev = dev;
	platform_set_drvdata(pdev, pi);

	pi->regs = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(pi->regs))
		return PTR_ERR(pi->regs);

	pi->regmap = syscon_regmap_lookup_by_phandle(np, "fsl,syscon");
	if (IS_ERR(pi->regmap))
		return dev_err_probe(dev, PTR_ERR(pi->regmap), "failed to get regmap\n");

	pi->clk_bus = devm_clk_get(dev, NULL);
	if (IS_ERR(pi->clk_bus))
		return dev_err_probe(dev, PTR_ERR(pi->clk_bus), "failed to get clock\n");

	pi->bridge.driver_private = pi;
	pi->bridge.of_node = np;

	remote = of_graph_get_remote_node(np, 1, 0);
	if (!remote)
		return dev_err_probe(dev, -EINVAL, "no remote node for port@1 endpoint\n");

	remote_port = of_graph_get_port_by_id(remote, 0);
	of_node_put(remote);
	if (!remote_port)
		return dev_err_probe(dev, -EINVAL, "no remote port\n");

	pi->next_bridge = of_drm_find_bridge(remote_port);
	of_node_put(remote_port);
	if (!pi->next_bridge) {
		dev_err(dev, "failed to find next bridge for port@1 endpoint\n");
		return -EPROBE_DEFER;
	}

	imx95_pixel_interleaver_bridge_sw_reset(pi);

	drm_bridge_add(&pi->bridge);

	return 0;
}

static void imx95_pixel_interleaver_bridge_remove(struct platform_device *pdev)
{
	struct imx95_pixel_interleaver_bridge *pi = platform_get_drvdata(pdev);

	drm_bridge_remove(&pi->bridge);
}

static const struct of_device_id imx95_pixel_interleaver_bridge_dt_ids[] = {
	{ .compatible = "fsl,imx95-pixel-interleaver", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, imx95_pixel_interleaver_bridge_dt_ids);

static struct platform_driver imx95_pixel_interleaver_bridge_driver = {
	.probe	= imx95_pixel_interleaver_bridge_probe,
	.remove	= imx95_pixel_interleaver_bridge_remove,
	.driver	= {
		.of_match_table = imx95_pixel_interleaver_bridge_dt_ids,
		.name = DRIVER_NAME,
	},
};

module_platform_driver(imx95_pixel_interleaver_bridge_driver);

MODULE_DESCRIPTION("i.MX95 display pixel interleaver bridge driver");
MODULE_AUTHOR("NXP Semiconductor");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" DRIVER_NAME);
