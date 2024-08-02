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

#include "net_wlan_monitor_common.h"

#include "ardrone_configuration.h"
#include "ardrone_defines.h"
#include "ardrone_network_common.h"
#include "ardrone_stream_common.h"
#include "ardrone_types.h"

// forward declarations
class ARDrone_Message;
class ARDrone_SessionMessage;
struct Net_UserData;

typedef Net_IConnection_T<ACE_INET_Addr,
                          struct ARDrone_ConnectionState,
                          struct Net_StreamStatistic> ARDrone_IConnection_t;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Net_Connection_Manager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 ARDrone_DirectShow_TCPConnectionConfiguration_t,
                                 struct ARDrone_ConnectionState,
                                 struct Net_StreamStatistic,
                                 struct Net_UserData> ARDrone_DirectShow_TCPConnectionManager_t;
typedef Net_Connection_Manager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 ARDrone_DirectShow_UDPConnectionConfiguration_t,
                                 struct ARDrone_ConnectionState,
                                 struct Net_StreamStatistic,
                                 struct Net_UserData> ARDrone_DirectShow_UDPConnectionManager_t;

typedef Net_Connection_Manager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 ARDrone_MediaFoundation_TCPConnectionConfiguration_t,
                                 struct ARDrone_ConnectionState,
                                 struct Net_StreamStatistic,
                                 struct Net_UserData> ARDrone_MediaFoundation_TCPConnectionManager_t;
typedef Net_Connection_Manager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 ARDrone_MediaFoundation_UDPConnectionConfiguration_t,
                                 struct ARDrone_ConnectionState,
                                 struct Net_StreamStatistic,
                                 struct Net_UserData> ARDrone_MediaFoundation_UDPConnectionManager_t;
#else
typedef Net_Connection_Manager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 ARDrone_TCPConnectionConfiguration_t,
                                 struct ARDrone_ConnectionState,
                                 struct Net_StreamStatistic,
                                 struct Net_UserData> ARDrone_TCPConnectionManager_t;
typedef Net_Connection_Manager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 ARDrone_UDPConnectionConfiguration_t,
                                 struct ARDrone_ConnectionState,
                                 struct Net_StreamStatistic,
                                 struct Net_UserData> ARDrone_UDPConnectionManager_t;
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
                                      struct Stream_Statistic,
                                      Common_Timer_Manager_t,
                                      struct ARDrone_DirectShow_ModuleHandlerConfiguration,
                                      ARDrone_DirectShow_SessionData,
                                      ARDrone_DirectShow_SessionData_t,
                                      Stream_ControlMessage_t,
                                      ARDrone_Message,
                                      ARDrone_DirectShow_SessionMessage,
                                      ACE_INET_Addr,
                                      ARDrone_DirectShow_TCPConnectionManager_t,
                                      struct Stream_UserData> ARDrone_DirectShow_TCPNetStream_t;
typedef Stream_Module_Net_IO_Stream_T<ACE_MT_SYNCH,
                                      Common_TimePolicy_t,
                                      libacestream_default_net_stream_name_string,
                                      enum Stream_ControlType,
                                      enum Stream_SessionMessageType,
                                      enum Stream_StateMachine_ControlState,
                                      struct ARDrone_StreamState,
                                      struct ARDrone_StreamConfiguration,
                                      struct Stream_Statistic,
                                      Common_Timer_Manager_t,
                                      struct ARDrone_DirectShow_ModuleHandlerConfiguration,
                                      ARDrone_DirectShow_SessionData,
                                      ARDrone_DirectShow_SessionData_t,
                                      Stream_ControlMessage_t,
                                      ARDrone_Message,
                                      ARDrone_DirectShow_SessionMessage,
                                      ACE_INET_Addr,
                                      ARDrone_DirectShow_UDPConnectionManager_t,
                                      struct Stream_UserData> ARDrone_DirectShow_UDPNetStream_t;

typedef Stream_Module_Net_IO_Stream_T<ACE_MT_SYNCH,
                                      Common_TimePolicy_t,
                                      libacestream_default_net_stream_name_string,
                                      enum Stream_ControlType,
                                      enum Stream_SessionMessageType,
                                      enum Stream_StateMachine_ControlState,
                                      struct ARDrone_StreamState,
                                      struct ARDrone_StreamConfiguration,
                                      struct Stream_Statistic,
                                      Common_Timer_Manager_t,
                                      struct ARDrone_MediaFoundation_ModuleHandlerConfiguration,
                                      ARDrone_DirectShow_SessionData,
                                      ARDrone_DirectShow_SessionData_t,
                                      Stream_ControlMessage_t,
                                      ARDrone_Message,
                                      ARDrone_DirectShow_SessionMessage,
                                      ACE_INET_Addr,
                                      ARDrone_MediaFoundation_TCPConnectionManager_t,
                                      struct Stream_UserData> ARDrone_MediaFoundation_NetStream_t;

