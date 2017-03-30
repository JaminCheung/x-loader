#include <common.h>
#include <string.h>

#ifdef printf
#undef printf
#endif
#include <stdio.h>

#define BETWEEN(T, MIN, MAX) if (T < MIN) T = MIN; if (T > MAX) T = MAX
struct tck tck_g = { 0, 0 };

static int calc_nck(int x, int y) {
    int value;

    value = x * 1000 % y == 0 ? x * 1000 / y : x * 1000 / y + 1;

    return value;
}

static void caculate_tck(struct ddr_params *params) {
    params->tck.ps = (1000000000 / (params->freq / 1000));
    params->tck.ns =
            (1000000000 % params->freq == 0) ?
                    (1000000000 / params->freq) :
                    (1000000000 / params->freq + 1);
    tck_g.ps = params->tck.ps;
    tck_g.ns = params->tck.ns;
}

static unsigned int sdram_size(int cs, struct ddr_params *p) {
    unsigned int dw;
    unsigned int banks;
    unsigned int size = 0;
    unsigned int row, col;

    switch (cs) {
    case 0:
        if (p->cs0 == 1) {
            row = p->row;
            col = p->col;
            break;
        } else
            return 0;
    case 1:
        if (p->cs1 == 1) {
            row = p->row1;
            col = p->col1;
            break;
        } else
            return 0;
    default:
        return 0;
    }

    banks = p->bank8 ? 8 : 4;
    dw = p->dw32 ? 4 : 2;
    size = (1 << (row + col)) * dw * banks;

    return size;
}

