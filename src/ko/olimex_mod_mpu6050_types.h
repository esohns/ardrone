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

#include <linux/hrtimer.h>
#include <linux/kobject.h>
#include <linux/mutex.h>
#include <linux/pinctrl/consumer.h>

// *TODO*: implement devicetree support
#include <plat/sys_config.h>

#include "olimex_mod_mpu6050_defines.h"
#include "olimex_mod_mpu6050_wq.h"

// forward declarations
struct i2c_client;
struct workqueue_struct;
struct i2c_mpu6050_server_t;
struct i2c_mpu6050_netlink_server_t;

struct i2c_mpu6050_client_data_t {
  struct hrtimer hr_timer;
  struct pinctrl* pin_ctrl;
//  struct pinctrl_state* pin_ctrl_state;
//  script_gpio_set_t gpio_int_data;
  script_gpio_set_t gpio_led_data;
  unsigned gpio_int_handle;
  unsigned gpio_led_handle;
  struct i2c_client* client;
//  struct kobject* sysfs_object;
  struct workqueue_struct* workqueue;
  struct i2c_mpu6050_wq_read_work_t work_read;
  struct i2c_mpu6050_wq_ringbuffer_entry_t ringbuffer[KO_OLIMEX_MOD_MPU6050_RINGBUFFER_SIZE];
  int ringbufferpos;
  struct mutex sync_lock;
  struct i2c_mpu6050_netlink_server_t* netlink_server;
  struct i2c_mpu6050_server_t* server;
};

#endif // #ifndef OLIMEX_MOD_MPU6050_TYPES_H