typedef Net_IStreamConnection_T<ACE_INET_Addr,
                                 ARDrone_DirectShow_TCPConnectionConfiguration_t,
                                 struct ARDrone_ConnectionState,
                                 struct Net_StreamStatistic,
                                 Net_TCPSocketConfiguration_t,
                                 ARDrone_DirectShow_TCPNetStream_t,
                                 enum Stream_StateMachine_ControlState> ARDrone_DirectShow_ITCPStreamConnection_t;
typedef Net_IStreamConnection_T<ACE_INET_Addr,
                                 ARDrone_MediaFoundation_TCPConnectionConfiguration_t,
                                 struct ARDrone_ConnectionState,
                                 struct Net_StreamStatistic,
                                 Net_TCPSocketConfiguration_t,
                                 ARDrone_MediaFoundation_NetStream_t,
                                 enum Stream_StateMachine_ControlState> ARDrone_MediaFoundation_ITCPStreamConnection_t;
#else
typedef Stream_Module_Net_IO_Stream_T<ACE_MT_SYNCH,
                                      Common_TimePolicy_t,
                                      libacestream_default_net_stream_name_string,
                                      enum Stream_ControlType,
                                      enum Stream_SessionMessageType,
                                      enum Stream_StateMachine_ControlState,
                                      struct ARDrone_StreamState,
                                      struct ARDrone_StreamConfiguration,
                                      struct Stream_Statistic,
                                      Common_Timer_Manager_t,
                                      struct ARDrone_ModuleHandlerConfiguration,
                                      struct ARDrone_SessionData,
                                      ARDrone_SessionData_t,
                                      Stream_ControlMessage_t,
                                      ARDrone_Message,
                                      ARDrone_SessionMessage,
                                      ACE_INET_Addr,
                                      ARDrone_TCPConnectionManager_t,
                                      struct Stream_UserData> ARDrone_NetStream_t;

typedef Net_IStreamConnection_T<ACE_INET_Addr,
                                 ARDrone_TCPConnectionConfiguration_t,
                                 struct ARDrone_ConnectionState,
                                 struct Net_StreamStatistic,
                                 Net_TCPSocketConfiguration_t,
                                 ARDrone_NetStream_t,
                                 enum Stream_StateMachine_ControlState> ARDrone_ITCPStreamConnection_t;
typedef Net_IStreamConnection_T<ACE_INET_Addr,
                                ARDrone_UDPConnectionConfiguration_t,
                                struct ARDrone_ConnectionState,
                                struct Net_StreamStatistic,
                                Net_UDPSocketConfiguration_t,
                                ARDrone_NetStream_t,
                                enum Stream_StateMachine_ControlState> ARDrone_IUDPStreamConnection_t;
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Net_IConnector_T<ACE_INET_Addr,
                         ARDrone_DirectShow_TCPConnectionConfiguration_t> ARDrone_DirectShow_ITCPConnector_t;
typedef Net_IAsynchConnector_T<ACE_INET_Addr,
                               ARDrone_DirectShow_TCPConnectionConfiguration_t> ARDrone_DirectShow_IAsynchTCPConnector_t;

typedef Net_IConnector_T<ACE_INET_Addr,
                         ARDrone_MediaFoundation_TCPConnectionConfiguration_t> ARDrone_MediaFoundation_ITCPConnector_t;
typedef Net_IAsynchConnector_T<ACE_INET_Addr,
                               ARDrone_MediaFoundation_TCPConnectionConfiguration_t> ARDrone_MediaFoundation_IAsynchTCPConnector_t;
#else
typedef Net_IConnector_T<ACE_INET_Addr,
                         ARDrone_TCPConnectionConfiguration_t> ARDrone_ITCPConnector_t;
