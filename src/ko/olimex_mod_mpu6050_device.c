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

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (!clientData_in) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -ENODEV;
  }

  // read who am i (#117, see page 45)
  /** @see MPU6050_RA_WHO_AM_I
    * @see MPU6050_WHO_AM_I_BIT
    * @see MPU6050_WHO_AM_I_LENGTH
    */
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 1, GPIO_LED_PIN_LABEL);
  reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_WHO_AM_I);
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
  reg &= ~(0xFF << (MPU6050_WHO_AM_I_BIT + 1));
  reg &= (0xFF << 1);

  return ((reg == WHO_AM_I_REG) ? 0 : -ENODEV);
}

void
i2c_mpu6050_device_reset(struct i2c_mpu6050_client_data_t* clientData_in, int dataOnly_in)
{
  u8 reg;
  s32 bytes_written;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (!clientData_in) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return;
  }

  if (dataOnly_in) {
    //  // step1: reset signal paths (#104, see page 37)
    //  /** @see MPU6050_RA_SIGNAL_PATH_RESET
    //    * @see MPU6050_PATHRESET_GYRO_RESET_BIT
    //    * @see MPU6050_PATHRESET_ACCEL_RESET_BIT
    //    * @see MPU6050_PATHRESET_TEMP_RESET_BIT
    //    */
    //  reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_SIGNAL_PATH_RESET);
    //  reg |= ((0x01 << MPU6050_PATHRESET_GYRO_RESET_BIT)  |
    //          (0x01 << MPU6050_PATHRESET_ACCEL_RESET_BIT) |
    //          (0x01 << MPU6050_PATHRESET_TEMP_RESET_BIT));
    //  bytes_written = i2c_smbus_write_byte_data(clientData_in->client,
    //                                            MPU6050_RA_SIGNAL_PATH_RESET,
    //                                            reg);
    //  if (bytes_written) {
    //    pr_err("%s: i2c_smbus_write_byte_data(%x,%d) failed: %d\n", __FUNCTION__,
    //           MPU6050_RA_SIGNAL_PATH_RESET, (int)reg,
    //           bytes_written);
    //    return -ENOSYS;
    //  }
    //  pr_debug("%s: reset signal paths...\n", __FUNCTION__);

    // step2: reset signal paths / clear registers (#106, see page 38)
    /** @see MPU6050_RA_USER_CTRL
    * @see MPU6050_USERCTRL_SIG_COND_RESET_BIT
    */
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 1, GPIO_LED_PIN_LABEL);
    reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_USER_CTRL);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
    reg |= (0x01 << MPU6050_USERCTRL_SIG_COND_RESET_BIT); // --> reset signal paths & registers
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 1, GPIO_LED_PIN_LABEL);
    bytes_written = i2c_smbus_write_byte_data(clientData_in->client,
                                              MPU6050_RA_USER_CTRL, reg);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
    if (bytes_written) {
      pr_err("%s: i2c_smbus_write_byte_data(%x,%d) failed: %d\n", __FUNCTION__,
             MPU6050_RA_USER_CTRL, (int)reg,
             bytes_written);
      return;
    }
    pr_debug("%s: reset signal paths & registers...\n", __FUNCTION__);

    //  // step3: reset FIFO (#106, see page 38)
    //  /** @see MPU6050_RA_USER_CTRL
    //    * @see MPU6050_USERCTRL_FIFO_EN_BIT
    //    * @see MPU6050_USERCTRL_FIFO_RESET_BIT
    //    */
    //  reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_USER_CTRL);
    //  reg &= ~(0x01 << MPU6050_USERCTRL_FIFO_EN_BIT);   // --> disable FIFO
    //  reg |= (0x01 << MPU6050_USERCTRL_FIFO_RESET_BIT); // --> reset FIFO
    //  bytes_written = i2c_smbus_write_byte_data(clientData_in->client,
    //                                            MPU6050_RA_USER_CTRL,
    //                                            reg);
    //  if (bytes_written) {
    //    pr_err("%s: i2c_smbus_write_byte_data(%x,%d) failed: %d\n", __FUNCTION__,
    //           MPU6050_RA_USER_CTRL, (int)reg,
    //           bytes_written);
    //    return;
    //  }
    //  pr_debug("%s: reset FIFO...\n", __FUNCTION__);
    pr_debug("%s: reset device (warm)...\n", __FUNCTION__);
  }
  else {
    // reset device (#107, see page 40)
    /** @see MPU6050_RA_PWR_MGMT_1
      * @see MPU6050_PWR1_DEVICE_RESET_BIT
      */
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 1, GPIO_LED_PIN_LABEL);
    reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_PWR_MGMT_1);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
    reg |= (0x01 << MPU6050_PWR1_DEVICE_RESET_BIT); // --> reset device
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 1, GPIO_LED_PIN_LABEL);
    bytes_written = i2c_smbus_write_byte_data(clientData_in->client,
                                              MPU6050_RA_PWR_MGMT_1,
                                              reg);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
    if (bytes_written) {
      pr_err("%s: i2c_smbus_write_byte_data(%x,%d) failed: %d\n", __FUNCTION__,
             MPU6050_RA_PWR_MGMT_1, (int)reg,
             bytes_written);
      return;
    }
    pr_debug("%s: reset device (cold)...\n", __FUNCTION__);
  }
}

