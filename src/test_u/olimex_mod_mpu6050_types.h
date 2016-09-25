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
#include "ace/Synch_Traits.h"
#include "ace/Time_Value.h"

#include "gtk/gtk.h"
#include "gtk/gtkgl.h"

#include "glade/glade.h"

#include "common_time_common.h"

#include "common_ui_common.h"

#include "stream_base.h"
#include "stream_common.h"
#include "stream_control_message.h"
#include "stream_isessionnotify.h"
#include "stream_statemachine_control.h"

#include "net_common.h"
#include "net_configuration.h"
#include "net_iconnection.h"
#include "net_iconnectionmanager.h"
#include "net_iconnector.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#include "net_netlinksockethandler.h"
#endif

#include "olimex_mod_mpu6050_defines.h"
#include "olimex_mod_mpu6050_stream_common.h"

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

typedef Stream_ControlMessage_T<Stream_ControlMessageType,
                                Stream_AllocatorConfiguration,
                                Olimex_Mod_MPU6050_Message,
                                Olimex_Mod_MPU6050_SessionMessage> Olimex_Mod_MPU6050_ControlMessage_t;

typedef Stream_MessageAllocatorHeapBase_T<Stream_AllocatorConfiguration,
                                          Olimex_Mod_MPU6050_ControlMessage_t,
                                          Olimex_Mod_MPU6050_Message,
                                          Olimex_Mod_MPU6050_SessionMessage> Olimex_Mod_MPU6050_MessageAllocator_t;

typedef Stream_INotify_T<Stream_SessionMessageType> Olimex_Mod_MPU6050_IStreamNotify_t;
typedef Stream_ISessionDataNotify_T<Stream_SessionId_t,
                                    Olimex_Mod_MPU6050_SessionData,
                                    Stream_SessionMessageType,
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
 : Net_UserData
{
  inline Olimex_Mod_MPU6050_UserData ()
   : Net_UserData ()
   , configuration (NULL)
  {};

  Olimex_Mod_MPU6050_Configuration* configuration;
};

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
struct Olimex_Mod_MPU6050_NetlinkConfiguration;
struct Olimex_Mod_MPU6050_NetlinkUserData
 : Olimex_Mod_MPU6050_UserData
{
  inline Olimex_Mod_MPU6050_NetlinkUserData ()
   : Olimex_Mod_MPU6050_UserData ()
   , configuration (NULL)
  {};

  Olimex_Mod_MPU6050_NetlinkConfiguration* configuration;
};
#endif

struct Olimex_Mod_MPU6050_Configuration;
typedef Stream_Statistic Olimex_Mod_MPU6050_RuntimeStatistic_t;
struct Olimex_Mod_MPU6050_ConnectionState
{
  inline Olimex_Mod_MPU6050_ConnectionState ()
   : configuration (NULL)
   , status (NET_CONNECTION_STATUS_INVALID)
   , currentStatistic ()
   , userData (NULL)
  {};

  // *TODO*: consider making this a separate entity (i.e. a pointer)
  Olimex_Mod_MPU6050_Configuration*     configuration;

  Net_Connection_Status                 status;

  Olimex_Mod_MPU6050_RuntimeStatistic_t currentStatistic;

  Olimex_Mod_MPU6050_UserData*          userData;
};

struct Olimex_Mod_MPU6050_Configuration;
struct Olimex_Mod_MPU6050_ConnectionState;
typedef Net_IConnection_T<ACE_INET_Addr,
                          Olimex_Mod_MPU6050_Configuration,
                          Olimex_Mod_MPU6050_ConnectionState,
                          Olimex_Mod_MPU6050_RuntimeStatistic_t> Olimex_Mod_MPU6050_IConnection_t;
struct Olimex_Mod_MPU6050_NetlinkConfiguration;
struct Olimex_Mod_MPU6050_NetlinkConnectionState;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
typedef Net_IConnection_T<Net_Netlink_Addr,
                          Olimex_Mod_MPU6050_NetlinkConfiguration,
                          Olimex_Mod_MPU6050_NetlinkConnectionState,
                          Olimex_Mod_MPU6050_RuntimeStatistic_t> Olimex_Mod_MPU6050_INetlinkConnection_t;
#endif
typedef Net_IConnectionManager_T<ACE_INET_Addr,
                                 Olimex_Mod_MPU6050_Configuration,
                                 Olimex_Mod_MPU6050_ConnectionState,
                                 Olimex_Mod_MPU6050_RuntimeStatistic_t,
                                 //////
                                 Olimex_Mod_MPU6050_UserData> Olimex_Mod_MPU6050_IConnectionManager_t;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
