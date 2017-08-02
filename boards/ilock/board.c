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
    //enable 3.3V - DCDC
    gpio_direction_output(GPIO_PC(9), 1);

    //disable zigbee_gpio
    gpio_direction_output(GPIO_PD(0), 0);
    gpio_direction_output(GPIO_PD(1), 0);
    gpio_direction_output(GPIO_PD(2), 0);
    gpio_direction_output(GPIO_PD(3), 0);
    gpio_direction_output(GPIO_PD(5), 0);
}

void board_init(void) {
#ifdef CONFIG_RTCCLK_SEL
    rtc_change_sel();
#endif
}
