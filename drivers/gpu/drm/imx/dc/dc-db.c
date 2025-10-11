// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2025 Marek Vasut <marek.vasut@mailbox.org>
 */

#include <linux/bitfield.h>
#include <linux/bits.h>
#include <linux/component.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>

#include <drm/drm_blend.h>

#include "dc-drv.h"
#include "dc-pe.h"

#define PIXENGCFG_DYNAMIC			0x8
#define  PIXENGCFG_DYNAMIC_PRIM_SEL_MASK	GENMASK(5, 0)
#define  PIXENGCFG_DYNAMIC_PRIM_SEL(x)		\
		FIELD_PREP(PIXENGCFG_DYNAMIC_PRIM_SEL_MASK, (x))
#define  PIXENGCFG_DYNAMIC_SEC_SEL_MASK		GENMASK(13, 8)
#define  PIXENGCFG_DYNAMIC_SEC_SEL(x)		\
		FIELD_PREP(PIXENGCFG_DYNAMIC_SEC_SEL_MASK, (x))

#define STATICCONTROL				0x8
#define  SHDTOKSEL_MASK				GENMASK(6, 4)
#define  SHDTOKSEL(x)				FIELD_PREP(SHDTOKSEL_MASK, (x))
#define  SHDLDSEL_MASK				GENMASK(3, 1)
#define  SHDLDSEL(x)				FIELD_PREP(SHDLDSEL_MASK, (x))

#define CONTROL					0xc
#define  SHDTOKGEN				BIT(0)

#define MODECONTROL				0x10

#define ALPHACONTROL				0x14
#define  ALPHAMASKENABLE			BIT(0)

#define BLENDCONTROL				0x18
#define  ALPHA_MASK				GENMASK(23, 16)
#define  ALPHA(x)				FIELD_PREP(ALPHA_MASK, (x))
#define  PRIM_C_BLD_FUNC_MASK			GENMASK(2, 0)
#define  PRIM_C_BLD_FUNC(x)			\
		FIELD_PREP(PRIM_C_BLD_FUNC_MASK, (x))
#define  SEC_C_BLD_FUNC_MASK			GENMASK(6, 4)
#define  SEC_C_BLD_FUNC(x)			\
		FIELD_PREP(SEC_C_BLD_FUNC_MASK, (x))
#define  PRIM_A_BLD_FUNC_MASK			GENMASK(10, 8)
#define  PRIM_A_BLD_FUNC(x)			\
		FIELD_PREP(PRIM_A_BLD_FUNC_MASK, (x))
#define  SEC_A_BLD_FUNC_MASK			GENMASK(14, 12)
#define  SEC_A_BLD_FUNC(x)			\
		FIELD_PREP(SEC_A_BLD_FUNC_MASK, (x))

enum dc_db_blend_func {
	DC_DOMAINBLEND_BLEND_ZERO,
	DC_DOMAINBLEND_BLEND_ONE,
	DC_DOMAINBLEND_BLEND_PRIM_ALPHA,
	DC_DOMAINBLEND_BLEND_ONE_MINUS_PRIM_ALPHA,
	DC_DOMAINBLEND_BLEND_SEC_ALPHA,
	DC_DOMAINBLEND_BLEND_ONE_MINUS_SEC_ALPHA,
	DC_DOMAINBLEND_BLEND_CONST_ALPHA,
	DC_DOMAINBLEND_BLEND_ONE_MINUS_CONST_ALPHA,
};

enum dc_db_shadow_sel {
	SW = 0x4,
	SW_PRIM = 0x5,
	SW_SEC = 0x6,
};

static const struct dc_subdev_info dc_db_info[] = {
	{ .reg_start = 0x4b6a0000, .id = 0, },
	{ .reg_start = 0x4b720000, .id = 1, },
};

static const struct regmap_range dc_db_regmap_ranges[] = {
	regmap_reg_range(STATICCONTROL, BLENDCONTROL),
};

static const struct regmap_access_table dc_db_regmap_access_table = {
	.yes_ranges = dc_db_regmap_ranges,
	.n_yes_ranges = ARRAY_SIZE(dc_db_regmap_ranges),
};

static const struct regmap_config dc_db_cfg_regmap_config = {
	.name = "cfg",
	.reg_bits = 32,
	.reg_stride = 4,
	.val_bits = 32,
	.fast_io = true,
	.wr_table = &dc_db_regmap_access_table,
	.rd_table = &dc_db_regmap_access_table,
	.max_register = BLENDCONTROL,
};

