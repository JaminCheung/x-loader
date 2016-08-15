 #
 #  Copyright (C) 2016 Ingenic Semiconductor Co.,Ltd
 #
 #  Zhang YanMing <yanming.zhang@ingenic.com, jamincheung@126.com>
 #
 #  X1000 series loader for u-boot/rtos/linux
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

SHELL := $(shell if [ -x "$$BASH" ]; then echo $$BASH; \
	else if [ -x /bin/bash ]; then echo /bin/bash; \
	else echo sh; fi; fi)

#
# Top directory
#
TOPDIR := $(shell pwd)

#
# Cross compiler
#
CROSS_COMPILE := mips-linux-gnu-
CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)ld
OBJDUMP := $(CROSS_COMPILE)objdump
OBJCOPY := $(CROSS_COMPILE)objcopy

#
# Out & Tools directory
#
OUTDIR := $(TOPDIR)/out
$(shell [ -d $(OUTDIR) ] || mkdir -p $(OUTDIR))
OUTDIR := $(shell cd $(OUTDIR) && /bin/pwd)
$(if $(OUTDIR),,$(error output directory "$(OUTDIR)" does not exist))
TOOLSDIR := $(TOPDIR)/tools

#
# Includes
#
ifeq ($(TOPDIR)/include/generated/config.mk, $(wildcard $(TOPDIR)/include/generated/config.mk))
include $(TOPDIR)/config.mk
include $(TOPDIR)/include/generated/config.mk

ifdef BOARD
ifeq ($(TOPDIR)/boards/$(BOARD)/board.mk, $(wildcard $(TOPDIR)/boards/$(BOARD)/board.mk))
	include $(TOPDIR)/boards/$(BOARD)/board.mk
else
$(error Could not found board named "$(BOADR)"!)
endif
else
$(error Please define BOARD first!)
endif

#
# Kernel parameter & next stage address
#
KERNEL_PARAMETER_ADDR := 0x80004000
ifeq ($(BOOT_NEXT_STAGE), 0)
BOOT_NEXT_STAGE_TEXT := 0x80100000
else
BOOT_NEXT_STAGE_TEXT := 0x80600000
endif

CFGFLAGS := -DCONFIG_BOOT_NEXT_STAGE_TEXT=$(BOOT_NEXT_STAGE_TEXT)
ifeq ($(BOOT_NEXT_STAGE), 1)
CFGFLAGS += -DCONFIG_BOOT_KERNEL -DCONFIG_KERNEL_PARAMETER_ADDR=$(KERNEL_PARAMETER_ADDR)
else
CFGFLAGS += -DCONFIG_BOOT_UBOOT
endif

ifdef BOOT_FROM
	ifeq ($(BOOT_FROM), nor)
		CFGFLAGS += -DCONFIG_BOOT_NOR -DCONFIG_BOOT_SFC
		CONFIG_BOOT_NOR=y
	endif
	ifeq ($(BOOT_FROM), nand)
		CFGFLAGS + -DCONFIG_BOOT_NAND -DCONFIG_BOOT_SFC
		CONFIG_BOOT_NAND=y
	endif
	ifeq ($(BOOT_FROM), mmc)
		CFGFLAGS += -DCONFIG_BOOT_MMC
		CONFIG_BOOT_MMC=y
	endif

#
# Compiler & Linker options
#
CFLAGS := -Os -g -march=mips32r2 -fno-pic -fno-builtin -mno-abicalls -nostdlib -EL -msoft-float -Bstatic -std=gnu11 -I$(TOPDIR)/include
CHECKFLAGS := -Wall -Wuninitialized -Wstrict-prototypes -Wundef -Werror
LDFLAGS := -nostdlib -T ldscripts/x-loader.lds -EL
DEBUGFLAGS := -DDEBUG
override CFLAGS := $(CHECKFLAGS) $(DEBUGFLAGS) $(CFLAGS) $(CFGFLAGS) $(BOARD_CFLAGS)

else
$(error Please define system boot method(nor/nand/mmc)!)
endif

#
# Sources
#
OBJS-y := start.o                                                              \
          main.o                                                               \
          boot.o                                                               \
          boot_sel.o                                                           \
          drivers/lpddr.o                                                      \
          drivers/uart.o                                                       \
          drivers/clk.o                                                        \
          drivers/gpio.o

