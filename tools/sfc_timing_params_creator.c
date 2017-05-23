#include <common.h>
#include <string.h>
#include <assert.h>

#ifdef printf
#undef printf
#endif
#include <stdio.h>

static void file_head_print(void)
{
    printf("/*\n");
    printf(" * DO NOT MODIFY.\n");
    printf(" *\n");
    printf(" * This file was generated by sfc_timing_params_creator\n");
    printf(" *\n");
    printf(" */\n");
    printf("\n");

    printf("#ifndef __SFC_TIMING_VAL_H__\n");
    printf("#define __SFC_TIMING_VAL_H__\n\n");
}

static void file_end_print(void)
{
    printf("\n#endif /* __SFC_TIMING_VAL_H__ */\n");
}

int main(int argc, char* argv[]) {
    unsigned int c_hold;
    unsigned int c_setup;
    unsigned int t_in, c_in, val = 0;
    unsigned long cycle;
    unsigned int tmp = 0x7;
    unsigned int rate =  CONFIG_SFC_FREQ;

    cycle = 1000 / rate;

    c_hold = DEF_TCHSH / cycle;
    if(c_hold > 0)
        val = c_hold - 1;
    tmp &= ~THOLD_MSK;
    tmp |= val << THOLD_OFFSET;

    c_setup = DEF_TSLCH / cycle;
    if(c_setup > 0)
        val = c_setup - 1;
    tmp &= ~TSETUP_MSK;
    tmp |= val << TSETUP_OFFSET;

    if (DEF_TSHSL_R >= DEF_TSHSL_W)
        t_in = DEF_TSHSL_R;
    else
        t_in = DEF_TSHSL_W;
    c_in = t_in / cycle;
    if(c_in > 0)
        val = c_in - 1;
    tmp &= ~TSH_MSK;
    tmp |= val << TSH_OFFSET;

    if(rate >= 100){
        val = 1;
        tmp &= ~SMP_DELAY_MSK;
        tmp |= val << SMP_DELAY_OFFSET;
    }

    file_head_print();
    printf("#define DEF_TIM_VAL 0x%08x\n", tmp);
    file_end_print();

    return 0;
}