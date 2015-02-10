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

#include "olimex_mod_mpu6050_device.h"

#include <linux/delay.h>

// *NOTE*: taken from i2cdevlib (see also: http://www.i2cdevlib.com/devices/mpu6050#source)
#include "MPU6050.h"

#include "olimex_mod_mpu6050_defines.h"
#include "olimex_mod_mpu6050_main.h"
#include "olimex_mod_mpu6050_types.h"

int
i2c_mpu6050_device_ping(struct i2c_mpu6050_client_data_t*  clientData_in)
{
  u8 reg;
  int value;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely(!clientData_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -EINVAL;
  }

  // read who am i (#117, see page 45)
  /** @see MPU6050_RA_WHO_AM_I
    * @see MPU6050_WHO_AM_I_BIT
    * @see MPU6050_ADDRESS_AD0_LOW
    */
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
  reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_WHO_AM_I);
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
  value = ((reg & ~(0xFF << (MPU6050_WHO_AM_I_BIT + 1))) & (0xFF << 1));

  return ((value == MPU6050_ADDRESS_AD0_LOW) ? 0 : -ENODEV);
}

void
i2c_mpu6050_device_reset(struct i2c_mpu6050_client_data_t* clientData_in, int dataOnly_in, int wait_in)
{
  u8 reg;
  s32 bytes_written;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely(!clientData_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return;
  }

  if (dataOnly_in == 0) {
    // reset device (#107, see page 40)
    /** @see MPU6050_RA_PWR_MGMT_1
      * @see MPU6050_PWR1_DEVICE_RESET_BIT
      */
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
    reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_PWR_MGMT_1);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
    reg |= (0x01 << MPU6050_PWR1_DEVICE_RESET_BIT); // --> reset device
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
    bytes_written = i2c_smbus_write_byte_data(clientData_in->client,
                                              MPU6050_RA_PWR_MGMT_1, reg);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
    if (unlikely(bytes_written)) {
      pr_err("%s: i2c_smbus_write_byte_data(0x%x,0x%x) failed: %d\n", __FUNCTION__,
             MPU6050_RA_PWR_MGMT_1, (int)reg,
             bytes_written);
      return;
    }

    if (likely(wait_in))
    {
      msleep(RESET_DELAY_MS);
      gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
      reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_PWR_MGMT_1);
      gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
      if (unlikely(reg & MPU6050_PWR1_DEVICE_RESET_BIT))
        pr_warn("%s: device reset not complete after %dms\n", __FUNCTION__,
                RESET_DELAY_MS);
    }
    pr_debug("%s: device has been reset...\n", __FUNCTION__);
  }

  if (unlikely(dataOnly_in)) {
//    // step1: reset signal paths (#104, see page 37)
//    /** @see MPU6050_RA_SIGNAL_PATH_RESET
//      * @see MPU6050_PATHRESET_GYRO_RESET_BIT
//      * @see MPU6050_PATHRESET_ACCEL_RESET_BIT
//      * @see MPU6050_PATHRESET_TEMP_RESET_BIT
//      */
//    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
//    reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_SIGNAL_PATH_RESET);
//    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
//    reg |= ((0x01 << MPU6050_PATHRESET_GYRO_RESET_BIT)  |
//            (0x01 << MPU6050_PATHRESET_ACCEL_RESET_BIT) |
//            (0x01 << MPU6050_PATHRESET_TEMP_RESET_BIT));
//    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
//    bytes_written = i2c_smbus_write_byte_data(clientData_in->client,
//                                              MPU6050_RA_SIGNAL_PATH_RESET, reg);
//    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
//    if (bytes_written) {
//      pr_err("%s: i2c_smbus_write_byte_data(0x%x,0x%x) failed: %d\n", __FUNCTION__,
//             MPU6050_RA_SIGNAL_PATH_RESET, reg,
//             bytes_written);
//      return;
//    }
//    pr_debug("%s: reset signal paths...\n", __FUNCTION__);

    // step2: reset signal paths and registers (#106, see page 38)
    /** @see MPU6050_RA_USER_CTRL
    * @see MPU6050_USERCTRL_SIG_COND_RESET_BIT
    */
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
    reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_USER_CTRL);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
    reg |= (0x01 << MPU6050_USERCTRL_SIG_COND_RESET_BIT); // --> reset signal paths & registers
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
    bytes_written = i2c_smbus_write_byte_data(clientData_in->client,
                                              MPU6050_RA_USER_CTRL, reg);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
    if (unlikely(bytes_written)) {
      pr_err("%s: i2c_smbus_write_byte_data(0x%x,0x%x) failed: %d\n", __FUNCTION__,
             MPU6050_RA_USER_CTRL, (int)reg,
             bytes_written);
      return;
    }
    pr_debug("%s: reset signal paths and registers...\n", __FUNCTION__);

    if (wait_in)
      msleep(RESET_DELAY_MS);

    if (nofifo == 0) {
      // step3: reset FIFO (#106, see page 38)
      /** @see MPU6050_RA_USER_CTRL
        * @see MPU6050_USERCTRL_FIFO_EN_BIT
        * @see MPU6050_USERCTRL_FIFO_RESET_BIT
        */
      gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
      reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_USER_CTRL);
      gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
      reg &= ~(0x01 << MPU6050_USERCTRL_FIFO_EN_BIT);   // --> disable FIFO
      reg |= (0x01 << MPU6050_USERCTRL_FIFO_RESET_BIT); // --> reset FIFO
      gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
      bytes_written = i2c_smbus_write_byte_data(clientData_in->client,
                                                MPU6050_RA_USER_CTRL, reg);
      gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
      if (unlikely(bytes_written)) {
        pr_err("%s: i2c_smbus_write_byte_data(0x%x,0x%x) failed: %d\n", __FUNCTION__,
               MPU6050_RA_USER_CTRL, reg,
               bytes_written);
        return;
      }
      pr_debug("%s: reset FIFO...\n", __FUNCTION__);
    }

    pr_debug("%s: device has been reset (warm)...\n", __FUNCTION__);
  }
}