int
i2c_mpu6050_device_fifo_count(struct i2c_mpu6050_client_data_t* clientData_in)
{
  s32 bytes_read;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (!clientData_in) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -ENOSYS;
  }

  // read FIFO buffer count (#114-115, see page 43)
  /** @see MPU6050_RA_FIFO_COUNTH
    * @see MPU6050_RA_FIFO_COUNTL
    */
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 1, GPIO_LED_PIN_LABEL);
  bytes_read = i2c_smbus_read_word_data(clientData_in->client, MPU6050_RA_FIFO_COUNTH);
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
  be16_to_cpus(((__be16*)&bytes_read) + 1);

  pr_debug("%s FIFO buffer: %d\n", __FUNCTION__,
           bytes_read);

  return bytes_read;
}

int
i2c_mpu6050_device_init(struct i2c_mpu6050_client_data_t* clientData_in)
{
  u8 reg;
  s32 bytes_written;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (!clientData_in) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -ENOSYS;
  }

  // *NOTE*: the device comes up in sleep mode during power-up
  // see: RM-MPU-6000A-00v4.2.pdf page 9

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
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 1, GPIO_LED_PIN_LABEL);
  reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_PWR_MGMT_1);
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
  pr_debug("%s: clocksource was: %d\n", __FUNCTION__,
           (reg & ~(0xFF << (MPU6050_PWR1_CLKSEL_BIT + 1))));
  reg |= ((0xFF << (MPU6050_PWR1_CLKSEL_BIT + 1)) | MPU6050_CLOCK_PLL_XGYRO);
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 1, GPIO_LED_PIN_LABEL);
  bytes_written = i2c_smbus_write_byte_data(clientData_in->client,
                                            MPU6050_RA_PWR_MGMT_1,
                                            reg);
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
  if (bytes_written) {
    pr_err("%s: i2c_smbus_write_byte_data(%x,%d) failed: %d\n", __FUNCTION__,
           MPU6050_RA_PWR_MGMT_1, (int)reg,
           bytes_written);
    return -ENOSYS;
  }
  pr_debug("%s: set clocksource...\n", __FUNCTION__);

//  // step2: set sample rate divider (#25, see page 11)
//  /** @see MPU6050_RA_SMPLRT_DIV
//    */
//  reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_SMPLRT_DIV);
//  pr_debug("%s: sample rate divider was: %d\n", __FUNCTION__,
//           reg);
//  reg = 0; // --> no divider (highest sample rate)
//  bytes_written = i2c_smbus_write_byte_data(clientData_in->client,
//                                            MPU6050_RA_SMPLRT_DIV,
//                                            reg);
//  if (bytes_written) {
//    pr_err("%s: i2c_smbus_write_byte_data(%x,%d) failed: %d\n", __FUNCTION__,
//           MPU6050_RA_SMPLRT_DIV, (int)reg,
//           bytes_written);
//    return -ENOSYS;
//  }
//  pr_debug("%s: set sample rate divider...\n", __FUNCTION__);

