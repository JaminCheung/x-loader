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

enum segment_id {
    CHIP_ID = 0,
    USER_ID = 2,
};

int efuse_init(void);
int efuse_read(void *buf, int seg_id, int length);

#endif /* EFUSE */
