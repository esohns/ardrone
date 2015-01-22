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

#include <linux/gpio.h>
#include <linux/kobject.h>

#include "olimex_mod_mpu6050_types.h"
#include "olimex_mod_mpu6050_device.h"

struct kobj_attribute store_attribute =           __ATTR(data, 0666, i2c_mpu6050_store_show, i2c_mpu6050_store_store);
struct kobj_attribute reg_attribute =             __ATTR(addr, 0666, i2c_mpu6050_reg_show, i2c_mpu6050_reg_store);
struct kobj_attribute clearringbuffer_attribute = __ATTR(clear_ringbuffer, 0222, NULL, i2c_mpu6050_clearringbuffer_store);
struct kobj_attribute intstate_attribute =        __ATTR(int_state, 0444, i2c_mpu6050_intstate_show, NULL);
struct kobj_attribute ledstate_attribute =        __ATTR(led_state, 0444, i2c_mpu6050_ledstate_show, NULL);
struct kobj_attribute fifostate_attribute =       __ATTR(fifo_state, 0444, i2c_mpu6050_fifostate_show, NULL);
/* *NOTE*: use a group of attributes so that the kernel can create and destroy
 *         them all at once
 */
struct attribute* i2c_mpu6050_attrs[] = {
  &store_attribute.attr,
  &reg_attribute.attr,
  &clearringbuffer_attribute.attr,
  &intstate_attribute.attr,
  &ledstate_attribute.attr,
  &fifostate_attribute.attr,
  NULL, // need to NULL terminate the list of attributes
};
//ATTRIBUTE_GROUPS(i2c_mpu6050);
const struct attribute_group i2c_mpu6050_group = {
  .attrs = i2c_mpu6050_attrs,
};
const struct attribute_group* i2c_mpu6050_groups[] = {
  &i2c_mpu6050_group,
  NULL, // need to NULL terminate the list of attribute groups
};

ssize_t
i2c_mpu6050_store_store(struct kobject* kobj_in,
                        struct kobj_attribute* attr_in,
                        const char* buf_in,
                        size_t count_in)
{
  struct i2c_client* client_p;
  struct i2c_mpu6050_client_data_t* client_data_p;
  int err;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (!kobj_in) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -ENOSYS;
  }
  if (count_in > FIFOSTOREDATASIZE) {
    pr_err("%s: invalid argument %d (expected <= %d)", __FUNCTION__,
           count_in,
           FIFOSTOREDATASIZE);
    return -ENOSYS;
  }
