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

#include "GL/gl.h"
#include "glm/glm.hpp"

#include "ace/Global_Macros.h"
#include "ace/OS.h"
#include "ace/Time_Value.h"

#include "gtk/gtk.h"
#include "gtk/gtkgl.h"

#include "common_inotify.h"

#include "common_ui_common.h"

#include "stream_common.h"

#include "net_common.h"
#include "net_iconnection.h"
#include "net_iconnectionmanager.h"

#include "olimex_mod_mpu6050_defines.h"

// forward declarations
class Olimex_Mod_MPU6050_Message;
class Olimex_Mod_MPU6050_SessionMessage;
class Stream_IAllocator;
struct Olimex_Mod_MPU6050_SessionData;

enum Olimex_Mod_MPU6050_MessageType
{
  OLIMEX_MOD_MPU6050_MESSAGE_INVALID = -1,
  OLIMEX_MOD_MPU6050_MESSAGE_SENSOR_DATA,
  ///////////////////////////////////////
  OLIMEX_MOD_MPU6050_MESSAGE_MAX
};

enum Olimex_Mod_MPU6050_Event
{
  OLIMEX_MOD_MPU6050_EVENT_INVALID = -1,
  OLIMEX_MOD_MPU6050_EVENT_CONNECT,
  OLIMEX_MOD_MPU6050_EVENT_DISCONNECT,
  OLIMEX_MOD_MPU6050_EVENT_MESSAGE,
  OLIMEX_MOD_MPU6050_EVENT_SESSION_MESSAGE,
  ///////////////////////////////////////
  OLIMEX_MOD_MPU6050_EVENT_MAX
};
typedef std::deque<Olimex_Mod_MPU6050_Event> Olimex_Mod_MPU6050_Events_t;
typedef Olimex_Mod_MPU6050_Events_t::const_iterator Olimex_Mod_MPU6050_EventsIterator_t;

typedef std::deque<Olimex_Mod_MPU6050_Message*> Olimex_Mod_MPU6050_Messages_t;
typedef Olimex_Mod_MPU6050_Messages_t::const_iterator Olimex_Mod_MPU6050_MessagesIterator_t;

typedef Common_INotify_T<Olimex_Mod_MPU6050_SessionData,
                         Olimex_Mod_MPU6050_Message,
                         Olimex_Mod_MPU6050_SessionMessage> Olimex_Mod_MPU6050_Notification_t;
typedef std::list<Olimex_Mod_MPU6050_Notification_t*> Olimex_Mod_MPU6050_Subscribers_t;
typedef Olimex_Mod_MPU6050_Subscribers_t::iterator Olimex_Mod_MPU6050_SubscribersIterator_t;

struct SensorBias
{
  gfloat ax_bias;
  gfloat ay_bias;
  gfloat az_bias;
  gfloat gx_bias;
  gfloat gy_bias;
  gfloat gz_bias;
};

struct Camera
{
  //glm::vec3 position;
  //glm::vec3 looking_at;
  //glm::vec3 up;

  float zoom;
  glm::vec3 rotation;
  glm::vec3 translation;
  int last[2];
};

struct Olimex_Mod_MPU6050_Configuration;
struct Olimex_Mod_MPU6050_UserData
 : public Net_UserData
{
  inline Olimex_Mod_MPU6050_UserData ()
   : Net_UserData ()
   , configuration (NULL)
  {};

  Olimex_Mod_MPU6050_Configuration* configuration;
};

typedef Stream_Statistic Olimex_Mod_MPU6050_RuntimeStatistic_t;
struct Olimex_Mod_MPU6050_Configuration;
struct Olimex_Mod_MPU6050_ConnectionState;
typedef Net_IConnection_T<ACE_INET_Addr,
                          Olimex_Mod_MPU6050_Configuration,
                          Olimex_Mod_MPU6050_ConnectionState,
                          Olimex_Mod_MPU6050_RuntimeStatistic_t> Olimex_Mod_MPU6050_IConnection_t;
typedef Net_IConnectionManager_T<ACE_INET_Addr,
                                 Olimex_Mod_MPU6050_Configuration,
                                 Olimex_Mod_MPU6050_ConnectionState,
                                 Olimex_Mod_MPU6050_RuntimeStatistic_t,
                                 //////
                                 Olimex_Mod_MPU6050_UserData> Olimex_Mod_MPU6050_InetConnectionManager_t;

