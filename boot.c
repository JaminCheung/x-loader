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

__attribute__ ((noreturn)) void jump_to_image(void) {
#if defined CONFIG_BOOT_UBOOT
    typedef void (*image_entry_t)(void) __attribute__ ((noreturn));

    image_entry_t image_entry = (image_entry_t) CONFIG_BOOT_NEXT_STAGE_TEXT;

    flush_cache_all();

    image_entry();

#elif defined CONFIG_BOOT_KERNEL /* CONFIG_BOOT_UBOOT */
    typedef void (*image_entry_t)(int, char **, void *)
            __attribute__ ((noreturn));

    uint32_t *linux_argv = (uint32_t *) CONFIG_KERNEL_PARAMETER_ADDR;

    image_entry_t image_entry = (image_entry_t) CONFIG_BOOT_NEXT_STAGE_TEXT;

    linux_argv[0] = 0;
    linux_argv[1] = (uint32_t) (CONFIG_KERNEL_ARGS);

    flush_cache_all();

    image_entry(2, (char **)linux_argv, NULL);
#endif /* CONFIG_BOOT_KERNEL */
}

#ifdef CONFIG_BOOT_MMC
static void mmc_boot(void) {

}
#endif /* CONFIG_BOOT_MMC */

#ifdef CONFIG_BOOT_NAND
static void nand_boot(void) {
#if defined CONFIG_BOOT_UBOOT
    spinand_load(CONFIG_UBOOT_OFFSET, CONFIG_UBOOT_LENGTH, CONFIG_BOOT_NEXT_STAGE_TEXT);
#elif defined CONFIG_BOOT_KERNEL
    spinand_load(CONFIG_KERNEL_OFFSET, CONFIG_KERNEL_LENGTH, CONFIG_BOOT_NEXT_STAGE_TEXT);
#endif
}
#endif /* CONFIG_BOOT_NAND */

#ifdef CONFIG_BOOT_NOR
static void nor_boot(void) {
#if defined CONFIG_BOOT_UBOOT
    spinor_load(CONFIG_UBOOT_OFFSET, CONFIG_UBOOT_LENGTH, CONFIG_BOOT_NEXT_STAGE_TEXT);
#elif defined CONFIG_BOOT_KERNEL
    spinor_load(CONFIG_KERNEL_OFFSET, CONFIG_KERNEL_LENGTH, CONFIG_BOOT_NEXT_STAGE_TEXT);
#endif
}
#endif /* CONFIG_BOOT_NOR */

void boot_next_stage(void) {
#ifdef CONFIG_BOOT_MMC
    mmc_boot();
#endif /* CONFIG_BOOT_MMC */

#ifdef CONFIG_BOOT_NAND
    nand_boot();
#endif /* CONFIG_BOOT_NAND */

#ifdef CONFIG_BOOT_NOR
    nor_boot();
#endif /* CONFIG_BOOT_NOR */

    /*
     * We will nerver return here
     */
    jump_to_image();
}
