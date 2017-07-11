/*
 * (C) Copyright 2017 Linaro
 *
 * Jorge Ramirez-Ortiz <jorge.ramirez-ortiz@linaro.org>
 *
 * Configuration for Poplar 96boards CE. Parts were derived from other ARM
 * configurations.
 *
 * SPDX-License-Identifier:	GPL-2.0+
 */

#ifndef _POPLAR_H_
#define _POPLAR_H_

#include <linux/sizes.h>

/* DRAM banks */
#define CONFIG_NR_DRAM_BANKS			4

/* SYS */
#define CONFIG_SYS_BOOTM_LEN			0x1400000
#define CONFIG_SYS_INIT_SP_ADDR			0x200000
#define CONFIG_SYS_LOAD_ADDR			0x800000
#define CONFIG_SYS_MALLOC_LEN			SZ_32M

/* ATF bl33.bin load address (must match) */
#define CONFIG_SYS_TEXT_BASE			0x37000000

/* PL010/PL011 */
#define CONFIG_PL01X_SERIAL

/* USB configuration */
#define CONFIG_SYS_USB_EHCI_MAX_ROOT_PORTS	3
#define CONFIG_USB_MAX_CONTROLLER_COUNT		2
#define CONFIG_SYS_USB_EVENT_POLL
#define CONFIG_USB_HOST_ETHER
#define CONFIG_USB_ETHER_ASIX

/* SD/MMC */
#define CONFIG_BOUNCE_BUFFER

/* Android Image format*/
#define CONFIG_ANDROID_BOOT_IMAGE
#define CONFIG_SYS_BOOT_RAMDISK_HIGH
#define CONFIG_CMDLINE_TAG
#define CONFIG_INITRD_TAG
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_SUPPORT_RAW_INITRD
/*****************************************************************************
 *  Initial environment variables
 *****************************************************************************/

#define BOOT_TARGET_DEVICES(func) 					\
					func(USB, usb, 0)		\
					func(MMC, mmc, 0) 		\
					func(DHCP, dhcp, na)
#ifndef CONFIG_SPL_BUILD
#include <config_distro_defaults.h>
#include <config_distro_bootcmd.h>
#endif

#define CONFIG_EXTRA_ENV_SETTINGS					\
					"loader_mmc_blknum=0x0\0"	\
					"loader_mmc_nblks=0x780\0"	\
					"env_mmc_blknum=0xF0000\0"	\
					"env_mmc_nblks=0x80\0"		\
					"kernel_addr_r=0x30000000\0"	\
					"pxefile_addr_r=0x32000000\0"	\
					"scriptaddr=0x32000000\0"	\
					"fdt_addr_r=0x32200000\0"	\
					"ramdisk_addr_r=0x32400000\0"	\
					"initrd_high=0xffffffffffffffff\0"	\
					"android_addr_r=0x30000000\0"  \
					"android_bootargs=androidboot.hardware=poplar androidboot.selinux=permissive\0"	\
					"setupa=setenv bootargs $android_bootargs;" \
					"usb start;"	\
					"fatload usb 0:1 ${kernel_addr_r} Image;"	\
					"fatload usb 0:1 ${fdt_addr_r} hi3798cv200-poplar.dtb;"	\
					"fatload usb 0:1 ${ramdisk_addr_r} ramdisk.android.uboot\0" \
					"boota=booti ${kernel_addr_r} ${ramdisk_addr_r} ${fdt_addr_r}\0" \
					"bootai=mmc read ${android_addr_r} 0x2000 0x81dc;booti ${android_addr_r}\0" \
	BOOTENV


/* Command line configuration */
#define CONFIG_ENV_IS_IN_MMC		1
#define CONFIG_SYS_MMC_ENV_DEV		0
#define CONFIG_ENV_OFFSET		0xF0000  /* env_mmc_blknum */
#define CONFIG_ENV_SIZE			0x10000  /* env_mmc_nblks bytes */
#define CONFIG_CMD_ENV
#define CONFIG_CMD_ASKENV
#define CONFIG_FAT_WRITE
#define CONFIG_ENV_VARS_UBOOT_CONFIG

/* Monitor Command Prompt */
#define CONFIG_CMDLINE_EDITING
#define CONFIG_SYS_LONGHELP
#define CONFIG_SYS_CBSIZE		512
#define CONFIG_SYS_MAXARGS		64
#define CONFIG_SYS_PBSIZE		(CONFIG_SYS_CBSIZE + \
					sizeof(CONFIG_SYS_PROMPT) + 16)
#define CONFIG_SYS_BARGSIZE		CONFIG_SYS_CBSIZE

#endif /* _POPLAR_H_ */
