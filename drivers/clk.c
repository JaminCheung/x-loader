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

#include <common.h>

struct cgu {
    unsigned en:8;
    unsigned off:8;
    unsigned sel_bit:8;
    unsigned sel_src:8;
    unsigned char sel[4];
    unsigned ce;
    unsigned busy;
    unsigned stop;
    unsigned clkgr_bit;
};

struct cgu cgu_clk_sel[CGU_CNT] = {
    [DDR]    = {1, CPM_DDRCDR,  30, CONFIG_DDR_SEL_PLL, { 0,     APLL,  MPLL,  -1  }, 29, 28, 27, 31},
    [MACPHY] = {0, CPM_MACCDR,  31, CONFIG_DDR_SEL_PLL, { APLL,  MPLL,  -1,    -1  }, 29, 28, 27, 25},

#if (defined CONFIG_BOOT_MMC || defined CONFIG_BURN_MMC)
    [MSC0]   = {1, CPM_MSC0CDR, 31, CONFIG_DDR_SEL_PLL, { APLL,  MPLL,  -1,    -1  }, 29, 28, 27, 4 },
#else
    [MSC0]   = {0, CPM_MSC0CDR, 31, CONFIG_DDR_SEL_PLL, { APLL,  MPLL,  -1,    -1  }, 29, 28, 27, 4 },
#endif /* CONFIG_BOOT_MMC || CONFIG_BURN_MMC */

#ifndef CONFIG_BOOT_USB
    [OTG]    = {0, CPM_USBCDR,  30, EXCLK,              { EXCLK, EXCLK, APLL,  MPLL}, 29, 28, 27, 3 },
#else
    [OTG]    = {1, CPM_USBCDR,  30, EXCLK,              { EXCLK, EXCLK, APLL,  MPLL}, 29, 28, 27, 3 },
#endif /* CONFIG_BOOT_USB */

    [I2S]    = {0, CPM_I2SCDR,  30, EXCLK,              { EXCLK, APLL,  EXCLK, MPLL  }, 29, 0,  0,  -1},
    [LCD]    = {0, CPM_LPCDR,   31, CONFIG_DDR_SEL_PLL, { APLL,  MPLL,  -1,    -1  }, 28, 27, 26, 23},

#if (defined CONFIG_MMC_PC_4BIT || defined CONFIG_BURN_MMC)
    [MSC1]   = {1, CPM_MSC1CDR, 0,  0,                  { -1,    -1,    -1,    -1  }, 29, 28, 27, 5 },
#else
    [MSC1]   = {0, CPM_MSC1CDR, 0,  0,                  { -1,    -1,    -1,    -1  }, 29, 28, 27, 5 },
#endif /* CONFIG_MMC_PC_4BIT || CONFIG_BURN_MMC */

#if (defined CONFIG_BOOT_SFC || defined CONFIG_BURN_SPI_FLASH)
    [SFC]    = {1, CPM_SSICDR,  30, CONFIG_DDR_SEL_PLL, { EXCLK, APLL,  EXCLK,  MPLL}, 29, 28, 27, 2 },
#else
    [SFC]    = {0, CPM_SSICDR,  30, CONFIG_DDR_SEL_PLL, { EXCLK, APLL,  EXCLK,  MPLL}, 29, 28, 27, 2 },
#endif /* CONFIG_BOOT_SFC || CONFIG_BURN_SPI_FLASH */

    [CIM]    = {0, CPM_CIMCDR,  31, CONFIG_DDR_SEL_PLL, { APLL,  MPLL,  -1,    -1  }, 29, 28, 27, 22},
    [PCM]    = {0, CPM_PCMCDR,  30, EXCLK,              { APLL,  EXCLK, MPLL,  -1  }, 29, 0,  0,  26},
};

static void dump_clk_regs(void) {
    debug("====================\n");
    debug("CPAPCR = 0x%x\n", cpm_inl(CPM_CPAPCR));
    debug("CPMPCR = 0x%x\n", cpm_inl(CPM_CPMPCR));
    debug("CPCCR  = 0x%x\n", cpm_inl(CPM_CPCCR));
    debug("CLKGR  = 0x%x\n", cpm_inl(CPM_CLKGR));
    debug("====================\n");
}

