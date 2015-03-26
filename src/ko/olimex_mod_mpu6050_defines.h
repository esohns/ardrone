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

#ifndef KO_OLIMEX_MOD_MPU6050_DEFINES_H
#define KO_OLIMEX_MOD_MPU6050_DEFINES_H

#include <linux/kernel.h>

// *NOTE*: taken from i2cdevlib (see also: http://www.i2cdevlib.com/devices/mpu6050#source)
#include "MPU6050.h"

// module
#define KO_OLIMEX_MOD_MPU6050_MODULE_LICENSE                  "GPL"
#define KO_OLIMEX_MOD_MPU6050_MODULE_AUTHOR                   "Erik Sohns"
#define KO_OLIMEX_MOD_MPU6050_MODULE_VERSION                  "0.1"
#define KO_OLIMEX_MOD_MPU6050_MODULE_DESCRIPTION              "I2C kernel module driver for the Olimex MOD-MPU6050 UEXT module"

// driver
#define KO_OLIMEX_MOD_MPU6050_DRIVER_NAME                     "olimex_mod_mpu6050"
#define KO_OLIMEX_MOD_MPU6050_DRIVER_WQ_NAME                  "olimex_mod_mpu6050_wq"

// macros
#define KO_OLIMEX_MOD_MPU6050_ARRAY_AND_SIZE (x)              (x), ARRAY_SIZE (x)

// gpio
#define KO_OLIMEX_MOD_MPU6050_GPIO_FEX_SECTION_HEADER         "gpio_para"
// *NOTE*: check the .fex file (bin2fex of script.bin) in the device boot partition
// *TODO*: these should be defined elsewhere... (check linux-sunxi development)
#define KO_OLIMEX_MOD_MPU6050_GPIO_UEXT4_PG11_PIN             10 // *NOTE*: connect UEXT to GPIO-1 port
#define KO_OLIMEX_MOD_MPU6050_GPIO_UEXT4_PG11_LABEL           "gpio_pin_10"
//#define KO_GPIO_UEXT4_PH17_PIN               30 // *NOTE*: connect UEXT to GPIO-3 port
//#define KO_GPIO_UEXT4_PH17_LABEL             "gpio_pin_30"
#define KO_OLIMEX_MOD_MPU6050_GPIO_LED_PH02_PIN               20
#define KO_OLIMEX_MOD_MPU6050_GPIO_LED_PH02_LABEL             "gpio_pin_20"
#define KO_OLIMEX_MOD_MPU6050_GPIO_INT_PIN                    KO_OLIMEX_MOD_MPU6050_GPIO_UEXT4_PG11_PIN
#define KO_OLIMEX_MOD_MPU6050_GPIO_INT_PIN_LABEL              KO_OLIMEX_MOD_MPU6050_GPIO_UEXT4_PG11_LABEL
#define KO_OLIMEX_MOD_MPU6050_GPIO_LED_PIN                    KO_OLIMEX_MOD_MPU6050_GPIO_LED_PH02_PIN
#define KO_OLIMEX_MOD_MPU6050_GPIO_LED_PIN_LABEL              KO_OLIMEX_MOD_MPU6050_GPIO_LED_PH02_LABEL

// timer
// *NOTE*: this (roughly) sets the device polling frequency (noint=1)
#define KO_OLIMEX_MOD_MPU6050_TIMER_DELAY_MS                  10 // ms

// device
#define KO_OLIMEX_MOD_MPU6050_DEVICE_RESET_DELAY_MS           100 // ms
#define KO_OLIMEX_MOD_MPU6050_DEVICE_REG_SET_DELAY_MS         5 // ms
#define KO_OLIMEX_MOD_MPU6050_DEVICE_BLOCK_LENGTH             14 // bytes
#define KO_OLIMEX_MOD_MPU6050_DEVICE_ACCEL_SENSITIVITY        MPU6050_ACCEL_FS_16
#define KO_OLIMEX_MOD_MPU6050_DEVICE_ACCEL_SENSITIVITY_FACTOR 16384 // LSB/g
#define KO_OLIMEX_MOD_MPU6050_DEVICE_THERMO_SENSITIVITY       340
#define KO_OLIMEX_MOD_MPU6050_DEVICE_THERMO_OFFSET            36.53F
#define KO_OLIMEX_MOD_MPU6050_DEVICE_GYRO_SENSITIVITY         MPU6050_GYRO_FS_2000 // +- 2000 °/s
#define KO_OLIMEX_MOD_MPU6050_DEVICE_GYRO_SENSITIVITY_FACTOR  131 // LSB/(°/s)

// ringbuffer
#define KO_OLIMEX_MOD_MPU6050_RINGBUFFER_SIZE                 64
#define KO_OLIMEX_MOD_MPU6050_RINGBUFFER_DATA_SIZE            KO_OLIMEX_MOD_MPU6050_DEVICE_BLOCK_LENGTH

// network
// *WARNING*: check <linux/netlink.h> for available identifiers !
//#define NETLINK_PROTOCOL_TYPE             NETLINK_GENERIC
#define KO_OLIMEX_MOD_MPU6050_NETLINK_PROTOCOL_FAMILY_NAME    "genl_mpu6050" // max GENL_NAMSIZ (==16) bytes
#define KO_OLIMEX_MOD_MPU6050_NETLINK_PROTOCOL_VERSION        1

#define KO_OLIMEX_MOD_MPU6050_SERVER_DEFAULT_PEER             "127.0.0.1"
#define KO_OLIMEX_MOD_MPU6050_SERVER_DEFAULT_PORT             10001
//#define SERVER_INADDR_SEND                ((unsigned long int)0x7f000001) /* 127.0.0.1: INADDR_LOOPBACK */
//#define SERVER_INADDR_SEND                INADDR_LOOPBACK

#endif // #ifndef OLIMEX_MOD_MPU6050_DEFINES_H
