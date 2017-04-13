/*
 * SPL params fixer.
 * It's used for speed up M200 bootrom performance.
 *
 * Copyright (C) 2013 Ingenic Semiconductor Co.,Ltd
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

#ifdef printf
#undef printf
#endif

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

extern void *malloc(size_t size);

struct desc
{
    unsigned set_addr :16;
    unsigned poll_addr :16;
    unsigned value :32;
    unsigned poll_h_mask :32;
    unsigned poll_l_mask :32;
};

typedef union reg_cpccr
{
    /** raw register data */
    uint32_t d32;
    /** register bits */
    struct
    {
        unsigned CDIV :4;
        unsigned L2CDIV :4;
        unsigned H0DIV :4;
        unsigned H2DIV :4;
        unsigned PDIV :4;
        unsigned CE_AHB2 :1;
        unsigned CE_AHB0 :1;
        unsigned CE_CPU :1;
        unsigned GATE_SCLKA :1;
        unsigned SEL_H2PLL :2;
        unsigned SEL_H0PLL :2;
        unsigned SEL_CPLL :2;
        unsigned SEL_SRC :2;
    } b;
} reg_cpccr_t;

typedef union nand_timing
{
    /** raw register data */
    uint32_t nand_timing[4];
    /** register bits */
    struct
    {
        unsigned set_rw :8;
        unsigned wait_rw :8;
        unsigned hold_rw :8;
        unsigned set_cs :8;
        unsigned wait_cs :8;
        unsigned trr :8;
        unsigned tedo :8;
        unsigned trpre :8;
        unsigned twpre :8;
        unsigned tds :8;
        unsigned tdh :8;
        unsigned twpst :8;
        unsigned tdqsre :8;
        unsigned trhw :8;
        unsigned t1 :8;
        unsigned t2 :8;
    } b;
} nand_timing_t;

struct params
{
    unsigned int id;
    unsigned int length;
    unsigned int pll_freq;
    reg_cpccr_t cpccr;
    nand_timing_t nand_timing;
    struct desc cpm_desc[0];
};

struct desc descriptors[14] = {
    /*
     * saddr,        paddr,        value,        poll_h_mask,        poll_l_mask
     */
    { 0x20,         0xffff,        0x1fffffb4,      0,                  0 },            /* CLKGR */
    { 0x10,         0x10,          0xa9008520,      0x400,              0 },            /* CPAPCR */
    { 0x14,         0x14,          0x98000083,      0x01,               0 },            /* CPMPCR */
    { 0,            0xd4,          0x55752210,      0,                  0x7 },          /* CPCCR div */
    { 0,            0xffff,        0x95752210,      0,                  0 },            /* CPCCR sel */
    #if (defined CONFIG_MMC_PA_8BIT) || (defined CONFIG_MMC_PA_4BIT)
    { 0x68,         0x68,          0xa0000005,      0,                  0x10000000 },   /* MSC0CDR */
    #else
    { 0xa4,         0xa4,          0xa0000005,      0,                  0x10000000 },   /* MSC1CDR */
    #endif
    { 0x20,         0xffff,        0x07ffff80,      0,                  0 },            /* CLKGR */

    { 0xffff,       0xffff,        0,               0,                  0 },            /* End */
};

void dump_params(struct params *p) {
    int i;

    printf("SPL Params Fixer:\n");
    printf("id:\t\t0x%08X (%c%c%c%c)\n", p->id, ((char *) (&p->id))[0],
            ((char *) (&p->id))[1], ((char *) (&p->id))[2],
            ((char *) (&p->id))[3]);
    printf("length:\t\t%u\n", p->length);
    printf("pll_freq:\t%u\n", p->pll_freq);
    printf("CPM_CPCCR:\t0x%08X\n", p->cpccr.d32);

    for (i = 0; i < 4; i++)
        printf("nand_timing[%d]:\t0x%08X\n", i, p->nand_timing.nand_timing[i]);

    printf("descriptors:\n");
    for (i = 0; i < 14; i++) {
        struct desc *desc = &p->cpm_desc[i];

        if ((desc->set_addr == 0xffff) && (desc->poll_addr = 0xffff))
            break;

        printf("NO.%d:\n", i);
        printf("\tsaddr = 0x%04X\n", desc->set_addr);
        printf("\tpaddr = 0x%04X\n", desc->poll_addr);
        printf("\tvalue = 0x%08X\n", desc->value);
        printf("\tpoll_h_mask = 0x%08X\n", desc->poll_h_mask);
        printf("\tpoll_l_mask = 0x%08X\n", desc->poll_l_mask);
    }
}

int main(int argc, char *argv[]) {
    int fd, i, offset, params_length;
    char *spl_path, *fix_file;
    unsigned int spl_length = 0;
    struct params *params;
    char valid_id[4] = { 'I', 'N', 'G', 'E' };
    struct desc *desc;
    unsigned int *p;

    if (argc != 3) {
        printf("Usage: %s fix_file spl_path\n", argv[0]);
        return 1;
    }

    fix_file = argv[1];
    spl_path = argv[2];
    offset = 0;
    params_length = 256;

    printf("fix_file:%s spl_path:%s offset:%d params_length=%d\n", fix_file,
            spl_path, offset, params_length);

    params = (struct params *) malloc(params_length);
    memset((uint8_t *) params, 0, params_length);

    p = (unsigned int *) valid_id;
    params->id = *p;

    fd = open(spl_path, O_RDONLY);
    if (fd < 0) {
        printf("open %s Error\n", spl_path);
        return -1;
    }
    spl_length = lseek(fd, 0, SEEK_END);
    close(fd);
    params->length =
            (spl_length & 0x1ff) == 0 ?
                    spl_length : (spl_length & ~0x1ff) + 0x200;

    uint32_t cpccr = 0;
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
            (cdiv - 1) << 0 |
            (0x7) << 20;

    uint32_t pll_rate = CONFIG_CPU_SEL_PLL == APLL ? CONFIG_APLL_FREQ : CONFIG_MPLL_FREQ;

    params->pll_freq = pll_rate * 1000 * 1000;
    params->cpccr.d32 = cpccr;

    desc = params->cpm_desc;

    for (i = 0; i < 14; i++) {
        memcpy(&desc[i], &descriptors[i], sizeof(struct desc));
    }
    dump_params(params);

    fd = open(fix_file, O_RDWR);
    if (fd < 0) {
        printf("open %s Error\n", fix_file);
        return -1;
    }

    i = lseek(fd, offset, SEEK_SET);
    if (i != offset) {
        printf("lseek to %d Error\n", offset);
        return -1;
    }

    if (write(fd, params, params_length) != params_length) {
        printf("write %s Error\n", spl_path);
        return -1;
    }

    close(fd);

    return 0;
}
