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

// macros
#define ARRAY_AND_SIZE (x)                (x), ARRAY_SIZE (x)

// gpio
#define GPIO_FEX_SECTION_HEADER           "gpio_para"
// *NOTE*: check the .fex file (bin2fex of script.bin) in the device boot partition
// *TODO*: these should be defined elsewhere... (check linux-sunxi development)
#define GPIO_UEXT4_PG11_PIN               10 // *NOTE*: connect UEXT to GPIO-1 port
#define GPIO_UEXT4_PG11_LABEL             "gpio_pin_10"
//#define GPIO_UEXT4_PH17_PIN               30 // *NOTE*: connect UEXT to GPIO-3 port
//#define GPIO_UEXT4_PH17_LABEL             "gpio_pin_30"
#define GPIO_LED_PH02_PIN                 20
#define GPIO_LED_PH02_LABEL               "gpio_pin_20"
#define GPIO_INT_PIN                      GPIO_UEXT4_PG11_PIN
#define GPIO_INT_PIN_LABEL                GPIO_UEXT4_PG11_LABEL
#define GPIO_LED_PIN                      GPIO_LED_PH02_PIN
#define GPIO_LED_PIN_LABEL                GPIO_LED_PH02_LABEL

// timer
// *NOTE*: this (roughly) sets the device polling frequency (noint=1)
#define TIMER_DELAY_MS                    10L // ms

// device
#define RESET_DELAY_MS                    100 // ms
#define REG_SET_DELAY_MS                  5 // ms
#define BLOCK_LENGTH                      14 // bytes
#define ACCEL_SENSITIVITY                 16384 // LSB/g
#define THERMO_SENSITIVITY                340
#define THERMO_OFFSET                     36.53F
#define GYRO_SENSITIVITY                  131 // LSB/(°/s)

// ringbuffer
#define RINGBUFFER_SIZE                   64
#define RINGBUFFER_DATA_SIZE              BLOCK_LENGTH

// network
// *WARNING*: check <linux/netlink.h> for available identifiers !
//#define NETLINK_PROTOCOL_TYPE             NETLINK_GENERIC
#define NETLINK_PROTOCOL_FAMILY_NAME      "genl_mpu6050" // max GENL_NAMSIZ (==16) bytes
#define NETLINK_PROTOCOL_VERSION          1

#define SERVER_DEFAULT_PEER               "127.0.0.1"
#define SERVER_DEFAULT_PORT               10001
//#define SERVER_INADDR_SEND                ((unsigned long int)0x7f000001) /* 127.0.0.1: INADDR_LOOPBACK */
//#define SERVER_INADDR_SEND                INADDR_LOOPBACK

#endif // #ifndef OLIMEX_MOD_MPU6050_DEFINES_H
