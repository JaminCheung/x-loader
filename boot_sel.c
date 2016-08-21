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

#if (CONFIG_RECOVERY_BOOT_KEY > 0)
static int get_key_level(unsigned pin)
{
#define GPIO_DEBOUNCE  20
    int cnt = GPIO_DEBOUNCE,v = 0, t = 0;

    while (cnt--) {
        t = !!gpio_get_value(pin);
        if (v != t) {
            cnt = GPIO_DEBOUNCE;
            mdelay(1);
        }
        v = t;
    }

    return v;
}

static int get_key_status(int pin, int en_level)
{
    gpio_direction_input(pin);
    gpio_disable_pull(pin);

    return en_level == get_key_level(pin) ? KEY_PRESS : KEY_UNPRESS;
}
#endif

static int get_signature(const int signature)
{
    unsigned int flag = cpm_get_scrpad();

    if ((flag & 0xffff) == signature) {
        /*
        * Clear the signature,
        * reset the signature to force into normal boot after factory reset
        */
        cpm_set_scrpad(flag & ~(0xffff));
        return KEY_PRESS;
    } else {
        return KEY_UNPRESS;
    }
}

int get_boot_sel(void)
{
    /* Recovery signature */
    if (get_signature(RECOVERY_SIGNATURE))
        return RECOVERY_BOOT;

#if (CONFIG_RECOVERY_BOOT_KEY > 0)
    /* Recovery boot keys */
    if (get_key_status(CONFIG_RECOVERY_BOOT_KEY, CONFIG_RECOVERY_BOOT_KEY_ENLEVEL))
        return RECOVERY_BOOT;
#endif

    return NORMAL_BOOT;
}
