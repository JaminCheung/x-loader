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

void wdt_feed(void) {
    wdt_write(0x0, WDT_TCNT);
}

void wdt_init(void) {
    wdt_start();
}

void wdt_restart(void) {
#define WDT_DIV             64
#define RTC_FREQ            32768
#define RESET_DELAY_MS      4
#define TCSR_PRESCALE_64    (3 << 3)
#define TCSR_PRESCALE       TCSR_PRESCALE_64

    int time = RTC_FREQ / WDT_DIV * RESET_DELAY_MS / 1000;

    if(time > 65535)
        time = 65535;

    writel(TSCR_WDTSC, TCU_BASE + TCU_TSCR);

    wdt_write(0, WDT_TCNT);
    wdt_write(time, WDT_TDR);
    wdt_write(TCSR_PRESCALE | TCSR_RTC_EN, WDT_TCSR);
    wdt_write(0, WDT_TCER);

    printf("reset in %dms", RESET_DELAY_MS);

    wdt_write(TCER_TCEN, WDT_TCER);

    while(1);

#undef WDT_DIV
#undef RTC_FREQ
#undef RESET_DELAY_MS
#undef TCSR_PRESCALE_64
#undef TCSR_PRESCALE
}
