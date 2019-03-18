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

#ifndef TEST_U_VIDEO_UI_NETWORK_H
#define TEST_U_VIDEO_UI_NETWORK_H

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

//#include "ardrone_configuration.h"
//#include "ardrone_defines.h"
//#include "ardrone_types.h"

#include "test_u_video_ui_network_common.h"
#include "test_u_video_ui_stream_common.h"

// forward declarations
//class Test_U_Message_t;
//class Test_U_SessionMessage_t;
//struct Net_UserData;

typedef Net_Connection_Manager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 Test_U_ConnectionConfiguration_t,
                                 struct Net_ConnectionState,
                                 struct Test_U_StatisticData,
                                 struct Net_UserData> Test_U_ConnectionManager_t;

typedef Stream_Module_Net_IO_Stream_T<ACE_MT_SYNCH,
                                      Common_TimePolicy_t,
                                      libacestream_default_net_stream_name_string,
                                      enum Stream_ControlType,
                                      enum Stream_SessionMessageType,
                                      enum Stream_StateMachine_ControlState,
                                      struct Test_U_StreamState,
                                      struct Test_U_StreamConfiguration,
                                      struct Test_U_StatisticData,
                                      Common_Timer_Manager_t,
                                      struct Stream_AllocatorConfiguration,
                                      struct Stream_ModuleConfiguration,
                                      struct Test_U_ModuleHandlerConfiguration,
                                      Test_U_SessionData,
                                      Test_U_SessionData_t,
                                      Stream_ControlMessage_t,
                                      Test_U_Message_t,
                                      Test_U_SessionMessage_t,
                                      ACE_INET_Addr,
                                      Test_U_ConnectionManager_t,
                                      struct Stream_UserData> Test_U_NetStream_t;

typedef Net_IStreamConnection_T<ACE_INET_Addr,
                                 Test_U_ConnectionConfiguration_t,
                                 struct Net_ConnectionState,
                                 struct Test_U_StatisticData,
                                 Net_TCPSocketConfiguration_t,
                                 Net_TCPSocketConfiguration_t,
                                 Test_U_NetStream_t,
                                 enum Stream_StateMachine_ControlState> Test_U_IStreamConnection_t;

typedef Net_TCPSocketHandler_T<ACE_NULL_SYNCH,
                               ACE_SOCK_STREAM,
                               Net_TCPSocketConfiguration_t> Test_U_TCPSocketHandler_t;
typedef Net_AsynchTCPSocketHandler_T<Net_TCPSocketConfiguration_t> Test_U_AsynchTCPSocketHandler_t;
typedef Net_UDPSocketHandler_T<ACE_NULL_SYNCH,
                               Net_SOCK_Dgram,
                               Net_UDPSocketConfiguration_t> Test_U_UDPSocketHandler_t;
typedef Net_AsynchUDPSocketHandler_T<Net_SOCK_Dgram,
                                     Net_UDPSocketConfiguration_t> Test_U_AsynchUDPSocketHandler_t;

typedef Net_Client_Connector_T<ACE_NULL_SYNCH,
                               Net_TCPConnectionBase_T<ACE_NULL_SYNCH,
                                                       Test_U_TCPSocketHandler_t,
                                                       Test_U_ConnectionConfiguration_t,
                                                       struct Net_ConnectionState,
                                                       struct Test_U_StatisticData,
                                                       Net_TCPSocketConfiguration_t,
                                                       Net_TCPListenerConfiguration_t,
                                                       Test_U_NetStream_t,
                                                       Common_Timer_Manager_t,
                                                       struct Net_UserData>,
                               ACE_SOCK_CONNECTOR,
                               ACE_INET_Addr,
                               Test_U_ConnectionConfiguration_t,
                               struct Net_ConnectionState,
                               struct Test_U_StatisticData,
                               Net_TCPSocketConfiguration_t,
                               Net_TCPSocketConfiguration_t,
                               Test_U_NetStream_t,
                               struct Net_UserData> Test_U_TCPConnector_t;
typedef Net_Client_Connector_T<ACE_NULL_SYNCH,
                               Net_UDPConnectionBase_T<ACE_NULL_SYNCH,
                                                       Test_U_UDPSocketHandler_t,
                                                       Test_U_ConnectionConfiguration_t,
                                                       struct Net_ConnectionState,
                                                       struct Test_U_StatisticData,
                                                       Net_UDPSocketConfiguration_t,
                                                       Test_U_NetStream_t,
                                                       Common_Timer_Manager_t,
                                                       struct Net_UserData>,
                               ACE_SOCK_CONNECTOR,
                               ACE_INET_Addr,
                               Test_U_ConnectionConfiguration_t,
                               struct Net_ConnectionState,
                               struct Test_U_StatisticData,
                               Net_UDPSocketConfiguration_t,
                               Net_UDPSocketConfiguration_t,
                               Test_U_NetStream_t,
                               struct Net_UserData> Test_U_UDPConnector_t;

typedef Net_Client_AsynchConnector_T<Net_AsynchTCPConnectionBase_T<Test_U_AsynchTCPSocketHandler_t,
                                                                   Test_U_ConnectionConfiguration_t,
                                                                   struct Net_ConnectionState,
                                                                   struct Test_U_StatisticData,
                                                                   Net_TCPSocketConfiguration_t,
                                                                   Net_TCPListenerConfiguration_t,
                                                                   Test_U_NetStream_t,
                                                                   Common_Timer_Manager_t,
                                                                   struct Net_UserData>,
                                     ACE_INET_Addr,
                                     Test_U_ConnectionConfiguration_t,
                                     struct Net_ConnectionState,
                                     struct Test_U_StatisticData,
                                     Net_TCPSocketConfiguration_t,
                                     Net_TCPSocketConfiguration_t,
                                     Test_U_NetStream_t,
                                     struct Net_UserData> Test_U_AsynchTCPConnector_t;
typedef Net_Client_AsynchConnector_T<Net_AsynchUDPConnectionBase_T<Test_U_AsynchUDPSocketHandler_t,
                                                                   Test_U_ConnectionConfiguration_t,
                                                                   struct Net_ConnectionState,
                                                                   struct Test_U_StatisticData,
                                                                   Net_UDPSocketConfiguration_t,
                                                                   Test_U_NetStream_t,
                                                                   Common_Timer_Manager_t,
                                                                   struct Net_UserData>,
                                     ACE_INET_Addr,
                                     Test_U_ConnectionConfiguration_t,
                                     struct Net_ConnectionState,
                                     struct Test_U_StatisticData,
                                     Net_UDPSocketConfiguration_t,
                                     Net_UDPSocketConfiguration_t,
                                     Test_U_NetStream_t,
                                     struct Net_UserData> Test_U_AsynchUDPConnector_t;

typedef ACE_Singleton<Test_U_ConnectionManager_t,
                      ACE_SYNCH_MUTEX> TEST_U_CONNECTIONMANAGER_SINGLETON;

#endif // #ifndef Test_U_NETWORK_H