typedef Net_IConnectionManager_T<Net_Netlink_Addr,
                                 Olimex_Mod_MPU6050_NetlinkConfiguration,
                                 Olimex_Mod_MPU6050_NetlinkConnectionState,
                                 Olimex_Mod_MPU6050_RuntimeStatistic_t,
                                 //////
                                 Olimex_Mod_MPU6050_NetlinkUserData> Olimex_Mod_MPU6050_INetlinkConnectionManager_t;
#endif

struct Olimex_Mod_MPU6050_SocketHandlerConfiguration
 : Net_SocketHandlerConfiguration
{
  inline Olimex_Mod_MPU6050_SocketHandlerConfiguration ()
   : Net_SocketHandlerConfiguration ()
   , userData (NULL)
  {};

  Olimex_Mod_MPU6050_UserData* userData;
};

typedef Net_IConnector_T<ACE_INET_Addr,
                         Olimex_Mod_MPU6050_SocketHandlerConfiguration> Olimex_Mod_MPU6050_IConnector_t;
struct Olimex_Mod_MPU6050_SignalHandlerConfiguration
{
 inline Olimex_Mod_MPU6050_SignalHandlerConfiguration ()
  : actionTimerID (-1)
  , consoleMode (false)
  , interfaceHandle (NULL)
  , peerAddress ()
  , useReactor (false)
  {};

  long                             actionTimerID;
  bool                             consoleMode;
  Olimex_Mod_MPU6050_IConnector_t* interfaceHandle;
  ACE_INET_Addr                    peerAddress;
  bool                             useReactor;
};

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
struct Olimex_Mod_MPU6050_NetlinkSocketHandlerConfiguration
 : Olimex_Mod_MPU6050_SocketHandlerConfiguration
{
  inline Olimex_Mod_MPU6050_NetlinkSocketHandlerConfiguration ()
   : Olimex_Mod_MPU6050_SocketHandlerConfiguration ()
   , userData (NULL)
  {};

  Olimex_Mod_MPU6050_NetlinkUserData* userData;
};
#endif

struct Olimex_Mod_MPU6050_StreamState;
struct Olimex_Mod_MPU6050_StreamConfiguration;
struct Olimex_Mod_MPU6050_ModuleHandlerConfiguration;
struct Olimex_Mod_MPU6050_SessionData;
typedef Stream_SessionData_T<Olimex_Mod_MPU6050_SessionData> Olimex_Mod_MPU6050_StreamSessionData_t;
typedef Stream_Base_T<ACE_MT_SYNCH,
                      ACE_MT_SYNCH,
                      Common_TimePolicy_t,
                      Stream_ControlType,
                      Stream_SessionMessageType,
                      Stream_StateMachine_ControlState,
                      Olimex_Mod_MPU6050_StreamState,
                      Olimex_Mod_MPU6050_StreamConfiguration,
                      Olimex_Mod_MPU6050_RuntimeStatistic_t,
                      Stream_ModuleConfiguration,
                      Olimex_Mod_MPU6050_ModuleHandlerConfiguration,
                      Olimex_Mod_MPU6050_SessionData,
                      Olimex_Mod_MPU6050_StreamSessionData_t,
                      Olimex_Mod_MPU6050_ControlMessage_t,
                      Olimex_Mod_MPU6050_Message,
                      Olimex_Mod_MPU6050_SessionMessage> Olimex_Mod_MPU6050_StreamBase_t;
