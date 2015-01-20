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

#include <linux/workqueue.h>

#include "olimex_mod_mpu6050_types.h"

// function declarations
static void i2c_mpu6050_wq_fifo_handler(struct work_struct*);
static void i2c_mpu6050_wq_read_handler(struct work_struct*);

static bool i2c_mpu6050_wq_init(struct i2c_mpu6050_client_data_t*);
static void i2c_mpu6050_wq_fini(struct i2c_mpu6050_client_data_t*);
