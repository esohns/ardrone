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

#include "olimex_mod_mpu6050_wq.h"

#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/workqueue.h>

// *NOTE*: taken from i2cdevlib (see also: http://www.i2cdevlib.com/devices/mpu6050#source)
#include "MPU6050.h"

#include "olimex_mod_mpu6050_defines.h"
#include "olimex_mod_mpu6050_device.h"
#include "olimex_mod_mpu6050_main.h"
#include "olimex_mod_mpu6050_types.h"

void
i2c_mpu6050_wq_read_handler(struct work_struct* work_in)
{
  struct i2c_mpu6050_wq_read_work_t* work_p;
  struct i2c_mpu6050_client_data_t* client_data_p;
  s32 bytes_to_read = 0;
  int err;

//  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  work_p = (struct i2c_mpu6050_wq_read_work_t*)work_in;
  if (unlikely(!work_p)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return;
  }
  client_data_p =
   (struct i2c_mpu6050_client_data_t*)i2c_get_clientdata(work_p->client);
  if (unlikely(IS_ERR(client_data_p))) {
    pr_err("%s: i2c_get_clientdata() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_data_p));
    return;
  }

  mutex_lock(&client_data_p->sync_lock);

  client_data_p->ringbufferpos++;
  if (client_data_p->ringbufferpos == KO_OLIMEX_MOD_MPU6050_RINGBUFFER_SIZE)
    client_data_p->ringbufferpos = 0;
  client_data_p->ringbuffer[client_data_p->ringbufferpos].used = 1;
  client_data_p->ringbuffer[client_data_p->ringbufferpos].size = 0;
  // *TODO*: move this to the IRQ handler...
  client_data_p->ringbuffer[client_data_p->ringbufferpos].timestamp =
   ktime_to_ns(ktime_get_real());

  if (likely(nofifo == 0)) {
    // *TODO*: use a single transaction for efficiency...
//    reg = MPU6050_RA_FIFO_R_W;
//    gpio_write_one_pin_value(client_data_p->gpio_led_handle, 1, GPIO_LED_PIN_LABEL);
//    err = i2c_master_send(client_data_p->client,
//                          &reg, 1);
//    if (unlikely(err != 1)) {
//      gpio_write_one_pin_value(client_data_p->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
//      pr_err("%s: i2c_master_send() failed: %d\n", __FUNCTION__,
//             err);
//      goto clean_up;
//    }
//    err = i2c_master_recv(client_data_p->client,
//                          client_data_p->ringbuffer[client_data_p->ringbufferpos].data,
//                          RINGBUFFERDATASIZE);
//    gpio_write_one_pin_value(client_data_p->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
//    if (unlikely(err <= 0)) {
//      pr_err("%s: i2c_master_recv() failed: %d\n", __FUNCTION__,
//             err);
//      goto clean_up;
//    }
    bytes_to_read = i2c_mpu6050_device_fifo_count(client_data_p);
    if (unlikely(bytes_to_read < 0)) {
      pr_err("%s: i2c_mpu6050_device_fifo_count() failed: %d\n", __FUNCTION__,
             bytes_to_read);
      goto clean_up;
    }
    if (unlikely(bytes_to_read == 0)) {
      goto clean_up;
    }
do_loop:
    while (client_data_p->ringbuffer[client_data_p->ringbufferpos].size < bytes_to_read) {
      gpio_write_one_pin_value(client_data_p->gpio_led_handle, 1, KO_OLIMEX_MOD_MPU6050_GPIO_LED_PIN_LABEL);
//      err = i2c_smbus_read_block_data(client_data_p->client,
//                                      MPU6050_RA_FIFO_R_W,
//                                      client_data_p->ringbuffer[client_data_p->ringbufferpos].data);
      err = i2c_smbus_read_i2c_block_data(client_data_p->client,
                                          MPU6050_RA_FIFO_R_W,
                                          bytes_to_read,
                                          client_data_p->ringbuffer[client_data_p->ringbufferpos].data);
      gpio_write_one_pin_value(client_data_p->gpio_led_handle, 0, KO_OLIMEX_MOD_MPU6050_GPIO_LED_PIN_LABEL);
      if (unlikely(err < 0)) {
        pr_err("%s: i2c_smbus_read_i2c_block_data(0x%x) failed: %d\n", __FUNCTION__,
               MPU6050_RA_FIFO_R_W,
               err);
        goto clean_up;
      }
      client_data_p->ringbuffer[client_data_p->ringbufferpos].size += err;
      goto do_dispatch;
    }
  }
  else {
    // *NOTE*: read one complete set of data only...
    bytes_to_read = KO_OLIMEX_MOD_MPU6050_DEVICE_BLOCK_LENGTH;
    gpio_write_one_pin_value(client_data_p->gpio_led_handle, 1, KO_OLIMEX_MOD_MPU6050_GPIO_LED_PIN_LABEL);
    client_data_p->ringbuffer[client_data_p->ringbufferpos].size =
     i2c_smbus_read_i2c_block_data(client_data_p->client,
                                   MPU6050_RA_ACCEL_XOUT_H, bytes_to_read,
                                   client_data_p->ringbuffer[client_data_p->ringbufferpos].data);
    gpio_write_one_pin_value(client_data_p->gpio_led_handle, 0, KO_OLIMEX_MOD_MPU6050_GPIO_LED_PIN_LABEL);
    if (unlikely(client_data_p->ringbuffer[client_data_p->ringbufferpos].size != bytes_to_read))
      pr_warn("%s: i2c_smbus_read_i2c_block_data(0x%x) failed: %d (expected: %d)\n", __FUNCTION__,
              MPU6050_RA_ACCEL_XOUT_H,
              client_data_p->ringbuffer[client_data_p->ringbufferpos].size,
              bytes_to_read);
  }

do_dispatch:
//  pr_debug("%s: read %d bytes into (ring-)buffer (slot# %d)...\n", __FUNCTION__,
//           bytes_read,
//           client_data_p->ringbufferpos);

//    i2c_mpu6050_wq_dump(client_data_p, client_data_p->ringbufferpos, 0);
  if (likely((nofifo == 0) &&
             (client_data_p->ringbuffer[client_data_p->ringbufferpos].size < bytes_to_read))) goto do_loop;

  mutex_unlock(&client_data_p->sync_lock);

  return;

clean_up:
  client_data_p->ringbuffer[client_data_p->ringbufferpos].used = 0;

  mutex_unlock(&client_data_p->sync_lock);
}

void
i2c_mpu6050_wq_dump(struct i2c_mpu6050_client_data_t* clientData_in, int slot_in, int lock_in)
{
  s16 accel_x, accel_y, accel_z, temp, gyro_x, gyro_y, gyro_z;
  struct timeval time;
  struct tm tm;

//  pr_debug("%s called.\n", __FUNCTION__);

  if (likely(lock_in))
    mutex_lock(&clientData_in->sync_lock);

  // sanity check(s)
  if (unlikely(clientData_in->ringbuffer[slot_in].used == 0))
    pr_warn("%s slot %d not in use.\n", __FUNCTION__,
            slot_in);
  if (unlikely(clientData_in->ringbuffer[slot_in].size < KO_OLIMEX_MOD_MPU6050_DEVICE_BLOCK_LENGTH))
    pr_warn("%s slot %d dataset incomplete (%d/%d byte(s)).\n", __FUNCTION__,
            slot_in,
            clientData_in->ringbuffer[slot_in].size, KO_OLIMEX_MOD_MPU6050_DEVICE_BLOCK_LENGTH);
  if (unlikely(clientData_in->ringbuffer[slot_in].timestamp == 0))
    pr_warn("%s slot %d missing timestamp.\n", __FUNCTION__,
            slot_in);

  time = ns_to_timeval(clientData_in->ringbuffer[slot_in].timestamp);
  time_to_tm(time.tv_sec, 0, &tm);

  i2c_mpu6050_device_extract_data(clientData_in->ringbuffer[slot_in].data,
                                  &accel_x, &accel_y, &accel_z,
                                  &temp,
                                  &gyro_x, &gyro_y, &gyro_z);

  //  pr_info("#%d: acceleration (x,y,z): %.5f,%.5f,%.5f g\n", slot_in,
  //          (float)value_x / (float)ACCEL_SENSITIVITY,
  //          (float)value_y / (float)ACCEL_SENSITIVITY,
  //          (float)value_z / (float)ACCEL_SENSITIVITY);
  //          ((float)value_x / (float)THERMO_SENSITIVITY) + THERMO_OFFSET);
  //          (float)value_x / (float)GYRO_SENSITIVITY,
  //          (float)value_y / (float)GYRO_SENSITIVITY,
  //          (float)value_z / (float)GYRO_SENSITIVITY);
  pr_debug("%d:%d:%d.%ld: #%d: acceleration: %d,%d,%d, temperature: %d, angular velocity: %d,%d,%d\n",
           tm.tm_hour, tm.tm_min, tm.tm_sec, time.tv_usec,
           slot_in,
           accel_x, accel_y, accel_z,
           temp,
           gyro_x, gyro_y, gyro_z);

  if (likely(lock_in))
    mutex_unlock(&clientData_in->sync_lock);
}

int
i2c_mpu6050_wq_init(struct i2c_mpu6050_client_data_t* clientData_in)
{
  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely(!clientData_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -EINVAL;
  }
  if (unlikely(clientData_in->workqueue)) {
    pr_warn("%s: workqueue already initialized\n", __FUNCTION__);
    return 0;
  }

//  memset(clientData_in->ringbuffer, 0, sizeof(clientData_in->ringbuffer));

  clientData_in->workqueue =
      create_singlethread_workqueue(KO_OLIMEX_MOD_MPU6050_DRIVER_WQ_NAME);
  if (unlikely(IS_ERR(clientData_in->workqueue))) {
    pr_err("%s: create_singlethread_workqueue(%s) failed: %ld\n", __FUNCTION__,
           KO_OLIMEX_MOD_MPU6050_DRIVER_WQ_NAME,
           PTR_ERR(clientData_in->workqueue));
    return PTR_ERR(clientData_in->workqueue);
  }

  INIT_WORK(&clientData_in->work_read.work, i2c_mpu6050_wq_read_handler);
  clientData_in->work_read.client = clientData_in->client;

  return 0;
}

void
i2c_mpu6050_wq_fini(struct i2c_mpu6050_client_data_t* clientData_in)
{
  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely(!clientData_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return;
  }

  destroy_workqueue(clientData_in->workqueue);
}
