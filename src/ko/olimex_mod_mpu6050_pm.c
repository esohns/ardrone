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

#include "olimex_mod_mpu6050_pm.h"

#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/pm.h>
#include <linux/printk.h>

#include "olimex_mod_mpu6050_device.h"

struct dev_pm_ops i2c_mpu6050_pm_ops = {
  .prepare = i2c_mpu6050_pm_prepare,                 // abstain from probe()ing new devices
  .complete = i2c_mpu6050_pm_complete,               // resume probe()ing new devices
  .suspend = i2c_mpu6050_pm_suspend,                 // sleep (preserve main memory)
  .suspend_late = i2c_mpu6050_pm_suspend_late,       // continue operations started by suspend()
  .resume = i2c_mpu6050_pm_resume,                   // wake up (from sleep)
  .resume_early = i2c_mpu6050_pm_resume_early,       // prepare to execute resume()
  .freeze = i2c_mpu6050_pm_freeze,                   // (prepare) deep-sleep (e.g. suspend to disk [hibernate])
  .freeze_late = i2c_mpu6050_pm_freeze_late,         // continue operations started by freeze()
  .thaw = i2c_mpu6050_pm_thaw,                       // (resume from) deep-sleep (e.g. load suspend to disk image)
  .thaw_early = i2c_mpu6050_pm_thaw_early,           // prepare to execute thaw()
  .poweroff = i2c_mpu6050_pm_poweroff,               // hibernate
  .poweroff_late = i2c_mpu6050_pm_poweroff_late,     // continue operations started by poweroff()
  .restore = i2c_mpu6050_pm_restore,                 // wake up (from hibernation)
  .restore_early = i2c_mpu6050_pm_restore_early,     // prepare to execute restore()
  //
  .suspend_noirq = i2c_mpu6050_pm_suspend_noirq,     // complete the actions started by suspend()
  .resume_noirq = i2c_mpu6050_pm_resume_noirq,       // prepare for the execution of resume()
  .freeze_noirq = i2c_mpu6050_pm_freeze_noirq,       // complete the actions started by freeze()
  .thaw_noirq = i2c_mpu6050_pm_thaw_noirq,           // prepare for the execution of thaw()
  .poweroff_noirq = i2c_mpu6050_pm_poweroff_noirq,   // complete the actions started by poweroff()
  .restore_noirq = i2c_mpu6050_pm_restore_noirq,     // prepare for the execution of restore()
  //
  .runtime_suspend = i2c_mpu6050_pm_runtime_suspend, // (prepare for) runtime suspend
  .runtime_resume = i2c_mpu6050_pm_runtime_resume,   // resume from runtime suspend
  .runtime_idle = i2c_mpu6050_pm_runtime_idle        // check for idleness
};

int
i2c_mpu6050_pm_prepare(struct device* device_in)
{
  pr_debug("%s called.\n", __FUNCTION__);
  
  return 0;
}

void
i2c_mpu6050_pm_complete(struct device* device_in)
{
  pr_debug("%s called.\n", __FUNCTION__);

}

