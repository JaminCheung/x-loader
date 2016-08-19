#include <common.h>

extern unsigned int sfc_quad_mode;
extern int mode;

void spinor_set_quad_mode(void)
{
    unsigned int buf;
    unsigned int tmp;
    int i = 10;
    struct jz_sfc sfc;

    SFC_SEND_COMMAND(&sfc,CMD_WREN,0,0,0,0,0,1);
    SFC_SEND_COMMAND(&sfc,CMD_WRSR_1,1,0,0,0,1,1);
    tmp = 0x02;
    sfc_write_data(&tmp,1);

    SFC_SEND_COMMAND(&sfc,CMD_RDSR,1,0,0,0,1,0);
    sfc_read_data(&tmp, 1);
    while(tmp & CMD_SR_WIP) {
        SFC_SEND_COMMAND(&sfc,CMD_RDSR,1,0,0,0,1,0);
        sfc_read_data(&tmp, 1);
    }

    SFC_SEND_COMMAND(&sfc,CMD_RDSR_1,1,0,0,0,1,0);
    sfc_read_data(&buf, 1);
    while(!(buf & 0x2)&&((i--) > 0)) {
        SFC_SEND_COMMAND(&sfc,CMD_RDSR_1,1,0,0,0,1,0);
        sfc_read_data(&buf, 1);
    }
}

static int spinor_read(unsigned int offset, unsigned int len, void *data)
{
    int addr_len = 3;
    int dummy_byte = 8;
    struct jz_sfc sfc;
    int ret;

    if(sfc_quad_mode == 1){
        mode = TRAN_SPI_QUAD;
    } else{
        mode = TRAN_SPI_STANDARD;
    }

    if(sfc_quad_mode == 1){
        SFC_SEND_COMMAND(&sfc,CMD_QUAD_READ,len,offset,addr_len,dummy_byte,1,0);
    }else{
        SFC_SEND_COMMAND(&sfc,CMD_READ,len,offset,addr_len,0,1,0);
    }

    ret = sfc_read_data(data, len);
    if (ret)
        return ret;
    else
        return 0;
}

static void spinor_init(void) {
    if(sfc_quad_mode == 1)
        spinor_set_quad_mode();
}

void spinor_load(unsigned int src_addr, unsigned int count, unsigned int dst_addr)
{
    int ret = 0;

    sfc_init();
    spinor_init();

    ret = spinor_read(src_addr, count, (void *)dst_addr);
    if (ret) {
        printf("sfc load error\n");
    }
}
