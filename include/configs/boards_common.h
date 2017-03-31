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

#ifndef BOARDS_COMMON_H
#define BOARDS_COMMON_H

#define __STR__(S)  #S
#define STR(S)      __STR__(S)

// ===========================================================
// The following configure could be override at BOARD.h
// ===========================================================

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
 * DDR
 */
#define CONFIG_MDDR_H5MS5122DFR_J3M
#define CONFIG_PROBE_MEM_SIZE

#ifdef CONFIG_PROBE_MEM_SIZE
#define CONFIG_MEM_SIZE_64M
#endif

/*
 * SFC
 */
#ifdef CONFIG_BOOT_SFC

/*
 * SFC freq - unit(MHz)
 */
#define  CONFIG_SFC_FREQ            (150)

#undef CONFIG_SPI_STANDARD

#endif /* CONFIG_BOOT_SFC */

#ifdef CONFIG_BOOT_SPI_NAND
/*
 * Nand - bytes per-page
 */
#define CONFIG_NAND_BPP             (2048)

/*
 * Nand - page per-block
 */
#define CONFIG_NAND_PPB             (64)

/*
 * Nand - bus width
 */
#define NAND_BUSWIDTH          NAND_BUSWIDTH_8
#endif /* CONFIG_BOOT_SPI_NAND */

#ifdef CONFIG_BOOT_MMC

/*
 * MMC freq - unit(MHz)
 */
#define CONFIG_MSC_FREQ             (50)

#endif /* CONFIG_BOOT_MMC */

/*
 * RTC clk sel
 */
#undef CONFIG_RTCCLK_SEL

/*
 * DDR R/W test
 */
#undef CONFIG_DDR_ACCESS_TEST

/*
 * WIFI MAC
 */
#undef CONFIG_GET_WIFI_MAC
#define KERNEL_ARGS_WIFI_MAC    "wifi_mac=xxxxxxxxxxxx "

/*
 * Recovery
 */
#undef CONFIG_RECOVERY
#define RECOVERY_UPDATE_FLAG_CHECK

#define RECOVERY_UPDATE_FLAG_OFFSET             0x6000      //24kb
#define RECOVERY_UPDATE_FLAG_SIZE                   4               //unit: byte
#define RECOVERY_UPDATE_FLAG_UPDATING         0x5A5A5A5A
#endif /* BOARDS_COMMON_H */
