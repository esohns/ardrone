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
  struct read_work_t* work_p;
  struct i2c_mpu6050_client_data_t* client_data_p;
  s32 bytes_to_read, bytes_read = 0;
  int err;

//  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  work_p = (struct read_work_t*)work_in;
  if (unlikely(!work_p)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return;
  }
  client_data_p = (struct i2c_mpu6050_client_data_t*)i2c_get_clientdata(work_p->client);
  if (unlikely(IS_ERR(client_data_p))) {
    pr_err("%s: i2c_get_clientdata() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_data_p));
    return;
  }

  mutex_lock(&client_data_p->sync_lock);
//  memset(&client_data_p->ringbuffer[client_data_p->ringbufferpos].data,
//         0,
//         RINGBUFFERDATASIZE);
  client_data_p->ringbuffer[client_data_p->ringbufferpos].used = 1;

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
    while (bytes_read < bytes_to_read) {
      gpio_write_one_pin_value(client_data_p->gpio_led_handle, 1, GPIO_LED_PIN_LABEL);
//      err = i2c_smbus_read_block_data(client_data_p->client,
//                                      MPU6050_RA_FIFO_R_W,
//                                      client_data_p->ringbuffer[client_data_p->ringbufferpos].data);
      err = i2c_smbus_read_i2c_block_data(client_data_p->client,
                                          MPU6050_RA_FIFO_R_W,
                                          bytes_to_read,
                                          client_data_p->ringbuffer[client_data_p->ringbufferpos].data);
      gpio_write_one_pin_value(client_data_p->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
      if (unlikely(err < 0)) {
        pr_err("%s: i2c_smbus_read_i2c_block_data(0x%x) failed: %d\n", __FUNCTION__,
               MPU6050_RA_FIFO_R_W,
               err);
        goto clean_up;
      }
      bytes_read += err;
      goto do_dispatch;
    }
  }
  else {
    // *NOTE*: read one complete set of data only...
    bytes_to_read = BLOCK_LENGTH;
    gpio_write_one_pin_value(client_data_p->gpio_led_handle, 1, GPIO_LED_PIN_LABEL);
    bytes_read = i2c_smbus_read_i2c_block_data(client_data_p->client,
                                               MPU6050_RA_ACCEL_XOUT_H, bytes_to_read,
                                               client_data_p->ringbuffer[client_data_p->ringbufferpos].data);
    gpio_write_one_pin_value(client_data_p->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
    if (unlikely(bytes_read != bytes_to_read)) {
      pr_err("%s: i2c_smbus_read_i2c_block_data(0x%x) failed: %d (expected: %d)\n", __FUNCTION__,
             MPU6050_RA_ACCEL_XOUT_H,
             bytes_read, bytes_to_read);
      goto clean_up;
    }
  }

do_dispatch:
//  pr_debug("%s: read %d bytes into (ring-)buffer (slot# %d)...\n", __FUNCTION__,
//           bytes_read,
//           client_data_p->ringbufferpos);

  client_data_p->ringbuffer[client_data_p->ringbufferpos].size = bytes_read;
  client_data_p->ringbuffer[client_data_p->ringbufferpos].completed = 1;

  client_data_p->ringbufferpos++;
  if (unlikely(client_data_p->ringbufferpos == RINGBUFFER_SIZE))
    client_data_p->ringbufferpos = 0;
  if (likely((nofifo == 0) &&
             (bytes_read < bytes_to_read))) goto do_loop;
  mutex_unlock(&client_data_p->sync_lock);

  return;

clean_up:
  client_data_p->ringbuffer[client_data_p->ringbufferpos].used = 0;
  mutex_unlock(&client_data_p->sync_lock);
}

int
i2c_mpu6050_wq_init(struct i2c_mpu6050_client_data_t* clientData_in)
{
  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely(!clientData_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -ENOSYS;
  }
  if (unlikely(clientData_in->workqueue)) {
    pr_warn("%s: workqueue already initialized\n", __FUNCTION__);
    return 0;
  }

  clientData_in->workqueue = create_singlethread_workqueue(KO_OLIMEX_MOD_MPU6050_WQ_NAME);
  if (unlikely(IS_ERR(clientData_in->workqueue))) {
    pr_err("%s: create_singlethread_workqueue(%s) failed: %ld, aborting\n", __FUNCTION__,
           KO_OLIMEX_MOD_MPU6050_WQ_NAME,
           PTR_ERR(clientData_in->workqueue));
    return -ENOSYS;
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
  clientData_in->workqueue = NULL;
}
