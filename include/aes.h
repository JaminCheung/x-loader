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

#ifndef AES_H
#define AES_H

/*
 * AES128
 */
#define AES_KEY_128BIT      (0x00000000)

/*
 * AES192
 */
#define AES_KEY_192BIT      (0x00000001)

/*
 * AES256
 */
#define AES_KEY_256BIT      (0x00000002)

/*
 * AES ECB mode
 */
#define AES_MODE_ECB        (0x00000001)

/*
 * AES CBC mode
 */
#define AES_MODE_CBC        (0x00000002)

#define AES_MODE_NONE       (0x00000000)

struct aes_key {
    uint8_t *key;
    uint32_t key_len;
    uint32_t bit_mode;
    uint32_t aes_mode;
    uint8_t* iv;
    uint32_t iv_len;
};

struct aes_data {
    uint8_t* input;
    uint32_t input_len;
    uint8_t* output;
};

void aes_init(void);
void aes_load_key(struct aes_key*);
void aes_decrypt(struct aes_data*);

#endif /* AES_KEY */
