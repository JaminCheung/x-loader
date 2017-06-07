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

#ifdef CONFIG_BOOT_MMC
uint32_t cpu_freq = CONFIG_CPU_SEL_PLL == APLL ? CONFIG_APLL_FREQ : CONFIG_MPLL_FREQ;

#elif (defined CONFIG_BOOT_USB)
/*
 * Bootrom set apll freq for usb boot
 */
#if (CONFIG_EXTAL_FREQ == 24)
uint32_t cpu_freq = 576;
#elif (CONFIG_EXTAL_FREQ == 26)
uint32_t cpu_freq = 624;
#endif

#else
uint32_t cpu_freq = CONFIG_EXTAL_FREQ;
#endif

__attribute__((weak, alias("board_init"))) void board_init(void) {}
__attribute__((weak, alias("board_early_init"))) void board_early_init(void) {}

#ifdef CONFIG_BOOT_USB
static void pass_params_to_burner(void) {
    /*
     * Memory size
     */
    uint32_t mem_size;

#ifdef CONFIG_PROBE_MEM_SIZE
    mem_size = get_lpddr_size();
#elif (defined CONFIG_MEM_SIZE_64M)
    mem_size = SZ_64M;
#else
    mem_size = SZ_32M;
#endif

    if (mem_size == SZ_64M)
        writel(MEM_SIZE_FLAG_64M, CONFIG_MEM_SIZE_FLAG_ADDR);
    else
        writel(MEM_SIZE_FLAG_32M, CONFIG_MEM_SIZE_FLAG_ADDR);

    /*
     * Uart index and baudrate
     */
    writel(CONFIG_CONSOLE_INDEX, CONFIG_UART_INDEX_ADDR);
    writel(CONFIG_CONSOLE_BAUDRATE, CONFIG_UART_BAUDRATE_ADDR);
}
#endif

void x_loader_main(void) {

    /*
     * Open DDR CPU AHB0 APB RTC EFUSE NEMC clock gate
     */
    cpm_outl(0x07fffffc, CPM_CLKGR);

    /*
     * Init board early
     */
    board_early_init();

    /*
     * Init uart
     */
    uart_init();

    /*
     * check SOC id
     */
#ifdef CONFIG_CHECK_SOC_ID
    check_socid();
#endif

    uart_puts("\n\nX-Loader Build: " X_LOADER_DATE " - " X_LOADER_TIME);

    /*
     * Print error pc register
     */
    uint32_t errorpc;
    __asm__ __volatile__ (
        "mfc0 %0, $30, 0\n\t"
        "nop \n\t"
        :"=r"(errorpc)
        :);
    printf("\nreset errorpc: 0x%x\n", errorpc);

    /*
     * Init clock
     */
    clk_init();

#ifdef CONFIG_WDT
    /*
     * Init wdt
     */
    wdt_init();
#endif

    /*
     * Init lpddr
     */
    lpddr_init();

#ifdef CONFIG_DDR_ACCESS_TEST
    /*
     * DDR R/W test
     */
    ddr_access_test();

    hang_reason("Memory test done.\n");
#endif

    /*
     * Init board
     */
    board_init();

    /*
     * Open this for debug power
     */
#if 0
    /*
     * PM_SUSPEND_STANDBY: cpu enter idle & memory entry self-refresh
     * PM_SUSPEND_MEM:     cpu enter sleep & memory entry self-refresh & clock
     *                     all stoped
     */
    suspend_enter(PM_SUSPEND_MEM);
#endif

#ifndef CONFIG_BOOT_USB
    uart_puts("Going to boot next stage.\n");

    /*
     * Boot next stage(kernel/u-boot/rtos)
     */
    boot_next_stage();

#else /* CONFIG_BOOT_USB */
    pass_params_to_burner();

    uart_puts("Going to start burner.\n");

    /*
     * Return to rom
     */
    return;
#endif
}
