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

int gpio_get_value(unsigned gpio) {
    unsigned port = gpio / 32;
    unsigned pin = gpio % 32;

    return !!(readl(GPIO_PXPIN(port)) & (1 << pin));
}

void gpio_direction_input(unsigned gpio) {
    unsigned port = gpio / 32;
    unsigned pin = gpio % 32;

    writel(1 << pin, GPIO_PXINTC(port));
    writel(1 << pin, GPIO_PXMSKS(port));
    writel(1 << pin, GPIO_PXPAT1S(port));
}

void gpio_direction_output(unsigned gpio, int value) {
    unsigned port = gpio / 32;
    unsigned pin = gpio % 32;

    writel(1 << pin, GPIO_PXINTC(port));
    writel(1 << pin, GPIO_PXMSKS(port));
    writel(1 << pin, GPIO_PXPAT1C(port));

    if (value)
        writel(1 << pin, GPIO_PXPAT0S(port));
    else
        writel(1 << pin, GPIO_PXPAT0C(port));
}

void gpio_enable_pull(unsigned gpio) {
    unsigned port = gpio / 32;
    unsigned pin = gpio % 32;

    writel(1 << pin, GPIO_PXPEC(port));
}

void gpio_disable_pull(unsigned gpio) {
    unsigned port = gpio / 32;
    unsigned pin = gpio % 32;

    writel(1 << pin, GPIO_PXPES(port));
}

void gpio_set_func(unsigned int gpio, enum gpio_function func) {
    int port = gpio / 32;
    int pin = gpio % 32;

    switch (func) {
    case GPIO_FUNC_0:
        writel(1 << pin, GPIO_PXINTC(port));
        writel(1 << pin, GPIO_PXMSKC(port));
        writel(1 << pin, GPIO_PXPAT1C(port));
        writel(1 << pin, GPIO_PXPAT0C(port));
        break;

    case GPIO_FUNC_1:
        writel(1 << pin, GPIO_PXINTC(port));
        writel(1 << pin, GPIO_PXMSKC(port));
        writel(1 << pin, GPIO_PXPAT1C(port));
        writel(1 << pin, GPIO_PXPAT0S(port));
        break;

    case GPIO_FUNC_2:
        writel(1 << pin, GPIO_PXINTC(port));
        writel(1 << pin, GPIO_PXMSKC(port));
        writel(1 << pin, GPIO_PXPAT1S(port));
        writel(1 << pin, GPIO_PXPAT0C(port));
        break;

    case GPIO_FUNC_3:
        writel(1 << pin, GPIO_PXINTC(port));
        writel(1 << pin, GPIO_PXMSKC(port));
        writel(1 << pin, GPIO_PXPAT1S(port));
        writel(1 << pin, GPIO_PXPAT0S(port));
        break;
    }
}
