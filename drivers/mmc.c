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

//#define CONFIG_MSC_DEBUG

#ifndef CONFIG_MMC_MAX_BLK_COUNT
#define CONFIG_MMC_MAX_BLK_COUNT 65535
#endif

#define CONFIG_MSC_FREQ (50 * 1000 * 1000)

#define JZ_MMC_BUS_WIDTH_MASK   0x3
#define JZ_MMC_BUS_WIDTH_1      0x0
#define JZ_MMC_BUS_WIDTH_4      0x2
#define JZ_MMC_BUS_WIDTH_8      0x3
#define JZMMC_CARD_NEED_INIT    (1 << 2)

#define MMC_BOOT_AREA_PROTECTED     (0x1234)    /* Can not modified the area protected */
#define MMC_BOOT_AREA_OPENED        (0x4321)    /* Can modified the area protected */

#define MSC_CTRL            0x000
#define MSC_STAT            0x004
#define MSC_CLKRT           0x008
#define MSC_CMDAT           0x00C
#define MSC_RESTO           0x010
#define MSC_RDTO            0x014
#define MSC_BLKLEN          0x018
#define MSC_NOB             0x01C
#define MSC_SNOB            0x020
#define MSC_IMASK           0x024
#define MSC_IFLG            0x028
#define MSC_CMD             0x02C
#define MSC_ARG             0x030
#define MSC_RES             0x034
#define MSC_RXFIFO          0x038
#define MSC_TXFIFO          0x03C
#define MSC_LPM             0x040
#define MSC_DMAC            0x044
#define MSC_DMANDA          0x048
#define MSC_DMADA           0x04C
#define MSC_DMALEN          0x050
#define MSC_DMACMD          0x054
#define MSC_CTRL2           0x058
#define MSC_RTCNT           0x05C
#define MSC_DEBUG           0x0FC

/* MSC Clock and Control Register (MSC_CTRL) */
#define CTRL_SEND_CCSD          (1 << 15) /*send command completion signal disable to ceata */
#define CTRL_SEND_AS_CCSD       (1 << 14) /*send internally generated stop after sending ccsd */
#define CTRL_EXIT_MULTIPLE      (1 << 7)
#define CTRL_EXIT_TRANSFER      (1 << 6)
#define CTRL_START_READWAIT     (1 << 5)
#define CTRL_STOP_READWAIT      (1 << 4)
#define CTRL_RESET              (1 << 3)
#define CTRL_START_OP           (1 << 2)
#define CTRL_CLOCK_SHF          0
#define CTRL_CLOCK_MASK         (0x3 << CTRL_CLOCK_SHF)
#define CTRL_CLOCK_STOP         (0x1 << CTRL_CLOCK_SHF) /* Stop MMC/SD clock */
#define CTRL_CLOCK_START        (0x2 << CTRL_CLOCK_SHF) /* Start MMC/SD clock */

/* MSC Control 2 Register (MSC_CTRL2) */
#define CTRL2_PIP_SHF           24
#define CTRL2_PIP_MASK          (0x1f << CTRL2_PIP_SHF)
#define CTRL2_RST_EN            (1 << 23)
#define CTRL2_STPRM             (1 << 4)
#define CTRL2_SVC               (1 << 3)
#define CTRL2_SMS_SHF           0
#define CTRL2_SMS_MASK          (0x7 << CTRL2_SMS_SHF)
#define CTRL2_SMS_DEFSPD        (0x0 << CTRL2_SMS_SHF)
#define CTRL2_SMS_HISPD         (0x1 << CTRL2_SMS_SHF)
#define CTRL2_SMS_SDR12         (0x2 << CTRL2_SMS_SHF)
#define CTRL2_SMS_SDR25         (0x3 << CTRL2_SMS_SHF)
#define CTRL2_SMS_SDR50         (0x4 << CTRL2_SMS_SHF)

/* MSC Status Register (MSC_STAT) */
#define STAT_AUTO_CMD12_DONE        (1 << 31)
#define STAT_AUTO_CMD23_DONE        (1 << 30)
#define STAT_SVS            (1 << 29)
#define STAT_PIN_LEVEL_SHF      24
#define STAT_PIN_LEVEL_MASK     (0x1f << STAT_PIN_LEVEL_SHF)
#define STAT_BCE            (1 << 20)
#define STAT_BDE            (1 << 19)
#define STAT_BAE            (1 << 18)
#define STAT_BAR            (1 << 17)
#define STAT_IS_RESETTING       (1 << 15)
#define STAT_SDIO_INT_ACTIVE        (1 << 14)
#define STAT_PRG_DONE           (1 << 13)
#define STAT_DATA_TRAN_DONE     (1 << 12)
#define STAT_END_CMD_RES        (1 << 11)
#define STAT_DATA_FIFO_AFULL        (1 << 10)
#define STAT_IS_READWAIT        (1 << 9)
#define STAT_CLK_EN         (1 << 8)
#define STAT_DATA_FIFO_FULL     (1 << 7)
#define STAT_DATA_FIFO_EMPTY        (1 << 6)
#define STAT_CRC_RES_ERR        (1 << 5)
#define STAT_CRC_READ_ERROR     (1 << 4)
#define STAT_CRC_WRITE_ERROR_SHF    2
#define STAT_CRC_WRITE_ERROR_MASK   (0x3 << STAT_CRC_WRITE_ERROR_SHF)
#define STAT_CRC_WRITE_ERROR_NO     (0 << STAT_CRC_WRITE_ERROR_SHF)
#define STAT_CRC_WRITE_ERROR        (1 << STAT_CRC_WRITE_ERROR_SHF)
#define STAT_CRC_WRITE_ERROR_NOSTS  (2 << STAT_CRC_WRITE_ERROR_SHF)
#define STAT_TIME_OUT_RES       (1 << 1)
#define STAT_TIME_OUT_READ      (1 << 0)

/* MSC Bus Clock Control Register (MSC_CLKRT) */
#define CLKRT_CLK_RATE_SHF      0
#define CLKRT_CLK_RATE_MASK     (0x7 << CLKRT_CLK_RATE_SHF)
#define CLKRT_CLK_RATE_DIV_1        (0x0 << CLKRT_CLK_RATE_SHF) /* CLK_SRC */
#define CLKRT_CLK_RATE_DIV_2        (0x1 << CLKRT_CLK_RATE_SHF) /* 1/2 of CLK_SRC */
#define CLKRT_CLK_RATE_DIV_4        (0x2 << CLKRT_CLK_RATE_SHF) /* 1/4 of CLK_SRC */
#define CLKRT_CLK_RATE_DIV_8        (0x3 << CLKRT_CLK_RATE_SHF) /* 1/8 of CLK_SRC */
#define CLKRT_CLK_RATE_DIV_16       (0x4 << CLKRT_CLK_RATE_SHF) /* 1/16 of CLK_SRC */
#define CLKRT_CLK_RATE_DIV_32       (0x5 << CLKRT_CLK_RATE_SHF) /* 1/32 of CLK_SRC */
#define CLKRT_CLK_RATE_DIV_64       (0x6 << CLKRT_CLK_RATE_SHF) /* 1/64 of CLK_SRC */
#define CLKRT_CLK_RATE_DIV_128      (0x7 << CLKRT_CLK_RATE_SHF) /* 1/128 of CLK_SRC */