static void ddrc_params_creat(struct ddrc_reg *ddrc, struct ddr_params *p) {
    unsigned int tmp = 0, mem_base0 = 0, mem_base1 = 0, mem_mask0 = 0,
            mem_mask1 = 0;
    unsigned int memsize_cs0, memsize_cs1, memsize;
    struct tck *tck = &p->tck;

    /* TIMING1,2,3,4,5,6 */
    ddrc->timing1.b.tRTP = calc_nck(p->lpddr_params.tRTP, tck->ps);
    ddrc->timing1.b.tWTR = calc_nck(p->lpddr_params.tWTR, tck->ps)
            + p->lpddr_params.tWL + p->bl / 2; //??
    ddrc->timing1.b.tWR = calc_nck(p->lpddr_params.tWR, tck->ps);
    if (ddrc->timing1.b.tWR < 5)
        ddrc->timing1.b.tWR = 5;
    if (ddrc->timing1.b.tWR > 12)
        ddrc->timing1.b.tWR = 12;
    ddrc->timing1.b.tWL = p->lpddr_params.tWL;

    ddrc->timing2.b.tCCD = p->lpddr_params.tCCD;
    ddrc->timing2.b.tRAS = calc_nck(p->lpddr_params.tRAS, tck->ps);
    ddrc->timing2.b.tRCD = calc_nck(p->lpddr_params.tRCD, tck->ps);
    ddrc->timing2.b.tRL = p->lpddr_params.tRL;

    ddrc->timing3.b.ONUM = 4;

    /* Set DDR_tCKSRE to max to ensafe suspend & resume */
    ddrc->timing3.b.tCKSRE = 7;

    ddrc->timing3.b.tRP = calc_nck(p->lpddr_params.tRP, tck->ps);
    ddrc->timing3.b.tRRD = calc_nck(p->lpddr_params.tRRD, tck->ps);
    ddrc->timing3.b.tRC = calc_nck(p->lpddr_params.tRC, tck->ps);

    ddrc->timing4.b.tRFC = (calc_nck(p->lpddr_params.tRFC, tck->ps) - 1) / 2;
    ddrc->timing4.b.tEXTRW = 3;/* Why?*/
    ddrc->timing4.b.tRWCOV = 3;/* Why?*/
    ddrc->timing4.b.tCKE = p->lpddr_params.tCKE - 1;
    tmp = p->lpddr_params.tMINSR;
    if (tmp < 9)
        tmp = 9;
    if (tmp > 129)
        tmp = 129;
    tmp = ((tmp - 1) % 8) ? ((tmp - 1) / 8) : ((tmp - 1) / 8 - 1);
    ddrc->timing4.b.tMINSR = tmp;
    ddrc->timing4.b.tXP = p->lpddr_params.tXP;
    ddrc->timing4.b.tMRD = p->lpddr_params.tMRD - 1;

    ddrc->timing5.b.tCTLUPD = 0xff; /* 0xff is the default value */
    ddrc->timing5.b.tRTW = p->lpddr_params.tRTW;
    ddrc->timing5.b.tRDLAT = p->lpddr_params.tRDLAT;
    ddrc->timing5.b.tWDLAT = p->lpddr_params.tWDLAT;

    ddrc->timing6.b.tXSRD = p->lpddr_params.tXSRD / 4;
    tmp = calc_nck(p->lpddr_params.tFAW, tck->ps); /* NOT sure */
    if (tmp < 1)
        tmp = 1;
    ddrc->timing6.b.tFAW = tmp; /* NOT sure */
    ddrc->timing6.b.tCFGW = 2;
    ddrc->timing6.b.tCFGR = 2;

    tmp = p->lpddr_params.tREFI / tck->ns;

    tmp = tmp / (16 * (1 << p->div)) - 1;
    if (tmp < 1)
        tmp = 1;
    if (tmp > 0xff)
        tmp = 0xff;
    ddrc->refcnt = (tmp << DDRC_REFCNT_CON_BIT)
            | (p->div << DDRC_REFCNT_CLK_DIV_BIT) | DDRC_REFCNT_REF_EN;

    /* CFG */
    ddrc->cfg.b.ROW1 = p->row1 - 12;
    ddrc->cfg.b.COL1 = p->col1 - 8;
    ddrc->cfg.b.BA1 = p->bank8;
    ddrc->cfg.b.IMBA = 1;
    ddrc->cfg.b.BSL = (p->bl == 8) ? 1 : 0;
#ifdef CONFIG_DDR_CHIP_ODT
    ddrc->cfg.b.ODTEN = 1;
#else
    ddrc->cfg.b.ODTEN = 0;
#endif
    ddrc->cfg.b.MISPE = 1;
    ddrc->cfg.b.ROW0 = p->row - 12;
    ddrc->cfg.b.COL0 = p->col - 8;
    ddrc->cfg.b.CS1EN = p->cs1;
    ddrc->cfg.b.CS0EN = p->cs0;

    tmp = p->cl - 1; /* NOT used in this version */
    if (tmp < 0)
        tmp = 0;
    if (tmp > 4)
        tmp = 4;

    //TODO: why?(| 0x8)
    ddrc->cfg.b.CL = tmp | 0x8; /* NOT used in this version */

    ddrc->cfg.b.BA0 = p->bank8;
    ddrc->cfg.b.DW = p->dw32;

    ddrc->cfg.b.TYPE = 3;

    /* CTRL */
    ddrc->ctrl = DDRC_CTRL_ACTPD | DDRC_CTRL_PDT_64 | DDRC_CTRL_ACTSTP
            | DDRC_CTRL_PRET_8 | 0 << 6| DDRC_CTRL_UNALIGN
            | DDRC_CTRL_ALH | DDRC_CTRL_RDC | DDRC_CTRL_CKE;
    /* MMAP0,1 */
    memsize_cs0 = p->size.chip0;
    memsize_cs1 = p->size.chip1;
    memsize = memsize_cs0 + memsize_cs1;

    if (memsize > 0x20000000) {
        if (memsize_cs1) {
            mem_base0 = 0x0;
            mem_mask0 = (~((memsize_cs0 >> 24) - 1) & ~(memsize >> 24))
                    & DDRC_MMAP_MASK_MASK;
            mem_base1 = (memsize_cs1 >> 24) & 0xff;
            mem_mask1 = (~((memsize_cs1 >> 24) - 1) & ~(memsize >> 24))
                    & DDRC_MMAP_MASK_MASK;
        } else {
            mem_base0 = 0x0;
            mem_mask0 = ~(((memsize_cs0 * 2) >> 24) - 1) & DDRC_MMAP_MASK_MASK;
            mem_mask1 = 0;
            mem_base1 = 0xff;
        }
    } else {
        mem_base0 = (DDR_MEM_PHY_BASE >> 24) & 0xff;
        mem_mask0 = ~((memsize_cs0 >> 24) - 1) & DDRC_MMAP_MASK_MASK;
        mem_base1 = ((DDR_MEM_PHY_BASE + memsize_cs0) >> 24) & 0xff;
        mem_mask1 = ~((memsize_cs1 >> 24) - 1) & DDRC_MMAP_MASK_MASK;

    }
    ddrc->mmap[0] = mem_base0 << DDRC_MMAP_BASE_BIT | mem_mask0;
    ddrc->mmap[1] = mem_base1 << DDRC_MMAP_BASE_BIT | mem_mask1;
}

