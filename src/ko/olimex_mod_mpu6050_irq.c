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

#include "olimex_mod_mpu6050_irq.h"

#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/i2c.h>
#include <linux/irq.h>

#include "olimex_mod_mpu6050_defines.h"
#include "olimex_mod_mpu6050_types.h"

irqreturn_t
i2c_mpu6050_interrupt_handler(int irq_in,
                              void* dev_id_in)
{
  struct device* device_p;
  struct i2c_client* client_p;
  struct i2c_mpu6050_client_data_t* client_data_p;
//  struct irq_data* irq_data_p;
//  u32 type;
  int err;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
//  if (!dev_id_in) {
//    pr_err("%s: invalid argument\n", __FUNCTION__);
//    return -ENOSYS;
//  }
  device_p = (struct device*)dev_id_in;
  if (!device_p) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return IRQ_NONE;
  }
  client_p = to_i2c_client(device_p);
  if (IS_ERR(client_p)) {
    pr_err("%s: to_i2c_client() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_p));
    return IRQ_NONE;
  }
  client_data_p = (struct i2c_mpu6050_client_data_t*)i2c_get_clientdata(client_p);
  if (IS_ERR(client_data_p)) {
    pr_err("%s: i2c_get_clientdata() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_data_p));
    return IRQ_NONE;
  }
//  if (irq_in != client_p->irq) {
//    pr_err("%s: expected irq %d (was: %d), aborting\n", __FUNCTION__,
//           client_p->irq,
//           irq_in);
//    return IRQ_NONE;
//  }
//  irq_data_p = irq_get_irq_data(irq_in);
//  if (IS_ERR(irq_data_p)) {
//    pr_err("%s: irq_get_irq_data(%d) failed: %d\n", __FUNCTION__,
//           irq_in);
//    return IRQ_NONE;
//  }
//  type = irqd_get_trigger_type(irq_data_p);
////    type = (gpio_get_value(irq_to_gpio(irq_in)) ? rising : falling);
////    if (type != rising) {
//  if ((type & IRQ_TYPE_SENSE_MASK) != IRQ_TYPE_EDGE_RISING) {
//    pr_err("%s: expected irq %d type IRQ_TYPE_EDGE_RISING (was: %d), aborting\n", __FUNCTION__,
//           irq_in,
//           (type & IRQ_TYPE_SENSE_MASK));
//    return IRQ_NONE;
//  }

//  gpio_write_one_pin_value(client_data_p->gpio_led_handle, 1, GPIO_LED_PH02_LABEL);
  err = queue_work(client_data_p->workqueue, &client_data_p->work_read.work);
  if (err < 0) {
    pr_err("%s: queue_work failed: %d\n", __FUNCTION__,
           err);
    return IRQ_NONE;
  }
//  gpio_write_one_pin_value(client_data_p->gpio_led_handle, 0, GPIO_LED_PH02_LABEL);

  return IRQ_HANDLED;
}

int
i2c_mpu6050_irq_init(struct i2c_mpu6050_client_data_t* clientData_in)
{
  int err;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (!clientData_in) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -ENOSYS;
  }
//  client_data_p->pin_ctrl = devm_pinctrl_get(&clientData_in->client->dev);
  clientData_in->pin_ctrl = pinctrl_get(&clientData_in->client->dev);
  if (IS_ERR(clientData_in->pin_ctrl)) {
    pr_err("%s: pinctrl_get() failed: %ld\n", __FUNCTION__,
           PTR_ERR(clientData_in->pin_ctrl));
    return -ENOSYS;
  }
  clientData_in->pin_ctrl_state = pinctrl_lookup_state(clientData_in->pin_ctrl,
                                                       PINCTRL_STATE_DEFAULT);
  if (IS_ERR(clientData_in->pin_ctrl_state)) {
    pr_err("%s: pinctrl_lookup_state() failed: %ld\n", __FUNCTION__,
           PTR_ERR(clientData_in->pin_ctrl_state));
    goto error1;
  }
  err = pinctrl_select_state(clientData_in->pin_ctrl,
                             clientData_in->pin_ctrl_state);
  if (err < 0) {
    pr_err("%s: pinctrl_select_state() failed: %d\n", __FUNCTION__,
           err);
    goto error1;
  }
  err = gpio_is_valid(GPIO_UEXT4_UART4RX_PG11_PIN);
  if (err) {
    pr_err("%s: gpio_is_valid(%d) failed\n", __FUNCTION__,
           GPIO_UEXT4_UART4RX_PG11_PIN);
    goto error1;
  }
  err = gpio_is_valid(GPIO_LED_PH02_PIN);
  if (err) {
    pr_err("%s: gpio_is_valid(%d) failed\n", __FUNCTION__,
           GPIO_LED_PH02_PIN);
    goto error1;
  }
  err = devm_gpio_request(&clientData_in->client->dev,
                          GPIO_UEXT4_UART4RX_PG11_PIN,
                          GPIO_UEXT4_UART4RX_PG11_LABEL);
  if (err < 0) {
    pr_err("%s: devm_gpio_request(%d) failed: %d\n", __FUNCTION__,
           GPIO_UEXT4_UART4RX_PG11_PIN,
           err);
    goto error1;
  }
  err = gpio_direction_input(GPIO_UEXT4_UART4RX_PG11_PIN);
  if (err < 0) {
    pr_err("%s: gpio_direction_input(%d) failed: %d\n", __FUNCTION__,
           GPIO_UEXT4_UART4RX_PG11_PIN,
           err);
    goto error1;
  }
  //  err = gpio_request_one(GPIO_UEXT4_UART4RX_PG11_PIN,
  //                         GPIOF_DIR_IN,
  //                         GPIO_UEXT4_UART4RX_PG11_LABEL);
  //  if (err) {
  //    pr_err("%s: gpio_request_one(%d) failed\n", __FUNCTION__,
  //           GPIO_UEXT4_UART4RX_PG11_PIN);
  //    goto error1;
  //  }
  clientData_in->gpio_led_handle = gpio_request_ex("gpio_para",
                                                   GPIO_LED_PH02_LABEL);
  if (clientData_in->gpio_led_handle < 0) {
    pr_err("%s: gpio_request_ex(%s) failed\n", __FUNCTION__,
           GPIO_LED_PH02_LABEL);
    goto error2;
  }
  // export GPIOs to userspace
  err = gpio_export(GPIO_UEXT4_UART4RX_PG11_PIN,
                    false);
  if (err) {
    pr_err("%s: gpio_export(%d) failed\n", __FUNCTION__,
           GPIO_UEXT4_UART4RX_PG11_PIN);
    goto error3;
  }
  err = gpio_export(GPIO_LED_PH02_PIN,
                    false);
  if (err) {
    pr_err("%s: gpio_export(%d) failed\n", __FUNCTION__,
           GPIO_LED_PH02_PIN);
    goto error4;
  }
  err = gpio_export_link(&clientData_in->client->dev,
                         GPIO_UEXT4_UART4RX_PG11_LABEL,
                         GPIO_UEXT4_UART4RX_PG11_PIN);
  if (err) {
    pr_err("%s: gpio_export_link(%s,%d) failed\n", __FUNCTION__,
           GPIO_UEXT4_UART4RX_PG11_LABEL,
           GPIO_UEXT4_UART4RX_PG11_PIN);
    goto error5;
  }
  clientData_in->client->irq = gpio_to_irq(GPIO_UEXT4_UART4RX_PG11_PIN);
  if (clientData_in->client->irq < 0) {
    pr_err("%s: gpio_to_irq(%d) failed: %d\n", __FUNCTION__,
           GPIO_UEXT4_UART4RX_PG11_PIN,
           clientData_in->client->irq);
    goto error5;
  }
  pr_info("%s: GPIO %s --> PIN %d --> IRQ %d\n", __FUNCTION__,
         GPIO_UEXT4_UART4RX_PG11_LABEL,
         GPIO_UEXT4_UART4RX_PG11_PIN,
         clientData_in->client->irq);
//  gpio_chip_p = gpio_to_chip(GPIO_UEXT4_UART4RX_PG11_PIN);
//  if (!IS_ERR(gpio_chip_p)) {
//    pr_err("%s: gpio_to_chip(%d) failed: %d\n", __FUNCTION__,
//           GPIO_UEXT4_UART4RX_PG11_PIN,
//           PTR_ERR(gpio_chip_p));
//    goto error5;
//  }
//  err = gpio_lock_as_irq(gpio_chip_p,
//                         GPIO_UEXT4_UART4RX_PG11_PIN);
//  if (err) {
//    pr_err("%s: gpio_lock_as_irq(%d) failed\n", __FUNCTION__,
//           GPIO_UEXT4_UART4RX_PG11_PIN);
//    goto error5;
//  }
  err = request_irq(clientData_in->client->irq,
                    i2c_mpu6050_interrupt_handler,
                    (IRQ_TYPE_EDGE_RISING),
                    GPIO_UEXT4_UART4RX_PG11_LABEL,
                    &clientData_in->client->dev);
  if (err) {
    pr_err("%s: request_irq(%d) failed: %d\n", __FUNCTION__,
           clientData_in->client->irq,
           err);
    goto error5;
  }
  
  return 0;

//error6:
//  gpio_unlock_as_irq(gpio_chip_p,
//                     GPIO_UEXT4_UART4RX_PG11_PIN);  
error5:
  gpio_unexport(GPIO_LED_PH02_PIN);
error4:
  gpio_unexport(GPIO_UEXT4_UART4RX_PG11_PIN);
error3:
  gpio_release(clientData_in->gpio_led_handle, 1);
error2:
  devm_gpio_free(&clientData_in->client->dev,
                 GPIO_UEXT4_UART4RX_PG11_PIN);
error1:
//  devm_pinctrl_put(clientData_in->pin_ctrl);
  pinctrl_put(clientData_in->pin_ctrl);

  return -ENOSYS;  
}

void
i2c_mpu6050_irq_fini(struct i2c_mpu6050_client_data_t* clientData_in)
{
  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (!clientData_in) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return;
  }

//  gpiochip_unlock_as_irq(gpio_chip_p,
//                         GPIO_UEXT4_UART4RX_PG11_PIN);
  free_irq(clientData_in->client->irq, NULL);
  clientData_in->client->irq = -1;
  gpio_unexport(GPIO_LED_PH02_PIN);
  gpio_unexport(GPIO_UEXT4_UART4RX_PG11_PIN);
  gpio_release(clientData_in->gpio_led_handle, 1);
  clientData_in->gpio_led_handle = 0;
  devm_gpio_free(&clientData_in->client->dev,
                 GPIO_UEXT4_UART4RX_PG11_PIN);
//  devm_pinctrl_put(clientData_in->pin_ctrl);
  pinctrl_put(clientData_in->pin_ctrl);
  clientData_in->pin_ctrl = NULL;
}