static void stop_clk(void) {
    uint8_t id;
    struct cgu *cgu = NULL;
    uint32_t regval = 0, reg = 0;

    for (id = 0; id < CGU_CNT; id++) {
        cgu = &(cgu_clk_sel[id]);
        reg = CPM_BASE + cgu->off;

        if(id == I2S)
            goto dis_aic;

        /*
         * Disable clk gate
         */
        if (cgu->clkgr_bit != -1)
            cpm_outl(cpm_inl(CPM_CLKGR) | (1 << cgu->clkgr_bit), CPM_CLKGR);

        if (id == PCM)
            goto dis_aic;

        if (id != OTG) {
            regval = readl(reg);

            /*
             * Set div max
             */
            regval |= 0xfe | (1 << cgu->ce);
            while (readl(reg) & (1 << cgu->busy));
            writel(regval, reg);
        }

        /*
         * Stop clk
         */
        while (readl(reg) & (1 << cgu->busy));
        regval = readl(reg);
        regval |= ((1 << cgu->stop) | (1 << cgu->ce));
        writel(regval, reg);
        while (readl(reg) & (1 << cgu->busy));

dis_aic:
        /*
         * Clear ce
         */
        regval = readl(reg);
        regval &= ~(1 << cgu->ce);
        writel(regval, reg);
    }
}

static void start_clk(void) {
    uint8_t id;
    struct cgu *cgu = NULL;
    uint32_t regval = 0, reg = 0;

    for (id = 0; id < ARRAY_SIZE(cgu_clk_sel); id++) {
        cgu = &(cgu_clk_sel[id]);
        reg = CPM_BASE + cgu->off;

        for (int j = 0; j < 4; j++) {

            if (cgu->sel_src == cgu->sel[j]) {

                if (id == I2S)
                    break;

                if (cgu->en == 1)
                    cpm_outl(cpm_inl(CPM_CLKGR) & ~(1 << cgu->clkgr_bit), CPM_CLKGR);

                if (id == PCM)
                    break;

                while (readl(reg) & (1 << cgu->busy));
                regval = readl(reg);
                regval &= ~(3 << 30);
                regval |= (j << cgu->sel_bit) | (1 << cgu->ce) | (!cgu->en << cgu->stop);
                writel(regval, reg);
                while (readl(reg) & (1 << cgu->busy));

                break;
            }
        }
    }

    /*
     * Enable PDMA clk
     */
    reg = cpm_inl(CPM_CLKGR);
    reg &= ~(0x1 << 21);
    cpm_outl(reg, CPM_CLKGR);

    /*
     * Enable DDRC clk
     */
    uint32_t div = CONFIG_DDR_FREQ_DIV - 1;
    reg = cpm_inl(CPM_DDRCDR);
    reg &= ~(0xf | (0x3f << 24));
    reg |= (1 << 29) | div;
    cpm_outl(reg, CPM_DDRCDR);
    while (cpm_inl(CPM_DDRCDR) & (1 << 28));
}

static void pll_init(void) {
    uint8_t n, m, od, bs;
    uint32_t regval = 0;
    uint32_t timeout = 0;

    if (CONFIG_APLL_FREQ > 0) {
        /*
         * Configure APLL
         */
        bs = 1;
        n = 0;
        m = CONFIG_APLL_FREQ / CONFIG_EXTAL_FREQ - 1;
        od = 0;

        regval |= (1 << CPM_CPAPCR_PLLEN_BIT) |
                  (0x20 << CPM_CPAPCR_PLLST_BIT) |
                  (bs << CPM_CPAPCR_BS_BIT) |
                  (m << CPM_CPAPCR_PLLM_BIT) |
                  (n << CPM_CPAPCR_PLLN_BIT) |
                  (od << CPM_CPAPCR_PLLOD_BIT);
        cpm_outl(regval, CPM_CPAPCR);

        /*
         * Wait APLL stable
         */
        timeout = 0x10000;
        while(!(cpm_inl(CPM_CPAPCR) & (1 << CPM_CPAPCR_PLLON_BIT)) && --timeout) {
            if (timeout == 0)
                hang_reason("\n\tAPLL init timeout.\n");
        }
    }

    if (CONFIG_MPLL_FREQ > 0) {
        /*
         * Configure MPLL
         */
        bs = 1;
        n = 0;
        m = CONFIG_MPLL_FREQ / CONFIG_EXTAL_FREQ - 1;
        od = 0;
        regval = 0;
        regval |= (1 << CPM_CPMPCR_PLLEN_BIT) |
                (bs << CPM_CPMPCR_BS_BIT) |
                (m << CPM_CPMPCR_PLLM_BIT) |
                (n << CPM_CPMPCR_PLLN_BIT) |
                (od << CPM_CPMPCR_PLLOD_BIT);
        cpm_outl(regval, CPM_CPMPCR);

        /*
         * Wait MPLL stable
         */
        timeout = 0x10000;
        while(!(cpm_inl(CPM_CPMPCR) & (1 << CPM_CPMPCR_PLLON_BIT)) && --timeout) {
            if (timeout == 0)
                hang_reason("\n\tMPLL init timeout.\n");
        }
    }
}

