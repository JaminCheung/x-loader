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

#define ASCR        (0x0000)    /* AES control register 0x00000000 0x0000 32        */
#define ASSR        (0x0004)    /* AES status register 0x00000000 0x0004 32     */
#define ASINTM      (0x0008)    /* AES interrupt mask register 0x00000000 0x0008 32 */
#define ASSA        (0x000c)    /* AES DMA source address 0x00000000 0x000c 32      */
#define ASDA        (0x0010)    /* AES DMA destine address 0x00000000 0x0010 32     */
#define ASTC        (0x0014)    /* AES DMA transfer count 0x00000000 0x0014 32      */
#define ASDI        (0x0018)    /* AES data input 0x00000000 0x0018 32          */
#define ASDO        (0x001c)    /* AES data output 0x00000000 0x001c 32         */
#define ASKY        (0x0020)    /* AES key input 0x00000000 0x0020 32           */
#define ASIV        (0x0024)    /* AES IV input 0x00000000 0x0024 32            */

#define ASCR_CLR        (1<<10)
#define ASCR_DMAS       (1<<9)
#define ASCR_DMAE       (1<<8)
#define ASCR_KEYL_BIT   (6)
#define ASCR_KEYL_MASK  (0x3<<6)
#define ASCR_MODE       (1<<5)
#define ASCR_DECE       (1<<4)
#define ASCR_AESS       (1<<3)
#define ASCR_KEYS       (1<<2)
#define ASCR_INIT_IV    (1<<1)
#define ASCR_EN         (1<<0)

#define ASSR_DMAD   (1<<2)
#define ASSR_AESD   (1<<1)
#define ASSR_KEYD   (1<<0)

#define ASINTM_DMA_INT  (1<<2)
#define ASINTM_AES_INT  (1<<1)
#define ASINTM_KEY_INT  (1<<0)

#define MAX_KEY_LEN_INWORD  (8)
#define IV_LEN_INWORD       (4)

#define AES_CPU_MODE
//#define AES_SUPPORT_CBC_MODE

#ifdef AES_CPU_MODE
#define MAX_BUFFER_SIZE (16)
#else
#define MAX_BUFFER_SIZE (8 * 1024)
#endif

#define aes_writel(value, reg) writel(value, AES_BASE + reg)
#define aes_readl(reg)         readl(AES_BASE + reg)

#define mcu_soft_reset()    \
    do {            \
    *(volatile unsigned int *)0xb3421030 &= ~1; \
    } while(0)

void aes_init(void) {
    enable_aes_clk();

    mdelay(1);

    mcu_soft_reset();

    mdelay(1);
}

void aes_load_key(struct aes_key* aes_key) {
    uint32_t ascr;
    uint32_t timeout;

    if (aes_key->key_len > MAX_KEY_LEN_INWORD * 4)
        hang_reason("AES key too long\n");

    ascr = aes_readl(ASCR);

    /*
     * enable and clear
     */
    ascr |= ASCR_EN;
    ascr |= ASCR_CLR;
    aes_writel(ascr, ASCR);

#ifdef AES_SUPPORT_CBC_MODE
    if (aes_key->aes_mode == AES_MODE_CBC) {

        for (int i = 0; i < aes_key->iv_len / 4; i++)
            aes_writel(((uint32_t*)aes_key->iv)[i], ASIV);

        /*
         * write initial IV
         */
        ascr |= (ASCR_MODE);
        ascr |= ASCR_INIT_IV;

    } else {
        ascr &= ~ASCR_MODE;
        ascr &= ~ASCR_INIT_IV;
    }
#else
    ascr &= ~ASCR_MODE;
    ascr &= ~ASCR_INIT_IV;
#endif

    for(int i = 0; i < aes_key->key_len / 4; i++)
        aes_writel(((uint32_t *)aes_key->key)[i], ASKY);

    /*
     * bit mode 128,192,256
     */
    ascr &= ~(ASCR_KEYL_MASK);
    ascr |= (aes_key->bit_mode << ASCR_KEYL_BIT);
    ascr |= ASCR_KEYS;
    aes_writel(ascr, ASCR);

    timeout = 0x10000;
    while(!(aes_readl(ASSR) & ASSR_KEYD) && timeout--);
    if (timeout == 0)
        hang_reason("AES key load timeout\n");

    /*
     * clear KEYD bit
     */
    aes_writel(ASSR_KEYD | aes_readl(ASSR), ASSR);
}