/* MSC Command Sequence Control Register (MSC_CMDAT) */
#define CMDAT_CCS_EXPECTED      (1 << 31) /* interrupts are enabled in ce-ata */
#define CMDAT_READ_CEATA        (1 << 30)
#define CMDAT_DIS_BOOT          (1 << 27)
#define CMDAT_ENB_BOOT          (1 << 26)
#define CMDAT_EXP_BOOT_ACK      (1 << 25)
#define CMDAT_BOOT_MODE         (1 << 24)
#define CMDAT_AUTO_CMD23        (1 << 18)
#define CMDAT_SDIO_PRDT         (1 << 17) /* exact 2 cycle */
#define CMDAT_AUTO_CMD12        (1 << 16)
#define CMDAT_RTRG_SHF          14
#define CMDAT_RTRG_EQUALT_16            (0x0 << CMDAT_RTRG_SHF) /*reset value*/
#define CMDAT_RTRG_EQUALT_32        (0x1 << CMDAT_RTRG_SHF)
#define CMDAT_RTRG_EQUALT_64        (0x2 << CMDAT_RTRG_SHF)
#define CMDAT_RTRG_EQUALT_96        (0x3 << CMDAT_RTRG_SHF)
#define CMDAT_TTRG_SHF          12
#define CMDAT_TTRG_LESS_16      (0x0 << CMDAT_TTRG_SHF) /*reset value*/
#define CMDAT_TTRG_LESS_32      (0x1 << CMDAT_TTRG_SHF)
#define CMDAT_TTRG_LESS_64      (0x2 << CMDAT_TTRG_SHF)
#define CMDAT_TTRG_LESS_96      (0x3 << CMDAT_TTRG_SHF)
#define CMDAT_IO_ABORT          (1 << 11)
#define CMDAT_BUS_WIDTH_SHF     9
#define CMDAT_BUS_WIDTH_MASK        (0x3 << CMDAT_BUS_WIDTH_SHF)
#define CMDAT_BUS_WIDTH_1BIT        (0x0 << CMDAT_BUS_WIDTH_SHF) /* 1-bit data bus */
#define CMDAT_BUS_WIDTH_4BIT        (0x2 << CMDAT_BUS_WIDTH_SHF) /* 4-bit data bus */
#define CMDAT_BUS_WIDTH_8BIT        (0x3 << CMDAT_BUS_WIDTH_SHF) /* 8-bit data bus */
#define CMDAT_INIT          (1 << 7)
#define CMDAT_BUSY          (1 << 6)
#define CMDAT_STREAM_BLOCK      (1 << 5)
#define CMDAT_WRITE_READ        (1 << 4)
#define CMDAT_DATA_EN           (1 << 3)
#define CMDAT_RESPONSE_SHF      0
#define CMDAT_RESPONSE_MASK     (0x7 << CMDAT_RESPONSE_SHF)
#define CMDAT_RESPONSE_NONE         (0x0 << CMDAT_RESPONSE_SHF) /* No response */
#define CMDAT_RESPONSE_R1       (0x1 << CMDAT_RESPONSE_SHF) /* Format R1 and R1b */
#define CMDAT_RESPONSE_R2       (0x2 << CMDAT_RESPONSE_SHF) /* Format R2 */
#define CMDAT_RESPONSE_R3       (0x3 << CMDAT_RESPONSE_SHF) /* Format R3 */
#define CMDAT_RESPONSE_R4       (0x4 << CMDAT_RESPONSE_SHF) /* Format R4 */
#define CMDAT_RESPONSE_R5       (0x5 << CMDAT_RESPONSE_SHF) /* Format R5 */
#define CMDAT_RESPONSE_R6       (0x6 << CMDAT_RESPONSE_SHF) /* Format R6 */
#define CMDAT_RESRONSE_R7       (0x7 << CMDAT_RESPONSE_SHF) /* Format R7 */

/* MSC Interrupts Mask Register (MSC_IMASK) */
#define IMASK_DMA_DATA_DONE     (1 << 31)
#define IMASK_WR_ALL_DONE       (1 << 23)
#define IMASK_AUTO_CMD23_DONE       (1 << 30)
#define IMASK_SVS           (1 << 29)
#define IMASK_PIN_LEVEL_SHF     24
#define IMASK_PIN_LEVEL_MASK        (0x1f << IMASK_PIN_LEVEL_SHF)
#define IMASK_BCE           (1 << 20)
#define IMASK_BDE           (1 << 19)
#define IMASK_BAE           (1 << 18)
#define IMASK_BAR           (1 << 17)
#define IMASK_DMAEND            (1 << 16)
#define IMASK_AUTO_CMD12_DONE       (1 << 15)
#define IMASK_DATA_FIFO_FULL        (1 << 14)
#define IMASK_DATA_FIFO_EMP     (1 << 13)
#define IMASK_CRC_RES_ERR       (1 << 12)
#define IMASK_CRC_READ_ERR      (1 << 11)
#define IMASK_CRC_WRITE_ERR     (1 << 10)
#define IMASK_TIME_OUT_RES      (1 << 9)
#define IMASK_TIME_OUT_READ     (1 << 8)
#define IMASK_SDIO          (1 << 7)
#define IMASK_TXFIFO_WR_REQ     (1 << 6)
#define IMASK_RXFIFO_RD_REQ     (1 << 5)
#define IMASK_END_CMD_RES       (1 << 2)
#define IMASK_PRG_DONE          (1 << 1)
#define IMASK_DATA_TRAN_DONE        (1 << 0)

/* MSC Interrupts Status Register (MSC_IREG) */
#define IFLG_DMA_DATA_DONE      (1 << 31)
#define IFLG_WR_ALL_DONE        (1 << 23)
#define IFLG_AUTO_CMD23_DONE    (1 << 30)
#define IFLG_SVS                (1 << 29)
#define IFLG_PIN_LEVEL_SHF      24
#define IFLG_PIN_LEVEL_MASK     (0x1f << IFLG_PIN_LEVEL_SHF)
#define IFLG_BCE                 (1 << 20)
#define IFLG_BDE                 (1 << 19)
#define IFLG_BAE                 (1 << 18)
#define IFLG_BAR                 (1 << 17)
#define IFLG_DMAEND              (1 << 16)
#define IFLG_AUTO_CMD12_DONE        (1 << 15)
#define IFLG_DATA_FIFO_FULL     (1 << 14)
#define IFLG_DATA_FIFO_EMP      (1 << 13)
#define IFLG_CRC_RES_ERR        (1 << 12)
#define IFLG_CRC_READ_ERR       (1 << 11)
#define IFLG_CRC_WRITE_ERR      (1 << 10)
#define IFLG_TIMEOUT_RES        (1 << 9)
#define IFLG_TIMEOUT_READ       (1 << 8)
#define IFLG_SDIO               (1 << 7)
#define IFLG_TXFIFO_WR_REQ      (1 << 6)
#define IFLG_RXFIFO_RD_REQ      (1 << 5)
#define IFLG_END_CMD_RES        (1 << 2)
#define IFLG_PRG_DONE           (1 << 1)
#define IFLG_DATA_TRAN_DONE     (1 << 0)

