/*
 * DDR driver for Synopsys DWC DDR PHY.
 * Used by Jz4775, JZ4780...
 *
 * Copyright (C) 2013 Ingenic Semiconductor Co.,Ltd
 * Author: Zoro <ykli@ingenic.cn>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <generated/ddr_reg_values.h>

#ifdef CONFIG_DWC_DEBUG
#define dwc_debug(fmt, args...)         \
    do {                    \
        printf(fmt, ##args);        \
    } while (0)
#else
#define dwc_debug(fmt, args...)         \
    do {                    \
    } while (0)
#endif

#define DDR_TYPE_MODE(x)     (((x) >> 1) & 0xf)
#define BYPASS_ENABLE       1
#define BYPASS_DISABLE      0
#define IS_BYPASS_MODE(x)     (((x) & 1) == BYPASS_ENABLE)

static int sdram_resized;
static int ddr_row = DDR_ROW;
static int ddr_col = DDR_COL;

static void dump_ddrc_register(void)
{
#ifdef CONFIG_DWC_DEBUG
    printf("DDRC_STATUS     0x%x\n", ddr_readl(DDRC_STATUS));
    printf("DDRC_CFG        0x%x\n", ddr_readl(DDRC_CFG));
    printf("DDRC_CTRL       0x%x\n", ddr_readl(DDRC_CTRL));
    printf("DDRC_LMR        0x%x\n", ddr_readl(DDRC_LMR));
    printf("DDRC_TIMING1        0x%x\n", ddr_readl(DDRC_TIMING(1)));
    printf("DDRC_TIMING2        0x%x\n", ddr_readl(DDRC_TIMING(2)));
    printf("DDRC_TIMING3        0x%x\n", ddr_readl(DDRC_TIMING(3)));
    printf("DDRC_TIMING4        0x%x\n", ddr_readl(DDRC_TIMING(4)));
    printf("DDRC_TIMING5        0x%x\n", ddr_readl(DDRC_TIMING(5)));
    printf("DDRC_TIMING6        0x%x\n", ddr_readl(DDRC_TIMING(6)));
    printf("DDRC_REFCNT     0x%x\n", ddr_readl(DDRC_REFCNT));
    printf("DDRC_MMAP0      0x%x\n", ddr_readl(DDRC_MMAP0));
    printf("DDRC_MMAP1      0x%x\n", ddr_readl(DDRC_MMAP1));
    printf("DDRC_REMAP1     0x%x\n", ddr_readl(DDRC_REMAP(1)));
    printf("DDRC_REMAP2     0x%x\n", ddr_readl(DDRC_REMAP(2)));
    printf("DDRC_REMAP3     0x%x\n", ddr_readl(DDRC_REMAP(3)));
    printf("DDRC_REMAP4     0x%x\n", ddr_readl(DDRC_REMAP(4)));
    printf("DDRC_REMAP5     0x%x\n", ddr_readl(DDRC_REMAP(5)));
#endif
}

static void dump_ddrp_register(void)
{
#ifdef CONFIG_DWC_DEBUG
    printf("DDRP_PIR        0x%x\n", ddr_readl(DDRP_PIR));
    printf("DDRP_PGCR       0x%x\n", ddr_readl(DDRP_PGCR));
    printf("DDRP_PGSR       0x%x\n", ddr_readl(DDRP_PGSR));
    printf("DDRP_PTR0       0x%x\n", ddr_readl(DDRP_PTR0));
    printf("DDRP_PTR1       0x%x\n", ddr_readl(DDRP_PTR1));
    printf("DDRP_PTR2       0x%x\n", ddr_readl(DDRP_PTR2));
    printf("DDRP_DCR        0x%x\n", ddr_readl(DDRP_DCR));
    printf("DDRP_DTPR0      0x%x\n", ddr_readl(DDRP_DTPR0));
    printf("DDRP_DTPR1      0x%x\n", ddr_readl(DDRP_DTPR1));
    printf("DDRP_DTPR2      0x%x\n", ddr_readl(DDRP_DTPR2));
    printf("DDRP_MR0        0x%x\n", ddr_readl(DDRP_MR0));
    printf("DDRP_MR1        0x%x\n", ddr_readl(DDRP_MR1));
    printf("DDRP_MR2        0x%x\n", ddr_readl(DDRP_MR2));
    printf("DDRP_MR3        0x%x\n", ddr_readl(DDRP_MR3));
    printf("DDRP_ODTCR      0x%x\n", ddr_readl(DDRP_ODTCR));
    int i=0;
    for(i=0;i<4;i++) {
        printf("DX%dGSR0: %x\n", i, ddr_readl(DDRP_DXGSR0(i)));
        printf("@pas:DXDQSTR(%d)= 0x%x\n", i,ddr_readl(DDRP_DXDQSTR(i)));
    }
#endif
}

#ifdef CONFIG_PROBE_MEM_SIZE
static void sdram_resize_to_32m(void) {
    uint32_t ddrc_cfg = 0;

    ddrc_cfg = ddr_readl(DDRC_CFG);

    ddrc_cfg &= ~((0x3f << 24) | (0x3f << 8));
    ddrc_cfg |= ((0x9 << 24) | (0x09 << 8));

    ddr_writel(ddrc_cfg, DDRC_CFG);
    ddr_writel(0x000020fe, DDRC_MMAP0);
    ddr_writel(0x00002200, DDRC_MMAP1);

    dwc_debug("DDRC_CFG    0x%x\n", ddr_readl(DDRC_CFG));
    dwc_debug("DDRC_MMAP0  0x%x\n", ddr_readl(DDRC_MMAP0));
    dwc_debug("DDRC_MMAP1  0x%x\n", ddr_readl(DDRC_MMAP1));

    ddr_row = DDR_ROW;
    ddr_col = DDR_COL - 1;

    sdram_resized = 1;
}

static void probe_sdram_size(void) {
    uint32_t addr = CKSEG1ADDR(0);
    uint32_t mirror_addr = CKSEG1ADDR(1 << DDR_COL);

    writew(0x5555, mirror_addr);

    if (readw(addr) == readw(mirror_addr)) {
        writew(0xaaaa, addr);

        if (readw(addr) == readw(mirror_addr))
            sdram_resize_to_32m();
    }
}
#endif

static void reset_dll(void) {
    cpm_outl(0x73 | (1 << 6) , CPM_DRCG);
    mdelay(5);
    cpm_outl(0x71 | (1 << 6), CPM_DRCG);
    mdelay(5);
}

static void reset_controller(void) {
    ddr_writel(0xf << 20, DDRC_CTRL);
    mdelay(5);
    ddr_writel(0, DDRC_CTRL);
    mdelay(5);
}

static void ddr_phy_param_init(unsigned int mode) {
    int i;
    unsigned int timeout = 10000;

    ddr_writel(DDRP_DCR_VALUE, DDRP_DCR);
    ddr_writel(DDRP_MR0_VALUE, DDRP_MR0);

    switch(DDR_TYPE_MODE(mode)){
    case LPDDR:
        break;
    //note: lpddr2 should set mr1 mr2 mr3.
    case LPDDR2:
        ddr_writel(DDRP_MR3_VALUE, DDRP_MR3);
    //note: ddr3 should set mr1 mr2 only.
    case DDR3:
        ddr_writel(DDRP_MR1_VALUE, DDRP_MR1);
        ddr_writel(DDRP_MR2_VALUE, DDRP_MR2);

        break;
    case DDR2:
        ddr_writel(DDRP_MR1_VALUE, DDRP_MR1);
        break;
    }

#ifdef CONFIG_SYS_DDR_CHIP_ODT
    ddr_writel(0, DDRP_ODTCR);
#endif

    ddr_writel(DDRP_PTR0_VALUE, DDRP_PTR0);
    ddr_writel(DDRP_PTR1_VALUE, DDRP_PTR1);
    ddr_writel(DDRP_PTR2_VALUE, DDRP_PTR2);
    ddr_writel(DDRP_DTPR0_VALUE, DDRP_DTPR0);
    ddr_writel(DDRP_DTPR1_VALUE, DDRP_DTPR1);
    ddr_writel(DDRP_DTPR2_VALUE, DDRP_DTPR2);

    for (i = 0; i < 4; i++) {
        unsigned int tmp = ddr_readl(DDRP_DXGCR(i));

        tmp &= ~(3 << 9);
#ifdef CONFIG_DDR_PHY_ODT
#ifdef CONFIG_DDR_PHY_DQ_ODT
        tmp |= 1 << 10;
#endif /* CONFIG_DDR_PHY_DQ_ODT */
#ifdef CONFIG_DDR_PHY_DQS_ODT
        tmp |= 1 << 9;
