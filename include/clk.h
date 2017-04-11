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

#ifndef CLK_H
#define CLK_H

enum clk_id {
    DDR,
    I2S,
    PCM,
    MACPHY,
    LCD,
    MSC,
    MSC0 = MSC,
    MSC1,
    OTG,
    SFC,
    SSI = SFC,
    CIM,
    CGU_CNT,
    CPU = CGU_CNT,
    H2CLK,
    APLL,
    MPLL,
    EXCLK,
};

void clk_init(void);
void enable_uart_clk(void);
void enable_aes_clk(void);
uint32_t get_mmc_freq(void);
uint32_t get_ahb_rate(void);
uint32_t get_ddr_rate(void);
void set_mmc_freq(uint32_t);
void set_sfc_freq(uint32_t);

#endif /* CLK_H */