/* MSC Low Power Mode Register (MSC_LPM) */
#define LPM_DRV_SEL_SHF         30
#define LPM_DRV_SEL_MASK        (0x3 << LPM_DRV_SEL_SHF)
#define LPM_SMP_SEL             (1 << 29)
#define LPM_LPM                 (1 << 0)

/* MSC DMA Control Register (MSC_DMAC) */
#define DMAC_MODE_SEL           (1 << 7)
#define DMAC_AOFST_SHF          5
#define DMAC_AOFST_MASK         (0x3 << DMAC_AOFST_SHF)
#define DMAC_AOFST_0            (0 << DMAC_AOFST_SHF)
#define DMAC_AOFST_1            (1 << DMAC_AOFST_SHF)
#define DMAC_AOFST_2            (2 << DMAC_AOFST_SHF)
#define DMAC_AOFST_3            (3 << DMAC_AOFST_SHF)
#define DMAC_ALIGNEN            (1 << 4)
#define DMAC_INCR_SHF           2
#define DMAC_INCR_MASK          (0x3 << DMAC_INCR_SHF)
#define DMAC_INCR_16            (0 << DMAC_INCR_SHF)
#define DMAC_INCR_32            (1 << DMAC_INCR_SHF)
#define DMAC_INCR_64            (2 << DMAC_INCR_SHF)
#define DMAC_DMASEL             (1 << 1)
#define DMAC_DMAEN           (1 << 0)

/* MSC DMA Command Register (MSC_DMACMD) */
#define DMACMD_IDI_SHF          24
#define DMACMD_IDI_MASK         (0xff << DMACMD_IDI_SHF)
#define DMACMD_ID_SHF           16
#define DMACMD_ID_MASK          (0xff << DMACMD_ID_SHF)
#define DMACMD_OFFSET_SHF       9
#define DMACMD_OFFSET_MASK      (0x3 << DMACMD_OFFSET_SHF)
#define DMACMD_ALIGN_EN         (1 << 8)
#define DMACMD_ENDI             (1 << 1)
#define DMACMD_LINK             (1 << 0)


#define SD_VERSION_SD        0x20000
#define SD_VERSION_3        (SD_VERSION_SD | 0x300)
#define SD_VERSION_2        (SD_VERSION_SD | 0x200)
#define SD_VERSION_1_0      (SD_VERSION_SD | 0x100)
#define SD_VERSION_1_10     (SD_VERSION_SD | 0x10a)
#define MMC_VERSION_MMC     0x10000
#define MMC_VERSION_UNKNOWN (MMC_VERSION_MMC)
#define MMC_VERSION_1_2     (MMC_VERSION_MMC | 0x102)
#define MMC_VERSION_1_4     (MMC_VERSION_MMC | 0x104)
#define MMC_VERSION_2_2     (MMC_VERSION_MMC | 0x202)
#define MMC_VERSION_3       (MMC_VERSION_MMC | 0x300)
#define MMC_VERSION_4       (MMC_VERSION_MMC | 0x400)
#define MMC_VERSION_4_1     (MMC_VERSION_MMC | 0x401)
#define MMC_VERSION_4_2     (MMC_VERSION_MMC | 0x402)
#define MMC_VERSION_4_3     (MMC_VERSION_MMC | 0x403)
#define MMC_VERSION_4_41    (MMC_VERSION_MMC | 0x429)
#define MMC_VERSION_4_5     (MMC_VERSION_MMC | 0x405)

#define MMC_MODE_HS          0x001
#define MMC_MODE_HS_52MHz   0x010
#define MMC_MODE_4BIT       0x100
#define MMC_MODE_8BIT       0x200
#define MMC_MODE_SPI        0x400
#define MMC_MODE_HC          0x800

#define MMC_MODE_MASK_WIDTH_BITS (MMC_MODE_4BIT | MMC_MODE_8BIT)
#define MMC_MODE_WIDTH_BITS_SHIFT 8

#define SD_DATA_4BIT    0x00040000

#define IS_SD(x) (x->version & SD_VERSION_SD)

#define MMC_DATA_READ       1
#define MMC_DATA_WRITE      2

#define NO_CARD_ERR     -16 /* No SD/MMC card inserted */
#define UNUSABLE_ERR        -17 /* Unusable Card */
#define COMM_ERR        -18 /* Communications Error */
#define TIMEOUT         -19
#define IN_PROGRESS     -20 /* operation is in progress */

#define MMC_CMD_GO_IDLE_STATE       0
#define MMC_CMD_SEND_OP_COND        1
#define MMC_CMD_ALL_SEND_CID        2
#define MMC_CMD_SET_RELATIVE_ADDR   3
#define MMC_CMD_SET_DSR         4
#define MMC_CMD_SWITCH          6
#define MMC_CMD_SELECT_CARD     7
#define MMC_CMD_SEND_EXT_CSD        8
#define MMC_CMD_SEND_CSD        9
#define MMC_CMD_SEND_CID        10
#define MMC_CMD_STOP_TRANSMISSION   12
#define MMC_CMD_SEND_STATUS     13
#define MMC_CMD_SET_BLOCKLEN        16
#define MMC_CMD_READ_SINGLE_BLOCK   17
#define MMC_CMD_READ_MULTIPLE_BLOCK 18
#define MMC_CMD_WRITE_SINGLE_BLOCK  24
#define MMC_CMD_WRITE_MULTIPLE_BLOCK    25
#define MMC_CMD_ERASE_GROUP_START   35
#define MMC_CMD_ERASE_GROUP_END     36
#define MMC_CMD_ERASE           38
#define MMC_CMD_APP_CMD         55
#define MMC_CMD_SPI_READ_OCR        58
#define MMC_CMD_SPI_CRC_ON_OFF      59
#define MMC_CMD_RES_MAN         62

#define MMC_CMD62_ARG1          0xefac62ec
#define MMC_CMD62_ARG2          0xcbaea7


#define SD_CMD_SEND_RELATIVE_ADDR   3
#define SD_CMD_SWITCH_FUNC      6
#define SD_CMD_SEND_IF_COND     8

#define SD_CMD_APP_SET_BUS_WIDTH    6
#define SD_CMD_ERASE_WR_BLK_START   32
#define SD_CMD_ERASE_WR_BLK_END     33
#define SD_CMD_APP_SEND_OP_COND     41
#define SD_CMD_APP_SEND_SCR     51

/* SCR definitions in different words */
#define SD_HIGHSPEED_BUSY   0x00020000
#define SD_HIGHSPEED_SUPPORTED  0x00020000

#define MMC_HS_TIMING       0x00000100
#define MMC_HS_52MHZ        0x2

#define OCR_BUSY        0x80000000
#define OCR_HCS         0x40000000
#define OCR_VOLTAGE_MASK    0x007FFF80
#define OCR_ACCESS_MODE     0x60000000

#define SECURE_ERASE        0x80000000

#define MMC_STATUS_MASK     (~0x0206BF7F)
#define MMC_STATUS_RDY_FOR_DATA (1 << 8)
#define MMC_STATUS_CURR_STATE   (0xf << 9)
#define MMC_STATUS_ERROR    (1 << 19)

