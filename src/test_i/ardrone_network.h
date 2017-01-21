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

#ifndef ARDRONE_NETWORK_H
#define ARDRONE_NETWORK_H

#include <ace/Global_Macros.h>
#include <ace/INET_Addr.h>
#include <ace/Singleton.h>
#include <ace/SOCK_Connector.h>
#include <ace/Synch_Traits.h>

#include "common_time_common.h"

#include "stream_common.h"
#include "stream_control_message.h"
#include "stream_session_data.h"

#include "stream_module_io_stream.h"

#include "net_asynch_tcpsockethandler.h"
#include "net_asynch_udpsockethandler.h"
#include "net_common.h"
#include "net_connection_manager.h"
#include "net_iconnector.h"
#include "net_itransportlayer.h"
#include "net_stream_asynch_tcpsocket_base.h"
#include "net_stream_asynch_udpsocket_base.h"
#include "net_stream_tcpsocket_base.h"
#include "net_stream_udpsocket_base.h"
#include "net_tcpconnection_base.h"
#include "net_tcpsockethandler.h"
#include "net_udpconnection_base.h"
#include "net_udpsockethandler.h"

#include "net_client_asynchconnector.h"
#include "net_client_connector.h"
#include "net_configuration.h"
#include "net_iconnection.h"
#include "net_iconnectionmanager.h"

//#include "ardrone_stream_common.h"
#include "ardrone_types.h"

// forward declarations
class ARDrone_Message;
class ARDrone_SessionMessage;

struct ARDrone_StreamConfiguration;
struct ARDrone_SocketHandlerConfiguration;
struct ARDrone_ConnectionConfiguration
 : Net_ConnectionConfiguration
{
  inline ARDrone_ConnectionConfiguration ()
   : Net_ConnectionConfiguration ()
   , streamConfiguration (NULL)
   , socketHandlerConfiguration (NULL)
   , userData (NULL)
  {};

  struct ARDrone_StreamConfiguration*        streamConfiguration;
  struct ARDrone_SocketHandlerConfiguration* socketHandlerConfiguration;

  struct ARDrone_UserData*                   userData;
};

struct ARDrone_UserData;
struct ARDrone_SocketHandlerConfiguration
 : Net_SocketHandlerConfiguration
{
  inline ARDrone_SocketHandlerConfiguration ()
   : Net_SocketHandlerConfiguration ()
   , userData (NULL)
  {};

  struct ARDrone_UserData* userData;
};

struct ARDrone_Configuration;
typedef Stream_Statistic ARDrone_RuntimeStatistic_t;
struct ARDrone_ConnectionState
 : Net_ConnectionState
{
  inline ARDrone_ConnectionState ()
   : Net_ConnectionState ()
   , configuration (NULL)
   //, currentStatistic ()
   , userData (NULL)
  {};

  // *TODO*: consider making this a separate entity (i.e. a pointer)
  struct ARDrone_Configuration* configuration;

  //ARDrone_RuntimeStatistic_t    currentStatistic;

  struct ARDrone_UserData*      userData;
};

typedef Net_IConnection_T<ACE_INET_Addr,
                          struct ARDrone_ConnectionConfiguration,
                          struct ARDrone_ConnectionState,
                          ARDrone_RuntimeStatistic_t> ARDrone_IConnection_t;
typedef Net_IConnectionManager_T<ACE_INET_Addr,
                                 struct ARDrone_ConnectionConfiguration,
                                 struct ARDrone_ConnectionState,
                                 ARDrone_RuntimeStatistic_t,
                                 struct ARDrone_UserData> ARDrone_IConnectionManager_t;

typedef Net_Connection_Manager_T<ACE_INET_Addr,
                                 struct ARDrone_ConnectionConfiguration,
                                 struct ARDrone_ConnectionState,
                                 ARDrone_RuntimeStatistic_t,
                                 struct ARDrone_UserData> ARDrone_ConnectionManager_t;

