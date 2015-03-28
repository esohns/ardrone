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

#include "olimex_mod_mpu6050_server.h"

#include <linux/delay.h>
#include <linux/inet.h>
#include <linux/kthread.h>
#include <linux/module.h>
#include <linux/net.h>
#include <linux/printk.h>
#include <linux/sched.h>
#include <linux/signal.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#include "olimex_mod_mpu6050_defines.h"
#include "olimex_mod_mpu6050_main.h"
#include "olimex_mod_mpu6050_types.h"

int
i2c_mpu6050_server_run (void* data_in)
{
  struct i2c_mpu6050_client_data_t* client_data_p;
  int bytes_sent, err, i;

  pr_debug ("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely (!data_in)) {
    pr_err ("%s: invalid argument\n", __FUNCTION__);
    return -EINVAL;
  }
  client_data_p = (struct i2c_mpu6050_client_data_t*)data_in;
  if (unlikely (client_data_p->server->running)) {
    pr_warn ("%s: server thread already running\n", __FUNCTION__);
    return -ENODEV;
  }

//  lock_kernel ();
  client_data_p->server->running = 1;
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

  // create a socket
//  err = sock_create (AF_INET, SOCK_DGRAM, IPPROTO_UDP,
//                     &client_data_p->server->socket);
//  if (unlikely (err < 0)) {
//    pr_err ("%s: sock_create() failed: %d\n", __FUNCTION__,
//            err);
//    return -EIO;
//  }
  err = sock_create (AF_INET, SOCK_DGRAM, IPPROTO_UDP,
                     &client_data_p->server->socket_send);
  if (unlikely (err < 0)) {
    pr_err ("%s: sock_create() failed: %d\n", __FUNCTION__,
            err);
    goto error1;
  }

//  memset (&client_data_p->server->address, 0, sizeof (struct sockaddr));
  memset (&client_data_p->server->address_send, 0, sizeof (struct sockaddr));
//  client_data_p->server->address.sin_family      = AF_INET;
  client_data_p->server->address_send.sin_family = AF_INET;

//  client_data_p->server->address.sin_addr.s_addr      = htonl (INADDR_ANY);
  client_data_p->server->address_send.sin_addr.s_addr = in_aton (peer);

//  client_data_p->server->address.sin_port      = htons (SERVER_DEFAULT_PORT);
  client_data_p->server->address_send.sin_port = htons (port);

//  err = client_data_p->server->socket->ops->bind (client_data_p->server->socket,
//                                                  (struct sockaddr*)&client_data_p->server->address,
//                                                  sizeof (struct sockaddr));
//  if (unlikely (err < 0)) {
//    pr_err ("%s: bind() failed: %d\n", __FUNCTION__,
//            err);
//    goto error2;
//  }
//  pr_info ("%s: listening on port: %d\n", __FUNCTION__,
//           port);

  err =
      client_data_p->server->socket_send->ops->connect (client_data_p->server->socket_send,
                                                        (struct sockaddr*)&client_data_p->server->address_send,
                                                        sizeof (struct sockaddr),
                                                        0);
  if (unlikely (err < 0)) {
    pr_err ("%s: connect(%s:%d) failed: %d\n", __FUNCTION__,
            peer, port,
            err);
    goto error2;
  }
  pr_info ("%s: opened server socket: %s:%d\n", __FUNCTION__,
           peer, port);

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
      pr_debug ("%s: no data\n", __FUNCTION__);
      continue;
    }

//    i = client_data_p->server->ringbufferpos;
    i = client_data_p->ringbufferpos;
    //while (i != client_data_p->ringbufferpos)
    //{
      if (client_data_p->ringbuffer[i].used == 0) goto done;

      bytes_sent =
       i2c_mpu6050_server_send (client_data_p->server->socket_send,
                                &client_data_p->server->address_send,
                                client_data_p->ringbuffer[i].data, client_data_p->ringbuffer[i].size);
      if (bytes_sent >= 0) {
        if (bytes_sent != client_data_p->ringbuffer[i].size) {
          pr_err ("%s: send() failed: %d/%d bytes transmitted\n", __FUNCTION__,
                  bytes_sent, client_data_p->ringbuffer[i].size);
        }
//        else
//          pr_debug ("%s: #%d: sent %d bytes\n", __FUNCTION__,
//                    i, bytes_sent);
      }
//      else
//        pr_err ("%s: send() failed: %d\n", __FUNCTION__,
//                bytes_sent);

done:
    //  i++;
    //  if (i == RINGBUFFER_SIZE) i = 0;
    //}
    mutex_unlock (&client_data_p->sync_lock);
    //client_data_p->server->ringbufferpos = i;

    msleep (KO_OLIMEX_MOD_MPU6050_TIMER_DELAY_MS);
  }
  pr_debug ("%s: left server loop\n", __FUNCTION__);

