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

#ifndef BURNER_H
#define BURNER_H

/*
 * Console
 */
#define CONFIG_CONSOLE_BAUDRATE      3000000
#define CONFIG_CONSOLE_INDEX         2

/*
 * PLL freq - unit(MHz)
 */
#undef CONFIG_APLL_FREQ
#define CONFIG_APLL_FREQ        1008

#undef CONFIG_MPLL_FREQ
#define CONFIG_MPLL_FREQ        600

#undef CONFIG_CPU_SEL_PLL
#define CONFIG_CPU_SEL_PLL      APLL

#undef CONFIG_DDR_SEL_PLL
#define CONFIG_DDR_SEL_PLL      MPLL

#undef CONFIG_DDR_FREQ_DIV
#define CONFIG_DDR_FREQ_DIV     3

#undef CONFIG_L2CACHE_CLK_DIV
#define CONFIG_L2CACHE_CLK_DIV  2

#undef CONFIG_AHB_CLK_DIV
#define CONFIG_AHB_CLK_DIV      3

/*
 * Support burn SPI flash
 */
#define CONFIG_BURN_SPI_FLASH

/*
 * Support burn emmc / sdcard
 */
#define CONFIG_BURN_MMC

/*
 * Ignore RTC 32KHz clk
 */
#define CONFIG_RTCCLK_SEL

#endif /* BURNER_H */