typedef Stream_SessionData_T<struct ARDrone_SessionData> ARDrone_StreamSessionData_t;
typedef Stream_ControlMessage_T<enum Stream_ControlType,
                                enum Stream_ControlMessageType,
                                struct Stream_AllocatorConfiguration> ARDrone_ControlMessage_t;
typedef Stream_Module_Net_IO_Stream_T<ACE_MT_SYNCH,
                                      Common_TimePolicy_t,
                                      enum Stream_ControlType,
                                      enum Stream_SessionMessageType,
                                      enum Stream_StateMachine_ControlState,
                                      struct ARDrone_StreamState,
                                      struct ARDrone_StreamConfiguration,
                                      ARDrone_RuntimeStatistic_t,
                                      struct Stream_ModuleConfiguration,
                                      struct ARDrone_ModuleHandlerConfiguration,
                                      struct ARDrone_SessionData,
                                      ARDrone_StreamSessionData_t,
                                      ARDrone_ControlMessage_t,
                                      ARDrone_Message,
                                      ARDrone_SessionMessage,
                                      ACE_INET_Addr,
                                      ARDrone_ConnectionManager_t,
                                      struct ARDrone_UserData> ARDrone_NetStream_t;

typedef Net_StreamTCPSocketBase_T<Net_TCPSocketHandler_T<struct ARDrone_SocketHandlerConfiguration,
                                                         ACE_SOCK_STREAM>,
                                  ACE_INET_Addr,
                                  struct ARDrone_ConnectionConfiguration,
                                  struct ARDrone_ConnectionState,
                                  ARDrone_RuntimeStatistic_t,
                                  ARDrone_NetStream_t,
                                  struct ARDrone_UserData,
                                  struct Stream_ModuleConfiguration,
                                  struct ARDrone_ModuleHandlerConfiguration> ARDrone_TCPHandler_t;
typedef Net_StreamAsynchTCPSocketBase_T<Net_AsynchTCPSocketHandler_T<struct ARDrone_SocketHandlerConfiguration>,
                                        ACE_INET_Addr,
                                        struct ARDrone_ConnectionConfiguration,
                                        struct ARDrone_ConnectionState,
                                        ARDrone_RuntimeStatistic_t,
                                        ARDrone_NetStream_t,
                                        struct ARDrone_UserData,
                                        struct Stream_ModuleConfiguration,
                                        struct ARDrone_ModuleHandlerConfiguration> ARDrone_AsynchTCPHandler_t;

typedef Net_StreamUDPSocketBase_T<Net_UDPSocketHandler_T<Net_SOCK_Dgram,
                                                         struct ARDrone_SocketHandlerConfiguration>,
                                  ACE_INET_Addr,
                                  struct ARDrone_ConnectionConfiguration,
                                  struct ARDrone_ConnectionState,
                                  ARDrone_RuntimeStatistic_t,
                                  struct ARDrone_SocketHandlerConfiguration,
                                  ARDrone_NetStream_t,
                                  struct ARDrone_UserData,
                                  struct Stream_ModuleConfiguration,
                                  struct ARDrone_ModuleHandlerConfiguration> ARDrone_UDPHandler_t;
typedef Net_StreamAsynchUDPSocketBase_T<Net_AsynchUDPSocketHandler_T<struct ARDrone_SocketHandlerConfiguration>,
                                        Net_SOCK_Dgram,
                                        ACE_INET_Addr,
                                        struct ARDrone_ConnectionConfiguration,
                                        struct ARDrone_ConnectionState,
                                        ARDrone_RuntimeStatistic_t,
                                        struct ARDrone_SocketHandlerConfiguration,
                                        ARDrone_NetStream_t,
                                        struct ARDrone_UserData,
                                        struct Stream_ModuleConfiguration,
                                        struct ARDrone_ModuleHandlerConfiguration> ARDrone_AsynchUDPHandler_t;

