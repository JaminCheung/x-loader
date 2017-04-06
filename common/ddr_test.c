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

enum kseg_region {
    KSEG0_REGION,
    KSEG1_REGION,
};

static int cpu_test_ddr(uint32_t start_addr, uint32_t end_addr,
        enum kseg_region region) {

    for (uint32_t i = start_addr; i < end_addr; i += sizeof(uint32_t))
        writel(i, i);

    if (KSEG0_REGION == region)
        flush_dcache_all();

    for (uint32_t i = start_addr; i < end_addr; i += sizeof(uint32_t)) {
        if (i != readl(i)) {
            printf("Error: write value: 0x%x, read value: 0x%x\n", i, readl(i));
            return -1;
        }
    }

    for (uint32_t i = start_addr; i < end_addr; i += sizeof(uint32_t))
        writel(end_addr - i, i);

    if (KSEG0_REGION == region)
        flush_dcache_all();

    for (uint32_t i = start_addr; i < end_addr; i += sizeof(uint32_t)) {
        if ((end_addr - i) != readl(i)) {
            printf("Error: write value: 0x%x, read value: 0x%x\n",
                    end_addr - i, readl(i));
            return -1;
        }
    }

    for (uint32_t i = start_addr; i < end_addr; i += sizeof(uint32_t))
        writel(0x5a5a5a5a, i);

    if (KSEG0_REGION == region)
        flush_dcache_all();

    for (uint32_t i = start_addr; i < end_addr; i += sizeof(uint32_t)) {
        if (0x5a5a5a5a != readl(i)) {
            printf("Error: write value: 0x5a5a5a5a, read value: 0x%x\n",
                    readl(i));
            return -1;
        }
    }

    return 0;
}

static int ddr_cpu_test(enum kseg_region region, uint32_t test_size) {
    uint32_t start_addr, end_addr;
    uint32_t mem_size = get_lpddr_size();
    int error = 0;

    if (test_size > mem_size || test_size == 0)
        test_size = mem_size;

    start_addr = 0x0;
    end_addr = start_addr + test_size;

    switch (region) {
    case KSEG0_REGION:
        start_addr += KSEG0;
        end_addr += KSEG0;
        if (end_addr >= 0x90000000)
            end_addr = 0x8ffffffc;
        printf("Memory test KSEG0 start, waiting...(start 0x%x, size=0x%x)\n",
                start_addr, test_size);
        break;

    case KSEG1_REGION:
    default:
        start_addr += KSEG1;
        end_addr += KSEG1;
        if (end_addr >= 0xb0000000)
            end_addr = 0xaffffffc;
        printf("Memory test KSEG1 start, waiting...(start 0x%x, size=0x%x)\n",
                start_addr, test_size);
        break;
    }

    error = cpu_test_ddr(start_addr, end_addr, region);

    printf("%s\n\n", !error ? "OK" : "FAIL");

    return 0;
}

static int ddr_dma_test(void) {
    return 0;
}

void ddr_access_test(void) {
    ddr_cpu_test(KSEG0_REGION, 0);
    ddr_cpu_test(KSEG1_REGION, 0);
    ddr_dma_test();
}
