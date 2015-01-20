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

#include "olimex_mod_mpu6050_sysfs.h"

//#include <linux/device.h>
//#include <linux/kernel.h>
//#include <linux/printk.h>
//#include <linux/sched.h>
//#include <linux/slab.h>
//#include <linux/sysfs.h>

//#include "olimex_mod_mpu6050_types.h"

static
ssize_t
i2c_mpu6050_store_store(struct kobject* kobj_in,
                        struct kobj_attribute* attr_in,
                        const char* buf_in,
                        size_t count_in)
{
  struct i2c_mpu6050_client_data_t* client_data_p = NULL;

  // sanity check(s)
//  struct device* device_p = kobj_to_dev(kobj_in);
////  struct kobj_type* ktype = get_ktype(kobj_in);
////  if (ktype == &device_ktype)
////    device_p = to_dev(kobj_in);
//  if (!device_p) {
//    printk(KERN_ERR "%s: invalid parameter (not a device ?)\n", __func__);
//    return -ENODEV;
//  }
//  struct i2c_client* client_p = to_i2c_client(device_p);
  struct i2c_client* client_p = kobj_to_i2c_client(kobj_in);
  if (!client_p) {
    printk(KERN_ERR "%s: invalid parameter (not a I2C device ?)\n", __func__);
    return -ENODEV;
  }
  client_data_p = (struct i2c_mpu6050_client_data_t*)i2c_get_clientdata(client_p);
  if (!client_data_p) {
    printk(KERN_ERR "unable to retrieve client state\n");
    return -ENODEV;
  }
  if (count_in > FIFOSTOREDATASIZE) {
    printk(KERN_ERR "can't store data because it's too big.");
    return -ENODEV;
  }
  if (client_data_p->fifostorepos >= FIFOSTORESIZE) {
    printk(KERN_ERR "can't store data because FIFO is full.");
    return -ENODEV;
  }
  printk(KERN_DEBUG "store_store(): storing %d bytes to store pos 0x%.2x\n", (int)count_in, client_data_p->fifostorepos);

  memcpy(client_data_p->fifostore[client_data_p->fifostorepos].data, buf_in, count_in);
  client_data_p->fifostore[client_data_p->fifostorepos].size = count_in;
  client_data_p->fifostorepos++;

  printk(KERN_DEBUG "queueing work PROCESSFIFOSTORE\n");
  queue_work(client_data_p->workqueue, &client_data_p->work_processfifostore.work);

  return count_in;
}

static
ssize_t
i2c_mpu6050_store_show(struct kobject* kobj_in,
                       struct kobj_attribute* attr_in,
                       char* buf_in)
{
  struct i2c_mpu6050_client_data_t* client_data_p = NULL;
  int i, currentbufsize = 0;

  // sanity check(s)
//  struct device* device_p = NULL;
//  struct kobj_type* ktype = get_ktype(kobj_in);
//  if (ktype == &device_ktype)
//    device_p = to_dev(kobj_in);
//  if (!device_p) {
//    printk(KERN_ERR "%s: invalid parameter (not a device ?)\n", __func__);
//    return -ENODEV;
//  }
//  struct i2c_client* client_p = to_i2c_client(device_p);
  struct i2c_client* client_p = kobj_to_i2c_client(kobj_in);
  if (!client_p) {
    printk(KERN_ERR "%s: invalid parameter (not a I2C device ?)\n", __func__);
    return -ENODEV;
  }
  client_data_p = (struct i2c_mpu6050_client_data_t*)i2c_get_clientdata(client_p);
  if (!client_data_p) {
    printk(KERN_ERR "unable to retrieve client state\n");
    return -ENODEV;
  }

  i = client_data_p->ringbufferpos+1;
  if (i == RINGBUFFERSIZE)
    i = 0;

  while (i != client_data_p->ringbufferpos) {
    if (client_data_p->ringbuffer[i].completed) {
      currentbufsize = client_data_p->ringbuffer[i].size;
      // we found a used & completed slot, outputting
      printk(KERN_DEBUG "store_show(): outputting ringbuf %.2x, %d bytes\n", i, currentbufsize);
      memcpy(buf_in, client_data_p->ringbuffer[i].data, currentbufsize);
      client_data_p->ringbuffer[i].completed = client_data_p->ringbuffer[i].used = 0;
      return currentbufsize;
    }

    i++;
    if (i == RINGBUFFERSIZE)
      i = 0;
  }

  return 0;
}

static
ssize_t
i2c_mpu6050_reg_store(struct kobject* kobj_in,
                      struct kobj_attribute* attr_in,
                      const char* buf_in, size_t count_in)
{
  unsigned int reg, val;
  struct i2c_client* client_p = kobj_to_i2c_client(kobj_in);
  if (!client_p) {
    printk(KERN_ERR "%s: invalid parameter (not a I2C device ?)\n", __func__);
    return -ENODEV;
  }

  sscanf(buf_in, "%x", &reg);
  sscanf(buf_in + 1, "%x", &val);
  i2c_smbus_write_byte_data(client_p, (u8)reg, (u8)val);
  printk(KERN_DEBUG "stored %.2x to register %.2x\n", (u8)reg, (u8)val);
  return count_in;
}

static
ssize_t
i2c_mpu6050_reg_show(struct kobject* kobj_in,
                     struct kobj_attribute* attr_in,
                     char* buf_in)
{
  unsigned int reg;
  struct i2c_client* client_p = kobj_to_i2c_client(kobj_in);
  if (!client_p) {
    printk(KERN_ERR "%s: invalid parameter (not a I2C device ?)\n", __func__);
    return -ENODEV;
  }

  sscanf(buf_in, "%x", &reg);
  return sprintf(buf_in, "%x\n", i2c_smbus_read_byte_data(client_p, (u8)reg));
}

