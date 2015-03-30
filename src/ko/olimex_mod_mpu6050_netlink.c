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
#include <linux/delay.h>
#include <linux/inet.h>
#include <linux/kthread.h>
#include <linux/linkage.h>
#include <linux/printk.h>
#include <linux/sched.h>
#include <net/genetlink.h>
#include <net/netlink.h>

#include "olimex_mod_mpu6050_device.h"
#include "olimex_mod_mpu6050_server.h"
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
//  .name = KO_OLIMEX_MOD_MPU6050_NETLINK_PROTOCOL_FAMILY_NAME,
  .version = KO_OLIMEX_MOD_MPU6050_NETLINK_PROTOCOL_VERSION,
  .maxattr = (NETLINK_ATTRIBUTE_MAX - 1),
  .netnsok = 0,
//  .parallel_ops = 0,
  .pre_doit = NULL,
  .post_doit = NULL,
//  .mcast_bind = NULL,
//  .mcast_unbind = NULL,
//  .attrbuf,
//  .ops_list,
//  .ops,
//  .mcgrps,
//  .n_ops,
//  .n_mcgrps,
//  .mcgrp_offset,
//  .family_list,
//  .mcast_groups,
//  .module
};

struct genl_ops i2c_mpu6050_netlink_operation_record = {
  .cmd = NETLINK_COMMAND_RECORD,
//  .internal_flags,
  .flags = 0,
  .policy = i2c_mpu6050_netlink_policy,
  .doit = i2c_mpu6050_netlink_handler,
  .dumpit = NULL,
  .done = NULL,
//  .ops_list
};

struct genl_ops i2c_mpu6050_netlink_ops[] = {
  {
    .cmd = NETLINK_COMMAND_RECORD,
//  .internal_flags,
    .flags = 0,
    .policy = i2c_mpu6050_netlink_policy,
    .doit = i2c_mpu6050_netlink_handler,
    .dumpit = NULL,
    .done = NULL,
//  .ops_list
  },
};

struct genl_multicast_group i2c_mpu6050_netlink_group = {
//  .family = &i2c_mpu6050_netlink_family,
//  .list,
//  .name = KO_OLIMEX_MOD_MPU6050_NETLINK_PROTOCOL_GROUP_NAME,
//  .id,
};

//struct genl_multicast_group i2c_mpu6050_netlink_groups[] = {
//  {
////    .name = KO_OLIMEX_MOD_MPU6050_NETLINK_PROTOCOL_GROUP_NAME,
//  },
//};

//struct netlink_kernel_cfg i2c_mpu6050_netlink_socket_cfg = {
//  .groups = (1 << (KO_OLIMEX_MOD_MPU6050_NETLINK_GROUP - 1)),
//  .flags = 0,
//  .input = i2c_mpu6050_netlink_input,
//  .cb_mutex = NULL,
//  .bind = NULL,
//  .unbind = NULL,
//  .compare = NULL,
//};

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

int
i2c_mpu6050_netlink_run (void* data_in)
{
  struct i2c_mpu6050_client_data_t* client_data_p;
//  int bytes_sent;
  int err, i;

  pr_debug ("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely (!data_in)) {
    pr_err ("%s: invalid argument\n", __FUNCTION__);
    return -EINVAL;
  }
  client_data_p = (struct i2c_mpu6050_client_data_t*)data_in;
  if (unlikely (client_data_p->netlink_server->running)) {
    pr_warn ("%s: netlink server thread already running\n", __FUNCTION__);
    return -ENODEV;
  }

//  lock_kernel ();
  client_data_p->netlink_server->running = 1;
  current->flags |= PF_NOFREEZE;

  // daemonize (*NOTE*: take care with signals - after daemonize(), they are
  // disabled)
  daemonize (KO_OLIMEX_MOD_MPU6050_DRIVER_NAME);
  err = allow_signal (SIGKILL);
  if (unlikely (err)) {
    pr_err ("%s: allow_signal(%d) failed: %d\n", __FUNCTION__,
            SIGKILL, err);
    return -ENODEV;
  }
//  unlock_kernel ();

  for (;;)
  {
    if (signal_pending (current))
      break;

//    bytes_sent = i2c_mpu6050_server_receive (client_data_p->server->socket,
//                                             &client_data_p->server->address,
//                                             buf, bufsize);
//    if (bytes_sent < 0)
//    {
//      pr_err ("%s: receive() failed: %d\n", __FUNCTION__,
//              bytes_sent);
//      continue;
//    } // end IF
//    pr_debug ("%s: received %d bytes: \"%s\"\n", __FUNCTION__,
//              bytes_sent, buf);

    mutex_lock (&client_data_p->sync_lock);

    // sanity check(s)
    if ((client_data_p->ringbufferpos == -1) ||
        (client_data_p->ringbuffer[client_data_p->ringbufferpos].used == 0)) {
//      pr_debug ("%s: no data\n", __FUNCTION__);
      continue;
    }

//    i = client_data_p->server->ringbufferpos;
    i = client_data_p->ringbufferpos;
    //while (i != client_data_p->ringbufferpos)
    //{
      if (client_data_p->ringbuffer[i].used == 0) goto done;

      err = i2c_mpu6050_netlink_forward(client_data_p, i);
      if (unlikely(err)) {
        pr_err ("%s: i2c_mpu6050_netlink_forward() failed: %d\n", __FUNCTION__,
                err);
      }

done:
    //  i++;
    //  if (i == RINGBUFFER_SIZE) i = 0;
    //}
    mutex_unlock (&client_data_p->sync_lock);
    //client_data_p->server->ringbufferpos = i;

    msleep (KO_OLIMEX_MOD_MPU6050_TIMER_DELAY_MS);
  }
  pr_debug ("%s: left netlink server loop\n", __FUNCTION__);

  client_data_p->netlink_server->running = 0;

  return 0;
}

