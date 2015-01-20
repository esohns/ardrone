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

#ifndef OLIMEX_MOD_MPU6050_DEFINES_H
#define OLIMEX_MOD_MPU6050_DEFINES_H

// module
#define KO_OLIMEX_MOD_MPU6050_LICENSE     "GPL"
#define KO_OLIMEX_MOD_MPU6050_AUTHOR      "Erik Sohns"
#define KO_OLIMEX_MOD_MPU6050_VERSION     "0.1"
#define KO_OLIMEX_MOD_MPU6050_DESCRIPTION "I2C kernel module driver for the Olimex MOD-MPU6050 UEXT module"

// driver
#define KO_OLIMEX_MOD_MPU6050_DRIVER_NAME "olimex_mod_mpu6050"
#define KO_OLIMEX_MOD_MPU6050_WQ_NAME     "olimex_mod_mpu6050_wq"

// defaults
#define BUFSIZ                            256

// macros
#define ARRAY_AND_SIZE(x)	                (x), ARRAY_SIZE(x)

// gpio
// *NOTE*: check the .fex file (bin2fex of script.bin) in the device boot partition
#define GPIO_UEXT4_UART4RX_PG11_PIN       10
#define GPIO_UEXT4_UART4RX_PG11_LABEL     "gpio_pin_10"
#define GPIO_LED_PH02_PIN                 20
#define GPIO_LED_PH02_LABEL               "gpio_pin_20"

// fifo
#define FIFOSTORESIZE                     20
#define FIFOSTOREDATASIZE                 64
#define RINGBUFFERSIZE                    20
#define RINGBUFFERDATASIZE                64

#endif // #ifndef OLIMEX_MOD_MPU6050_DEFINES_H