int
i2c_mpu6050_device_fifo_count(struct i2c_mpu6050_client_data_t* clientData_in)
{
  s32 bytes_read;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely(!clientData_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -EINVAL;
  }

  // read FIFO buffer count (#114-115, see page 43)
  /** @see MPU6050_RA_FIFO_COUNTH
    * @see MPU6050_RA_FIFO_COUNTL
    */
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
  bytes_read = i2c_smbus_read_word_data(clientData_in->client, MPU6050_RA_FIFO_COUNTH);
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
  // *NOTE*: i2c uses a big-endian transfer syntax
  be16_to_cpus(((__be16*)&bytes_read) + 1);

  return bytes_read;
}

int
i2c_mpu6050_device_low_power_mode(struct i2c_mpu6050_client_data_t* clientData_in)
{
  u8 reg;
  int value;
  s32 bytes_written;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely(!clientData_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -EINVAL;
  }

  // *NOTE*: low power mode means the device powers-down (only the I2C interface
  // is enabled) and takes single (accelerometer [configurable]) measurements at
  // fixed intervals
  // see: RM-MPU-6000A-00v4.2.pdf page 42

  // step0: reset device
  i2c_mpu6050_device_reset(clientData_in, 0, 1);

  // step1a: configure cycle mode (#108, see page 42)
  /** @see MPU6050_RA_PWR_MGMT_2
    * @see MPU6050_PWR2_LP_WAKE_CTRL_BIT
    * @see MPU6050_PWR2_LP_WAKE_CTRL_LENGTH
    * @see MPU6050_WAKE_FREQ_1P25
    */
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
  reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_PWR_MGMT_2);
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
  value = (reg >> (MPU6050_PWR2_LP_WAKE_CTRL_BIT - 1));
  if (unlikely(value != MPU6050_WAKE_FREQ_1P25)) {
    reg |= (MPU6050_WAKE_FREQ_1P25 << (MPU6050_PWR2_LP_WAKE_CTRL_BIT - 1));
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
    bytes_written = i2c_smbus_write_byte_data(clientData_in->client,
                                              MPU6050_RA_PWR_MGMT_2, reg);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
    if (unlikely(bytes_written)) {
      pr_err("%s: i2c_smbus_write_byte_data(0x%x,0x%x) failed: %d\n", __FUNCTION__,
             MPU6050_RA_PWR_MGMT_2, (int)reg,
             bytes_written);
      return bytes_written;
    }
    pr_debug("%s: set wake up frequency: %d (was: %d)...\n", __FUNCTION__,
             MPU6050_WAKE_FREQ_1P25,
             value);
  }
  // step1b: disable thermometer (#107, see page 40)
  /** @see MPU6050_RA_PWR_MGMT_1
    * @see MPU6050_PWR1_TEMP_DIS_BIT
    */
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
  reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_PWR_MGMT_1);
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
  value = ((reg & ~(0x01 << MPU6050_PWR1_TEMP_DIS_BIT)) >> MPU6050_PWR1_TEMP_DIS_BIT);
  if (likely(value == 0)) {
    reg |= (0x01 << MPU6050_PWR1_TEMP_DIS_BIT); // --> disable
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
    bytes_written = i2c_smbus_write_byte_data(clientData_in->client,
                                              MPU6050_RA_PWR_MGMT_1, reg);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
    if (unlikely(bytes_written)) {
      pr_err("%s: i2c_smbus_write_byte_data(0x%x,0x%x) failed: %d\n", __FUNCTION__,
             MPU6050_RA_PWR_MGMT_1, (int)reg,
             bytes_written);
      return bytes_written;
    }
    pr_debug("%s: disabled thermometer...\n", __FUNCTION__);
  }
  // step1c: disable gyroscope (#108, see page 42)
  /** @see MPU6050_RA_PWR_MGMT_2
    * @see MPU6050_PWR2_STBY_XG_BIT
    * @see MPU6050_PWR2_STBY_YG_BIT
    * @see MPU6050_PWR2_STBY_ZG_BIT
    */
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
  reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_PWR_MGMT_2);
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
  value = (reg & (0x01 << MPU6050_PWR2_STBY_XG_BIT) >> MPU6050_PWR2_STBY_XG_BIT);
  if (likely(value == 0))
    reg |= (0x01 << MPU6050_PWR2_STBY_XG_BIT);
  value = (reg & (0x01 << MPU6050_PWR2_STBY_YG_BIT) >> MPU6050_PWR2_STBY_YG_BIT);
  if (likely(value == 0))
    reg |= (0x01 << MPU6050_PWR2_STBY_YG_BIT);
  value = (reg & (0x01 << MPU6050_PWR2_STBY_ZG_BIT) >> MPU6050_PWR2_STBY_ZG_BIT);
  if (likely(value == 0))
    reg |= (0x01 << MPU6050_PWR2_STBY_ZG_BIT);
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
  bytes_written = i2c_smbus_write_byte_data(clientData_in->client,
                                            MPU6050_RA_PWR_MGMT_2, reg);
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
  if (unlikely(bytes_written)) {
    pr_err("%s: i2c_smbus_write_byte_data(0x%x,0x%x) failed: %d\n", __FUNCTION__,
           MPU6050_RA_PWR_MGMT_2, (int)reg,
           bytes_written);
    return bytes_written;
  }
  pr_debug("%s: disabled gyroscope...\n", __FUNCTION__);

  // step2: enable cycle mode (#107, see page 40)
  /** @see MPU6050_RA_PWR_MGMT_1
    * @see MPU6050_PWR1_CYCLE_BIT
    */
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
  reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_PWR_MGMT_1);
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
  value = ((reg &= (0x01 << MPU6050_PWR1_CYCLE_BIT)) >> MPU6050_PWR1_CYCLE_BIT);
  if (likely(value == 0)) {
    reg |= (0x01 << MPU6050_PWR1_CYCLE_BIT);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
    bytes_written = i2c_smbus_write_byte_data(clientData_in->client,
                                              MPU6050_RA_PWR_MGMT_1, reg);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
    if (unlikely(bytes_written)) {
      pr_err("%s: i2c_smbus_write_byte_data(0x%x,0x%x) failed: %d\n", __FUNCTION__,
             MPU6050_RA_PWR_MGMT_1, (int)reg,
             bytes_written);
      return bytes_written;
    }
    pr_debug("%s: enabled cycles mode...\n", __FUNCTION__);
  }

  // step3: disable sleep mode (#107, see page 40)
  /** @see MPU6050_RA_PWR_MGMT_1
    * @see MPU6050_PWR1_SLEEP_BIT
    */
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
  reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_PWR_MGMT_1);
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
  value = ((reg &= (0x01 << MPU6050_PWR1_SLEEP_BIT)) >> MPU6050_PWR1_SLEEP_BIT);
  if (unlikely(value)) {
    reg |= (0x01 << MPU6050_PWR1_SLEEP_BIT);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
    bytes_written = i2c_smbus_write_byte_data(clientData_in->client,
                                              MPU6050_RA_PWR_MGMT_1, reg);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
    if (unlikely(bytes_written)) {
      pr_err("%s: i2c_smbus_write_byte_data(0x%x,0x%x) failed: %d\n", __FUNCTION__,
             MPU6050_RA_PWR_MGMT_1, (int)reg,
             bytes_written);
      return bytes_written;
    }
    pr_debug("%s: disabled sleep mode...\n", __FUNCTION__);
  }

  return 0;
}

