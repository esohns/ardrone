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

#include <linux/pm.h>

static int i2c_mpu6050_pm_prepare(struct device*);
static void i2c_mpu6050_pm_complete(struct device*);
static int i2c_mpu6050_pm_suspend(struct device*);
static int i2c_mpu6050_pm_suspend_late(struct device*);
static int i2c_mpu6050_pm_resume(struct device*);
static int i2c_mpu6050_pm_resume_early(struct device*);
static int i2c_mpu6050_pm_freeze(struct device*);
static int i2c_mpu6050_pm_freeze_late(struct device*);
static int i2c_mpu6050_pm_thaw(struct device*);
static int i2c_mpu6050_pm_thaw_early(struct device*);
static int i2c_mpu6050_pm_poweroff(struct device*);
static int i2c_mpu6050_pm_poweroff_late(struct device*);
static int i2c_mpu6050_pm_restore(struct device*);
static int i2c_mpu6050_pm_restore_early(struct device*);
//
static int i2c_mpu6050_pm_suspend_noirq(struct device*);
static int i2c_mpu6050_pm_resume_noirq(struct device*);
static int i2c_mpu6050_pm_freeze_noirq(struct device*);
static int i2c_mpu6050_pm_thaw_noirq(struct device*);
static int i2c_mpu6050_pm_poweroff_noirq(struct device*);
static int i2c_mpu6050_pm_restore_noirq(struct device*);
//
static int i2c_mpu6050_pm_runtime_suspend(struct device*);
static int i2c_mpu6050_pm_runtime_resume(struct device*);
static int i2c_mpu6050_pm_runtime_idle(struct device*);

static struct dev_pm_ops i2c_mpu6050_pm_ops = {
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
