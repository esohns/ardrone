/***************************************************************************
*   Copyright (C) 2009 by Erik Sohns   *
*   erik.sohns@web.de   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

#ifndef ARDRONE_NETWORK_H
#define ARDRONE_NETWORK_H

#include <map>
#include <string>

#include "ace/Global_Macros.h"
#include "ace/INET_Addr.h"
#include "ace/Singleton.h"
#include "ace/SOCK_Connector.h"
#include "ace/Synch_Traits.h"

#include "common_time_common.h"
#include "common_timer_manager_common.h"

#include "stream_common.h"
#include "stream_configuration.h"
#include "stream_control_message.h"
#include "stream_session_data.h"
#include "common_statistic_handler.h"

#include "stream_net_io_stream.h"

#include "net_asynch_tcpsockethandler.h"
#include "net_asynch_udpsockethandler.h"
#include "net_common.h"
#include "net_configuration.h"
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

#include "net_wlan_inetmonitor.h"

#include "ardrone_defines.h"
#include "ardrone_types.h"

// forward declarations
class ARDrone_Message;
class ARDrone_SessionMessage;
struct ARDrone_ConnectionState;
//typedef Stream_StatisticHandler_T<struct ARDrone_Statistic> Common_Timer_Manager_t;
struct ARDrone_UserData;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
struct ARDrone_DirectShow_ConnectionConfiguration;
struct ARDrone_MediaFoundation_ConnectionConfiguration;
#else
struct ARDrone_ConnectionConfiguration;
#endif
struct ARDrone_SocketHandlerConfiguration
 : Net_SocketHandlerConfiguration
{
  ARDrone_SocketHandlerConfiguration ()
   : Net_SocketHandlerConfiguration ()
   , socketConfiguration_2 ()
   , socketConfiguration_3 ()
#if defined (ACE_WIN32) || defined (ACE_WIN64)
   , directShowConnectionConfiguration (NULL)
   , mediaFoundationConnectionConfiguration (NULL)
#else
   , connectionConfiguration (NULL)
#endif
   , userData (NULL)
  {
    socketConfiguration = &socketConfiguration_2;
  }

  struct Net_TCPSocketConfiguration                       socketConfiguration_2;
  struct Net_UDPSocketConfiguration                       socketConfiguration_3;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct ARDrone_DirectShow_ConnectionConfiguration*      directShowConnectionConfiguration;
  struct ARDrone_MediaFoundation_ConnectionConfiguration* mediaFoundationConnectionConfiguration;
#else
  struct ARDrone_ConnectionConfiguration*                 connectionConfiguration;
#endif

  struct ARDrone_UserData*                                userData;
};

#if defined (ACE_WIN32) || defined (ACE_WIN64)
//extern const char stream_name_string_[];
struct ARDrone_StreamConfiguration;
struct ARDrone_DirectShow_ModuleHandlerConfiguration;
typedef Stream_Configuration_T<//stream_name_string_,
                               struct ARDrone_AllocatorConfiguration,
                               struct ARDrone_StreamConfiguration,
                               struct Stream_ModuleConfiguration,
                               struct ARDrone_DirectShow_ModuleHandlerConfiguration> ARDrone_DirectShow_StreamConfiguration_t;
struct ARDrone_MediaFoundation_ModuleHandlerConfiguration;
typedef Stream_Configuration_T<//stream_name_string_,
                               struct ARDrone_AllocatorConfiguration,
                               struct ARDrone_StreamConfiguration,
                               struct Stream_ModuleConfiguration,
                               struct ARDrone_MediaFoundation_ModuleHandlerConfiguration> ARDrone_MediaFoundation_StreamConfiguration_t;
#else
//extern const char stream_name_string_[];
struct ARDrone_ModuleHandlerConfiguration;
typedef Stream_Configuration_T<//stream_name_string_,
                               struct ARDrone_AllocatorConfiguration,
                               struct ARDrone_StreamConfiguration,
                               struct Stream_ModuleConfiguration,
                               struct ARDrone_ModuleHandlerConfiguration> ARDrone_StreamConfiguration_t;
#endif

#if defined (ACE_WIN32) || defined (ACE_WIN64)
struct ARDrone_DirectShow_ConnectionConfiguration;
typedef Net_StreamConnectionConfiguration_T<struct ARDrone_DirectShow_ConnectionConfiguration,
                                            struct ARDrone_AllocatorConfiguration,
                                            ARDrone_DirectShow_StreamConfiguration_t> ARDrone_DirectShow_ConnectionConfiguration_t;
typedef Net_IConnectionManager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 ARDrone_DirectShow_ConnectionConfiguration_t,
                                 struct ARDrone_ConnectionState,
                                 struct ARDrone_Statistic,
                                 struct ARDrone_UserData> ARDrone_DirectShow_IConnectionManager_t;
struct ARDrone_DirectShow_ConnectionConfiguration
 : Net_ConnectionConfiguration
{
  ARDrone_DirectShow_ConnectionConfiguration ()
   : Net_ConnectionConfiguration ()
   , connectionManager (NULL)
   , socketHandlerConfiguration ()
   , userData (NULL)
  {
    PDUSize = ARDRONE_MESSAGE_BUFFER_SIZE;
  }

  ARDrone_DirectShow_IConnectionManager_t*  connectionManager;
  struct ARDrone_SocketHandlerConfiguration socketHandlerConfiguration;

  struct ARDrone_UserData*                  userData;
};

struct ARDrone_MediaFoundation_ConnectionConfiguration;
typedef Net_StreamConnectionConfiguration_T<struct ARDrone_MediaFoundation_ConnectionConfiguration,
                                            struct ARDrone_AllocatorConfiguration,
                                            ARDrone_MediaFoundation_StreamConfiguration_t> ARDrone_MediaFoundation_ConnectionConfiguration_t;
typedef Net_IConnectionManager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 ARDrone_MediaFoundation_ConnectionConfiguration_t,
                                 struct ARDrone_ConnectionState,
                                 struct ARDrone_Statistic,
                                 struct ARDrone_UserData> ARDrone_MediaFoundation_IConnectionManager_t;
struct ARDrone_MediaFoundation_ConnectionConfiguration
 : Net_ConnectionConfiguration
{
  ARDrone_MediaFoundation_ConnectionConfiguration ()
   : Net_ConnectionConfiguration ()
   , connectionManager (NULL)
   , socketHandlerConfiguration ()
   , userData (NULL)
  {
    PDUSize = ARDRONE_MESSAGE_BUFFER_SIZE;
  }

  ARDrone_MediaFoundation_IConnectionManager_t* connectionManager;
  struct ARDrone_SocketHandlerConfiguration     socketHandlerConfiguration;

  struct ARDrone_UserData*                      userData;
};

typedef std::map<std::string, // module name
                 ARDrone_DirectShow_ConnectionConfiguration_t> ARDrone_DirectShow_StreamConnectionConfigurations_t;
typedef ARDrone_DirectShow_StreamConnectionConfigurations_t::iterator ARDrone_DirectShow_StreamConnectionConfigurationIterator_t;
typedef std::map<std::string, // stream name
                 ARDrone_DirectShow_StreamConnectionConfigurations_t> ARDrone_DirectShow_ConnectionConfigurations_t;
typedef ARDrone_DirectShow_ConnectionConfigurations_t::iterator ARDrone_DirectShow_ConnectionConfigurationIterator_t;
typedef std::map<std::string, // module name
                 ARDrone_MediaFoundation_ConnectionConfiguration_t> ARDrone_MediaFoundation_StreamConnectionConfigurations_t;
typedef ARDrone_MediaFoundation_StreamConnectionConfigurations_t::iterator ARDrone_MediaFoundation_StreamConnectionConfigurationIterator_t;
typedef std::map<std::string, // stream name
                 ARDrone_MediaFoundation_StreamConnectionConfigurations_t> ARDrone_MediaFoundation_ConnectionConfigurations_t;
typedef ARDrone_MediaFoundation_ConnectionConfigurations_t::iterator ARDrone_MediaFoundation_ConnectionConfigurationIterator_t;
#else
struct ARDrone_ConnectionConfiguration;
typedef Net_StreamConnectionConfiguration_T<struct ARDrone_ConnectionConfiguration,
                                            struct ARDrone_AllocatorConfiguration,
                                            ARDrone_StreamConfiguration_t> ARDrone_ConnectionConfiguration_t;
typedef Net_IConnectionManager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 ARDrone_ConnectionConfiguration_t,
                                 struct ARDrone_ConnectionState,
                                 struct ARDrone_Statistic,
                                 struct ARDrone_UserData> ARDrone_IConnectionManager_t;
struct ARDrone_ConnectionConfiguration
 : Net_ConnectionConfiguration
{
  ARDrone_ConnectionConfiguration ()
   : Net_ConnectionConfiguration ()
   , connectionManager (NULL)
   , socketHandlerConfiguration ()
   , userData (NULL)
  {
    PDUSize = ARDRONE_MESSAGE_BUFFER_SIZE;
  }

  ARDrone_IConnectionManager_t*             connectionManager;
  struct ARDrone_SocketHandlerConfiguration socketHandlerConfiguration;

  struct ARDrone_UserData*                  userData;
};
typedef std::map<std::string, // module name
                 ARDrone_ConnectionConfiguration_t> ARDrone_StreamConnectionConfigurations_t;
typedef ARDrone_StreamConnectionConfigurations_t::iterator ARDrone_StreamConnectionConfigurationIterator_t;
typedef std::map<std::string, // stream name
                 ARDrone_StreamConnectionConfigurations_t> ARDrone_ConnectionConfigurations_t;
typedef ARDrone_ConnectionConfigurations_t::iterator ARDrone_ConnectionConfigurationIterator_t;
#endif

struct ARDrone_Configuration;
struct ARDrone_ConnectionState
 : Net_ConnectionState
{
  ARDrone_ConnectionState ()
   : Net_ConnectionState ()
   , configuration (NULL)
   , statistic ()
   , userData (NULL)
  {}

  // *TODO*: consider making this a separate entity (i.e. a pointer)
  struct ARDrone_Configuration* configuration;
  struct ARDrone_Statistic      statistic;

  struct ARDrone_UserData*      userData;
};

typedef Stream_SessionData_T<struct ARDrone_SessionData> ARDrone_StreamSessionData_t;
typedef Stream_ControlMessage_T<enum Stream_ControlType,
                                enum Stream_ControlMessageType,
                                struct ARDrone_AllocatorConfiguration> ARDrone_ControlMessage_t;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Net_IConnection_T<ACE_INET_Addr,
                          ARDrone_DirectShow_ConnectionConfiguration_t,
                          struct ARDrone_ConnectionState,
                          struct ARDrone_Statistic> ARDrone_DirectShow_IConnection_t;
typedef Net_IConnection_T<ACE_INET_Addr,
                          ARDrone_MediaFoundation_ConnectionConfiguration_t,
                          struct ARDrone_ConnectionState,
                          struct ARDrone_Statistic> ARDrone_MediaFoundation_IConnection_t;

typedef Net_IConnectionManager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 ARDrone_DirectShow_ConnectionConfiguration_t,
                                 struct ARDrone_ConnectionState,
                                 struct ARDrone_Statistic,
                                 struct ARDrone_UserData> ARDrone_DirectShow_IConnectionManager_t;
typedef Net_IConnectionManager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 ARDrone_MediaFoundation_ConnectionConfiguration_t,
                                 struct ARDrone_ConnectionState,
                                 struct ARDrone_Statistic,
                                 struct ARDrone_UserData> ARDrone_MediaFoundation_IConnectionManager_t;

typedef Net_Connection_Manager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 ARDrone_DirectShow_ConnectionConfiguration_t,
                                 struct ARDrone_ConnectionState,
                                 struct ARDrone_Statistic,
                                 struct ARDrone_UserData> ARDrone_DirectShow_ConnectionManager_t;
typedef Net_Connection_Manager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 ARDrone_MediaFoundation_ConnectionConfiguration_t,
                                 struct ARDrone_ConnectionState,
                                 struct ARDrone_Statistic,
                                 struct ARDrone_UserData> ARDrone_MediaFoundation_ConnectionManager_t;
#else
typedef Net_IConnection_T<ACE_INET_Addr,
                          ARDrone_ConnectionConfiguration_t,
                          struct ARDrone_ConnectionState,
                          struct ARDrone_Statistic> ARDrone_IConnection_t;

typedef Net_IConnectionManager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 ARDrone_ConnectionConfiguration_t,
                                 struct ARDrone_ConnectionState,
                                 struct ARDrone_Statistic,
                                 struct ARDrone_UserData> ARDrone_IConnectionManager_t;
typedef Net_Connection_Manager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 ARDrone_ConnectionConfiguration_t,
                                 struct ARDrone_ConnectionState,
                                 struct ARDrone_Statistic,
                                 struct ARDrone_UserData> ARDrone_ConnectionManager_t;
#endif
#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Stream_Module_Net_IO_Stream_T<ACE_MT_SYNCH,
                                      Common_TimePolicy_t,
                                      libacestream_default_net_stream_name_string,
                                      enum Stream_ControlType,
                                      enum Stream_SessionMessageType,
                                      enum Stream_StateMachine_ControlState,
                                      struct ARDrone_StreamState,
                                      struct ARDrone_StreamConfiguration,
                                      struct ARDrone_Statistic,
                                      Common_Timer_Manager_t,
                                      struct ARDrone_AllocatorConfiguration,
                                      struct Stream_ModuleConfiguration,
                                      struct ARDrone_DirectShow_ModuleHandlerConfiguration,
                                      struct ARDrone_SessionData,
                                      ARDrone_StreamSessionData_t,
                                      ARDrone_ControlMessage_t,
                                      ARDrone_Message,
                                      ARDrone_SessionMessage,
                                      ACE_INET_Addr,
                                      ARDrone_DirectShow_ConnectionManager_t,
                                      struct ARDrone_UserData> ARDrone_DirectShow_NetStream_t;
typedef Stream_Module_Net_IO_Stream_T<ACE_MT_SYNCH,
                                      Common_TimePolicy_t,
                                      libacestream_default_net_stream_name_string,
                                      enum Stream_ControlType,
                                      enum Stream_SessionMessageType,
                                      enum Stream_StateMachine_ControlState,
                                      struct ARDrone_StreamState,
                                      struct ARDrone_StreamConfiguration,
                                      struct ARDrone_Statistic,
                                      Common_Timer_Manager_t,
                                      struct ARDrone_AllocatorConfiguration,
                                      struct Stream_ModuleConfiguration,
                                      struct ARDrone_MediaFoundation_ModuleHandlerConfiguration,
                                      struct ARDrone_SessionData,
                                      ARDrone_StreamSessionData_t,
                                      ARDrone_ControlMessage_t,
                                      ARDrone_Message,
                                      ARDrone_SessionMessage,
                                      ACE_INET_Addr,
                                      ARDrone_MediaFoundation_ConnectionManager_t,
                                      struct ARDrone_UserData> ARDrone_MediaFoundation_NetStream_t;

typedef Net_IStreamConnection_T<ACE_INET_Addr,
                                 ARDrone_DirectShow_ConnectionConfiguration_t,
                                 struct ARDrone_ConnectionState,
                                 struct ARDrone_Statistic,
                                 struct Net_UDPSocketConfiguration,
                                 struct ARDrone_SocketHandlerConfiguration,
                                 ARDrone_DirectShow_NetStream_t,
                                 enum Stream_StateMachine_ControlState> ARDrone_DirectShow_IStreamConnection_t;
typedef Net_IStreamConnection_T<ACE_INET_Addr,
                                 ARDrone_MediaFoundation_ConnectionConfiguration_t,
                                 struct ARDrone_ConnectionState,
                                 struct ARDrone_Statistic,
                                 struct Net_UDPSocketConfiguration,
                                 struct ARDrone_SocketHandlerConfiguration,
                                 ARDrone_MediaFoundation_NetStream_t,
                                 enum Stream_StateMachine_ControlState> ARDrone_MediaFoundation_IStreamConnection_t;
#else
typedef Stream_Module_Net_IO_Stream_T<ACE_MT_SYNCH,
                                      Common_TimePolicy_t,
                                      libacestream_default_net_stream_name_string,
                                      enum Stream_ControlType,
                                      enum Stream_SessionMessageType,
                                      enum Stream_StateMachine_ControlState,
                                      struct ARDrone_StreamState,
                                      struct ARDrone_StreamConfiguration,
                                      struct ARDrone_Statistic,
                                      Common_Timer_Manager_t,
                                      struct ARDrone_AllocatorConfiguration,
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

typedef Net_IStreamConnection_T<ACE_INET_Addr,
                                 ARDrone_ConnectionConfiguration_t,
                                 struct ARDrone_ConnectionState,
                                 struct ARDrone_Statistic,
                                 struct Net_UDPSocketConfiguration,
                                 struct ARDrone_SocketHandlerConfiguration,
                                 ARDrone_NetStream_t,
                                 enum Stream_StateMachine_ControlState> ARDrone_IStreamConnection_t;
#endif

typedef Net_WLAN_InetMonitor_T<struct ARDrone_WLANMonitorConfiguration,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                               NET_WLAN_MONITOR_API_WLANAPI,
#else
                               ACE_MT_SYNCH,
                               Common_TimePolicy_t,
                               NET_WLAN_MONITOR_API_NL80211,
#endif
                               struct ARDrone_UserData> ARDrone_WLANMonitor_t;

typedef Net_TCPSocketHandler_T<ACE_NULL_SYNCH,
                               ACE_SOCK_STREAM,
                               struct ARDrone_SocketHandlerConfiguration> ARDrone_TCPSocketHandler_t;
typedef Net_AsynchTCPSocketHandler_T<struct ARDrone_SocketHandlerConfiguration> ARDrone_AsynchTCPSocketHandler_t;
typedef Net_UDPSocketHandler_T<ACE_NULL_SYNCH,
                               Net_SOCK_Dgram,
                               struct ARDrone_SocketHandlerConfiguration> ARDrone_UDPSocketHandler_t;
typedef Net_AsynchUDPSocketHandler_T<Net_SOCK_Dgram,
                                     struct ARDrone_SocketHandlerConfiguration> ARDrone_AsynchUDPSocketHandler_t;

typedef Net_IConnector_T<ACE_INET_Addr,
                         struct ARDrone_SocketHandlerConfiguration> ARDrone_IConnector_t;
typedef Net_IAsynchConnector_T<ACE_INET_Addr,
                               struct ARDrone_SocketHandlerConfiguration> ARDrone_IAsynchConnector_t;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Net_Client_Connector_T<ACE_NULL_SYNCH,
                               Net_TCPConnectionBase_T<ACE_NULL_SYNCH,
                                                       ARDrone_TCPSocketHandler_t,
                                                       ARDrone_DirectShow_ConnectionConfiguration_t,
                                                       struct ARDrone_ConnectionState,
                                                       struct ARDrone_Statistic,
                                                       struct ARDrone_SocketHandlerConfiguration,
                                                       struct Net_ListenerConfiguration,
                                                       ARDrone_DirectShow_NetStream_t,
                                                       Common_Timer_Manager_t,
                                                       struct ARDrone_UserData>,
                               ACE_SOCK_CONNECTOR,
                               ACE_INET_Addr,
                               ARDrone_DirectShow_ConnectionConfiguration_t,
                               struct ARDrone_ConnectionState,
                               struct ARDrone_Statistic,
                               struct Net_TCPSocketConfiguration,
                               struct ARDrone_SocketHandlerConfiguration,
                               ARDrone_DirectShow_NetStream_t,
                               struct ARDrone_UserData> ARDrone_DirectShow_TCPConnector_t;
typedef Net_Client_Connector_T<ACE_NULL_SYNCH,
                               Net_UDPConnectionBase_T<ACE_NULL_SYNCH,
                                                       ARDrone_UDPSocketHandler_t,
                                                       ARDrone_DirectShow_ConnectionConfiguration_t,
                                                       struct ARDrone_ConnectionState,
                                                       struct ARDrone_Statistic,
                                                       struct ARDrone_SocketHandlerConfiguration,
                                                       ARDrone_DirectShow_NetStream_t,
                                                       Common_Timer_Manager_t,
                                                       struct ARDrone_UserData>,
                               ACE_SOCK_CONNECTOR,
                               ACE_INET_Addr,
                               ARDrone_DirectShow_ConnectionConfiguration_t,
                               struct ARDrone_ConnectionState,
                               struct ARDrone_Statistic,
                               struct Net_UDPSocketConfiguration,
                               struct ARDrone_SocketHandlerConfiguration,
                               ARDrone_DirectShow_NetStream_t,
                               struct ARDrone_UserData> ARDrone_DirectShow_UDPConnector_t;

typedef Net_Client_AsynchConnector_T<Net_AsynchTCPConnectionBase_T<ARDrone_AsynchTCPSocketHandler_t,
                                                                   ARDrone_DirectShow_ConnectionConfiguration_t,
                                                                   struct ARDrone_ConnectionState,
                                                                   struct ARDrone_Statistic,
                                                                   struct ARDrone_SocketHandlerConfiguration,
                                                                   struct Net_ListenerConfiguration,
                                                                   ARDrone_DirectShow_NetStream_t,
                                                                   Common_Timer_Manager_t,
                                                                   struct ARDrone_UserData>,
                                     ACE_INET_Addr,
                                     ARDrone_DirectShow_ConnectionConfiguration_t,
                                     struct ARDrone_ConnectionState,
                                     struct ARDrone_Statistic,
                                     struct Net_TCPSocketConfiguration,
                                     struct ARDrone_SocketHandlerConfiguration,
                                     ARDrone_DirectShow_NetStream_t,
                                     struct ARDrone_UserData> ARDrone_DirectShow_AsynchTCPConnector_t;
typedef Net_Client_AsynchConnector_T<Net_AsynchUDPConnectionBase_T<ARDrone_AsynchUDPSocketHandler_t,
                                                                   ARDrone_DirectShow_ConnectionConfiguration_t,
                                                                   struct ARDrone_ConnectionState,
                                                                   struct ARDrone_Statistic,
                                                                   struct ARDrone_SocketHandlerConfiguration,
                                                                   ARDrone_DirectShow_NetStream_t,
                                                                   Common_Timer_Manager_t,
                                                                   struct ARDrone_UserData>,
                                     ACE_INET_Addr,
                                     ARDrone_DirectShow_ConnectionConfiguration_t,
                                     struct ARDrone_ConnectionState,
                                     struct ARDrone_Statistic,
                                     struct Net_UDPSocketConfiguration,
                                     struct ARDrone_SocketHandlerConfiguration,
                                     ARDrone_DirectShow_NetStream_t,
                                     struct ARDrone_UserData> ARDrone_DirectShow_AsynchUDPConnector_t;

typedef Net_Client_Connector_T<ACE_NULL_SYNCH,
                               Net_TCPConnectionBase_T<ACE_NULL_SYNCH,
                                                       ARDrone_TCPSocketHandler_t,
                                                       ARDrone_MediaFoundation_ConnectionConfiguration_t,
                                                       struct ARDrone_ConnectionState,
                                                       struct ARDrone_Statistic,
                                                       struct ARDrone_SocketHandlerConfiguration,
                                                       struct Net_ListenerConfiguration,
                                                       ARDrone_MediaFoundation_NetStream_t,
                                                       Common_Timer_Manager_t,
                                                       struct ARDrone_UserData>,
                               ACE_SOCK_CONNECTOR,
                               ACE_INET_Addr,
                               ARDrone_MediaFoundation_ConnectionConfiguration_t,
                               struct ARDrone_ConnectionState,
                               struct ARDrone_Statistic,
                               struct Net_TCPSocketConfiguration,
                               struct ARDrone_SocketHandlerConfiguration,
                               ARDrone_MediaFoundation_NetStream_t,
                               struct ARDrone_UserData> ARDrone_MediaFoundation_TCPConnector_t;
typedef Net_Client_Connector_T<ACE_NULL_SYNCH,
                               Net_UDPConnectionBase_T<ACE_NULL_SYNCH,
                                                       ARDrone_UDPSocketHandler_t,
                                                       ARDrone_MediaFoundation_ConnectionConfiguration_t,
                                                       struct ARDrone_ConnectionState,
                                                       struct ARDrone_Statistic,
                                                       struct ARDrone_SocketHandlerConfiguration,
                                                       ARDrone_MediaFoundation_NetStream_t,
                                                       Common_Timer_Manager_t,
                                                       struct ARDrone_UserData>,
                               ACE_SOCK_CONNECTOR,
                               ACE_INET_Addr,
                               ARDrone_MediaFoundation_ConnectionConfiguration_t,
                               struct ARDrone_ConnectionState,
                               struct ARDrone_Statistic,
                               struct Net_UDPSocketConfiguration,
                               struct ARDrone_SocketHandlerConfiguration,
                               ARDrone_MediaFoundation_NetStream_t,
                               struct ARDrone_UserData> ARDrone_MediaFoundation_UDPConnector_t;

typedef Net_Client_AsynchConnector_T<Net_AsynchTCPConnectionBase_T<ARDrone_AsynchTCPSocketHandler_t,
                                                                   ARDrone_MediaFoundation_ConnectionConfiguration_t,
                                                                   struct ARDrone_ConnectionState,
                                                                   struct ARDrone_Statistic,
                                                                   struct ARDrone_SocketHandlerConfiguration,
                                                                   struct Net_ListenerConfiguration,
                                                                   ARDrone_MediaFoundation_NetStream_t,
                                                                   Common_Timer_Manager_t,
                                                                   struct ARDrone_UserData>,
                                     ACE_INET_Addr,
                                     ARDrone_MediaFoundation_ConnectionConfiguration_t,
                                     struct ARDrone_ConnectionState,
                                     struct ARDrone_Statistic,
                                     struct Net_TCPSocketConfiguration,
                                     struct ARDrone_SocketHandlerConfiguration,
                                     ARDrone_MediaFoundation_NetStream_t,
                                     struct ARDrone_UserData> ARDrone_MediaFoundation_AsynchTCPConnector_t;
typedef Net_Client_AsynchConnector_T<Net_AsynchUDPConnectionBase_T<ARDrone_AsynchUDPSocketHandler_t,
                                                                   ARDrone_MediaFoundation_ConnectionConfiguration_t,
                                                                   struct ARDrone_ConnectionState,
                                                                   struct ARDrone_Statistic,
                                                                   struct ARDrone_SocketHandlerConfiguration,
                                                                   ARDrone_MediaFoundation_NetStream_t,
                                                                   Common_Timer_Manager_t,
                                                                   struct ARDrone_UserData>,
                                     ACE_INET_Addr,
                                     ARDrone_MediaFoundation_ConnectionConfiguration_t,
                                     struct ARDrone_ConnectionState,
                                     struct ARDrone_Statistic,
                                     struct Net_UDPSocketConfiguration,
                                     struct ARDrone_SocketHandlerConfiguration,
                                     ARDrone_MediaFoundation_NetStream_t,
                                     struct ARDrone_UserData> ARDrone_MediaFoundation_AsynchUDPConnector_t;
#else
typedef Net_Client_Connector_T<ACE_NULL_SYNCH,
                               Net_TCPConnectionBase_T<ACE_NULL_SYNCH,
                                                       ARDrone_TCPSocketHandler_t,
                                                       ARDrone_ConnectionConfiguration_t,
                                                       struct ARDrone_ConnectionState,
                                                       struct ARDrone_Statistic,
                                                       struct ARDrone_SocketHandlerConfiguration,
                                                       struct Net_ListenerConfiguration,
                                                       ARDrone_NetStream_t,
                                                       Common_Timer_Manager_t,
                                                       struct ARDrone_UserData>,
                               ACE_SOCK_CONNECTOR,
                               ACE_INET_Addr,
                               ARDrone_ConnectionConfiguration_t,
                               struct ARDrone_ConnectionState,
                               struct ARDrone_Statistic,
                               struct Net_TCPSocketConfiguration,
                               struct ARDrone_SocketHandlerConfiguration,
                               ARDrone_NetStream_t,
                               struct ARDrone_UserData> ARDrone_TCPConnector_t;
typedef Net_Client_Connector_T<ACE_NULL_SYNCH,
                               Net_UDPConnectionBase_T<ACE_NULL_SYNCH,
                                                       ARDrone_UDPSocketHandler_t,
                                                       ARDrone_ConnectionConfiguration_t,
                                                       struct ARDrone_ConnectionState,
                                                       struct ARDrone_Statistic,
                                                       struct ARDrone_SocketHandlerConfiguration,
                                                       ARDrone_NetStream_t,
                                                       Common_Timer_Manager_t,
                                                       struct ARDrone_UserData>,
                               ACE_SOCK_CONNECTOR,
                               ACE_INET_Addr,
                               ARDrone_ConnectionConfiguration_t,
                               struct ARDrone_ConnectionState,
                               struct ARDrone_Statistic,
                               struct Net_UDPSocketConfiguration,
                               struct ARDrone_SocketHandlerConfiguration,
                               ARDrone_NetStream_t,
                               struct ARDrone_UserData> ARDrone_UDPConnector_t;

typedef Net_Client_AsynchConnector_T<Net_AsynchTCPConnectionBase_T<ARDrone_AsynchTCPSocketHandler_t,
                                                                   ARDrone_ConnectionConfiguration_t,
                                                                   struct ARDrone_ConnectionState,
                                                                   struct ARDrone_Statistic,
                                                                   struct ARDrone_SocketHandlerConfiguration,
                                                                   struct Net_ListenerConfiguration,
                                                                   ARDrone_NetStream_t,
                                                                   Common_Timer_Manager_t,
                                                                   struct ARDrone_UserData>,
                                     ACE_INET_Addr,
                                     ARDrone_ConnectionConfiguration_t,
                                     struct ARDrone_ConnectionState,
                                     struct ARDrone_Statistic,
                                     struct Net_TCPSocketConfiguration,
                                     struct ARDrone_SocketHandlerConfiguration,
                                     ARDrone_NetStream_t,
                                     struct ARDrone_UserData> ARDrone_AsynchTCPConnector_t;
typedef Net_Client_AsynchConnector_T<Net_AsynchUDPConnectionBase_T<ARDrone_AsynchUDPSocketHandler_t,
                                                                   ARDrone_ConnectionConfiguration_t,
                                                                   struct ARDrone_ConnectionState,
                                                                   struct ARDrone_Statistic,
                                                                   struct ARDrone_SocketHandlerConfiguration,
                                                                   ARDrone_NetStream_t,
                                                                   Common_Timer_Manager_t,
                                                                   struct ARDrone_UserData>,
                                     ACE_INET_Addr,
                                     ARDrone_ConnectionConfiguration_t,
                                     struct ARDrone_ConnectionState,
                                     struct ARDrone_Statistic,
                                     struct Net_UDPSocketConfiguration,
                                     struct ARDrone_SocketHandlerConfiguration,
                                     ARDrone_NetStream_t,
                                     struct ARDrone_UserData> ARDrone_AsynchUDPConnector_t;
#endif

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef ACE_Singleton<ARDrone_DirectShow_ConnectionManager_t,
                      ACE_SYNCH_MUTEX> ARDRONE_DIRECTSHOW_CONNECTIONMANAGER_SINGLETON;
typedef ACE_Singleton<ARDrone_MediaFoundation_ConnectionManager_t,
                      ACE_SYNCH_MUTEX> ARDRONE_MEDIAFOUNDATION_CONNECTIONMANAGER_SINGLETON;
#else
typedef ACE_Singleton<ARDrone_ConnectionManager_t,
                      ACE_SYNCH_MUTEX> ARDRONE_CONNECTIONMANAGER_SINGLETON;
#endif
typedef ACE_Singleton<ARDrone_WLANMonitor_t,
                      ACE_SYNCH_MUTEX> ARDRONE_WLANMONITOR_SINGLETON;

#endif // #ifndef ARDRONE_NETWORK_H
