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

#include <linux/device.h>
#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/sysfs.h>

#include "olimex_mod_mpu6050_types.h"
#include "olimex_mod_mpu6050_device.h"

DEVICE_ATTR(reg, 0666, i2c_mpu6050_reg_show, i2c_mpu6050_reg_store);
//DEVICE_ATTR(buffer, 0666, i2c_mpu6050_ringbuffer_show, i2c_mpu6050_ringbuffer_store);
DEVICE_ATTR(int_state, 0444, i2c_mpu6050_intstate_show, NULL);
DEVICE_ATTR(led_state, 0444, i2c_mpu6050_ledstate_show, NULL);

struct bin_attribute dev_attr_buffer = {
  .attr = {
    .name = __stringify(buffer),
    .mode = 0666,
  },
  .size = (RINGBUFFER_DATA_SIZE * RINGBUFFER_SIZE),
  .private = NULL,
  .read = i2c_mpu6050_ringbuffer_read,
  .write = i2c_mpu6050_ringbuffer_write,
  .mmap = i2c_mpu6050_ringbuffer_mmap,
};

/* *NOTE*: use a group of attributes so that the kernel can create and destroy
 *         them all at once
 */
struct attribute* i2c_mpu6050_attrs[] = {
  &dev_attr_reg.attr,
  &dev_attr_int_state.attr,
  &dev_attr_led_state.attr,
  NULL, // need to NULL terminate the list of attributes
};
//ATTRIBUTE_GROUPS(i2c_mpu6050);
const struct attribute_group i2c_mpu6050_group = {
  .name = KO_OLIMEX_MOD_MPU6050_DRIVER_NAME,
  .is_visible = i2c_mpu6050_attr_is_visible,
  .attrs = i2c_mpu6050_attrs,
};
const struct attribute_group* i2c_mpu6050_groups[] = {
  &i2c_mpu6050_group,
  NULL, // need to NULL terminate the list of attribute groups
};

umode_t
i2c_mpu6050_attr_is_visible(struct kobject* kobj_in,
                            struct attribute* attr_in,
                            int arg_in)
{
  pr_debug("%s called.\n", __FUNCTION__);

  return 0;
}