static void clk_tree_init(void) {
    uint32_t cpccr = 0,regval = 0;

    uint8_t cdiv, l2div, h0div, h2div, pdiv;
    uint8_t src_sel, cpu_clk_sel, h0_clk_sel, h2_clk_sel;

    src_sel = 0x2; // APLL
    cpu_clk_sel = (CONFIG_CPU_SEL_PLL == APLL) ? 0x1 : 0x2;
    h0_clk_sel = (CONFIG_DDR_SEL_PLL == APLL) ? 0x1 : 0x2;
    h2_clk_sel = h0_clk_sel;

    cdiv = 1;
    l2div = CONFIG_L2CACHE_CLK_DIV;
    h0div = CONFIG_AHB_CLK_DIV;
    h2div = h0div;
    pdiv = h2div * 2;

    cpccr = (src_sel << 30) |
            (cpu_clk_sel << 28) |
            (h0_clk_sel << 26) |
            (h2_clk_sel << 24) |
            (pdiv - 1) << 16 |
            (h2div - 1) << 12 |
            (h0div - 1) << 8 |
            (l2div - 1) << 4 |
            (cdiv - 1) << 0;

    /*
     * Change div
     */
    regval = (cpm_inl(CPM_CPCCR) & (0xff << 24)) | (cpccr & ~(0xff << 24)) | (7 << 20);
    cpm_outl(regval, CPM_CPCCR);
    while(cpm_inl(CPM_CPCSR) & 0x7);

    /*
     * Change sel
     */
    regval = (cpccr & (0xff << 24)) | (cpm_inl(CPM_CPCCR) & ~(0xff << 24));
    cpm_outl(regval, CPM_CPCCR);
    while(!(cpm_inl(CPM_CPCSR) & 0xf0000000));

    cpu_freq = CONFIG_CPU_SEL_PLL == APLL ? CONFIG_APLL_FREQ : CONFIG_MPLL_FREQ;
}

#ifdef CONFIG_BOOT_USB
static void reset_clk_tree(void) {
    uint32_t cpccr = 0x55000000;
    uint32_t regval = 0;

    cpccr |= ((1 - 1) << 16)  /* PDIV                   */
          | ((1 - 1) << 12)   /* H2DIV                  */
          | ((1 - 1) << 8)    /* H0DIV                  */
          | ((1 - 1) << 4)    /* L2DIV                  */
          | (1 - 1);          /* CDIV                   */


    /*
     * Change sel
     */
    regval = (cpccr & (0xff << 24)) | (cpm_inl(CPM_CPCCR) & ~(0xff << 24));
    cpm_outl(regval, CPM_CPCCR);
    while(!(cpm_inl(CPM_CPCSR) & 0xf0000000));

    /*
     * Change div
     */
    regval = (cpm_inl(CPM_CPCCR) & (0xff << 24)) | (cpccr & ~(0xff << 24)) | (7 << 20);
    cpm_outl(regval, CPM_CPCCR);
    while(cpm_inl(CPM_CPCSR) & 0x7);

    cpu_freq = CONFIG_EXTAL_FREQ;
}
#endif

