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

extern uint8_t sleep_lib_entry[];

static struct sleep_context sleep_context;
struct sleep_context* context = &sleep_context;

void dump_sleep_context(void) {
    uint8_t i;

    debug("========== sleep context ==========\n");
    for (i = 0; i < 8; i++)
        debug("gpr_s%d       = 0x%x\n", i, context->gpr_s[i]);
    debug("gpr_gp       = 0x%x\n", context->gpr_gp);
    debug("gpr_sp       = 0x%x\n", context->gpr_sp);
    debug("gpr_fp       = 0x%x\n", context->gpr_fp);
    debug("gpr_ra       = 0x%x\n", context->gpr_ra);
    debug("cp0_pagemask = 0x%x\n", context->cp0_pagemask);
    debug("cp0_tlb_spec = 0x%x\n", context->cp0_tlb_spec);
    debug("cp0_status   = 0x%x\n", context->cp0_status);
    debug("cp0_intctl   = 0x%x\n", context->cp0_intctl);
    debug("cp0_cause    = 0x%x\n", context->cp0_cause);
    debug("cp0_ebase    = 0x%x\n", context->cp0_ebase);
    debug("cp0_config   = 0x%x\n", context->cp0_config);
    debug("cp0_config1  = 0x%x\n", context->cp0_config1);
    debug("cp0_config2  = 0x%x\n", context->cp0_config2);
    debug("cp0_config3  = 0x%x\n", context->cp0_config3);
    debug("cp0_config7  = 0x%x\n", context->cp0_config7);
    debug("cp0_lladdr   = 0x%x\n", context->cp0_lladdr);
    debug("pmon_csr     = 0x%x\n", context->pmon_csr);
    debug("pmon_high    = 0x%x\n", context->pmon_high);
    debug("pmon_lc      = 0x%x\n", context->pmon_lc);
    debug("pmon_rc      = 0x%x\n", context->pmon_rc);
    debug("cp0_watchlo  = 0x%x\n", context->cp0_watchlo);
    debug("cp0_watchhi  = 0x%x\n", context->cp0_watchhi);
    debug("cpm_lcr      = 0x%x\n", context->cpm_lcr);
    debug("cpm_opcr     = 0x%x\n", context->cpm_opcr);
    debug("cpm_clkgr    = 0x%x\n", context->cpm_clkgr);
    debug("cpm_usbpcr   = 0x%x\n", context->cpm_usbpcr);
    debug("===================================\n");
}

void restore_context(void)
{
    extern void restore_context_goto(void);
    restore_context_goto();
}

static unsigned int cpm_barrier(void)
{
    volatile unsigned int temp;
    temp = cpm_inl(CPM_OPCR);

    return temp;
}

static void resume(void)
{
    /*
     * Restore clock
     */
    sleep_lib_init_clk();

    /*
     * Enable all clock
     */
    cpm_outl(0, CPM_CLKGR);
    udelay(1);

    debug("Wakeup Step 1 done.\n");

    /*
     * Restore lpddr2
     */
#if 0
    restore_lpddr2();
#endif
    debug("Wakeup Step 2 skip.\n");

    /*
     * Restore CPM
     */
    cpm_outl(context->cpm_lcr, CPM_LCR);
    cpm_outl(context->cpm_opcr, CPM_OPCR);
    cpm_outl(context->cpm_usbpcr, CPM_USBPCR);
    cpm_outl(context->cpm_clkgr, CPM_CLKGR);

    cpm_barrier();

    debug("Wakeup Step 3 done.\n");

    cache_init();

    debug("Wakeup Step 4 done.\n");

    dump_sleep_context();

    /*
     * Like nothing happened...
     */
    restore_context();
}

static void resume_goto(void)
{
    __asm__ volatile(
        ".set mips32\n\t"
        "move $29, %0\n\t"
        ".set mips32\n\t"
        :
        :"r" (sleep_lib_entry + SLEEP_LIB_LENGTH - 4)
        :);

    __asm__ volatile(".set mips32\n\t"
             "jr %0\n\t"
             "nop\n\t"
             ".set mips32 \n\t" :: "r" (resume));
}

