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

#include "olimex_mod_mpu6050_main.h"

#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/inet.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/regmap.h>
#include <linux/slab.h>

// *NOTE*: taken from i2cdevlib (see also: http://www.i2cdevlib.com/devices/mpu6050#source)
#include "MPU6050.h"

#include "olimex_mod_mpu6050_defines.h"
#include "olimex_mod_mpu6050_device.h"
#include "olimex_mod_mpu6050_irq.h"
#include "olimex_mod_mpu6050_netlink.h"
#include "olimex_mod_mpu6050_pm.h"
#include "olimex_mod_mpu6050_server.h"
#include "olimex_mod_mpu6050_sysfs.h"
#include "olimex_mod_mpu6050_timer.h"
#include "olimex_mod_mpu6050_types.h"
#include "olimex_mod_mpu6050_wq.h"

// *** driver ***
// *NOTE*: see https://www.kernel.org/doc/Documentation/i2c/writing-clients
struct i2c_device_id i2c_mpu6050_id_table[] = {
  { KO_OLIMEX_MOD_MPU6050_DRIVER_NAME, 0 },
  { }
};
MODULE_DEVICE_TABLE(i2c, i2c_mpu6050_id_table);

struct __initdata i2c_board_info i2c_mpu6050_board_infos[] = {
  {
    I2C_BOARD_INFO(KO_OLIMEX_MOD_MPU6050_DRIVER_NAME, MPU6050_DEFAULT_ADDRESS),
//    .type          = ,
    .flags         = 0,
//    .addr          = ,
    .platform_data = NULL,
    .archdata      = NULL,
    .of_node       = NULL,
//    .irq           = gpio_to_irq(GPIO_INT_PIN),
  },
};

unsigned short normal_i2c[] = {
  MPU6050_ADDRESS_AD0_LOW,
  MPU6050_ADDRESS_AD0_HIGH,
  I2C_CLIENT_END
};
//unsigned short normal_i2c_range[] = {
//  0x00, 0xff,
//  I2C_CLIENT_END
//};
//unsigned int normal_isa[] = {
//  I2C_CLIENT_ISA_END
//};
//unsigned int normal_isa_range[] = {
//  I2C_CLIENT_ISA_END
//};

//struct device_driver i2c_mpu6050_device_driver = {
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
//struct list_head i2c_mpu6050_clients;
struct i2c_driver i2c_mpu6050_i2c_driver = {
  .class          = I2C_CLASS_HWMON,       // What kind of i2c device we instantiate (for detect)

   /* Notifies the driver that a new bus has appeared or is about to be
    * removed. You should avoid using this, it will be removed in a
    * near future.
    */
//  .attach_adapter = i2c_mpu6050_attach_adapter, // Callback for bus addition (deprecated)
//  .detach_adapter = i2c_mpu6050_detach_adapter,

    /* Standard driver model interfaces */
  .probe          = i2c_mpu6050_probe,     // Callback for device binding
  .remove         = __devexit_p(i2c_mpu6050_remove),
  /* driver model interfaces that don't relate to enumeration  */
  .shutdown       = i2c_mpu6050_shutdown,  // Callback for device shutdown
//  .suspend        = NULL,
//  .resume         = NULL,
//  .suspend        = i2c_mpu6050_suspend,
//  .resume         = i2c_mpu6050_resume,

  /* Alert callback, for example for the SMBus alert protocol.
   * The format and meaning of the data value depends on the protocol.
   * For the SMBus alert protocol, there is a single bit of data passed
   * as the alert response's low bit ("event flag").
   */
  .alert          = i2c_mpu6050_alert,     // Alert callback, for example for the SMBus alert protocol

  /* a ioctl like command that can be used to perform specific functions
   * with the device.
   */
  .command        = i2c_mpu6050_command,   // Callback for bus-wide signaling (optional)

//  .driver         = i2c_mpu6050_device_driver,
  .driver         = {                      // Device driver model driver
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
    .pm                    = &i2c_mpu6050_pm_ops,
    .p                     = NULL,
  },
  .id_table       = i2c_mpu6050_id_table,  // List of I2C devices supported by this driver

  /* Device detection callback for automatic device creation */
  .detect         = i2c_mpu6050_detect,    // Callback for device detection
  .address_list   = normal_i2c,            // The I2C addresses to probe (for detect)
//  .clients        = i2c_mpu6050_clients, // List of detected clients we created (for i2c-core use only)
};

