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
i2c_mpu6050_interrupt_handler(int irq_in, void* dev_id_in)
{
  struct device* device_p;
  struct i2c_client* client_p;
  struct i2c_mpu6050_client_data_t* client_data_p;
//  struct irq_data* irq_data_p;
//  u32 type;
  int err;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
//  if (unlikely(!dev_id_in)) {
//    pr_err("%s: invalid argument\n", __FUNCTION__);
//    return -ENOSYS;
//  }
  device_p = (struct device*)dev_id_in;
  if (unlikely(!device_p)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return IRQ_NONE;
  }
  client_p = to_i2c_client(device_p);
  if (unlikely(IS_ERR(client_p))) {
    pr_err("%s: to_i2c_client() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_p));
    return IRQ_NONE;
  }
  client_data_p = (struct i2c_mpu6050_client_data_t*)i2c_get_clientdata(client_p);
  if (unlikely(IS_ERR(client_data_p))) {
    pr_err("%s: i2c_get_clientdata() failed: %ld\n", __FUNCTION__,
           PTR_ERR(client_data_p));
    return IRQ_NONE;
  }
//  if (unlikely(irq_in != client_p->irq)) {
//    pr_err("%s: expected irq %d (was: %d), aborting\n", __FUNCTION__,
//           client_p->irq,
//           irq_in);
//    return IRQ_NONE;
//  }
//  irq_data_p = irq_get_irq_data(irq_in);
//  if (unlikely(IS_ERR(irq_data_p))) {
//    pr_err("%s: irq_get_irq_data(%d) failed: %d\n", __FUNCTION__,
//           irq_in);
//    return IRQ_NONE;
//  }
//  type = irqd_get_trigger_type(irq_data_p);
////    type = (gpio_get_value(irq_to_gpio(irq_in)) ? rising : falling);
////    if (type != rising) {
//  if (unlikely((type & IRQ_TYPE_SENSE_MASK) != IRQ_TYPE_EDGE_RISING)) {
//    pr_err("%s: expected irq %d type IRQ_TYPE_EDGE_RISING (was: %d), aborting\n", __FUNCTION__,
//           irq_in,
//           (type & IRQ_TYPE_SENSE_MASK));
//    return IRQ_NONE;
//  }

//  gpio_write_one_pin_value(client_data_p->gpio_led_handle, 1, GPIO_LED_PIn_LABEL);
  err = queue_work(client_data_p->workqueue, &client_data_p->work_read.work);
  if (unlikely(err < 0)) {
    pr_err("%s: queue_work failed: %d\n", __FUNCTION__,
           err);
    return IRQ_NONE;
  }
//  gpio_write_one_pin_value(client_data_p->gpio_led_handle, 0, GPIO_LED_PIn_LABEL);

  return IRQ_HANDLED;
}

int
i2c_mpu6050_irq_init(struct i2c_mpu6050_client_data_t* clientData_in)
{
  int err;
  struct pinctrl_state* pin_ctrl_state_p;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely(!clientData_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -ENOSYS;
  }
  if (unlikely(!gpio_is_valid(GPIO_INT_PIN))) {
    pr_err("%s: gpio_is_valid(%d) failed\n", __FUNCTION__,
           GPIO_INT_PIN);
    return -EINVAL;
  }

//  err = script_parser_fetch(GPIO_FEX_SECTION_HEADER,
//                            GPIO_INT_PIN_LABEL,
//                            (int*)&clientData_in->gpio_int_data,
//                            sizeof(script_gpio_set_t));
//  if (unlikely(err != SCRIPT_PARSER_OK)) {
//    pr_err("%s: script_parser_fetch(\"%s\",\"%s\") failed: %d\n", __FUNCTION__,
//           GPIO_FEX_SECTION_HEADER,
//           GPIO_INT_PIN_LABEL,
//           err);
//    return err;
//  }

//  clientData_in->pin_ctrl = devm_pinctrl_get(&clientData_in->client->dev);
  clientData_in->pin_ctrl = pinctrl_get(&clientData_in->client->dev);
  if (unlikely(IS_ERR(clientData_in->pin_ctrl))) {
    pr_err("%s: pinctrl_get() failed: %ld\n", __FUNCTION__,
           PTR_ERR(clientData_in->pin_ctrl));
    return PTR_ERR(clientData_in->pin_ctrl);
  }
//  clientData_in->pin_ctrl_state = pinctrl_lookup_state(clientData_in->pin_ctrl,
//                                                       PINCTRL_STATE_DEFAULT);
  pin_ctrl_state_p = pinctrl_lookup_state(clientData_in->pin_ctrl,
                                          PINCTRL_STATE_DEFAULT);
//  if (unlikely(IS_ERR(clientData_in->pin_ctrl_state))) {
//    pr_err("%s: pinctrl_lookup_state() failed: %ld\n", __FUNCTION__,
//           PTR_ERR(clientData_in->pin_ctrl_state));
//    err = PTR_ERR(clientData_in->pin_ctrl_state);
//    goto error1;
//  }
  if (unlikely(IS_ERR(pin_ctrl_state_p))) {
    pr_err("%s: pinctrl_lookup_state() failed: %ld\n", __FUNCTION__,
           PTR_ERR(pin_ctrl_state_p));
    err = PTR_ERR(pin_ctrl_state_p);
    goto error1;
  }
//  err = pinctrl_select_state(clientData_in->pin_ctrl,
//                             clientData_in->pin_ctrl_state);
  err = pinctrl_select_state(clientData_in->pin_ctrl,
                             pin_ctrl_state_p);
  if (unlikely(err)) {
    pr_err("%s: pinctrl_select_state() failed: %d\n", __FUNCTION__,
           err);
    goto error1;
  }

  clientData_in->gpio_int_handle = gpio_request_ex(GPIO_FEX_SECTION_HEADER,
                                                   GPIO_INT_PIN_LABEL);
  if (unlikely(!clientData_in->gpio_int_handle)) {
    pr_err("%s: gpio_request_ex(\"%s\") failed\n", __FUNCTION__,
           GPIO_INT_PIN_LABEL);
    err = -ENOSYS;
    goto error1;
  }

//  err = gpio_request_one(GPIO_INT_PIN,
//                         GPIOF_DIR_IN,
//                         GPIO_INT_PIN_LABEL);
//  if (unlikely(err)) {
//    pr_err("%s: gpio_request_one(%d) failed: %d\n", __FUNCTION__,
//           GPIO_INT_PIN,
//           err);
//    goto error2;
//  }
//  err = gpio_export(GPIO_INT_PIN,
//                    false);
//  if (unlikely(err)) {
//    pr_err("%s: gpio_export(%d) failed: %d\n", __FUNCTION__,
//           GPIO_INT_PIN,
//           err);
//    goto error3;
//  }
//  err = gpio_export_link(&clientData_in->client->dev,
//                         GPIO_INT_PIN_LABEL,
//                         GPIO_INT_PIN);
//  if (unlikely(err)) {
//    pr_err("%s: gpio_export_link(%s,%d) failed\n", __FUNCTION__,
//           GPIO_INT_PIN_LABEL,
//           GPIO_INT_PIN);
//    goto error4;
//  }

  clientData_in->client->irq = gpio_to_irq(GPIO_INT_PIN);
  if (unlikely(clientData_in->client->irq < 0)) {
    pr_err("%s: gpio_to_irq(%d) failed: %d\n", __FUNCTION__,
           GPIO_INT_PIN,
           clientData_in->client->irq);
    err = clientData_in->client->irq;
    goto error4;
  }
  else
    pr_info("%s: GPIO %s --> PIN %d --> IRQ %d\n", __FUNCTION__,
            GPIO_INT_PIN_LABEL,
            GPIO_INT_PIN,
            clientData_in->client->irq);
//  gpio_chip_p = gpio_to_chip(GPIO_INT_PIN);
//  if (unlikely(IS_ERR(gpio_chip_p))) {
//    pr_err("%s: gpio_to_chip(%d) failed: %d\n", __FUNCTION__,
//           GPIO_INT_PIN,
//           PTR_ERR(gpio_chip_p));
//    goto error4;
//  }
//  err = gpio_lock_as_irq(gpio_chip_p,
//                         GPIO_INT_PIN);
//  if (unlikely(err)) {
//    pr_err("%s: gpio_lock_as_irq(%d) failed\n", __FUNCTION__,
//           GPIO_INT_PIN);
//    goto error4;
//  }
  err = request_irq(clientData_in->client->irq,
                    i2c_mpu6050_interrupt_handler,
                    (IRQF_TRIGGER_RISING|IRQF_NO_SUSPEND|IRQF_SHARED),
                    GPIO_INT_PIN_LABEL,
                    &clientData_in->client->dev);
  if (unlikely(err)) {
    pr_err("%s: request_irq(%d) failed: %d\n", __FUNCTION__,
           clientData_in->client->irq,
           err);
    goto error5;
  }

  return 0;

error5:
//  gpio_unlock_as_irq(gpio_chip_p,
//                     GPIO_INT_PIN);
error4:
  gpio_unexport(GPIO_INT_PIN);
//error3:
////  devm_gpio_free(&clientData_in->client->dev,
////                 GPIO_INT_PIN);
  gpio_free(GPIO_INT_PIN);
//error2:
  gpio_release(clientData_in->gpio_int_handle, 1);
error1:
////  devm_pinctrl_put(clientData_in->pin_ctrl);
  pinctrl_put(clientData_in->pin_ctrl);

  return err;
}

void
i2c_mpu6050_irq_fini(struct i2c_mpu6050_client_data_t* clientData_in)
{
  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely(!clientData_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return;
  }

//  gpiochip_unlock_as_irq(gpio_chip_p,
//                         GPIO_INT_PIN);
  free_irq(clientData_in->client->irq, NULL);
  gpio_unexport(GPIO_INT_PIN);
////  devm_gpio_free(&clientData_in->client->dev,
////                 GPIO_INT_PIN);
  gpio_free(GPIO_INT_PIN);
  gpio_release(clientData_in->gpio_int_handle, 1);
////  devm_pinctrl_put(clientData_in->pin_ctrl);
  pinctrl_put(clientData_in->pin_ctrl);
}