int
i2c_mpu6050_device_init(struct i2c_mpu6050_client_data_t* clientData_in)
{
  u8 reg;
  int value;
  s32 bytes_written;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely(!clientData_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -EINVAL;
  }

  // *NOTE*: the device comes up in sleep mode during power-up
  // see: RM-MPU-6000A-00v4.2.pdf page 9

  // step0: disable sleep mode (#107, see page 40)
  /** @see MPU6050_RA_PWR_MGMT_1
    * @see MPU6050_PWR1_SLEEP_BIT
    */
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
  reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_PWR_MGMT_1);
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
  value = ((reg &= (0x01 << MPU6050_PWR1_SLEEP_BIT)) >> MPU6050_PWR1_SLEEP_BIT);
  if (likely(value)) {
    reg &= ~(0x01 << MPU6050_PWR1_SLEEP_BIT);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
    bytes_written = i2c_smbus_write_byte_data(clientData_in->client,
                                              MPU6050_RA_PWR_MGMT_1, reg);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
    if (unlikely(bytes_written)) {
      pr_err("%s: i2c_smbus_write_byte_data(0x%x,0x%x) failed: %d\n", __FUNCTION__,
             MPU6050_RA_PWR_MGMT_1, (int)reg,
             bytes_written);
      return bytes_written;
    }
    pr_debug("%s: disabled sleep mode...\n", __FUNCTION__);
  }

  // step1: set clocksource (#107, see page 40)
  /** CLK_SEL | Clock Source
    * --------+--------------------------------------
    * 0       | Internal oscillator
    * 1       | PLL with X Gyro reference
    * 2       | PLL with Y Gyro reference
    * 3       | PLL with Z Gyro reference
    * 4       | PLL with external 32.768kHz reference
    * 5       | PLL with external 19.2MHz reference
    * 6       | Reserved
    * 7       | Stops the clock and keeps the timing generator in reset
    * --------+--------------------------------------
    * @see MPU6050_RA_PWR_MGMT_1
    * @see MPU6050_PWR1_CLKSEL_BIT
    * @see MPU6050_PWR1_CLKSEL_LENGTH
    */
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
  reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_PWR_MGMT_1);
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
  value = (reg & ~(0xFF << MPU6050_PWR1_CLKSEL_LENGTH));
  if (likely(value != MPU6050_CLOCK_PLL_XGYRO)) {
    reg &= (0xFF << MPU6050_PWR1_CLKSEL_LENGTH);
    reg |= MPU6050_CLOCK_PLL_XGYRO;
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
    bytes_written = i2c_smbus_write_byte_data(clientData_in->client,
                                              MPU6050_RA_PWR_MGMT_1, reg);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
    if (unlikely(bytes_written)) {
      pr_err("%s: i2c_smbus_write_byte_data(0x%x,0x%x) failed: %d\n", __FUNCTION__,
             MPU6050_RA_PWR_MGMT_1, (int)reg,
             bytes_written);
      return bytes_written;
    }
    pr_debug("%s: set clock source: %d (was: %d)...\n", __FUNCTION__,
             MPU6050_CLOCK_PLL_XGYRO,
             value);
  }

  // step2a: set dynamic low-pass filter (#26, see page 13)
  /** DLPF_CFG | Bandwidth (Hz estimate)
    * ---------+--------------------------------------
    * 0        | 256
    * 1        | 188
    * 2        | 98
    * 3        | 42
    * 4        | 20
    * 5        | 10
    * 6        | 5
    * 7        | Reserved
    * ---------+--------------------------------------
    * @see MPU6050_RA_CONFIG
    * @see MPU6050_CFG_DLPF_CFG_BIT
    * @see MPU6050_CFG_DLPF_CFG_LENGTH
    * @see MPU6050_DLPF_BW_256
    */
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
  reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_CONFIG);
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
  value = (reg & ~(0xFF << MPU6050_CFG_DLPF_CFG_LENGTH));
  if (unlikely(value != MPU6050_DLPF_BW_256)) {
    reg &= (0xFF << MPU6050_CFG_DLPF_CFG_LENGTH);
    reg |= MPU6050_DLPF_BW_256; // --> highest bandwidth
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
    bytes_written = i2c_smbus_write_byte_data(clientData_in->client,
                                              MPU6050_RA_CONFIG, reg);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
    if (unlikely(bytes_written)) {
      pr_err("%s: i2c_smbus_write_byte_data(0x%x,0x%x) failed: %d\n", __FUNCTION__,
             MPU6050_RA_CONFIG, (int)reg,
             bytes_written);
      return bytes_written;
    }
    pr_debug("%s: set dynamic low-pass filter: %d (was: %d)...\n", __FUNCTION__,
             MPU6050_DLPF_BW_256,
             value);
  }
  // step2b: set sample rate divider (#25, see page 11)
  /** @see MPU6050_RA_SMPLRT_DIV
    */
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
  reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_SMPLRT_DIV);
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
  value = reg;
  if (unlikely(value != 0)) {
    reg = 0; // --> no divider (highest sample rate [== gyroscope output rate])
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
    bytes_written = i2c_smbus_write_byte_data(clientData_in->client,
                                              MPU6050_RA_SMPLRT_DIV, reg);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
    if (unlikely(bytes_written)) {
      pr_err("%s: i2c_smbus_write_byte_data(0x%x,%d) failed: %d\n", __FUNCTION__,
             MPU6050_RA_SMPLRT_DIV, (int)reg,
             bytes_written);
      return bytes_written;
    }
    pr_debug("%s: set sample rate divider: %d (was: %d)...\n", __FUNCTION__,
             reg,
             value);
  }

  // step3a: enable thermometer (#107, see page 40)
  /** @see MPU6050_RA_PWR_MGMT_1
    * @see MPU6050_PWR1_TEMP_DIS_BIT
    */
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
  reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_PWR_MGMT_1);
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
  value = ((reg & ~(0x01 << MPU6050_PWR1_TEMP_DIS_BIT)) >> MPU6050_PWR1_TEMP_DIS_BIT);
  if (unlikely(value == 1)) {
    reg &= ~(0x01 << MPU6050_PWR1_TEMP_DIS_BIT); // --> enable
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
    bytes_written = i2c_smbus_write_byte_data(clientData_in->client,
                                              MPU6050_RA_PWR_MGMT_1, reg);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
    if (unlikely(bytes_written)) {
      pr_err("%s: i2c_smbus_write_byte_data(0x%x,0x%x) failed: %d\n", __FUNCTION__,
             MPU6050_RA_PWR_MGMT_1, (int)reg,
             bytes_written);
      return bytes_written;
    }
    pr_debug("%s: enabled thermometer...\n", __FUNCTION__);
  }
  // step3b: set gyroscope range (#27, see page 14)
  /** FS_SEL  | Full Scale Range
    * --------+--------------------------------------
    * 0       | +- 250  °/s
    * 1       | +- 500  °/s
    * 2       | +- 1000 °/s
    * 3       | +- 2000 °/s
    * --------+--------------------------------------
    * @see MPU6050_RA_GYRO_CONFIG
    * @see MPU6050_GCONFIG_FS_SEL_BIT
    * @see MPU6050_GCONFIG_FS_SEL_LENGTH
    * @see MPU6050_GYRO_FS_250
    */
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
  reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_GYRO_CONFIG);
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
  value = (reg >> (MPU6050_GCONFIG_FS_SEL_BIT - 1)) &
          ~(0xFF << MPU6050_GCONFIG_FS_SEL_LENGTH);
  if (unlikely(value != MPU6050_GYRO_FS_250)) {
    reg |= (MPU6050_GYRO_FS_250 << MPU6050_GCONFIG_FS_SEL_BIT);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
    bytes_written = i2c_smbus_write_byte_data(clientData_in->client,
                                              MPU6050_RA_GYRO_CONFIG, reg);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
    if (unlikely(bytes_written)) {
      pr_err("%s: i2c_smbus_write_byte_data(%x,%d) failed: %d\n", __FUNCTION__,
             MPU6050_RA_GYRO_CONFIG, (int)reg,
             bytes_written);
      return bytes_written;
    }
    pr_debug("%s: set gyroscope range: %d (was: %d)...\n", __FUNCTION__,
             MPU6050_GYRO_FS_250,
             value);
  }
  // step3c: set accelerometer range (#28, see page 15)
  /** AFS_SEL | Full Scale Range
    * --------+--------------------------------------
    * 0       | +- 2  g
    * 1       | +- 4  g
    * 2       | +- 8  g
    * 3       | +- 16 g
    * --------+--------------------------------------
    * @see MPU6050_RA_ACCEL_CONFIG
    * @see MPU6050_ACONFIG_AFS_SEL_BIT
    * @see MPU6050_ACONFIG_AFS_SEL_LENGTH
    * @see MPU6050_ACCEL_FS_2
    */
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
  reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_ACCEL_CONFIG);
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
  value = (reg >> (MPU6050_ACONFIG_AFS_SEL_BIT - 1)) &
          ~(0xFF << MPU6050_ACONFIG_AFS_SEL_LENGTH);
  if (unlikely(value != MPU6050_ACCEL_FS_2)) {
    reg |= (MPU6050_ACCEL_FS_2 << MPU6050_ACONFIG_AFS_SEL_BIT);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
    bytes_written = i2c_smbus_write_byte_data(clientData_in->client,
                                              MPU6050_RA_ACCEL_CONFIG, reg);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
    if (unlikely(bytes_written)) {
      pr_err("%s: i2c_smbus_write_byte_data(0x%x,0x%x) failed: %d\n", __FUNCTION__,
             MPU6050_RA_ACCEL_CONFIG, (int)reg,
             bytes_written);
      return bytes_written;
    }
    pr_debug("%s: set accelerometer range: %d (was: %d)...\n", __FUNCTION__,
             MPU6050_ACCEL_FS_2,
             value);
  }

  // step4: enable FIFO ?
  if (nofifo == 0) {
    // step4a: configure FIFO (#35, see page 16)
    /** @see MPU6050_RA_FIFO_EN
      * @see MPU6050_TEMP_FIFO_EN_BIT
      * @see MPU6050_XG_FIFO_EN_BIT
      * @see MPU6050_YG_FIFO_EN_BIT
      * @see MPU6050_ZG_FIFO_EN_BIT
      * @see MPU6050_ACCEL_FIFO_EN_BIT
      */
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
    reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_FIFO_EN);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
    value = ((reg & (0x01 << MPU6050_TEMP_FIFO_EN_BIT)) >> MPU6050_TEMP_FIFO_EN_BIT);
    value &= ((reg & (0x01 << MPU6050_XG_FIFO_EN_BIT)) >> MPU6050_XG_FIFO_EN_BIT);
    value &= ((reg & (0x01 << MPU6050_YG_FIFO_EN_BIT)) >> MPU6050_YG_FIFO_EN_BIT);
    value &= ((reg & (0x01 << MPU6050_ZG_FIFO_EN_BIT)) >> MPU6050_ZG_FIFO_EN_BIT);
    value &= ((reg & (0x01 << MPU6050_ACCEL_FIFO_EN_BIT)) >> MPU6050_ACCEL_FIFO_EN_BIT);
    if (likely(value == 0)) {
      reg |= ((0x01 << MPU6050_TEMP_FIFO_EN_BIT) |  // --> thermometer
              (0x01 << MPU6050_XG_FIFO_EN_BIT)   |  // --> gyroscope (x-axis)
              (0x01 << MPU6050_YG_FIFO_EN_BIT)   |  // --> gyroscope (y-axis)
              (0x01 << MPU6050_ZG_FIFO_EN_BIT)   |  // --> gyroscope (z-axis)
              (0x01 << MPU6050_ACCEL_FIFO_EN_BIT)); // --> accelerometer
      gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
      bytes_written = i2c_smbus_write_byte_data(clientData_in->client,
                                                MPU6050_RA_FIFO_EN, reg);
      gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
      if (unlikely(bytes_written)) {
        pr_err("%s: i2c_smbus_write_byte_data(0x%x,0x%x) failed: %d\n", __FUNCTION__,
               MPU6050_RA_FIFO_EN, (int)reg,
               bytes_written);
        return bytes_written;
      }
      pr_debug("%s: configured FIFO...\n", __FUNCTION__);
    }

    // step4b: disable FIFO overfow interrupt (#56, see page 27)
    /** @see MPU6050_RA_INT_ENABLE
      * @see MPU6050_INTERRUPT_FIFO_OFLOW_BIT
      */
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
    reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_INT_ENABLE);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
    value = ((reg &= (0x01 << MPU6050_INTERRUPT_FIFO_OFLOW_BIT)) >> MPU6050_INTERRUPT_FIFO_OFLOW_BIT);
    if (unlikely(value)) {
      reg &= ~(0x01 << MPU6050_INTERRUPT_FIFO_OFLOW_BIT);
      gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
      bytes_written = i2c_smbus_write_byte_data(clientData_in->client,
                                                MPU6050_RA_INT_ENABLE, reg);
      gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
      if (unlikely(bytes_written)) {
        pr_err("%s: i2c_smbus_write_byte_data(0x%x,0x%x) failed: %d\n", __FUNCTION__,
               MPU6050_RA_INT_ENABLE, (int)reg,
               bytes_written);
        return bytes_written;
      }
      pr_debug("%s: disabled FIFO overflow interrupt...\n", __FUNCTION__);
    }

    // step4c: enable FIFO (#106, see page 38)
    /** @see MPU6050_RA_USER_CTRL
      * @see MPU6050_USERCTRL_FIFO_EN_BIT
      */
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
    reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_USER_CTRL);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
    value = ((reg & (0x01 << MPU6050_USERCTRL_FIFO_EN_BIT)) >> MPU6050_USERCTRL_FIFO_EN_BIT);
    if (likely(value == 0)) {
      reg |= (0x01 << MPU6050_USERCTRL_FIFO_EN_BIT); // --> enable
      gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
      bytes_written = i2c_smbus_write_byte_data(clientData_in->client,
                                                MPU6050_RA_USER_CTRL, reg);
      gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
      if (unlikely(bytes_written)) {
        pr_err("%s: i2c_smbus_write_byte_data(0x%x,0x%x) failed: %d\n", __FUNCTION__,
               MPU6050_RA_USER_CTRL, (int)reg,
               bytes_written);
        return bytes_written;
      }
      pr_debug("%s: enabled FIFO...\n", __FUNCTION__);
    }
  }

  // step5: enable (data ready-) interrupt(s) ?
  if (noirq == 0) {
    // step5a: configure (data ready-) interrupt(s) (#55, see page 26)
    /** @see MPU6050_RA_INT_PIN_CFG
      * @see MPU6050_INTCFG_INT_LEVEL_BIT
      * @see MPU6050_INTCFG_INT_OPEN_BIT
      * @see MPU6050_INTCFG_LATCH_INT_EN_BIT
      * @see MPU6050_INTCFG_INT_RD_CLEAR_BIT
      * @see [MPU6050_INTCFG_FSYNC_INT_LEVEL_BIT]
      * @see [MPU6050_INTCFG_FSYNC_INT_EN_BIT]
      */
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
    reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_INT_PIN_CFG);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
    value = ((reg &= (0x01 << MPU6050_INTCFG_INT_LEVEL_BIT)) >> MPU6050_INTCFG_INT_LEVEL_BIT);
    if (unlikely(value))
      reg &= ~(0x01 << MPU6050_INTCFG_INT_LEVEL_BIT);       // 0 --> active-high
    value = ((reg &= (0x01 << MPU6050_INTCFG_INT_OPEN_BIT)) >> MPU6050_INTCFG_INT_OPEN_BIT);
    if (unlikely(value))
      reg &= ~(0x01 << MPU6050_INTCFG_INT_OPEN_BIT);        // 0 --> push-pull
    value = ((reg &= (0x01 << MPU6050_INTCFG_LATCH_INT_EN_BIT)) >> MPU6050_INTCFG_LATCH_INT_EN_BIT);
    if (unlikely(value))
      reg &= ~(0x01 << MPU6050_INTCFG_LATCH_INT_EN_BIT);    // 0 (latch off) --> 50us pulse
    value = ((reg &= (0x01 << MPU6050_INTCFG_INT_RD_CLEAR_BIT)) >> MPU6050_INTCFG_INT_RD_CLEAR_BIT);
    // *TODO*: this behaviour may be wrong...
    if (likely(value == 0))
      reg |=  (0x01 << MPU6050_INTCFG_INT_RD_CLEAR_BIT);    // 1 --> clear interrupt on ANY read
