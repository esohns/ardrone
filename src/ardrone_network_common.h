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

#ifndef ARDRONE_NETWORK_COMMON_H
#define ARDRONE_NETWORK_COMMON_H

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

#include "net_wlan_common.h"
#include "net_wlan_inetmonitor.h"

#include "ardrone_defines.h"
#include "ardrone_stream_common.h"
#include "ardrone_types.h"

// forward declarations
struct ARDrone_Configuration;
struct ARDrone_UserData;

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

#if defined (ACE_WIN32) || defined (ACE_WIN64)
struct ARDrone_DirectShow_ConnectionConfiguration;
struct ARDrone_DirectShow_SocketHandlerConfiguration
 : Net_SocketHandlerConfiguration
{
  ARDrone_DirectShow_SocketHandlerConfiguration ()
   : Net_SocketHandlerConfiguration ()
   , socketConfiguration_2 ()
   , socketConfiguration_3 ()
   , connectionConfiguration (NULL)
   , userData (NULL)
  {
    socketConfiguration = &socketConfiguration_2;
  }

  struct Net_TCPSocketConfiguration                  socketConfiguration_2;
  struct Net_UDPSocketConfiguration                  socketConfiguration_3;
  struct ARDrone_DirectShow_ConnectionConfiguration* connectionConfiguration;

  struct ARDrone_UserData*                           userData;
};

struct ARDrone_MediaFoundation_ConnectionConfiguration;
struct ARDrone_MediaFoundation_SocketHandlerConfiguration
 : Net_SocketHandlerConfiguration
{
  ARDrone_MediaFoundation_SocketHandlerConfiguration ()
   : Net_SocketHandlerConfiguration ()
   , socketConfiguration_2 ()
   , socketConfiguration_3 ()
   , connectionConfiguration (NULL)
   , userData (NULL)
  {
    socketConfiguration = &socketConfiguration_2;
  }

  struct Net_TCPSocketConfiguration                       socketConfiguration_2;
  struct Net_UDPSocketConfiguration                       socketConfiguration_3;
  struct ARDrone_MediaFoundation_ConnectionConfiguration* connectionConfiguration;

  struct ARDrone_UserData*                                userData;
};
#else
struct ARDrone_ConnectionConfiguration;
struct ARDrone_SocketHandlerConfiguration
 : Net_SocketHandlerConfiguration
{
  ARDrone_SocketHandlerConfiguration ()
   : Net_SocketHandlerConfiguration ()
   , socketConfiguration_2 ()
   , socketConfiguration_3 ()
   , connectionConfiguration (NULL)
   , userData (NULL)
  {
    socketConfiguration = &socketConfiguration_2;
  }

  struct Net_TCPSocketConfiguration       socketConfiguration_2;
  struct Net_UDPSocketConfiguration       socketConfiguration_3;
  struct ARDrone_ConnectionConfiguration* connectionConfiguration;

  struct ARDrone_UserData*                userData;
};
#endif // ACE_WIN32 || ACE_WIN64

//extern const char stream_name_string_[];
#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Stream_Configuration_T<//stream_name_string_,
                               struct ARDrone_AllocatorConfiguration,
                               struct ARDrone_StreamConfiguration,
                               struct Stream_ModuleConfiguration,
                               struct ARDrone_DirectShow_ModuleHandlerConfiguration> ARDrone_DirectShow_StreamConfiguration_t;
typedef Stream_Configuration_T<//stream_name_string_,
                               struct ARDrone_AllocatorConfiguration,
                               struct ARDrone_StreamConfiguration,
                               struct Stream_ModuleConfiguration,
                               struct ARDrone_MediaFoundation_ModuleHandlerConfiguration> ARDrone_MediaFoundation_StreamConfiguration_t;