//  // step2: set configuration (#26, see page 13)
//  /** @see MPU6050_RA_CONFIG
//    * @see MPU6050_CFG_DLPF_CFG_BIT
//    * @see MPU6050_CFG_DLPF_CFG_LENGTH
//    */
//  reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_CONFIG);
//  pr_debug("%s: digital low-pass filter (DLPF) setting was: %d\n", __FUNCTION__,
//           (reg & ~(0xFF << (MPU6050_CFG_DLPF_CFG_BIT + 1))));
//  reg &= ~(0xFF << (MPU6050_CFG_DLPF_CFG_BIT + 1)); // --> 0 (highest bandwidth)
//  bytes_written = i2c_smbus_write_byte_data(clientData_in->client,
//                                            MPU6050_RA_CONFIG,
//                                            reg);
//  if (bytes_written) {
//    pr_err("%s: i2c_smbus_write_byte_data(%x,%d) failed: %d\n", __FUNCTION__,
//           MPU6050_RA_CONFIG, (int)reg,
//           bytes_written);
//    return -ENOSYS;
//  }
//  pr_debug("%s: set digital low-pass filter (DLPF)...\n", __FUNCTION__);

//  // step2: enable thermometer (#107, see page 40)
//  /** @see MPU6050_RA_PWR_MGMT_1
//    * @see MPU6050_PWR1_TEMP_DIS_BIT
//    */
//  reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_PWR_MGMT_1);
//  reg &= ~(0x01 << MPU6050_PWR1_TEMP_DIS_BIT); // --> enable
//  bytes_written = i2c_smbus_write_byte_data(clientData_in->client,
//                                            MPU6050_RA_PWR_MGMT_1,
//                                            reg);
//  if (bytes_written) {
//    pr_err("%s: i2c_smbus_write_byte_data(%x,%d) failed: %d\n", __FUNCTION__,
//           MPU6050_RA_PWR_MGMT_1, (int)reg,
//           bytes_written);
//    return -ENOSYS;
//  }
//  pr_debug("%s: enable thermometer...\n", __FUNCTION__);

//  // step3: set gyroscope range (#27, see page 14)
//  /** FS_SEL  | Full Scale Range
//    * --------+--------------------------------------
//    * 0       | +- 250  °/s
//    * 1       | +- 500  °/s
//    * 2       | +- 1000 °/s
//    * 3       | +- 2000 °/s
//    * --------+--------------------------------------
//    * @see MPU6050_RA_GYRO_CONFIG
//    * @see MPU6050_GCONFIG_FS_SEL_BIT
//    * @see MPU6050_GCONFIG_FS_SEL_LENGTH
//    */
//  reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_GYRO_CONFIG);
//  reg |= ((0xFF << MPU6050_GCONFIG_FS_SEL_BIT + 1) |
//          (MPU6050_GYRO_FS_250 << MPU6050_GCONFIG_FS_SEL_BIT));
//  bytes_written = i2c_smbus_write_byte_data(clientData_in->client,
//                                            MPU6050_RA_GYRO_CONFIG,
//                                            reg);
//  if (bytes_written) {
//    pr_err("%s: i2c_smbus_write_byte_data(%x,%d) failed: %d\n", __FUNCTION__,
//           MPU6050_RA_GYRO_CONFIG, (int)reg,
//           bytes_written);
//    return -ENOSYS;
//  }
//  pr_debug("%s: set gyroscope range...\n", __FUNCTION__);

