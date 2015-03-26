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

#include "olimex_mod_mpu6050_timer.h"

#include <linux/err.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <media/rc-core.h>

#include "olimex_mod_mpu6050_defines.h"
#include "olimex_mod_mpu6050_types.h"

enum hrtimer_restart
i2c_mpu6050_hr_timer_handler(struct hrtimer* timer_in)
{
  struct i2c_mpu6050_client_data_t* client_data_p = NULL;
  int err;
//  ktime_t now;
  ktime_t delay;
  u64 missed_intervals;

//  pr_debug("%s called (%ld).\n", __FUNCTION__,
//           jiffies);

  // sanity check(s)
  if (unlikely(!timer_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return HRTIMER_NORESTART;
  }
  client_data_p = container_of(timer_in,
                               struct i2c_mpu6050_client_data_t,
                               hr_timer);
  if (unlikely(!client_data_p)) {
    pr_err("%s: container_of() failed\n", __FUNCTION__);
    return HRTIMER_NORESTART;
  }

  err = queue_work(client_data_p->workqueue,
                   &client_data_p->work_read.work);
//  if (unlikely(err == 0)) {
//    pr_warn("%s: queue_work() failed\n", __FUNCTION__);
//  }

  // now = hrtimer_cb_get_time(timeri);
//  now = ktime_get();
  delay = ktime_set(0, MS_TO_NS(KO_OLIMEX_MOD_MPU6050_TIMER_DELAY_MS));
//  hrtimer_forward(&i2c_mpu6050_hr_timer, now, delay);
  missed_intervals = hrtimer_forward_now(&client_data_p->hr_timer,
                                         delay);

  return HRTIMER_RESTART;
}

int
i2c_mpu6050_timer_init(struct i2c_mpu6050_client_data_t* clientData_in)
{
  ktime_t delay;
  int err;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely(!clientData_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -ENOSYS;
  }

  hrtimer_init(&clientData_in->hr_timer,
               CLOCK_MONOTONIC,
               HRTIMER_MODE_REL);
  clientData_in->hr_timer.function = &i2c_mpu6050_hr_timer_handler;

  delay = ktime_set(0, MS_TO_NS(KO_OLIMEX_MOD_MPU6050_TIMER_DELAY_MS));
  err = hrtimer_start(&clientData_in->hr_timer,
                      delay,
                      HRTIMER_MODE_REL);
  if (unlikely(err)) {
    pr_err("%s: hrtimer_start() failed: %d\n", __FUNCTION__,
           err);
    return err;
  }
  pr_debug("%s: started timer (interval: %dms) @%ld\n", __FUNCTION__,
           KO_OLIMEX_MOD_MPU6050_TIMER_DELAY_MS, jiffies);

  return 0;
}

void
i2c_mpu6050_timer_fini(struct i2c_mpu6050_client_data_t* clientData_in)
{
  int err;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely(!clientData_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return;
  }

  err = hrtimer_cancel(&clientData_in->hr_timer);
  if (unlikely(err < 0)) {
    pr_err("%s: hrtimer_cancel() failed: %d\n", __FUNCTION__,
           err);
    return;
  }

  pr_debug("%s: cancelled timer\n", __FUNCTION__);
}
