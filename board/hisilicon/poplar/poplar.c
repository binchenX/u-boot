/*
 * (C) Copyright 2017 Linaro
 * Jorge Ramirez-Ortiz <jorge.ramirez-ortiz@linaro.org>
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#include <common.h>
#include <asm/arch/hi3798cv200.h>
#include <linux/arm-smccc.h>
#include <asm/arch/dwmmc.h>
#include <asm/armv8/mmu.h>
#include <asm/io.h>

DECLARE_GLOBAL_DATA_PTR;

static struct mm_region poplar_mem_map[] = {
	{
		.virt = 0x0UL,
		.phys = 0x0UL,
		.size = 0x80000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) |
			 PTE_BLOCK_INNER_SHARE
	}, {
		.virt = 0x80000000UL,
		.phys = 0x80000000UL,
		.size = 0x80000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
			 PTE_BLOCK_NON_SHARE |
			 PTE_BLOCK_PXN | PTE_BLOCK_UXN
	}, {
		0,
	}
};

struct mm_region *mem_map = poplar_mem_map;

int checkboard(void)
{
	puts("BOARD: Hisilicon HI3798cv200 Poplar\n");

	return 0;
}

void reset_cpu(ulong addr)
{
	psci_system_reset();
}

int dram_init(void)
{
	gd->ram_size = get_ram_size(NULL, 0x80000000);

	return 0;
}

int dram_init_banksize(void)
{
	const struct {
		phys_addr_t start;
		phys_addr_t end;
		char *name;
	} reserved = {
		/* ATF PSCI runs in DDR (no SRAM available for it)  */
		.name = "atf-bl31",
		.start = 0x129000,
		.end =  0x129000 + 0x8020,
	};

	gd->bd->bi_dram[0].start = 0x0;
	gd->bd->bi_dram[0].size = reserved.start;

	gd->bd->bi_dram[1].start = reserved.end;
	gd->bd->bi_dram[1].size = gd->ram_size - reserved.end;

	return 0;
}

static void usb2_phy_config(void)
{
	const u32 config[] = {
		/* close EOP pre-emphasis. open data pre-emphasis */
		0xa1001c,
		/* Rcomp = 150mW, increase DC level */
		0xa00607,
		/* keep Rcomp working */
		0xa10700,
		/* Icomp = 212mW, increase current drive */
		0xa00aab,
		/* EMI fix: rx_active not stay 1 when error packets received */
		0xa11140,
		/* Comp mode select */
		0xa11041,
		/* adjust eye diagram */
		0xa0098c,
		/* adjust eye diagram */
		0xa10a0a,
	};
	int i;

	for (i = 0; i < ARRAY_SIZE(config); i++) {
		writel(config[i], PERI_CTRL_USB0);
		clrsetbits_le32(PERI_CTRL_USB0, BIT(21), BIT(20) | BIT(22));
		udelay(20);
	}
}

static void usb2_phy_init(void)
{
	/* reset usb2 controller bus/utmi/roothub */
	setbits_le32(PERI_CRG46,
		USB2_BUS_SRST_REQ | USB2_UTMI0_SRST_REQ |
		USB2_HST_PHY_SYST_REQ | USB2_OTG_PHY_SYST_REQ);
	udelay(200);

	/* reset usb2 phy por/utmi */
	setbits_le32(PERI_CRG47, USB2_PHY01_SRST_REQ | USB2_PHY01_SRST_TREQ1);
	udelay(200);

	/* open usb2 ref clk */
	setbits_le32(PERI_CRG47, USB2_PHY01_REF_CKEN);
	udelay(300);

	/* cancel usb2 power on reset */
	clrbits_le32(PERI_CRG47, USB2_PHY01_SRST_REQ);
	udelay(500);

	usb2_phy_config();

	/* cancel usb2 port reset, wait comp circuit stable */
	clrbits_le32(PERI_CRG47, USB2_PHY01_SRST_TREQ1);
	mdelay(10);

	/* open usb2 controller clk */
	setbits_le32(PERI_CRG46,
		USB2_BUS_CKEN | USB2_OHCI48M_CKEN | USB2_OHCI12M_CKEN |
		USB2_OTG_UTMI_CKEN | USB2_HST_PHY_CKEN | USB2_UTMI0_CKEN);
	udelay(200);

	/* cancel usb2 control reset */
	clrbits_le32(PERI_CRG46,
		USB2_BUS_SRST_REQ | USB2_UTMI0_SRST_REQ |
		USB2_HST_PHY_SYST_REQ | USB2_OTG_PHY_SYST_REQ);
	udelay(200);
}

int board_mmc_init(bd_t *bis)
{
	int ret;

	ret = hi6220_dwmci_add_port(0, REG_BASE_MCI, 8);
	if (ret)
		printf("mmc init error (%d)\n", ret);

	return ret;
}

int board_init(void)
{
	usb2_phy_init();

	return 0;
}
