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

static uint8_t gd5fxfq4xc_series;
static uint8_t gd5fxgq4xbxig_series;
static uint8_t addr_len;

struct spi_mode_peer spi_mode_local[] = {
    [SPI_MODE_STANDARD] = {TRAN_SPI_STANDARD, CMD_R_CACHE},
    [SPI_MODE_QUAD] = {TRAN_SPI_QUAD, CMD_FR_CACHE_QUAD},
};

struct special_spiflash_desc spinand_descs[] = {
    {
            WINBOND_VID,
            WINBOND_PID0,
            WINBOND_PID1,
            {
                    FEATURE_REG_FEATURE1,
                    BITS_BUF_EN,
                    VALUE_SET
            }
    },

    /* ======================== GD5FxGQ4xC ======================= */
    {
            GIGADEVICE_VID,
            GD5F1GQ4UC_PID0,
            GD5FxGQ4xC_PID1,
    },

    {
            GIGADEVICE_VID,
            GD5F2GQ4UC_PID0,
            GD5FxGQ4xC_PID1,
    },

    {
            GIGADEVICE_VID,
            GD5F1GQ4RC_PID0,
            GD5FxGQ4xC_PID1,
    },

    {
            GIGADEVICE_VID,
            GD5F2GQ4RC_PID0,
            GD5FxGQ4xC_PID1,
    },
    /* ====================== GD5FxGQ4xC end ===================== */
};

static inline int spinand_bad_block_check(uint32_t len, uint8_t *buf) {
    uint32_t bit0_cnt = 0;
    uint16_t *check_buf = (uint16_t *) buf;

    if(check_buf[0] != 0xff) {
        for(int i = 0; i < len * 8; i++) {
            if(!((check_buf[0] >> 1) & 0x1))
                bit0_cnt++;
        }
    }

    if(bit0_cnt > 6 * len)
        return 1;

    return 0;
}

static int spinand_read_page(uint32_t page, uint8_t *dst_addr,
        uint32_t pagesize, uint32_t blksize) {
    uint32_t read_buf;
    int column = 0;
    int oob_flag = 0;
    struct jz_sfc sfc;
    uint8_t error = 0;

read_oob:
    if (oob_flag) {
        column = pagesize;
        pagesize = 4;
        dst_addr = (uint8_t *)&read_buf;
    }

    SFC_SEND_COMMAND(&sfc, CMD_PARD, 0, page, 3, 0, 0, 0);

    SFC_SEND_COMMAND(&sfc, CMD_GET_FEATURE, 1, FEATURE_REG_STATUS1, 1, 0, 1, 0);
    sfc_read_data(&read_buf, 1);
    while((read_buf & 0x1)) {
        SFC_SEND_COMMAND(&sfc, CMD_GET_FEATURE, 1, FEATURE_REG_STATUS1, 1, 0, 1, 0);
        sfc_read_data(&read_buf, 1);
    }

    if (gd5fxgq4xbxig_series) {
        if ((read_buf >> 4) == 0x02)
            error = 1;

    } else if (gd5fxfq4xc_series){
        if ((read_buf >> 4) == 0x07)
            error = 1;
    } else {
        if(read_buf & 0x20)
            error = 1;
    }

    if (error) {
        printf("ecc error at page%d\n", page);
        return -1;
    }

    column = (column << 8) & 0xffffff00;
#ifndef CONFIG_SPI_STANDARD
    SFC_SEND_COMMAND(&sfc, SPI_MODE_QUAD, pagesize, column, addr_len, 0, 1, 0);
#else
    SFC_SEND_COMMAND(&sfc, SPI_MODE_STANDARD, pagesize, column, addr_len, 0, 1, 0);
#endif

    sfc_read_data((uint32_t *)dst_addr, pagesize);

    if (!oob_flag && !(page % CONFIG_NAND_PPB)) {
        oob_flag = 1;
        goto read_oob;

    } else if (oob_flag) {
        if (spinand_bad_block_check(2, (uint8_t *)&read_buf))
            return 1;
    }

    return 0;
}

