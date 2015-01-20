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

#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/pm.h>

// *NOTE*: taken from i2cdevlib (see also: http://www.i2cdevlib.com/devices/mpu6050#source)
#include "MPU6050.h"

#include "olimex_mod_mpu6050_defines.h"

// function declarations
// irq handlers
static irqreturn_t i2c_mpu6050_interrupt_handler(int, void*);

// driver
// *NOTE*: see https://www.kernel.org/doc/Documentation/i2c/writing-clients
static struct i2c_device_id i2c_mpu6050_id_table[] = {
  { KO_OLIMEX_MOD_MPU6050_DRIVER_NAME, 0 },
  { }
};
MODULE_DEVICE_TABLE(i2c, i2c_mpu6050_id_table);

static struct __initdata i2c_board_info i2c_mpu6050_board_infos[] = {
  {
    I2C_BOARD_INFO(KO_OLIMEX_MOD_MPU6050_DRIVER_NAME, MPU6050_DEFAULT_ADDRESS),
//    .type          = ,
    .flags         = 0,
//    .addr          = ,
    .platform_data = NULL,
    .archdata      = NULL,
    .of_node       = NULL,
//    .irq           = gpio_to_irq(GPIO_UEXT4_UART4RX_PG11_PIN),
  },
};
static unsigned short normal_i2c[] = {
  MPU6050_ADDRESS_AD0_LOW,
  MPU6050_ADDRESS_AD0_HIGH,
  I2C_CLIENT_END
};
//static unsigned short normal_i2c_range[] = {
//  0x00, 0xff,
//  I2C_CLIENT_END
//};
//static unsigned int normal_isa[] = {
//  I2C_CLIENT_ISA_END
//};
//static unsigned int normal_isa_range[] = {
//  I2C_CLIENT_ISA_END
//};

//static struct device_driver i2c_mpu6050_device_driver = {
//  .name	= KO_OLIMEX_MOD_MPU6050_DRIVER, // be sure to match this with any aliased 'i2c_board_info's
////    .bus = ,
//  .owner = THIS_MODULE,
////    .mod_name = ,
////    .suppress_bind_attrs = ,
////    .of_match_table = ,
////    .acpi_match_table = ,
////    .probe = ,
////    .remove = ,
////    .shutdown = ,
////    .suspend = ,
////    .resume = ,
//  .groups = &attr_group,
//  .pm	= &i2c_mpu6050_pm_ops,
////    .p = ,
//};

static int i2c_mpu6050_attach_adapter(struct i2c_adapter*);
static int i2c_mpu6050_detach_adapter(struct i2c_adapter*);

static int i2c_mpu6050_probe(struct i2c_client*, const struct i2c_device_id*);
static int i2c_mpu6050_remove(struct i2c_client*);
static void i2c_mpu6050_shutdown(struct i2c_client*);
static int i2c_mpu6050_suspend(struct i2c_client*, pm_message_t);
static int i2c_mpu6050_resume(struct i2c_client*);

static void i2c_mpu6050_alert(struct i2c_client*, unsigned int);

static int i2c_mpu6050_command(struct i2c_client*, unsigned int, void*);

static int i2c_mpu6050_detect(struct i2c_client*, struct i2c_board_info*);

//static struct list_head i2c_mpu6050_clients;
static struct i2c_driver i2c_mpu6050_i2c_driver = {
  .class          = I2C_CLASS_HWMON,

   /* Notifies the driver that a new bus has appeared or is about to be
    * removed. You should avoid using this, it will be removed in a
    * near future.
    */
  .attach_adapter = i2c_mpu6050_attach_adapter,
  .detach_adapter = i2c_mpu6050_detach_adapter,

    /* Standard driver model interfaces */
  .probe          = i2c_mpu6050_probe,
  .remove         = __devexit_p(i2c_mpu6050_remove),

  /* driver model interfaces that don't relate to enumeration  */
  .shutdown       = i2c_mpu6050_shutdown,
  .suspend        = i2c_mpu6050_suspend,
  .resume         = i2c_mpu6050_resume,

  /* Alert callback, for example for the SMBus alert protocol.
   * The format and meaning of the data value depends on the protocol.
   * For the SMBus alert protocol, there is a single bit of data passed
   * as the alert response's low bit ("event flag").
   */
  .alert          = i2c_mpu6050_alert,

  /* a ioctl like command that can be used to perform specific functions
   * with the device.
   */
  .command        = i2c_mpu6050_command,

//  .driver         = i2c_mpu6050_device_driver,
  .driver         = {
    // *NOTE*: be sure to match this with any aliased 'i2c_board_info's
    .name                = KO_OLIMEX_MOD_MPU6050_DRIVER_NAME,
    .bus                 = NULL,
    .owner               = THIS_MODULE,
    .mod_name            = NULL,
    .suppress_bind_attrs = false,
    .of_match_table      = NULL,
//    .acpi_match_table    = NULL,
//    .probe               = ,
//    .remove              = ,
//    .shutdown            = ,
//    .suspend             = ,
//    .resume              = ,
    .groups                = i2c_mpu6050_groups,
    .pm	                   = &i2c_mpu6050_pm_ops,
    .p                     = NULL,
  },
  .id_table       = i2c_mpu6050_id_table,

  /* Device detection callback for automatic device creation */
  .detect         = i2c_mpu6050_detect,
  .address_list   = normal_i2c,
//  .clients        = i2c_mpu6050_clients,
};

//static int chip_detect(struct i2c_adapter*, int, int);

// module
static const struct regmap_config i2c_mpu6050_regmap_config = {
  .reg_bits = 8,
  .val_bits = 8,
  .max_register = MPU6050_RA_WHO_AM_I,
};

static int i2c_mpu6050_init(void);
static void i2c_mpu6050_exit(void);

MODULE_LICENSE(KO_OLIMEX_MOD_MPU6050_LICENSE);
MODULE_AUTHOR(KO_OLIMEX_MOD_MPU6050_AUTHOR);
MODULE_VERSION(KO_OLIMEX_MOD_MPU6050_VERSION);
MODULE_DESCRIPTION(KO_OLIMEX_MOD_MPU6050_DESCRIPTION);