static void ddrp_params_creat(struct ddrp_reg *ddrp, struct ddr_params *p) {
    unsigned int tmp = 0;
    unsigned int dinit1 = 0;
    struct tck *tck = &p->tck;
    unsigned int count = 0;

#define PNDEF(N, P, T, MIN, MAX, PS, type)  \
        T = calc_nck(p->type.P, PS); \
        BETWEEN(T, MIN, MAX);   \
        ddrp->dtpr##N.b.P = T

    ddrp->dcr = 0 | (p->bank8 << 3);

    /* MRn registers */

    ddrp->mr0.lpddr.CL = p->cl;
    tmp = p->bl;
    while (tmp >>= 1)
        count++;
    ddrp->mr0.lpddr.BL = count;

    /* PTRn registers */
    ddrp->ptr0.b.tDLLSRST = calc_nck(p->lpddr_params.tDLLSRST, tck->ps);
    ddrp->ptr0.b.tDLLLOCK = calc_nck(5120, tck->ps); /* LPDDR default 5.12us*/
    ddrp->ptr0.b.tITMSRST = 8;

    ddrp->ptr1.b.tDINIT0 = calc_nck(200000, tck->ps); /* LPDDR default 200us*/
    tmp = calc_nck(100, tck->ps);
    ddrp->ptr1.b.tDINIT1 = tmp;

    ddrp->ptr2.b.tDINIT2 = calc_nck(100, tck->ps); /* DDR3 default 200us*/
    ddrp->ptr2.b.tDINIT3 = calc_nck(100, tck->ps);

    /* DTPR0 registers */
    ddrp->dtpr0.b.tMRD = p->lpddr_params.tMRD;
    PNDEF(0, tRTP, tmp, 2, 6, tck->ps, lpddr_params);
    PNDEF(0, tWTR, tmp, 1, 6, tck->ps, lpddr_params);
    PNDEF(0, tRP, tmp, 2, 11, tck->ps, lpddr_params);
    PNDEF(0, tRCD, tmp, 2, 11, tck->ps, lpddr_params);
    PNDEF(0, tRAS, tmp, 2, 31, tck->ps, lpddr_params);
    PNDEF(0, tRRD, tmp, 1, 8, tck->ps, lpddr_params);
    PNDEF(0, tRC, tmp, 2, 42, tck->ps, lpddr_params);
    ddrp->dtpr0.b.tCCD = (p->lpddr_params.tCCD > (p->bl / 2)) ? 1 : 0;

    /* DTPR1 registers */
    PNDEF(1, tFAW, tmp, 2, 31, tck->ps, lpddr_params);
    PNDEF(1, tRFC, tmp, 1, 255, tck->ps, lpddr_params);

    /* DTPR2 registers */
    tmp = calc_nck(p->lpddr_params.tXS, tck->ps);
    BETWEEN(tmp, 2, 1023);
    ddrp->dtpr2.b.tXS = tmp;

//      tmp = calc_nck(p->private_params.lpddr_params.tXP, tck->ps);
    tmp = p->lpddr_params.tXP;
//      BETWEEN(tmp, 2, 31);
    ddrp->dtpr2.b.tXP = tmp;

    tmp = calc_nck(5120, tck->ps);
    BETWEEN(tmp, 2, 1023);
    ddrp->dtpr2.b.tDLLK = tmp;

    /* PGCR registers */
    ddrp->pgcr =
            DDRP_PGCR_ITMDMD | DDRP_PGCR_DQSCFG | 7 << DDRP_PGCR_CKEN_BIT
                    | 2 << DDRP_PGCR_CKDV_BIT
                    | (p->cs0 | p->cs1 << 1) << DDRP_PGCR_RANKEN_BIT
                    | DDRP_PGCR_PDDISDX;

#undef BETWEEN
#undef PNDEF
}

