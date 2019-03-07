﻿/***************************************************************************
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

#ifndef TEST_U_VIDEO_UI_NETWORK_COMMON_H
#define TEST_U_VIDEO_UI_NETWORK_COMMON_H

#include <string>
#include <unordered_map>

#include "ace/Global_Macros.h"
#include "ace/INET_Addr.h"
#include "ace/Synch_Traits.h"

#include "common_time_common.h"

#include "stream_common.h"
#include "stream_configuration.h"

#include "net_common.h"
#include "net_configuration.h"
#include "net_iconnectionmanager.h"

//#include "test_u_defines.h"
//#include "test_u_stream_common.h"
//#include "test_u_types.h"

// forward declarations
struct Test_U_Configuration;
struct Test_U_UserData;

struct Test_U_ConnectionState
 : Net_ConnectionState
{
  Test_U_ConnectionState ()
   : Net_ConnectionState ()
   , configuration (NULL)
  {}

  // *TODO*: consider making this a separate entity (i.e. a pointer)
  struct Test_U_Configuration* configuration;
};

struct Test_U_ConnectionConfiguration;
struct Test_U_SocketHandlerConfiguration
 : Net_SocketHandlerConfiguration
{
  Test_U_SocketHandlerConfiguration ()
   : Net_SocketHandlerConfiguration ()
   , socketConfiguration_2 ()
   , socketConfiguration_3 ()
   , connectionConfiguration (NULL)
  {
    socketConfiguration = &socketConfiguration_2;
  }

  struct Net_TCPSocketConfiguration      socketConfiguration_2;
  struct Net_UDPSocketConfiguration      socketConfiguration_3;
  struct Test_U_ConnectionConfiguration* connectionConfiguration;
};

//extern const char stream_name_string_[];
struct Test_U_StreamConfiguration;
struct Test_U_ModuleHandlerConfiguration;
typedef Stream_Configuration_T<//stream_name_string_,
                               struct Stream_AllocatorConfiguration,
                               struct Test_U_StreamConfiguration,
                               struct Stream_ModuleConfiguration,
                               struct Test_U_ModuleHandlerConfiguration> Test_U_StreamConfiguration_t;

struct Test_U_ConnectionConfiguration;
typedef Net_ConnectionConfiguration_T<struct Test_U_ConnectionConfiguration,
                                      struct Stream_AllocatorConfiguration,
                                      Test_U_StreamConfiguration_t> Test_U_ConnectionConfiguration_t;

typedef Net_IConnection_T<ACE_INET_Addr,
                          Test_U_ConnectionConfiguration_t,
                          struct Test_U_ConnectionState,
                          struct Test_U_StatisticData> Test_U_IConnection_t;

typedef Net_IConnectionManager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 Test_U_ConnectionConfiguration_t,
                                 struct Test_U_ConnectionState,
                                 struct Test_U_StatisticData,
                                 struct Net_UserData> Test_U_IConnectionManager_t;
struct Test_U_ConnectionConfiguration
 : Net_ConnectionConfiguration
{
  Test_U_ConnectionConfiguration ()
   : Net_ConnectionConfiguration ()
   , connectionManager (NULL)
   , socketHandlerConfiguration ()
  {
//    PDUSize = TEST_U_MESSAGE_BUFFER_SIZE;
  }

  Test_U_IConnectionManager_t*             connectionManager;
  struct Test_U_SocketHandlerConfiguration socketHandlerConfiguration;
};
typedef std::unordered_map<std::string, // module name
                           Test_U_ConnectionConfiguration_t> Test_U_Stream_ConnectionConfigurations_t;
typedef Test_U_Stream_ConnectionConfigurations_t::iterator Test_U_Stream_ConnectionConfigurationIterator_t;
typedef std::unordered_map<std::string, // stream name
                           Test_U_Stream_ConnectionConfigurations_t> Test_U_ConnectionConfigurations_t;
typedef Test_U_ConnectionConfigurations_t::iterator Test_U_ConnectionConfigurationIterator_t;

#endif // #ifndef Test_U_NETWORK_COMMON_H