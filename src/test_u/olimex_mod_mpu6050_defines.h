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

#ifndef OLIMEX_MOD_MPU6050_DEFINES_H
#define OLIMEX_MOD_MPU6050_DEFINES_H

#include "ace/Default_Constants.h"

#define DEFAULT_USE_REACTOR                         true
#define DEFAULT_LOG_FILE                            "olimex_mod_mpu6050.log"

// *** network-related ***
// *PORTABILITY*: interface names are not portable, so we let the
// user choose the interface from a list on Windows (see select_Interface())...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#define DEFAULT_NETWORK_INTERFACE                   ""
#else
#define DEFAULT_NETWORK_INTERFACE                   "eth0"
#endif

#define DEFAULT_PORT                                32767

// default event dispatcher (default: use asynch I/O (proactor))
#define DEFAULT_CONNECTION_HANDLER_THREAD_NAME      "connection dispatch"
#define DEFAULT_CONNECTION_HANDLER_THREAD_GROUP_ID  1

#define DEFAULT_SOCKET_RECEIVE_BUFFER_SIZE          ACE_DEFAULT_MAX_SOCKET_BUFSIZ
#define DEFAULT_TCP_NODELAY                         true
#define DEFAULT_TCP_KEEPALIVE                       false
#define DEFAULT_TCP_LINGER                          10 // seconds {0 --> off}

#define DEFAULT_STREAM_BUFFER_SIZE                  1024 // 1 kB

// *** pro/reactor-related ***
#define DEFAULT_TASK_GROUP_ID                       11
// *** stream-related ***
// *IMPORTANT NOTE*: any of these COULD seriously affect performance
#define DEFAULT_MAXIMUM_QUEUE_SLOTS                 1000
#define DEFAULT_MAXIMUM_NUMBER_OF_INFLIGHT_MESSAGES 100

#define DEFAULT_STATISTICS_COLLECTION_INTERVAL      60 // seconds [0 --> OFF]
#define DEFAULT_STATISTICS_REPORTING_INTERVAL       0  // seconds [0 --> OFF]

#endif // #ifndef OLIMEX_MOD_MPU6050_DEFINES_H