static void ddr_params_creator(struct ddrc_reg *ddrc, struct ddrp_reg *ddrp,
        struct ddr_params *ddr_params) {
    struct ddr_params *ddr_params_p = ddr_params;
    memset(ddrc, 0, sizeof(struct ddrc_reg));
    memset(ddrp, 0, sizeof(struct ddrp_reg));

    ddrc_params_creat(ddrc, ddr_params);
    ddrp_params_creat(ddrp, ddr_params);
}

static void fill_in_params(struct ddr_params *ddr_params, int type) {
    struct lpddr_params *params = NULL;
    uint32_t pll_rate = CONFIG_DDR_SEL_PLL == APLL ? CONFIG_APLL_FREQ : CONFIG_MPLL_FREQ;

    params = &ddr_params->lpddr_params;

    memset(params, 0, sizeof(struct lpddr_params));

    ddr_params->type = type;
    ddr_params->freq = (pll_rate * 1000 * 1000 / CONFIG_DDR_FREQ_DIV);

    caculate_tck(ddr_params);

    ddr_params->div = DDR_CLK_DIV;
    ddr_params->cs0 = 1;
    ddr_params->cs1 = 0;
    ddr_params->dw32 = 0;
    ddr_params->cl = DDR_CL;
    ddr_params->bl = DDR_BL;
    ddr_params->col = DDR_COL;
    ddr_params->row = DDR_ROW;
#ifdef DDR_COL1
    ddr_params->col1 = DDR_COL1;
#endif
#ifdef DDR_ROW1
    ddr_params->row1 = DDR_ROW1;
#endif
    ddr_params->bank8 = DDR_BANK8;

    params->tRAS = DDR_tRAS;
    params->tRP = DDR_tRP;
    params->tRCD = DDR_tRCD;
    params->tRC = DDR_tRC;
    params->tWR = DDR_tWR;
    params->tRRD = DDR_tRRD;
    params->tRTP = DDR_tRTP;
    params->tWTR = DDR_tWTR;
    params->tRFC = DDR_tRFC;
    params->tMINSR = DDR_tMINSR;
    params->tXP = DDR_tXP;
    params->tMRD = DDR_tMRD;
    params->tCCD = DDR_tCCD;
    params->tFAW = DDR_tFAW;
    params->tCKE = DDR_tCKE;
    params->tRL = DDR_tRL;
    params->tWL = DDR_tWL;
    params->tRDLAT = DDR_tRDLAT;
    params->tWDLAT = DDR_tWDLAT;
    params->tRTW = DDR_tRTW;
    params->tCKSRE = DDR_tCKSRE;
    params->tXS = DDR_tXS;
    params->tXSRD = DDR_tXSRD;
    params->tREFI = DDR_tREFI;
    params->tDLLSRST = 50; /* default 50ns */

    ddr_params->size.chip0 = sdram_size(0, ddr_params);
    ddr_params->size.chip1 = sdram_size(1, ddr_params);
}