//    value = ((reg &= (0x01 << MPU6050_INTCFG_FSYNC_INT_LEVEL_BIT)) >> MPU6050_INTCFG_FSYNC_INT_LEVEL_BIT);
//    if (unlikely(value))
//      reg &= ~(0x01 << MPU6050_INTCFG_FSYNC_INT_LEVEL_BIT); // 0 --> FSYNC active-high
//    value = ((reg &= (0x01 << MPU6050_INTCFG_FSYNC_INT_EN_BIT)) >> MPU6050_INTCFG_FSYNC_INT_EN_BIT);
//    if (unlikely(value))
//      reg &= ~(0x01 << MPU6050_INTCFG_FSYNC_INT_EN_BIT);    // 0 --> FSYNC INT disabled
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
    bytes_written = i2c_smbus_write_byte_data(clientData_in->client,
                                              MPU6050_RA_INT_PIN_CFG, reg);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
    if (unlikely(bytes_written)) {
      pr_err("%s: i2c_smbus_write_byte_data(0x%x,0x%x) failed: %d\n", __FUNCTION__,
             MPU6050_RA_INT_PIN_CFG, (int)reg,
             bytes_written);
      return bytes_written;
    }
    pr_debug("%s: configured data-ready interrupt...\n", __FUNCTION__);


    // step5b: disable waiting for external data (#36, see page 17)
    /** @see MPU6050_RA_I2C_MST_CTRL
      * @see MPU6050_WAIT_FOR_ES_BIT
      */
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
    reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_I2C_MST_CTRL);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
    value = ((reg &= (0x01 << MPU6050_WAIT_FOR_ES_BIT)) >> MPU6050_WAIT_FOR_ES_BIT);
    if (unlikely(value)) {
      reg &= ~(0x01 << MPU6050_WAIT_FOR_ES_BIT);
      gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
      bytes_written = i2c_smbus_write_byte_data(clientData_in->client,
                                                MPU6050_RA_I2C_MST_CTRL, reg);
      gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
      if (unlikely(bytes_written)) {
        pr_err("%s: i2c_smbus_write_byte_data(0x%x,0x%x) failed: %d\n", __FUNCTION__,
               MPU6050_RA_I2C_MST_CTRL, (int)reg,
               bytes_written);
        return bytes_written;
      }
      pr_debug("%s: disabled waiting for external data...\n", __FUNCTION__);
    }

    // step5c: enable (data ready-) interrupt(s) (#56, see page 27)
    /** @see MPU6050_RA_INT_ENABLE
      * @see MPU6050_INTERRUPT_DATA_RDY_BIT
      */
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
    reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_INT_ENABLE);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
    value = ((reg &= (0x01 << MPU6050_INTERRUPT_DATA_RDY_BIT)) >> MPU6050_INTERRUPT_DATA_RDY_BIT);
    if (likely(value == 0)) {
      reg |= (0x01 << MPU6050_INTERRUPT_DATA_RDY_BIT);
      gpio_write_one_pin_value(clientData_in->gpio_led_handle, 255, GPIO_LED_PIN_LABEL);
      bytes_written = i2c_smbus_write_byte_data(clientData_in->client,
                                                MPU6050_RA_INT_ENABLE, reg);
      gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
      if (unlikely(bytes_written)) {
        pr_err("%s: i2c_smbus_write_byte_data(0x%x,0x%x) failed: %d\n", __FUNCTION__,
               MPU6050_RA_INT_ENABLE, (int)reg,
               bytes_written);
        return bytes_written;
      }
      pr_debug("%s: enabled data-ready interrupt...\n", __FUNCTION__);
    }
  }

  return 0;
}

void
i2c_mpu6050_device_fini(struct i2c_mpu6050_client_data_t* clientData_in)
{
  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely(!clientData_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return;
  }

  i2c_mpu6050_device_reset(clientData_in, 0, 0);
}
