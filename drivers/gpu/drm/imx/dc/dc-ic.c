// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2024 NXP
 */

#include <linux/clk.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/irqchip/chained_irq.h>
#include <linux/irqdomain.h>
#include <linux/of.h>
#include <linux/of_irq.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/regmap.h>

#define USERINTERRUPTMASK(n)	(0x8 + 4 * (n))
#define INTERRUPTENABLE(n)	(0x10 + 4 * (n))
#define INTERRUPTPRESET(n)	(0x18 + 4 * (n))
#define INTERRUPTCLEAR(n)	(0x20 + 4 * (n))
#define INTERRUPTSTATUS(n)	(0x28 + 4 * (n))
#define USERINTERRUPTENABLE(n)	(0x40 + 4 * (n))
#define USERINTERRUPTPRESET(n)	(0x48 + 4 * (n))
#define USERINTERRUPTCLEAR(n)	(0x50 + 4 * (n))
#define USERINTERRUPTSTATUS(n)	(0x58 + 4 * (n))

#define INTERRUPTENABLE_MX95(n)	(0x8 + 4 * (n))
#define INTERRUPTPRESET_MX95(n)	(0x14 + 4 * (n))
#define INTERRUPTCLEAR_MX95(n)	(0x20 + 4 * (n))
#define INTERRUPTSTATUS_MX95(n)	(0x2c + 4 * (n))

#define IRQ_COUNT_MAX	86
#define REG_NUM_MAX	3

struct dc_ic_data {
	struct regmap		*regs;
	struct clk_bulk_data	*clk_axi;
	int			clk_axi_count;
	int			irq[IRQ_COUNT_MAX];
	struct irq_domain	*domain;
	unsigned int		reg_enable;
	unsigned int		reg_status;
};

struct dc_ic_entry {
	struct dc_ic_data *data;
	int irq;
};

struct dc_ic_subdev_match_data {
	const struct regmap_config	*regmap_config;
	unsigned int			reg_enable;
	unsigned int			reg_clear;
	unsigned int			reg_status;
	unsigned int			reg_count;
	unsigned int			irq_count;
	unsigned long			unused_irq[REG_NUM_MAX];
	int				reserved_irq;
	bool				user_irq;
};

static const struct regmap_range dc_ic_regmap_write_ranges_imx8qxp[] = {
	regmap_reg_range(USERINTERRUPTMASK(0), INTERRUPTCLEAR(1)),
	regmap_reg_range(USERINTERRUPTENABLE(0), USERINTERRUPTCLEAR(1)),
};

static const struct regmap_access_table dc_ic_regmap_write_table_imx8qxp = {
	.yes_ranges = dc_ic_regmap_write_ranges_imx8qxp,
	.n_yes_ranges = ARRAY_SIZE(dc_ic_regmap_write_ranges_imx8qxp),
};

static const struct regmap_range dc_ic_regmap_read_ranges_imx8qxp[] = {
	regmap_reg_range(USERINTERRUPTMASK(0), INTERRUPTENABLE(1)),
	regmap_reg_range(INTERRUPTSTATUS(0), INTERRUPTSTATUS(1)),
	regmap_reg_range(USERINTERRUPTENABLE(0), USERINTERRUPTENABLE(1)),
	regmap_reg_range(USERINTERRUPTSTATUS(0), USERINTERRUPTSTATUS(1)),
};

static const struct regmap_access_table dc_ic_regmap_read_table_imx8qxp = {
	.yes_ranges = dc_ic_regmap_read_ranges_imx8qxp,
	.n_yes_ranges = ARRAY_SIZE(dc_ic_regmap_read_ranges_imx8qxp),
};

static const struct regmap_range dc_ic_regmap_volatile_ranges_imx8qxp[] = {
	regmap_reg_range(INTERRUPTPRESET(0), INTERRUPTCLEAR(1)),
	regmap_reg_range(USERINTERRUPTPRESET(0), USERINTERRUPTCLEAR(1)),
};

static const struct regmap_access_table dc_ic_regmap_volatile_table_imx8qxp = {
	.yes_ranges = dc_ic_regmap_volatile_ranges_imx8qxp,
	.n_yes_ranges = ARRAY_SIZE(dc_ic_regmap_volatile_ranges_imx8qxp),
};

static const struct regmap_config dc_ic_regmap_config_imx8qxp = {
	.reg_bits = 32,
	.reg_stride = 4,
	.val_bits = 32,
	.fast_io = true,
	.wr_table = &dc_ic_regmap_write_table_imx8qxp,
	.rd_table = &dc_ic_regmap_read_table_imx8qxp,
	.volatile_table = &dc_ic_regmap_volatile_table_imx8qxp,
	.max_register = USERINTERRUPTSTATUS(1),
};

static const struct dc_ic_subdev_match_data dc_ic_match_data_imx8qxp = {
	.regmap_config = &dc_ic_regmap_config_imx8qxp,
	.reg_enable = USERINTERRUPTENABLE(0),
	.reg_clear = USERINTERRUPTCLEAR(0),
	.reg_status = USERINTERRUPTSTATUS(0),
	.reg_count = 2,
	.irq_count = 49,
	.user_irq = true,
	.unused_irq = { 0x00000000, 0xfffe0008 },
	.reserved_irq = 35,
};

