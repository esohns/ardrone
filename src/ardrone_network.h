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

#include "ardrone_configuration.h"
#include "ardrone_defines.h"
#include "ardrone_network_common.h"
#include "ardrone_stream_common.h"
#include "ardrone_types.h"

// forward declarations
class ARDrone_Message;
class ARDrone_SessionMessage;
struct ARDrone_UserData;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Net_IConnection_T<ACE_INET_Addr,
                          ARDrone_DirectShow_ConnectionConfiguration_t,
                          struct ARDrone_ConnectionState,
                          struct ARDrone_Statistic> ARDrone_DirectShow_IConnection_t;
typedef Net_IConnection_T<ACE_INET_Addr,
                          ARDrone_MediaFoundation_ConnectionConfiguration_t,
                          struct ARDrone_ConnectionState,
                          struct ARDrone_Statistic> ARDrone_MediaFoundation_IConnection_t;

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
typedef Net_Connection_Manager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 ARDrone_ConnectionConfiguration_t,
                                 struct ARDrone_ConnectionState,
                                 struct ARDrone_Statistic,
                                 struct ARDrone_UserData> ARDrone_ConnectionManager_t;
#endif // ACE_WIN32 || ACE_WIN64

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
                                      ARDrone_DirectShow_SessionData,
                                      ARDrone_DirectShow_SessionData_t,
                                      ARDrone_ControlMessage_t,
                                      ARDrone_Message,
                                      ARDrone_DirectShow_SessionMessage,
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
                                      ARDrone_DirectShow_SessionData,
                                      ARDrone_DirectShow_SessionData_t,
                                      ARDrone_ControlMessage_t,
                                      ARDrone_Message,
                                      ARDrone_DirectShow_SessionMessage,
                                      ACE_INET_Addr,
                                      ARDrone_MediaFoundation_ConnectionManager_t,
                                      struct ARDrone_UserData> ARDrone_MediaFoundation_NetStream_t;

typedef Net_IStreamConnection_T<ACE_INET_Addr,
                                 ARDrone_DirectShow_ConnectionConfiguration_t,
                                 struct ARDrone_ConnectionState,
                                 struct ARDrone_Statistic,
                                 struct Net_UDPSocketConfiguration,
                                 struct ARDrone_DirectShow_SocketHandlerConfiguration,
                                 ARDrone_DirectShow_NetStream_t,
                                 enum Stream_StateMachine_ControlState> ARDrone_DirectShow_IStreamConnection_t;
typedef Net_IStreamConnection_T<ACE_INET_Addr,
                                 ARDrone_MediaFoundation_ConnectionConfiguration_t,
                                 struct ARDrone_ConnectionState,
                                 struct ARDrone_Statistic,
                                 struct Net_UDPSocketConfiguration,
                                 struct ARDrone_MediaFoundation_SocketHandlerConfiguration,
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
                                      ARDrone_SessionData_t,
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
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Net_TCPSocketHandler_T<ACE_NULL_SYNCH,
                               ACE_SOCK_STREAM,
                               struct ARDrone_DirectShow_SocketHandlerConfiguration> ARDrone_DirectShow_TCPSocketHandler_t;
typedef Net_AsynchTCPSocketHandler_T<struct ARDrone_DirectShow_SocketHandlerConfiguration> ARDrone_DirectShow_AsynchTCPSocketHandler_t;
typedef Net_UDPSocketHandler_T<ACE_NULL_SYNCH,
                               Net_SOCK_Dgram,
                               struct ARDrone_DirectShow_SocketHandlerConfiguration> ARDrone_DirectShow_UDPSocketHandler_t;
typedef Net_AsynchUDPSocketHandler_T<Net_SOCK_Dgram,
                                     struct ARDrone_DirectShow_SocketHandlerConfiguration> ARDrone_DirectShow_AsynchUDPSocketHandler_t;

typedef Net_IConnector_T<ACE_INET_Addr,
                         struct ARDrone_DirectShow_SocketHandlerConfiguration> ARDrone_DirectShow_IConnector_t;
typedef Net_IAsynchConnector_T<ACE_INET_Addr,
                               struct ARDrone_DirectShow_SocketHandlerConfiguration> ARDrone_DirectShow_IAsynchConnector_t;

typedef Net_TCPSocketHandler_T<ACE_NULL_SYNCH,
                               ACE_SOCK_STREAM,
                               struct ARDrone_MediaFoundation_SocketHandlerConfiguration> ARDrone_MediaFoundation_TCPSocketHandler_t;
typedef Net_AsynchTCPSocketHandler_T<struct ARDrone_MediaFoundation_SocketHandlerConfiguration> ARDrone_MediaFoundation_AsynchTCPSocketHandler_t;
typedef Net_UDPSocketHandler_T<ACE_NULL_SYNCH,
                               Net_SOCK_Dgram,
                               struct ARDrone_MediaFoundation_SocketHandlerConfiguration> ARDrone_MediaFoundation_UDPSocketHandler_t;
