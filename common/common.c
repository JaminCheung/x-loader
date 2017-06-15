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

const static uint32_t burn_magic = 'b' << 24 | 'u' << 16 | 'r' << 8 | 'n';

#define cache_op(op, addr)      \
    __asm__ __volatile__(       \
        ".set   push\n"     \
        ".set   noreorder\n"    \
        ".set   mips3\n"    \
        "cache  %0, %1\n"   \
        ".set   pop\n"      \
        :           \
        : "i" (op), "R" (*(unsigned char *)(addr)))

#define __sync()                \
    __asm__ __volatile__(           \
        ".set   push\n\t"       \
        ".set   noreorder\n\t"      \
        ".set   mips2\n\t"      \
        "sync\n\t"          \
        ".set   pop"            \
        : /* no output */       \
        : /* no input */        \
        : "memory")

#define __fast_iob()                \
    __asm__ __volatile__(           \
        ".set   push\n\t"       \
        ".set   noreorder\n\t"      \
        "lw $0,%0\n\t"      \
        "nop\n\t"           \
        ".set   pop"            \
        : /* no output */       \
        : "m" (*(int *)0xa0000000)  \
        : "memory")

#define fast_iob()              \
    do {                    \
        __sync();           \
        __fast_iob();           \
    } while (0)

uint32_t __div64_32(uint64_t *n, uint32_t base) {
    uint64_t rem = *n;
    uint64_t b = base;
    uint64_t res, d = 1;
    uint32_t high = rem >> 32;

    /* Reduce the thing a bit first */
    res = 0;
    if (high >= base) {
        high /= base;
        res = (uint64_t) high << 32;
        rem -= (uint64_t) (high*base) << 32;
    }
    while ((int64_t)b > 0 && b < rem) {
        b = b+b;
        d = d+d;
    }

    do {
        if (rem >= b) {
            rem -= b;
            res += d;
        }
        b >>= 1;
        d >>= 1;
    } while (d);

    *n = res;

    return rem;
}

void set_bit(int nr, volatile void * addr)
{
    int mask;
    volatile int    *a = addr;

    a += nr >> 5;
    mask = 1 << (nr & 0x1f);
    *a |= mask;
}

void clear_bit(int nr, volatile void * addr)
{
    int mask;
    volatile int    *a = addr;

    a += nr >> 5;
    mask = 1 << (nr & 0x1f);
    *a &= ~mask;
}

int test_bit(int nr, const volatile void *addr)
{
    return ((1UL << (nr & 31)) & (((const unsigned int *) addr)[nr >> 5])) != 0;
}

/* Note:
 * avoid delay time overflow
 * the longest delay time:
 * 12MHz    357s
 * 24MHz    178s
 * 48MHz    89s
 * */
void udelay(uint64_t usec) {
    pmon_stop();
    pmon_clear_cnt();
    pmon_start();

    while (!((get_pmon_rc()) >= (cpu_freq * usec)));

    pmon_stop();
}

void mdelay(uint32_t msec) {
    udelay(msec * 1000);
}

__attribute__((noreturn)) void hang() {
    uart_puts("\n### Hang-up - Please reset board ###\n");
    while (1)
        ;
}

__attribute__((noreturn)) void hang_reason(const char* reason) {
    uart_puts(reason);
    hang();
}

void dump_mem_content(uint32_t *src, uint32_t len)
{
    debug("====================\n");
    for(int i = 0; i < len / 4; i++) {
        debug("%x:%x\n", src, *(unsigned int *)src);
        src++;
    }
    debug("====================\n");
}

#ifndef __HOST__
void *memcpy(void *dst, const void *src, unsigned int len) {
    char *ret = dst;
    while (len-- > 0) {
        *ret++ = *((char *)src);
        src++;
    }
    return (void *)ret;
}