ssize_t
i2c_mpu6050_reg_store(struct device* dev_in,
                      struct device_attribute* attr_in,
                      const char* buf_in, size_t count_in)
{
  struct i2c_mpu6050_client_data_t* client_data_p;
  u8 reg, value;
  int err;
  s32 bytes_written;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely(!dev_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -EINVAL;
  }
  if (unlikely(!buf_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -EINVAL;
  }
  if (unlikely(count_in < 2)) {
    pr_err("%s: invalid argument: %d (expected: %d)\n", __FUNCTION__,
           count_in, 2);
    return -EINVAL;
  }
  client_data_p = (struct i2c_mpu6050_client_data_t*)dev_get_drvdata(dev_in->parent);
  if (unlikely(IS_ERR(client_data_p))) {
    pr_err("%s: dev_get_drvdata() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_data_p));
    return PTR_ERR(client_data_p);
  }
  err = sscanf(buf_in, "%c", &reg);
  if (unlikely(err != 1)) {
    pr_err("%s: sscanf() failed: %d\n", __FUNCTION__,
           err);
    return err;
  }
  err = sscanf(buf_in + 1, "%c", &value);
  if (unlikely(err != 1)) {
    pr_err("%s: sscanf() failed: %d\n", __FUNCTION__,
           err);
    return err;
  }

  gpio_write_one_pin_value(client_data_p->gpio_led_handle, 1, GPIO_LED_PIN_LABEL);
  bytes_written = i2c_smbus_write_byte_data(client_data_p->client, reg, value);
  gpio_write_one_pin_value(client_data_p->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
  if (unlikely(bytes_written != 1)) {
    pr_err("%s: i2c_smbus_write_byte_data(0x%x,0x%x) failed: %d\n", __FUNCTION__,
           reg, value,
           bytes_written);
    return -EIO;
  }

  pr_debug("%s: wrote 0x%x to register 0x%x\n", __FUNCTION__,
           reg, value);

  return count_in;
}

ssize_t
i2c_mpu6050_reg_show(struct device* dev_in,
                     struct device_attribute* attr_in,
                     char* buf_in)
{
  struct i2c_mpu6050_client_data_t* client_data_p;
  u8 reg;
  int err;
  s32 value;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely(!dev_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -EINVAL;
  }
  if (unlikely(!buf_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -EINVAL;
  }
  client_data_p = (struct i2c_mpu6050_client_data_t*)dev_get_drvdata(dev_in->parent);
  if (unlikely(IS_ERR(client_data_p))) {
    pr_err("%s: dev_get_drvdata() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_data_p));
    return PTR_ERR(client_data_p);
  }
  err = sscanf(buf_in, "%c", &reg);
  if (unlikely(err != 1)) {
    pr_err("%s: sscanf() failed: %d\n", __FUNCTION__,
           err);
    return err;
  }

  memset(buf_in, 0, PAGE_SIZE);
  gpio_write_one_pin_value(client_data_p->gpio_led_handle, 1, GPIO_LED_PIN_LABEL);
  value = i2c_smbus_read_byte_data(client_data_p->client, reg);
  gpio_write_one_pin_value(client_data_p->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
  // *TODO*: how can this work ?
  if (unlikely(value < 0)) {
    pr_err("%s: i2c_smbus_read_byte_data(0x%x) failed: %d\n", __FUNCTION__,
           reg, value);
    return -EIO;
  }
  err = scnprintf(buf_in, PAGE_SIZE, "0x%x\n", value);
  if (unlikely(err < 0)) {
    pr_err("%s: scnprintf() failed: %d\n", __FUNCTION__,
           err);
    return err;
  }

  return err;
}

void
i2c_mpu6050_ringbuffer_clear(void* data_in)
{
  struct i2c_mpu6050_client_data_t* client_data_p;
  int i;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  client_data_p = (struct i2c_mpu6050_client_data_t*)data_in;
  if (unlikely(!client_data_p)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return;
  }

  mutex_lock(&client_data_p->sync_lock);
  for (i = 0; i < RINGBUFFER_SIZE; i++)
    client_data_p->ringbuffer[i].completed = client_data_p->ringbuffer[i].used = 0;
  mutex_unlock(&client_data_p->sync_lock);
}

ssize_t
i2c_mpu6050_ringbuffer_read(struct file* file_in,
                            struct kobject* kobj_in,
                            struct bin_attribute* attr_in,
                            char* buf_in,
                            loff_t offset_in,
                            size_t count_in)
{
  struct i2c_client* client_p;
  struct i2c_mpu6050_client_data_t* client_data_p;
  int i;
  u8* reg_p;
  u16 value_x, value_y, value_z;
  int err;
  ssize_t bytes_written = 0;
  unsigned int remaining = PAGE_SIZE;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely(!kobj_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -EINVAL;
  }
  client_p = kobj_to_i2c_client(kobj_in);
  if (unlikely(IS_ERR(client_p))) {
    pr_err("%s: kobj_to_i2c_client() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_p));
    return PTR_ERR(client_p);
  }
  client_data_p = (struct i2c_mpu6050_client_data_t*)i2c_get_clientdata(client_p);
  if (unlikely(IS_ERR(client_data_p))) {
    pr_err("%s: i2c_get_clientdata() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_data_p));
    return PTR_ERR(client_data_p);
  }

  memset(buf_in, 0, remaining);
  mutex_lock(&client_data_p->sync_lock);
  i = ((client_data_p->ringbufferpos == (RINGBUFFER_SIZE - 1)) ? 0
                                                               : (client_data_p->ringbufferpos + 1));
  do {
    if (!client_data_p->ringbuffer[i].completed ||
        !client_data_p->ringbuffer[i].used)
      goto done; // unused slot --> continue

    reg_p = client_data_p->ringbuffer[i].data;
    value_x = ~*(u16*)reg_p;
//    pr_info("%s: acceleration (x): %.5f g\n", __FUNCTION__,
//            (float)value / (float)ACCEL_SENSITIVITY);
//    reg_p += 2;
//    value = ~*(u16*)reg_p;
//    pr_info("%s: acceleration (y): %.5f g\n", __FUNCTION__,
//            (float)value / (float)ACCEL_SENSITIVITY);
//    reg_p += 2;
//    value = ~*(u16*)reg_p;
//    pr_info("%s: acceleration (z): %.5f g\n", __FUNCTION__,
//            (float)value / (float)ACCEL_SENSITIVITY);
//    reg_p += 2;
//    value = *(s16*)reg_p;
//    pr_info("%s: temperature: %.5f °C\n", __FUNCTION__,
//            ((float)(s16)value / THERMO_SENSITIVITY) + THERMO_OFFSET);
//    reg_p += 2;
//    value = ~*(u16*)reg_p;
//    pr_info("%s: rotation (x): %.5f °/s\n", __FUNCTION__,
//            (float)value / (float)GYRO_SENSITIVITY);
//    reg_p += 2;
//    value = ~*(u16*)reg_p;
//    pr_info("%s: rotation (y): %.5f °/s\n", __FUNCTION__,
//            (float)value / (float)GYRO_SENSITIVITY);
//    reg_p += 2;
//    value = ~*(u16*)reg_p;
//    pr_info("%s: rotation (z): %.5f °/s\n", __FUNCTION__,
//            (float)value / (float)GYRO_SENSITIVITY);
    reg_p += 2;
    value_y = ~*(u16*)reg_p;
    reg_p += 2;
    value_z = ~*(u16*)reg_p;
    err = scnprintf(buf_in + bytes_written, remaining,
                    "#%d: acceleration (x,y,z): %d,%d,%d\n",
                    i,
                    value_x, value_y, value_z);
    if (unlikely(err < 0)) {
      pr_err("%s: scnprintf() failed: %d\n", __FUNCTION__,
             err);
      bytes_written = err;
      break;
    }
    bytes_written += err;
    remaining -= err;
    if (remaining == 0) break;
    reg_p += 2;
    value_x = *(s16*)reg_p;
    err = scnprintf(buf_in + bytes_written, remaining,
                    "#%d: temperature: %d\n",
                    i,
                    (s16)value_x);
    if (unlikely(err < 0)) {
      pr_err("%s: scnprintf() failed: %d\n", __FUNCTION__,
             err);
      bytes_written = err;
      break;
    }
    bytes_written += err;
    remaining -= err;
    if (remaining == 0) break;
    reg_p += 2;
    value_x = ~*(u16*)reg_p;
    reg_p += 2;
    value_y = ~*(u16*)reg_p;
    reg_p += 2;
    value_z = ~*(u16*)reg_p;
    err = scnprintf(buf_in + bytes_written, remaining,
                    "#%d: rotation (x,y,z): %d,%d,%d\n",
                    i,
                    value_x, value_y, value_z);
    if (unlikely(err < 0)) {
      pr_err("%s: scnprintf() failed: %d\n", __FUNCTION__,
             err);
      bytes_written = err;
      break;
    }
    bytes_written += err;
    remaining -= err;
    if (remaining == 0) break;

done:
    i++;
    if (i == RINGBUFFER_SIZE) i = 0;
  } while (i != client_data_p->ringbufferpos);
  mutex_unlock(&client_data_p->sync_lock);

  return bytes_written;
}

ssize_t
i2c_mpu6050_ringbuffer_write(struct file* file_in,
                             struct kobject* kobj_in,
                             struct bin_attribute* attr_in,
                             char* buf_in,
                             loff_t offset_in,
                             size_t count_in)
{
  struct i2c_client* client_p;
  struct i2c_mpu6050_client_data_t* client_data_p;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely(!kobj_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -EINVAL;
  }
  client_p = kobj_to_i2c_client(kobj_in);
  if (unlikely(IS_ERR(client_p))) {
    pr_err("%s: kobj_to_i2c_client() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_p));
    return PTR_ERR(client_p);
  }
  pr_debug("%s device: 0x%p\n", __FUNCTION__,
           container_of(kobj_in, struct device, kobj));
  client_data_p = (struct i2c_mpu6050_client_data_t*)i2c_get_clientdata(client_p);
  if (unlikely(IS_ERR(client_data_p))) {
    pr_err("%s: i2c_get_clientdata() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_data_p));
    return PTR_ERR(client_data_p);
  }

  i2c_mpu6050_ringbuffer_clear(client_data_p);

  return 0;
}

int
i2c_mpu6050_ringbuffer_mmap(struct file* file_in,
                            struct kobject* kobj_in,
                            struct bin_attribute* attr_in,
                            struct vm_area_struct* area_in)
{
  pr_debug("%s called.\n", __FUNCTION__);

  return -ENOSYS;
}

ssize_t
i2c_mpu6050_intstate_show(struct device* dev_in,
                          struct device_attribute* attr_in,
                          char* buf_in)
{
  struct i2c_mpu6050_client_data_t* client_data_p;
  int value;
  int err;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely(!dev_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -EINVAL;
  }
  client_data_p = (struct i2c_mpu6050_client_data_t*)dev_get_drvdata(dev_in->parent);
  if (unlikely(IS_ERR(client_data_p))) {
    pr_err("%s: dev_get_drvdata() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_data_p));
    return PTR_ERR(client_data_p);
  }

  value = gpio_read_one_pin_value(client_data_p->gpio_int_handle,
                                  GPIO_INT_PIN_LABEL);
  err = scnprintf(buf_in, PAGE_SIZE, "%d\n", value);
  if (unlikely(err < 0)) {
    pr_err("%s: scnprintf() failed: %d\n", __FUNCTION__,
           err);
    return err;
  }

  return err;
}

ssize_t
i2c_mpu6050_ledstate_show(struct device* dev_in,
                          struct device_attribute* attr_in,
                          char* buf_in)
{
  struct i2c_mpu6050_client_data_t* client_data_p;
  int value;
  int err;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely(!dev_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -EINVAL;
  }
  client_data_p = (struct i2c_mpu6050_client_data_t*)dev_get_drvdata(dev_in->parent);
  if (unlikely(IS_ERR(client_data_p))) {
    pr_err("%s: dev_get_drvdata() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_data_p));
    return PTR_ERR(client_data_p);
  }

  value = gpio_read_one_pin_value(client_data_p->gpio_led_handle,
                                  GPIO_LED_PIN_LABEL);
  err = scnprintf(buf_in, PAGE_SIZE, "%d\n", value);
  if (unlikely(err < 0)) {
    pr_err("%s: scnprintf() failed: %d\n", __FUNCTION__,
           err);
    return err;
  }

  return err;
}

int
i2c_mpu6050_sysfs_init(struct i2c_mpu6050_client_data_t* clientData_in)
{
  int err;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely(!clientData_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -EINVAL;
  }
//  if (unlikely(clientData_in->sysfs_object)) {
//    pr_warn("%s: sysfs handle already initialized, returning\n", __FUNCTION__);
//    return 0;
//  }

//  clientData_in->sysfs_object = kobject_create_and_add(KO_OLIMEX_MOD_MPU6050_DRIVER_NAME,
//                                                       kernel_kobj);
//  if (unlikely(IS_ERR(clientData_in->sysfs_object))) {
//    pr_err("%s: kobject_create_and_add(%s) failed: %ld\n", __FUNCTION__,
//           KO_OLIMEX_MOD_MPU6050_DRIVER_NAME,
//           PTR_ERR(clientData_in->sysfs_object));
//    return PTR_ERR(clientData_in->sysfs_object);
//  }
//  err = sysfs_create_group(&clientData_in->client->dev.kobj, &i2c_mpu6050_group);
//  if (unlikely(err)) {
//    pr_err("%s: sysfs_create_group() failed: %d\n", __FUNCTION__,
//           err);
//    goto error1;
//  }
  err = sysfs_create_bin_file(&clientData_in->client->dev.kobj,
                              &dev_attr_buffer);
  if (unlikely(err)) {
    pr_err("%s: sysfs_create_bin_file() failed: %d\n", __FUNCTION__,
           err);
    goto error2;
  }
//  err = kobject_uevent(clientData_in->sysfs_object, KOBJ_ADD);
//  if (unlikely(err)) {
//    pr_err("%s: kobject_uevent(%d) failed: %d\n", __FUNCTION__,
//           KOBJ_ADD,
//           err);
//    goto error3;
//  }

  return 0;

//error3:
  sysfs_remove_bin_file(&clientData_in->client->dev.kobj, &dev_attr_buffer);
error2:
//  sysfs_remove_group(clientData_in->sysfs_object, &i2c_mpu6050_group);
//error1:
//  kobject_put(clientData_in->sysfs_object);

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

  sysfs_remove_bin_file(&clientData_in->client->dev.kobj, &dev_attr_buffer);
//  sysfs_remove_group(&clientData_in->client->dev.kobj, &i2c_mpu6050_group);
//  kobject_put(clientData_in->sysfs_object);
}