#else
struct ARDrone_AllocatorConfiguration;
struct ARDrone_StreamConfiguration;
struct ARDrone_ModuleHandlerConfiguration;
typedef Stream_Configuration_T<//stream_name_string_,
                               struct ARDrone_AllocatorConfiguration,
                               struct ARDrone_StreamConfiguration,
                               struct Stream_ModuleConfiguration,
                               struct ARDrone_ModuleHandlerConfiguration> ARDrone_StreamConfiguration_t;
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Net_ConnectionConfiguration_T<struct ARDrone_DirectShow_ConnectionConfiguration,
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

  ARDrone_DirectShow_IConnectionManager_t*             connectionManager;
  struct ARDrone_DirectShow_SocketHandlerConfiguration socketHandlerConfiguration;

  struct ARDrone_UserData*                             userData;
};

typedef Net_ConnectionConfiguration_T<struct ARDrone_MediaFoundation_ConnectionConfiguration,
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

  ARDrone_MediaFoundation_IConnectionManager_t*             connectionManager;
  struct ARDrone_MediaFoundation_SocketHandlerConfiguration socketHandlerConfiguration;

  struct ARDrone_UserData*                                  userData;
};

typedef std::unordered_map<std::string, // module name
                           ARDrone_DirectShow_ConnectionConfiguration_t> ARDrone_DirectShow_Stream_ConnectionConfigurations_t;
typedef ARDrone_DirectShow_Stream_ConnectionConfigurations_t::iterator ARDrone_DirectShow_Stream_ConnectionConfigurationIterator_t;
typedef std::unordered_map<std::string, // stream name
                           ARDrone_DirectShow_Stream_ConnectionConfigurations_t> ARDrone_DirectShow_ConnectionConfigurations_t;
typedef ARDrone_DirectShow_ConnectionConfigurations_t::iterator ARDrone_DirectShow_ConnectionConfigurationIterator_t;

typedef std::unordered_map<std::string, // module name
                           ARDrone_MediaFoundation_ConnectionConfiguration_t> ARDrone_MediaFoundation_Stream_ConnectionConfigurations_t;
typedef ARDrone_MediaFoundation_Stream_ConnectionConfigurations_t::iterator ARDrone_MediaFoundation_Stream_ConnectionConfigurationIterator_t;
typedef std::unordered_map<std::string, // stream name
                           ARDrone_MediaFoundation_Stream_ConnectionConfigurations_t> ARDrone_MediaFoundation_ConnectionConfigurations_t;
typedef ARDrone_MediaFoundation_ConnectionConfigurations_t::iterator ARDrone_MediaFoundation_ConnectionConfigurationIterator_t;
#else
typedef Net_ConnectionConfiguration_T<struct ARDrone_ConnectionConfiguration,
                                      struct ARDrone_AllocatorConfiguration,
                                      ARDrone_StreamConfiguration_t> ARDrone_ConnectionConfiguration_t;

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
typedef std::unordered_map<std::string, // module name
                           ARDrone_ConnectionConfiguration_t> ARDrone_Stream_ConnectionConfigurations_t;
typedef ARDrone_Stream_ConnectionConfigurations_t::iterator ARDrone_Stream_ConnectionConfigurationIterator_t;
typedef std::unordered_map<std::string, // stream name
                           ARDrone_Stream_ConnectionConfigurations_t> ARDrone_ConnectionConfigurations_t;
typedef ARDrone_ConnectionConfigurations_t::iterator ARDrone_ConnectionConfigurationIterator_t;
#endif // ACE_WIN32 || ACE_WIN64

typedef Net_WLAN_InetMonitor_T<struct Net_WLAN_MonitorConfiguration,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                               NET_WLAN_MONITOR_API_WLANAPI,
#else
                               ACE_MT_SYNCH,
                               Common_TimePolicy_t,
                               NET_WLAN_MONITOR_API_NL80211,
#endif // ACE_WIN32 || ACE_WIN64
                               struct Net_UserData> ARDrone_WLANMonitor_t;

#endif // #ifndef ARDRONE_NETWORK_COMMON_H
