 #
 #  Copyright (C) 2016 Ingenic Semiconductor Co.,Ltd
 #
 #  X1000 series bootloader for u-boot/rtos/linux
 #
 #  Zhang YanMing <yanming.zhang@ingenic.com, jamincheung@126.com>
 #
 #  This program is free software; you can redistribute it and/or modify it
 #  under  the terms of the GNU General  Public License as published by the
 #  Free Software Foundation;  either version 2 of the License, or (at your
 #  option) any later version.
 #
 #  You should have received a copy of the GNU General Public License along
 #  with this program; if not, write to the Free Software Foundation, Inc.,
 #  675 Mass Ave, Cambridge, MA 02139, USA.
 #
 #

#
# For boot from mmc/sdcard
#
ifeq ($(BOOT_FROM), mmc)

CONFIG_GPT_TABLE=y

#
# MBR partitions
#
ifneq ($(CONFIG_GPT_TABLE), y)
CONFIG_MBR_P0_OFF=3mb
CONFIG_MBR_P0_END=15mb
CONFIG_MBR_P0_TYPE=linux

CONFIG_MBR_P1_OFF=0mb
CONFIG_MBR_P1_END=0mb
CONFIG_MBR_P1_TYPE=linux

CONFIG_MBR_P2_OFF=0mb
CONFIG_MBR_P2_END=0mb
CONFIG_MBR_P2_TYPE=linux

CONFIG_MBR_P3_OFF=0mb
CONFIG_MBR_P3_END=0mb
CONFIG_MBR_P3_TYPE=linux
endif

endif

#
# For boot kernel
#
ifeq ($(BOOT_NEXT_STAGE), 1)
#
# zImage: 0
# xImage: 1
#
KERNEL_IN_XIMAGE := 1
endif

BOARD_CFLAGS :=

#
#For compiling efuse
#
CONFIG_EFUSE := n

ifeq ($(CONFIG_EFUSE), y)
BOARD_CFLAGS += -DCONFIG_EFUSE
endif
