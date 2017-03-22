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

uint32_t cpu_freq = CONFIG_APLL_FREQ;

static void dump_register(void)
{
    debug("=====================\n");
    debug("Dump CPM:\n");
    debug("=====================\n");
    debug("CPM_LCR     = 0x%x\n", cpm_inl(CPM_LCR));
    debug("CPM_CLKGR   = 0x%x\n", cpm_inl(CPM_CLKGR));
    debug("CPM_OPCR    = 0x%x\n", cpm_inl(CPM_OPCR));
    debug("=====================\n");
}

static void wait_pll_stable(void)
{
    /*
     * Wait APLL
     */
    while(!(cpm_inl(CPM_CPAPCR) & (0x1 << 10)))
        continue;

    debug("CPM_CPAPCR = 0x%x\n", cpm_inl(CPM_CPAPCR));

    /*
     * Wait MPLL
     */
    while(!(cpm_inl(CPM_CPMPCR) & (0x1 << 0)))
        continue;

    debug("CPM_CPMPCR = 0x%x\n", cpm_inl(CPM_CPMPCR));
}

void sleep_lib_reset_clk_tree(void)
{
    unsigned int reg;
    unsigned int cpccr_cfg = 0x55000000;

    cpccr_cfg |= ((1 - 1) << 16)    /* PDIV                   */
                | ((1 - 1) << 12)   /* H2DIV                  */
                | ((1 - 1) << 8)    /* H0DIV                  */
                | ((1 - 1) << 4)    /* L2DIV                  */
                | (1 - 1);          /* CDIV                   */

    /* Change sel */
    reg = (cpccr_cfg & (0xff << 24)) | (cpm_inl(CPM_CPCCR) & ~(0xff << 24));
    cpm_outl(reg, CPM_CPCCR);

    /* Change div */
    reg = (cpm_inl(CPM_CPCCR) & (0xff << 24))
                                        | (cpccr_cfg & ~(0xff << 24))
                                        | (7 << 20);
    cpm_outl(reg, CPM_CPCCR);
    while(cpm_inl(CPM_CPCSR) & 0x7)
        continue;
    /*
     * xxdelay can use now
     */
    cpu_freq = 24;
}

static void init_clk_tree(void)
{
    unsigned int pdiv, h0div, h2div, l2div, cdiv;

    l2div = CONFIG_L2CACHE_CLK_DIV;
    h0div = CONFIG_AHB_CLK_DIV;
    h2div = h0div;

    pdiv = h2div * 2;

    cdiv = 1;

    uint8_t soc_clk_sel = CONFIG_DDR_SEL_PLL == MPLL ? 0x2 : 0x1;
    uint8_t cpu_clk_sel = CONFIG_CPU_SEL_PLL == MPLL ? 0x2 : 0x1;

    unsigned int cpccr_cfg = (0x2 << 30)            /* SCLK_A: APLL           */
                    | (cpu_clk_sel << 28)           /* Core & L2: cpu_clk_sel */
                    | (soc_clk_sel << 26)           /* H0: soc_clk_sel        */
                    | (soc_clk_sel << 24)           /* H2: soc_clk_sel        */
                    | ((pdiv - 1) << 16)            /* PDIV                   */
                    | ((h2div - 1) << 12)           /* H2DIV                  */
                    | ((h0div - 1) << 8)            /* H0DIV                  */
                    | ((l2div - 1) << 4)            /* L2DIV                  */
                    | (cdiv - 1)                    /* CDIV                   */
                    | (0x7 << 20);

    /* Change div */
    unsigned int reg = (cpm_inl(CPM_CPCCR) & (0xff << 24))
                                        | (cpccr_cfg & ~(0xff << 24))
                                        | (7 << 20);
    cpm_outl(reg, CPM_CPCCR);
    while(cpm_inl(CPM_CPCSR) & 0x7)
        continue;

    /* Change sel */
    reg = (cpccr_cfg & (0xff << 24)) | (cpm_inl(CPM_CPCCR) & ~(0xff << 24));
    cpm_outl(reg, CPM_CPCCR);

    debug("CPM_CPCCR = 0x%x\n",cpm_inl(CPM_CPCCR));
}

void sleep_lib_init_clk(void)
{
    dump_register();

    wait_pll_stable();
    init_clk_tree();
}