int memcmp(const void * cs,const void * ct, size_t count) {
    const unsigned char *su1, *su2;
    int res = 0;

    for( su1 = cs, su2 = ct; 0 < count; ++su1, ++su2, count--)
        if ((res = *su1 - *su2) != 0)
            break;
    return res;
}

size_t strlen(const char * s) {
    const char *sc;

    for (sc = s; *sc != '\0'; ++sc)
        /* nothing */;
    return sc - s;
}

char * strstr(const char * s1,const char * s2) {
    int l1, l2;

    l2 = strlen(s2);
    if (!l2)
        return (char *) s1;
        l1 = strlen(s1);
    while (l1 >= l2) {
        l1--;
        if (!memcmp(s1,s2,l2))
            return (char *) s1;
            s1++;
    }

    return NULL;
}
#endif

void local_irq_disable(void) {
    __asm__ __volatile__(
        ".set\tpush\n\t"
        ".set\treorder\n\t"
        ".set\tnoat\n\t"
        "mfc0\t$1,$12\n\t"
        "ori\t$1,1\n\t"
        "xori\t$1,1\n\t"
        ".set\tnoreorder\n\t"
        "mtc0\t$1,$12\n\t"
        "nop\n\t"
        "nop\n\t"
        "nop\n\t"
        ".set\tpop\n\t"
        : /* no outputs */
        : /* no inputs */
        : "$1", "memory");
}

void local_irq_enable(void) {
    __asm__ __volatile__(
        ".set\tpush\n\t"
        ".set\treorder\n\t"
        ".set\tnoat\n\t"
        "mfc0\t$1,$12\n\t"
        "ori\t$1,0x1f\n\t"
        "xori\t$1,0x1e\n\t"
        "mtc0\t$1,$12\n\t"
        ".set\tpop\n\t"
        : /* no outputs */
        : /* no inputs */
        : "$1", "memory");
}

void cache_init(void) {
    register unsigned long addr;
    __asm__ __volatile__ ("mtc0 $0, $28\n\t"::);

    for (addr = CKSEG0; addr < (CKSEG0 + CONFIG_SYS_DCACHE_SIZE); addr += CONFIG_SYS_CACHELINE_SIZE) {
        __asm__ __volatile__ (".set mips32\n\t"
                " cache %0, 0(%1)\n\t"
                ".set mips32\n\t"
                :
                : "I" (INDEX_STORE_TAG_D), "r"(addr));
    }
    for (addr = CKSEG0; addr < (CKSEG0 + CONFIG_SYS_ICACHE_SIZE); addr += CONFIG_SYS_CACHELINE_SIZE) {
        __asm__ __volatile__ (".set mips32\n\t"
                " cache %0, 0(%1)\n\t"
                ".set mips32\n\t"
                :
                : "I" (INDEX_STORE_TAG_I), "r"(addr));
    }
    /* invalidate BTB */
    __asm__ __volatile__ (".set mips32\n\t"
            " mfc0 $26, $16, 7\n\t"
            " nop\n\t"
            " ori $26, 2\n\t"
            " mtc0 $26, $16, 7\n\t"
            " nop\n\t"
            "nop\n\t"
            ".set mips32\n\t"
            );
}

void flush_icache_all(void) {
    uint32_t addr, t = 0;

    __asm__ __volatile__("mtc0 $0, $28"); /* Clear Taglo */
    __asm__ __volatile__("mtc0 $0, $29"); /* Clear TagHi */

    for (addr = CKSEG0; addr < CKSEG0 + CONFIG_SYS_ICACHE_SIZE;
         addr += CONFIG_SYS_CACHELINE_SIZE) {
        cache_op(INDEX_STORE_TAG_I, addr);
    }

    /* invalidate btb */
    __asm__ __volatile__(
        ".set mips32\n\t"
        "mfc0 %0, $16, 7\n\t"
        "nop\n\t"
        "ori %0,2\n\t"
        "mtc0 %0, $16, 7\n\t"
        ".set mips2\n\t"
        :
        : "r" (t));
}