#endif /* CONFIG_DDR_PHY_DQS_ODT */
#endif /* CONFIG_DDR_PHY_ODT */

        ddr_writel(tmp, DDRP_DXGCR(i));
    }
    ddr_writel(DDRP_PGCR_VALUE, DDRP_PGCR);

    /***************************************************************
     *  DXCCR:
     *       DQSRES:  4...7bit  is DQSRES[].
     *       DQSNRES: 8...11bit is DQSRES[] too.
     *
     *      Selects the on-die pull-down/pull-up resistor for DQS pins.
     *      DQSRES[3]: selects pull-down (when set to 0) or pull-up (when set to 1).
     *      DQSRES[2:0] selects the resistor value as follows:
     *      000 = Open: On-die resistor disconnected
     *      001 = 688 ohms
     *      010 = 611 ohms
     *      011 = 550 ohms
     *      100 = 500 ohms
     *      101 = 458 ohms
     *      110 = 393 ohms
     *      111 = 344 ohms
     *****************************************************************
     *      Note: DQS resistor must be connected for LPDDR/LPDDR2    *
     *****************************************************************
     *     the config will affect power and stablity
     */
    switch(DDR_TYPE_MODE(mode)){
    case LPDDR:
        ddr_writel(0x30c00813, DDRP_ACIOCR);
        ddr_writel(0x4910, DDRP_DXCCR);
        break;
    case DDR3:
        break;
    case LPDDR2:
        ddr_writel(0x910, DDRP_DXCCR);
        break;
    case DDR2:
        break;
    }
    while (!(ddr_readl(DDRP_PGSR) == (DDRP_PGSR_IDONE
                    | DDRP_PGSR_DLDONE
                    | DDRP_PGSR_ZCDONE))
            && (ddr_readl(DDRP_PGSR) != 0x1f)
            && --timeout);
    if (timeout == 0)
        panic("DDR PHY init timeout: PGSR=%X\n", ddr_readl(DDRP_PGSR));
}