#define MMC_STATE_PRG       (7 << 9)

#define MMC_CSD_PERM_WRITE_PROTECT  (1 << 13)

#define MMC_VDD_165_195     0x00000080  /* VDD voltage 1.65 - 1.95 */
#define MMC_VDD_20_21       0x00000100  /* VDD voltage 2.0 ~ 2.1 */
#define MMC_VDD_21_22       0x00000200  /* VDD voltage 2.1 ~ 2.2 */
#define MMC_VDD_22_23       0x00000400  /* VDD voltage 2.2 ~ 2.3 */
#define MMC_VDD_23_24       0x00000800  /* VDD voltage 2.3 ~ 2.4 */
#define MMC_VDD_24_25       0x00001000  /* VDD voltage 2.4 ~ 2.5 */
#define MMC_VDD_25_26       0x00002000  /* VDD voltage 2.5 ~ 2.6 */
#define MMC_VDD_26_27       0x00004000  /* VDD voltage 2.6 ~ 2.7 */
#define MMC_VDD_27_28       0x00008000  /* VDD voltage 2.7 ~ 2.8 */
#define MMC_VDD_28_29       0x00010000  /* VDD voltage 2.8 ~ 2.9 */
#define MMC_VDD_29_30       0x00020000  /* VDD voltage 2.9 ~ 3.0 */
#define MMC_VDD_30_31       0x00040000  /* VDD voltage 3.0 ~ 3.1 */
#define MMC_VDD_31_32       0x00080000  /* VDD voltage 3.1 ~ 3.2 */
#define MMC_VDD_32_33       0x00100000  /* VDD voltage 3.2 ~ 3.3 */
#define MMC_VDD_33_34       0x00200000  /* VDD voltage 3.3 ~ 3.4 */
#define MMC_VDD_34_35       0x00400000  /* VDD voltage 3.4 ~ 3.5 */
#define MMC_VDD_35_36       0x00800000  /* VDD voltage 3.5 ~ 3.6 */

#define MMC_SWITCH_MODE_CMD_SET     0x00 /* Change the command set */
#define MMC_SWITCH_MODE_SET_BITS    0x01 /* Set bits in EXT_CSD byte
                        addressed by index which are
                        1 in value field */
#define MMC_SWITCH_MODE_CLEAR_BITS  0x02 /* Clear bits in EXT_CSD byte
                        addressed by index, which are
                        1 in value field */
#define MMC_SWITCH_MODE_WRITE_BYTE  0x03 /* Set target byte to value */

#define SD_SWITCH_CHECK     0
#define SD_SWITCH_SWITCH    1

/*
 * EXT_CSD fields
 */
#define EXT_CSD_GP_SIZE_MULT        143 /* R/W */
#define EXT_CSD_PARTITIONING_SUPPORT    160 /* RO */
#define EXT_CSD_RPMB_MULT       168 /* RO */
#define EXT_CSD_ERASE_GROUP_DEF     175 /* R/W */
#define EXT_CSD_BOOT_BUS_WIDTH      177
#define EXT_CSD_PART_CONF       179 /* R/W */
#define EXT_CSD_BUS_WIDTH       183 /* R/W */
#define EXT_CSD_HS_TIMING       185 /* R/W */
#define EXT_CSD_REV         192 /* RO */
#define EXT_CSD_CARD_TYPE       196 /* RO */
#define EXT_CSD_SEC_CNT         212 /* RO, 4 bytes */
#define EXT_CSD_HC_WP_GRP_SIZE      221 /* RO */
#define EXT_CSD_HC_ERASE_GRP_SIZE   224 /* RO */
#define EXT_CSD_BOOT_MULT       226 /* RO */

/*
 * EXT_CSD field definitions
 */

#define EXT_CSD_CMD_SET_NORMAL      (1 << 0)
#define EXT_CSD_CMD_SET_SECURE      (1 << 1)
#define EXT_CSD_CMD_SET_CPSECURE    (1 << 2)

#define EXT_CSD_CARD_TYPE_26    (1 << 0)    /* Card can run at 26MHz */
#define EXT_CSD_CARD_TYPE_52    (1 << 1)    /* Card can run at 52MHz */

#define EXT_CSD_BUS_WIDTH_1 0   /* Card is in 1 bit mode */
#define EXT_CSD_BUS_WIDTH_4 1   /* Card is in 4 bit mode */
#define EXT_CSD_BUS_WIDTH_8 2   /* Card is in 8 bit mode */

#define EXT_CSD_BOOT_ACK_ENABLE         (1 << 6)
#define EXT_CSD_BOOT_PARTITION_ENABLE       (1 << 3)
#define EXT_CSD_PARTITION_ACCESS_ENABLE     (1 << 0)
#define EXT_CSD_PARTITION_ACCESS_DISABLE    (0 << 0)

#define EXT_CSD_BOOT_ACK(x)     (x << 6)
#define EXT_CSD_BOOT_PART_NUM(x)    (x << 3)
#define EXT_CSD_PARTITION_ACCESS(x) (x << 0)


#define R1_ILLEGAL_COMMAND      (1 << 22)
#define R1_APP_CMD          (1 << 5)

#define MMC_RSP_PRESENT (1 << 0)
#define MMC_RSP_136 (1 << 1)        /* 136 bit response */
#define MMC_RSP_CRC (1 << 2)        /* expect valid crc */
#define MMC_RSP_BUSY    (1 << 3)        /* card may send busy */
#define MMC_RSP_OPCODE  (1 << 4)        /* response contains opcode */

#define MMC_RSP_NONE    (0)
#define MMC_RSP_R1  (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
#define MMC_RSP_R1b (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE| \
            MMC_RSP_BUSY)
#define MMC_RSP_R2  (MMC_RSP_PRESENT|MMC_RSP_136|MMC_RSP_CRC)
#define MMC_RSP_R3  (MMC_RSP_PRESENT)
#define MMC_RSP_R4  (MMC_RSP_PRESENT)
#define MMC_RSP_R5  (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
#define MMC_RSP_R6  (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)
#define MMC_RSP_R7  (MMC_RSP_PRESENT|MMC_RSP_CRC|MMC_RSP_OPCODE)

#define MMCPART_NOAVAILABLE (0xff)
#define PART_ACCESS_MASK    (0x7)
#define PART_SUPPORT        (0x1)

/* Maximum block size for MMC */
#define MMC_MAX_BLOCK_LEN   512

/* The number of MMC physical partitions.  These consist of:
 * boot partitions (2), general purpose partitions (4) in MMC v4.4.
 */
#define MMC_NUM_BOOT_PARTITION  2

/*
 * Error status including CRC_READ_ERROR, CRC_WRITE_ERROR,
 * CRC_RES_ERR, TIME_OUT_RES, TIME_OUT_READ
 */
#define ERROR_STAT      0x3f

#if (defined CONFIG_BOOT_MMC_PA_8BIT) || (defined CONFIG_BOOT_MMC_PA_4BIT)
#define IO_BASE MSC0_BASE

#elif defined(CONFIG_BOOT_MMC_PC_4BIT)

#define IO_BASE MSC1_BASE

#endif

