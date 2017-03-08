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

void board_early_init(void) {
    gpio_set_func(GPIO_PC(30), GPIO_FUNC_1);
    gpio_set_func(GPIO_PC(31), GPIO_FUNC_1);

#ifdef CONFIG_BURN_SPI_FLASH
    gpio_set_func(GPIO_PA(26), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(27), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(28), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(29), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(30), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(31), GPIO_FUNC_1);
#endif
}

void board_init(void) {
#ifdef CONFIG_RTCCLK_SEL
    rtc_change_sel();
#endif
}
