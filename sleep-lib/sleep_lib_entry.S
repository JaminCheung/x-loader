/*
 * Copyright (C) 2016 Ingenic Semiconductor
 *
 * SunWenZhong(Fighter) <wenzhong.sun@ingenic.com, wanmyqawdr@126.com>
 *
 * For project-5
 *
 * Release under GPLv2
 *
 */


#include <common.h>

    .extern restore_context
    .extern enter_sleep

    .extern context

    .global sleep_lib_entry

    .text
sleep_lib_entry:
    .word restore_context
    .word enter_sleep

    .global save_context_goto
    .align  2
save_context_goto:
    .set push
    .set noreorder
    .set noat

    la  k1, context
    lw  k1, 0(k1)

    /* s0-s7 */
    sw  s0, 0*4(k1)
    sw  s1, 1*4(k1)
    sw  s2, 2*4(k1)
    sw  s3, 3*4(k1)
    sw  s4, 4*4(k1)
    sw  s5, 5*4(k1)
    sw  s6, 6*4(k1)
    sw  s7, 7*4(k1)

    /* gp,sp,fp,ra */
    sw  gp, 8*4(k1)
    sw  sp, 9*4(k1)
    sw  fp, 10*4(k1)
    sw  ra, 11*4(k1)

    mfc0    t0, CP0_PAGEMASK, 0
    sw  t0, 12*4(k1)

    mfc0    t0, CP0_PAGEMASK, 4
    sw  t0, 13*4(k1)

    mfc0    t0, CP0_STATUS, 0
    sw  t0, 14*4(k1)

    mfc0    t0, CP0_STATUS, 1
    sw  t0, 15*4(k1)

    mfc0    t0, CP0_CAUSE, 0
    sw  t0, 16*4(k1)

    mfc0    t0, CP0_EBASE, 1
    sw  t0, 17*4(k1)

    mfc0    t0, CP0_CONFIG, 0
    sw  t0, 18*4(k1)

    mfc0    t0, CP0_CONFIG, 1
    sw  t0, 19*4(k1)

    mfc0    t0, CP0_CONFIG, 2
    sw  t0, 20*4(k1)

    mfc0    t0, CP0_CONFIG, 3
    sw  t0, 21*4(k1)

    mfc0    t0, CP0_CONFIG, 7
    sw  t0, 22*4(k1)

    mfc0    t0, CP0_LLADDR, 0
    sw  t0, 23*4(k1)

    mfc0    t0, PMON_CSR
    sw  t0, 24*4(k1)

    mfc0    t0, PMON_HIGH
    sw  t0, 25*4(k1)

    mfc0    t0, PMON_LC
    sw  t0, 26*4(k1)

    mfc0    t0, PMON_RC
    sw  t0, 27*4(k1)

    mfc0    t0, CP0_WATCHLO, 0
    sw  t0, 28*4(k1)

    mfc0    t0, CP0_WATCHHI, 0
    sw  t0, 29*4(k1)

	mfc0    t0, CP0_ECC, 0
	sw  t0, 30*4(k1)

    jr  a0
    nop

    .set at
    .set reorder
    .set pop

    .global restore_context_goto
    .align  2
restore_context_goto:
    .set push
    .set noreorder
    .set noat

    la  k1, context
    lw  k1, 0(k1)

    lw  t0, 12*4(k1)
    mtc0    t0, CP0_PAGEMASK, 0

    lw  t0, 13*4(k1)
    mtc0    t0, CP0_PAGEMASK, 4

    lw  t0, 14*4(k1)
    mtc0    t0, CP0_STATUS, 0

    lw  t0, 15*4(k1)
    mtc0    t0, CP0_STATUS, 1

    lw  t0, 16*4(k1)
    mtc0    t0, CP0_CAUSE, 0

    lw  t0, 17*4(k1)
    mtc0    t0, CP0_EBASE, 1

    lw  t0, 18*4(k1)
    mtc0    t0, CP0_CONFIG, 0

    lw  t0, 19*4(k1)
    mtc0    t0, CP0_CONFIG, 1

    lw  t0, 20*4(k1)
    mtc0    t0, CP0_CONFIG, 2

    lw  t0, 21*4(k1)
    mtc0    t0, CP0_CONFIG, 3

    lw  t0, 22*4(k1)
    mtc0    t0, CP0_CONFIG, 7

    lw  t0, 23*4(k1)
    mtc0    t0, CP0_LLADDR, 0

    lw  t0, 24*4(k1)
    mtc0    t0, PMON_CSR

    lw  t0, 25*4(k1)
    mtc0    t0, PMON_HIGH

    lw  t0, 26*4(k1)
    mtc0    t0, PMON_LC

    lw  t0, 27*4(k1)
    mtc0    t0, PMON_RC

    lw  t0, 28*4(k1)
    mtc0    t0, CP0_WATCHLO, 0

    lw  t0, 29*4(k1)
    mtc0    t0, CP0_WATCHHI, 0

    lw  t0, 30*4(k1)
    mtc0    t0, CP0_ECC, 0

    /* restore regs */
    lw  s0, 0*4(k1)
    lw  s1, 1*4(k1)
    lw  s2, 2*4(k1)
    lw  s3, 3*4(k1)
    lw  s4, 4*4(k1)
    lw  s5, 5*4(k1)
    lw  s6, 6*4(k1)
    lw  s7, 7*4(k1)

    lw  gp, 8*4(k1)
    lw  sp, 9*4(k1)
    lw  fp, 10*4(k1)
    lw  ra, 11*4(k1)

    jr ra
    nop

    .set at
    .set reorder
    .set pop
