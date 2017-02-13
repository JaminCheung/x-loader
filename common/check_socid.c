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

int ddr_autosr = 0;


int check_socid(void)
{
    int socid;

    efuse_read(&socid, 0x3c + EFU_ROM_BASE, sizeof(socid));
    socid &= 0xffff;

    switch(socid) {
    case X1000E:
    case X1000_NEW:
    case X1000E_NEW:
    case X1500_NEW:
        ddr_autosr = 1;
        uart_puts("\nR");
        break;
    case X1500:
    case X1000:
    case 0:
        ddr_autosr = 0;
        break;
    default:
        socid = -1;
        uart_puts("Check SOC id failed!\n");
    }

    return socid;
}
