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


#ifndef EFUSE_H
#define EFUSE_H

#define EFU_ROM_BASE  (0x200)

#define CHIP_ID_ADDR  (0x200)
#define CHIP_ID_END   (0x20F)
#define CHIP_ID_SIZE  (128)
#define RN_ADDR       (0x210)
#define RN_END        (0x21F)
#define RN_SIZE       (128)
#define CUT_ID_ADDR   (0x220)
#define CUT_ID_END    (0x23D)
#define CUT_ID_SIZE   (240)
#define PTR_ADDR      (0x23E)
#define PTR_END       (0x23F)
#define PTR_SIZE      (16)

#define efuse_readl(offset) readl(EFUSE_BASE + offset)
#define efuse_writel(value, offset) writel(value, EFUSE_BASE + offset)

int efuse_read(void *buf, uint32_t addr, int length);

#endif /* EFUSE */