enum dc_db_mode {
	DB_PRIMARY,
	DB_SECONDARY,
	DB_BLEND,
	DB_SIDEBYSIDE,
};

static inline void dc_db_enable_shden(struct dc_db *db)
{
	regmap_write_bits(db->reg_cfg, STATICCONTROL, SHDEN, SHDEN);
}

static inline void dc_db_shdtoksel(struct dc_db *db, enum dc_db_shadow_sel sel)
{
	regmap_write_bits(db->reg_cfg, STATICCONTROL, SHDTOKSEL_MASK,
			  SHDTOKSEL(sel));
}

static inline void dc_db_shdldsel(struct dc_db *db, enum dc_db_shadow_sel sel)
{
	regmap_write_bits(db->reg_cfg, STATICCONTROL, SHDLDSEL_MASK,
			  SHDLDSEL(sel));
}

void dc_db_shdtokgen(struct dc_db *db)
{
	regmap_write(db->reg_cfg, CONTROL, SHDTOKGEN);
}

static void dc_db_mode(struct dc_db *db, enum dc_db_mode mode)
{
	regmap_write(db->reg_cfg, MODECONTROL, mode);
}

static inline void dc_db_alphamaskmode_disable(struct dc_db *db)
{
	regmap_write_bits(db->reg_cfg, ALPHACONTROL, ALPHAMASKENABLE, 0);
}

static inline void dc_db_blendcontrol(struct dc_db *db)
{
	u32 val = PRIM_A_BLD_FUNC(DC_DOMAINBLEND_BLEND_ZERO) |
		  SEC_A_BLD_FUNC(DC_DOMAINBLEND_BLEND_ZERO) |
		  PRIM_C_BLD_FUNC(DC_DOMAINBLEND_BLEND_ZERO) |
		  SEC_C_BLD_FUNC(DC_DOMAINBLEND_BLEND_ONE);

	regmap_write(db->reg_cfg, BLENDCONTROL, val);
}

void dc_db_init(struct dc_db *db)
{
	dc_db_enable_shden(db);
	dc_db_shdtoksel(db, SW);
	dc_db_shdldsel(db, SW);
	dc_db_mode(db, DB_PRIMARY);
	dc_db_alphamaskmode_disable(db);
	dc_db_blendcontrol(db);
}

static int dc_db_bind(struct device *dev, struct device *master, void *data)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct dc_drm_device *dc_drm = data;
	struct resource *res_cfg;
	void __iomem *base_cfg;
	struct dc_db *db;

	db = devm_kzalloc(dev, sizeof(*db), GFP_KERNEL);
	if (!db)
		return -ENOMEM;

	base_cfg = devm_platform_get_and_ioremap_resource(pdev, 0, &res_cfg);
	if (IS_ERR(base_cfg))
		return PTR_ERR(base_cfg);

	db->reg_cfg = devm_regmap_init_mmio(dev, base_cfg,
					    &dc_db_cfg_regmap_config);
	if (IS_ERR(db->reg_cfg))
		return PTR_ERR(db->reg_cfg);

	db->id = dc_subdev_get_id(dc_db_info, ARRAY_SIZE(dc_db_info), res_cfg);
	if (db->id < 0) {
		dev_err(dev, "failed to get instance number: %d\n", db->id);
		return db->id;
	}

	db->dev = dev;
	dc_drm->db[db->id] = db;

	return 0;
}

static const struct component_ops dc_db_ops = {
	.bind = dc_db_bind,
};

static int dc_db_probe(struct platform_device *pdev)
{
	int ret;

	ret = component_add(&pdev->dev, &dc_db_ops);
	if (ret)
		return dev_err_probe(&pdev->dev, ret,
				     "failed to add component\n");

	return 0;
}

static void dc_db_remove(struct platform_device *pdev)
{
	component_del(&pdev->dev, &dc_db_ops);
}

static const struct of_device_id dc_db_dt_ids[] = {
	{ .compatible = "fsl,imx95-dc-domainblend" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, dc_db_dt_ids);

struct platform_driver dc_db_driver = {
	.probe = dc_db_probe,
	.remove = dc_db_remove,
	.driver = {
		.name = "imx95-dc-domainblend",
		.suppress_bind_attrs = true,
		.of_match_table = dc_db_dt_ids,
	},
};
