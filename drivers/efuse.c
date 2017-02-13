/*
 *  Copyright (C) 2016 Ingenic Semiconductor Co.,Ltd
 *
 *  X1000 series bootloader for u-boot/rtos/linux
 *
 *  Wang Qiuwei <qiuwei.wang@ingenic.com>
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
#include <efuse.h>
#include <generated/efuse_reg_values.h>

#define EFUSE_CTRL		0x0
#define EFUSE_CFG		0x4
#define EFUSE_STATE		0x8
#define EFUSE_DATA(n)   (0xC + (n)*4)

static int check_vaild(uint32_t addr, int length)
{
    uint32_t length_bits = length * 8;

    if (addr >= CHIP_ID_ADDR && addr <= CHIP_ID_END) {
        debug("chip id\n");
        if (length_bits > CHIP_ID_SIZE) {
            length_bits = CHIP_ID_SIZE;
            goto error;
        }
    } else if ( addr >= RN_ADDR && addr <= RN_END) {
        debug("random number\n");
        if (length_bits > RN_SIZE) {
            length_bits = RN_SIZE;
            goto error;
        }
    } else if (addr >= CUT_ID_ADDR && addr <= CUT_ID_END) {
        debug("customer id\n");
        if (length_bits > CUT_ID_SIZE) {
            length_bits = CUT_ID_SIZE;
            goto error;
        }
    } else if (addr >= PTR_ADDR && addr <= PTR_END) {
        debug("protect id\n");
        if (length_bits > PTR_SIZE) {
            length_bits = PTR_SIZE;
            goto error;
        }
    } else {
        printf("Err: invalid addr!\n");
        return -1;
    }

    return 0;
error:
    printf("ERR: length max than %d bits\n", length_bits);
    return -1;
}

int efuse_read(void *buf, uint32_t addr, int length) {
    int i;
    uint32_t *pbuf = buf;
    uint32_t val, word_num;

    if (check_vaild(addr, length))
        return -1;

    addr -= EFU_ROM_BASE;
    word_num = (length + 3) / 4;

    /* set efuse configure resister */
    val = EFUCFG_RD_ADJ << 20 | EFUCFG_RD_STROBE << 16;
    efuse_writel(val, EFUSE_CFG);

    /* clear read done status */
    efuse_writel(0, EFUSE_STATE);

    /*indicat addr and length, enable read */
    val = addr << 21 | length << 16 | 1;
    efuse_writel(val, EFUSE_CTRL);

    /* wait read done status */
    while(!(efuse_readl(EFUSE_STATE) & 1));

    for(i = 0; i < word_num; i ++) {
         val = efuse_readl(EFUSE_DATA(i));
        *(pbuf + i) = val;
        debug("word%d: 0x%x\n", i, val);
    }

    return word_num;
}