int
i2c_mpu6050_pm_suspend(struct device* device_in)
{
  struct i2c_client* client_p;
  struct i2c_mpu6050_client_data_t* client_data_p;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely(!device_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -EINVAL;
  }
  client_p = to_i2c_client(device_in);
  if (unlikely(IS_ERR(client_p))) {
    pr_err("%s: to_i2c_client() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_p));
    return PTR_ERR(client_p);
  }
  client_data_p = (struct i2c_mpu6050_client_data_t*)i2c_get_clientdata(client_p);
  if (unlikely(IS_ERR(client_data_p))) {
    pr_err("%s: i2c_get_clientdata() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_data_p));
    return PTR_ERR(client_data_p);
  }

  i2c_mpu6050_device_reset(client_data_p, 0, 1);

  return 0;
}

int
i2c_mpu6050_pm_resume(struct device* device_in)
{
  struct i2c_client* client_p;
  struct i2c_mpu6050_client_data_t* client_data_p;
  int err;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely(!device_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -EINVAL;
  }
  client_p = to_i2c_client(device_in);
  if (unlikely(IS_ERR(client_p))) {
    pr_err("%s: to_i2c_client() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_p));
    return PTR_ERR(client_p);
  }
  client_data_p = (struct i2c_mpu6050_client_data_t*)i2c_get_clientdata(client_p);
  if (unlikely(IS_ERR(client_data_p))) {
    pr_err("%s: i2c_get_clientdata() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_data_p));
    return PTR_ERR(client_data_p);
  }

  err = i2c_mpu6050_device_init(client_data_p);
  if (unlikely(err)) {
    pr_err("%s: i2c_mpu6050_device_init() failed: %d\n", __FUNCTION__,
           err);
    return err;
  }

  return 0;
}

int
i2c_mpu6050_pm_freeze(struct device* device_in)
{
  pr_debug("%s called.\n", __FUNCTION__);

  return 0;
}

int
i2c_mpu6050_pm_thaw(struct device* device_in)
{
  pr_debug("%s called.\n", __FUNCTION__);

  return 0;
}

int
i2c_mpu6050_pm_poweroff(struct device* device_in)
{
  pr_debug("%s called.\n", __FUNCTION__);

  return 0;
}

int
i2c_mpu6050_pm_restore(struct device* device_in)
{
  pr_debug("%s called.\n", __FUNCTION__);

  return 0;
}

int
i2c_mpu6050_pm_suspend_late(struct device* device_in)
{
  pr_debug("%s called.\n", __FUNCTION__);

  return 0;
}

int
i2c_mpu6050_pm_resume_early(struct device* device_in)
{
  pr_debug("%s called.\n", __FUNCTION__);

  return 0;
}

int
i2c_mpu6050_pm_freeze_late(struct device* device_in)
{
  pr_debug("%s called.\n", __FUNCTION__);

  return 0;
}

int
i2c_mpu6050_pm_thaw_early(struct device* device_in)
{
  pr_debug("%s called.\n", __FUNCTION__);

  return 0;
}

int
i2c_mpu6050_pm_poweroff_late(struct device* device_in)
{
  pr_debug("%s called.\n", __FUNCTION__);

  return 0;
}

int
i2c_mpu6050_pm_restore_early(struct device* device_in)
{
  pr_debug("%s called.\n", __FUNCTION__);

  return 0;
}

int
i2c_mpu6050_pm_suspend_noirq(struct device* device_in)
{
  pr_debug("%s called.\n", __FUNCTION__);

  return 0;
}

int
i2c_mpu6050_pm_resume_noirq(struct device* device_in)
{
  pr_debug("%s called.\n", __FUNCTION__);

  return 0;
}

int
i2c_mpu6050_pm_freeze_noirq(struct device* device_in)
{
  pr_debug("%s called.\n", __FUNCTION__);

  return 0;
}

int
i2c_mpu6050_pm_thaw_noirq(struct device* device_in)
{
  pr_debug("%s called.\n", __FUNCTION__);

  return 0;
}

int
i2c_mpu6050_pm_poweroff_noirq(struct device* device_in)
{
  pr_debug("%s called.\n", __FUNCTION__);

  return 0;
}

int
i2c_mpu6050_pm_restore_noirq(struct device* device_in)
{
  pr_debug("%s called.\n", __FUNCTION__);

  return 0;
}

int
i2c_mpu6050_pm_runtime_suspend(struct device* device_in)
{
  pr_debug("%s called.\n", __FUNCTION__);

  return 0;
}

int
i2c_mpu6050_pm_runtime_resume(struct device* device_in)
{
  pr_debug("%s called.\n", __FUNCTION__);

  return 0;
}

int
i2c_mpu6050_pm_runtime_idle(struct device* device_in)
{
  pr_debug("%s called.\n", __FUNCTION__);

  return 0;
}