int
i2c_mpu6050_netlink_forward(struct i2c_mpu6050_client_data_t* clientData_in,
                            int slot_in)
{
  struct sk_buff* buffer_p;
  int err;
  void* header_p;
  s16 accel_x, accel_y, accel_z, t, gyro_x, gyro_y, gyro_z;

//  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely(!clientData_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -EINVAL;
  }

//  buffer_p = alloc_skb(RINGBUFFER_DATA_SIZE, GFP_KERNEL);
  buffer_p = genlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL);
  if (unlikely(!buffer_p)) {
    pr_err("%s: genlmsg_new() failed\n", __FUNCTION__);
    return -ENOMEM;
  }
  header_p =
      genlmsg_put(buffer_p, 0, ++i2c_mpu6050_netlink_sequence_number,
                  &i2c_mpu6050_netlink_family, 0, NETLINK_COMMAND_RECORD);
  if (unlikely(!header_p)) {
    pr_err("%s: genlmsg_put() failed\n", __FUNCTION__);
    err = -ENOSYS;
    goto error1;
  }

//  mutex_lock (&clientData_in->sync_lock);
  i2c_mpu6050_device_extract_data(clientData_in->ringbuffer[slot_in].data,
                                  &accel_x, &accel_y, &accel_z,
                                  &t,
                                  &gyro_x, &gyro_y, &gyro_z);
//  mutex_unlock (&clientData_in->sync_lock);

  err = nla_put_u16(buffer_p, NETLINK_ATTRIBUTE_ACCEL_X,
                    accel_x);
  if (unlikely(err)) {
    pr_err("%s: nla_put_u16(%d) failed: %d\n", __FUNCTION__,
           NETLINK_ATTRIBUTE_ACCEL_X, err);
    goto error2;
  }
  err = nla_put_u16(buffer_p, NETLINK_ATTRIBUTE_ACCEL_Y,
                    accel_y);
  if (unlikely(err)) {
    pr_err("%s: nla_put_u16(%d) failed: %d\n", __FUNCTION__,
           NETLINK_ATTRIBUTE_ACCEL_Y, err);
    goto error2;
  }
  err = nla_put_u16(buffer_p, NETLINK_ATTRIBUTE_ACCEL_Z,
                    accel_z);
  if (unlikely(err)) {
    pr_err("%s: nla_put_u16(%d) failed: %d\n", __FUNCTION__,
           NETLINK_ATTRIBUTE_ACCEL_Z, err);
    goto error2;
  }
  err = nla_put_u16(buffer_p, NETLINK_ATTRIBUTE_TEMP,
                    t);
  if (unlikely(err)) {
    pr_err("%s: nla_put_u16(%d) failed: %d\n", __FUNCTION__,
           NETLINK_ATTRIBUTE_TEMP, err);
    goto error2;
  }
  err = nla_put_u16(buffer_p, NETLINK_ATTRIBUTE_GYRO_X,
                    gyro_x);
  if (unlikely(err)) {
    pr_err("%s: nla_put_u16(%d) failed: %d\n", __FUNCTION__,
           NETLINK_ATTRIBUTE_GYRO_X, err);
    goto error2;
  }
  err = nla_put_u16(buffer_p, NETLINK_ATTRIBUTE_GYRO_Y,
                    gyro_y);
  if (unlikely(err)) {
    pr_err("%s: nla_put_u16(%d) failed: %d\n", __FUNCTION__,
           NETLINK_ATTRIBUTE_GYRO_Y, err);
    goto error2;
  }
  err = nla_put_u16(buffer_p, NETLINK_ATTRIBUTE_GYRO_Z,
                    gyro_z);
  if (unlikely(err)) {
    pr_err("%s: nla_put_u16(%d) failed: %d\n", __FUNCTION__,
           NETLINK_ATTRIBUTE_GYRO_Z, err);
    goto error2;
  }

  err = genlmsg_end(buffer_p, header_p);
  if (unlikely(err < 0)) {
    pr_err("%s: genlmsg_end() failed: %d\n", __FUNCTION__,
           err);
    goto error2;
  }

