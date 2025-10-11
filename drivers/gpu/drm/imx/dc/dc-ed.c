// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2024 NXP
 */

#include <linux/bitfield.h>
#include <linux/bits.h>
#include <linux/component.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/property.h>
#include <linux/regmap.h>

#include "dc-drv.h"
#include "dc-pe.h"

#define PIXENGCFG_STATIC	0x8
#define  DIV_MASK		GENMASK(23, 16)
#define  DIV(x)			FIELD_PREP(DIV_MASK, (x))
#define  DIV_RESET		0x80
#define  SYNC_MODE		BIT(8)
#define  SINGLE			0
#define  POWERDOWN		BIT(4)

#define PIXENGCFG_DYNAMIC	0xc

#define PIXENGCFG_TRIGGER	0x14
#define  SYNC_TRIGGER		BIT(0)

#define STATICCONTROL		0x8
#define  PERFCOUNTMODE		BIT(12)
#define  KICK_MODE		BIT(8)
#define  EXTERNAL		BIT(8)

#define CONTROL			0xc
#define  GAMMAAPPLYENABLE	BIT(0)

struct dc_ed_subdev_match_data {
	const enum dc_link_id		*src_sels;
	const struct dc_subdev_info	*info;
};

static const struct dc_subdev_info dc_ed_info_imx8qxp[] = {
	{ .reg_start = 0x56180980, .id = 0, },
	{ .reg_start = 0x56180a00, .id = 1, },
	{ .reg_start = 0x561809c0, .id = 4, },
	{ .reg_start = 0x56180a40, .id = 5, },
	{ /* sentinel */ },
};

static const struct dc_subdev_info dc_ed_info_imx95[] = {
	{ .reg_start = 0x4b511000, .id = 0, },
	{ .reg_start = 0x4b551000, .id = 1, },
	{ .reg_start = 0x4b521000, .id = 4, },
	{ .reg_start = 0x4b561000, .id = 5, },
	{ /* sentinel */ },
};

static const struct regmap_range dc_ed_pec_regmap_write_ranges[] = {
	regmap_reg_range(PIXENGCFG_STATIC, PIXENGCFG_STATIC),
	regmap_reg_range(PIXENGCFG_DYNAMIC, PIXENGCFG_DYNAMIC),
	regmap_reg_range(PIXENGCFG_TRIGGER, PIXENGCFG_TRIGGER),
};

static const struct regmap_access_table dc_ed_pec_regmap_write_table = {
	.yes_ranges = dc_ed_pec_regmap_write_ranges,
	.n_yes_ranges = ARRAY_SIZE(dc_ed_pec_regmap_write_ranges),
};

static const struct regmap_range dc_ed_pec_regmap_read_ranges[] = {
	regmap_reg_range(PIXENGCFG_STATIC, PIXENGCFG_STATIC),
	regmap_reg_range(PIXENGCFG_DYNAMIC, PIXENGCFG_DYNAMIC),
};

static const struct regmap_access_table dc_ed_pec_regmap_read_table = {
	.yes_ranges = dc_ed_pec_regmap_read_ranges,
	.n_yes_ranges = ARRAY_SIZE(dc_ed_pec_regmap_read_ranges),
};

static const struct regmap_range dc_ed_pec_regmap_volatile_ranges[] = {
	regmap_reg_range(PIXENGCFG_TRIGGER, PIXENGCFG_TRIGGER),
};

static const struct regmap_access_table dc_ed_pec_regmap_volatile_table = {
	.yes_ranges = dc_ed_pec_regmap_volatile_ranges,
	.n_yes_ranges = ARRAY_SIZE(dc_ed_pec_regmap_volatile_ranges),
};

static const struct regmap_config dc_ed_pec_regmap_config = {
	.name = "pec",
	.reg_bits = 32,
	.reg_stride = 4,
	.val_bits = 32,
	.fast_io = true,
	.wr_table = &dc_ed_pec_regmap_write_table,
	.rd_table = &dc_ed_pec_regmap_read_table,
	.volatile_table = &dc_ed_pec_regmap_volatile_table,
	.max_register = PIXENGCFG_TRIGGER,
};

