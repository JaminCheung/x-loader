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

#if (defined CONFIG_BURN_MMC || defined CONFIG_BOOT_MMC)

#ifdef CONFIG_MMC_PA_8BIT
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

#ifdef CONFIG_MMC_PA_4BIT
static void set_gpio_pa_as_mmc0_4bit(void) {
    gpio_set_func(GPIO_PA(20), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(21), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(22), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(23), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(24), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(25), GPIO_FUNC_1);
}
#endif

#ifdef CONFIG_MMC_PC_4BIT
static void set_gpio_pc_as_mmc1_4bit(void) {
    gpio_set_func(GPIO_PC(0), GPIO_FUNC_0);
    gpio_set_func(GPIO_PC(1), GPIO_FUNC_0);
    gpio_set_func(GPIO_PC(2), GPIO_FUNC_0);
    gpio_set_func(GPIO_PC(3), GPIO_FUNC_0);
    gpio_set_func(GPIO_PC(4), GPIO_FUNC_0);
    gpio_set_func(GPIO_PC(5), GPIO_FUNC_0);
}
#endif

void mmc_set_gpio(void) {
#if (defined CONFIG_MMC_PA_8BIT)
    set_gpio_pa_as_mmc0_8bit();
#elif (defined CONFIG_MMC_PA_4BIT)
    set_gpio_pa_as_mmc0_4bit();
#elif (defined CONFIG_MMC_PC_4BIT)
    set_gpio_pc_as_mmc1_4bit();
#else
#error Unknown mmc I/O port!
#endif
}

#endif /* CONFIG_BOOT_MMC || CONFIG_BURN_MMC */

#if (defined CONFIG_BOOT_SFC || defined CONFIG_BURN_SPI_FLASH)
void sfc_set_gpio_pa_as_6bit(void) {
    gpio_set_func(GPIO_PA(26), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(27), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(28), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(29), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(30), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(31), GPIO_FUNC_1);
}
#endif /* CONFIG_BOOT_SFC || CONFIG_BURN_SPI_FLASH */

#ifdef CONFIG_CONSOLE_ENABLE

void console_set_gpio(void) {

#if (CONFIG_CONSOLE_INDEX == 0)

#ifdef CONFIG_CONSOLE_PC
    gpio_set_func(GPIO_PC(10), GPIO_FUNC_0);
    gpio_set_func(GPIO_PC(11), GPIO_FUNC_0);
#else
#error Unknown console 0 I/O port!
#endif

#endif /* CONFIG_CONSOLE_INDEX == 0 */

#if (CONFIG_CONSOLE_INDEX == 1)

#if (defined CONFIG_CONSOLE_PA)
    gpio_set_func(GPIO_PA(4), GPIO_FUNC_2);
    gpio_set_func(GPIO_PA(5), GPIO_FUNC_2);

#elif (defined CONFIG_CONSOLE_PD)
    gpio_set_func(GPIO_PD(2), GPIO_FUNC_1);
    gpio_set_func(GPIO_PD(3), GPIO_FUNC_1);
#else
#error Unknown console 1 I/O port!
#endif

#endif /* CONFIG_CONSOLE_INDEX == 1 */

#if (CONFIG_CONSOLE_INDEX == 2)

#if (defined CONFIG_CONSOLE_PD)
    gpio_set_func(GPIO_PD(4), GPIO_FUNC_0);
    gpio_set_func(GPIO_PD(5), GPIO_FUNC_0);

#elif (defined CONFIG_CONSOLE_PA)
    gpio_set_func(GPIO_PA(2), GPIO_FUNC_2);
    gpio_set_func(GPIO_PA(3), GPIO_FUNC_2);

#elif (defined CONFIG_CONSOLE_PC)
    gpio_set_func(GPIO_PC(30), GPIO_FUNC_1);
    gpio_set_func(GPIO_PC(31), GPIO_FUNC_1);

#else
#error Unknown console 2 I/O port!
#endif

#endif /* CONFIG_CONSOLE_INDEX == 2 */
}

#endif /* CONFIG_CONSOLE_ENABLE */