//  err = genlmsg_unicast(&init_net, buffer_p, 0);
//  err = genlmsg_multicast_netns(&init_net, buffer_p,
//                                0, i2c_mpu6050_netlink_group.id ,0);
//  if (unlikely(err)) {
//    pr_err("%s: genlmsg_multicast_netns() failed: %d\n", __FUNCTION__,
//           err);
//    goto error1;
//  }
  err = genlmsg_multicast_allns(buffer_p,
                                0, i2c_mpu6050_netlink_group.id ,0);
  if (unlikely(err)) {
    pr_err("%s: genlmsg_multicast_allns() failed: %d\n", __FUNCTION__,
           err);
    goto error1;
  }
  pr_debug("sent netlink message (%d bytes) to group %d...\n",
           genlmsg_total_size(NLMSG_GOODSIZE),
           i2c_mpu6050_netlink_group.id);

  return 0;

error2:
  genlmsg_cancel(buffer_p, header_p);
error1:
  nlmsg_free(buffer_p);

  return err;
}

int
i2c_mpu6050_netlink_init(struct i2c_mpu6050_client_data_t* clientData_in)
{
  int err, err_2;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely(!clientData_in)) {
    pr_err("%s: invalid argument\n", __FUNCTION__);
    return -EINVAL;
  }
  if (unlikely (clientData_in->netlink_server)) {
    pr_warn ("%s: netlink server already initialized\n", __FUNCTION__);
    return 0;
  }

  clientData_in->netlink_server = kzalloc (sizeof (struct i2c_mpu6050_netlink_server_t), GFP_KERNEL);
  if (unlikely (IS_ERR (clientData_in->netlink_server))) {
    err = PTR_ERR (clientData_in->netlink_server);
    pr_err ("%s: kzalloc() failed: %d\n", __FUNCTION__,
            err);
    return err;
   }
//  mutex_init(&clientData_in->netlink_server->cb_mutex);
  clientData_in->netlink_server->socket =
      netlink_kernel_create(&init_net,
                            KO_OLIMEX_MOD_MPU6050_NETLINK_PROTOCOL,
                            (1 << (KO_OLIMEX_MOD_MPU6050_NETLINK_GROUP - 1)),
                            i2c_mpu6050_netlink_input,
                            NULL,
                            THIS_MODULE);
//      netlink_kernel_create(&init_net,
//                            KO_OLIMEX_MOD_MPU6050_NETLINK_PROTOCOL,
//                            &i2c_mpu6050_netlink_socket_cfg);
      if (unlikely(IS_ERR(clientData_in->netlink_server->socket))) {
    err = PTR_ERR(clientData_in->netlink_server->socket);
    pr_err("%s: netlink_kernel_create() failed: %d\n", __FUNCTION__,
           err);
    goto error1;
  }

  strcpy (i2c_mpu6050_netlink_family.name,
          KO_OLIMEX_MOD_MPU6050_NETLINK_PROTOCOL_FAMILY_NAME);
//  err = genl_register_family(&i2c_mpu6050_netlink_family);
//  if (unlikely(err)) {
//    pr_err("%s: genl_register_family() failed: %d\n", __FUNCTION__,
//           err);
//    goto error2;
//  }
  err =
      genl_register_family_with_ops(&i2c_mpu6050_netlink_family,
                                    KO_OLIMEX_MOD_MPU6050_ARRAY_AND_SIZE(i2c_mpu6050_netlink_ops));
  if (unlikely(err)) {
    pr_err("%s: genl_register_family_with_ops() failed: %d\n", __FUNCTION__,
           err);
    goto error2;
  }