static void file_head_print(void) {
    printf("/*\n");
    printf(" * DO NOT MODIFY.\n");
    printf(" *\n");
    printf(" * This file was generated by ddr_params_creator\n");
    printf(" *\n");
    printf(" */\n");
    printf("\n");

    printf("#ifndef __DDR_REG_VALUES_H__\n");
    printf("#define __DDR_REG_VALUES_H__\n\n");
}

static void params_print(struct ddrc_reg *ddrc, struct ddrp_reg *ddrp) {
    /* DDRC registers print */
    printf("#define DDRC_CFG_VALUE          0x%08x\n", ddrc->cfg.d32);
    printf("#define DDRC_CTRL_VALUE         0x%08x\n", ddrc->ctrl);
    printf("#define DDRC_MMAP0_VALUE        0x%08x\n", ddrc->mmap[0]);
    printf("#define DDRC_MMAP1_VALUE        0x%08x\n", ddrc->mmap[1]);
    printf("#define DDRC_REFCNT_VALUE       0x%08x\n", ddrc->refcnt);
    printf("#define DDRC_TIMING1_VALUE      0x%08x\n", ddrc->timing1.d32);
    printf("#define DDRC_TIMING2_VALUE      0x%08x\n", ddrc->timing2.d32);
    printf("#define DDRC_TIMING3_VALUE      0x%08x\n", ddrc->timing3.d32);
    printf("#define DDRC_TIMING4_VALUE      0x%08x\n", ddrc->timing4.d32);
    printf("#define DDRC_TIMING5_VALUE      0x%08x\n", ddrc->timing5.d32);
    printf("#define DDRC_TIMING6_VALUE      0x%08x\n", ddrc->timing6.d32);

    /* DDRP registers print */
    printf("#define DDRP_DCR_VALUE          0x%08x\n", ddrp->dcr);
    printf("#define DDRP_MR0_VALUE          0x%08x\n", ddrp->mr0.d32);
    printf("#define DDRP_MR1_VALUE          0x%08x\n", ddrp->mr1.d32);
    printf("#define DDRP_MR2_VALUE          0x%08x\n", ddrp->mr2.d32);
    printf("#define DDRP_MR3_VALUE          0x%08x\n", ddrp->mr3.d32);
    printf("#define DDRP_PTR0_VALUE         0x%08x\n", ddrp->ptr0.d32);
    printf("#define DDRP_PTR1_VALUE         0x%08x\n", ddrp->ptr1.d32);
    printf("#define DDRP_PTR2_VALUE         0x%08x\n", ddrp->ptr2.d32);
    printf("#define DDRP_DTPR0_VALUE        0x%08x\n", ddrp->dtpr0.d32);
    printf("#define DDRP_DTPR1_VALUE        0x%08x\n", ddrp->dtpr1.d32);
    printf("#define DDRP_DTPR2_VALUE        0x%08x\n", ddrp->dtpr2.d32);
    printf("#define DDRP_PGCR_VALUE         0x%08x\n", ddrp->pgcr);
}

static void sdram_size_print(struct ddr_params *p) {
    printf("#define DDR_CHIP_0_SIZE         %u\n", p->size.chip0);
    printf("#define DDR_CHIP_1_SIZE         %u\n", p->size.chip1);
}

static void file_end_print(void) {
    printf("\n#endif /* __DDR_REG_VALUES_H__ */\n");
}

int main(void) {
    struct ddrc_reg ddrc;
    struct ddrp_reg ddrp;
    struct ddr_params ddr_params;

    fill_in_params(&ddr_params, LPDDR);

    ddr_params_creator(&ddrc, &ddrp, &ddr_params);

    file_head_print();
    params_print(&ddrc, &ddrp);
    sdram_size_print(&ddr_params);
    file_end_print();

    return 0;
}