typedef Net_IAsynchConnector_T<ACE_INET_Addr,
                               ARDrone_TCPConnectionConfiguration_t> ARDrone_IAsynchTCPConnector_t;
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Net_Client_Connector_T<ACE_NULL_SYNCH,
                               Net_TCPConnectionBase_T<ACE_NULL_SYNCH,
                                                       Net_TCPSocketHandler_t,
                                                       ARDrone_DirectShow_TCPConnectionConfiguration_t,
                                                       struct ARDrone_ConnectionState,
                                                       struct Net_StreamStatistic,
                                                       ARDrone_DirectShow_TCPNetStream_t,
                                                       struct Net_UserData>,
                               ACE_SOCK_CONNECTOR,
                               ACE_INET_Addr,
                               ARDrone_DirectShow_TCPConnectionConfiguration_t,
                               struct ARDrone_ConnectionState,
                               struct Net_StreamStatistic,
                               Net_TCPSocketConfiguration_t,
                               ARDrone_DirectShow_TCPNetStream_t,
                               struct Net_UserData> ARDrone_DirectShow_TCPConnector_t;

//typedef Net_StreamConnectionBase_T<ACE_NULL_SYNCH,
//                                   Net_UDPSocketHandler_t,
//                                   ACE_INET_Addr,
//                                   ARDrone_DirectShow_UDPConnectionConfiguration_t,
//                                   struct ARDrone_ConnectionState,
//                                   struct Net_StreamStatistic,
//                                   Net_UDPSocketConfiguration_t,
//                                   ARDrone_DirectShow_UDPNetStream_t,
//                                   enum Stream_StateMachine_ControlState,
//                                   struct Net_UserData> ARDrone_DirectShow_IUDPStreamConnection_t;
typedef Net_IStreamConnection_T<ACE_INET_Addr,
                                ARDrone_DirectShow_UDPConnectionConfiguration_t,
                                struct ARDrone_ConnectionState,
                                struct Net_StreamStatistic,
                                Net_UDPSocketConfiguration_t,
                                ARDrone_DirectShow_UDPNetStream_t,
                                enum Stream_StateMachine_ControlState> ARDrone_DirectShow_IUDPStreamConnection_t;
typedef Net_Client_Connector_T<ACE_NULL_SYNCH,
                               Net_UDPConnectionBase_T<ACE_NULL_SYNCH,
                                                       Net_UDPSocketHandler_t,
                                                       ARDrone_DirectShow_UDPConnectionConfiguration_t,
                                                       struct ARDrone_ConnectionState,
                                                       struct Net_StreamStatistic,
                                                       ARDrone_DirectShow_UDPNetStream_t,
                                                       struct Net_UserData>,
                               ACE_SOCK_CONNECTOR,
                               ACE_INET_Addr,
                               ARDrone_DirectShow_UDPConnectionConfiguration_t,
                               struct ARDrone_ConnectionState,
                               struct Net_StreamStatistic,
                               Net_UDPSocketConfiguration_t,
                               ARDrone_DirectShow_UDPNetStream_t,
                               struct Net_UserData> ARDrone_DirectShow_UDPConnector_t;

typedef Net_Client_AsynchConnector_T<Net_AsynchTCPConnectionBase_T<Net_AsynchTCPSocketHandler_t,
                                                                   ARDrone_DirectShow_TCPConnectionConfiguration_t,
                                                                   struct ARDrone_ConnectionState,
                                                                   struct Net_StreamStatistic,
                                                                   ARDrone_DirectShow_TCPNetStream_t,
                                                                   struct Net_UserData>,
                                     ACE_INET_Addr,
                                     ARDrone_DirectShow_TCPConnectionConfiguration_t,
                                     struct ARDrone_ConnectionState,
                                     struct Net_StreamStatistic,
                                     Net_TCPSocketConfiguration_t,
                                     ARDrone_DirectShow_TCPNetStream_t,
                                     struct Net_UserData> ARDrone_DirectShow_AsynchTCPConnector_t;
typedef Net_Client_AsynchConnector_T<Net_AsynchUDPConnectionBase_T<Net_AsynchUDPSocketHandler_t,
                                                                   ARDrone_DirectShow_UDPConnectionConfiguration_t,
                                                                   struct ARDrone_ConnectionState,
                                                                   struct Net_StreamStatistic,
                                                                   ARDrone_DirectShow_UDPNetStream_t,
                                                                   struct Net_UserData>,
                                     ACE_INET_Addr,
                                     ARDrone_DirectShow_UDPConnectionConfiguration_t,
                                     struct ARDrone_ConnectionState,
                                     struct Net_StreamStatistic,
                                     Net_UDPSocketConfiguration_t,
                                     ARDrone_DirectShow_UDPNetStream_t,
                                     struct Net_UserData> ARDrone_DirectShow_AsynchUDPConnector_t;

