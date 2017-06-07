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

#define REG32(addr) *(volatile unsigned int *)(addr)

#define EFUSE_CTRL	    0xb3540000
#define EFUSE_CFG	    0xb3540004
#define EFUSE_STATE	    0xb3540008
#define EFUSE_DATA0	    0xb354000C

enum socid {
    X1000 = 0xff00,
    X1000E = 0xff01,
    X1500 = 0xff02,
    X1000_NEW = 0xff08,
    X1000E_NEW = 0xff09,
    X1500_NEW = 0xff0a,
};

#ifdef CONFIG_CHECK_SOC_ID
int ddr_autosr = 0;
#else
int ddr_autosr = 1;
#endif

#define LOTID_LOW          0x07
#define LOTID_LOW_MASK     0x1F
#define LOTID_HIGH         0x0E90E02F
#define LOTID_HIGH_MASK    0x3FFFFFFF
#define WAFERID_BIT_SHIFT  11
#define WAFERID_MASK       0x1f


void check_socid(void)
{
    unsigned int socid, waferid;
    unsigned int data1, data3;

    efuse_read(&socid, 0x3c + EFU_ROM_BASE, sizeof(socid));
    socid &= 0xffff;

    switch(socid) {
    case X1000E:
    case X1000_NEW:
    case X1000E_NEW:
    case X1500_NEW:
        goto check_finished;
    case X1500: {
        efuse_read(&data1, 0x04 + EFU_ROM_BASE, sizeof(data1));
        efuse_read(&data3, 0x0c + EFU_ROM_BASE, sizeof(data3));

        if ((data1 & LOTID_LOW_MASK) == LOTID_LOW && \
            (data3 & LOTID_HIGH_MASK) == LOTID_HIGH) {
            waferid = (data1 >> WAFERID_BIT_SHIFT) & WAFERID_MASK;
            if (waferid >= 0x10 && waferid <= 0x19) {
                goto check_finished;
            }
        }
    }
    case X1000:
    case 0:
    default:
        ddr_autosr = 0;
    }

    return;

check_finished:
    ddr_autosr = 1;
    uart_puts("\nR");
}