static void ddr_chip_init(unsigned int mode)
{
    int timeout = 10000;
    unsigned int pir_val = DDRP_PIR_INIT;
    unsigned int val;
    dwc_debug("DDR chip init\n");

    // DDRP_PIR_DRAMRST for ddr3 only
#ifndef CONFIG_FPGA
    switch(DDR_TYPE_MODE(mode)){
    case DDR3:
        pir_val |= DDRP_PIR_DRAMINT | DDRP_PIR_DRAMRST | DDRP_PIR_DLLSRST;
        break;
    case LPDDR2:
        pir_val |= DDRP_PIR_DRAMINT | DDRP_PIR_DLLSRST;
        break;
    case LPDDR:
        pir_val |= DDRP_PIR_DRAMINT;
        break;
    case DDR2:
        pir_val |= DDRP_PIR_DRAMINT | DDRP_PIR_DRAMINT;
        break;
    }
#else
    pir_val |= DDRP_PIR_DRAMINT | DDRP_PIR_DRAMRST | DDRP_PIR_DLLBYP;
#endif
    if(IS_BYPASS_MODE(mode)) {
        pir_val |= DDRP_PIR_DLLBYP | (1 << 29);
        pir_val &= ~DDRP_PIR_DLLSRST;
        // DLL Disable: only bypassmode
        ddr_writel(0x1 << 31, DDRP_ACDLLCR);
        val = ddr_readl(DDRP_DSGCR);
        /*  LPDLLPD:  only for ddr bypass mode
         * Low Power DLL Power Down: Specifies if set that the PHY should respond to the *
         * DFI low power opportunity request and power down the DLL of the PHY if the *
         * wakeup time request satisfies the DLL lock time */
        val &= ~(1 << 4);
        ddr_writel(val,DDRP_DSGCR);

        val = ddr_readl(DDRP_DLLGCR);
        val |= 1 << 23;
        ddr_writel(val,DDRP_DLLGCR);

    }
    ddr_writel(pir_val, DDRP_PIR);
    while (!(ddr_readl(DDRP_PGSR) == (DDRP_PGSR_IDONE
                      | DDRP_PGSR_DLDONE
                      | DDRP_PGSR_ZCDONE
                      | DDRP_PGSR_DIDONE))
           && (ddr_readl(DDRP_PGSR) != 0x1f)
           && --timeout);
    if (timeout == 0)
        panic("DDR init timeout: PGSR=%X\n", ddr_readl(DDRP_PGSR));
}