struct Olimex_Mod_MPU6050_SocketHandlerConfiguration
 : public Net_SocketHandlerConfiguration
{
  inline Olimex_Mod_MPU6050_SocketHandlerConfiguration ()
   : Net_SocketHandlerConfiguration ()
   , userData (NULL)
  {};

  Olimex_Mod_MPU6050_UserData* userData;
};

struct Olimex_Mod_MPU6050_ModuleHandlerConfiguration
 : public Stream_ModuleHandlerConfiguration
{
  inline Olimex_Mod_MPU6050_ModuleHandlerConfiguration ()
   : Stream_ModuleHandlerConfiguration ()
   , connection (NULL)
   , connectionManager (NULL)
   , consoleMode (false)
   , inbound (true)
   , passive (false)
   , socketConfiguration (NULL)
   , socketHandlerConfiguration (NULL)
  {};

  Olimex_Mod_MPU6050_IConnection_t*              connection; // UDP source/IO module
  Olimex_Mod_MPU6050_InetConnectionManager_t*    connectionManager; // UDP IO module
  bool                                           consoleMode;
  bool                                           inbound;
  bool                                           passive;

  Net_SocketConfiguration*                       socketConfiguration;
  Olimex_Mod_MPU6050_SocketHandlerConfiguration* socketHandlerConfiguration;
};

struct Olimex_Mod_MPU6050_StreamConfiguration
 : public Stream_Configuration
{
  inline Olimex_Mod_MPU6050_StreamConfiguration ()
   : Stream_Configuration ()
   , moduleHandlerConfiguration (NULL)
  {};

  Olimex_Mod_MPU6050_ModuleHandlerConfiguration* moduleHandlerConfiguration;
};

struct Olimex_Mod_MPU6050_Configuration
{
  inline Olimex_Mod_MPU6050_Configuration ()
   : socketConfiguration ()
   , socketHandlerConfiguration ()
   , streamConfiguration ()
   , userData (NULL)
  {};

  Net_SocketConfiguration                       socketConfiguration;
  Olimex_Mod_MPU6050_SocketHandlerConfiguration socketHandlerConfiguration;

  Stream_ModuleConfiguration                    moduleConfiguration;
  Olimex_Mod_MPU6050_ModuleHandlerConfiguration moduleHandlerConfiguration;
  Olimex_Mod_MPU6050_StreamConfiguration        streamConfiguration;

  Olimex_Mod_MPU6050_UserData*                  userData;
};

struct Olimex_Mod_MPU6050_GtkCBData
 : public Common_UI_GTKState
{
 inline Olimex_Mod_MPU6050_GtkCBData ()
  : Common_UI_GTKState ()
  , argc (0)
  , argv (NULL)
  , clientMode (false)
  , contextIdData (0)
  , contextIdInformation (0)
  , eventQueue ()
  , frameCounter (0)
  , messageQueue ()
  , openGLAxesListId (0)
  , openGLCamera ()
  , openGLContext (NULL)
  , openGLDrawable (NULL)
  , openGLRefreshId (0)
  , openGLDoubleBuffered (OLIMEX_MOD_MPU6050_OPENGL_DOUBLE_BUFFERED)
  , temperature ()
  , temperatureIndex (-1)
  , timestamp (ACE_Time_Value::zero)
 {
   resetCamera ();
 };

 inline void resetCamera ()
 {
   ACE_OS::memset (&openGLCamera, 0, sizeof (openGLCamera));
   openGLCamera.zoom = OLIMEX_MOD_MPU6050_OPENGL_CAMERA_DEFAULT_ZOOM;
 };

 int                                argc;
 ACE_TCHAR**                        argv;
 bool                               clientMode;
 // *NOTE*: on the host ("server"), use the device bias registers instead !
 // *TODO*: implement a client->server protocol to do this
 SensorBias                         clientSensorBias; // client side ONLY (!)
 guint                              contextIdData; // status bar context
 guint                              contextIdInformation; // status bar context
 Olimex_Mod_MPU6050_Events_t        eventQueue;
 unsigned int                       frameCounter;
 Olimex_Mod_MPU6050_Messages_t      messageQueue;
 GLuint                             openGLAxesListId;
 Camera                             openGLCamera;
 GdkGLContext*                      openGLContext;
 GdkGLDrawable*                     openGLDrawable;
 guint                              openGLRefreshId;
 bool                               openGLDoubleBuffered;
 gfloat                             temperature[OLIMEX_MOD_MPU6050_TEMPERATURE_BUFFER_SIZE * 2];
 int                                temperatureIndex;
 ACE_Time_Value                     timestamp;
};

#endif // #ifndef OLIMEX_MOD_MPU6050_TYPES_H