typedef Net_AsynchUDPSocketHandler_T<Net_SOCK_Dgram,
                                     struct ARDrone_MediaFoundation_SocketHandlerConfiguration> ARDrone_MediaFoundation_AsynchUDPSocketHandler_t;

typedef Net_IConnector_T<ACE_INET_Addr,
                         struct ARDrone_MediaFoundation_SocketHandlerConfiguration> ARDrone_MediaFoundation_IConnector_t;
typedef Net_IAsynchConnector_T<ACE_INET_Addr,
                               struct ARDrone_MediaFoundation_SocketHandlerConfiguration> ARDrone_MediaFoundation_IAsynchConnector_t;
#else
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
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Net_Client_Connector_T<ACE_NULL_SYNCH,
                               Net_TCPConnectionBase_T<ACE_NULL_SYNCH,
                                                       ARDrone_DirectShow_TCPSocketHandler_t,
                                                       ARDrone_DirectShow_ConnectionConfiguration_t,
                                                       struct ARDrone_ConnectionState,
                                                       struct ARDrone_Statistic,
                                                       struct ARDrone_DirectShow_SocketHandlerConfiguration,
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
                               struct ARDrone_DirectShow_SocketHandlerConfiguration,
                               ARDrone_DirectShow_NetStream_t,
                               struct ARDrone_UserData> ARDrone_DirectShow_TCPConnector_t;
typedef Net_Client_Connector_T<ACE_NULL_SYNCH,
                               Net_UDPConnectionBase_T<ACE_NULL_SYNCH,
                                                       ARDrone_DirectShow_UDPSocketHandler_t,
                                                       ARDrone_DirectShow_ConnectionConfiguration_t,
                                                       struct ARDrone_ConnectionState,
                                                       struct ARDrone_Statistic,
                                                       struct ARDrone_DirectShow_SocketHandlerConfiguration,
                                                       ARDrone_DirectShow_NetStream_t,
                                                       Common_Timer_Manager_t,
                                                       struct ARDrone_UserData>,
                               ACE_SOCK_CONNECTOR,
                               ACE_INET_Addr,
                               ARDrone_DirectShow_ConnectionConfiguration_t,
                               struct ARDrone_ConnectionState,
                               struct ARDrone_Statistic,
                               struct Net_UDPSocketConfiguration,
                               struct ARDrone_DirectShow_SocketHandlerConfiguration,
                               ARDrone_DirectShow_NetStream_t,
                               struct ARDrone_UserData> ARDrone_DirectShow_UDPConnector_t;

typedef Net_Client_AsynchConnector_T<Net_AsynchTCPConnectionBase_T<ARDrone_DirectShow_AsynchTCPSocketHandler_t,
                                                                   ARDrone_DirectShow_ConnectionConfiguration_t,
                                                                   struct ARDrone_ConnectionState,
                                                                   struct ARDrone_Statistic,
                                                                   struct ARDrone_DirectShow_SocketHandlerConfiguration,
                                                                   struct Net_ListenerConfiguration,
                                                                   ARDrone_DirectShow_NetStream_t,
                                                                   Common_Timer_Manager_t,
                                                                   struct ARDrone_UserData>,
                                     ACE_INET_Addr,
                                     ARDrone_DirectShow_ConnectionConfiguration_t,
                                     struct ARDrone_ConnectionState,
                                     struct ARDrone_Statistic,
                                     struct Net_TCPSocketConfiguration,
                                     struct ARDrone_DirectShow_SocketHandlerConfiguration,
                                     ARDrone_DirectShow_NetStream_t,
                                     struct ARDrone_UserData> ARDrone_DirectShow_AsynchTCPConnector_t;
typedef Net_Client_AsynchConnector_T<Net_AsynchUDPConnectionBase_T<ARDrone_DirectShow_AsynchUDPSocketHandler_t,
                                                                   ARDrone_DirectShow_ConnectionConfiguration_t,
                                                                   struct ARDrone_ConnectionState,
                                                                   struct ARDrone_Statistic,
                                                                   struct ARDrone_DirectShow_SocketHandlerConfiguration,
                                                                   ARDrone_DirectShow_NetStream_t,
                                                                   Common_Timer_Manager_t,
                                                                   struct ARDrone_UserData>,
                                     ACE_INET_Addr,
                                     ARDrone_DirectShow_ConnectionConfiguration_t,
                                     struct ARDrone_ConnectionState,
                                     struct ARDrone_Statistic,
                                     struct Net_UDPSocketConfiguration,
                                     struct ARDrone_DirectShow_SocketHandlerConfiguration,
                                     ARDrone_DirectShow_NetStream_t,
                                     struct ARDrone_UserData> ARDrone_DirectShow_AsynchUDPConnector_t;

