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

#define src_readl(offset) readl(SFC_BASE + offset)
#define sfc_writel(value, offset) writel(value, SFC_BASE + offset);

inline void sfc_set_mode(int channel, int value)
{
    unsigned int tmp;
    tmp = src_readl(SFC_TRAN_CONF(channel));
    tmp &= ~(TRAN_MODE_MSK);
    tmp |= (value << TRAN_MODE_OFFSET);
    sfc_writel(tmp,SFC_TRAN_CONF(channel));
}

inline void sfc_dev_addr_dummy_bytes(int channel, unsigned int value)
{
    unsigned int tmp;
    tmp = src_readl(SFC_TRAN_CONF(channel));
    tmp &= ~TRAN_CONF_DMYBITS_MSK;
    tmp |= (value << TRAN_CONF_DMYBITS_OFFSET);
    sfc_writel(tmp,SFC_TRAN_CONF(channel));
}

inline void sfc_transfer_direction(int value)
{
    if(value == 0) {
        unsigned int tmp;
        tmp = src_readl(SFC_GLB);
        tmp &= ~TRAN_DIR;
        sfc_writel(tmp,SFC_GLB);
    } else {
        unsigned int tmp;
        tmp = src_readl(SFC_GLB);
        tmp |= TRAN_DIR;
        sfc_writel(tmp,SFC_GLB);
    }
}

inline void sfc_set_length(int value)
{
    sfc_writel(value,SFC_TRAN_LEN);
}

inline void sfc_set_addr_length(int channel, unsigned int value)
{
    unsigned int tmp;
    tmp = src_readl(SFC_TRAN_CONF(channel));
    tmp &= ~(ADDR_WIDTH_MSK);
    tmp |= (value << ADDR_WIDTH_OFFSET);
    sfc_writel(tmp,SFC_TRAN_CONF(channel));
}

inline void sfc_cmd_en(int channel, unsigned int value)
{
    if(value == 1) {
        unsigned int tmp;
        tmp = src_readl(SFC_TRAN_CONF(channel));
        tmp |= CMDEN;
        sfc_writel(tmp,SFC_TRAN_CONF(channel));
    } else {
        unsigned int tmp;
        tmp = src_readl(SFC_TRAN_CONF(channel));
        tmp &= ~CMDEN;
        sfc_writel(tmp,SFC_TRAN_CONF(channel));
    }
}

inline void sfc_data_en(int channel, unsigned int value)
{
    if(value == 1) {
        unsigned int tmp;
        tmp = src_readl(SFC_TRAN_CONF(channel));
        tmp |= DATEEN;
        sfc_writel(tmp,SFC_TRAN_CONF(channel));
    } else {
        unsigned int tmp;
        tmp = src_readl(SFC_TRAN_CONF(channel));
        tmp &= ~DATEEN;
        sfc_writel(tmp,SFC_TRAN_CONF(channel));
    }
}

inline void sfc_write_cmd(int channel, unsigned int value)
{
    unsigned int tmp;
    tmp = src_readl(SFC_TRAN_CONF(channel));
    tmp &= ~CMD_MSK;
    tmp |= value;
    sfc_writel(tmp,SFC_TRAN_CONF(channel));
}

inline void sfc_dev_addr(int channel, unsigned int value)
{
    sfc_writel(value, SFC_DEV_ADDR(channel));
}

inline void sfc_dev_addr_plus(int channel, unsigned int value)
{
    sfc_writel(value,SFC_DEV_ADDR_PLUS(channel));
}

inline void sfc_set_transfer(struct jz_sfc *hw, int dir)
{
    if(dir == 1)
        sfc_transfer_direction(GLB_TRAN_DIR_WRITE);
    else
        sfc_transfer_direction(GLB_TRAN_DIR_READ);
    sfc_set_length(hw->len);
    sfc_set_addr_length(0, hw->addr_len);
    sfc_cmd_en(0, 0x1);
    sfc_data_en(0, hw->daten);
    sfc_write_cmd(0, hw->cmd);
    sfc_dev_addr(0, hw->addr);
    sfc_dev_addr_plus(0, hw->addr_plus);
    sfc_dev_addr_dummy_bytes(0,hw->dummy_byte);
    sfc_set_mode(0,hw->sfc_mode);
}

