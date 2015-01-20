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

#ifndef OLIMEX_MOD_MPU6050_SYSFS_H
#define OLIMEX_MOD_MPU6050_SYSFS_H

#include <linux/types.h>

// forward declarations
struct kobject;
struct kobj_attribute;
struct attribute;
struct i2c_mpu6050_client_data_t;

extern struct kobj_attribute store_attribute;
extern struct kobj_attribute reg_attribute;
extern struct kobj_attribute clearringbuffer_attribute;
extern struct kobj_attribute intstate_attribute;
extern struct kobj_attribute ledstate_attribute;
extern struct attribute* i2c_mpu6050_attrs[];
extern const struct attribute_group i2c_mpu6050_group;
extern const struct attribute_group* i2c_mpu6050_groups[];

// function declarations
ssize_t i2c_mpu6050_store_store(struct kobject*, struct kobj_attribute*, const char*, size_t);
ssize_t i2c_mpu6050_store_show(struct kobject*, struct kobj_attribute*, char*);

// register access
ssize_t i2c_mpu6050_reg_store(struct kobject*, struct kobj_attribute*, const char*, size_t);
ssize_t i2c_mpu6050_reg_show(struct kobject*, struct kobj_attribute*, char*);

// ringbuffer
void i2c_mpu6050_clearringbuffer(void*);
ssize_t i2c_mpu6050_clearringbuffer_store(struct kobject*, struct kobj_attribute*, const char*, size_t);

// INT / LED
ssize_t i2c_mpu6050_intstate_show(struct kobject*, struct kobj_attribute*, char*);
ssize_t i2c_mpu6050_ledstate_show(struct kobject*, struct kobj_attribute*, char*);

int i2c_mpu6050_sysfs_init(struct i2c_mpu6050_client_data_t*);
void i2c_mpu6050_sysfs_fini(struct i2c_mpu6050_client_data_t*);

#endif // #ifndef OLIMEX_MOD_MPU6050_SYSFS_H
