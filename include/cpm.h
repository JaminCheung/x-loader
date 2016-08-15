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

#ifndef CPM_H
#define CPM_H

#define CPM_CPCCR   (0x00)
#define CPM_CPCSR   (0xd4)

#define CPM_DDRCDR  (0x2c)
#define CPM_VPUCDR  (0x30)
#define CPM_CPSPR   (0x34)
#define CPM_CPSPPR  (0x38)
#define CPM_USBPCR  (0x3c)
#define CPM_USBRDT  (0x40)
#define CPM_USBVBFIL    (0x44)
#define CPM_USBPCR1 (0x48)
#define CPM_USBCDR  (0x50)
#define CPM_MACCDR  (0x54)
#define CPM_I2SCDR  (0x60)
#define CPM_LPCDR   (0x64)
#define CPM_MSC0CDR (0x68)
#define CPM_MSC1CDR (0xa4)
#define CPM_MSC2CDR (0xa8)
#define CPM_UHCCDR  (0x6c)
#define CPM_SSICDR  (0x74)
#define CPM_CIMCDR  (0x7c)
#define CPM_CIM1CDR (0x80)
#define CPM_PCMCDR  (0x84)
#define CPM_BCHCDR  (0xac)

#define CPM_MPHYCR  (0xe8)

#define CPM_CPAPCR  (0x10)
#define CPM_CPMPCR  (0x14)

#define CPM_INTR    (0xb0)
#define CPM_INTRE   (0xb4)
#define CPM_CPSPPR  (0x38)
#define CPM_CPPSR   (0x34)

#define CPM_USBPCR  (0x3c)
#define CPM_USBRDT  (0x40)
#define CPM_USBVBFIL    (0x44)
#define CPM_USBPCR1 (0x48)

#define CPM_LCR     (0x04)
#define CPM_PSWC0ST     (0x90)
#define CPM_PSWC1ST     (0x94)
#define CPM_PSWC2ST     (0x98)
#define CPM_PSWC3ST     (0x9c)
#define CPM_CLKGR   (0x20)
#define CPM_SRBC    (0xc4)
#define CPM_SLBC    (0xc8)
#define CPM_SLPC    (0xcc)
#define CPM_OPCR    (0x24)

#define CPM_RSR     (0x08)

#define LCR_LPM_MASK        (0x3)
#define LCR_LPM_SLEEP       (0x1)

#define CPM_CPAPCR_BS_BIT       (31)
#define CPM_CPAPCR_PLLM_BIT     (24)
#define CPM_CPAPCR_PLLN_BIT     (18)
#define CPM_CPAPCR_PLLOD_BIT    (16)
#define CPM_CPAPCR_PLLLOCK_BIT  (15)
#define CPM_CPAPCR_PLLON_BIT    (10)
#define CPM_CPAPCR_PLLBP_BIT    (9)
#define CPM_CPAPCR_PLLEN_BIT    (8)
#define CPM_CPAPCR_PLLST_BIT    (0)

#define CPM_CPMPCR_BS_BIT       (31)
#define CPM_CPMPCR_PLLM_BIT     (24)
#define CPM_CPMPCR_PLLN_BIT     (18)
#define CPM_CPMPCR_PLLOD_BIT    (16)
#define CPM_CPMPCR_PLLEN_BIT    (7)
#define CPM_CPMPCR_PLLBP_BIT    (6)
#define CPM_CPMPCR_PLLLOCK_BIT  (1)
#define CPM_CPMPCR_PLLON_BIT    (0)

#define CPM_LCR_PD_X2D      (0x1<<31)
#define CPM_LCR_PD_VPU      (0x1<<30)
#define CPM_LCR_PD_IPU      (0x1<<29)
#define CPM_LCR_PD_EPD      (0x1<<28)
#define CPM_LCR_PD_MASK     (0x7<<28)
#define CPM_LCR_X2DS        (0x1<<27)
#define CPM_LCR_VPUS        (0x1<<26)
#define CPM_LCR_IPUS        (0x1<<25)
#define CPM_LCR_EPDS        (0x1<<24)
#define CPM_LCR_STATUS_MASK     (0xf<<24)