void aes_decrypt(struct aes_data* data) {
    uint32_t ascr;
    uint32_t aes_len;
    uint32_t input_len;
    uint8_t *data_input = data->input;
    uint8_t *data_output = data->output;
    uint32_t timeout;

    /*
     * Only support decrypt
     */
    ascr = aes_readl(ASCR);
    ascr |= (ASCR_DECE);

    input_len = data->input_len;
    while (input_len > 0) {
        if (input_len > MAX_BUFFER_SIZE)
            aes_len = MAX_BUFFER_SIZE;
        else
            aes_len = input_len;

#ifdef AES_CPU_MODE
        /*
         * CPU mode
         */
        uint32_t cpu_count = 0;
        if (aes_len % 4 != 0)
            cpu_count = aes_len / 4 + 1;
        else
            cpu_count = aes_len / 4;

        for (int i = 0; i < cpu_count; i++)
            aes_writel(((uint32_t*)data->input)[i], ASDI);

        ascr &= ~ASCR_DMAE;
        ascr |= ASCR_AESS;

        flush_dcache_all();

#else
        /*
         * DMA mode
         */
        uint32_t dma_count = 0;
        if (aes_len * 8 / 128 != 0)
            dma_count = aes_len * 8 / 128 + 1;
        else
            dma_count = aes_len * 8 / 128;

        aes_writel(dma_count, ASTC);
        aes_writel((uint32_t)data->input, ASSA);
        aes_writel((uint32_t)data->output, ASDA);

        ascr |= ASCR_DMAE;
        ascr &= ~(ASCR_AESS);
        ascr |= ASCR_DMAS;
#endif
        /*
         * start encrypt/decrypt
         */
        aes_writel(ascr, ASCR);

#ifdef AES_CPU_MODE
        timeout = 0x10000;
        while(!(aes_readl(ASSR) & ASSR_AESD) && timeout--);
        if (timeout == 0)
            hang_reason("AES decrypt timeout\n");

        aes_writel(ASSR_AESD | aes_readl(ASSR), ASSR);

        for (int i = 0; i < cpu_count; i++)
            ((uint32_t*)data->output)[i] = aes_readl(ASDO);
#else
        timeout = 0x10000;
        while(!(aes_readl(ASSR) & ASSR_DMAD) && timeout--);
        if (timeout == 0)
            hang_reason("AES decrypt timeout\n");

        aes_writel(ASSR_DMAD | aes_readl(ASSR), ASSR);
#endif

        flush_dcache_all();

        input_len -= aes_len;
        data_input += aes_len;
        data_output += aes_len;
    }
}

void aes_test(void) {
    uint32_t test_aes_key[4] = {
            0x2b7e1516, 0x28aed2a6, 0xabf71588,0x09cf4f3c,
    };
#ifdef AES_SUPPORT_CBC_MODE
    uint32_t test_iv[4] = {
            0x00010203, 0x04050607, 0x08090a0b, 0x0c0d0e0f,
    };
    uint32_t test_indata[4] = {
            0xe6fc19f8, 0xd2695885, 0x24c00008, 0xfb1a572f,
    };
#else
    uint32_t test_indata[4] = {
            0x3925841d, 0x2dc09fb, 0xdc118597, 0x196a0b32,
    };
#endif
    const uint32_t origin_data[4] = {
            0x3243f6a8, 0x885a308d, 0x313198a2, 0xe0370734,
    };

    uint32_t decrypted_buf[4];

    struct aes_key aes_key;
    struct aes_data aes_data;

    aes_key.key = (uint8_t*) test_aes_key;
    aes_key.key_len = sizeof(test_aes_key);

#ifdef AES_SUPPORT_CBC_MODE
    aes_key.iv = (uint8_t*) test_iv;
    aes_key.iv_len = sizeof(test_iv);
    aes_key.aes_mode = AES_MODE_CBC;
#else
    aes_key.aes_mode = AES_MODE_ECB;
#endif

    aes_key.bit_mode = AES_KEY_128BIT;

    aes_init();

    aes_load_key(&aes_key);

    aes_data.input = (uint8_t*) test_indata;
    aes_data.output = (uint8_t*) decrypted_buf;
    aes_data.input_len = sizeof(test_indata);

    aes_decrypt(&aes_data);

    printf("=============================\n");
    printf("Decrypt:\n");
    for (int i = 0; i < sizeof(decrypted_buf) / 4; i++)
        printf("out data[%d]=0x%x\n", i, decrypted_buf[i]);
    printf("=============================\n");

    if (memcmp(origin_data, decrypted_buf, sizeof(origin_data)))
        hang_reason("Decrypt error\n");

    printf("AES test ok.\n");
}
