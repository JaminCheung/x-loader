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

#ifndef COMMON_H
#define COMMON_H

#ifndef __ASSEMBLY__
#include <generated/config.h>
#include <types.h>
#include <uart.h>
#include <gpio.h>
#include <cpm.h>
#include <clk.h>
#include <mmc.h>
#include <nand.h>
#include <nor.h>
#include <lpddr/lpddr.h>
#include <lpddr/lpddr_chip.h>
#endif

/*
 *  Configure language
 */
#ifdef __ASSEMBLY__
#define _ULCAST_
#else
#define _ULCAST_ (unsigned long)
#endif

/*
 * Symbolic register names for 32 bit ABI
 */
#define zero    $0  /* wired zero */
#define AT      $1  /* assembler temp  - uppercase because of ".set at" */
#define v0      $2  /* return value */
#define v1      $3
#define a0      $4  /* argument registers */
#define a1      $5
#define a2      $6
#define a3      $7
#define t0      $8  /* caller saved */
#define t1      $9
#define t2      $10
#define t3      $11
#define t4      $12
#define t5      $13
#define t6      $14
#define t7      $15
#define s0      $16 /* callee saved */
#define s1      $17
#define s2      $18
#define s3      $19
#define s4      $20
#define s5      $21
#define s6      $22
#define s7      $23
#define t8      $24 /* caller saved */
#define t9      $25
#define jp      $25 /* PIC jump register */
#define k0      $26 /* kernel scratch */
#define k1      $27
#define gp      $28 /* global pointer */
#define sp      $29 /* stack pointer */
#define fp      $30 /* frame pointer */
#define s8      $30 /* same like fp! */
#define ra      $31 /* return address */

/*
 * Coprocessor 0 register names
 */
#define CP0_INDEX       $0
#define CP0_RANDOM      $1
#define CP0_ENTRYLO0    $2
#define CP0_ENTRYLO1    $3
#define CP0_CONF        $3
#define CP0_CONTEXT     $4
#define CP0_PAGEMASK    $5
#define CP0_WIRED       $6
#define CP0_INFO        $7
#define CP0_BADVADDR    $8
#define CP0_COUNT       $9
#define CP0_ENTRYHI     $10
#define CP0_COMPARE     $11
#define CP0_STATUS      $12
#define CP0_CAUSE       $13
#define CP0_EPC         $14
#define CP0_PRID        $15
#define CP0_CONFIG      $16
#define CP0_LLADDR      $17
#define CP0_WATCHLO     $18
#define CP0_WATCHHI     $19
#define CP0_XCONTEXT    $20
#define CP0_FRAMEMASK   $21
#define CP0_DIAGNOSTIC  $22
#define CP0_DEBUG       $23
#define CP0_DEPC        $24
#define CP0_PERFORMANCE $25
#define CP0_ECC         $26
#define CP0_CACHEERR    $27
#define CP0_TAGLO       $28
#define CP0_TAGHI       $29
#define CP0_ERROREPC    $30
#define CP0_DESAVE      $31


/*
 * Bits in the coprocessor 0 config register.
 */
/* Generic bits.  */
#define CONF_CM_CACHABLE_NO_WA      0
#define CONF_CM_CACHABLE_WA     1
#define CONF_CM_UNCACHED        2
#define CONF_CM_CACHABLE_NONCOHERENT    3
#define CONF_CM_CACHABLE_CE     4
#define CONF_CM_CACHABLE_COW        5
#define CONF_CM_CACHABLE_CUW        6
#define CONF_CM_CACHABLE_ACCELERATED    7
#define CONF_CM_CMASK           7
#define CONF_BE         (_ULCAST_(1) << 15)


/*
 * Cache Operations available on all MIPS processors with R4000-style caches
 */