//  struct device* device_p = kobj_to_dev(kobj_in);
////  struct kobj_type* ktype = get_ktype(kobj_in);
////  if (ktype == &device_ktype)
////    device_p = to_dev(kobj_in);
//  if (!device_p) {
//    printk(KERN_ERR "%s: invalid parameter (not a device ?)\n", __func__);
//    return -ENODEV;
//  }
//  client_p = to_i2c_client(device_p);
  client_p = kobj_to_i2c_client(kobj_in);
  if (IS_ERR(client_p)) {
    pr_err("%s: kobj_to_i2c_client() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_p));
    return -ENOSYS;
  }
  client_data_p = (struct i2c_mpu6050_client_data_t*)i2c_get_clientdata(client_p);
  if (IS_ERR(client_data_p)) {
    pr_err("%s: i2c_get_clientdata() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_data_p));
    return -ENOSYS;
  }
  if (client_data_p->fifostorepos >= FIFOSTORESIZE) {
    pr_err("%s: can't store data because FIFO is full.", __FUNCTION__);
    return -ENOSYS;
  }

  pr_debug("%s: storing %d bytes to store pos 0x%.2x\n", __FUNCTION__,
           (int)count_in,
           client_data_p->fifostorepos);
  memcpy(client_data_p->fifostore[client_data_p->fifostorepos].data, buf_in, count_in);
  client_data_p->fifostore[client_data_p->fifostorepos].size = count_in;
  client_data_p->fifostorepos++;

  pr_debug("%s: queueing work PROCESSFIFOSTORE\n", __FUNCTION__);
  err = queue_work(client_data_p->workqueue,
                   &client_data_p->work_processfifostore.work);
  if (err < 0) {
    pr_err("%s: queue_work failed: %d\n", __FUNCTION__,
           err);
    return -ENOSYS;
  }

  return count_in;
}

ssize_t
i2c_mpu6050_store_show(struct kobject* kobj_in,
                       struct kobj_attribute* attr_in,
                       char* buf_in)
{
  struct i2c_client* client_p;
  struct i2c_mpu6050_client_data_t* client_data_p;
  int i, currentbufsize;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (!kobj_in) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -ENOSYS;
  }
//  struct device* device_p = NULL;
//  struct kobj_type* ktype = get_ktype(kobj_in);
//  if (ktype == &device_ktype)
//    device_p = to_dev(kobj_in);
//  if (!device_p) {
//    printk(KERN_ERR "%s: invalid parameter (not a device ?)\n", __func__);
//    return -ENODEV;
//  }
//  client_p = to_i2c_client(device_p);
  client_p = kobj_to_i2c_client(kobj_in);
  if (IS_ERR(client_p)) {
    pr_err("%s: kobj_to_i2c_client() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_p));
    return -ENOSYS;
  }
  client_data_p = (struct i2c_mpu6050_client_data_t*)i2c_get_clientdata(client_p);
  if (IS_ERR(client_data_p)) {
    pr_err("%s: i2c_get_clientdata() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_data_p));
    return -ENOSYS;
  }

  i = (client_data_p->ringbufferpos + 1);
  if (i == RINGBUFFERSIZE) i = 0;
  while (i != client_data_p->ringbufferpos) {
    if (client_data_p->ringbuffer[i].completed) {
      currentbufsize = client_data_p->ringbuffer[i].size;
      // found a used & completed slot, outputting
      pr_debug("%s: outputting ringbuffer %.2x, %d bytes\n", __FUNCTION__,
               i,
               currentbufsize);
      memcpy(buf_in, client_data_p->ringbuffer[i].data, currentbufsize);
      client_data_p->ringbuffer[i].completed = client_data_p->ringbuffer[i].used = 0;
      return currentbufsize;
    }

    i++;
    if (i == RINGBUFFERSIZE) i = 0;
  }

  return 0;
}

ssize_t
i2c_mpu6050_reg_store(struct kobject* kobj_in,
                      struct kobj_attribute* attr_in,
                      const char* buf_in, size_t count_in)
{
  struct i2c_client* client_p;
  unsigned int reg, val;
  int err;
  s32 bytes_written;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (!kobj_in) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -ENOSYS;
  }
  if (count_in != 2) {
    pr_err("%s: invalid argument: %d (expected: %d)\n", __FUNCTION__,
           count_in, 2);
    return -ENOSYS;
  }
  client_p = kobj_to_i2c_client(kobj_in);
  if (IS_ERR(client_p)) {
    pr_err("%s: kobj_to_i2c_client() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_p));
    return -ENOSYS;
  }

  err = sscanf(buf_in, "%x", &reg);
  err = sscanf(buf_in + 1, "%x", &val);
  bytes_written = i2c_smbus_write_byte_data(client_p, (u8)reg, (u8)val);
  pr_debug("%s: stored %.2x to register %.2x\n", __FUNCTION__,
           (u8)reg,
           (u8)val);

  return count_in;
}

ssize_t
i2c_mpu6050_reg_show(struct kobject* kobj_in,
                     struct kobj_attribute* attr_in,
                     char* buf_in)
{
  struct i2c_client* client_p;
  unsigned int reg;
  int err;
  s32 bytes_read;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (!kobj_in) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -ENOSYS;
  }
  client_p = kobj_to_i2c_client(kobj_in);
  if (IS_ERR(client_p)) {
    pr_err("%s: kobj_to_i2c_client() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_p));
    return -ENOSYS;
  }

  err = sscanf(buf_in, "%x", &reg);
  bytes_read = i2c_smbus_read_byte_data(client_p, (u8)reg);

  return sprintf(buf_in, "%x\n", bytes_read);
}

void
i2c_mpu6050_clearringbuffer(void* data_in)
{
  struct i2c_mpu6050_client_data_t* client_data_p;
  int i;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (!data_in) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return;
  }
  client_data_p = (struct i2c_mpu6050_client_data_t*)data_in;
  if (!client_data_p) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return;
  }

  for (i = 0; i < RINGBUFFERSIZE; i++)
    client_data_p->ringbuffer[i].completed = client_data_p->ringbuffer[i].used = 0;
}

ssize_t
i2c_mpu6050_clearringbuffer_store(struct kobject* kobj_in,
                                  struct kobj_attribute* attr_in,
                                  const char* buf_in, size_t count_in)
{
  struct i2c_client* client_p;
  struct i2c_mpu6050_client_data_t* client_data_p;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (!kobj_in) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -ENOSYS;
  }