static void sleep(void)
{
    debug("Prepare sleep configuration...\n");

    /*
     * Step 1.
     ************************
     */
    disable_fpu();

    debug("Sleep step 1 done\n");

    /*
     * Step 2.
     ************************
     * For CPM LCR
     */
    volatile unsigned int temp;

    /*
     * Save open all clock
     */
    context->cpm_clkgr = cpm_inl(CPM_CLKGR);
    cpm_outl(0x0, CPM_CLKGR);
    udelay(1);

    temp = cpm_inl(CPM_LCR);
    context->cpm_lcr = temp;

    /*
     * Enter sleep status
     */
    temp &= ~(3 | (0xfff << 8));
    temp |= 0xf << 8;
    temp |= 0x1;
    cpm_outl(temp, CPM_LCR);

    /*
     * For CPM USBPCR
     */
    context->cpm_usbpcr = cpm_inl(CPM_USBPCR);
    temp = context->cpm_usbpcr;
    temp |= (1 << 25) | (1 << 20);
    cpm_outl(temp, CPM_USBPCR);

    debug("Sleep step 2 done\n");

    /*
     * Step 3.
     ************************
     * For CPM OPCR and Core resume address
     */
    temp = cpm_inl(CPM_OPCR);
    context->cpm_opcr = temp;

    /*
     * Disable otg, uhc, extoscillator
     */
    temp &= ~((1 << 7) | (1 << 6) | (1 << 4) | (1 << 22));

    /*
     * extoscillator state wait time,
     * mask interrupt,
     * use RTC clock,
     * L2 power down
     * RTC clock enable
     * Core power down
     */
    temp |= (0xf << 8) | (1 << 30) | (1 << 2) | (1 << 25)
            | (1 << 23) | (1 << 3);
    cpm_outl(temp, CPM_OPCR);

    /*
     * Set core resume address
     */
    cpm_outl(1, CPM_SLBC);
    cpm_outl((uint32_t)resume_goto, CPM_SLPC);

    debug("Sleep step 3 done\n");

    /*
     * Step 4.
     ************************
     * For CPM CLKGR
     */

    /*
     * CLKGR
     */
    temp = (0x3f << 22)    /* RTC,PCM,MAC,AES,LCD,CIM              */
            | (0x3 << 19)  /* SSI2,SSI1                             */
            | (0x1f << 14) /* TCU,DMIC,UART2~0                               */
            | 0x3ffe;      /* SADC,JPEG,AIC,SMB3~0,SCC,MSC1~0,SFC,EFUSE */
    /*
     * Enable UART2 if DEBUG
     */
#ifdef DEBUG
    temp &= ~(0x1 << 16);                         /* UART2 */
#endif
    cpm_outl(temp, CPM_CLKGR);

    debug("Sleep step 4 done\n");

    /*
     * Step 5.
     ************************
     * Clear CPM RSR
     */
    cpm_outl(0, CPM_RSR);

    debug("Sleep step 5 done\n");

    /*
     * Step 6.
     ************************
     */
#if 0
    prepare_lpddr2_for_sleep();
#endif
    debug("Sleep step 6 skip\n");

    /*
     * Step 7.
     ************************
     */
    sleep_lib_reset_clk_tree();

    /*
     * Here gating clk of APB0
     */
    cpm_outl(cpm_inl(CPM_CLKGR) | (0x1 << 28), CPM_CLKGR);

    debug("Sleep step 7 done\n");

    /*
     * Barriers all all all...
     */
    cpm_barrier();

    debug("Sleep done.\n");

    dump_sleep_context();

    /*
     * The End
     ************************
     * We are going to sleep...
     */
    __asm__ volatile(".set mips32\n\t"
         "wait\n\t"
         "nop\n\t"
         "nop\n\t"
         "nop\n\t"
    );
}

static void sleep_goto(void)
{
    __asm__ volatile(
        ".set mips32\n\t"
        "move $29, %0\n\t"
        ".set mips32\n\t"
        :
        :"r" (sleep_lib_entry + SLEEP_LIB_LENGTH - 4)
        :);

    __asm__ volatile(".set mips32\n\t"
             "jr %0\n\t"
             "nop\n\t"
             ".set mips32 \n\t" :: "r" (sleep));
}

int enter_sleep(int state)
{
    debug("Go to sleep...\n");

    flush_cache_all();

    extern void save_context_goto(void *address);
    save_context_goto(sleep_goto);

#ifdef DEBUG
    static uint32_t sleep_times = 0;
    debug("Sleep back, times: %d\n", ++sleep_times);
#endif

    return 0;
}

