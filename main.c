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
uint32_t cpu_freq = CONFIG_APLL_FREQ;
#else
uint32_t cpu_freq = CONFIG_EXTAL_FREQ;
#endif

__attribute__((weak, alias("board_init"))) void board_init(void) {}
__attribute__((weak, alias("board_early_init"))) void board_early_init(void) {}

void x_loader_main(void) {
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
    check_socid();

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

    /*
     * Init lpddr
     */
    lpddr_init();

    /*
     * Init board
     */
    board_init();

    uart_puts("Going to boot next stage.\n");

    /*
     * Boot next stage(kernel/u-boot/rtos)
     */
    boot_next_stage();
}
