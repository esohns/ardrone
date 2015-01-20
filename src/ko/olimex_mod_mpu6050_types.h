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

#ifndef OLIMEX_MOD_MPU6050_TYPES_H
#define OLIMEX_MOD_MPU6050_TYPES_H

#include <linux/i2c.h>
#include <linux/kobject.h>
#include <linux/pinctrl/consumer.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>

#include <plat/sys_config.h>

#include "olimex_mod_mpu6050_defines.h"

struct fifostoreentry_t {
  uint8_t data[FIFOSTOREDATASIZE];
  int size;
};

struct ringbufferentry_t {
  int used;
  int completed;
  uint8_t data[RINGBUFFERDATASIZE];
  int size;
};

struct read_work_t {
  struct work_struct work;
  struct i2c_client* client;
};
struct fifo_work_t {
  struct work_struct work;
  struct i2c_client* client;
};

struct i2c_mpu6050_client_data_t {
  struct pinctrl* pin_ctrl;
  struct pinctrl_state* pin_ctrl_state;
  script_gpio_set_t gpio_int_data;
  script_gpio_set_t gpio_led_data;
  unsigned gpio_led_handle;
  struct i2c_client* client;
  struct kobject* object; // used for the sysfs entries
  struct workqueue_struct* workqueue;
  struct fifo_work_t work_processfifostore;
  struct read_work_t work_read;
  struct fifostoreentry_t fifostore[FIFOSTORESIZE];
  int fifostorepos;
  struct ringbufferentry_t ringbuffer[RINGBUFFERSIZE];
  int ringbufferpos;
  spinlock_t sync_lock; // disable interrupts while i2c_sync() is running
  unsigned long sync_lock_flags;
};

#endif // #ifndef OLIMEX_MOD_MPU6050_TYPES_H
