#ifndef SPIFLASH_H
#define SPIFLASH_H

#define CMD_WREN                    0x06    /* Write Enable */
#define CMD_WRDI                    0x04    /* Write Disable */
#define CMD_RDSR                    0x05    /* Read Status Register */
#define CMD_RDSR_1                  0x35    /* Read Status1 Register */
#define CMD_RDSR_2                  0x15    /* Read Status2 Register */
#define CMD_WRSR                    0x01    /* Write Status Register */
#define CMD_WRSR_1                  0x31    /* Write Status1 Register */
#define CMD_WRSR_2                  0x11    /* Write Status2 Register */
#define CMD_READ                    0x03    /* Read Data */
#define CMD_FAST_READ               0x0B    /* Read Data at high speed */
#define CMD_DUAL_READ               0x3b    /* Read Data at QUAD fast speed*/
#define CMD_QUAD_READ               0x6b    /* Read Data at QUAD fast speed*/
#define CMD_QUAD_IO_FAST_READ       0xeb    /* Read Data at QUAD IO fast speed*/
#define CMD_PP                      0x02    /* Page Program(write data) */
#define CMD_QPP                     0x32    /* QUAD Page Program(write data) */
#define CMD_SE                      0xD8    /* Sector Erase */
#define CMD_BE                      0xC7    /* Bulk or Chip Erase */
#define CMD_DP                      0xB9    /* Deep Power-Down */
#define CMD_RES                     0xAB    /* Release from Power-Down and Read Electronic Signature */
#define CMD_RDID                    0x9F    /* Read Identification */
#define CMD_SR_WIP                  (1 << 0)
#define CMD_SR_QEP                  (1 << 1)
#define CMD_ERASE_4K                0x20
#define CMD_ERASE_32K               0x52
#define CMD_ERASE_64K               0xd8
#define CMD_ERASE_CE                0x60
#define CMD_EN4B                    0xB7
#define CMD_EX4B                    0xE9
#define CMD_READ4                   0x13    /* Read Data */
#define CMD_FAST_READ4              0x0C    /* Read Data at high speed */
#define CMD_PP_4B                   0x12    /* Page Program(write data) */
#define CMD_ERASE_4K_4B             0x21
#define CMD_ERASE_32K_4B            0x5C
#define CMD_ERASE_64K_4B            0xDC

#define CMD_PARD                    0x13    /* page read */
#define CMD_PE                      0x10    /* program execute*/
#define CMD_PRO_LOAD                0x02    /* program load */
#define CMD_PRO_RDM                 0x84    /* program random */
#define CMD_ERASE_128K              0xd8
#define CMD_R_CACHE                 0x03    /* read from cache */
#define CMD_FR_CACHE                0x0b    /* fast read from cache */
#define CMD_FR_CACHE_QUAD           0x6b
#define CMD_GET_FEATURE             0x0f
#define CMD_SET_FEATURE             0x1f

#define FEATURE_REG_PROTECT         0xa0
#define FEATURE_REG_FEATURE1        0xb0
#define FEATURE_REG_STATUS1         0xc0
#define FEATURE_REG_FEATURE2        0xd0
#define FEATURE_REG_STATUS2         0xf0
#define BITS_ECC_EN                 (1 << 4)
#define BITS_QUAD_EN                (1 << 0)

/* some manufacture with unusual method */
#define MANUFACTURE_WINBOND_ID      0xef
#define BITS_BUF_EN                 (1 << 3)

enum {
    VALUE_SET,
    VALUE_CLR,
};

#define OPERAND_CONTROL(action, val, ret) do{\
    if (action == VALUE_SET)\
        ret |= val;\
    else\
        ret &= ~val;\
}while(0)

struct spiflash_register {
    unsigned int addr;
    unsigned int val;
    unsigned char action;
};

struct spiflash_desc {
    unsigned int id;
    struct spiflash_register regs;
};

int spinor_load(unsigned int src_addr, unsigned int count, unsigned int dst_addr);
int spinand_load(unsigned int src_addr, unsigned int count, unsigned int dst_addr);
#ifdef CONFIG_BEIJING_OTA
int ota_load(unsigned int *argv, unsigned int dst_addr);
#endif

#endif /* SPIFLASH_H */
