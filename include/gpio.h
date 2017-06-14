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

#ifndef GPIO_H
#define GPIO_H

#define GPIO_PA(n)  (0*32 + n)
#define GPIO_PB(n)  (1*32 + n)
#define GPIO_PC(n)  (2*32 + n)
#define GPIO_PD(n)  (3*32 + n)

#define GSS_OUTPUT_LOW          1
#define GSS_OUTPUT_HIGH         2
#define GSS_INPUT_PULL          3
#define GSS_INPUT_NOPULL        4
#define GSS_IGNORE              5
#define GSS_TABLET_END          0x999

enum gpio_function {
    GPIO_FUNC_0     = 0x00,  //0000, GPIO as function 0 / device 0
    GPIO_FUNC_1     = 0x01,  //0001, GPIO as function 1 / device 1
    GPIO_FUNC_2     = 0x02,  //0010, GPIO as function 2 / device 2
    GPIO_FUNC_3     = 0x03,  //0011, GPIO as function 3 / device 3
    GPIO_INT_FE     = 0x0a,  //1010, Fall Edge trigger interrupt
    GPIO_INT_RE     = 0x0b,  //1011, Rise Edge trigger interrupt
};

enum gpio_port {
    GPIO_PORT_A,
    GPIO_PORT_B,
    GPIO_PORT_C,
    GPIO_PORT_D,
    /* this must be last */
    GPIO_NR_PORTS,
};

#define MAX_GPIO_NUM    192

#define PXPIN       0x00   /* PIN Level Register */
#define PXINT       0x10   /* Port Interrupt Register */
#define PXINTS      0x14   /* Port Interrupt Set Register */
#define PXINTC      0x18   /* Port Interrupt Clear Register */
#define PXMSK       0x20   /* Port Interrupt Mask Reg */
#define PXMSKS      0x24   /* Port Interrupt Mask Set Reg */
#define PXMSKC      0x28   /* Port Interrupt Mask Clear Reg */
#define PXPAT1      0x30   /* Port Pattern 1 Set Reg. */
#define PXPAT1S     0x34   /* Port Pattern 1 Set Reg. */
#define PXPAT1C     0x38   /* Port Pattern 1 Clear Reg. */
#define PXPAT0      0x40   /* Port Pattern 0 Register */
#define PXPAT0S     0x44   /* Port Pattern 0 Set Register */
#define PXPAT0C     0x48   /* Port Pattern 0 Clear Register */
#define PXFLG       0x50   /* Port Flag Register */
#define PXFLGC      0x58   /* Port Flag clear Register */
#define PXPE        0x70   /* Port Pull Disable Register */
#define PXPES       0x74   /* Port Pull Disable Set Register */
#define PXPEC       0x78   /* Port Pull Disable Clear Register */

#define GPIO_PXPIN(n)   (GPIO_BASE + (PXPIN + (n)*0x100)) /* PIN Level Register */
#define GPIO_PXINT(n)   (GPIO_BASE + (PXINT + (n)*0x100)) /* Port Interrupt Register */
#define GPIO_PXINTS(n)  (GPIO_BASE + (PXINTS + (n)*0x100)) /* Port Interrupt Set Register */
#define GPIO_PXINTC(n)  (GPIO_BASE + (PXINTC + (n)*0x100)) /* Port Interrupt Clear Register */
#define GPIO_PXMSK(n)   (GPIO_BASE + (PXMSK + (n)*0x100)) /* Port Interrupt Mask Register */
#define GPIO_PXMSKS(n)  (GPIO_BASE + (PXMSKS + (n)*0x100)) /* Port Interrupt Mask Set Reg */
#define GPIO_PXMSKC(n)  (GPIO_BASE + (PXMSKC + (n)*0x100)) /* Port Interrupt Mask Clear Reg */
#define GPIO_PXPAT1(n)  (GPIO_BASE + (PXPAT1 + (n)*0x100)) /* Port Pattern 1 Register */
#define GPIO_PXPAT1S(n) (GPIO_BASE + (PXPAT1S + (n)*0x100)) /* Port Pattern 1 Set Reg. */
#define GPIO_PXPAT1C(n) (GPIO_BASE + (PXPAT1C + (n)*0x100)) /* Port Pattern 1 Clear Reg. */
#define GPIO_PXPAT0(n)  (GPIO_BASE + (PXPAT0 + (n)*0x100)) /* Port Pattern 0 Register */
#define GPIO_PXPAT0S(n) (GPIO_BASE + (PXPAT0S + (n)*0x100)) /* Port Pattern 0 Set Register */
#define GPIO_PXPAT0C(n) (GPIO_BASE + (PXPAT0C + (n)*0x100)) /* Port Pattern 0 Clear Register */
#define GPIO_PXFLG(n)   (GPIO_BASE + (PXFLG + (n)*0x100)) /* Port Flag Register */
#define GPIO_PXFLGC(n)  (GPIO_BASE + (PXFLGC + (n)*0x100)) /* Port Flag clear Register */
#define GPIO_PXPE(n)    (GPIO_BASE + (PXPE + (n)*0x100)) /* Port Pull Disable Register */
#define GPIO_PXPES(n)   (GPIO_BASE + (PXPES + (n)*0x100)) /* Port Pull Disable Set Register */
#define GPIO_PXPEC(n)   (GPIO_BASE + (PXPEC + (n)*0x100)) /* Port Pull Disable Clear Register */

void gpio_set_func(unsigned int gpio, enum gpio_function func);
void gpio_enable_pull(unsigned gpio);
void gpio_disable_pull(unsigned gpio);
void gpio_direction_input(unsigned gpio);
void gpio_direction_output(unsigned gpio, int value);
int gpio_get_value(unsigned gpio);

void mmc_set_gpio(void);
void sfc_set_gpio_pa_as_6bit(void);
void console_set_gpio(void);

#endif /* GPIO_H */
