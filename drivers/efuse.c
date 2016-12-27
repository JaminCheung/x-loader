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
#include <generated/efuse_reg_values.h>

#define EFUSE_CTRL		0x0
#define EFUSE_CFG		0x4
#define EFUSE_STATE		0x8
#define EFUSE_DATA(n)   (0xC + (n)*4)

#define EFU_ROM_BASE	0x200
#define CHIP_ID_ADDR	(0x200)
#define CHIP_ID_END	    (0x20F)
#define CHIP_ID_SIZE	(128)
#define USER_ID_ADDR	(0x220)
#define USER_ID_END	    (0x23D)
#define USER_ID_SIZE	(240)

#define efuse_readl(offset) readl(EFUSE_BASE + offset)
#define efuse_writel(value, offset) writel(value, EFUSE_BASE + offset)


static int efuse_read_data(void *buf, unsigned int addr, int length) {
    int i;
    unsigned int *pbuf = buf;
    unsigned int val, word_num;

    word_num = (length + 3) / 4;

    /* clear read done status */
    efuse_writel(0, EFUSE_STATE);

    /*indicat addr and length */
    val = addr << 21 | length << 16;
    /* enable read */
    val |= 1;
    efuse_writel(val, EFUSE_CTRL);

    /* wait read done status */
    while(!(efuse_readl(EFUSE_STATE) &1));

    for(i = 0; i < word_num; i ++) {
         val = efuse_readl(EFUSE_DATA(i));
        *(pbuf + i) = val;
        debug("word%d: 0x%x\n", i, val);
    }

    return word_num;
}

int efuse_read(void *buf, int seg_id, int length) {
    int ret = 0;
    int val = 0;
    int bits = length * 8;

    if(buf == NULL) {
        uart_puts("Error: buf pointer cannot be NULL\n");
        return -1;
    }

    /* set efuse configure resister */
    val = EFUCFG_RD_ADJ << 20 | EFUCFG_RD_STROBE << 16;
    efuse_writel(val, EFUSE_CFG);

    switch(seg_id) {
    case CHIP_ID:
        if(bits > CHIP_ID_SIZE) {
            uart_puts("Chip id size error\n");
            return -1;
        }

        ret = efuse_read_data(buf, CHIP_ID_ADDR, length);
        break;

    case USER_ID:
        if(bits > USER_ID_SIZE) {
            uart_puts("User id size error\n");
            return -1;
        }

        ret = efuse_read_data(buf, USER_ID_ADDR, length);
        break;

    default:
        printf("segment_id:%d not support read\n", seg_id);
        return -1;
    }

    return ret;
}
