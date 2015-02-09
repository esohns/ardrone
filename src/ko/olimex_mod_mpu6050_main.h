/*  I2C kernel module driver for the Olimex MOD-MPU6050 UEXT module
    (see https://www.olimex.com/Products/Modules/Sensors/MOD-MPU6050/open-source-hardware,
         http://www.invensense.com/mems/gyro/mpu6050.html)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef OLIMEX_MOD_MPU6050_MAIN_H
#define OLIMEX_MOD_MPU6050_MAIN_H

#include <linux/i2c.h>
#include <linux/pm.h>

// forward declarations
struct device_driver;
struct regmap_config;

// driver
extern struct i2c_device_id i2c_mpu6050_id_table[];
extern struct i2c_board_info i2c_mpu6050_board_infos[];
extern unsigned short normal_i2c[];
//extern short normal_i2c_range[];
//extern unsigned int normal_isa[];
//extern unsigned int normal_isa_range[];
//extern struct list_head i2c_mpu6050_clients;
//extern struct device_driver i2c_mpu6050_device_driver;
extern struct i2c_driver i2c_mpu6050_i2c_driver;

//int i2c_mpu6050_attach_adapter(struct i2c_adapter*);
//int i2c_mpu6050_detach_adapter(struct i2c_adapter*);

int i2c_mpu6050_probe(struct i2c_client*, const struct i2c_device_id*);
int i2c_mpu6050_remove(struct i2c_client*);
void i2c_mpu6050_shutdown(struct i2c_client*);
//int i2c_mpu6050_suspend(struct i2c_client*, pm_message_t);
//int i2c_mpu6050_resume(struct i2c_client*);

void i2c_mpu6050_alert(struct i2c_client*, unsigned int);

int i2c_mpu6050_command(struct i2c_client*, unsigned int, void*);

int i2c_mpu6050_detect(struct i2c_client*, struct i2c_board_info*);

//static int chip_detect(struct i2c_adapter*, int, int);

// module
extern const struct regmap_config i2c_mpu6050_regmap_config;
// parameters
extern int noirq;
extern int nofifo;

int i2c_mpu6050_init(void);
void i2c_mpu6050_exit(void);

#endif // #ifndef OLIMEX_MOD_MPU6050_MAIN_H