typedef Net_Client_Connector_T<ACE_NULL_SYNCH,
                               Net_TCPConnectionBase_T<ACE_NULL_SYNCH,
                                                       Net_TCPSocketHandler_t,
                                                       ARDrone_MediaFoundation_TCPConnectionConfiguration_t,
                                                       struct ARDrone_ConnectionState,
                                                       struct Net_StreamStatistic,
                                                       ARDrone_MediaFoundation_NetStream_t,
                                                       struct Net_UserData>,
                               ACE_SOCK_CONNECTOR,
                               ACE_INET_Addr,
                               ARDrone_MediaFoundation_TCPConnectionConfiguration_t,
                               struct ARDrone_ConnectionState,
                               struct Net_StreamStatistic,
                               Net_TCPSocketConfiguration_t,
                               ARDrone_MediaFoundation_NetStream_t,
                               struct Net_UserData> ARDrone_MediaFoundation_TCPConnector_t;

typedef Net_StreamConnectionBase_T<ACE_NULL_SYNCH,
                                   Net_UDPSocketHandler_t,
                                   ACE_INET_Addr,
                                   ARDrone_MediaFoundation_UDPConnectionConfiguration_t,
                                   struct ARDrone_ConnectionState,
                                   struct Net_StreamStatistic,
                                   Net_UDPSocketConfiguration_t,
                                   ARDrone_MediaFoundation_NetStream_t,
                                   enum Stream_StateMachine_ControlState,
                                   struct Net_UserData> ARDrone_MediaFoundation_IUDPStreamConnection_t;
typedef Net_Client_Connector_T<ACE_NULL_SYNCH,
                               Net_UDPConnectionBase_T<ACE_NULL_SYNCH,
                                                       Net_UDPSocketHandler_t,
                                                       ARDrone_MediaFoundation_UDPConnectionConfiguration_t,
                                                       struct ARDrone_ConnectionState,
                                                       struct Net_StreamStatistic,
                                                       ARDrone_MediaFoundation_NetStream_t,
                                                       struct Net_UserData>,
                               ACE_SOCK_CONNECTOR,
                               ACE_INET_Addr,
                               ARDrone_MediaFoundation_UDPConnectionConfiguration_t,
                               struct ARDrone_ConnectionState,
                               struct Net_StreamStatistic,
                               Net_UDPSocketConfiguration_t,
                               ARDrone_MediaFoundation_NetStream_t,
                               struct Net_UserData> ARDrone_MediaFoundation_UDPConnector_t;

typedef Net_Client_AsynchConnector_T<Net_AsynchTCPConnectionBase_T<Net_AsynchTCPSocketHandler_t,
                                                                   ARDrone_MediaFoundation_TCPConnectionConfiguration_t,
                                                                   struct ARDrone_ConnectionState,
                                                                   struct Net_StreamStatistic,
                                                                   ARDrone_MediaFoundation_NetStream_t,
                                                                   struct Net_UserData>,
                                     ACE_INET_Addr,
                                     ARDrone_MediaFoundation_TCPConnectionConfiguration_t,
                                     struct ARDrone_ConnectionState,
                                     struct Net_StreamStatistic,
                                     Net_TCPSocketConfiguration_t,
                                     ARDrone_MediaFoundation_NetStream_t,
                                     struct Net_UserData> ARDrone_MediaFoundation_AsynchTCPConnector_t;
typedef Net_Client_AsynchConnector_T<Net_AsynchUDPConnectionBase_T<Net_AsynchUDPSocketHandler_t,
                                                                   ARDrone_MediaFoundation_UDPConnectionConfiguration_t,
                                                                   struct ARDrone_ConnectionState,
                                                                   struct Net_StreamStatistic,
                                                                   ARDrone_MediaFoundation_NetStream_t,
                                                                   struct Net_UserData>,
                                     ACE_INET_Addr,
                                     ARDrone_MediaFoundation_UDPConnectionConfiguration_t,
                                     struct ARDrone_ConnectionState,
                                     struct Net_StreamStatistic,
                                     Net_UDPSocketConfiguration_t,
                                     ARDrone_MediaFoundation_NetStream_t,
                                     struct Net_UserData> ARDrone_MediaFoundation_AsynchUDPConnector_t;
#else
typedef Net_Client_Connector_T<ACE_NULL_SYNCH,
                               Net_TCPConnectionBase_T<ACE_NULL_SYNCH,
                                                       Net_TCPSocketHandler_t,
                                                       ARDrone_TCPConnectionConfiguration_t,
                                                       struct ARDrone_ConnectionState,
                                                       struct Net_StreamStatistic,
                                                       ARDrone_NetStream_t,
                                                       struct Net_UserData>,
                               ACE_SOCK_CONNECTOR,
                               ACE_INET_Addr,
                               ARDrone_TCPConnectionConfiguration_t,
                               struct ARDrone_ConnectionState,
                               struct Net_StreamStatistic,
                               Net_TCPSocketConfiguration_t,
                               ARDrone_NetStream_t,
                               struct Net_UserData> ARDrone_TCPConnector_t;
