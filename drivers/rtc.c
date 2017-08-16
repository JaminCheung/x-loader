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

static inline void wait_write_ready(void) {
    int timeout = 0x100000;

    while (!(rtc_inl(RTC_RTCCR) & RTC_RTCCR_WRDY) && timeout--);
    if (timeout <= 0)
        uart_puts("RTC wait_write_ready timeout!\n");
}

void rtc_clk_src_to_ext(void) {
    /*
     * Set OPCR.ERCS of CPM to 1
     */
    uint32_t opcr = cpm_inl(CPM_OPCR);
    opcr |= OPCR_ERCS;
    cpm_outl(opcr, CPM_OPCR);

    /*
     * Set CLKGR.RTC of CPM to 0
     */
    uint32_t clkgr = cpm_inl(CPM_CLKGR);
    clkgr &= ~CPM_CLKGR_RTC;
    cpm_outl(clkgr, CPM_CLKGR);

    /*
     * Set RTCCR.SELEXC to 1
     */
    wait_write_ready();
    uint32_t rtccr = rtc_inl(RTC_RTCCR);
    rtccr |= RTC_RTCCR_SELEXC;
    rtc_outl(rtccr, RTC_RTCCR);

    /*
     * Wait two clock period of clock
     */
    udelay(10);

    opcr &= ~OPCR_ERCS;
    cpm_outl(opcr, CPM_OPCR);

    udelay(10);

    /*
     * Check RTCCR.SELEXC == 1
     */
    rtccr = rtc_inl(RTC_RTCCR);
    if (!(rtccr & RTC_RTCCR_SELEXC)) {
        hang_reason("rtc clock sel failed\n\n");
    }
}