#ifdef CONFIG_MSC_DEBUG
#define msc_debug(fmt, args...)         \
    do {                    \
        printf(fmt, ##args);        \
    } while (0)
#else
#define msc_debug(fmt, args...)         \
    do {                    \
    } while (0)
#endif

#define msc_readl(reg)          readl(IO_BASE + reg)
#define msc_writel(val, reg)    writel(val, IO_BASE + reg)
#define msc_readw(reg)          readw(IO_BASE + reg)
#define msc_writew(val, reg)    writew(val, IO_BASE + reg);

struct mmc_cmd;
struct mmc_data {
    union {
        char *dest;
        const char *src;
    };
    uint32_t        blksz;      /* data block size */
    uint32_t        blocks;     /* number of blocks */
    uint32_t        error;      /* data error */
    uint32_t        flags;
};

struct mmc_cmd {
    uint32_t opcode;
    uint32_t arg;
    uint32_t resp[4];
    uint32_t resp_type;
};

struct jz_mmc {
    uint32_t voltages;
    uint32_t version;
    uint32_t clock;
    uint32_t f_min;
    uint32_t f_max;
    uint32_t high_capacity;
    uint32_t bus_width;
    uint32_t card_caps;
    uint32_t host_caps;
    uint32_t ocr;
    uint32_t rca;
    uint32_t read_bl_len;
    uint32_t write_bl_len;
    uint32_t flags;
    uint32_t b_max;
    void (*set_ios)(struct jz_mmc *mmc);
    int (*send_cmd)(struct jz_mmc *mmc, struct mmc_cmd *cmd,
            struct mmc_data *data);
    int (*init)(struct jz_mmc *mmc);
    uint32_t op_cond_pending;
    uint32_t op_cond_response;
};

static struct jz_mmc jz_mmc_dev;

#ifdef CONFIG_BOOT_MMC_PA_8BIT
static void set_gpio_pa_as_mmc0_8bit(void) {
    gpio_set_func(GPIO_PA(16), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(17), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(18), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(19), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(20), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(21), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(22), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(23), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(24), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(25), GPIO_FUNC_1);
}
#endif

#ifdef CONFIG_BOOT_MMC_PA_4BIT
static void set_gpio_pa_as_mmc0_4bit(void) {
    gpio_set_func(GPIO_PA(20), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(21), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(22), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(23), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(24), GPIO_FUNC_1);
    gpio_set_func(GPIO_PA(25), GPIO_FUNC_1);
}
#endif

#ifdef CONFIG_BOOT_MMC_PC_4BIT
static void set_gpio_pc_as_mmc1_4bit(void) {
    gpio_set_func(GPIO_PC(0), GPIO_FUNC_0);
    gpio_set_func(GPIO_PC(1), GPIO_FUNC_0);
    gpio_set_func(GPIO_PC(2), GPIO_FUNC_0);
    gpio_set_func(GPIO_PC(3), GPIO_FUNC_0);
    gpio_set_func(GPIO_PC(4), GPIO_FUNC_0);
    gpio_set_func(GPIO_PC(5), GPIO_FUNC_0);
}
#endif


static void gpio_init(void) {
#if (defined CONFIG_BOOT_MMC_PA_8BIT)
    set_gpio_pa_as_mmc0_8bit();
#elif (defined CONFIG_BOOT_MMC_PA_4BIT)
    set_gpio_pa_as_mmc0_4bit();
#elif (defined CONFIG_BOOT_MMC_PC_4BIT)
    set_gpio_pc_as_mmc1_4bit();
#else
#error Unknown mmc I/O port!
#endif
}

static void msc_dump_reg(void)
{
    msc_debug("==================\n");
    msc_debug("CTRL  = 0x%x\n", msc_readl(MSC_CTRL));
    msc_debug("STAT  = 0x%x\n", msc_readl(MSC_STAT));
    msc_debug("CLKRT = 0x%x\n", msc_readl(MSC_CLKRT));
    msc_debug("CMDAT = 0x%x\n", msc_readl(MSC_CMDAT));
    msc_debug("RESTO = 0x%x\n", msc_readl(MSC_RESTO));
    msc_debug("RDTO  = 0x%x\n", msc_readl(MSC_RDTO));
    msc_debug("BLKLEN= 0x%x\n", msc_readl(MSC_BLKLEN));
    msc_debug("NOB   = 0x%x\n", msc_readl(MSC_NOB));
    msc_debug("SNOP  = 0x%x\n", msc_readl(MSC_SNOB));
    msc_debug("IMASK = 0x%x\n", msc_readl(MSC_IMASK));
    msc_debug("IFLG  = 0x%x\n", msc_readl(MSC_IFLG));
    msc_debug("CMD   = 0x%x\n", msc_readl(MSC_CMD));
    msc_debug("ARG   = 0x%x\n", msc_readl(MSC_ARG));
    msc_debug("RES   = 0x%x\n", msc_readl(MSC_RES));
    msc_debug("LPM   = 0x%x\n", msc_readl(MSC_LPM));
    msc_debug("CTRL2 = 0x%x\n", msc_readl(MSC_CTRL2));
    msc_debug("RTCNT = 0x%x\n", msc_readl(MSC_RTCNT));
    msc_debug("==================\n");
}

static inline void clear_msc_irq(struct jz_mmc* mmc, uint32_t bits) {
    msc_writel(bits, MSC_IFLG);
}

static int mmc_polling_status(struct jz_mmc* mmc, uint32_t status) {
    uint32_t timeout = 50 * 1000 * 1000;

    while(!(msc_readl(MSC_STAT) & (status | ERROR_STAT)) && (--timeout));

    if (!timeout) {
       printf("polling status(0x%x) timeout, MSC_STAT=0x%x\n", status,
                msc_readl(MSC_STAT));
       return -1;
    }

    if (msc_readl(MSC_STAT) & ERROR_STAT) {
        printf("polling status(0x%x) error, status=0x%x\n", status,
                msc_readl(MSC_STAT));
        return -1;
    }

    return 0;
}

static int mmc_wait_cmd_response(struct jz_mmc* mmc) {
    if (mmc_polling_status(mmc, STAT_END_CMD_RES) < 0) {
        printf("wait commond response error\n");
        return -1;
    }

    msc_writel(IFLG_END_CMD_RES, MSC_IFLG);

    return 0;
}

static int mmc_send_cmd(struct jz_mmc* mmc, struct mmc_cmd* cmd,
        struct mmc_data* data) {
    uint32_t cmdat = 0, status = 0;

    msc_writel(cmd->opcode, MSC_CMD);
    msc_writel(cmd->arg, MSC_ARG);

    if (data) {
        cmdat |= CMDAT_DATA_EN;
        if (data->flags & MMC_DATA_WRITE)
            cmdat |= CMDAT_WRITE_READ;

        msc_writel(data->blocks, MSC_NOB);
        msc_writel(data->blksz, MSC_BLKLEN);
    }

    switch (cmd->resp_type) {
    case MMC_RSP_R1:
    case MMC_RSP_R1b:
        cmdat |= CMDAT_RESPONSE_R1;
        break;
    case MMC_RSP_R2:
        cmdat |= CMDAT_RESPONSE_R2;
        break;
    case MMC_RSP_R3:
        cmdat |= CMDAT_RESPONSE_R3;
        break;
    default:
        break;
    }

    if (cmd->resp_type & MMC_RSP_BUSY)
        cmdat |= CMDAT_BUSY;

    if (!(mmc->flags & JZMMC_CARD_NEED_INIT)) {
        cmdat |= CMDAT_INIT;
        mmc->flags |= JZMMC_CARD_NEED_INIT;
    }

    cmdat |= (mmc->flags & JZ_MMC_BUS_WIDTH_MASK) << 9;

    msc_writel(cmdat, MSC_CMDAT);

    msc_writel(0xffffffff, MSC_IMASK);
    msc_writel(0xffffffff, MSC_IFLG);

    msc_writel(CTRL_START_OP, MSC_CTRL);

    if (mmc_wait_cmd_response(mmc))
        return -1;

    if (cmd->resp_type & MMC_RSP_PRESENT) {
        if (cmd->resp_type & MMC_RSP_136) {
            uint16_t a, b, c, i;
            a = msc_readw(MSC_RES);
            for (i = 0; i < 4; i++) {
                b = msc_readw(MSC_RES);
                c = msc_readw(MSC_RES);
                cmd->resp[i] = (a << 24) | (b << 8) | (c >> 8);
                a = c;
            }

        } else {
            cmd->resp[0] = msc_readw(MSC_RES) << 24;
            cmd->resp[0] |= msc_readw(MSC_RES) << 8;
            cmd->resp[0] |= msc_readw(MSC_RES) & 0xff;
        }
    }

    if (cmd->resp_type == MMC_RSP_R1b) {
        if (mmc_polling_status(mmc, STAT_PRG_DONE))
            return -1;

        clear_msc_irq(mmc, IFLG_PRG_DONE);
    }

    //TODO:
    if (data && (data->flags & MMC_DATA_WRITE)) {
        printf("MSC: Unsupport write\n");
        return -1;
    }

    if (data && (data->flags & MMC_DATA_READ)) {
        uint32_t size = data->blocks * data->blksz;
        void* buf = data->dest;

        do {
            status = msc_readl(MSC_STAT);

            if (status & STAT_TIME_OUT_READ) {
                printf("MSC: read timeout\n");
                return -1;
            }

            if (status & STAT_CRC_READ_ERROR) {
                printf("MSC: read crc error\n");
                return -1;
            }

            if (status & STAT_DATA_FIFO_EMPTY) {
                udelay(100);
                continue;
            }

            do {
                uint32_t val = msc_readl(MSC_RXFIFO);
                if (size == 1)
                    *(uint8_t *)buf = (uint8_t)val;
                else if (size == 2)
                    put_unaligned_le16(val, buf);
                else if (size >= 4)
                    put_unaligned_le32(val, buf);

                buf += 4;
                size -= 4;

                status = msc_readl(MSC_STAT);
            } while (!(status & STAT_DATA_FIFO_EMPTY));

        } while (!(status & STAT_DATA_TRAN_DONE));

        while (!(msc_readl(MSC_IFLG) & IFLG_DATA_TRAN_DONE));

        clear_msc_irq(mmc, IFLG_DATA_TRAN_DONE);
    }

    return 0;
}

static void mmc_set_ios(struct jz_mmc* mmc) {
    uint32_t lpm = 0;
    uint32_t real_rate = 0;
    uint8_t clk_div = 0;

    /*
     * Set mmc controler clock
     */
    if (mmc->clock > 1000000)
        set_mmc_freq(mmc->clock);
    else
        set_mmc_freq(25000000);

    real_rate = get_mmc_freq();

    while ((real_rate > mmc->clock) && (clk_div < 7)) {
        real_rate >>= 1;
        clk_div++;
    }

    msc_writel(clk_div, MSC_CLKRT);

    real_rate = get_mmc_freq() / (1 << clk_div);

    if (real_rate > 25000000)
        lpm = (0x2 << LPM_DRV_SEL_SHF) | LPM_SMP_SEL;

    lpm |= LPM_LPM;
    msc_writel(lpm, MSC_LPM);

    mmc->flags &= ~JZ_MMC_BUS_WIDTH_MASK;
    if (mmc->bus_width == 8)
        mmc->flags |= JZ_MMC_BUS_WIDTH_8;
    else if (mmc->bus_width == 4)
        mmc->flags |= JZ_MMC_BUS_WIDTH_4;
    else
        mmc->flags |= JZ_MMC_BUS_WIDTH_1;

    msc_debug("MSC: clk_want=%d, clk_set=%d, bus_width=%d\n", mmc->clock,
            real_rate, mmc->bus_width);
}

static void mmc_set_bus_width(struct jz_mmc* mmc, uint32_t width) {
    mmc->bus_width = width;

    mmc_set_ios(mmc);
}

static void mmc_set_clock(struct jz_mmc* mmc, uint32_t clock) {
    if (clock > mmc->f_max)
        clock = mmc->f_max;

    if (clock < mmc->f_min)
        clock = mmc->f_min;

    mmc->clock = clock;

    mmc_set_ios(mmc);
}

static int mmc_go_idle(struct jz_mmc* mmc) {
    struct mmc_cmd cmd;
    int error;

    udelay(1000);

    cmd.opcode = MMC_CMD_GO_IDLE_STATE;
    cmd.arg = 0;
    cmd.resp_type = MMC_RSP_NONE;

    error = mmc_send_cmd(mmc, &cmd, NULL);

    if (error)
        return error;

    udelay(2000);

    return 0;
}

static int mmc_send_if_cond(struct jz_mmc* mmc) {
    struct mmc_cmd cmd;
    int error = 0;

    cmd.opcode = SD_CMD_SEND_IF_COND;
    cmd.arg = ((mmc->voltages & 0xff8000) != 0) << 8 | 0xaa;
    cmd.resp_type = MMC_RSP_R7;

    error = mmc_send_cmd(mmc, &cmd, NULL);
    if (error)
        return error;

    if ((cmd.resp[0] & 0xff) != 0xaa)
        return -1;
    else
        mmc->version = SD_VERSION_2;

    return 0;
}

static int sd_send_op_cond(struct jz_mmc *mmc)
{
    int timeout = 1000;
    int error = 0;
    struct mmc_cmd cmd;

    do {
        udelay(1000);

        cmd.opcode = MMC_CMD_APP_CMD;
        cmd.resp_type = MMC_RSP_R1;
        cmd.arg = 0;

        error = mmc_send_cmd(mmc, &cmd, NULL);

        if (error)
            return error;

        cmd.opcode = SD_CMD_APP_SEND_OP_COND;
        cmd.resp_type = MMC_RSP_R3;

        /*
         * Most cards do not answer if some reserved bits
         * in the ocr are set. However, Some controller
         * can set bit 7 (reserved for low voltages), but
         * how to manage low voltages SD card is not yet
         * specified.
         */
        cmd.arg = (mmc->voltages & 0xff8000);

        if (mmc->version == SD_VERSION_2)
            cmd.arg |= OCR_HCS;

        error = mmc_send_cmd(mmc, &cmd, NULL);

        if (error)
            return error;

    } while ((!(cmd.resp[0] & OCR_BUSY)) && timeout--);

    if (timeout <= 0)
        return -1;

    if (mmc->version != SD_VERSION_2)
        mmc->version = SD_VERSION_1_0;

    mmc->ocr = cmd.resp[0];

    mmc->high_capacity = ((mmc->ocr & OCR_HCS) == OCR_HCS);
    mmc->rca = 0;

    return 0;
}

/* We pass in the cmd since otherwise the init seems to fail */
static int mmc_send_op_cond_iter(struct jz_mmc *mmc, struct mmc_cmd *cmd,
        int use_arg)
{
    int error = 0;

    cmd->opcode = MMC_CMD_SEND_OP_COND;
    cmd->resp_type = MMC_RSP_R3;
    cmd->arg = 0;
    if (use_arg) {
        cmd->arg =
            (mmc->voltages &
            (mmc->op_cond_response & OCR_VOLTAGE_MASK)) |
            (mmc->op_cond_response & OCR_ACCESS_MODE);

        if (mmc->host_caps & MMC_MODE_HC)
            cmd->arg |= OCR_HCS;
    }
    error = mmc_send_cmd(mmc, cmd, NULL);
    if (error)
        return error;
    mmc->op_cond_response = cmd->resp[0];
    return 0;
}

static int mmc_send_op_cond(struct jz_mmc *mmc)
{
    struct mmc_cmd cmd;
    int error, i;

    /* Some cards seem to need this */
    mmc_go_idle(mmc);

    /* Asking to the card its capabilities */
    mmc->op_cond_pending = 1;
    for (i = 0; i < 2; i++) {
        error = mmc_send_op_cond_iter(mmc, &cmd, i != 0);
        if (error)
            return error;

        /* exit if not busy (flag seems to be inverted) */
        if (mmc->op_cond_response & OCR_BUSY) {
            mmc->op_cond_pending = 0;
            mmc->version = MMC_VERSION_UNKNOWN;
            mmc->ocr = cmd.resp[0];

            mmc->high_capacity = ((mmc->ocr & OCR_HCS) == OCR_HCS);
            mmc->rca = 0;

            return 0;
        }
    }

    return -1;
}

int mmc_send_status(struct jz_mmc *mmc)
{
    struct mmc_cmd cmd;
    int error = 0, retries = 5;
    int timeout = 1000;

    cmd.opcode = MMC_CMD_SEND_STATUS;
    cmd.resp_type = MMC_RSP_R1;

    cmd.arg = mmc->rca << 16;

    do {
        error = mmc_send_cmd(mmc, &cmd, NULL);
        if (!error) {
            if ((cmd.resp[0] & MMC_STATUS_RDY_FOR_DATA) &&
                (cmd.resp[0] & MMC_STATUS_CURR_STATE) !=
                 MMC_STATE_PRG)
                break;
            else if (cmd.resp[0] & MMC_STATUS_MASK) {
                return -1;
            }
        } else if (--retries < 0)
            return error;

        udelay(1000);

    } while (timeout--);

    if (timeout <= 0)
        return -1;

    return 0;
}

static int mmc_set_blocklen(struct jz_mmc* mmc, uint32_t len) {
    struct mmc_cmd cmd;

    cmd.opcode = MMC_CMD_SET_BLOCKLEN;
    cmd.resp_type = MMC_RSP_R1;
    cmd.arg = len;

    return mmc_send_cmd(mmc, &cmd, NULL);
}

static int mmc_complete_op_cond(struct jz_mmc *mmc)
{
    struct mmc_cmd cmd;
    int timeout = 1000;
    int error = 0;

    mmc->op_cond_pending = 0;
    do {
        error = mmc_send_op_cond_iter(mmc, &cmd, 1);
        if (error)
            return error;

        udelay(100);
    } while (!(mmc->op_cond_response & OCR_BUSY) && timeout--);

    if (timeout <= 0)
        return -1;

    mmc->version = MMC_VERSION_UNKNOWN;
    mmc->ocr = cmd.resp[0];

    mmc->high_capacity = ((mmc->ocr & OCR_HCS) == OCR_HCS);
    mmc->rca = 0;

    return 0;
}

static int mmc_read_blocks(struct jz_mmc* mmc, void* dest, lbaint_t start,
        lbaint_t blkcnt) {
    struct mmc_cmd cmd;
    struct mmc_data data;

    if (blkcnt > 1)
        cmd.opcode = MMC_CMD_READ_MULTIPLE_BLOCK;
    else
        cmd.opcode = MMC_CMD_READ_SINGLE_BLOCK;

    if (mmc->high_capacity)
        cmd.arg = start;
    else
        cmd.arg = start * mmc->read_bl_len;

    cmd.resp_type = MMC_RSP_R1;

    data.dest = dest;
    data.blocks = blkcnt;
    data.blksz = mmc->read_bl_len;
    data.flags = MMC_DATA_READ;

    if (mmc_send_cmd(mmc, &cmd, &data))
        return 0;

    if (blkcnt > 1) {
        cmd.opcode = MMC_CMD_STOP_TRANSMISSION;
        cmd.arg = 0;
        cmd.resp_type = MMC_RSP_R1b;
        if (mmc_send_cmd(mmc, &cmd, NULL)) {
            printf("mmc failed to send stop cmd\n");
            return 0;
        }
    }

    return blkcnt;
}

static uint32_t mmc_bread(struct jz_mmc* mmc, lbaint_t start, lbaint_t blkcnt,
        void* dest) {
    lbaint_t cur = 0, blocks_todo = blkcnt;

    if (blkcnt == 0)
        return 0;

    if (mmc_set_blocklen(mmc, mmc->read_bl_len))
        return 0;

    do {
        cur = (blocks_todo > mmc->b_max) ?  mmc->b_max : blocks_todo;
        if(mmc_read_blocks(mmc, dest, start, cur) != cur)
            return 0;

        blocks_todo -= cur;
        start += cur;
        dest += cur * mmc->read_bl_len;
    } while (blocks_todo > 0);

    return blkcnt;
}

static int mmc_card_starup(struct jz_mmc* mmc) {
    struct mmc_cmd cmd;
    int error = 0;

    cmd.opcode = MMC_CMD_ALL_SEND_CID;
    cmd.resp_type = MMC_RSP_R2;
    cmd.arg = 0;

    error = mmc_send_cmd(mmc, &cmd, NULL);
    if (error < 0)
        return error;

    udelay(1000);

    /*
     * For MMC cards, set the Relative Address.
     * For SD cards, get the Relatvie Address.
     * This also puts the cards into Standby State
     */
    cmd.opcode = SD_CMD_SEND_RELATIVE_ADDR;
    cmd.arg = mmc->rca << 16;
    cmd.resp_type = MMC_RSP_R6;

    error = mmc_send_cmd(mmc, &cmd, NULL);

    if (error)
        return error;

    if (IS_SD(mmc))
        mmc->rca = (cmd.resp[0] >> 16) & 0xffff;

    /* Get the Card-Specific Data */
    cmd.opcode = MMC_CMD_SEND_CSD;
    cmd.resp_type = MMC_RSP_R2;
    cmd.arg = mmc->rca << 16;

    error = mmc_send_cmd(mmc, &cmd, NULL);
    if (error)
        return error;

    /* Waiting for the ready status */
    error = mmc_send_status(mmc);
    if (error)
        return error;

    mmc->read_bl_len = 1 << ((cmd.resp[1] >> 16) & 0xf);

    if (IS_SD(mmc))
        mmc->write_bl_len = mmc->read_bl_len;
    else
        mmc->write_bl_len = 1 << ((cmd.resp[3] >> 22) & 0xf);

    if (mmc->read_bl_len > MMC_MAX_BLOCK_LEN)
        mmc->read_bl_len = MMC_MAX_BLOCK_LEN;

    if (mmc->write_bl_len > MMC_MAX_BLOCK_LEN)
        mmc->write_bl_len = MMC_MAX_BLOCK_LEN;

    cmd.opcode = MMC_CMD_SELECT_CARD;
    cmd.resp_type = MMC_RSP_R1;
    cmd.arg = mmc->rca << 16;
    error = mmc_send_cmd(mmc, &cmd, NULL);
    if (error)
        return error;

    if (IS_SD(mmc)) {
        if (mmc->host_caps & MMC_MODE_4BIT) {
            cmd.opcode = MMC_CMD_APP_CMD;
            cmd.resp_type = MMC_RSP_R1;
            cmd.arg = mmc->rca << 16;

            error = mmc_send_cmd(mmc, &cmd, NULL);
            if (error)
                return error;

            cmd.opcode = SD_CMD_APP_SET_BUS_WIDTH;
            cmd.resp_type = MMC_RSP_R1;
            cmd.arg = 2;
            error = mmc_send_cmd(mmc, &cmd, NULL);
            if (error)
                return error;

            mmc_set_bus_width(mmc, 4);
        }

    } else {
        cmd.opcode = MMC_CMD_SWITCH;
        cmd.resp_type = MMC_RSP_R1b;
        cmd.arg = (MMC_SWITCH_MODE_WRITE_BYTE << 24) |
                     (EXT_CSD_BUS_WIDTH << 16) |
                     (EXT_CSD_BUS_WIDTH_4 << 8);

        error = mmc_send_cmd(mmc, &cmd, NULL);
        if (error < 0)
            return error;

        /* Waiting for the ready status */
        error = mmc_send_status(mmc);
        if (error < 0)
            return error;

        mmc_set_bus_width(mmc, 4);
    }

    mmc_set_clock(mmc, CONFIG_MSC_FREQ);

    return 0;
}

static int mmc_card_probe(struct jz_mmc *mmc) {
    int error = 0;

    mmc_set_bus_width(mmc, 1);

    mmc_set_clock(mmc, 1);

    /*
     * Reset the card
     */
    error = mmc_go_idle(mmc);
    if (error)
        return error;

    /*
     * Test for SD version 2
     */
    error = mmc_send_if_cond(mmc);

    /*
     * Now try to get the SD card's operating condition
     */
    error = sd_send_op_cond(mmc);
    if (error < 0) {
        error = mmc_send_op_cond(mmc);
        if (error) {
            printf("MSC: card did not respond to voltage select!\n");
            return -1;
        }
    }

    if (mmc->op_cond_pending)
        error = mmc_complete_op_cond(mmc);
    if (error < 0)
        return error;

    error = mmc_card_starup(mmc);
    if (error < 0)
        return error;

    return 0;
}

static int mmc_reset(struct jz_mmc* mmc) {
    uint32_t ctrl = 0, timeout = 0x1fff;

    /*
     * Reset controller
     */
    ctrl = CTRL_RESET;
    msc_writel(ctrl, MSC_CTRL);
    mdelay(1);
    ctrl &= ~CTRL_RESET;
    msc_writel(ctrl, MSC_CTRL);

    while ((msc_readl(MSC_STAT) & STAT_IS_RESETTING) && (--timeout)) {
        printf("MSC: controller reset timeout!");
        return -1;
    }

    /*
     * Clear interrupt mask & flag
     */
    msc_writel(0xffffffff, MSC_IMASK);
    msc_writel(0xffffffff, MSC_IFLG);

    /*
     * Set response & read timeout
     */
    msc_writel(0x100, MSC_RESTO);
    msc_writel(0x1ffffff, MSC_RDTO);

    /*
     * Enable low power mode
     */
    msc_writel(LPM_LPM, MSC_LPM);

    return 0;
}

static int mmc_controller_init(struct jz_mmc* mmc) {
    int error = 0;

    /*
     * Init gpio
     */
    gpio_init();

    /*
     * Reset mmc controller
     */
    error = mmc_reset(mmc);
    if (error < 0)
        return error;

    msc_debug("MSC: controller init done\n");

    return 0;
}

static int install_sleep_lib(struct jz_mmc* mmc) {
    uint32_t start_blk = 0;
    uint32_t blkcnt = 0;

    start_blk = (SLEEP_LIB_OFFSET + mmc->read_bl_len - 1) / mmc->read_bl_len;
    blkcnt = (SLEEP_LIB_LENGTH + mmc->read_bl_len - 1) / mmc->read_bl_len;

    if (mmc_bread(mmc, start_blk, blkcnt, (void*) SLEEP_LIB_TCSM) != blkcnt) {
        printf("Install sleep lib failed\n");
        return -1;
    }

    return 0;
}

int mmc_load(uint32_t offset, uint32_t length, uint32_t dest) {
    int error = 0;
    uint32_t blkcnt = 0;
    uint32_t start_blk = 0;
    struct jz_mmc* mmc = &jz_mmc_dev;

    mmc->send_cmd = mmc_send_cmd;
    mmc->set_ios = mmc_set_ios;
    mmc->init = mmc_controller_init;
    mmc->voltages = MMC_VDD_27_28 | MMC_VDD_28_29 | MMC_VDD_29_30 |
            MMC_VDD_30_31 | MMC_VDD_31_32 | MMC_VDD_32_33 | MMC_VDD_33_34 |
            MMC_VDD_34_35 | MMC_VDD_35_36;

    mmc->f_min = 200000;
    mmc->f_max = 52000000;

#ifdef CONFIG_BOOT_MMC_PA_8BIT
    mmc->host_caps = MMC_MODE_8BIT | MMC_MODE_HS_52MHz | MMC_MODE_HS | MMC_MODE_HC;
#else
    mmc->host_caps = MMC_MODE_4BIT | MMC_MODE_HS_52MHz | MMC_MODE_HS | MMC_MODE_HC;
#endif

    mmc->b_max = CONFIG_MMC_MAX_BLK_COUNT;

    mmc->init(mmc);

    error = mmc_card_probe(mmc);
    if (error < 0)
        goto error;

    error = install_sleep_lib(mmc);
    if (error < 0)
        goto error;

    start_blk = (offset + mmc->read_bl_len - 1) / mmc->read_bl_len;
    blkcnt =  (length + mmc->read_bl_len - 1) / mmc->read_bl_len;

    if (mmc_bread(mmc, start_blk, blkcnt, (void *) dest) != blkcnt) {
        error = -1;
        goto error;
    }

    dump_mem_content((uint32_t *) dest, 200);

    return 0;

error:
    msc_dump_reg();

    return error;
}
