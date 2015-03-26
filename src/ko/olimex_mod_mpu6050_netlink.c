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

#include "olimex_mod_mpu6050_netlink.h"

#include <stdarg.h>
#include <linux/linkage.h>
#include <linux/printk.h>
#include <net/genetlink.h>

#include "olimex_mod_mpu6050_types.h"

struct nla_policy i2c_mpu6050_netlink_policy[NETLINK_ATTRIBUTE_MAX] = {
  [NETLINK_ATTRIBUTE_ACCEL_X] = { .type = NLA_U16 },
  [NETLINK_ATTRIBUTE_ACCEL_Y] = { .type = NLA_U16 },
  [NETLINK_ATTRIBUTE_ACCEL_Z] = { .type = NLA_U16 },
  [NETLINK_ATTRIBUTE_TEMP] = { .type = NLA_U16 },
  [NETLINK_ATTRIBUTE_GYRO_X] = { .type = NLA_U16 },
  [NETLINK_ATTRIBUTE_GYRO_Y] = { .type = NLA_U16 },
  [NETLINK_ATTRIBUTE_GYRO_Z] = { .type = NLA_U16 },
};
struct genl_family i2c_mpu6050_netlink_family = {
  .id = GENL_ID_GENERATE,
  .hdrsize = 0,
  .name = KO_OLIMEX_MOD_MPU6050_NETLINK_PROTOCOL_FAMILY_NAME,
  .version = KO_OLIMEX_MOD_MPU6050_NETLINK_PROTOCOL_VERSION,
  .maxattr = (NETLINK_ATTRIBUTE_MAX - 1),
};

int i2c_mpu6050_netlink_sequence_number = 0;

int
i2c_mpu6050_netlink_handler(struct sk_buff* buffer_in, struct genl_info* info_in)
{
  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely(!buffer_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -EINVAL;
  }

  kfree_skb(buffer_in);

  return 0;
}
struct genl_ops i2c_mpu6050_netlink_operations_record = {
  .cmd = NETLINK_COMMAND_RECORD,
  .flags = 0,
  .policy = i2c_mpu6050_netlink_policy,
  .doit = i2c_mpu6050_netlink_handler,
  .dumpit = NULL,
};

void
i2c_mpu6050_netlink_input(struct sk_buff* buffer_in)
{
//  struct nlmsghdr* header_p;
//  u8* payload_p;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely(!buffer_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return;
  }

//  header_p = (struct nlmsghdr*)buffer_p->data;
//  payload_p = NLMSG_DATA(header_p);
  kfree_skb(buffer_in);
}

void
i2c_mpu6050_netlink_forward(struct i2c_mpu6050_client_data_t* clientData_in, int slot_in)
{
  struct sk_buff* buffer_p;
  int err;
  void* header_p;
  u8* reg_p;
  u16 value_xyz;
  s16 value_t;

//  pr_debug("%s called.\n", __FUNCTION__);

//  buffer_p = alloc_skb(RINGBUFFER_DATA_SIZE, GFP_KERNEL);
//  buffer_p = genlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL);
  buffer_p = genlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL);
  if (unlikely(!buffer_p)) {
    pr_err("%s: genlmsg_new() failed\n", __FUNCTION__);
    return;
  }
  header_p = genlmsg_put(buffer_p, 0, ++i2c_mpu6050_netlink_sequence_number,
                         &i2c_mpu6050_netlink_family, 0, NETLINK_COMMAND_RECORD);
  if (unlikely(!header_p)) {
    pr_err("%s: genlmsg_put() failed\n", __FUNCTION__);
    goto error1;
  }
  reg_p = clientData_in->ringbuffer[slot_in].data;
  // *NOTE*: i2c uses a big-endian transfer syntax
  be16_to_cpus((__be16*)reg_p);
  // convert two's complement
  value_xyz = ((*(s16*)reg_p < 0) ? -((~*(u16*)reg_p) + 1)
                                  : *(s16*)reg_p);
  err = nla_put_u16(buffer_p, NETLINK_ATTRIBUTE_ACCEL_X,
                    value_xyz);
  if (unlikely(err)) {
    pr_err("%s: nla_put_u16(%d) failed: %d\n", __FUNCTION__,
           NETLINK_ATTRIBUTE_ACCEL_X, err);
    goto error1;
  }
  reg_p += 2;
  be16_to_cpus((__be16*)reg_p);
  value_xyz = ((*(s16*)reg_p < 0) ? -((~*(u16*)reg_p) + 1)
                                  : *(s16*)reg_p);
  err = nla_put_u16(buffer_p, NETLINK_ATTRIBUTE_ACCEL_Y,
                    value_xyz);
  if (unlikely(err)) {
    pr_err("%s: nla_put_u16(%d) failed: %d\n", __FUNCTION__,
           NETLINK_ATTRIBUTE_ACCEL_Y, err);
    goto error1;
  }
  reg_p += 2;
  be16_to_cpus((__be16*)reg_p);
  value_xyz = ((*(s16*)reg_p < 0) ? -((~*(u16*)reg_p) + 1)
                                  : *(s16*)reg_p);
  err = nla_put_u16(buffer_p, NETLINK_ATTRIBUTE_ACCEL_Z,
                    value_xyz);
  if (unlikely(err)) {
    pr_err("%s: nla_put_u16(%d) failed: %d\n", __FUNCTION__,
           NETLINK_ATTRIBUTE_ACCEL_Z, err);
    goto error1;
  }
  reg_p += 2;
  value_t = *(s16*)reg_p;
  err = nla_put_u16(buffer_p, NETLINK_ATTRIBUTE_TEMP,
                    (u16)value_t);
  if (unlikely(err)) {
    pr_err("%s: nla_put_u16(%d) failed: %d\n", __FUNCTION__,
           NETLINK_ATTRIBUTE_TEMP, err);
    goto error1;
  }
  reg_p += 2;
  be16_to_cpus((__be16*)reg_p);
  value_xyz = ((*(s16*)reg_p < 0) ? -((~*(u16*)reg_p) + 1)
                                  : *(s16*)reg_p);
  err = nla_put_u16(buffer_p, NETLINK_ATTRIBUTE_GYRO_X,
                    value_xyz);
  if (unlikely(err)) {
    pr_err("%s: nla_put_u16(%d) failed: %d\n", __FUNCTION__,
           NETLINK_ATTRIBUTE_GYRO_X, err);
    goto error1;
  }
  reg_p += 2;
  be16_to_cpus((__be16*)reg_p);
  value_xyz = ((*(s16*)reg_p < 0) ? -((~*(u16*)reg_p) + 1)
                                  : *(s16*)reg_p);
  err = nla_put_u16(buffer_p, NETLINK_ATTRIBUTE_GYRO_Y,
                    value_xyz);
  if (unlikely(err)) {
    pr_err("%s: nla_put_u16(%d) failed: %d\n", __FUNCTION__,
           NETLINK_ATTRIBUTE_GYRO_Y, err);
    goto error1;
  }
  reg_p += 2;
  be16_to_cpus((__be16*)reg_p);
  value_xyz = ((*(s16*)reg_p < 0) ? -((~*(u16*)reg_p) + 1)
                                  : *(s16*)reg_p);
  err = nla_put_u16(buffer_p, NETLINK_ATTRIBUTE_GYRO_Z,
                    value_xyz);
  if (unlikely(err)) {
    pr_err("%s: nla_put_u16(%d) failed: %d\n", __FUNCTION__,
           NETLINK_ATTRIBUTE_GYRO_Z, err);
    goto error1;
  }
  value_t = genlmsg_end(buffer_p, header_p);
  if (unlikely(value_t < 0)) {
    pr_err("%s: genlmsg_end() failed: %d\n", __FUNCTION__,
           value_t);
    goto error1;
  }

  err = genlmsg_unicast(&init_net, buffer_p, 0);
  if (unlikely(err)) {
    pr_err("%s: genlmsg_unicast() failed: %d\n", __FUNCTION__,
           err);
    goto error1;
  }
