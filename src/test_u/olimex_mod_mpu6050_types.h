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

#ifndef OLIMEX_MOD_MPU6050_TYPES_H
#define OLIMEX_MOD_MPU6050_TYPES_H

#include <deque>

#include "ace/Synch.h"

enum Olimex_Mod_MPU6050_Event_t
{
  OLIMEX_MOD_MPU6050_EVENT_INVALID = 1,
  OLIMEX_MOD_MPU6050_EVENT_CONNECT,
  OLIMEX_MOD_MPU6050_EVENT_DISCONNECT,
  OLIMEX_MOD_MPU6050_EVENT_MESSAGE,
  ///////////////////////////////////////
  OLIMEX_MOD_MPU6050_EVENT_MAX
};
typedef std::deque<Olimex_Mod_MPU6050_Event_t> Olimex_Mod_MPU6050_Events_t;
typedef Olimex_Mod_MPU6050_Events_t::const_iterator Olimex_Mod_MPU6050_EventsIterator_t;

typedef std::deque<Olimex_Mod_MPU6050_Message*> Olimex_Mod_MPU6050_Messages_t;
typedef Olimex_Mod_MPU6050_Messages_t::const_iterator Olimex_Mod_MPU6050_MessagesIterator_t;

struct Olimex_Mod_MPU6050_GtkCBData_t
{
 inline Olimex_Mod_MPU6050_GtkCBData_t ()
  : lock (NULL, NULL),
//    event_queue (),
//    message_queue (),
//    subscribers (),
//    event_source_ids (),
//    timeout_handler (NULL),
    timer_id (-1)
 { };

 mutable ACE_Recursive_Thread_Mutex lock;
 Olimex_Mod_MPU6050_Events_t        event_queue;
 Olimex_Mod_MPU6050_Messages_t      message_queue;
 // RPG_Net_NotifySubscribers_t        subscribers;
// Net_GTK_EventSourceIDs_t           event_source_ids;
// Net_Client_TimeoutHandler*         timeout_handler; // *NOTE*: client only !
 long                               timer_id;        // *NOTE*: client only !
};

#endif // #ifndef OLIMEX_MOD_MPU6050_TYPES_H