#define INDEX_INVALIDATE_I      0x00
#define INDEX_WRITEBACK_INV_D   0x01
#define INDEX_LOAD_TAG_I        0x04
#define INDEX_LOAD_TAG_D        0x05
#define INDEX_STORE_TAG_I       0x08
#define INDEX_STORE_TAG_D       0x09
#define HIT_INVALIDATE_I        0x10
#define HIT_INVALIDATE_D        0x11
#define HIT_WRITEBACK_INV_D     0x15

/*
 * R4000-specific cacheops
 */
#define CREATE_DIRTY_EXCL_D     0x0d
#define FILL                    0x14
#define HIT_WRITEBACK_I         0x18
#define HIT_WRITEBACK_D         0x19

/*
 * R4000SC and R4400SC-specific cacheops
 */
#define INDEX_INVALIDATE_SI     0x02
#define INDEX_WRITEBACK_INV_SD  0x03
#define INDEX_LOAD_TAG_SI       0x06
#define INDEX_LOAD_TAG_SD       0x07
#define INDEX_STORE_TAG_SI      0x0A
#define INDEX_STORE_TAG_SD      0x0B
#define CREATE_DIRTY_EXCL_SD    0x0f
#define HIT_INVALIDATE_SI       0x12
#define HIT_INVALIDATE_SD       0x13
#define HIT_WRITEBACK_INV_SD    0x17
#define HIT_WRITEBACK_SD        0x1b
#define HIT_SET_VIRTUAL_SI      0x1e
#define HIT_SET_VIRTUAL_SD      0x1f


/*
 *  Configure language
 */
#ifdef __ASSEMBLY__
#define _ATYPE_
#define _ATYPE32_
#define _ATYPE64_
#define _CONST64_(x)    x
#else
#define _ATYPE_     __PTRDIFF_TYPE__
#define _ATYPE32_   int
#define _ATYPE64_   __s64
#ifdef CONFIG_64BIT
#define _CONST64_(x)    x ## L
#else
#define _CONST64_(x)    x ## LL
#endif
#endif

/*
 *  32-bit MIPS address spaces
 */
#ifdef __ASSEMBLY__
#define _ACAST32_
#define _ACAST64_
#else
#define _ACAST32_       (_ATYPE_)(_ATYPE32_)    /* widen if necessary */
#define _ACAST64_       (_ATYPE64_)     /* do _not_ narrow */
#endif

/*
 * Returns the kernel segment base of a given address
 */
#define KSEGX(a)        ((_ACAST32_ (a)) & 0xe0000000)

/*
 * Returns the physical address of a CKSEGx / XKPHYS address
 */
#define CPHYSADDR(a)        ((_ACAST32_(a)) & 0x1fffffff)
#define XPHYSADDR(a)        ((_ACAST64_(a)) &           \
                 _CONST64_(0x000000ffffffffff))


#define CKSEG0ADDR(a)       (CPHYSADDR(a) | KSEG0)
#define CKSEG1ADDR(a)       (CPHYSADDR(a) | KSEG1)
#define CKSEG2ADDR(a)       (CPHYSADDR(a) | KSEG2)
#define CKSEG3ADDR(a)       (CPHYSADDR(a) | KSEG3)

/*
 * Map an address to a certain kernel segment
 */
#define KSEG0ADDR(a)        (CPHYSADDR(a) | KSEG0)
#define KSEG1ADDR(a)        (CPHYSADDR(a) | KSEG1)
#define KSEG2ADDR(a)        (CPHYSADDR(a) | KSEG2)
#define KSEG3ADDR(a)        (CPHYSADDR(a) | KSEG3)

/*
 * Memory segments (32bit kernel mode addresses)
 * These are the traditional names used in the 32-bit universe.
 */
#define KUSEG           0x00000000
#define KSEG0           0x80000000
#define KSEG1           0xa0000000
#define KSEG2           0xc0000000
#define KSEG3           0xe0000000

#define CKUSEG          0x00000000
#define CKSEG0          0x80000000
#define CKSEG1          0xa0000000
#define CKSEG2          0xc0000000
#define CKSEG3          0xe0000000



/*
 * Define the module base addresses
 */