//  // step4: set accelerometer range (#28, see page 15)
//  /** AFS_SEL | Full Scale Range
//    * --------+--------------------------------------
//    * 0       | +- 2  g
//    * 1       | +- 4  g
//    * 2       | +- 8  g
//    * 3       | +- 16 g
//    * --------+--------------------------------------
//    * @see MPU6050_RA_ACCEL_CONFIG
//    * @see MPU6050_ACONFIG_AFS_SEL_BIT
//    * @see MPU6050_ACONFIG_AFS_SEL_LENGTH
//    */
//  reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_ACCEL_CONFIG);
//  reg |= ((0xFF << MPU6050_ACONFIG_AFS_SEL_BIT + 1) |
//          (MPU6050_ACCEL_FS_2 << MPU6050_ACONFIG_AFS_SEL_BIT));
//  bytes_written = i2c_smbus_write_byte_data(clientData_in->client,
//                                            MPU6050_RA_ACCEL_CONFIG,
//                                            reg);
//  if (bytes_written) {
//    pr_err("%s: i2c_smbus_write_byte_data(%x,%d) failed: %d\n", __FUNCTION__,
//           MPU6050_RA_ACCEL_CONFIG, (int)reg,
//           bytes_written);
//    return -ENOSYS;
//  }
//  pr_debug("%s: set accelerometer range...\n", __FUNCTION__);

  // step2: enable FIFO ?
  if (fifo) {
    // step2a: configure FIFO (#35, see page 16)
    /** @see MPU6050_RA_FIFO_EN
    * @see MPU6050_TEMP_FIFO_EN_BIT
    * @see MPU6050_XG_FIFO_EN_BIT
    * @see MPU6050_YG_FIFO_EN_BIT
    * @see MPU6050_ZG_FIFO_EN_BIT
    * @see MPU6050_ACCEL_FIFO_EN_BIT
    */
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 1, GPIO_LED_PIN_LABEL);
    reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_FIFO_EN);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
    pr_debug("%s: FIFO setting was: 0x%x\n", __FUNCTION__,
             reg);
    reg |= ((0x01 << MPU6050_TEMP_FIFO_EN_BIT) |  // --> thermometer
            (0x01 << MPU6050_XG_FIFO_EN_BIT)   |  // --> gyroscope (x-axis)
            (0x01 << MPU6050_YG_FIFO_EN_BIT)   |  // --> gyroscope (y-axis)
            (0x01 << MPU6050_ZG_FIFO_EN_BIT)   |  // --> gyroscope (z-axis)
            (0x01 << MPU6050_ACCEL_FIFO_EN_BIT)); // --> accelerometer
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 1, GPIO_LED_PIN_LABEL);
    bytes_written = i2c_smbus_write_byte_data(clientData_in->client,
                                              MPU6050_RA_FIFO_EN,
                                              reg);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
    if (bytes_written) {
      pr_err("%s: i2c_smbus_write_byte_data(%x,%d) failed: %d\n", __FUNCTION__,
             MPU6050_RA_FIFO_EN, (int)reg,
             bytes_written);
      return -ENOSYS;
    }
    pr_debug("%s: configured FIFO...\n", __FUNCTION__);
    // step2b: enable FIFO (#106, see page 38)
    /** @see MPU6050_RA_USER_CTRL
    * @see MPU6050_USERCTRL_FIFO_EN_BIT
    */
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 1, GPIO_LED_PIN_LABEL);
    reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_USER_CTRL);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
    pr_debug("%s: FIFO setting was: 0x%x\n", __FUNCTION__,
             reg);
    reg |= (0x01 << MPU6050_USERCTRL_FIFO_EN_BIT); // --> enable
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 1, GPIO_LED_PIN_LABEL);
    bytes_written = i2c_smbus_write_byte_data(clientData_in->client,
                                              MPU6050_RA_USER_CTRL,
                                              reg);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
    if (bytes_written) {
      pr_err("%s: i2c_smbus_write_byte_data(%x,%d) failed: %d\n", __FUNCTION__,
             MPU6050_RA_USER_CTRL, (int)reg,
             bytes_written);
      return -ENOSYS;
    }
    pr_debug("%s: enabled FIFO...\n", __FUNCTION__);
  }

  // step5: enable (data ready-) interrupt(s) ?
  if (noirq == 0) {
    // step5a: configure (data ready-) interrupt(s) (#37, see page 26)
    /** @see MPU6050_RA_INT_PIN_CFG
      * @see MPU6050_INTCFG_INT_LEVEL_BIT
      * @see MPU6050_INTCFG_INT_OPEN_BIT
      * @see MPU6050_INTCFG_LATCH_INT_EN_BIT
      * @see [MPU6050_INTCFG_INT_RD_CLEAR_BIT]
      * @see [MPU6050_INTCFG_FSYNC_INT_LEVEL_BIT]
      * @see MPU6050_INTCFG_FSYNC_INT_EN_BIT
      * @see [MPU6050_INTCFG_I2C_BYPASS_EN_BIT]
      */
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 1, GPIO_LED_PIN_LABEL);
    reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_INT_PIN_CFG);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
    reg &= ~(0x01 << MPU6050_INTCFG_INT_LEVEL_BIT);       // --> active-high
    reg &= ~(0x01 << MPU6050_INTCFG_INT_OPEN_BIT);        // --> push-pull
    reg &= ~(0x01 << MPU6050_INTCFG_LATCH_INT_EN_BIT);    // --> latch off (50us pulse)