OBJS-y += common/printf.o                                                       \
          common/common.o

OBJS-y += boards/$(BOARD)/board.o

OBJS-$(CONFIG_BOOT_MMC) +=  drivers/mmc.o
OBJS-$(CONFIG_BOOT_NAND) += drivers/sfc.o                                      \
                            drivers/nand.o
OBJS-$(CONFIG_BOOT_NOR) +=  drivers/sfc.o                                      \
                            drivers/nor.o

OBJS := $(addprefix $(TOPDIR)/, $(OBJS-y))

LIBS-y :=
LIBS := $(addprefix $(TOPDIR)/, $(LIBS-y))

#
# Targets
#
%.o:%.c
	$(CC) -c $(CFLAGS) $< -o $@

%.o:%.S
	$(CC) -c $(CFLAGS) -D__ASSEMBLY__ $< -o $@

.PHONY: all clean

all: clean $(OUTDIR)/x-loader-pad.bin

$(OUTDIR)/x-loader-pad.bin: $(OUTDIR)/x-loader.bin
	$(OBJDUMP) -D $(OUTDIR)/x-loader.elf > $(OUTDIR)/x-loader.elf.dump
	$(OBJCOPY) --gap-fill=0xff --pad-to=16384 -I binary -O binary $< $@

ifneq ($(CONFIG_BOOT_MMC),y)
$(OUTDIR)/x-loader.bin: $(OUTDIR)/x-loader.elf $(TOOLSDIR)/sfc_boot_checksum
	$(OBJCOPY) --gap-fill=0xff -O binary $< $@
	$(TOOLSDIR)/sfc_boot_checksum $@
else
$(OUTDIR)/x-loader.bin: $(OUTDIR)/x-loader.elf
	@echo -e "\n\tMMC boot not support yet!\n" >&2
	@exit 1
endif

$(OUTDIR)/x-loader.elf: $(TOOLSDIR)/ddr_params_creator $(TOOLSDIR)/uart_baudrate_lut $(OBJS) $(LIBS)
	$(LD) $(LDFLAGS) $(OBJS) $(LIBS) -o $@

ifneq ($(CONFIG_BOOT_MMC), y)
$(TOOLSDIR)/sfc_boot_checksum:
	gcc -o $@ $(TOOLSDIR)/sfc_boot_checksum.c
	strip $@
endif

$(TOOLSDIR)/ddr_params_creator: $(TOOLSDIR)/ddr_params_creator.c
	gcc -o $@ -DHOST -I$(TOPDIR)/include $<
	strip $@
	$@ > $(TOPDIR)/include/generated/ddr_reg_values.h

$(TOOLSDIR)/uart_baudrate_lut: $(TOOLSDIR)/uart_baudrate_lut.c
	gcc -o $@ -DHOST -I$(TOPDIR)/include $<
	strip $@
	$@ > $(TOPDIR)/include/generated/uart_baudrate_reg_values.h

else
all:
	@echo "Please configure first!- see README" >&2
	@exit 1
endif

#
# Board config
#
unconfig:
	rm -f include/generated/config.h include/generated/config.mk

phoenix_nor_config: unconfig
	@./mkconfig $(@:_config) phoenix nor

phoenix_nand_config: unconfig
	@./mkconfig $(@:_config) phoenix nand

phoenix_mmc_config: unconfig
	@./mkconfig $(@:_config) phoenix mmc

halley2_nor_config: unconfig
	@./mkconfig $(@:_config) halley2 nor

halley2_nand_config: unconfig
	@./mkconfig $(@:_config) halley2 nand

halley2_mmc_config: unconfig
	@./mkconfig $(@:_config) halley2 mmc

clean:
	rm -rf $(OUTDIR)/* \
			$(OBJS) \
			$(TOOLSDIR)/sfc_boot_checksum \
			$(TOOLSDIR)/ddr_params_creator \
			$(TOOLSDIR)/uart_baudrate_lut \
			$(TOPDIR)/include/generated/ddr_reg_values.h \
			$(TOPDIR)/include/generated/uart_baudrate_reg_values.h

distclean: clean unconfig

