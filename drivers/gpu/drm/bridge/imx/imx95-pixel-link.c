// SPDX-License-Identifier: GPL-2.0+

/*
 * Copyright 2023 NXP
 */

#include <linux/bits.h>
#include <linux/media-bus-format.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_graph.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>

#include <drm/drm_atomic_state_helper.h>
#include <drm/drm_bridge.h>

#define CTRL		0x8
#define  PL_VALID(n)	BIT(1 + 4 * (n))
#define  PL_ENABLE(n)	BIT(4 * (n))

#define OUT_ENDPOINTS	2

#define DRIVER_NAME	"imx95-pixel-link"

struct imx95_pixel_link_bridge {
	struct drm_bridge bridge;
	struct drm_bridge *next_bridge;
	struct device *dev;
	struct regmap *regmap;
	u8 stream_id;
};

static int imx95_pixel_link_bridge_attach(struct drm_bridge *bridge,
					  struct drm_encoder *encoder,
					  enum drm_bridge_attach_flags flags)
{
	struct imx95_pixel_link_bridge *pl = bridge->driver_private;

	if (!(flags & DRM_BRIDGE_ATTACH_NO_CONNECTOR)) {
		dev_err(pl->dev, "do not support creating a drm_connector\n");
		return -EINVAL;
	}

	return drm_bridge_attach(encoder, pl->next_bridge, bridge,
				 DRM_BRIDGE_ATTACH_NO_CONNECTOR);
}

static void imx95_pixel_link_bridge_disable(struct drm_bridge *bridge)
{
	struct imx95_pixel_link_bridge *pl = bridge->driver_private;
	unsigned int id = pl->stream_id;

	regmap_update_bits(pl->regmap, CTRL, PL_ENABLE(id), 0);
	regmap_update_bits(pl->regmap, CTRL, PL_VALID(id), 0);
}

static void imx95_pixel_link_bridge_enable(struct drm_bridge *bridge)
{
	struct imx95_pixel_link_bridge *pl = bridge->driver_private;
	unsigned int id = pl->stream_id;

	regmap_update_bits(pl->regmap, CTRL, PL_VALID(id), PL_VALID(id));
	regmap_update_bits(pl->regmap, CTRL, PL_ENABLE(id), PL_ENABLE(id));
}

static u32 *
imx95_pixel_link_bridge_atomic_get_input_bus_fmts(struct drm_bridge *bridge,
						  struct drm_bridge_state *bridge_state,
						  struct drm_crtc_state *crtc_state,
						  struct drm_connector_state *conn_state,
						  u32 output_fmt,
						  unsigned int *num_input_fmts)
{
	u32 *input_fmts;

	if (output_fmt != MEDIA_BUS_FMT_RGB888_1X36_CPADLO &&
	    output_fmt != MEDIA_BUS_FMT_FIXED)
		return NULL;

	*num_input_fmts = 1;

	input_fmts = kmalloc(sizeof(*input_fmts), GFP_KERNEL);
	if (!input_fmts)
		return NULL;

	input_fmts[0] = MEDIA_BUS_FMT_RGB888_1X24;

	return input_fmts;
}

static const struct drm_bridge_funcs imx95_pixel_link_bridge_funcs = {
	.atomic_duplicate_state	= drm_atomic_helper_bridge_duplicate_state,
	.atomic_destroy_state	= drm_atomic_helper_bridge_destroy_state,
	.atomic_reset		= drm_atomic_helper_bridge_reset,
	.attach			= imx95_pixel_link_bridge_attach,
	.disable		= imx95_pixel_link_bridge_disable,
	.enable			= imx95_pixel_link_bridge_enable,
	.atomic_get_input_bus_fmts =
				imx95_pixel_link_bridge_atomic_get_input_bus_fmts,
};

static int imx95_pixel_link_bridge_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *remote, *np = dev->of_node;
	struct imx95_pixel_link_bridge *pl;
	int i, ret;

	pl = devm_drm_bridge_alloc(dev, struct imx95_pixel_link_bridge, bridge,
				   &imx95_pixel_link_bridge_funcs);
	if (IS_ERR(pl))
		return PTR_ERR(pl);

	pl->dev = dev;
	platform_set_drvdata(pdev, pl);

	pl->regmap = syscon_regmap_lookup_by_phandle(np, "fsl,syscon");
	if (IS_ERR(pl->regmap))
		return dev_err_probe(dev, PTR_ERR(pl->regmap), "failed to get regmap\n");

	ret = of_property_read_u8(np, "fsl,dc-stream-id", &pl->stream_id);
	if (ret)
		return dev_err_probe(dev, ret, "failed to get stream index\n");

	pl->bridge.driver_private = pl;
	pl->bridge.of_node = of_graph_get_port_by_id(np, 0);
	if (!pl->bridge.of_node)
		return dev_err_probe(dev, -ENODEV, "failed to get port@0\n");
	of_node_put(pl->bridge.of_node);

	for (i = 0; i < OUT_ENDPOINTS; i++) {
		remote = of_graph_get_remote_node(np, 1, i);
		if (!remote) {
			dev_dbg(dev, "no remote node for port@1 ep%u\n", i);
			continue;
		}

		pl->next_bridge = of_drm_find_bridge(remote);
		if (!pl->next_bridge) {
			dev_dbg(dev, "failed to find next bridge for port@1 ep%u\n", i);
			of_node_put(remote);
			return -EPROBE_DEFER;
		}

		of_node_put(remote);

		drm_bridge_add(&pl->bridge);

		return 0;
	}

	return -EINVAL;
}

static void imx95_pixel_link_bridge_remove(struct platform_device *pdev)
{
	struct imx95_pixel_link_bridge *pl = platform_get_drvdata(pdev);

	drm_bridge_remove(&pl->bridge);
}

static const struct of_device_id imx95_pixel_link_bridge_dt_ids[] = {
	{ .compatible = "fsl,imx95-dc-pixel-link", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, imx95_pixel_link_bridge_dt_ids);

static struct platform_driver imx95_pixel_link_bridge_driver = {
	.probe	= imx95_pixel_link_bridge_probe,
	.remove	= imx95_pixel_link_bridge_remove,
	.driver	= {
		.of_match_table = imx95_pixel_link_bridge_dt_ids,
		.name = DRIVER_NAME,
	},
};

module_platform_driver(imx95_pixel_link_bridge_driver);

MODULE_DESCRIPTION("i.MX95 display pixel link bridge driver");
MODULE_AUTHOR("NXP Semiconductor");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" DRIVER_NAME);
