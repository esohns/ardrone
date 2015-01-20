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

static
int
i2c_mpu6050_pm_prepare(struct device* device_in)
{
  return 0;
}
static
void
i2c_mpu6050_pm_complete(struct device* device_in)
{

}
static
int
i2c_mpu6050_pm_suspend(struct device* device_in)
{
  return 0;
}
static
int
i2c_mpu6050_pm_resume(struct device* device_in)
{
  return 0;
}
static
int
i2c_mpu6050_pm_freeze(struct device* device_in)
{
  return 0;
}
static
int
i2c_mpu6050_pm_thaw(struct device* device_in)
{
  return 0;
}
static
int
i2c_mpu6050_pm_poweroff(struct device* device_in)
{
  return 0;
}
static
int
i2c_mpu6050_pm_restore(struct device* device_in)
{
  return 0;
}
static
int
i2c_mpu6050_pm_suspend_late(struct device* device_in)
{
  return 0;
}
static
int
i2c_mpu6050_pm_resume_early(struct device* device_in)
{
  return 0;
}
static
int
i2c_mpu6050_pm_freeze_late(struct device* device_in)
{
  return 0;
}
static
int
i2c_mpu6050_pm_thaw_early(struct device* device_in)
{
  return 0;
}
static
int
i2c_mpu6050_pm_poweroff_late(struct device* device_in)
{
  return 0;
}
static
int
i2c_mpu6050_pm_restore_early(struct device* device_in)
{
  return 0;
}
static
int
i2c_mpu6050_pm_suspend_noirq(struct device* device_in)
{
  return 0;
}
static
int
i2c_mpu6050_pm_resume_noirq(struct device* device_in)
{
  return 0;
}
static
int
i2c_mpu6050_pm_freeze_noirq(struct device* device_in)
{
  return 0;
}
static
int
i2c_mpu6050_pm_thaw_noirq(struct device* device_in)
{
  return 0;
}
static
int
i2c_mpu6050_pm_poweroff_noirq(struct device* device_in)
{
  return 0;
}
static
int
i2c_mpu6050_pm_restore_noirq(struct device* device_in)
{
  return 0;
}
static
int
i2c_mpu6050_pm_runtime_suspend(struct device* device_in)
{
  return 0;
}
static
int
i2c_mpu6050_pm_runtime_resume(struct device* device_in)
{
  return 0;
}
static
int
i2c_mpu6050_pm_runtime_idle(struct device* device_in)
{
  return 0;
}