static int ddr_training_hardware(unsigned int mode)
{
    int result = 0;
    int timeout = 500000;
    unsigned int pir_val = DDRP_PIR_INIT;
    switch(DDR_TYPE_MODE(mode)){
    case DDR3:
    case LPDDR2:
        pir_val |= DDRP_PIR_QSTRN;
        break;
    case LPDDR:
        pir_val |= DDRP_PIR_QSTRN | DDRP_PIR_DLLLOCK;
        break;
    case DDR2:
        pir_val |= DDRP_PIR_QSTRN;
        break;
    }
    if(IS_BYPASS_MODE(mode)) {
        pir_val |= DDRP_PIR_DLLBYP | (1 << 29);
        pir_val &= ~(DDRP_PIR_DLLLOCK);
    }
    ddr_writel(pir_val, DDRP_PIR);
    while ((ddr_readl(DDRP_PGSR) != (DDRP_PGSR_IDONE
                     | DDRP_PGSR_DLDONE
                     | DDRP_PGSR_ZCDONE
                     | DDRP_PGSR_DIDONE
                     | DDRP_PGSR_DTDONE))
           && !(ddr_readl(DDRP_PGSR)
            & (DDRP_PGSR_DTDONE | DDRP_PGSR_DTERR | DDRP_PGSR_DTIERR))
           && --timeout);

    if (timeout == 0) {
        dwc_debug("DDR training timeout\n");
        result = -1;
    } else if (ddr_readl(DDRP_PGSR)
           & (DDRP_PGSR_DTERR | DDRP_PGSR_DTIERR)) {
        dwc_debug("DDR hardware training error\n");
        result = ddr_readl(DDRP_PGSR);
    }
    return result;
}

static void ddr_training(unsigned int mode)
{
    unsigned int training_state = -1;
    dwc_debug("DDR training\n");
#ifndef CONFIG_DDR_FORCE_SOFT_TRAINING
    training_state = ddr_training_hardware(mode);
#endif
    if(training_state)
    {
        int i = 0;
        for (i = 0; i < 4; i++) {
            dwc_debug("DX%dGSR0: %x\n", i, ddr_readl(DDRP_DXGSR0(i)));
        }
        dump_ddrp_register();
#ifdef CONFIG_SPL_DDR_SOFT_TRAINING
        training_state = ddr_training_software(mode);
#endif // CONFIG_SPL_DDR_SOFT_TRAINING
    }
    if(training_state)
        hang_reason("DDR training failed!\n");
}

static void ddr_impedance_matching(void)
{
#if defined(CONFIG_DDR_PHY_IMPED_PULLUP) && defined(CONFIG_DDR_PHY_IMPED_PULLDOWN)
    /**
     * DDR3 240ohm RZQ output impedance:
     *  55.1ohm     0xc
     *  49.2ohm     0xd
     *  44.5ohm     0xe
     *  40.6ohm     0xf
     *  37.4ohm     0xa
     *  34.7ohm     0xb
     *  32.4ohm     0x8
     *  30.4ohm     0x9
     *  28.6ohm     0x18
     */
    unsigned int i;
    i = ddr_readl(DDRP_ZQXCR0(0)) & ~0x3ff;
    i |= DDRP_ZQXCR_ZDEN
        | ((CONFIG_DDR_PHY_IMPED_PULLUP & 0x1f) << DDRP_ZQXCR_PULLUP_IMPED_BIT)
        | ((CONFIG_DDR_PHY_IMPED_PULLDOWN & 0x1f) << DDRP_ZQXCR_PULLDOWN_IMPED_BIT);
    ddr_writel(i, DDRP_ZQXCR0(0));
#endif
}

static void ddr_phy_init(unsigned int mode)
{
    dwc_debug("DDR PHY init\n");
    ddr_writel(0x150000, DDRP_DTAR);
    /* DDR training address set*/
    ddr_phy_param_init(mode);
    ddr_chip_init(mode);
    ddr_training(mode);
    ddr_impedance_matching();
    dwc_debug("DDR PHY init OK\n");
}

