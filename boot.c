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

__attribute__ ((noreturn)) static void jump_to_image(uint32_t entry_addr,
        uint32_t argv) {

#if (defined CONFIG_BOOT_UBOOT)
    typedef void (*image_entry_t)(void) __attribute__ ((noreturn));

    image_entry_t image_entry = (image_entry_t) entry_addr;

    flush_cache_all();

    image_entry();

#elif (defined CONFIG_BOOT_KERNEL) /* CONFIG_BOOT_UBOOT */
    typedef void (*image_entry_t)(int, char **, void *)
            __attribute__ ((noreturn));

    (void) argv;

    uint32_t *linux_argv = (uint32_t *) CONFIG_KERNEL_PARAMETER_ADDR;

    image_entry_t image_entry = (image_entry_t) entry_addr;

    linux_argv[0] = 0;
    linux_argv[1] = argv;

    flush_cache_all();

    image_entry(2, (char **)linux_argv, NULL);
#endif /* CONFIG_BOOT_KERNEL */
}

void boot_next_stage(void) {
    int error = 0;

    uint32_t load_addr = 0;
    uint32_t entry_addr = 0;
    uint32_t argv = 0;

    entry_addr = CONFIG_BOOT_NEXT_STAGE_ENTRY_ADDR;

#if (defined CONFIG_BOOT_KERNEL)
    int boot_sel = -1;

    if (get_boot_sel() == RECOVERY_BOOT) {
        uart_puts("Mod: Recovery\n");
        boot_sel = RECOVERY_BOOT;
        argv = (uint32_t) CONFIG_RECOVERY_ARGS;
    } else {
        uart_puts("Mod: Normal\n");
        boot_sel = NORMAL_BOOT;
        argv = (uint32_t) CONFIG_KERNEL_ARGS;
    }

    load_addr = CONFIG_BOOT_NEXT_STAGE_LOAD_ADDR;

#elif (defined CONFIG_BOOT_UBOOT) /* CONFIG_BOOT_KERNEL */

    load_addr = CONFIG_BOOT_NEXT_STAGE_LOAD_ADDR;

#endif

    /*
     * For mmc
     */
#ifdef CONFIG_BOOT_MMC
    #if (defined CONFIG_BOOT_UBOOT)
        error = mmc_load(CONFIG_UBOOT_OFFSET, CONFIG_UBOOT_LENGTH, load_addr);

    #elif (defined CONFIG_BOOT_KERNEL)
        if (boot_sel == RECOVERY_BOOT)
            error = mmc_load(CONFIG_RECOVERY_OFFSET, CONFIG_RECOVERY_LENGTH, load_addr);
        else
            error = mmc_load(CONFIG_KERNEL_OFFSET, CONFIG_KERNEL_LENGTH, load_addr);
    #endif
#endif /* CONFIG_BOOT_MMC */

    /*
     * For spi nand
     */
#ifdef CONFIG_BOOT_SPI_NAND
    #if (defined CONFIG_BOOT_UBOOT)
        error = spinand_load(CONFIG_UBOOT_OFFSET, CONFIG_UBOOT_LENGTH, load_addr);

    #elif (defined CONFIG_BOOT_KERNEL)
        if (boot_sel == RECOVERY_BOOT)
            error = spinand_load(CONFIG_RECOVERY_OFFSET, CONFIG_RECOVERY_LENGTH, load_addr);
        else
            error = spinand_load(CONFIG_KERNEL_OFFSET, CONFIG_KERNEL_LENGTH, load_addr);
    #endif
#endif /* CONFIG_BOOT_SPI_NAND */

    /*
     * For spi nor
     */
#ifdef CONFIG_BOOT_SPI_NOR
    #if (defined CONFIG_BOOT_UBOOT)
        error = spinor_load(CONFIG_UBOOT_OFFSET, CONFIG_UBOOT_LENGTH, load_addr);

    #elif (defined CONFIG_BOOT_KERNEL)
        if (boot_sel == RECOVERY_BOOT)
            error = spinor_load(CONFIG_RECOVERY_OFFSET, CONFIG_RECOVERY_LENGTH, load_addr);
        else
            error = spinor_load(CONFIG_KERNEL_OFFSET, CONFIG_KERNEL_LENGTH, load_addr);
    #endif
#endif /* CONFIG_BOOT_SPI_NOR */

    printf("Load address:  0x%x\n", load_addr);
    printf("Entry address: 0x%x\n", entry_addr);

    if (error < 0)
        panic("\n\tLoad next stage failed.\n");

    uart_puts("\nJump...\n\n");

    /*
     * We will nerver return here
     */
    jump_to_image(entry_addr, argv);
}