error2:
//  sock_release (client_data_p->server->socket);
error1:
  sock_release (client_data_p->server->socket_send);

  client_data_p->server->running = 0;

  return 0;
}

int
i2c_mpu6050_server_send (struct socket* socket_in, struct sockaddr_in* address_in,
                         unsigned char* buffer_in, int length_in)
{
  struct msghdr message;
  struct iovec io_vector;
  mm_segment_t old_fs;
  int size;

  if (socket_in->sk == NULL)
    return 0;

  io_vector.iov_base = buffer_in;
  io_vector.iov_len = length_in;

  message.msg_flags = 0;
  message.msg_name = address_in;
  message.msg_namelen = sizeof (struct sockaddr_in);
  message.msg_control = NULL;
  message.msg_controllen = 0;
  message.msg_iov = &io_vector;
  message.msg_iovlen = 1;
  message.msg_control = NULL;

  old_fs = get_fs ();
  set_fs (KERNEL_DS);
  size = sock_sendmsg (socket_in, &message, length_in);
  set_fs (old_fs);

  return size;
}

int
i2c_mpu6050_server_receive (struct socket* socket_in, struct sockaddr_in* address_in,
                            unsigned char* buffer_in, int length_in)
{
  struct msghdr message;
  struct iovec io_vector;
  mm_segment_t old_fs;
  int size;

  if (socket_in->sk == NULL)
    return 0;

  io_vector.iov_base = buffer_in;
  io_vector.iov_len = length_in;

  message.msg_flags = 0;
  message.msg_name = address_in;
  message.msg_namelen = sizeof (struct sockaddr_in);
  message.msg_control = NULL;
  message.msg_controllen = 0;
  message.msg_iov = &io_vector;
  message.msg_iovlen = 1;
  message.msg_control = NULL;

  old_fs = get_fs ();
  set_fs (KERNEL_DS);
  size = sock_recvmsg (socket_in, &message, length_in, message.msg_flags);
  set_fs (old_fs);

  return size;
}

int
i2c_mpu6050_server_init (struct i2c_mpu6050_client_data_t* clientData_in)
{
  int err;

  pr_debug ("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely (!clientData_in)) {
    pr_err ("%s: invalid argument\n", __FUNCTION__);
    return -EINVAL;
  }
  if ((in_aton (peer) == 0) ||
      (port < 0)) {
    pr_err ("%s: invalid peer address/port: \"%s\":%d\n", __FUNCTION__,
            peer, port);
    return -EINVAL;
  }
  if (unlikely (clientData_in->server)) {
    pr_warn ("%s: server already initialized\n", __FUNCTION__);
    return 0;
  }

  clientData_in->server =
      kzalloc (sizeof (struct i2c_mpu6050_server_t), GFP_KERNEL);
  if (unlikely (IS_ERR (clientData_in->server))) {
    err = PTR_ERR (clientData_in->server);
    pr_err ("%s: kzalloc() failed: %d\n", __FUNCTION__,
            err);
    return err;
   }

  clientData_in->server->thread =
      kthread_run (i2c_mpu6050_server_run, clientData_in,
                   KO_OLIMEX_MOD_MPU6050_SERVER_THREAD_NAME);
  if (unlikely (IS_ERR (clientData_in->server->thread)))
  {
    err = PTR_ERR (clientData_in->server->thread);
    pr_err ("%s: kthread_run() failed: %d\n", __FUNCTION__,
            err);
    goto error1;
  }

  return 0;

error1:
  kfree (clientData_in->server);
  clientData_in->server = NULL;

  return err;
}

void
i2c_mpu6050_server_fini (struct i2c_mpu6050_client_data_t* clientData_in)
{
  int err;

  pr_debug("%s called.\n", __FUNCTION__);

  // sanity check(s)
  if (unlikely (!clientData_in)) {
    pr_err ("%s: invalid argument\n", __FUNCTION__);
    return;
  }
  if (unlikely (!clientData_in->server)) {
//    pr_debug ("%s: server not initialized\n", __FUNCTION__);
    return;
  }
  if (unlikely (!clientData_in->server->thread)) {
//    pr_debug ("%s: server not started\n", __FUNCTION__);
    goto error1;
  }

//  lock_kernel ();
//  err = kill_proc (clientData_in->server->thread->pid, SIGKILL, 1);
  err = send_sig (SIGKILL, clientData_in->server->thread, 1);
//  unlock_kernel ();
  if (unlikely (err < 0)) {
//    pr_err ("%s: kill_proc() failed: %d\n", __FUNCTION__,
    pr_err ("%s: send_sig() failed: %d\n", __FUNCTION__,
            err);
    goto error1;
  }
  while (clientData_in->server->running == 1)
    msleep (10);
  pr_info ("%s: server thread terminated\n", __FUNCTION__);

  kfree (clientData_in->server->thread);
error1:
  kfree (clientData_in->server);
  clientData_in->server = NULL;
}