/* AHB0 BUS Devices Base */
#define HARB0_BASE      0xb3000000
#define DDRC_BASE       0xb34f0000
#define DDR_PHY_OFFSET  (-0x4e0000 + 0x1000)
#define X2D_BASE        0xb3030000
#define GPU_BASE        0xb3040000
#define LCDC0_BASE      0xb3050000
#define CIM_BASE        0xb3060000
#define COMPRESS_BASE   0xb3070000
#define IPU0_BASE       0xb3080000
#define GPVLC_BASE      0xb3090000
#define IPU1_BASE       0xb30b0000
#define MONITOR_BASE    0xb30f0000

/* AHB1 BUS Devices Base */
#define SCH_BASE    0xb3200000
#define VDMA_BASE   0xb3210000
#define EFE_BASE    0xb3240000
#define MCE_BASE    0xb3250000
#define DBLK_BASE   0xb3270000
#define VMAU_BASE   0xb3280000
#define SDE_BASE    0xb3290000
#define AUX_BASE    0xb32a0000
#define TCSM_BASE   0xb32c0000
#define JPGC_BASE   0xb32e0000
#define SRAM_BASE   0xb32f0000

/* AHB2 BUS Devices Base */
#define HARB2_BASE  0xb3400000
#define NEMC_BASE   0xb3410000
#define PDMA_BASE   0xb3420000
#define SFC_BASE    0xb3440000
#define MSC0_BASE   0xb3450000
#define MSC1_BASE   0xb3460000
#define MSC2_BASE   0xb3470000
#define GPS_BASE    0xb3480000
#define EHCI_BASE   0xb3490000
#define OHCI_BASE   0xb34a0000
#define ETHC_BASE   0xb34b0000
#define BCH_BASE    0xb34d0000
#define TSSI0_BASE  0xb34e0000
#define TSSI1_BASE  0xb34f0000
#define OTG_BASE    0xb3500000
#define EFUSE_BASE  0xb3540000

#define OST_BASE    0xb2000000
#define HDMI_BASE   0xb0180000

/* APB BUS Devices Base */
#define CPM_BASE    0xb0000000
#define INTC_BASE   0xb0001000
#define TCU_BASE    0xb0002000
#define RTC_BASE    0xb0003000
#define GPIO_BASE   0xb0010000
#define AIC0_BASE   0xb0020000
#define AIC1_BASE   0xb0021000
#define UART0_BASE  0xb0030000
#define UART1_BASE  0xb0031000
#define UART2_BASE  0xb0032000
#define UART3_BASE  0xb0033000
#define UART4_BASE  0xb0034000
#define SSC_BASE    0xb0040000
#define SSI0_BASE   0xb0043000
#define SSI1_BASE   0xb0044000
#define I2C0_BASE   0xb0050000
#define I2C1_BASE   0xb0051000
#define I2C2_BASE   0xb0052000
#define I2C3_BASE   0xb0053000
#define I2C4_BASE   0xb0054000
#define KMC_BASE    0xb0060000
#define DES_BASE    0xb0061000
#define SADC_BASE   0xb0070000
#define PCM0_BASE   0xb0071000
#define OWI_BASE    0xb0072000
#define PCM1_BASE   0xb0074000
#define WDT_BASE    0xb0002000

/* NAND CHIP Base Address*/
#define NEMC_CS1_BASE 0xbb000000
#define NEMC_CS2_BASE 0xba000000
#define NEMC_CS3_BASE 0xb9000000
#define NEMC_CS4_BASE 0xb8000000
#define NEMC_CS5_BASE 0xb7000000
#define NEMC_CS6_BASE 0xb6000000

#define AUX_BASE    0xb32a0000

/*
 * Cache defines
 */
#define CONFIG_SYS_DCACHE_SIZE      16384
#define CONFIG_SYS_ICACHE_SIZE      16384
#define CONFIG_SYS_CACHELINE_SIZE   32

/*
 * Bits
 */
