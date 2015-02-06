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

#include <linux/i2c.h>
#include <linux/workqueue.h>

// *NOTE*: taken from i2cdevlib (see also: http://www.i2cdevlib.com/devices/mpu6050#source)
#include "MPU6050.h"

#include "olimex_mod_mpu6050_defines.h"
#include "olimex_mod_mpu6050_device.h"
#include "olimex_mod_mpu6050_main.h"
#include "olimex_mod_mpu6050_types.h"

void
i2c_mpu6050_wq_fifo_handler(struct work_struct* work_in)
{
  struct fifo_work_t* work_p;
  struct i2c_mpu6050_client_data_t* client_data_p;
  int i, j;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  work_p = (struct fifo_work_t*)work_in;
  if (!work_p) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return;
  }
  client_data_p = (struct i2c_mpu6050_client_data_t*)i2c_get_clientdata(work_p->client);
  if (IS_ERR(client_data_p)) {
    pr_err("%s: i2c_get_clientdata() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_data_p));
    return;
  }

  while (client_data_p->fifostorepos > 0) {
    pr_debug("%s: %d entries in fifo store\n", __FUNCTION__,
             client_data_p->fifostorepos);
    pr_debug("%s: sending %d bytes\n", __FUNCTION__,
             client_data_p->fifostore[0].size);

    // left-shift the FIFO store
    for (i = 1; i < FIFOSTORESIZE; i++) {
      for (j = 0; j < client_data_p->fifostore[i].size; j++)
        client_data_p->fifostore[i-1].data[j] = client_data_p->fifostore[i].data[j];
      client_data_p->fifostore[i-1].size = client_data_p->fifostore[i].size;
    }
  
    client_data_p->fifostorepos--;
  }

  pr_debug("%s: work exit\n", __FUNCTION__);
}

void
i2c_mpu6050_wq_read_handler(struct work_struct* work_in)
{
  struct fifo_work_t* work_p;
  struct i2c_mpu6050_client_data_t* client_data_p;
  int err;
  s32 bytes_read, bytes_to_read = 0;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  work_p = (struct fifo_work_t*)work_in;
  if (!work_p) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return;
  }
  client_data_p = (struct i2c_mpu6050_client_data_t*)i2c_get_clientdata(work_p->client);
  if (IS_ERR(client_data_p)) {
    pr_err("%s: i2c_get_clientdata() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_data_p));
    return;
  }

//  memset(&client_data_p->ringbuffer[client_data_p->ringbufferpos].data,
//         0,
//         RINGBUFFERDATASIZE);
  client_data_p->ringbuffer[client_data_p->ringbufferpos].used = 1;

  if (fifo) {
    // *TODO*: use a single transaction for efficiency...
//    reg = MPU6050_RA_FIFO_R_W;
//    gpio_write_one_pin_value(client_data_p->gpio_led_handle, 1, GPIO_LED_PIN_LABEL);
//    err = i2c_master_send(client_data_p->client,
//                          &reg, 1);
//    if (err != 1) {
//      gpio_write_one_pin_value(client_data_p->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
//      pr_err("%s: i2c_master_send() failed: %d\n", __FUNCTION__,
//             err);
//      return;
//    }
//    err = i2c_master_recv(client_data_p->client,
//                          client_data_p->ringbuffer[client_data_p->ringbufferpos].data,
//                          RINGBUFFERDATASIZE);
//    gpio_write_one_pin_value(client_data_p->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
//    if (err <= 0) {
//      pr_err("%s: i2c_master_recv() failed: %d\n", __FUNCTION__,
//             err);
//      return;
//    }
    bytes_to_read = i2c_mpu6050_device_fifo_count(client_data_p);
    if (bytes_to_read < 0) {
      pr_err("%s: i2c_mpu6050_device_fifo_count() failed: %d\n", __FUNCTION__,
             bytes_to_read);
      return;
    }
    bytes_read = 0;
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
      if (err < 0) {
        pr_err("%s: i2c_smbus_read_i2c_block_data() failed: %d\n", __FUNCTION__,
               err);
        return;
      }
      bytes_read += err;
      goto do_dispatch;
    }
  }
  else {
    // *NOTE*: read one complete set of data only...
    gpio_write_one_pin_value(client_data_p->gpio_led_handle, 1, GPIO_LED_PIN_LABEL);
    bytes_read = i2c_smbus_read_i2c_block_data(client_data_p->client,
                                               MPU6050_RA_ACCEL_XOUT_H, BLOCK_LENGTH,
                                               client_data_p->ringbuffer[client_data_p->ringbufferpos].data);
    gpio_write_one_pin_value(client_data_p->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
    if (bytes_read != BLOCK_LENGTH) {
      pr_err("%s: i2c_smbus_read_i2c_block_data() failed: %d (expected: %d)\n", __FUNCTION__,
             bytes_read, BLOCK_LENGTH);
      return;
    }
  }

do_dispatch:
//  pr_debug("%s: read %d bytes into ringbuffer slot# %.2x...\n", __FUNCTION__,
//           err, client_data_p->ringbufferpos);
  pr_debug("%s: read %d bytes into ringbuffer slot# %.2x...\n", __FUNCTION__,
           bytes_read, client_data_p->ringbufferpos);

//  client_data_p->ringbuffer[client_data_p->ringbufferpos].size = err;
  client_data_p->ringbuffer[client_data_p->ringbufferpos].size = bytes_read;
  client_data_p->ringbuffer[client_data_p->ringbufferpos].completed = 1;

  client_data_p->ringbufferpos++;
  if (client_data_p->ringbufferpos == RINGBUFFERSIZE)
    client_data_p->ringbufferpos = 0;
  if (fifo && (bytes_read < bytes_to_read)) goto do_loop;

  pr_debug("%s: work exit\n", __FUNCTION__);

  return;
}

int
i2c_mpu6050_wq_init(struct i2c_mpu6050_client_data_t* clientData_in)
{
  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (!clientData_in) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -ENOSYS;
  }
  if (clientData_in->workqueue) {
    pr_warn("%s: workqueue already initialized\n", __FUNCTION__);
    return 0;
  }

  clientData_in->workqueue = create_singlethread_workqueue(KO_OLIMEX_MOD_MPU6050_WQ_NAME);
  if (IS_ERR(clientData_in->workqueue)) {
    pr_err("%s: create_singlethread_workqueue(%s) failed: %ld, aborting\n", __FUNCTION__,
           KO_OLIMEX_MOD_MPU6050_WQ_NAME,
           PTR_ERR(clientData_in->workqueue));
    return -ENOSYS;
  }

  INIT_WORK(&clientData_in->work_processfifostore.work, i2c_mpu6050_wq_fifo_handler);
  clientData_in->work_processfifostore.client = clientData_in->client;
  INIT_WORK(&clientData_in->work_read.work, i2c_mpu6050_wq_read_handler);
  clientData_in->work_read.client = clientData_in->client;

  return 0;
}

void
i2c_mpu6050_wq_fini(struct i2c_mpu6050_client_data_t* clientData_in)
{
  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (!clientData_in) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return;
  }

  destroy_workqueue(clientData_in->workqueue);
  clientData_in->workqueue = NULL;
}