//    reg |=  (0x01 << MPU6050_INTCFG_INT_RD_CLEAR_BIT);    // --> clear INT_STATUS on ANY read
//    reg &= ~(0x01 << MPU6050_INTCFG_FSYNC_INT_LEVEL_BIT); // --> FSYNC active-high
    reg &= ~(0x01 << MPU6050_INTCFG_FSYNC_INT_EN_BIT);    // --> FSYNC INT disabled
//    reg &= ~(0x01 << MPU6050_INTCFG_I2C_BYPASS_EN_BIT);   // --> direct aux I2C bus access disabled
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 1, GPIO_LED_PIN_LABEL);
    bytes_written = i2c_smbus_write_byte_data(clientData_in->client,
                                              MPU6050_RA_INT_PIN_CFG,
                                              reg);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
    if (bytes_written) {
      pr_err("%s: i2c_smbus_write_byte_data(%x,%d) failed: %d\n", __FUNCTION__,
             MPU6050_RA_INT_PIN_CFG, (int)reg,
             bytes_written);
      return -ENOSYS;
    }
    pr_debug("%s: configured data-ready interrupt...\n", __FUNCTION__);

    // step5b: enable (data ready-) interrupt(s) (#38, see page 27)
    /** @see MPU6050_RA_INT_ENABLE
      * @see [MPU6050_INTERRUPT_FF_BIT]
      * @see [MPU6050_INTERRUPT_MOT_BIT]
      * @see [MPU6050_INTERRUPT_ZMOT_BIT]
      * @see [MPU6050_INTERRUPT_PLL_RDY_INT_BIT]
      * @see [MPU6050_INTERRUPT_DMP_INT_BIT]
      * @see MPU6050_INTERRUPT_DATA_RDY_BIT
      */
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 1, GPIO_LED_PIN_LABEL);
    reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_INT_ENABLE);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
    reg |= (0x01 << MPU6050_INTERRUPT_DATA_RDY_BIT);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 1, GPIO_LED_PIN_LABEL);
    bytes_written = i2c_smbus_write_byte_data(clientData_in->client,
                                              MPU6050_RA_INT_ENABLE,
                                              reg);
    gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
    if (bytes_written) {
      pr_err("%s: i2c_smbus_write_byte_data(%x,%d) failed: %d\n", __FUNCTION__,
             MPU6050_RA_INT_ENABLE, (int)reg,
             bytes_written);
      return -ENOSYS;
    }
    pr_debug("%s: enabled data-ready interrupt...\n", __FUNCTION__);
  }

  // step6: disable sleep mode (#107, see page 40)
  /** @see MPU6050_RA_PWR_MGMT_1
    * @see MPU6050_PWR1_SLEEP_BIT
    */
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 1, GPIO_LED_PIN_LABEL);
  reg = i2c_smbus_read_byte_data(clientData_in->client, MPU6050_RA_PWR_MGMT_1);
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
  reg |= (0x01 << MPU6050_PWR1_SLEEP_BIT);
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 1, GPIO_LED_PIN_LABEL);
  bytes_written = i2c_smbus_write_byte_data(clientData_in->client,
                                            MPU6050_RA_PWR_MGMT_1,
                                            reg);
  gpio_write_one_pin_value(clientData_in->gpio_led_handle, 0, GPIO_LED_PIN_LABEL);
  if (bytes_written) {
    pr_err("%s: i2c_smbus_write_byte_data(%x,%d) failed: %d\n", __FUNCTION__,
           MPU6050_RA_PWR_MGMT_1, (int)reg,
           bytes_written);
    return -ENOSYS;
  }
  pr_debug("%s: disabled sleep mode...\n", __FUNCTION__);

  return 0;
}

void
i2c_mpu6050_device_fini(struct i2c_mpu6050_client_data_t* clientData_in)
{
  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (!clientData_in) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return;
  }

  i2c_mpu6050_device_reset(clientData_in, 0);
}
