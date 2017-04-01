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

#include <common.h>
#include <generated/wdt_cfg_params.h>

#define wdt_write(value, reg)   writel(value, WDT_BASE + reg)
#define wdt_read(reg)           readl(WDT_BASE + reg)

static inline void wdt_start(void) {
    writel(TSCR_WDTSC, TCU_BASE + TCU_TSCR);

    wdt_write(0, WDT_TCNT);
    wdt_write(WDT_CFG_TDR, WDT_TDR);
    wdt_write(WDT_CFG_TCSR, WDT_TCSR);
    wdt_write(0, WDT_TCER);
    wdt_write(TCER_TCEN, WDT_TCER);
}

void wdt_stop(void) {
    wdt_write(wdt_read(WDT_TCER) & ~TCER_TCEN, WDT_TCER); /* Timer stop */
    writel(TSSR_WDTSS, TCU_BASE + TCU_TSSR); /* Set clock supplies to WDT is  stopped */
}

void wdt_reset(void) {
    wdt_write(0x0, WDT_TCNT);
}

void wdt_init(void) {
    wdt_start();
}

