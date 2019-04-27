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

#ifndef TEST_U_NETWORK_COMMON_H
#define TEST_U_NETWORK_COMMON_H

#include "ace/Global_Macros.h"
#include "ace/INET_Addr.h"
#include "ace/Singleton.h"
#include "ace/Synch_Traits.h"

//#include "common_time_common.h"

#include "stream_common.h"
#include "stream_configuration.h"

#include "net_common.h"
#include "net_configuration.h"
#include "net_iconnection.h"
#include "net_connection_manager.h"

//extern const char stream_name_string_[];
struct Test_U_StreamConfiguration;
struct Test_U_ModuleHandlerConfiguration;
typedef Stream_Configuration_T<//stream_name_string_,
                               struct Stream_AllocatorConfiguration,
                               struct Test_U_StreamConfiguration,
                               struct Stream_ModuleConfiguration,
                               struct Test_U_ModuleHandlerConfiguration> Test_U_StreamConfiguration_t;

typedef Net_ConnectionConfiguration_T<struct Stream_AllocatorConfiguration,
                                      Test_U_StreamConfiguration_t,
                                      NET_TRANSPORTLAYER_UDP> Test_U_UDPConnectionConfiguration_t;
typedef Net_ConnectionConfiguration_T<struct Stream_AllocatorConfiguration,
                                      Test_U_StreamConfiguration_t,
                                      NET_TRANSPORTLAYER_TCP> Test_U_TCPConnectionConfiguration_t;

typedef Net_IConnection_T<ACE_INET_Addr,
                          Test_U_UDPConnectionConfiguration_t,
                          struct Net_ConnectionState,
                          struct Test_U_StatisticData> Test_U_IUDPConnection_t;
typedef Net_IConnection_T<ACE_INET_Addr,
                          Test_U_TCPConnectionConfiguration_t,
                          struct Net_ConnectionState,
                          struct Test_U_StatisticData> Test_U_ITCPConnection_t;

//typedef Net_IConnectionManager_T<ACE_MT_SYNCH,
//                                 ACE_INET_Addr,
//                                 Test_U_ConnectionConfiguration_t,
//                                 struct Net_ConnectionState,
//                                 struct Test_U_StatisticData,
//                                 struct Net_UserData> Test_U_IConnectionManager_t;

typedef Net_Connection_Manager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 Test_U_TCPConnectionConfiguration_t,
                                 struct Net_ConnectionState,
                                 struct Test_U_StatisticData,
                                 struct Net_UserData> Test_U_TCPConnectionManager_t;
typedef Net_Connection_Manager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 Test_U_UDPConnectionConfiguration_t,
                                 struct Net_ConnectionState,
                                 struct Test_U_StatisticData,
                                 struct Net_UserData> Test_U_UDPConnectionManager_t;

typedef ACE_Singleton<Test_U_UDPConnectionManager_t,
                      ACE_SYNCH_MUTEX> TEST_U_UDP_CONNECTIONMANAGER_SINGLETON;
typedef ACE_Singleton<Test_U_TCPConnectionManager_t,
                      ACE_SYNCH_MUTEX> TEST_U_TCP_CONNECTIONMANAGER_SINGLETON;

#endif // #ifndef Test_U_NETWORK_COMMON_H