static const struct regmap_range dc_ic_regmap_write_ranges_imx95[] = {
	regmap_reg_range(INTERRUPTENABLE_MX95(0), INTERRUPTCLEAR_MX95(2)),
};

static const struct regmap_access_table dc_ic_regmap_write_table_imx95 = {
	.yes_ranges = dc_ic_regmap_write_ranges_imx95,
	.n_yes_ranges = ARRAY_SIZE(dc_ic_regmap_write_ranges_imx95),
};

static const struct regmap_range dc_ic_regmap_read_ranges_imx95[] = {
	regmap_reg_range(INTERRUPTENABLE_MX95(0), INTERRUPTENABLE_MX95(2)),
	regmap_reg_range(INTERRUPTSTATUS_MX95(0), INTERRUPTSTATUS_MX95(2)),
};

static const struct regmap_access_table dc_ic_regmap_read_table_imx95 = {
	.yes_ranges = dc_ic_regmap_read_ranges_imx95,
	.n_yes_ranges = ARRAY_SIZE(dc_ic_regmap_read_ranges_imx95),
};

static const struct regmap_range dc_ic_regmap_volatile_ranges_imx95[] = {
	regmap_reg_range(INTERRUPTPRESET_MX95(0), INTERRUPTCLEAR_MX95(2)),
};

static const struct regmap_access_table dc_ic_regmap_volatile_table_imx95 = {
	.yes_ranges = dc_ic_regmap_volatile_ranges_imx95,
	.n_yes_ranges = ARRAY_SIZE(dc_ic_regmap_volatile_ranges_imx95),
};

static const struct regmap_config dc_ic_regmap_config_imx95 = {
	.reg_bits = 32,
	.reg_stride = 4,
	.val_bits = 32,
	.fast_io = true,
	.wr_table = &dc_ic_regmap_write_table_imx95,
	.rd_table = &dc_ic_regmap_read_table_imx95,
	.volatile_table = &dc_ic_regmap_volatile_table_imx95,
	.max_register = INTERRUPTSTATUS_MX95(2),
};

static const struct dc_ic_subdev_match_data dc_ic_match_data_imx95 = {
	.regmap_config = &dc_ic_regmap_config_imx95,
	.reg_enable = INTERRUPTENABLE_MX95(0),
	.reg_clear = INTERRUPTCLEAR_MX95(0),
	.reg_status = INTERRUPTSTATUS_MX95(0),
	.reg_count = 3,
	.irq_count = 86,
	.user_irq = false,
	.unused_irq = { 0x00000000, 0x00000000, 0xffc00000 },
	.reserved_irq = -1,
};

static void dc_ic_irq_handler(struct irq_desc *desc)
{
	struct dc_ic_entry *entry = irq_desc_get_handler_data(desc);
	struct dc_ic_data *data = entry->data;
	unsigned int status, enable, offset;
	unsigned int virq;

	chained_irq_enter(irq_desc_get_chip(desc), desc);

	offset = entry->irq / 32;
	regmap_read(data->regs, data->reg_status + 4 * offset, &status);
	regmap_read(data->regs, data->reg_enable + 4 * offset, &enable);

	status &= enable;

	if (status & BIT(entry->irq % 32)) {
		virq = irq_find_mapping(data->domain, entry->irq);
		if (virq)
			generic_handle_irq(virq);
	}

	chained_irq_exit(irq_desc_get_chip(desc), desc);
}