struct Olimex_Mod_MPU6050_ModuleHandlerConfiguration
 : Stream_ModuleHandlerConfiguration
{
  inline Olimex_Mod_MPU6050_ModuleHandlerConfiguration ()
   : Stream_ModuleHandlerConfiguration ()
   , connection (NULL)
   , connectionManager (NULL)
   , consoleMode (false)
   , inbound (true)
   , passive (false)
   , printFinalReport (true)
   , pushStatisticMessages (true)
   , socketConfiguration (NULL)
   , socketHandlerConfiguration (NULL)
   , stream (NULL)
  {};

  Olimex_Mod_MPU6050_IConnection_t*               connection; // UDP source/IO module
  Olimex_Mod_MPU6050_IConnectionManager_t*        connectionManager; // UDP IO module

  bool                                            consoleMode;
  bool                                            inbound;
  bool                                            passive;
  bool                                            printFinalReport;
  bool                                            pushStatisticMessages;

  Net_SocketConfiguration*                        socketConfiguration;
  Olimex_Mod_MPU6050_SocketHandlerConfiguration*  socketHandlerConfiguration;
  Olimex_Mod_MPU6050_StreamBase_t*                stream;
};

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
struct Olimex_Mod_MPU6050_NetlinkModuleHandlerConfiguration
 : Olimex_Mod_MPU6050_ModuleHandlerConfiguration
{
  inline Olimex_Mod_MPU6050_NetlinkModuleHandlerConfiguration ()
   : Olimex_Mod_MPU6050_ModuleHandlerConfiguration ()
   , connection (NULL)
   , connectionManager (NULL)
   , socketHandlerConfiguration (NULL)
  {};

  Olimex_Mod_MPU6050_INetlinkConnection_t*              connection; // Netlink source/IO module
  Olimex_Mod_MPU6050_INetlinkConnectionManager_t*       connectionManager; // Netlink IO module

  Olimex_Mod_MPU6050_NetlinkSocketHandlerConfiguration* socketHandlerConfiguration;
};
#endif

struct Olimex_Mod_MPU6050_StreamConfiguration
 : Stream_Configuration
{
  inline Olimex_Mod_MPU6050_StreamConfiguration ()
   : Stream_Configuration ()
   , moduleHandlerConfiguration (NULL)
  {};

  Olimex_Mod_MPU6050_ModuleHandlerConfiguration* moduleHandlerConfiguration;
};

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
struct Olimex_Mod_MPU6050_NetlinkStreamConfiguration
 : Olimex_Mod_MPU6050_StreamConfiguration
{
  inline Olimex_Mod_MPU6050_NetlinkStreamConfiguration ()
   : Olimex_Mod_MPU6050_StreamConfiguration ()
   , moduleHandlerConfiguration (NULL)
  {};

  Olimex_Mod_MPU6050_NetlinkModuleHandlerConfiguration* moduleHandlerConfiguration;
};
#endif

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

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
struct Olimex_Mod_MPU6050_NetlinkConfiguration
 : Olimex_Mod_MPU6050_Configuration
{
  inline Olimex_Mod_MPU6050_NetlinkConfiguration ()
   : Olimex_Mod_MPU6050_Configuration ()
   , socketHandlerConfiguration ()
   , moduleHandlerConfiguration ()
   , streamConfiguration ()
   , userData (NULL)
  {};

  Olimex_Mod_MPU6050_NetlinkSocketHandlerConfiguration socketHandlerConfiguration;

  Olimex_Mod_MPU6050_NetlinkModuleHandlerConfiguration moduleHandlerConfiguration;
  Olimex_Mod_MPU6050_NetlinkStreamConfiguration        streamConfiguration;

  Olimex_Mod_MPU6050_NetlinkUserData*                  userData;
};
#endif

struct Olimex_Mod_MPU6050_GtkCBData
 : Common_UI_GTKState
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
  , XML (NULL)
 {
   resetCamera ();
 };

 inline void resetCamera ()
 {
   ACE_OS::memset (&openGLCamera, 0, sizeof (openGLCamera));
   openGLCamera.zoom = OLIMEX_MOD_MPU6050_OPENGL_CAMERA_DEFAULT_ZOOM;
 };

 int                           argc;
 ACE_TCHAR**                   argv;
 bool                          clientMode;
 // *NOTE*: on the host ("server"), use the device bias registers instead !
 // *TODO*: implement a client->server protocol to do this
 SensorBias                    clientSensorBias; // client side ONLY (!)
 guint                         contextIdData; // status bar context
 guint                         contextIdInformation; // status bar context
 Olimex_Mod_MPU6050_Events_t   eventQueue;
 unsigned int                  frameCounter;
 Olimex_Mod_MPU6050_Messages_t messageQueue;
 GLuint                        openGLAxesListId;
 Camera                        openGLCamera;
 GdkGLContext*                 openGLContext;
 GdkGLDrawable*                openGLDrawable;
 guint                         openGLRefreshId;
 bool                          openGLDoubleBuffered;
 gfloat                        temperature[OLIMEX_MOD_MPU6050_TEMPERATURE_BUFFER_SIZE * 2];
 int                           temperatureIndex;
 ACE_Time_Value                timestamp;

 GladeXML*                     XML;
};

#endif // #ifndef OLIMEX_MOD_MPU6050_TYPES_H