static
void
i2c_mpu6050_clearringbuffer(void* data_in)
{
  struct i2c_mpu6050_client_data_t* client_data_p = NULL;
  int i;

  // sanity check(s)
  client_data_p = (struct i2c_mpu6050_client_data_t*)data_in;
  if (!client_data_p) {
    printk(KERN_ERR "unable to retrieve client state\n");
    return;
  }

  for (i = 0; i < RINGBUFFERSIZE; i++)
    client_data_p->ringbuffer[i].completed = client_data_p->ringbuffer[i].used = 0;
}

static
ssize_t
i2c_mpu6050_clearringbuffer_store(struct kobject* kobj_in,
                                  struct kobj_attribute* attr_in,
                                  const char* buf_in, size_t count_in)
{
  struct i2c_mpu6050_client_data_t* client_data_p = NULL;

  // sanity check(s)
//  struct device* device_p = kobj_to_dev(kobj_in);
////  struct kobj_type* ktype = get_ktype(kobj_in);
////  if (ktype == &device_ktype)
////    device_p = to_dev(kobj_in);
//  if (!device_p) {
//    printk(KERN_ERR "%s: invalid parameter (not a device ?)\n", __func__);
//    return -ENODEV;
//  }
//  struct i2c_client* client_p = to_i2c_client(device_p);
  struct i2c_client* client_p = kobj_to_i2c_client(kobj_in);
  if (!client_p) {
    printk(KERN_ERR "%s: invalid parameter (not a I2C device ?)\n", __func__);
    return -ENODEV;
  }
  client_data_p = (struct i2c_mpu6050_client_data_t*)i2c_get_clientdata(client_p);
  if (!client_data_p) {
    printk(KERN_ERR "unable to retrieve client state\n");
    return -ENODEV;
  }

  i2c_mpu6050_clearringbuffer(client_data_p);

  printk(KERN_DEBUG "ringbuffer cleared.\n");

  return count_in;
}

static
ssize_t
i2c_mpu6050_intstate_show(struct kobject* kobj_in,
                          struct kobj_attribute* attr_in,
                          char* buf_in)
{
//  struct i2c_client* client_p;
  int gpio;

//  client_p = kobj_to_i2c_client(kobj_in);
//  if (!client_p) {
//    printk(KERN_ERR "%s: invalid parameter (not a I2C device ?)\n", __func__);
//    return -ENODEV;
//  }
//  gpio = irq_to_gpio(client_p->irq);
  gpio = GPIO_UEXT4_UART4RX_PG11_PIN;
  if (gpio < 0) {
    printk(KERN_ERR "%s: irq_to_gpio(%d) failed\n",
           __func__,
           client_p->irq);
    return -ENODEV;
  }

  return sprintf(buf_in, "%d\n", gpio_get_value(gpio));
}

static
ssize_t
i2c_mpu6050_ledstate_show(struct kobject* kobj_in,
                          struct kobj_attribute* attr_in,
                          char* buf_in)
{
  struct i2c_mpu6050_client_data_t* client_data_p = NULL;
  struct i2c_client* client_p = kobj_to_i2c_client(kobj_in);
  if (!client_p) {
    printk(KERN_ERR "%s: invalid parameter (not a I2C device ?)\n", __func__);
    return -ENODEV;
  }
  client_data_p = (struct i2c_mpu6050_client_data_t*)i2c_get_clientdata(client_p);
  if (!client_data_p) {
    printk(KERN_ERR "unable to retrieve client state\n");
    return -ENODEV;
  }

  return sprintf(buf_in, "%d\n", gpio_read_one_pin_value(client_data_p->gpio_led_handle,
                                                         GPIO_LED_PH02_LABEL));
}

static
bool
i2c_mpu6050_sysfs_init(struct i2c_mpu6050_client_data_t* clientData_in)
{
  // sanity check(s)
  if (!clientData_in)
  {
    pr_err("%s: invalid argument, aborting\n", __FUNCTION__);
    return false;
  }
  if (clientData_in->object)
  {
    pr_warn("%s: sysfs handle already initialized, returning\n", __FUNCTION__);
    return true;
  }

  clientData_in->object = kobject_create_and_add(KO_OLIMEX_MOD_MPU6050_DRIVER_NAME, kernel_kobj);
  if (IS_ERR(client_data_p->object)) {
    pr_err("%s: kobject_create_and_add(%s) failed: %d, aborting\n", __FUNCTION__,
           KO_OLIMEX_MOD_MPU6050_DRIVER_NAME,
           PTR_ERR(client_data_p->object));
    return false;
  }
//  // create the files associated with this kobject
//  err = sysfs_create_group(client_data_p->object, &i2c_mpu6050_attribute_group);
//  if (err) {
//    printk(KERN_ERR "unable to create sysfs files\n");

//    // clean up
//    kobject_del(client_data_p->object);
//    kfree(client_data_p);

//    return -ENOMEM;
//  }
  // *TODO*: announce new sysfs object
//  err = kobject_uevent(client_data_p->object, enum kobject_action action);
//  if (err) {
//    printk(KERN_ERR "unable to announce sysfs files\n");

//    // clean up
//    kobject_unregister(client_data_p->object);
//    kfree(client_data_p);

//    return -ENOMEM;
//  }
}

static
void
i2c_mpu6050_sysfs_fini(struct i2c_mpu6050_client_data_t* clientData_in)
{
  // sanity check(s)
  if (!clientData_in)
  {
    pr_err("%s: invalid argument, returning\n", __FUNCTION__);
    return;
  }

  kobject_del(clientData_in->object);
  client_data_p->object = NULL;
}