//  err = genl_register_family_with_ops_groups(&i2c_mpu6050_netlink_family,
//                                             i2c_mpu6050_netlink_operations_ops,
//                                             i2c_mpu6050_netlink_operations_groups);
//  if (unlikely(err)) {
//    pr_err("%s: genl_register_family_with_ops_groups() failed: %d\n", __FUNCTION__,
//           err);
//    goto error2;
//  }
  pr_info("%s: registered netlink family \"%s\" version: %d --> id: %d\n", __FUNCTION__,
          i2c_mpu6050_netlink_family.name, i2c_mpu6050_netlink_family.version,
          i2c_mpu6050_netlink_family.id);
//  err = genl_register_ops(&i2c_mpu6050_netlink_family,
//                          &i2c_mpu6050_netlink_operations_record);
//  if (unlikely(err)) {
//    pr_err("%s: genl_register_ops() failed: %d\n", __FUNCTION__,
//           err);
//    goto error3;
//  }
  strcpy (i2c_mpu6050_netlink_group.name,
          KO_OLIMEX_MOD_MPU6050_NETLINK_PROTOCOL_GROUP_NAME);
  err = genl_register_mc_group (&i2c_mpu6050_netlink_family,
                                &i2c_mpu6050_netlink_group);
  if (unlikely(err)) {
    pr_err("%s: genl_register_mc_group() failed: %d\n", __FUNCTION__,
           err);
    goto error4;
  }

  clientData_in->netlink_server->thread =
      kthread_run (i2c_mpu6050_netlink_run, clientData_in,
                   KO_OLIMEX_MOD_MPU6050_NETLINK_SERVER_THREAD_NAME);
  if (unlikely (IS_ERR (clientData_in->netlink_server->thread)))
  {
    err = PTR_ERR (clientData_in->netlink_server->thread);
    pr_err ("%s: kthread_run() failed: %d\n", __FUNCTION__,
            err);
    goto error5;
  }

  return 0;

error5:
  genl_unregister_mc_group (&i2c_mpu6050_netlink_family,
                            &i2c_mpu6050_netlink_group);
error4:
  err_2 = genl_unregister_ops(&i2c_mpu6050_netlink_family,
                              &i2c_mpu6050_netlink_operation_record);
  if (unlikely(err_2))
    pr_err("%s: genl_unregister_ops() failed: %d\n", __FUNCTION__,
           err_2);
//error3:
  err_2 = genl_unregister_family(&i2c_mpu6050_netlink_family);
  if (unlikely(err_2))
    pr_err("%s: genl_unregister_family() failed: %d\n", __FUNCTION__,
           err_2);
error2:
  netlink_kernel_release(clientData_in->netlink_server->socket);
error1:
//  mutex_destroy(&clientData_in->netlink_server->cb_mutex);
  kfree (clientData_in->netlink_server);
  clientData_in->netlink_server = NULL;

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
  if (unlikely (!clientData_in->netlink_server)) {
//    pr_debug ("%s: server not initialized\n", __FUNCTION__);
    return;
  }
  if (unlikely (!clientData_in->netlink_server->thread)) {
//    pr_debug ("%s: server not started\n", __FUNCTION__);
    goto error1;
  }

  //  lock_kernel ();
  //  err = kill_proc (clientData_in->server->thread->pid, SIGKILL, 1);
  err = send_sig (SIGKILL, clientData_in->netlink_server->thread, 1);
  //  unlock_kernel ();
  if (unlikely (err < 0)) {
    //    pr_err ("%s: kill_proc() failed: %d\n", __FUNCTION__,
    pr_err ("%s: send_sig() failed: %d\n", __FUNCTION__,
            err);
    goto error1;
  }
  while (clientData_in->netlink_server->running == 1)
    msleep (10);
  pr_info ("%s: netlink server thread terminated\n", __FUNCTION__);

  kfree (clientData_in->netlink_server->thread);
error1:
  genl_unregister_mc_group (&i2c_mpu6050_netlink_family,
                            &i2c_mpu6050_netlink_group);
  err = genl_unregister_ops(&i2c_mpu6050_netlink_family,
                            &i2c_mpu6050_netlink_operation_record);
  if (unlikely(err))
    pr_err("%s: genl_unregister_ops() failed: %d\n", __FUNCTION__,
           err);
  err = genl_unregister_family(&i2c_mpu6050_netlink_family);
  if (unlikely(err))
    pr_err("%s: genl_unregister_family() failed: %d\n", __FUNCTION__,
           err);
  netlink_kernel_release(clientData_in->netlink_server->socket);
//  mutex_destroy(&clientData_in->netlink_server->cb_mutex);
  kfree (clientData_in->netlink_server);
  clientData_in->netlink_server = NULL;
}
