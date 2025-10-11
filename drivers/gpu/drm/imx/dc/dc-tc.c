// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2024 NXP
 */

#include <linux/component.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/property.h>
#include <linux/regmap.h>

#include "dc-drv.h"
#include "dc-de.h"

#define TCON_CTRL	0x410
#define  CTRL_RST_VAL	0x01401408

/* red: MAPBIT 29-20, green: MAPBIT 19-10, blue: MAPBIT 9-0 */
#define MAPBIT3_0	0x418
#define MAPBIT7_4	0x41c
#define MAPBIT11_8	0x420
#define MAPBIT15_12	0x424
#define MAPBIT19_16	0x428
#define MAPBIT23_20	0x42c
#define MAPBIT27_24	0x430
#define MAPBIT31_28	0x434

struct dc_tc_subdev_match_data {
	bool				need_config;
	const struct dc_subdev_info	*info;
};

static const struct dc_subdev_info dc_tc_info_imx8qxp[] = {
	{ .reg_start = 0x5618c800, .id = 0, },
	{ .reg_start = 0x5618e400, .id = 1, },
	{ /* sentinel */ },
};

static const struct dc_subdev_info dc_tc_info_imx95[] = {
	{ .reg_start = 0x4b711000, .id = 0, },
	{ .reg_start = 0x4b771000, .id = 1, },
	{ /* sentinel */ },
};

static const struct regmap_range dc_tc_regmap_ranges[] = {
	regmap_reg_range(TCON_CTRL, TCON_CTRL),
	regmap_reg_range(MAPBIT3_0, MAPBIT31_28),
};

static const struct regmap_access_table dc_tc_regmap_access_table = {
	.yes_ranges = dc_tc_regmap_ranges,
	.n_yes_ranges = ARRAY_SIZE(dc_tc_regmap_ranges),
};

static const struct regmap_config dc_tc_regmap_config = {
	.reg_bits = 32,
	.reg_stride = 4,
	.val_bits = 32,
	.fast_io = true,
	.wr_table = &dc_tc_regmap_access_table,
	.rd_table = &dc_tc_regmap_access_table,
	.max_register = MAPBIT31_28,
};

/*
 * The pixels reach TCON are always in 30-bit BGR format.
 * The first bridge always receives pixels in 30-bit RGB format.
 * So, map the format to MEDIA_BUS_FMT_RGB101010_1X30.
 */
static const u32 dc_tc_mapbit[] = {
	0x17161514, 0x1b1a1918, 0x0b0a1d1c, 0x0f0e0d0c,
	0x13121110, 0x03020100, 0x07060504, 0x00000908,
};

static const struct dc_tc_subdev_match_data dc_tc_match_data_imx8qxp = {
	.need_config = true,
	.info = dc_tc_info_imx8qxp,
};

static const struct dc_tc_subdev_match_data dc_tc_match_data_imx95 = {
	.need_config = false,
	.info = dc_tc_info_imx95,
};

void dc_tc_init(struct dc_tc *tc)
{
	if (!tc->need_config)
		return;

	/* reset TCON_CTRL to POR default so that TCON works in bypass mode */
	regmap_write(tc->reg, TCON_CTRL, CTRL_RST_VAL);

	/* set format */
	regmap_bulk_write(tc->reg, MAPBIT3_0, dc_tc_mapbit,
			  ARRAY_SIZE(dc_tc_mapbit));
}

static int dc_tc_bind(struct device *dev, struct device *master, void *data)
{
	const struct dc_tc_subdev_match_data *dc_tc_match_data = device_get_match_data(dev);
	const struct dc_subdev_info *dc_tc_info = dc_tc_match_data->info;
	struct platform_device *pdev = to_platform_device(dev);
	struct dc_drm_device *dc_drm = data;
	struct resource *res;
	void __iomem *base;
	struct dc_tc *tc;
	int id;

	tc = devm_kzalloc(dev, sizeof(*tc), GFP_KERNEL);
	if (!tc)
		return -ENOMEM;

	if (dc_tc_match_data->need_config) {
		base = devm_platform_get_and_ioremap_resource(pdev, 0, &res);
		if (IS_ERR(base))
			return PTR_ERR(base);

		tc->reg = devm_regmap_init_mmio(dev, base, &dc_tc_regmap_config);
		if (IS_ERR(tc->reg))
			return PTR_ERR(tc->reg);
	} else {
		res = platform_get_resource(to_platform_device(pdev->dev.parent), IORESOURCE_MEM, 0);
		if (IS_ERR(res))
			return PTR_ERR(res);
	}

	id = dc_subdev_get_id(dc_tc_info, res);
	if (id < 0) {
		dev_err(dev, "failed to get instance number: %d\n", id);
		return id;
	}

	tc->dev = dev;
	dc_drm->tc[id] = tc;

	return 0;
}

static const struct component_ops dc_tc_ops = {
	.bind = dc_tc_bind,
};

static int dc_tc_probe(struct platform_device *pdev)
{
	int ret;

	ret = component_add(&pdev->dev, &dc_tc_ops);
	if (ret)
		return dev_err_probe(&pdev->dev, ret,
				     "failed to add component\n");

	return 0;
}

static void dc_tc_remove(struct platform_device *pdev)
{
	component_del(&pdev->dev, &dc_tc_ops);
}

static const struct of_device_id dc_tc_dt_ids[] = {
	{ .compatible = "fsl,imx8qxp-dc-tcon", .data = &dc_tc_match_data_imx8qxp },
	{ .compatible = "fsl,imx95-dc-tcon", .data = &dc_tc_match_data_imx95 },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, dc_tc_dt_ids);

struct platform_driver dc_tc_driver = {
	.probe = dc_tc_probe,
	.remove = dc_tc_remove,
	.driver = {
		.name = "imx8-dc-tcon",
		.suppress_bind_attrs = true,
		.of_match_table = dc_tc_dt_ids,
	},
};