void flush_dcache_all(void) {
    uint32_t addr;

    for (addr = CKSEG0; addr < CKSEG0 + CONFIG_SYS_DCACHE_SIZE; addr +=
            CONFIG_SYS_CACHELINE_SIZE) {
        cache_op(INDEX_WRITEBACK_INV_D, addr);
    }

    fast_iob();
}

void flush_cache_all(void) {
    flush_dcache_all();
    flush_icache_all();
}

void jump_to_usbboot(void) {
    uint32_t reg;

    reg = cpm_inl(CPM_SLPC);

    if(reg == burn_magic) {
        typedef void (*image_entry_t)(void) __attribute__ ((noreturn));

        image_entry_t image_entry = (image_entry_t) (0xbfc0320c);

            cpm_outl(0, CPM_SLPC);

            image_entry();
    }
}

void set_jump_to_usbboot(void) {
    cpm_outl(burn_magic, CPM_SLPC);

    wdt_restart();
}

void pass_params_to_uboot(void) {
    /*
     * Memory size
     */
    uint32_t mem_size;

#ifdef CONFIG_PROBE_MEM_SIZE
    mem_size = get_lpddr_size();
#elif (defined CONFIG_MEM_SIZE_64M)
    mem_size = SZ_64M;
#else
    mem_size = SZ_32M;
#endif

    if (mem_size == SZ_64M)
        writel(MEM_SIZE_FLAG_64M, CONFIG_MEM_SIZE_FLAG_ADDR);
    else
        writel(MEM_SIZE_FLAG_32M, CONFIG_MEM_SIZE_FLAG_ADDR);

    /*
     * Uart index and baudrate
     */
    writel(CONFIG_CONSOLE_INDEX, CONFIG_UART_INDEX_ADDR);
    writel(CONFIG_CONSOLE_BAUDRATE, CONFIG_UART_BAUDRATE_ADDR);
}

__attribute__((weak)) const int gpio_ss_table[2][2] = {
        {GSS_TABLET_END, GSS_TABLET_END},
};

inline static void board_gpio_suspend(void) {
    int pin = 0;
    int state = 0;

    for (int i = 0; gpio_ss_table[i][1] != GSS_TABLET_END; i++) {
        pin = gpio_ss_table[i][0];
        state = gpio_ss_table[i][1];

        debug("pin=%d, state=%d\n", pin, state);

        switch(state) {
        case GSS_OUTPUT_HIGH:
            gpio_direction_output(pin, 1);
            break;

        case GSS_OUTPUT_LOW:
            gpio_direction_output(pin, 0);
            break;

        case GSS_INPUT_PULL:
            gpio_direction_input(pin);
            gpio_enable_pull(pin);
            break;

        case GSS_INPUT_NOPULL:
            gpio_direction_input(pin);
            gpio_disable_pull(pin);
            break;

        default:
            break;
        }
    }
}

inline static void board_gpio_resume(void) {

}

void suspend_enter(int state) {
    printf("enter suspend state: 0x%x\n", state);

    board_gpio_suspend();

#ifndef CONFIG_BOOT_USB

    struct sleep_lib_entry *entry = (void *)(SLEEP_LIB_TCSM);

    switch(state) {
    case PM_SUSPEND_STANDBY:
        entry->enter_idle();
        break;

    case PM_SUSPEND_MEM:
        entry->enter_sleep(state);
        break;

    default:
        panic("unknown suspend state: 0x%x\n", state);
        break;
    }
#else
    switch(state) {
    case PM_SUSPEND_STANDBY:
        enter_idle();
        break;

    case PM_SUSPEND_MEM:
        enter_sleep(state);
        break;

    default:
        panic("unknown suspend state: 0x%x\n", state);
        break;
    }
#endif

    board_gpio_resume();

    printf("exit suspend state: 0x%x\n", state);
}