static int dc_ic_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	const struct dc_ic_subdev_match_data *dc_ic_match_data = device_get_match_data(dev);
	struct irq_chip_generic *gc;
	struct dc_ic_entry *entry;
	struct irq_chip_type *ct;
	struct dc_ic_data *data;
	void __iomem *base;
	int i, ret;

	data = devm_kzalloc(dev, sizeof(*data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	entry = devm_kcalloc(dev, dc_ic_match_data->irq_count, sizeof(*entry), GFP_KERNEL);
	if (!entry)
		return -ENOMEM;

	base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(base)) {
		dev_err(dev, "failed to initialize reg\n");
		return PTR_ERR(base);
	}

	data->regs = devm_regmap_init_mmio(dev, base, dc_ic_match_data->regmap_config);
	if (IS_ERR(data->regs))
		return PTR_ERR(data->regs);

	ret = devm_clk_bulk_get_all(dev, &data->clk_axi);
	if (ret < 0)
		return dev_err_probe(dev, ret,
				     "failed to get AXI clock\n");
	data->clk_axi_count = ret;

	data->reg_enable = dc_ic_match_data->reg_enable;
	data->reg_status = dc_ic_match_data->reg_status;

	for (i = 0; i < dc_ic_match_data->irq_count; i++) {
		/* skip the reserved IRQ */
		if (i == dc_ic_match_data->reserved_irq)
			continue;

		ret = platform_get_irq(pdev, i);
		if (ret < 0)
			return ret;
	}

	dev_set_drvdata(dev, data);

	ret = devm_pm_runtime_enable(dev);
	if (ret)
		return ret;

	ret = pm_runtime_resume_and_get(dev);
	if (ret < 0) {
		dev_err(dev, "failed to get runtime PM sync: %d\n", ret);
		return ret;
	}

	for (i = 0; i < dc_ic_match_data->reg_count; i++) {
		/* mask and clear all interrupts */
		regmap_write(data->regs, dc_ic_match_data->reg_enable + (4 * i), 0x0);
		regmap_write(data->regs, dc_ic_match_data->reg_clear + (4 * i), 0xffffffff);

		if (!dc_ic_match_data->user_irq)
			continue;

		regmap_write(data->regs, USERINTERRUPTENABLE(i), 0x0);
		regmap_write(data->regs, USERINTERRUPTCLEAR(i), 0xffffffff);

		/* set all interrupts to user mode */
		regmap_write(data->regs, USERINTERRUPTMASK(i), 0xffffffff);
	}

	data->domain = irq_domain_add_linear(dev->of_node, dc_ic_match_data->irq_count,
					     &irq_generic_chip_ops, data);
	if (!data->domain) {
		dev_err(dev, "failed to create IRQ domain\n");
		pm_runtime_put(dev);
		return -ENOMEM;
	}
	irq_domain_set_pm_device(data->domain, dev);

	ret = irq_alloc_domain_generic_chips(data->domain, 32, 1,
					     of_node_full_name(dev->of_node),
					     handle_level_irq, 0, 0, 0);
	if (ret) {
		dev_err(dev, "failed to alloc generic IRQ chips: %d\n", ret);
		irq_domain_remove(data->domain);
		pm_runtime_put(dev);
		return ret;
	}

	for (i = 0; i < dc_ic_match_data->irq_count; i += 32) {
		gc = irq_get_domain_generic_chip(data->domain, i);
		gc->reg_base = base;
		gc->unused = dc_ic_match_data->unused_irq[i / 32];
		ct = gc->chip_types;
		ct->chip.irq_ack = irq_gc_ack_set_bit;
		ct->chip.irq_mask = irq_gc_mask_clr_bit;
		ct->chip.irq_unmask = irq_gc_mask_set_bit;
		ct->regs.ack = dc_ic_match_data->reg_clear + (i / 8);
		ct->regs.mask = dc_ic_match_data->reg_enable + (i / 8);
	}

	for (i = 0; i < dc_ic_match_data->irq_count; i++) {
		/* skip the reserved IRQ */
		if (i == dc_ic_match_data->reserved_irq)
			continue;

		data->irq[i] = irq_of_parse_and_map(dev->of_node, i);

		entry[i].data = data;
		entry[i].irq = i;

		irq_set_chained_handler_and_data(data->irq[i],
						 dc_ic_irq_handler, &entry[i]);
	}

	return 0;
}

static void dc_ic_remove(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	const struct dc_ic_subdev_match_data *dc_ic_match_data = device_get_match_data(dev);
	struct dc_ic_data *data = dev_get_drvdata(&pdev->dev);
	int i;

	for (i = 0; i < dc_ic_match_data->irq_count; i++) {
		if (i == dc_ic_match_data->reserved_irq)
			continue;

		irq_set_chained_handler_and_data(data->irq[i], NULL, NULL);
	}

	irq_domain_remove(data->domain);

	pm_runtime_put_sync(&pdev->dev);
}

static int dc_ic_runtime_suspend(struct device *dev)
{
	struct dc_ic_data *data = dev_get_drvdata(dev);

	clk_bulk_disable_unprepare(data->clk_axi_count, data->clk_axi);

	return 0;
}

static int dc_ic_runtime_resume(struct device *dev)
{
	struct dc_ic_data *data = dev_get_drvdata(dev);
	int ret;

	ret = clk_bulk_prepare_enable(data->clk_axi_count, data->clk_axi);
	if (ret)
		dev_err(dev, "failed to enable AXI clock: %d\n", ret);

	return ret;
}

static const struct dev_pm_ops dc_ic_pm_ops = {
	SET_NOIRQ_SYSTEM_SLEEP_PM_OPS(pm_runtime_force_suspend,
				      pm_runtime_force_resume)
	RUNTIME_PM_OPS(dc_ic_runtime_suspend, dc_ic_runtime_resume, NULL)
};

static const struct of_device_id dc_ic_dt_ids[] = {
	{ .compatible = "fsl,imx8qxp-dc-intc", .data = &dc_ic_match_data_imx8qxp },
	{ .compatible = "fsl,imx95-dc-intc", .data = &dc_ic_match_data_imx95 },
	{ /* sentinel */ }
};

struct platform_driver dc_ic_driver = {
	.probe = dc_ic_probe,
	.remove = dc_ic_remove,
	.driver = {
		.name = "imx8-dc-intc",
		.suppress_bind_attrs = true,
		.of_match_table	= dc_ic_dt_ids,
		.pm = pm_sleep_ptr(&dc_ic_pm_ops),
	},
};
