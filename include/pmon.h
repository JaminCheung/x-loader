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

#ifndef PMON_H
#define PMON_H

#include <common.h>

#define get_pmon_csr()      __read_32bit_c0_register($16, 7)
#define set_pmon_csr(val)   __write_32bit_c0_register($16, 7, val)

#define get_pmon_high()     __read_32bit_c0_register($16, 4)
#define set_pmon_high(val)  __write_32bit_c0_register($16, 4, val)
#define get_pmon_lc()       __read_32bit_c0_register($16, 5)
#define set_pmon_lc(val)    __write_32bit_c0_register($16, 5, val)
#define get_pmon_rc()       __read_32bit_c0_register($16, 6)
#define set_pmon_rc(val)    __write_32bit_c0_register($16, 6, val)

#define pmon_clear_cnt() do {           \
        set_pmon_high(0);       \
        set_pmon_lc(0);         \
        set_pmon_rc(0);         \
    } while(0)

#define pmon_start() do {           \
        unsigned int csr;       \
        csr = get_pmon_csr();       \
        csr |= 0x100;           \
        set_pmon_csr(csr);      \
    } while(0)
#define pmon_stop() do {            \
        unsigned int csr;       \
        csr = get_pmon_csr();       \
        csr &= ~0x100;          \
        set_pmon_csr(csr);      \
    } while(0)

#define PMON_EVENT_CYCLE 0
#define PMON_EVENT_CACHE 1
#define PMON_EVENT_INST  2

#define pmon_prepare(event) do {        \
        unsigned int csr;       \
        pmon_stop();            \
        pmon_clear_cnt();       \
        csr = get_pmon_csr();       \
        csr &= ~0xf000;         \
        csr |= (event)<<12;     \
        set_pmon_csr(csr);      \
    } while(0)


#define pmon_get_cnt32(lc,rc) do {      \
        lc = get_pmon_lc();     \
        rc = get_pmon_rc();     \
    } while(0)


#define pmon_btb_invalid() do {         \
        unsigned int csr;       \
        csr = get_pmon_csr();       \
        csr |= 0x2;         \
        set_pmon_csr(csr);      \
    } while(0)

#define pmon_btb_enable() do {          \
        unsigned int csr;       \
        csr = get_pmon_csr();       \
        csr &= ~0x1;            \
        set_pmon_csr(csr);      \
    } while(0)

#define pmon_btb_disable() do {         \
        pmon_btb_invalid();     \
        unsigned int csr;       \
        csr = get_pmon_csr();       \
        csr |= 0x1;         \
        set_pmon_csr(csr);      \
    } while(0)

#endif
