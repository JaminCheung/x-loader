/*
 * (C) Copyright 2001, 2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 * This has been changed substantially by Gerald Van Baren, Custom IDEAS,
 * vanbaren@cideas.com.  It was heavily influenced by LiMon, written by
 * Neil Russell.
 */

#ifndef I2C_H
#define I2C_H

struct i2c {
    unsigned int scl;
    unsigned int sda;
};

void i2c_init(struct i2c *i2c);
int  i2c_write(struct i2c *i2c, uint8_t chip, uint32_t addr, uint32_t alen,
        uint8_t *buffer, uint32_t len);
int  i2c_read(struct i2c *i2c,uint8_t chip, uint32_t addr, uint32_t alen,
        uint8_t *buffer, uint32_t len);
int i2c_probe(struct i2c *i2c, uint8_t addr);

#endif /* I2C_H */
