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

struct spi_mode_peer spi_mode_local[] = {
    [SPI_MODE_STANDARD] = {TRAN_SPI_STANDARD, CMD_READ},
    [SPI_MODE_QUAD] = {TRAN_SPI_QUAD, CMD_QUAD_READ},
};

#ifndef CONFIG_SPI_STANDARD
void spinor_set_quad_mode(void)
{
    unsigned int buf;
    unsigned int tmp;
    int i = 10;
    struct jz_sfc sfc;

    SFC_SEND_COMMAND(&sfc,CMD_WREN,0,0,0,0,0,1);
    SFC_SEND_COMMAND(&sfc,CMD_WRSR_1,1,0,0,0,1,1);
    tmp = 0x02;
    sfc_write_data(&tmp,1);

    SFC_SEND_COMMAND(&sfc,CMD_RDSR,1,0,0,0,1,0);
    sfc_read_data(&tmp, 1);
    while(tmp & CMD_SR_WIP) {
        SFC_SEND_COMMAND(&sfc,CMD_RDSR,1,0,0,0,1,0);
        sfc_read_data(&tmp, 1);
    }

    SFC_SEND_COMMAND(&sfc,CMD_RDSR_1,1,0,0,0,1,0);
    sfc_read_data(&buf, 1);
    while(!(buf & 0x2)&&((i--) > 0)) {
        SFC_SEND_COMMAND(&sfc,CMD_RDSR_1,1,0,0,0,1,0);
        sfc_read_data(&buf, 1);
    }
}
#endif

static int spinor_read(unsigned int offset, unsigned int len, void *data)
{
    int addr_len = 3;
    struct jz_sfc sfc;
    int ret;

#ifndef CONFIG_SPI_STANDARD
    int dummy_byte = 8;
    SFC_SEND_COMMAND(&sfc,SPI_MODE_QUAD,len,offset,addr_len,dummy_byte,1,0);
#else
    SFC_SEND_COMMAND(&sfc,SPI_MODE_STANDARD,len,offset,addr_len,0,1,0);
#endif

    ret = sfc_read_data(data, len);
    if (ret)
        return ret;
    else
        return 0;
}

static void spinor_init(void) {
#ifndef CONFIG_SPI_STANDARD
    spinor_set_quad_mode();
#endif
}

static int install_sleep_lib(void) {
    return spinor_read(SLEEP_LIB_OFFSET, SLEEP_LIB_LENGTH, (void *)SLEEP_LIB_TCSM);
}

int spinor_load(unsigned int src_addr, unsigned int count, unsigned int dst_addr)
{
    sfc_init();
    spinor_init();

    if (install_sleep_lib() < 0){
        printf("install sleep lib failed\n");
        return -1;
    }

    return spinor_read(src_addr, count, (void *)dst_addr);
}