#define BIT0  (1<<0)
#define BIT1  (1<<1)
#define BIT2  (1<<2)
#define BIT3  (1<<3)
#define BIT4  (1<<4)
#define BIT5  (1<<5)
#define BIT6  (1<<6)
#define BIT7  (1<<7)
#define BIT8  (1<<8)
#define BIT9  (1<<9)
#define BIT10 (1<<10)
#define BIT11 (1<<11)
#define BIT12 (1<<12)
#define BIT13 (1<<13)
#define BIT14 (1<<14)
#define BIT15 (1<<15)
#define BIT16 (1<<16)
#define BIT17 (1<<17)
#define BIT18 (1<<18)
#define BIT19 (1<<19)
#define BIT20 (1<<20)
#define BIT21 (1<<21)
#define BIT22 (1<<22)
#define BIT23 (1<<23)
#define BIT24 (1<<24)
#define BIT25 (1<<25)
#define BIT26 (1<<26)
#define BIT27 (1<<27)
#define BIT28 (1<<28)
#define BIT29 (1<<29)
#define BIT30 (1<<30)
#define BIT31 (1<<31)

/*
 * IO
 */
/*
 * Sane hardware offers swapping of I/O space accesses in hardware; less
 * sane hardware forces software to fiddle with this ...
 */
#if defined(CONFIG_SWAP_IO_SPACE) && defined(__MIPSEB__)

#define __ioswab8(x) (x)
#define __ioswab16(x) swab16(x)
#define __ioswab32(x) swab32(x)

#else
#define __ioswab8(x) (x)
#define __ioswab16(x) (x)
#define __ioswab32(x) (x)
#endif

#define __raw_readb(addr) (*(volatile unsigned char *)(addr))
#define __raw_readw(addr) (*(volatile unsigned short *)(addr))
#define __raw_readl(addr) (*(volatile unsigned int *)(addr))
#define readb(addr) __raw_readb((addr))
#define readw(addr) __ioswab16(__raw_readw((addr)))
#define readl(addr) __ioswab32(__raw_readl((addr)))

#define __raw_writeb(b, addr) (*(volatile unsigned char *)(addr)) = (b)
#define __raw_writew(b, addr) (*(volatile unsigned short *)(addr)) = (b)
#define __raw_writel(b, addr) (*(volatile unsigned int *)(addr)) = (b)
#define writeb(b, addr) __raw_writeb((b), (addr))
#define writew(b, addr) __raw_writew(__ioswab16(b), (addr))
#define writel(b, addr) __raw_writel(__ioswab32(b), (addr))

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

/*
 * Boot next stage
 */
#ifndef __ASSEMBLY__
void boot_next_stage(void);
#endif

/*
 * Console
 */
#ifndef __ASSEMBLY__
void serial_printf(const char *fmt, ...);
void panic(const char *fmt, ...);
#define printf(fmt, args...) serial_printf(fmt, ##args)
#ifdef DEBUG
#define debug(fmt, args...) serial_printf(fmt, ##args)
#else
#define debug(fmt, args...) do{}while(0)
#endif /* DEBUG */
#endif

/*
 * Delay
 */
#ifndef __ASSEMBLY__
void udelay(unsigned long us);
void mdelay(unsigned long ms);
#endif

#ifndef __ASSEMBLY__
uint32_t get_ddr_rate(void);
#endif

#ifndef __ASSEMBLY__
void hang(void);
void hang_reason(const char* reason);
#endif

/*
 * Recovery
 */
#ifndef __ASSEMBLY__
#define NORMAL_BOOT                     1
#define RECOVERY_BOOT                   2

enum {
    KEY_UNPRESS = 0,
    KEY_PRESS,
};
int get_boot_sel(void);
#endif

/*
 * Cache ops
 */
#ifndef __ASSEMBLY__
void flush_dcache_all(void);
void flush_icache_all(void);
void flush_cache_all(void);
#endif

#endif /* COMMON_H */