//int
//chip_detect(struct i2c_adapter* adapter_in, int address_in, int kind_in)
//{
//  struct i2c_client* client_p;
//  struct i2c_mpu6050_client_data_t* client_data_p;
//  int err = 0;

//  client_p = kmalloc(sizeof(*client_p), GFP_KERNEL);
//  if (!client_p) {
//    pr_err("%s: failed to kmalloc\n", __FUNCTION__);
//    return -EINVAL;
//  }
//  client_data_p = kmalloc(sizeof(*client_data_p), GFP_KERNEL);
//  if (!client_data_p) {
//    pr_err("%s: failed to kmalloc\n", __FUNCTION__);

//    // clean up
//    kfree(client_p);

//    return -ENOMEM;
//  }

//  i2c_set_clientdata(client_p, client_data_p);
//  client_p->addr = address_in;
//  client_p->adapter = adapter_in;
//  client_p->driver = &i2c_mpu6050_i2c_driver;
//  client_p->flags = 0;
//  strncpy(client_p->name, KO_OLIMEX_MOD_MPU6050_DRIVER_NAME,
//          I2C_NAME_SIZE);

//  /* Tell the I2C layer a new client has arrived */
//  err = i2c_attach_client(client_p);
//  if (IS_ERR(err))
//  {
//    pr_err("%s: failed to i2c_attach_client: %d\n", __FUNCTION__, err);

//    // clean up
//    kfree(client_p);
//    kfree(client_data_p);

//    return -ENOSYS;
//  }

//  return 0;
//}

//int
//i2c_mpu6050_attach_adapter(struct i2c_adapter* adapter_in)
//{
//  pr_debug("%s() called (bus id was: %d).\n", __FUNCTION__,
//           i2c_adapter_id(adapter_in));

////  return i2c_detect(adapter_in, &addr_data,
////                    chip_detect);
//  return 0;
//}

//int
//i2c_mpu6050_detach_adapter(struct i2c_adapter* adapter_in)
//{
//  pr_debug("%s() called (bus id was: %d).\n", __FUNCTION__,
//           i2c_adapter_id(adapter_in));

//  return 0;
//}