#define CPM_CLKGR_DDR       (1 << 31)
#define CPM_CLKGR_CPU       (1 << 30)
#define CPM_CLKGR_AHB0      (1 << 29)
#define CPM_CLKGR_APB0      (1 << 28)
#define CPM_CLKGR_RTC       (1 << 27)
#define CPM_CLKGR_PCM       (1 << 26)
#define CPM_CLKGR_MAC       (1 << 25)
#define CPM_CLKGR_AES       (1 << 24)
#define CPM_CLKGR_LCD       (1 << 23)
#define CPM_CLKGR_CIM       (1 << 22)
#define CPM_CLKGR_PDMA      (1 << 21)
#define CPM_CLKGR_OST       (1 << 20)
#define CPM_CLKGR_SSI       (1 << 19)
#define CPM_CLKGR_TCU       (1 << 18)
#define CPM_CLKGR_DMIC      (1 << 17)
#define CPM_CLKGR_UART2     (1 << 16)
#define CPM_CLKGR_UART1     (1 << 15)
#define CPM_CLKGR_UART0     (1 << 14)
#define CPM_CLKGR_SADC      (1 << 13)
#define CPM_CLKGR_JPEG      (1 << 12)
#define CPM_CLKGR_AIC       (1 << 11)
#define CPM_CLKGR_I2C3          (1 << 10)
#define CPM_CLKGR_I2C2      (1 << 9)
#define CPM_CLKGR_I2C1      (1 << 8)
#define CPM_CLKGR_I2C0      (1 << 7)
#define CPM_CLKGR_SCC       (1 << 6)
#define CPM_CLKGR_MSC1      (1 << 5)
#define CPM_CLKGR_MSC0      (1 << 4)
#define CPM_CLKGR_OTG       (1 << 3)
#define CPM_CLKGR_SFC       (1 << 2)
#define CPM_CLKGR_EFUSE     (1 << 1)
#define CPM_CLKGR_NEMC      (1 << 0)

#define CPM_RSR_HR      (1 << 3)
#define CPM_RSR_P0R     (1 << 2)
#define CPM_RSR_WR      (1 << 1)
#define CPM_RSR_PR      (1 << 0)

#define OPCR_ERCS       (0x1<<2)
#define OPCR_PD         (0x1<<3)
#define OPCR_IDLE       (0x1<<31)

#define CLKGR_VPU              (0x1<<19)

#define cpm_inl(off)        readl(CPM_BASE + (off))
#define cpm_outl(val,off)   writel(val, CPM_BASE + (off))
#define cpm_test_bit(bit,off)   (cpm_inl(off) & 0x1<<(bit))
#define cpm_set_bit(bit,off)    (cpm_outl((cpm_inl(off) | 0x1<<(bit)),off))
#define cpm_clear_bit(bit,off)  (cpm_outl(cpm_inl(off) & ~(0x1 << bit), off))

/*USBCDR*/
#define USBCDR_UCS_PLL      (1 << 31)
#define USBCDR_UPCS_MPLL    (1 << 30)
#define USBCDR_CE_USB       (1 << 29)
#define USBCDR_USB_BUSY     (1 << 28)
#define USBCDR_USB_STOP     (1 << 27)
#define USBCDR_USBCDR_MSK   (0xff)

/*USBPCR*/
#define USBPCR_USB_MODE_ORG (1 << 31)
#define USBPCR_VBUSVLDEXT   (1 << 24)
#define USBPCR_VBUSVLDEXTSEL    (1 << 23)
#define USBPCR_POR      (1 << 22)
#define USBPCR_OTG_DISABLE  (1 << 20)

/*USBPCR1*/
#define USBPCR1_REFCLKSEL_BIT   (26)
#define USBPCR1_REFCLKSEL_MSK   (0x3 << USBPCR1_REFCLKSEL_BIT)
#define USBPCR1_REFCLKSEL_CORE  (0x2 << USBPCR1_REFCLKSEL_BIT)
#define USBPCR1_REFCLKSEL_EXT   (0x1 << USBPCR1_REFCLKSEL_BIT)
#define USBPCR1_REFCLKSEL_CSL   (0x0 << USBPCR1_REFCLKSEL_BIT)
#define USBPCR1_REFCLKDIV_BIT   (24)
#define USBPCR1_REFCLKDIV_MSK   (0X3 << USBPCR1_REFCLKDIV_BIT)
#define USBPCR1_REFCLKDIV_19_2M (0x3 << USBPCR1_REFCLKDIV_BIT)
#define USBPCR1_REFCLKDIV_48M   (0x2 << USBPCR1_REFCLKDIV_BIT)
#define USBPCR1_REFCLKDIV_24M   (0x1 << USBPCR1_REFCLKDIV_BIT)
#define USBPCR1_REFCLKDIV_12M   (0x0 << USBPCR1_REFCLKDIV_BIT)
#define USBPCR1_WORD_IF_16_30   (1 << 19)

/*OPCR*/
#define OPCR_SPENDN     (1 << 7)

/* CPM scratch pad protected register(CPSPPR) */
#define CPSPPR_CPSPR_WRITABLE   (0x00005a5a)
#define RECOVERY_SIGNATURE      (0x1a1a)        /* means "RECY" */
#define RECOVERY_SIGNATURE_SEC  (0x800)         /* means "RECY" */
#define FASTBOOT_SIGNATURE      (0x0666)        /* means "FASTBOOT" */

#define cpm_get_scrpad()        readl(CPM_BASE + CPM_CPSPR)
#define cpm_set_scrpad(data)                    \
do {                                            \
    volatile int i = 0x3fff;                \
    writel(0x00005a5a,CPM_BASE + CPM_CPSPPR);       \
    while(i--);             \
    writel(data,CPM_BASE + CPM_CPSPR);          \
    writel(0x0000a5a5,CPM_BASE + CPM_CPSPPR);       \
} while (0)

#endif
