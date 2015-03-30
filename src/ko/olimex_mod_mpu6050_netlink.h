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

#ifndef OLIMEX_MOD_MPU6050_NETLINK_H
#define OLIMEX_MOD_MPU6050_NETLINK_H

//#include <linux/mutex.h>
#include <net/genetlink.h>
#include <net/netlink.h>

// forward declarations
struct sock;
struct sk_buff;
struct i2c_mpu6050_client_data_t;

// types
enum i2c_mpu6050_netlink_attributes_t {
  NETLINK_ATTRIBUTE_INVALID,
  NETLINK_ATTRIBUTE_ACCEL_X,
  NETLINK_ATTRIBUTE_ACCEL_Y,
  NETLINK_ATTRIBUTE_ACCEL_Z,
  NETLINK_ATTRIBUTE_TEMP,
  NETLINK_ATTRIBUTE_GYRO_X,
  NETLINK_ATTRIBUTE_GYRO_Y,
  NETLINK_ATTRIBUTE_GYRO_Z,
  ///////////////////////////////////////
  NETLINK_ATTRIBUTE_MAX,
};

enum i2c_mpu6050_netlink_commands_t {
  NETLINK_COMMAND_INVALID,
  NETLINK_COMMAND_RECORD,
  ///////////////////////////////////////
  NETLINK_COMMAND_MAX,
};

// types
struct i2c_mpu6050_netlink_server_t
{
//  struct mutex cb_mutex;
  int running;
  struct sock* socket;
  struct task_struct* thread;
};

// globals
extern struct nla_policy i2c_mpu6050_netlink_policy[];
extern struct genl_family i2c_mpu6050_netlink_family;
extern struct genl_ops i2c_mpu6050_netlink_ops[];
extern struct genl_ops i2c_mpu6050_netlink_operation_record;
//extern struct genl_multicast_group i2c_mpu6050_netlink_groups[];
extern struct genl_multicast_group i2c_mpu6050_netlink_group;
//extern struct netlink_kernel_cfg i2c_mpu6050_netlink_socket_cfg;
extern int i2c_mpu6050_netlink_sequence_number;

// function declarations
int i2c_mpu6050_netlink_handler(struct sk_buff*, struct genl_info*);
void i2c_mpu6050_netlink_input(struct sk_buff*);
// *WARNING*: must be called with i2c_mpu6050_client_data_t.sync_lock held !
int i2c_mpu6050_netlink_forward(struct i2c_mpu6050_client_data_t*, // state
                                int);                              // slot

int i2c_mpu6050_netlink_init(struct i2c_mpu6050_client_data_t*);
void i2c_mpu6050_netlink_fini(struct i2c_mpu6050_client_data_t*);

#endif // #ifndef OLIMEX_MOD_MPU6050_NETLINK_H
