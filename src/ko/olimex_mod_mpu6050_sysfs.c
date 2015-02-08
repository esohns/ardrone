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

#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/kobject.h>

#include "olimex_mod_mpu6050_types.h"
#include "olimex_mod_mpu6050_device.h"

struct kobj_attribute reg_attribute        = __ATTR(reg, 0666, i2c_mpu6050_reg_show, i2c_mpu6050_reg_store);
struct kobj_attribute ringbuffer_attribute = __ATTR(buffer, 0444, i2c_mpu6050_ringbuffer_show, NULL);
struct kobj_attribute intstate_attribute   = __ATTR(int_state, 0444, i2c_mpu6050_intstate_show, NULL);
struct kobj_attribute ledstate_attribute   = __ATTR(led_state, 0444, i2c_mpu6050_ledstate_show, NULL);
/* *NOTE*: use a group of attributes so that the kernel can create and destroy
 *         them all at once
 */
struct attribute* i2c_mpu6050_attrs[] = {
  &reg_attribute.attr,
  &ringbuffer_attribute.attr,
  &intstate_attribute.attr,
  &ledstate_attribute.attr,
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
  if (unlikely(!kobj_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -ENOSYS;
  }
  if (unlikely(count_in != 2)) {
    pr_err("%s: invalid argument: %d (expected: %d)\n", __FUNCTION__,
           count_in, 2);
    return -ENOSYS;
  }
  client_p = kobj_to_i2c_client(kobj_in);
  if (unlikely(IS_ERR(client_p))) {
    pr_err("%s: kobj_to_i2c_client() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_p));
    return -ENOSYS;
  }

  err = sscanf(buf_in, "%x", &reg);
  err = sscanf(buf_in + 1, "%x", &val);
  bytes_written = i2c_smbus_write_byte_data(client_p, (u8)reg, (u8)val);
  if (unlikely(bytes_written != 1)) {
    pr_err("%s: i2c_smbus_write_byte_data(0x%x) failed: %d\n", __FUNCTION__,
           reg, bytes_written);
    return -EIO;
  }

  pr_debug("%s: wrote 0x%.2x to register 0x%.2x\n", __FUNCTION__,
           (u8)reg, (u8)val);

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
  if (unlikely(!kobj_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -ENOSYS;
  }
  client_p = kobj_to_i2c_client(kobj_in);
  if (unlikely(IS_ERR(client_p))) {
    pr_err("%s: kobj_to_i2c_client() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_p));
    return -ENOSYS;
  }

  err = sscanf(buf_in, "%x", &reg);
  bytes_read = i2c_smbus_read_byte_data(client_p, (u8)reg);
  // *TODO*: how can this work ?
  if (unlikely(bytes_read < 0)) {
    pr_err("%s: i2c_smbus_read_byte_data(0x%x) failed: %d\n", __FUNCTION__,
           reg, bytes_read);
    return -EIO;
  }

  return sprintf(buf_in, "%x\n", bytes_read);
}

void
i2c_mpu6050_ringbuffer_clear(void* data_in)
{
  struct i2c_mpu6050_client_data_t* client_data_p;
  int i;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely(!data_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return;
  }
  client_data_p = (struct i2c_mpu6050_client_data_t*)data_in;
  if (unlikely(!client_data_p)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return;
  }

  spin_lock(&client_data_p->sync_lock);
  for (i = 0; i < RINGBUFFER_SIZE; i++)
    client_data_p->ringbuffer[i].completed = client_data_p->ringbuffer[i].used = 0;
  spin_unlock(&client_data_p->sync_lock);
}

ssize_t
i2c_mpu6050_ringbuffer_store(struct kobject* kobj_in,
                             struct kobj_attribute* attr_in,
                             const char* buf_in, size_t count_in)
{
  struct i2c_client* client_p;
  struct i2c_mpu6050_client_data_t* client_data_p;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely(!kobj_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -ENOSYS;
  }
  client_p = kobj_to_i2c_client(kobj_in);
  if (unlikely(IS_ERR(client_p))) {
    pr_err("%s: kobj_to_i2c_client() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_p));
    return -ENOSYS;
  }
  client_data_p = (struct i2c_mpu6050_client_data_t*)i2c_get_clientdata(client_p);
  if (unlikely(IS_ERR(client_data_p))) {
    pr_err("%s: i2c_get_clientdata() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_data_p));
    return -ENOSYS;
  }

  i2c_mpu6050_ringbuffer_clear(client_data_p);

  return 0;
}

ssize_t
i2c_mpu6050_ringbuffer_show(struct kobject* kobj_in,
                            struct kobj_attribute* attr_in,
                            char* buf_in)
{
  struct i2c_client* client_p;
  struct i2c_mpu6050_client_data_t* client_data_p;
  int i;
  u8* reg_p;
  u16 value;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely(!kobj_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -ENOSYS;
  }
  client_p = kobj_to_i2c_client(kobj_in);
  if (unlikely(IS_ERR(client_p))) {
    pr_err("%s: kobj_to_i2c_client() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_p));
    return -ENOSYS;
  }
  client_data_p = (struct i2c_mpu6050_client_data_t*)i2c_get_clientdata(client_p);
  if (unlikely(IS_ERR(client_data_p))) {
    pr_err("%s: i2c_get_clientdata() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_data_p));
    return -ENOSYS;
  }

  spin_lock(&client_data_p->sync_lock);
  for (i = 0; i < RINGBUFFER_SIZE; i++) {
    if (!client_data_p->ringbuffer[i].completed ||
        !client_data_p->ringbuffer[i].used)
      continue; // unused slot

    reg_p = client_data_p->ringbuffer[i].data;
    value = ~*(u16*)reg_p;
//    kernel_fpu_begin();
    pr_info("%s: acceleration (x): %.5f g\n", __FUNCTION__,
            (float)value / (float)ACCEL_SENSITIVITY);
    reg_p += 2;
    value = ~*(u16*)reg_p;
    pr_info("%s: acceleration (y): %.5f g\n", __FUNCTION__,
            (float)value / (float)ACCEL_SENSITIVITY);
    reg_p += 2;
    value = ~*(u16*)reg_p;
    pr_info("%s: acceleration (z): %.5f g\n", __FUNCTION__,
            (float)value / (float)ACCEL_SENSITIVITY);
    reg_p += 2;
    value = *(s16*)reg_p;
    pr_info("%s: temperature: %.5f °C\n", __FUNCTION__,
            ((float)(s16)value / THERMO_SENSITIVITY) + THERMO_OFFSET);
    reg_p += 2;
    value = ~*(u16*)reg_p;
    pr_info("%s: rotation (x): %.5f °/s\n", __FUNCTION__,
            (float)value / (float)GYRO_SENSITIVITY);
    reg_p += 2;
    value = ~*(u16*)reg_p;
    pr_info("%s: rotation (y): %.5f °/s\n", __FUNCTION__,
            (float)value / (float)GYRO_SENSITIVITY);
    reg_p += 2;
    value = ~*(u16*)reg_p;
    pr_info("%s: rotation (z): %.5f °/s\n", __FUNCTION__,
            (float)value / (float)GYRO_SENSITIVITY);
//    kernel_fpu_end();
  }
  spin_unlock(&client_data_p->sync_lock);

  i2c_mpu6050_ringbuffer_clear(client_data_p);
  pr_debug("%s: ringbuffer cleared.\n", __FUNCTION__);

  return 0;
}

ssize_t
i2c_mpu6050_intstate_show(struct kobject* kobj_in,
                          struct kobj_attribute* attr_in,
                          char* buf_in)
{
  struct i2c_client* client_p;
  struct i2c_mpu6050_client_data_t* client_data_p;
  int value;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely(!kobj_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -ENOSYS;
  }
  client_p = kobj_to_i2c_client(kobj_in);
  if (unlikely(IS_ERR(client_p))) {
    pr_err("%s: kobj_to_i2c_client() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_p));
    return -ENOSYS;
  }
  client_data_p = (struct i2c_mpu6050_client_data_t*)i2c_get_clientdata(client_p);
  if (unlikely(IS_ERR(client_data_p))) {
    pr_err("%s: i2c_get_clientdata() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_data_p));
    return -ENOSYS;
  }

  value = gpio_read_one_pin_value(client_data_p->gpio_int_handle,
                                  GPIO_INT_PIN_LABEL);

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
  if (unlikely(!kobj_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -ENOSYS;
  }
  client_p = kobj_to_i2c_client(kobj_in);
  if (unlikely(IS_ERR(client_p))) {
    pr_err("%s: kobj_to_i2c_client() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_p));
    return -ENOSYS;
  }
  client_data_p = (struct i2c_mpu6050_client_data_t*)i2c_get_clientdata(client_p);
  if (unlikely(IS_ERR(client_data_p))) {
    pr_err("%s: i2c_get_clientdata() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_data_p));
    return -ENOSYS;
  }

  value = gpio_read_one_pin_value(client_data_p->gpio_led_handle,
                                  GPIO_LED_PIN_LABEL);

  return sprintf(buf_in, "%d\n", value);
}

int
i2c_mpu6050_sysfs_init(struct i2c_mpu6050_client_data_t* clientData_in)
{
  int err;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely(!clientData_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -ENOSYS;
  }
  if (unlikely(clientData_in->sysfs_object)) {
    pr_warn("%s: sysfs handle already initialized, returning\n", __FUNCTION__);
    return 0;
  }

  clientData_in->sysfs_object = kobject_create_and_add(KO_OLIMEX_MOD_MPU6050_DRIVER_NAME,
                                                       kernel_kobj);
  if (unlikely(IS_ERR(clientData_in->sysfs_object))) {
    pr_err("%s: kobject_create_and_add(%s) failed: %ld\n", __FUNCTION__,
           KO_OLIMEX_MOD_MPU6050_DRIVER_NAME,
           PTR_ERR(clientData_in->sysfs_object));
    return PTR_ERR(clientData_in->sysfs_object);
  }
  err = sysfs_create_group(clientData_in->sysfs_object, &i2c_mpu6050_group);
  if (unlikely(err)) {
    pr_err("%s: sysfs_create_group() failed: %d\n", __FUNCTION__,
           err);
    goto error1;
  }
  err = kobject_uevent(clientData_in->sysfs_object, KOBJ_ADD);
  if (unlikely(err)) {
    pr_err("%s: kobject_uevent(%d) failed: %d\n", __FUNCTION__,
           KOBJ_ADD,
           err);
    goto error2;
  }

  return 0;

error2:
  sysfs_remove_group(clientData_in->sysfs_object, &i2c_mpu6050_group);
error1:
  kobject_put(clientData_in->sysfs_object);

  return err;
}

void
i2c_mpu6050_sysfs_fini(struct i2c_mpu6050_client_data_t* clientData_in)
{
  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely(!clientData_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return;
  }

  sysfs_remove_group(clientData_in->sysfs_object, &i2c_mpu6050_group);
  kobject_put(clientData_in->sysfs_object);
}
