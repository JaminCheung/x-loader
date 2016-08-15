/*
 *  Copyright (C) 2016 Ingenic Semiconductor Co.,Ltd
 *
 *  X1000 series bootloader for u-boot/rtos/linux
 *
 *  Zhang YanMing <yanming.zhang@ingenic.com, jamincheung@126.com>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under  the terms of the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef HALLEY2_H
#define HALLEY2_H

/*
 * External crystal freq - unit(MHz)
 */
#define CONFIG_EXTAL_FREQ           24

/*
 * PLL freq - unit(MHz)
 */
#define CONFIG_APLL_FREQ        1008
#define CONFIG_MPLL_FREQ        600
#define CONFIG_CPU_SEL_PLL      APLL
#define CONFIG_DDR_SEL_PLL      MPLL
#define CONFIG_DDR_FREQ_DIV     3
#define CONFIG_L2CACHE_CLK_DIV  2
#define CONFIG_AHB_CLK_DIV      3

/*
 * Console
 */
#define CONFIG_CONSOLE_BAUDRATE      115200
#define CONFIG_CONSOLE_INDEX        2
//#define CONFIG_UART_PORTD
#define CONFIG_UART_PORTC

/*
 * DDR
 */
#define CONFIG_MDDR_H5MS5122DFR_J3M

/*
 * The following configure only for boot u-boot
 */
#ifdef CONFIG_BOOT_UBOOT

#define CONFIG_UBOOT_OFFSET                 -1
#define CONFIG_UBOOT_LENGTH                 -1

#endif /* CONFIG_BOOT_UBOOT */

/*
 * The following configure only for boot kernel
 */
#ifdef CONFIG_BOOT_KERNEL

#define KERNEL_ARGS_COMMON "mem=32M@0 console=ttyS2,115200n8 ip=off init=/linuxrc"

#define CONFIG_KERNEL_OFFSET                -1
#define CONFIG_KERNEL_LENGTH                -1

#define CONFIG_RECOVERY_BOOT_KEY            -1
#define CONFIG_RECOVERY_BOOT_KEY_ENLEVEL    -1
#define CONFIG_RECOVERY_OFFSET              -1
#define CONFIG_RECOVERY_LENGTH              -1
#define CONFIG_RECOVERY_ARGS  KERNEL_ARGS_COMMON

#endif /* CONFIG_BOOT_KERNEL */

/*
 * The following configure only for NAND boot
 */
#ifdef CONFIG_BOOT_NAND
/*
 * Nand - bytes per-page
 */
#define CONFIG_NAND_BPP             (2048 + 64)

/*
 * Nand - page per-block
 */
#define CONFIG_NAND_PPB             (64)

#define CONFIG_KERNEL_ARGS KERNEL_ARGS_COMMON "ubi.mtd=5 root=ubi0:rootfs ubi.mtd=6 rootfstype=ubifs rw"

#endif /* CONFIG_BOOT_NAND */

/*
 * The following configure only for NOR boot
 */
#ifdef CONFIG_BOOT_NOR

#define CONFIG_KERNEL_ARGS KERNEL_ARGS_COMMON "rootfstype=jffs2 root=/dev/mtdblock2 rw"

#endif /* CONFIG_BOOT_NOR */

/*
 * The following configure only for MMC boot
 */
#ifdef CONFIG_BOOT_MMC

//#define CONFIG_BOOT_MMC_PA_4BIT
//#define CONFIG_BOOT_MMC_PA_8BIT
#define CONFIG_BOOT_MMC_PC_4BIT

#endif /* CONFIG_BOOT_MMC */

#endif /* HALLEY2_H */