void ddr_controller_init(void)
{
    dwc_debug("DDR Controller init\n");
//  dsqiu
//  mdelay(1);
    ddr_writel(DDRC_CTRL_CKE | DDRC_CTRL_ALH, DDRC_CTRL);
    ddr_writel(0, DDRC_CTRL);
    /* DDRC CFG init*/
    ddr_writel(DDRC_CFG_VALUE, DDRC_CFG);
    /* DDRC timing init*/
    ddr_writel(DDRC_TIMING1_VALUE, DDRC_TIMING(1));
    ddr_writel(DDRC_TIMING2_VALUE, DDRC_TIMING(2));
    ddr_writel(DDRC_TIMING3_VALUE, DDRC_TIMING(3));
    ddr_writel(DDRC_TIMING4_VALUE, DDRC_TIMING(4));
    ddr_writel(DDRC_TIMING5_VALUE, DDRC_TIMING(5));
    ddr_writel(DDRC_TIMING6_VALUE, DDRC_TIMING(6));

    /* DDRC memory map configure*/
    ddr_writel(DDRC_MMAP0_VALUE, DDRC_MMAP0);
    ddr_writel(DDRC_MMAP1_VALUE, DDRC_MMAP1);
    ddr_writel(DDRC_CTRL_CKE | DDRC_CTRL_ALH, DDRC_CTRL);
    ddr_writel(DDRC_REFCNT_VALUE, DDRC_REFCNT);
    ddr_writel(DDRC_CTRL_VALUE, DDRC_CTRL);
}

static void remap_swap(int a, int b)
{
    uint32_t remmap[2], tmp[2];

    remmap[0] = ddr_readl(DDRC_REMAP(a / 4 + 1));
    remmap[1] = ddr_readl(DDRC_REMAP(b / 4 + 1));

#define BIT(bit) ((bit % 4) * 8)
#define MASK(bit) (0x1f << BIT(bit))
    tmp[0] = (remmap[0] & MASK(a)) >> BIT(a);
    tmp[1] = (remmap[1] & MASK(b)) >> BIT(b);

    remmap[0] &= ~MASK(a);
    remmap[1] &= ~MASK(b);

    ddr_writel(remmap[0] | (tmp[1] << BIT(a)), DDRC_REMAP(a / 4 + 1));
    ddr_writel(remmap[1] | (tmp[0] << BIT(b)), DDRC_REMAP(b / 4 + 1));
#undef BIT
#undef MASK
}

static void mem_remap(void)
{
    uint32_t start = 0, num = 0;
    int row, col, dw32, bank8, cs0, cs1;

    row = ddr_row;
    col = ddr_col;
    dw32 = 0;
    bank8 = DDR_BANK8;

    cs0 = 1;
    cs1 = 0;

    start += row + col + (dw32 ? 4 : 2) / 2;
    start -= 12;

    if (bank8)
        num += 3;
    else
        num += 2;

    if (cs0 && cs1)
        num++;

    for (; num > 0; num--)
        remap_swap(0 + num - 1, start + num - 1);
}

void lpddr_init(void) {
    int type = LPDDR;
    unsigned int mode;
    unsigned int bypass = 0;
    unsigned int rate;

    /*
     * Reset DLL in PHY
     */
    reset_dll();

    rate = get_ddr_rate();

    if (rate <= 300000000)
        bypass = 1;

    /*
     * Reset DDR controller
     */
    reset_controller();

    /*
     * DDR configure
     * BL=4
     * CL=11 ?
     * DW=16
     * ODTEN=0
     * ROW=13
     * COL=9
     */
    ddr_writel(DDRC_CFG_VALUE, DDRC_CFG);

    /*
     * CKE to high
     */
    ddr_writel((1 << 1), DDRC_CTRL);

    /*
     * DDR PHY init
     */
    mode = (type << 1) | (bypass & 0x01);
    ddr_phy_init(mode);

    /*
     * End reset DDR controller
     */
    ddr_writel(0, DDRC_CTRL);

    /*
     * DDR controller init
     */
    ddr_controller_init();

    /*
     * Probe sdram size for X1000/X1000E
     */
#ifdef CONFIG_PROBE_MEM_SIZE
    probe_sdram_size();
#endif

    dump_ddrc_register();

    /*
     * DDRC address remap configure
     */
    mem_remap();

    ddr_writel(ddr_readl(DDRC_STATUS) & ~DDRC_DSTATUS_MISS, DDRC_STATUS);

    if (ddr_autosr) {
        if(!bypass)
            ddr_writel(0 , DDRC_DLP);
    }

    ddr_writel(ddr_autosr, DDRC_AUTOSR_EN);

    dwc_debug("sdram init finished\n");
}

uint32_t get_lpddr_size(void) {
    uint32_t ram_size;
    ram_size = (unsigned int)(DDR_CHIP_0_SIZE) + (unsigned int)(DDR_CHIP_1_SIZE);

    if (sdram_resized)
        return ram_size / 2;

    return ram_size;
}