void clk_init(void) {
#ifdef CONFIG_BOOT_USB
    reset_clk_tree();
#endif

    /*
     * Stop all clock
     */
    stop_clk();

    /*
     * Configure PLL
     */
    pll_init();

    /*
     * Configure clock tree
     */
    clk_tree_init();

    /*
     * Start all clock
     */
    start_clk();

    dump_clk_regs();
}

void enable_uart_clk(void) {
    uint32_t clkgr;

    clkgr = cpm_inl(CPM_CLKGR);

#if CONFIG_CONSOLE_INDEX == 0
    clkgr &= ~CPM_CLKGR_UART0;

#elif CONFIG_CONSOLE_INDEX == 1
    clkgr &= ~CPM_CLKGR_UART1;

#elif CONFIG_CONSOLE_INDEX == 2
    clkgr &= ~CPM_CLKGR_UART2;

#else
#error Unknown console index!
#endif

    cpm_outl(clkgr, CPM_CLKGR);
}

void enable_aes_clk(void) {
    uint32_t clkgr;

    clkgr = cpm_inl(CPM_CLKGR);
    clkgr &= ~CPM_CLKGR_AES;
    cpm_outl(clkgr, CPM_CLKGR);
}

uint32_t get_ahb_rate(void) {
    uint32_t freq = CONFIG_DDR_SEL_PLL == APLL ? CONFIG_APLL_FREQ / CONFIG_AHB_CLK_DIV
            : CONFIG_MPLL_FREQ / CONFIG_AHB_CLK_DIV;

    return freq * 1000 * 1000;
}

uint32_t get_ddr_rate(void) {
    uint32_t freq = CONFIG_DDR_SEL_PLL == APLL ? CONFIG_APLL_FREQ / CONFIG_DDR_FREQ_DIV
            : CONFIG_MPLL_FREQ / CONFIG_DDR_FREQ_DIV;

    return freq * 1000 * 1000;
}

uint32_t get_mmc_freq(void) {
    uint32_t msc_cdr_reg;
    uint32_t div;
    uint32_t freq;
    uint32_t pll_rate = CONFIG_DDR_SEL_PLL == APLL ? CONFIG_APLL_FREQ : CONFIG_MPLL_FREQ;

#if (defined CONFIG_MMC_PA_8BIT) || (defined CONFIG_MMC_PA_4BIT)
    msc_cdr_reg = CPM_MSC0CDR;
#else
    msc_cdr_reg = CPM_MSC1CDR;
#endif

    div = ((cpm_inl(msc_cdr_reg) & 0xff) + 1) * 2;

    freq = (pll_rate * 1000 * 1000) / div;

    return freq;
}

void set_mmc_freq(uint32_t freq) {
    uint32_t div;
    uint32_t msc_cdr_reg;
    uint32_t pll_rate = CONFIG_DDR_SEL_PLL == APLL ? CONFIG_APLL_FREQ : CONFIG_MPLL_FREQ;

    div = ((pll_rate * 1000 * 1000 + freq - 1) / freq / 2 - 1) & 0xff;

#if (defined CONFIG_MMC_PA_8BIT) || (defined CONFIG_MMC_PA_4BIT)
    msc_cdr_reg = CPM_MSC0CDR;
#else
    msc_cdr_reg = CPM_MSC1CDR;
#endif

    uint32_t reg = cpm_inl(msc_cdr_reg);
    reg = cpm_inl(msc_cdr_reg);
    reg &= ~((0x3 << 27) | 0xff);
    reg |= (1 << 29) | div;

    cpm_outl(reg, msc_cdr_reg);
    while (cpm_inl(msc_cdr_reg) & (1 << 28));
}

void set_sfc_freq(uint32_t freq) {
    uint32_t div;
    uint32_t pll_rate = CONFIG_DDR_SEL_PLL == APLL ? CONFIG_APLL_FREQ : CONFIG_MPLL_FREQ;

    div = ((pll_rate * 1000 * 1000 + freq - 1) / freq - 1) & 0xff;

    uint32_t reg = cpm_inl(CPM_SSICDR);
    reg = cpm_inl(CPM_SSICDR);
    reg &= ~((0x3 << 27) | 0xff);
    reg |= (1 << 29) | div;

    cpm_outl(reg, CPM_SSICDR);
    while (cpm_inl(CPM_SSICDR) & (1 << 28));
}
