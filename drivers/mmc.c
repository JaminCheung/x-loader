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

#ifdef CONFIG_BOOT_MMC_PA_8BIT
static void set_gpio_pa_as_mmc0_8bit(void) {
    gpio_set_func(GPIO_PA(16), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(17), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(18), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(19), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(20), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(21), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(22), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(23), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(24), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(25), GPIO_FUNC_1);
}
#endif

#ifdef CONFIG_BOOT_MMC_PA_4BIT
static void set_gpio_pa_as_mmc0_4bit(void) {
    gpio_set_func(GPIO_PA(20), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(21), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(22), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(23), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(24), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(25), GPIO_FUNC_1);
}
#endif

#ifdef CONFIG_BOOT_MMC_PC_4BIT
static void set_gpio_pc_as_mmc1_4bit(void) {
    gpio_set_func(GPIO_PC(0), GPIO_FUNC_0);
    gpio_set_func(GPIO_PC(1), GPIO_FUNC_0);
    gpio_set_func(GPIO_PC(2), GPIO_FUNC_0);
    gpio_set_func(GPIO_PC(3), GPIO_FUNC_0);
    gpio_set_func(GPIO_PC(4), GPIO_FUNC_0);
    gpio_set_func(GPIO_PC(5), GPIO_FUNC_0);
}
#endif

void mmc_init(void) {
    /*
     * Init gpio
     */
#if (defined CONFIG_BOOT_MMC_PA_8BIT)
    set_gpio_pa_as_mmc0_8bit();
#elif (defined CONFIG_BOOT_MMC_PA_4BIT)
    set_gpio_pa_as_mmc0_4bit();
#elif (defined CONFIG_BOOT_MMC_PC_4BIT)
    set_gpio_pc_as_mmc1_4bit();
#else
#error Unknown mmc I/O port!
#endif
}
