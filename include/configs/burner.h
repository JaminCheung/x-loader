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

//#define CONFIG_PM_SUSPEND
//#define CONFIG_PM_SUSPEND_STATE PM_SUSPEND_STANDBY

/*
 * Console
 */
#define CONFIG_CONSOLE_BAUDRATE      3000000
#define CONFIG_CONSOLE_PC
#define CONFIG_CONSOLE_INDEX         2

/*
 * Support burn SPI flash
 */
#define CONFIG_BURN_SPI_FLASH

/*
 * Support burn emmc / sdcard
 */
#undef CONFIG_BURN_MMC
#undef CONFIG_MMC_PA_4BIT
#undef CONFIG_MMC_PC_4BIT
#undef CONFIG_MMC_PA_8BIT

/*
 * Ignore RTC 32KHz clk
 */
#define CONFIG_RTCCLK_SRC_EXT

#endif /* BURNER_H */
