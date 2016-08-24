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

#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>

#undef NULL
#if defined(__cplusplus)
#define NULL 0
#else
#define NULL ((void *)0)
#endif

typedef __signed__ char __s8;
typedef unsigned char __u8;

typedef __signed__ short __s16;
typedef unsigned short __u16;

typedef __signed__ int __s32;
typedef unsigned int __u32;

typedef __signed__ long long __s64;
typedef unsigned long long __u64;

typedef     __u8        u_int8_t;
typedef     __s8        int8_t;
typedef     __u16       u_int16_t;
typedef     __s16       int16_t;
typedef     __u32       u_int32_t;
typedef     __s32       int32_t;

#ifndef __HOST__
typedef     __s64       int64_t;
typedef     __u64       u_int64_t;
#endif

typedef     __u8        uint8_t;
typedef     __u16       uint16_t;
typedef     __u32       uint32_t;
typedef     __u64       uint64_t;

typedef     __u8        uint8;
typedef     __u16       uint16;
typedef     __u32       uint32;
typedef     __u64       uint64;

typedef __u32 dma_addr_t;

typedef unsigned long phys_addr_t;
typedef unsigned long phys_size_t;

/* bsd */
typedef unsigned char       u_char;
typedef unsigned short      u_short;
typedef unsigned int        u_int;
typedef unsigned long       u_long;

/* sysv */
typedef unsigned char       unchar;
typedef unsigned short      ushort;
typedef unsigned int        uint;
typedef unsigned long       ulong;

#ifndef __HOST__
typedef unsigned int        size_t;
#endif

typedef unsigned long int uintptr_t;

typedef     ulong       lbaint_t;

#endif