inline void sfc_send_cmd(struct jz_sfc *sfc, unsigned char dir)
{
    unsigned int reg_tmp = 0;

    sfc_set_transfer(sfc,dir);
    sfc_writel(1 << 2,SFC_TRIG);
    sfc_writel(START,SFC_TRIG);

    /*this must judge the end status*/
    if((sfc->daten == 0)){
        reg_tmp = src_readl(SFC_SR);
        while (!(reg_tmp & END)){
            reg_tmp = src_readl(SFC_SR);
        }

        if ((src_readl(SFC_SR)) & END)
            sfc_writel(CLR_END,SFC_SCR);
    }
}

int sfc_write_data(unsigned int *data, unsigned int length)
{
    unsigned int tmp_len = 0;
    unsigned int fifo_num = 0;
    unsigned int i;
    unsigned int reg_tmp = 0;
    unsigned int  len = (length + 3) / 4 ;

    while(1){
        reg_tmp = src_readl(SFC_SR);
        if (reg_tmp & TRAN_REQ) {
            sfc_writel(CLR_TREQ,SFC_SCR);
            if ((len - tmp_len) > THRESHOLD)
                fifo_num = THRESHOLD;
            else {
                fifo_num = len - tmp_len;
            }

            for (i = 0; i < fifo_num; i++) {
                sfc_writel(*data,SFC_DR);
                data++;
                tmp_len++;
            }
        }
        if (tmp_len == len)
            break;
    }

    reg_tmp = src_readl(SFC_SR);
    while (!(reg_tmp & END)){
        reg_tmp = src_readl(SFC_SR);
    }

    if ((src_readl(SFC_SR)) & END)
        sfc_writel(CLR_END,SFC_SCR);

    return 0;
}

int sfc_read_data(unsigned int *data, unsigned int length)
{
    unsigned int tmp_len = 0;
    unsigned int fifo_num = 0;
    unsigned int i;
    unsigned int reg_tmp = 0;
    unsigned int  len = (length + 3) / 4 ;

    while(1){
        reg_tmp = src_readl(SFC_SR);
        if (reg_tmp & RECE_REQ) {
            sfc_writel(CLR_RREQ,SFC_SCR);
            if ((len - tmp_len) > THRESHOLD)
                fifo_num = THRESHOLD;
            else {
                fifo_num = len - tmp_len;
            }

            for (i = 0; i < fifo_num; i++) {
                *data = src_readl(SFC_DR);
                data++;
                tmp_len++;
            }
        }
        if (tmp_len == len)
            break;
    }

    reg_tmp = src_readl(SFC_SR);
    while (!(reg_tmp & END)){
        reg_tmp = src_readl(SFC_SR);
    }

    if ((src_readl(SFC_SR)) & END)
        sfc_writel(CLR_END,SFC_SCR);


    return 0;
}

static void sfc_set_gpio_pa_as_6bit(void) {
    gpio_set_func(GPIO_PA(26), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(27), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(28), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(29), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(30), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(31), GPIO_FUNC_1);
}

void sfc_init(void) {
    unsigned int i;
    volatile unsigned int tmp;

    sfc_set_gpio_pa_as_6bit();

    set_sfc_freq(CONFIG_SFC_FREQ);

    tmp = src_readl(SFC_GLB);
    tmp &= ~(TRAN_DIR | OP_MODE );
    tmp |= WP_EN;
    sfc_writel(tmp, SFC_GLB);
    tmp = src_readl(SFC_DEV_CONF);
    tmp &= ~(CMD_TYPE | CPHA | CPOL | SMP_DELAY_MSK |
            THOLD_MSK | TSETUP_MSK | TSH_MSK);
    tmp |= (CEDL | HOLDDL | WPDL | 1 << SMP_DELAY_OFFSET);
    sfc_writel(tmp,SFC_DEV_CONF);
    for (i = 0; i < 6; i++) {
        sfc_writel((src_readl(SFC_TRAN_CONF(i))& (~(TRAN_MODE_MSK | FMAT))),SFC_TRAN_CONF(i));
    }
    sfc_writel((CLR_END | CLR_TREQ | CLR_RREQ | CLR_OVER | CLR_UNDER),SFC_INTC);
    sfc_writel(0,SFC_CGE);
    tmp = src_readl(SFC_GLB);
    tmp &= ~(THRESHOLD_MSK);
    tmp |= (THRESHOLD << THRESHOLD_OFFSET);
    sfc_writel(tmp,SFC_GLB);
}