static const struct regmap_range dc_ed_regmap_ranges[] = {
	regmap_reg_range(STATICCONTROL, STATICCONTROL),
	regmap_reg_range(CONTROL, CONTROL),
};

static const struct regmap_access_table dc_ed_regmap_access_table = {
	.yes_ranges = dc_ed_regmap_ranges,
	.n_yes_ranges = ARRAY_SIZE(dc_ed_regmap_ranges),
};

static const struct regmap_config dc_ed_cfg_regmap_config = {
	.name = "cfg",
	.reg_bits = 32,
	.reg_stride = 4,
	.val_bits = 32,
	.fast_io = true,
	.wr_table = &dc_ed_regmap_access_table,
	.rd_table = &dc_ed_regmap_access_table,
	.max_register = CONTROL,
};

static const enum dc_link_id src_sels_imx8qxp[] = {
	LINK_ID_NONE,
	LINK_ID_CONSTFRAME0,
	LINK_ID_CONSTFRAME1,
	LINK_ID_CONSTFRAME4_MX8QXP,
	LINK_ID_CONSTFRAME5_MX8QXP,
	LINK_ID_LAYERBLEND3_MX8QXP,
	LINK_ID_LAYERBLEND2_MX8QXP,
	LINK_ID_LAYERBLEND1_MX8QXP,
	LINK_ID_LAYERBLEND0_MX8QXP,
	LINK_ID_LAST	/* sentinel */
};

static const enum dc_link_id src_sels_imx95[] = {
	LINK_ID_NONE,
	LINK_ID_LAYERBLEND5_MX95,
	LINK_ID_LAYERBLEND4_MX95,
	LINK_ID_LAYERBLEND3_MX95,
	LINK_ID_LAYERBLEND2_MX95,
	LINK_ID_LAYERBLEND1_MX95,
	LINK_ID_LAYERBLEND0_MX95,
	LINK_ID_LAST	/* sentinel */
};

static const struct dc_ed_subdev_match_data dc_ed_match_data_imx8qxp = {
	.src_sels = src_sels_imx8qxp,
	.info = dc_ed_info_imx8qxp,
};

static const struct dc_ed_subdev_match_data dc_ed_match_data_imx95 = {
	.src_sels = src_sels_imx95,
	.info = dc_ed_info_imx95,
};

static inline void dc_ed_pec_enable_shden(struct dc_ed *ed)
{
	regmap_write_bits(ed->reg_pec, PIXENGCFG_STATIC, SHDEN, SHDEN);
}

static inline void dc_ed_pec_poweron(struct dc_ed *ed)
{
	regmap_write_bits(ed->reg_pec, PIXENGCFG_STATIC, POWERDOWN, 0);
}

static inline void dc_ed_pec_sync_mode_single(struct dc_ed *ed)
{
	regmap_write_bits(ed->reg_pec, PIXENGCFG_STATIC, SYNC_MODE, SINGLE);
}

static inline void dc_ed_pec_div_reset(struct dc_ed *ed)
{
	regmap_write_bits(ed->reg_pec, PIXENGCFG_STATIC, DIV_MASK,
			  DIV(DIV_RESET));
}

void dc_ed_pec_src_sel(struct dc_ed *ed, enum dc_link_id src)
{
	int i = 0;

	while (ed->src_sels[i] != LINK_ID_LAST) {
		if (ed->src_sels[i++] == src) {
			regmap_write(ed->reg_pec, PIXENGCFG_DYNAMIC, src);
			return;
		}
	}
}

void dc_ed_pec_sync_trigger(struct dc_ed *ed)
{
	regmap_write(ed->reg_pec, PIXENGCFG_TRIGGER, SYNC_TRIGGER);
}

static inline void dc_ed_enable_shden(struct dc_ed *ed)
{
	regmap_write_bits(ed->reg_cfg, STATICCONTROL, SHDEN, SHDEN);
}

static inline void dc_ed_kick_mode_external(struct dc_ed *ed)
{
	regmap_write_bits(ed->reg_cfg, STATICCONTROL, KICK_MODE, EXTERNAL);
}

static inline void dc_ed_disable_perfcountmode(struct dc_ed *ed)
{
	regmap_write_bits(ed->reg_cfg, STATICCONTROL, PERFCOUNTMODE, 0);
}