int
__devinit i2c_mpu6050_probe(struct i2c_client* client_in,
                            const struct i2c_device_id* id_in)
{
  int err;
//  int gpio_used;
  struct i2c_mpu6050_client_data_t* client_data_p;
//  struct gpio_chip* gpio_chip_p;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely(!client_in))
  {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -EINVAL;
  }
  if (unlikely(!i2c_check_functionality(client_in->adapter,
//                                        (I2C_FUNC_SMBUS_QUICK           |
                                        (I2C_FUNC_SMBUS_BYTE_DATA       |
                                         I2C_FUNC_SMBUS_READ_WORD_DATA  |
//                                         I2C_FUNC_SMBUS_READ_BLOCK_DATA |
                                         I2C_FUNC_SMBUS_READ_I2C_BLOCK)))) {
    pr_err("%s: required i2c functionality is not supported on this adapter\n", __FUNCTION__);
    return -EIO;
  }
  if (unlikely(!gpio_is_valid(KO_OLIMEX_MOD_MPU6050_GPIO_LED_PIN))) {
    pr_err("%s: gpio_is_valid(%d) failed\n", __FUNCTION__,
           KO_OLIMEX_MOD_MPU6050_GPIO_LED_PIN);
    return -ENOSYS;
  }

  client_data_p = kzalloc(sizeof(struct i2c_mpu6050_client_data_t), GFP_KERNEL);
  if (unlikely(IS_ERR(client_data_p))) {
    pr_err("%s: kzalloc() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_data_p));
    return -ENOMEM;
  }
  //  client_p = i2c_new_probed_device(client_in->adapter,
  //                                   &i2c_mpu6050_board_infos[0],
  //                                   normal_i2c,
  //                                   NULL); // use default probing method
  //  if (unlikely(IS_ERR(client_p))) {
  //    pr_err("%s: i2c_new_probed_device() failed: %ld\n", __FUNCTION__,
  //           PTR_ERR(client_p));
  //    return -ENODEV;
  //  }
  client_data_p->client = client_in;
  client_data_p->ringbufferpos = -1;
  mutex_init(&client_data_p->sync_lock);
  i2c_set_clientdata(client_in, client_data_p);

//  err = script_parser_fetch(GPIO_FEX_SECTION_HEADER,
//                            "gpio_used",
//                            &gpio_used,
//                            sizeof(gpio_used)/sizeof(int));
//  if (unlikely(err)) {
//    pr_err("%s: script_parser_fetch(\"%s\",\"gpio_used\") failed\n", __FUNCTION__,
//           GPIO_FEX_SECTION_HEADER);
//    goto error2;
//  }
  err = script_parser_fetch(KO_OLIMEX_MOD_MPU6050_GPIO_FEX_SECTION_HEADER,
                            KO_OLIMEX_MOD_MPU6050_GPIO_LED_PIN_LABEL,
                            (int*)&client_data_p->gpio_led_data,
                            sizeof(script_gpio_set_t));
  if (unlikely(err != SCRIPT_PARSER_OK)) {
    pr_err("%s: script_parser_fetch(\"%s\",\"%s\") failed: %d\n", __FUNCTION__,
           KO_OLIMEX_MOD_MPU6050_GPIO_FEX_SECTION_HEADER,
           KO_OLIMEX_MOD_MPU6050_GPIO_LED_PIN_LABEL,
           err);
    goto error2;
  }
  client_data_p->gpio_led_handle =
      gpio_request_ex(KO_OLIMEX_MOD_MPU6050_GPIO_FEX_SECTION_HEADER,
                      KO_OLIMEX_MOD_MPU6050_GPIO_LED_PIN_LABEL);
  if (unlikely(!client_data_p->gpio_led_handle)) {
    pr_err("%s: gpio_request_ex(\"%s\") failed\n", __FUNCTION__,
           KO_OLIMEX_MOD_MPU6050_GPIO_LED_PIN_LABEL);
    err = -ENOSYS;
    goto error2;
  }
  gpio_write_one_pin_value(client_data_p->gpio_led_handle, 0, KO_OLIMEX_MOD_MPU6050_GPIO_LED_PIN_LABEL);

//  err = gpio_request(KO_OLIMEX_MOD_MPU6050_GPIO_LED_PIN,
//                     KO_OLIMEX_MOD_MPU6050_GPIO_LED_PIN_LABEL);
//  if (unlikely(err)) {
//    pr_err("%s: gpio_request(%d) failed: %d\n", __FUNCTION__,
//           KO_OLIMEX_MOD_MPU6050_GPIO_LED_PIN,
//           err);
//    goto error3;
//  }
//  err = gpio_export(KO_OLIMEX_MOD_MPU6050_GPIO_LED_PIN,
//                    false);
//  if (unlikely(err)) {
//    pr_err("%s: gpio_export(%d) failed: %d\n", __FUNCTION__,
//           KO_OLIMEX_MOD_MPU6050_GPIO_LED_PIN,
//           err);
//    goto error4;
//  }

  err = i2c_mpu6050_sysfs_init(client_data_p);
  if (unlikely(err)) {
    pr_err("%s: i2c_mpu6050_sysfs_init() failed\n", __FUNCTION__);
    goto error5;
  }

  err = i2c_mpu6050_device_ping(client_data_p);
  if (unlikely(err)) {
    pr_err("%s: i2c_mpu6050_device_ping() failed\n", __FUNCTION__);
    goto error6;
  }

  err = i2c_mpu6050_device_init(client_data_p);
  if (unlikely(err)) {
    pr_err("%s: i2c_mpu6050_device_init() failed\n", __FUNCTION__);
    goto error6;
  }

  err = i2c_mpu6050_wq_init(client_data_p);
  if (unlikely(err)) {
    pr_err("%s: i2c_mpu6050_wq_init() failed\n", __FUNCTION__);
    goto error7;
  }

  if (likely(nonetlink == 0)) {
    err = i2c_mpu6050_netlink_init(client_data_p);
    if (unlikely(err)) {
      pr_err("%s: i2c_mpu6050_netlink_init() failed\n", __FUNCTION__);
      goto error8;
    }
  }

  if (likely(noserver == 0)) {
    err = i2c_mpu6050_server_init(client_data_p);
    if (unlikely(err)) {
      pr_err("%s: i2c_mpu6050_server_init() failed\n", __FUNCTION__);
      goto error9;
    }
  }

  if (unlikely(noirq)) {
    err = i2c_mpu6050_timer_init(client_data_p);
    if (unlikely(err)) {
      pr_err("%s: i2c_mpu6050_timer_init() failed\n", __FUNCTION__);
      goto error10;
    }
  } else {
    err = i2c_mpu6050_irq_init(client_data_p);
    if (unlikely(err)) {
      pr_err("%s: i2c_mpu6050_irq_init() failed\n", __FUNCTION__);
      goto error10;
    }
  }

  // debug info
  dev_info(&client_data_p->client->dev,
           "%s initialized\n",
           KO_OLIMEX_MOD_MPU6050_DRIVER_NAME);

  return 0;

error10:
  if (likely(noserver == 0))
    i2c_mpu6050_server_fini(client_data_p);
error9:
  if (likely(nonetlink == 0))
    i2c_mpu6050_netlink_fini(client_data_p);
error8:
  i2c_mpu6050_wq_fini(client_data_p);
error7:
  i2c_mpu6050_device_fini(client_data_p);
error6:
  i2c_mpu6050_sysfs_fini(client_data_p);
error5:
//  gpio_unexport(OLIMEX_MOD_MPU6050_GPIO_LED_PIN);
//error4:
//  gpio_free(OLIMEX_MOD_MPU6050_GPIO_LED_PIN);
//error3:
  gpio_release(client_data_p->gpio_led_handle, 1);
error2:
//  i2c_unregister_device(client_data_p->client);
//error1:
  mutex_destroy(&client_data_p->sync_lock);
  kfree(client_data_p);

  return err;
}

int
i2c_mpu6050_remove(struct i2c_client* client_in)
{
  struct i2c_mpu6050_client_data_t* client_data_p;
//  struct gpio_chip* gpio_chip_p;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely(!client_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -EINVAL;
  }
  client_data_p =
   (struct i2c_mpu6050_client_data_t*)i2c_get_clientdata(client_in);
  if (unlikely(IS_ERR(client_data_p))) {
    pr_err("%s: i2c_get_clientdata() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_data_p));
    return PTR_ERR(client_data_p);
  }
//  gpio_chip_p = gpio_to_chip(GPIO_INT_PIN);
//  if (IS_ERR(gpio_chip_p)) {
//    pr_err("%s: gpio_to_chip(%d) failed: %d\n", __FUNCTION__,
//           GPIO_INT_PIN,
//           PTR_ERR(gpio_chip_p));
//    return PTR_ERR(gpio_chip_p);
//  }

  if (unlikely(noirq)) {
    i2c_mpu6050_timer_fini(client_data_p);
  } else {
    i2c_mpu6050_irq_fini(client_data_p);
  }
  if (likely(noserver == 0))
    i2c_mpu6050_server_fini(client_data_p);
  if (likely(nonetlink == 0))
    i2c_mpu6050_netlink_fini(client_data_p);
  i2c_mpu6050_wq_fini(client_data_p);
  i2c_mpu6050_device_fini(client_data_p);
  i2c_mpu6050_sysfs_fini(client_data_p);
//  gpio_unexport(GPIO_LED_PIN);
//  gpio_free(GPIO_LED_PIN);
  gpio_release(client_data_p->gpio_led_handle, 1);
//  i2c_unregister_device(client_data_p->client);
  mutex_destroy(&client_data_p->sync_lock);
  kfree(client_data_p);

  return 0;
}

void
i2c_mpu6050_shutdown(struct i2c_client* client_in)
{
  struct i2c_mpu6050_client_data_t* client_data_p;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely(!client_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return;
  }
  client_data_p = (struct i2c_mpu6050_client_data_t*)i2c_get_clientdata(client_in);
  if (unlikely(IS_ERR(client_data_p))) {
    pr_err("%s: i2c_get_clientdata() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_data_p));
    return;
  }

  i2c_mpu6050_device_reset(client_data_p, 0, 0);
}

//int
//i2c_mpu6050_suspend(struct i2c_client* client_in, pm_message_t message_in)
//{
//  pr_debug("%s called.\n", __FUNCTION__);

//  return 0;
//}

//int
//i2c_mpu6050_resume(struct i2c_client* client_in)
//{
//  pr_debug("%s called.\n", __FUNCTION__);

//  return 0;
//}

void
i2c_mpu6050_alert(struct i2c_client* client_in, unsigned int data_in)
{
  pr_debug("%s called.\n", __FUNCTION__);

}

int
i2c_mpu6050_command(struct i2c_client* client_in, unsigned int command_in, void* data_in)
{
  pr_debug("%s called.\n", __FUNCTION__);

  return 0;
}

int
__devinit i2c_mpu6050_detect(struct i2c_client* client_in, struct i2c_board_info* info_in)
{
  int err = 0;
  union i2c_smbus_data dummy;

  // sanity check(s)
  if (!client_in || !info_in) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -EINVAL;
  }

  pr_debug("%s() called.\n", __FUNCTION__);

  // *NOTE*: return 0 for supported, -ENODEV for unsupported devices

// *OPEN*: use I2C_SMBUS_QUICK instead ?
// *WARNING*: (see man i2cdetect)
//  err = i2c_smbus_xfer(client_in->adapter, info_in->addr, 0,
//                       I2C_SMBUS_WRITE, 0, I2C_SMBUS_QUICK,
//                       &dummy);
  err = i2c_smbus_xfer(client_in->adapter, info_in->addr, 0,
                       I2C_SMBUS_READ, 0, I2C_SMBUS_BYTE,
                       &dummy);
  if (err) {
    pr_debug("%s: i2c_smbus_xfer(0x%x) failed: %d\n", __FUNCTION__,
             info_in->addr,
             err);
    return -ENODEV;
  }

  strcpy(info_in->type, KO_OLIMEX_MOD_MPU6050_DRIVER_NAME);

  return 0;
}
//module_i2c_driver(i2c_mpu6050_device_driver);

// *** module ***
const struct regmap_config i2c_mpu6050_regmap_config = {
  .reg_bits = 8,
  .val_bits = 8,
  .max_register = MPU6050_RA_WHO_AM_I,
};

int noirq=0;
module_param(noirq, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(noirq, "use polling (instead of INT line (default))");
int nofifo=1;
module_param(nofifo, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(nofifo, "do not use the device FIFO buffer (default)");
int nonetlink=1;
module_param(nonetlink, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(nonetlink, "do not forward sensor data over a netlink socket");
int noserver=0;
module_param(noserver, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(noserver, "do not forward sensor data over a UDP socket");
char* peer="";
module_param(peer, charp, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(peer, "peer address (IPv4)");
short int port=KO_OLIMEX_MOD_MPU6050_SERVER_DEFAULT_PORT;
module_param(port, short, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(port, "peer port (IPv4)");

const char* default_peer = KO_OLIMEX_MOD_MPU6050_SERVER_DEFAULT_PEER;

int
__init i2c_mpu6050_init(void)
{
  int err;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (noserver == 0) {
    if (strlen (peer) == 0) peer = (char*)default_peer;
  }

//  i2c_mpu6050_board_infos[0].irq = gpio_to_irq(KO_OLIMEX_MOD_MPU6050_GPIO_INT_PIN);
//  if (unlikely(i2c_mpu6050_board_infos[0].irq < 0)) {
//    pr_err("%s: gpio_to_irq(%d) failed: %d\n", __FUNCTION__,
//           KO_OLIMEX_MOD_MPU6050_GPIO_INT_PIN,
//           err);
//    goto init_error1;
//  }
//  err = i2c_register_board_info(0,
//                                KO_OLIMEX_MOD_MPU6050_ARRAY_AND_SIZE(i2c_mpu6050_board_infos));
//  if (unlikely(err < 0)) {
//    pr_err("%s: i2c_register_board_info() failed: %d\n", __FUNCTION__,
//           err);
//    return -ENOSYS;
//  }

  INIT_LIST_HEAD(&i2c_mpu6050_i2c_driver.clients);

  // register I2C driver
  // *NOTE*: this calls i2c_mpu6050_probe()
  err = i2c_add_driver(&i2c_mpu6050_i2c_driver);
  if (unlikely(err)) {
    pr_err("%s: i2c_add_driver() failed: %d\n", __FUNCTION__,
           err);
    return -ENODEV;
  }

  pr_info("%s: %s...added", __FUNCTION__,
          KO_OLIMEX_MOD_MPU6050_DRIVER_NAME);

  return 0;

//init_error1:
//  i2c_del_driver(&i2c_mpu6050_i2c_driver);

//  return -ENOSYS;
}

void
__exit i2c_mpu6050_exit(void)
{
  pr_debug("%s called.\n", __FUNCTION__);

  i2c_del_driver(&i2c_mpu6050_i2c_driver);

  pr_info("%s: %s...removed", __FUNCTION__,
          KO_OLIMEX_MOD_MPU6050_DRIVER_NAME);
}

module_init(i2c_mpu6050_init);
module_exit(i2c_mpu6050_exit);

MODULE_LICENSE(KO_OLIMEX_MOD_MPU6050_MODULE_LICENSE);
MODULE_AUTHOR(KO_OLIMEX_MOD_MPU6050_MODULE_AUTHOR);
MODULE_VERSION(KO_OLIMEX_MOD_MPU6050_MODULE_VERSION);
MODULE_DESCRIPTION(KO_OLIMEX_MOD_MPU6050_MODULE_DESCRIPTION);