typedef Net_Client_Connector_T<ACE_NULL_SYNCH,
                               Net_UDPConnectionBase_T<ACE_NULL_SYNCH,
                                                       Net_UDPSocketHandler_t,
                                                       ARDrone_UDPConnectionConfiguration_t,
                                                       struct ARDrone_ConnectionState,
                                                       struct Net_StreamStatistic,
                                                       ARDrone_NetStream_t,
                                                       struct Net_UserData>,
                               ACE_SOCK_CONNECTOR,
                               ACE_INET_Addr,
                               ARDrone_UDPConnectionConfiguration_t,
                               struct ARDrone_ConnectionState,
                               struct Net_StreamStatistic,
                               Net_UDPSocketConfiguration_t,
                               ARDrone_NetStream_t,
                               struct Net_UserData> ARDrone_UDPConnector_t;

typedef Net_Client_AsynchConnector_T<Net_AsynchTCPConnectionBase_T<Net_AsynchTCPSocketHandler_t,
                                                                   ARDrone_TCPConnectionConfiguration_t,
                                                                   struct ARDrone_ConnectionState,
                                                                   struct Net_StreamStatistic,
                                                                   ARDrone_NetStream_t,
                                                                   struct Net_UserData>,
                                     ACE_INET_Addr,
                                     ARDrone_TCPConnectionConfiguration_t,
                                     struct ARDrone_ConnectionState,
                                     struct Net_StreamStatistic,
                                     Net_TCPSocketConfiguration_t,
                                     ARDrone_NetStream_t,
                                     struct Net_UserData> ARDrone_AsynchTCPConnector_t;
typedef Net_Client_AsynchConnector_T<Net_AsynchUDPConnectionBase_T<Net_AsynchUDPSocketHandler_t,
                                                                   ARDrone_UDPConnectionConfiguration_t,
                                                                   struct ARDrone_ConnectionState,
                                                                   struct Net_StreamStatistic,
                                                                   ARDrone_NetStream_t,
                                                                   struct Net_UserData>,
                                     ACE_INET_Addr,
                                     ARDrone_UDPConnectionConfiguration_t,
                                     struct ARDrone_ConnectionState,
                                     struct Net_StreamStatistic,
                                     Net_UDPSocketConfiguration_t,
                                     ARDrone_NetStream_t,
                                     struct Net_UserData> ARDrone_AsynchUDPConnector_t;
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef ACE_Singleton<ARDrone_DirectShow_TCPConnectionManager_t,
                      ACE_SYNCH_MUTEX> ARDRONE_DIRECTSHOW_TCP_CONNECTIONMANAGER_SINGLETON;
typedef ACE_Singleton<ARDrone_DirectShow_UDPConnectionManager_t,
                      ACE_SYNCH_MUTEX> ARDRONE_DIRECTSHOW_UDP_CONNECTIONMANAGER_SINGLETON;

typedef ACE_Singleton<ARDrone_MediaFoundation_TCPConnectionManager_t,
                      ACE_SYNCH_MUTEX> ARDRONE_MEDIAFOUNDATION_TCP_CONNECTIONMANAGER_SINGLETON;
typedef ACE_Singleton<ARDrone_MediaFoundation_UDPConnectionManager_t,
                      ACE_SYNCH_MUTEX> ARDRONE_MEDIAFOUNDATION_UDP_CONNECTIONMANAGER_SINGLETON;

typedef Net_WLAN_WlanAPIMonitor_t ARDrone_WLANMonitor_t;
#else
typedef ACE_Singleton<ARDrone_TCPConnectionManager_t,
                      ACE_SYNCH_MUTEX> ARDRONE_TCP_CONNECTIONMANAGER_SINGLETON;
typedef ACE_Singleton<ARDrone_UDPConnectionManager_t,
                      ACE_SYNCH_MUTEX> ARDRONE_UDP_CONNECTIONMANAGER_SINGLETON;

typedef Net_WLAN_Nl80211Monitor_t ARDrone_WLANMonitor_t;
#endif // ACE_WIN32 || ACE_WIN64
typedef ACE_Singleton<ARDrone_WLANMonitor_t,
                      ACE_SYNCH_MUTEX> ARDRONE_WLANMONITOR_SINGLETON;

#endif // #ifndef ARDRONE_NETWORK_H