static void spinand_dev_special_init(struct jz_sfc *sfc, uint32_t vid) {
    struct spiflash_register *regs;
    uint32_t x = 0;

    for (int i = 0; i < ARRAY_SIZE(spinand_descs); i++) {
        if (vid == spinand_descs[i].vid) {
            if ((uint8_t *)&spinand_descs[i].regs != NULL) {
                regs = &spinand_descs[i].regs;
                SFC_SEND_COMMAND(sfc, CMD_GET_FEATURE, 1, regs->addr, 1, 0, 1, 0);
                sfc_read_data(&x, 1);
                OPERAND_CONTROL(regs->action, regs->val, x);
                SFC_SEND_COMMAND(sfc, CMD_SET_FEATURE, 1, regs->addr, 1, 0, 1, 1);
                sfc_write_data(&x, 1);
                SFC_SEND_COMMAND(sfc, CMD_GET_FEATURE, 1, regs->addr, 1, 0, 1, 0);
                sfc_read_data(&x, 1);
            }
        }
    }
}

int spinand_init(void) {
    uint8_t id[4];
    uint32_t x;
    uint8_t i;
    struct jz_sfc sfc;

    /*
     * Read vid, pid0, pid1
     *
     * cmd-->vid-->pid0-->pid1
     */
    SFC_SEND_COMMAND(&sfc, CMD_RDID, 3, 0, 0, 0, 1, 0);
    sfc_read_data((uint32_t *)id, 3);

    for (i = 0; i < ARRAY_SIZE(spinand_descs); i++) {
        if (spinand_descs[i].vid == id[0] &&
                spinand_descs[i].pid0 == id[1] &&
                spinand_descs[i].pid1 == id[2])
            break;
    }

    if (i == ARRAY_SIZE(spinand_descs)) {
        addr_len = 3;
        /*
         * cmd-->addr-->pid0
         */
        SFC_SEND_COMMAND(&sfc, CMD_RDID, 2, 0, 1, 0, 1, 0);
        sfc_read_data((uint32_t *)id, 2);
        if (id[0] == GIGADEVICE_VID)
            gd5fxgq4xbxig_series = 1;
    } else {
        addr_len = 4;
        if (id[0] == GIGADEVICE_VID)
            gd5fxfq4xc_series = 1;
    }

    debug("vid=0x%x, pid0=0x%x, pid1=0x%x\n", id[0], id[1], id[2]);

    /* disable write protect */
    x = 0;
    SFC_SEND_COMMAND(&sfc, CMD_SET_FEATURE, 1, FEATURE_REG_PROTECT, 1, 0, 1, 1);
    sfc_write_data(&x, 1);

    /* enable ecc */
    x = BITS_ECC_EN;

#ifndef CONFIG_SPI_STANDARD
    x |= BITS_QUAD_EN;
#endif

    SFC_SEND_COMMAND(&sfc, CMD_SET_FEATURE, 1, FEATURE_REG_FEATURE1, 1, 0, 1, 1);
    sfc_write_data(&x, 1);

    spinand_dev_special_init(&sfc, id[0]);

    return 0;
}

int spinand_read(uint32_t src_addr, uint32_t count, uint32_t dst_addr) {
    uint32_t blksize, pagesize, page;
    uint32_t pagecopy_cnt = 0;
    uint32_t ret;
    uint8_t *buf = (uint8_t *)dst_addr;

    pagesize = CONFIG_NAND_BPP;
    blksize = CONFIG_NAND_PPB * pagesize;

    page = src_addr / pagesize;
    while (pagecopy_cnt * pagesize < count) {
        ret = spinand_read_page(page, buf, pagesize, blksize);
        if (ret > 0){
            printf("bad block %d\n", page / CONFIG_NAND_PPB);
            page += CONFIG_NAND_PPB;
            continue;

        } else if (ret < 0)
            return -1;

        buf += pagesize;
        page++;
        pagecopy_cnt++;
    }

    return 0;
}

#ifdef CONFIG_RECOVERY
int is_recovery_update_failed(void) {
    uint32_t offset = RECOVERY_UPDATE_FLAG_OFFSET;
    uint32_t length = CONFIG_NAND_BPP;
    uint32_t buffer = 0x80f00000;
    uint32_t flag = 0;

    if (spinand_read(offset, length, (uint32_t)buffer) != 0) {
        return -1;
    }
    flag = *(uint32_t*)(buffer);
    if (flag == RECOVERY_UPDATE_FLAG_UPDATING)
        return 1;

    return 0;
}

#endif
