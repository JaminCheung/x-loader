/*
 * Copyright (C) 2016 Ingenic Semiconductor
 *
 * SunWenZhong(Fighter) <wenzhong.sun@ingenic.com, wanmyqawdr@126.com>
 *
 * For project-5
 *
 * Release under GPLv2
 *
 */

#include <common.h>

static struct jz_uart *uart =
        (struct jz_uart *)(UART0_BASE + CONFIG_CONSOLE_INDEX * 0x1000);

void uart_putc(const char c)
{
    if (c == '\n')
        uart_putc('\r');

    writeb((uint8_t)c, &uart->rbr_thr_dllr);

    /* Wait for fifo to shift out some bytes */
    while (!((readb(&uart->lsr) &
            (UART_LSR_TDRQ | UART_LSR_TEMT)) == 0x60))
        continue;
}

void uart_puts(const char *s)
{
    while (*s)
        uart_putc(*s++);
}