static inline void dc_ed_disable_gamma_apply(struct dc_ed *ed)
{
	regmap_write_bits(ed->reg_cfg, CONTROL, GAMMAAPPLYENABLE, 0);
}

void dc_ed_init(struct dc_ed *ed)
{
	dc_ed_pec_src_sel(ed, LINK_ID_NONE);
	dc_ed_pec_enable_shden(ed);
	dc_ed_pec_poweron(ed);
	dc_ed_pec_sync_mode_single(ed);
	dc_ed_pec_div_reset(ed);
	dc_ed_enable_shden(ed);
	dc_ed_disable_perfcountmode(ed);
	dc_ed_kick_mode_external(ed);
	dc_ed_disable_gamma_apply(ed);
}

static int dc_ed_bind(struct device *dev, struct device *master, void *data)
{
	const struct dc_ed_subdev_match_data *dc_ed_match_data = device_get_match_data(dev);
	const struct dc_subdev_info *dc_ed_info = dc_ed_match_data->info;
	struct platform_device *pdev = to_platform_device(dev);
	struct dc_drm_device *dc_drm = data;
	struct resource *res_pec;
	void __iomem *base_pec;
	void __iomem *base_cfg;
	struct dc_ed *ed;
	int id;

	ed = devm_kzalloc(dev, sizeof(*ed), GFP_KERNEL);
	if (!ed)
		return -ENOMEM;

	base_pec = devm_platform_get_and_ioremap_resource(pdev, 0, &res_pec);
	if (IS_ERR(base_pec))
		return PTR_ERR(base_pec);

	base_cfg = devm_platform_ioremap_resource_byname(pdev, "cfg");
	if (IS_ERR(base_cfg))
		return PTR_ERR(base_cfg);

	ed->reg_pec = devm_regmap_init_mmio(dev, base_pec,
					    &dc_ed_pec_regmap_config);
	if (IS_ERR(ed->reg_pec))
		return PTR_ERR(ed->reg_pec);

	ed->reg_cfg = devm_regmap_init_mmio(dev, base_cfg,
					    &dc_ed_cfg_regmap_config);
	if (IS_ERR(ed->reg_cfg))
		return PTR_ERR(ed->reg_cfg);

	ed->irq_shdload = platform_get_irq_byname(pdev, "shdload");
	if (ed->irq_shdload < 0)
		return ed->irq_shdload;

	ed->dev = dev;
	ed->src_sels = dc_ed_match_data->src_sels;

	id = dc_subdev_get_id(dc_ed_info, res_pec);
	if (id < 0) {
		dev_err(dev, "failed to get instance number: %d\n", id);
		return id;
	}

	switch (id) {
	case 0:
		dc_drm->ed_cont[0] = ed;
		break;
	case 1:
		dc_drm->ed_cont[1] = ed;
		break;
	case 4:
		dc_drm->ed_safe[0] = ed;
		break;
	case 5:
		dc_drm->ed_safe[1] = ed;
		break;
	}

	return 0;
}

static const struct component_ops dc_ed_ops = {
	.bind = dc_ed_bind,
};

static int dc_ed_probe(struct platform_device *pdev)
{
	int ret;

	ret = component_add(&pdev->dev, &dc_ed_ops);
	if (ret)
		return dev_err_probe(&pdev->dev, ret,
				     "failed to add component\n");

	return 0;
}

static void dc_ed_remove(struct platform_device *pdev)
{
	component_del(&pdev->dev, &dc_ed_ops);
}

static const struct of_device_id dc_ed_dt_ids[] = {
	{ .compatible = "fsl,imx8qxp-dc-extdst", .data = &dc_ed_match_data_imx8qxp },
	{ .compatible = "fsl,imx95-dc-extdst", .data = &dc_ed_match_data_imx95 },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, dc_ed_dt_ids);

struct platform_driver dc_ed_driver = {
	.probe = dc_ed_probe,
	.remove = dc_ed_remove,
	.driver = {
		.name = "imx8-dc-extdst",
		.suppress_bind_attrs = true,
		.of_match_table = dc_ed_dt_ids,
	},
};