typedef Net_IConnector_T<ACE_INET_Addr,
                         struct ARDrone_SocketHandlerConfiguration> ARDrone_IConnector_t;

typedef Net_Client_Connector_T<Net_TCPConnectionBase_T<ARDrone_TCPHandler_t,
                                                       struct ARDrone_ConnectionConfiguration,
                                                       struct ARDrone_ConnectionState,
                                                       ARDrone_RuntimeStatistic_t,
                                                       struct ARDrone_SocketHandlerConfiguration,
                                                       ARDrone_NetStream_t,
                                                       struct ARDrone_UserData>,
                               ACE_SOCK_CONNECTOR,
                               ACE_INET_Addr,
                               struct ARDrone_ConnectionConfiguration,
                               struct ARDrone_ConnectionState,
                               ARDrone_RuntimeStatistic_t,
                               struct ARDrone_SocketHandlerConfiguration,
                               ARDrone_NetStream_t,
                               struct ARDrone_UserData> ARDrone_TCPConnector_t;
typedef Net_Client_Connector_T<Net_UDPConnectionBase_T<ARDrone_UDPHandler_t,
                                                       struct ARDrone_ConnectionConfiguration,
                                                       struct ARDrone_ConnectionState,
                                                       ARDrone_RuntimeStatistic_t,
                                                       struct ARDrone_SocketHandlerConfiguration,
                                                       ARDrone_NetStream_t,
                                                       struct ARDrone_UserData>,
                               ACE_SOCK_CONNECTOR,
                               ACE_INET_Addr,
                               struct ARDrone_ConnectionConfiguration,
                               struct ARDrone_ConnectionState,
                               ARDrone_RuntimeStatistic_t,
                               struct ARDrone_SocketHandlerConfiguration,
                               ARDrone_NetStream_t,
                               struct ARDrone_UserData> ARDrone_UDPConnector_t;

typedef Net_Client_AsynchConnector_T<Net_AsynchTCPConnectionBase_T<ARDrone_AsynchTCPHandler_t,
                                                                   struct ARDrone_ConnectionConfiguration,
                                                                   struct ARDrone_ConnectionState,
                                                                   ARDrone_RuntimeStatistic_t,
                                                                   struct ARDrone_SocketHandlerConfiguration,
                                                                   ARDrone_NetStream_t,
                                                                   struct ARDrone_UserData>,
                                     ACE_INET_Addr,
                                     struct ARDrone_ConnectionConfiguration,
                                     struct ARDrone_ConnectionState,
                                     ARDrone_RuntimeStatistic_t,
                                     struct ARDrone_SocketHandlerConfiguration,
                                     ARDrone_NetStream_t,
                                     struct ARDrone_UserData> ARDrone_AsynchTCPConnector_t;
typedef Net_Client_AsynchConnector_T<Net_AsynchUDPConnectionBase_T<ARDrone_AsynchUDPHandler_t,
                                                                   struct ARDrone_ConnectionConfiguration,
                                                                   struct ARDrone_ConnectionState,
                                                                   ARDrone_RuntimeStatistic_t,
                                                                   struct ARDrone_SocketHandlerConfiguration,
                                                                   ARDrone_NetStream_t,
                                                                   struct ARDrone_UserData>,
                                     ACE_INET_Addr,
                                     struct ARDrone_ConnectionConfiguration,
                                     struct ARDrone_ConnectionState,
                                     ARDrone_RuntimeStatistic_t,
                                     struct ARDrone_SocketHandlerConfiguration,
                                     ARDrone_NetStream_t,
                                     struct ARDrone_UserData> ARDrone_AsynchUDPConnector_t;

typedef ACE_Singleton<ARDrone_ConnectionManager_t,
                      ACE_SYNCH_MUTEX> ARDRONE_CONNECTIONMANAGER_SINGLETON;

#endif // #ifndef ARDRONE_NETWORK_H
