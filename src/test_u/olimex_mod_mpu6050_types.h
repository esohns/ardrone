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
#include <list>

#include "ace/Notification_Strategy.h"
#include "ace/Singleton.h"
#include "ace/Synch.h"
#include "ace/Time_Value.h"

#include "common.h"
#include "common_inotify.h"

#include "net_connection_manager.h"
#include "net_stream_common.h"

#include "olimex_mod_mpu6050_message.h"

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

typedef Common_INotify_T<Olimex_Mod_MPU6050_Message> Olimex_Mod_MPU6050_Notification_t;
typedef std::list<Olimex_Mod_MPU6050_Notification_t*> Olimex_Mod_MPU6050_Subscribers_t;
typedef Olimex_Mod_MPU6050_Subscribers_t::iterator Olimex_Mod_MPU6050_SubscribersIterator_t;

struct Olimex_Mod_MPU6050_GtkCBData_t
{
 inline Olimex_Mod_MPU6050_GtkCBData_t ()
  : lock (NULL, NULL)//,
//    event_queue (),
//    message_queue (),
//    subscribers (),
//    event_source_ids (),
//    timeout_handler (NULL),
//    timer_id (-1)
 { };

 mutable ACE_Recursive_Thread_Mutex lock;
 Olimex_Mod_MPU6050_Events_t        event_queue;
 Olimex_Mod_MPU6050_Messages_t      message_queue;
 Olimex_Mod_MPU6050_Subscribers_t   subscribers;
// Net_GTK_EventSourceIDs_t           event_source_ids;
// Net_Client_TimeoutHandler*         timeout_handler; // *NOTE*: client only !
// long                               timer_id;        // *NOTE*: client only !
};

struct Olimex_Mod_MPU6050_StreamProtocolConfigurationState_t
{
  // *********************** stream / socket data ******************************
  Net_StreamSocketConfiguration_t configuration;
  // **************************** runtime data *********************************
  unsigned int                    sessionID; // (== socket handle !)
  Net_RuntimeStatistic_t          currentStatistics;
  ACE_Time_Value                  lastCollectionTimestamp;
};

typedef Net_Connection_Manager_T<Olimex_Mod_MPU6050_StreamProtocolConfigurationState_t,
                                 Net_RuntimeStatistic_t> Olimex_Mod_MPU6050_ConnectionManager_t;
typedef ACE_Singleton<Olimex_Mod_MPU6050_ConnectionManager_t,
                      ACE_Recursive_Thread_Mutex> CONNECTIONMANAGER_SINGLETON;

#endif // #ifndef OLIMEX_MOD_MPU6050_TYPES_H
