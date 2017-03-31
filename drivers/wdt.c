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
#include <generated/config.h>
#include <common.h>
#include <wdt.h>

static void wdt_start(unsigned int msec)
{
	unsigned int t = RTC_FREQ / WDT_DIV * msec / 1000;
	if (t > 65535)
		t = 65535;

	writel(TSCR_WDTSC, TCU_BASE + TCU_TSCR);

	writel(0, WDT_BASE + WDT_TCNT);
	writel(t, WDT_BASE + WDT_TDR);
	writel(TCSR_PRESCALE | TCSR_RTC_EN, WDT_BASE + WDT_TCSR);
	writel(0, WDT_BASE + WDT_TCER);
	writel(TCER_TCEN,WDT_BASE + WDT_TCER);
}

void wdt_stop(void)
{
	writel(0, WDT_BASE + WDT_TCER);   /* Timer stop */
	writel(TSSR_WDTSS, TCU_BASE + TCU_TSSR); /* Set clock supplies to WDT is  stopped */
}

void wdt_init(void) {
	wdt_start(CONFIG_WDT_TIMEOUT_MS);
}