typedef Net_Client_Connector_T<ACE_NULL_SYNCH,
                               Net_TCPConnectionBase_T<ACE_NULL_SYNCH,
                                                       ARDrone_MediaFoundation_TCPSocketHandler_t,
                                                       ARDrone_MediaFoundation_ConnectionConfiguration_t,
                                                       struct ARDrone_ConnectionState,
                                                       struct ARDrone_Statistic,
                                                       struct ARDrone_MediaFoundation_SocketHandlerConfiguration,
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
                               struct ARDrone_MediaFoundation_SocketHandlerConfiguration,
                               ARDrone_MediaFoundation_NetStream_t,
                               struct ARDrone_UserData> ARDrone_MediaFoundation_TCPConnector_t;
typedef Net_Client_Connector_T<ACE_NULL_SYNCH,
                               Net_UDPConnectionBase_T<ACE_NULL_SYNCH,
                                                       ARDrone_MediaFoundation_UDPSocketHandler_t,
                                                       ARDrone_MediaFoundation_ConnectionConfiguration_t,
                                                       struct ARDrone_ConnectionState,
                                                       struct ARDrone_Statistic,
                                                       struct ARDrone_MediaFoundation_SocketHandlerConfiguration,
                                                       ARDrone_MediaFoundation_NetStream_t,
                                                       Common_Timer_Manager_t,
                                                       struct ARDrone_UserData>,
                               ACE_SOCK_CONNECTOR,
                               ACE_INET_Addr,
                               ARDrone_MediaFoundation_ConnectionConfiguration_t,
                               struct ARDrone_ConnectionState,
                               struct ARDrone_Statistic,
                               struct Net_UDPSocketConfiguration,
                               struct ARDrone_MediaFoundation_SocketHandlerConfiguration,
                               ARDrone_MediaFoundation_NetStream_t,
                               struct ARDrone_UserData> ARDrone_MediaFoundation_UDPConnector_t;

typedef Net_Client_AsynchConnector_T<Net_AsynchTCPConnectionBase_T<ARDrone_MediaFoundation_AsynchTCPSocketHandler_t,
                                                                   ARDrone_MediaFoundation_ConnectionConfiguration_t,
                                                                   struct ARDrone_ConnectionState,
                                                                   struct ARDrone_Statistic,
                                                                   struct ARDrone_MediaFoundation_SocketHandlerConfiguration,
                                                                   struct Net_ListenerConfiguration,
                                                                   ARDrone_MediaFoundation_NetStream_t,
                                                                   Common_Timer_Manager_t,
                                                                   struct ARDrone_UserData>,
                                     ACE_INET_Addr,
                                     ARDrone_MediaFoundation_ConnectionConfiguration_t,
                                     struct ARDrone_ConnectionState,
                                     struct ARDrone_Statistic,
                                     struct Net_TCPSocketConfiguration,
                                     struct ARDrone_MediaFoundation_SocketHandlerConfiguration,
                                     ARDrone_MediaFoundation_NetStream_t,
                                     struct ARDrone_UserData> ARDrone_MediaFoundation_AsynchTCPConnector_t;
typedef Net_Client_AsynchConnector_T<Net_AsynchUDPConnectionBase_T<ARDrone_MediaFoundation_AsynchUDPSocketHandler_t,
                                                                   ARDrone_MediaFoundation_ConnectionConfiguration_t,
                                                                   struct ARDrone_ConnectionState,
                                                                   struct ARDrone_Statistic,
                                                                   struct ARDrone_MediaFoundation_SocketHandlerConfiguration,
                                                                   ARDrone_MediaFoundation_NetStream_t,
                                                                   Common_Timer_Manager_t,
                                                                   struct ARDrone_UserData>,
                                     ACE_INET_Addr,
                                     ARDrone_MediaFoundation_ConnectionConfiguration_t,
                                     struct ARDrone_ConnectionState,
                                     struct ARDrone_Statistic,
                                     struct Net_UDPSocketConfiguration,
                                     struct ARDrone_MediaFoundation_SocketHandlerConfiguration,
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
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef ACE_Singleton<ARDrone_DirectShow_ConnectionManager_t,
                      ACE_SYNCH_MUTEX> ARDRONE_DIRECTSHOW_CONNECTIONMANAGER_SINGLETON;
typedef ACE_Singleton<ARDrone_MediaFoundation_ConnectionManager_t,
                      ACE_SYNCH_MUTEX> ARDRONE_MEDIAFOUNDATION_CONNECTIONMANAGER_SINGLETON;
#else
typedef ACE_Singleton<ARDrone_ConnectionManager_t,
                      ACE_SYNCH_MUTEX> ARDRONE_CONNECTIONMANAGER_SINGLETON;
#endif // ACE_WIN32 || ACE_WIN64
typedef ACE_Singleton<ARDrone_WLANMonitor_t,
                      ACE_SYNCH_MUTEX> ARDRONE_WLANMONITOR_SINGLETON;

#endif // #ifndef ARDRONE_NETWORK_H