//  pr_debug("sent netlink message (%d bytes)...\n",
//           value_t);

  return;

error1:
  nlmsg_free(buffer_p);
}

int
i2c_mpu6050_netlink_init(struct i2c_mpu6050_client_data_t* clientData_in)
{
  int err;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely(!clientData_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -EINVAL;
  }
//  if (unlikely(clientData_in->netlink_socket)) {
//    pr_warn("%s: netlink socket already initialized\n", __FUNCTION__);
//    return 0;
//  }

  err = genl_register_family(&i2c_mpu6050_netlink_family);
  if (unlikely(err)) {
    pr_err("%s: genl_register_family() failed: %d\n", __FUNCTION__,
           err);
    return err;
  }
  pr_info("%s: registered netlink family \"%s\" version: %d --> id: %d\n", __FUNCTION__,
          i2c_mpu6050_netlink_family.name, i2c_mpu6050_netlink_family.version,
          i2c_mpu6050_netlink_family.id);
  err = genl_register_ops(&i2c_mpu6050_netlink_family,
                          &i2c_mpu6050_netlink_operations_record);
  if (unlikely(err)) {
    pr_err("%s: genl_register_ops() failed: %d\n", __FUNCTION__,
           err);
    goto error1;
  }

//  clientData_in->netlink_socket = netlink_kernel_create(&init_net,
//                                                        NETLINK_GENERIC, 0,
//                                                        i2c_mpu6050_netlink_input,
//                                                        NULL,
//                                                        THIS_MODULE);
//  if (unlikely(IS_ERR(clientData_in->netlink_socket))) {
//    pr_err("%s: netlink_kernel_create() failed: %ld\n", __FUNCTION__,
//           PTR_ERR(clientData_in->netlink_socket));
//    return PTR_ERR(clientData_in->netlink_socket);
//  }

  return 0;

error1:
  err = genl_unregister_family(&i2c_mpu6050_netlink_family);
  if (unlikely(err))
    pr_err("%s: genl_unregister_family() failed: %d\n", __FUNCTION__,
           err);

  return err;
}

void
i2c_mpu6050_netlink_fini(struct i2c_mpu6050_client_data_t* clientData_in)
{
  int err;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely(!clientData_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return;
  }
//  if (unlikely(!clientData_in->netlink_socket)) {
//    pr_err("%s: invalid argument\n", __FUNCTION__);
//    return;
//  }

  err = genl_unregister_ops(&i2c_mpu6050_netlink_family,
                            &i2c_mpu6050_netlink_operations_record);
  if (unlikely(err))
    pr_err("%s: genl_unregister_ops() failed: %d\n", __FUNCTION__,
           err);
  err = genl_unregister_family(&i2c_mpu6050_netlink_family);
  if (unlikely(err))
    pr_err("%s: genl_unregister_family() failed: %d\n", __FUNCTION__,
           err);
//  netlink_kernel_release(clientData_in->netlink_socket);
}
