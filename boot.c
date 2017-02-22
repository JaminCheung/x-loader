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

__attribute__ ((noreturn)) static void jump_to_next_stage(uint32_t entry_addr,
        uint32_t argv) {

#if (defined CONFIG_BOOT_UBOOT)
    typedef void (*image_entry_t)(void) __attribute__ ((noreturn));

    (void) argv;

    image_entry_t image_entry = (image_entry_t) entry_addr;

    flush_cache_all();

    image_entry();

#elif (defined CONFIG_BOOT_KERNEL) /* CONFIG_BOOT_UBOOT */
    typedef void (*image_entry_t)(int, char **, void *, int *)
            __attribute__ ((noreturn));

    uint32_t *linux_argv = (uint32_t *) KERNEL_PARAMETER_ADDR;

    image_entry_t image_entry = (image_entry_t) entry_addr;

    linux_argv[0] = 0;
    linux_argv[1] = argv;

    flush_cache_all();

    image_entry(2, (char **)linux_argv, NULL, 0);

#endif /* CONFIG_BOOT_KERNEL */
}

static int load_init(void) {
/*
 * For mmc
 */
#ifdef CONFIG_BOOT_MMC

    return mmc_init();

#endif /* CONFIG_BOOT_MMC */

/*
 * For sfc
 */
#ifdef CONFIG_BOOT_SFC

    sfc_init();

#endif /* CONFIG_BOOT_SFC */

/*
 * For spi nand
 */
#ifdef CONFIG_BOOT_SPI_NAND

    return spinand_init();

#endif /* CONFIG_BOOT_SPI_NAND */

/*
 * For spi nor
 */
#ifdef CONFIG_BOOT_SPI_NOR

    return spinor_init();

#endif /* CONFIG_BOOT_SPI_NOR */

    return -1;
}

static int load(uint32_t offset, uint32_t length, uint32_t load_addr) {
/*
 * For mmc
 */
#ifdef CONFIG_BOOT_MMC

    return mmc_read(offset, length, load_addr);

#endif /* CONFIG_BOOT_MMC */

/*
 * For spi nand
 */
#ifdef CONFIG_BOOT_SPI_NAND

    return spinand_read(offset, length, load_addr);

#endif /* CONFIG_BOOT_SPI_NAND */

/*
 * For spi nor
 */
#ifdef CONFIG_BOOT_SPI_NOR

    return spinor_read(offset, length, load_addr);

#endif /* CONFIG_BOOT_SPI_NOR */

    return -1;
}

static int pre_handle_before_jump(void* arg) {
    uint32_t mem_size;

#ifdef CONFIG_PROBE_MEM_SIZE
    mem_size = get_lpddr_size();
#elif (defined CONFIG_MEM_SIZE_64M)
    mem_size = SZ_64M;
#else
    mem_size = SZ_32M;
#endif

#if (defined CONFIG_BOOT_KERNEL)

#ifdef CONFIG_GET_WIFI_MAC
    int error = 0;
    char *wifi_mac_str = NULL;
    uint8_t mac_addr[12] = {0};

    error = load(CONFIG_WIFI_MAC_ADDR, sizeof(mac_addr), (uint32_t) &mac_addr);
    if (error < 0)
        return error;

    wifi_mac_str = strstr(arg, "wifi_mac");
    if (wifi_mac_str != NULL)
        memcpy(wifi_mac_str + 9, mac_addr, sizeof(mac_addr));
#endif /* CONFIG_GET_WIFI_MAC */

    char* mem_str = NULL;

    mem_str = strstr(arg, "mem");
    if (mem_str == NULL)
        return -1;

    switch (mem_size) {
    case SZ_64M:
        memcpy(mem_str + 4, "64", 2);
        break;

    case SZ_32M:
        memcpy(mem_str + 4, "32", 2);
        break;

    default:
        return -1;
    }

#elif (defined CONFIG_BOOT_UBOOT) /* CONFIG_BOOT_UBOOT */

    switch (mem_size) {
    case SZ_64M:
        writel(MEM_SIZE_FLAG_64M, arg);
        break;

    case SZ_32M:
        writel(MEM_SIZE_FLAG_32M, arg);
        break;

    default:
        return -1;
    }
#endif

    return 0;
}

void boot_next_stage(void) {
    int error = 0;

    uint32_t load_addr = 0;
    uint32_t entry_addr = 0;
    uint32_t argv = 0;

    entry_addr = CONFIG_BOOT_NEXT_STAGE_ENTRY_ADDR;
    load_addr = CONFIG_BOOT_NEXT_STAGE_LOAD_ADDR;

    /*
     * Step 1: init controller & device
     */
    error = load_init();
    if (error)
        hang_reason("\n\tLoad init failed.\n");

    /*
     * Step 2: install sleep lib
     */
    error = load(SLEEP_LIB_OFFSET, SLEEP_LIB_LENGTH, SLEEP_LIB_TCSM);
    if (error)
        hang_reason("\n\tInstall sleep lib failed.\n");

    /*
     * Step 3: prepare kernel parameter
     */
/*
 * For boot kernel
 */
#if (defined CONFIG_BOOT_KERNEL)
    int boot_sel = -1;

    if (get_boot_sel() == RECOVERY_BOOT) {

#ifdef CONFIG_RECOVERY
        uart_puts("Mod: Recovery.\n");
        boot_sel = RECOVERY_BOOT;
        argv = (uint32_t) CONFIG_RECOVERY_ARGS;
#endif /* CONFIG_RECOVERY  */

    } else {
        uart_puts("Mod: Normal.\n");
        boot_sel = NORMAL_BOOT;
        argv = (uint32_t) CONFIG_KERNEL_ARGS;
    }

    if (boot_sel == RECOVERY_BOOT) {

#ifdef CONFIG_RECOVERY
        error = load(CONFIG_RECOVERY_OFFSET, CONFIG_RECOVERY_LENGTH, load_addr);
#endif /* CONFIG_RECOVERY */

    } else {

       #ifdef CONFIG_BEIJING_OTA
           error = ota_load(&argv, load_addr);
       #else
           error = load(CONFIG_KERNEL_OFFSET, CONFIG_KERNEL_LENGTH, load_addr);
       #endif

    }

/*
 * For boot u-boot
 */
#elif (defined CONFIG_BOOT_UBOOT)
    uart_puts("Mod: U-Boot.\n");

    argv = (uint32_t) CONFIG_MEM_SIZE_FLAG_ADDR;

    error = load(CONFIG_UBOOT_OFFSET, CONFIG_UBOOT_LENGTH, load_addr);

#endif /* CONFIG_BOOT_UBOOT */

    printf("Load address:  0x%x\n", load_addr);
    printf("Entry address: 0x%x\n", entry_addr);

    if (error < 0)
        hang_reason("\n\tLoad next stage failed.\n");

    /*
     * Step 4: prepare jump to next stage
     */
    error = pre_handle_before_jump((void *) argv);
    if (error < 0)
        hang_reason("\n\tPre handle before jump failed.\n");

    uart_puts("\nJump...\n\n");

    /*
     * Step 5: we will nerver return here
     */
    jump_to_next_stage(entry_addr, argv);
}
