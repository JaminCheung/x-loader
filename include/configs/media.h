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

#ifndef MEDIA_H
#define MEDIA_H

/*
 * Console
 */
#define CONFIG_CONSOLE_BAUDRATE      115200
#define CONFIG_CONSOLE_INDEX         2

/*
 * Override default configure
 */
#undef CONFIG_APLL_FREQ
#undef CONFIG_SFC_FREQ
#undef CONFIG_DDR_FREQ_DIV

#define CONFIG_APLL_FREQ        916
#define CONFIG_DDR_FREQ_DIV     4
#define CONFIG_SFC_FREQ         120

/*
 * The following configure only for boot kernel
 */
#ifdef CONFIG_BOOT_KERNEL

#define KERNEL_ARGS_MEM         "mem=xxM@0x0 "
#define KERNEL_ARGS_CONSOLE     "no_console_suspend console=ttyS"STR(CONFIG_CONSOLE_INDEX)","STR(CONFIG_CONSOLE_BAUDRATE)"n8 "
#define KERNEL_ARGS_OTHERS      "lpj=5009408 ip=off "

#ifdef CONFIG_GET_WIFI_MAC
#define KERNEL_ARGS_COMMON KERNEL_ARGS_MEM KERNEL_ARGS_CONSOLE KERNEL_ARGS_OTHERS KERNEL_ARGS_WIFI_MAC
#else
#define KERNEL_ARGS_COMMON KERNEL_ARGS_MEM KERNEL_ARGS_CONSOLE KERNEL_ARGS_OTHERS
#endif

#define KERNEL_ARGS_INIT        "init=/linuxrc "

#ifdef CONFIG_RECOVERY
#define CONFIG_RECOVERY_BOOT_KEY            -1
#define CONFIG_RECOVERY_BOOT_KEY_ENLEVEL    0
#define CONFIG_RECOVERY_ARGS KERNEL_ARGS_COMMON
#endif /* CONFIG_RECOVERY */

#endif /* CONFIG_BOOT_KERNEL */

/*
 * The following configure only for NAND boot
 */
#ifdef CONFIG_BOOT_SPI_NAND

#ifdef CONFIG_RECOVERY
#define CONFIG_KERNEL_ARGS KERNEL_ARGS_COMMON KERNEL_ARGS_INIT "ubi.mtd=4 root=ubi0:rootfs ubi.mtd=5 rootfstype=ubifs rw"
#else
#define CONFIG_KERNEL_ARGS KERNEL_ARGS_COMMON KERNEL_ARGS_INIT "ubi.mtd=3 root=ubi0:rootfs ubi.mtd=4 rootfstype=ubifs rw"
#endif /* CONFIG_RECOVERY */

/*
 * unit(byte)
 */
#define CONFIG_UBOOT_OFFSET         0x6400
#define CONFIG_UBOOT_LENGTH         0x40000

#define CONFIG_KERNEL_OFFSET        0x100000
#define CONFIG_KERNEL_LENGTH        0x800000

#define CONFIG_RECOVERY_OFFSET      0x980000
#define CONFIG_RECOVERY_LENGTH      0x800000

#define CONFIG_WIFI_MAC_ADDR        0x901000

#endif /* CONFIG_BOOT_SPI_NAND */

/*
 * The following configure only for NOR boot
 */
#ifdef CONFIG_BOOT_SPI_NOR

#define CONFIG_KERNEL_ARGS KERNEL_ARGS_COMMON KERNEL_ARGS_INIT "rootfstype=jffs2 root=/dev/mtdblock2 rw"

/*
 * unit(byte)
 */
#define CONFIG_UBOOT_OFFSET         0x6400
#define CONFIG_UBOOT_LENGTH         0x40000

#define CONFIG_KERNEL_OFFSET        0x8000
#define CONFIG_KERNEL_LENGTH        0x200000

#define CONFIG_RECOVERY_OFFSET      0x400000
#define CONFIG_RECOVERY_LENGTH      0x300000

#define CONFIG_WIFI_MAC_ADDR        0x341000

#endif /* CONFIG_BOOT_SPI_NOR */

/*
 * The following configure only for MMC boot
 */
#ifdef CONFIG_BOOT_MMC

#define CONFIG_BOOT_MMC_PA_4BIT
#undef CONFIG_BOOT_MMC_PA_8BIT
#undef CONFIG_BOOT_MMC_PC_4BIT

#define CONFIG_KERNEL_ARGS KERNEL_ARGS_COMMON KERNEL_ARGS_INIT "rootfstype=jffs2 root=/dev/mtdblock2 rw"

/*
 * unit(byte)
 */
#define CONFIG_UBOOT_OFFSET         0xA400
#define CONFIG_UBOOT_LENGTH         0x40000

#define CONFIG_KERNEL_OFFSET        0x40000
#define CONFIG_KERNEL_LENGTH        0x300000

#define CONFIG_RECOVERY_OFFSET      0x400000
#define CONFIG_RECOVERY_LENGTH      0x300000

#define CONFIG_WIFI_MAC_ADDR        0x39000

#endif /* CONFIG_BOOT_MMC */

/*
 * OTA
 */
#undef CONFIG_BEIJING_OTA

#ifdef CONFIG_BEIJING_OTA
#define CONFIG_OTA_KERNEL_ARGS KERNEL_ARGS_COMMON KERNEL_ARGS_INIT "rootfstype=cramfs root=/dev/mtdblock4 rw"
#define CONFIG_OTA_STEP2_KERNEL_OFFSET    0xd00000
#endif

#endif /* MEDIA_H */
