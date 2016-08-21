#include <common.h>

static inline int spinand_bad_block_check(int len,unsigned char *buf)
{
    int i,bit0_cnt = 0;
    unsigned short *check_buf = (unsigned short *)buf;

    if(check_buf[0] != 0xff){
        for(i = 0; i < len * 8; i++){
            if(!((check_buf[0] >> 1) & 0x1))
                bit0_cnt++;
        }
    }
    if(bit0_cnt > 6 * len)
        return 1;
    return 0;
}

static int spinand_read_page(unsigned int page, unsigned char *dst_addr,
        int pagesize, int blksize)
{
    unsigned int read_buf;
    int column = 0;
    int oob_flag = 0;
    struct jz_sfc sfc;

    read_oob:
    if (oob_flag){
        column = pagesize;
        pagesize = 4;
        dst_addr = (unsigned char *)&read_buf;
    }

    SFC_SEND_COMMAND(&sfc,CMD_PARD,0,page,3,0,0,0);

    SFC_SEND_COMMAND(&sfc,CMD_GET_FEATURE,1,FEATURE_REG_STATUS1,1,0,1,0);
    sfc_read_data(&read_buf,1);
    while((read_buf & 0x1))
    {
        SFC_SEND_COMMAND(&sfc,CMD_GET_FEATURE,1,FEATURE_REG_STATUS1,1,0,1,0);
        sfc_read_data(&read_buf,1);
    }
    if(read_buf & 0x20)
    {
        printf("ecc error at page%d\n", page);
        return -1;
    }

    column = (column << 8) & 0xffffff00;
    SFC_SEND_COMMAND(&sfc,CMD_R_CACHE,pagesize,column,3,0,1,0);
    sfc_read_data((unsigned int *)dst_addr,pagesize);

    if (!oob_flag && !(page % (blksize/pagesize))){
        oob_flag = 1;
        goto read_oob;
    }else if (oob_flag){
        if (spinand_bad_block_check(2, (unsigned char *)&read_buf))
            return 1;
    }
    return 0;
}

static inline void spinand_get_base_param(int *pagesize, int *blocksize)
{
    unsigned char *spl_flag = (unsigned char *)0xF4001000;
    int type_len = 6;

    *pagesize = spl_flag[type_len + 5] * 1024;//pagesize off 5,blocksize off 4
    *blocksize = 128 * 1024;
}

static void spinand_init(void) {

    unsigned char id[4];
    unsigned int x=0;
    struct jz_sfc sfc;

    /* get id */
    SFC_SEND_COMMAND(&sfc,CMD_RDID,2,0,1,0,1,0);
    sfc_read_data((unsigned int *)id,2);
    /* disable write protect */
    SFC_SEND_COMMAND(&sfc,CMD_SET_FEATURE,1,FEATURE_REG_PROTECT,1,0,1,1);
    sfc_write_data(&x,1);

    /* enable ecc */
    x = BITS_ECC_EN;
    if (id[0] == MANUFACTURE_WINBOND_ID)
        x |= (1 << 3);  // BUF equaled 1
    SFC_SEND_COMMAND(&sfc,CMD_SET_FEATURE,1,FEATURE_REG_FEATURE1,1,0,1,1);
    sfc_write_data(&x,1);
}

int spinand_load(unsigned int src_addr, unsigned int count, unsigned int dst_addr)
{
    int blksize, pagesize, page;
    int pagecopy_cnt = 0;
    unsigned int ret;
    unsigned char *buf = (unsigned char *)dst_addr;

    sfc_init();
    spinand_init();

    spinand_get_base_param(&pagesize, &blksize);
    page = src_addr / pagesize;
    while (pagecopy_cnt * pagesize < count) {
        ret = spinand_read_page(page, buf, pagesize, blksize);
        if (ret > 0){
            printf("bad block %d\n", page/(blksize/pagesize));
            page += blksize/pagesize;
            continue;
        }
        buf += pagesize;
        page++;
        pagecopy_cnt++;
    }

    return 0;
}

