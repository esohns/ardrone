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

#include <linux/kobject.h>

#include "olimex_mod_mpu6050_types.h"

// function declarations
static ssize_t i2c_mpu6050_store_store(struct kobject*, struct kobj_attribute*, const char*, size_t);
static ssize_t i2c_mpu6050_store_show(struct kobject*, struct kobj_attribute*, char*);

// register access
static ssize_t i2c_mpu6050_reg_store(struct kobject*, struct kobj_attribute*, const char*, size_t);
static ssize_t i2c_mpu6050_reg_show(struct kobject*, struct kobj_attribute*, char*);

// ringbuffer
static void i2c_mpu6050_clearringbuffer(void*);
static ssize_t i2c_mpu6050_clearringbuffer_store(struct kobject*, struct kobj_attribute*, const char*, size_t);

// INT / LED
static ssize_t i2c_mpu6050_intstate_show(struct kobject*, struct kobj_attribute*, char*);
static ssize_t i2c_mpu6050_ledstate_show(struct kobject*, struct kobj_attribute*, char*);

static struct kobj_attribute store_attribute =           __ATTR(data, 0666, i2c_mpu6050_store_show, i2c_mpu6050_store_store);
static struct kobj_attribute reg_attribute =             __ATTR(addr, 0666, i2c_mpu6050_reg_show, i2c_mpu6050_reg_store);
static struct kobj_attribute clearringbuffer_attribute = __ATTR(clear_ringbuffer, 0666, NULL, i2c_mpu6050_clearringbuffer_store);
static struct kobj_attribute intstate_attribute =        __ATTR(int_state, 0666, i2c_mpu6050_intstate_show, NULL);
static struct kobj_attribute ledstate_attribute =        __ATTR(led_state, 0666, i2c_mpu6050_ledstate_show, NULL);
/* *NOTE*: use a group of attributes so that the kernel can create and destroy
 *         them all at once
 */
static struct attribute* i2c_mpu6050_attrs[] = {
  &store_attribute.attr,
  &reg_attribute.attr,
  &clearringbuffer_attribute.attr,
  &intstate_attribute.attr,
  &ledstate_attribute.attr,
  NULL, // need to NULL terminate the list of attributes
};
//ATTRIBUTE_GROUPS(i2c_mpu6050);
static const struct attribute_group i2c_mpu6050_group = {
  .attrs = i2c_mpu6050_attrs,
};
static const struct attribute_group* i2c_mpu6050_groups[] = {
  &i2c_mpu6050_group,
  NULL, // need to NULL terminate the list of attribute groups
};

static bool i2c_mpu6050_sysfs_init(struct i2c_mpu6050_client_data_t*);
static void i2c_mpu6050_sysfs_fini(struct i2c_mpu6050_client_data_t*);