//  struct device* device_p = kobj_to_dev(kobj_in);
////  struct kobj_type* ktype = get_ktype(kobj_in);
////  if (ktype == &device_ktype)
////    device_p = to_dev(kobj_in);
//  if (!device_p) {
//    printk(KERN_ERR "%s: invalid parameter (not a device ?)\n", __func__);
//    return -ENODEV;
//  }
//  client_p = to_i2c_client(device_p);
  client_p = kobj_to_i2c_client(kobj_in);
  if (IS_ERR(client_p)) {
    pr_err("%s: kobj_to_i2c_client() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_p));
    return -ENOSYS;
  }
  client_data_p = (struct i2c_mpu6050_client_data_t*)i2c_get_clientdata(client_p);
  if (IS_ERR(client_data_p)) {
    pr_err("%s: i2c_get_clientdata() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_data_p));
    return -ENOSYS;
  }

  i2c_mpu6050_clearringbuffer(client_data_p);

  pr_debug("%s: ringbuffer cleared.\n", __FUNCTION__);

  return count_in;
}

ssize_t
i2c_mpu6050_intstate_show(struct kobject* kobj_in,
                          struct kobj_attribute* attr_in,
                          char* buf_in)
{
  struct i2c_client* client_p;
  int gpio, value;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (!kobj_in) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -ENOSYS;
  }
  client_p = kobj_to_i2c_client(kobj_in);
  if (IS_ERR(client_p)) {
    pr_err("%s: kobj_to_i2c_client() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_p));
    return -ENOSYS;
  }
//  gpio = irq_to_gpio(client_p->irq);
  gpio = GPIO_INT_PIN;
//  if (gpio < 0) {
//    pr_err("%s: irq_to_gpio(%d) failed\n", __FUNCTION__,
//           client_p->irq);
//    return -ENOSYS;
//  }

  value = gpio_get_value(gpio);

  return sprintf(buf_in, "%d\n", value);
}

ssize_t
i2c_mpu6050_ledstate_show(struct kobject* kobj_in,
                          struct kobj_attribute* attr_in,
                          char* buf_in)
{
  struct i2c_client* client_p;
  struct i2c_mpu6050_client_data_t* client_data_p;
  int value;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (!kobj_in) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -ENOSYS;
  }
  client_p = kobj_to_i2c_client(kobj_in);
  if (IS_ERR(client_p)) {
    pr_err("%s: kobj_to_i2c_client() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_p));
    return -ENOSYS;
  }
  client_data_p = (struct i2c_mpu6050_client_data_t*)i2c_get_clientdata(client_p);
  if (IS_ERR(client_data_p)) {
    pr_err("%s: i2c_get_clientdata() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_data_p));
    return -ENOSYS;
  }

  value = gpio_read_one_pin_value(client_data_p->gpio_led_handle,
                                  GPIO_LED_PIN_LABEL);

  return sprintf(buf_in, "%d\n", value);
}

ssize_t
i2c_mpu6050_fifostate_show(struct kobject* kobj_in,
                           struct kobj_attribute* attr_in,
                           char* buf_in)
{
  struct i2c_client* client_p;
  struct i2c_mpu6050_client_data_t* client_data_p;
  int value;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (!kobj_in) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -ENOSYS;
  }
  client_p = kobj_to_i2c_client(kobj_in);
  if (IS_ERR(client_p)) {
    pr_err("%s: kobj_to_i2c_client() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_p));
    return -ENOSYS;
  }
  client_data_p = (struct i2c_mpu6050_client_data_t*)i2c_get_clientdata(client_p);
  if (IS_ERR(client_data_p)) {
    pr_err("%s: i2c_get_clientdata() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_data_p));
    return -ENOSYS;
  }

  value = i2c_mpu6050_device_fifo_count(client_data_p);

  return sprintf(buf_in, "%d\n", value);
}

int
i2c_mpu6050_sysfs_init(struct i2c_mpu6050_client_data_t* clientData_in)
{
  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (!clientData_in) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -ENOSYS;
  }
  if (clientData_in->object) {
    pr_warn("%s: sysfs handle already initialized, returning\n", __FUNCTION__);
    return 0;
  }

  clientData_in->object = kobject_create_and_add(KO_OLIMEX_MOD_MPU6050_DRIVER_NAME,
                                                 kernel_kobj);
  if (IS_ERR(clientData_in->object)) {
    pr_err("%s: kobject_create_and_add(%s) failed: %ld, aborting\n", __FUNCTION__,
           KO_OLIMEX_MOD_MPU6050_DRIVER_NAME,
           PTR_ERR(clientData_in->object));
    return -ENOSYS;
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

  return 0;
}

void
i2c_mpu6050_sysfs_fini(struct i2c_mpu6050_client_data_t* clientData_in)
{
  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (!clientData_in) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return;
  }

  kobject_del(clientData_in->object);
  clientData_in->object = NULL;
}
