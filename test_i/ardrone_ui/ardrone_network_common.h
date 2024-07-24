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

struct ARDrone_ConnectionConfiguration
 : Net_ConnectionConfiguration_T<NET_TRANSPORTLAYER_TCP>
{
  ARDrone_ConnectionConfiguration ()
   : Net_ConnectionConfiguration_T<NET_TRANSPORTLAYER_TCP> ()
   , socketConfiguration_2 ()
   , socketConfiguration_3 ()
   , userData (NULL)
  {
    socketConfiguration = socketConfiguration_2;
  }

  Net_SocketConfiguration_T<NET_TRANSPORTLAYER_TCP>  socketConfiguration_2;
  Net_SocketConfiguration_T<NET_TRANSPORTLAYER_UDP>  socketConfiguration_3;

  struct Net_UserData*                               userData;
};

//extern const char stream_name_string_[];
#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Stream_Configuration_T<//stream_name_string_,
                               struct ARDrone_StreamConfiguration,
                               struct ARDrone_DirectShow_ModuleHandlerConfiguration> ARDrone_DirectShow_StreamConfiguration_t;
typedef Stream_Configuration_T<//stream_name_string_,
                               struct ARDrone_StreamConfiguration,
                               struct ARDrone_MediaFoundation_ModuleHandlerConfiguration> ARDrone_MediaFoundation_StreamConfiguration_t;
#else
struct ARDrone_AllocatorConfiguration;
struct ARDrone_StreamConfiguration;
struct ARDrone_ModuleHandlerConfiguration;
typedef Stream_Configuration_T<//stream_name_string_,
                               struct ARDrone_StreamConfiguration,
                               struct ARDrone_ModuleHandlerConfiguration> ARDrone_StreamConfiguration_t;
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Net_StreamConnectionConfiguration_T<ARDrone_DirectShow_StreamConfiguration_t,
                                            NET_TRANSPORTLAYER_UDP> ARDrone_DirectShow_UDPConnectionConfiguration_t;
typedef Net_IConnectionManager_T<ACE_INET_Addr,
                                 ARDrone_DirectShow_UDPConnectionConfiguration_t,
                                 struct ARDrone_ConnectionState,
                                 struct ARDrone_Statistic,
                                 struct Net_UserData> ARDrone_DirectShow_IUDPConnectionManager_t;

typedef Net_StreamConnectionConfiguration_T<ARDrone_MediaFoundation_StreamConfiguration_t,
                                            NET_TRANSPORTLAYER_UDP> ARDrone_MediaFoundation_UDPConnectionConfiguration_t;
typedef Net_IConnectionManager_T<ACE_INET_Addr,
                                 ARDrone_MediaFoundation_UDPConnectionConfiguration_t,
                                 struct ARDrone_ConnectionState,
                                 struct ARDrone_Statistic,
                                 struct Net_UserData> ARDrone_MediaFoundation_IConnectionManager_t;

typedef std::unordered_map<std::string, // module name
                           ARDrone_DirectShow_UDPConnectionConfiguration_t> ARDrone_DirectShow_UDPConnectionConfigurations_t;
typedef ARDrone_DirectShow_UDPConnectionConfigurations_t::iterator ARDrone_DirectShow_UDPConnectionConfigurationIterator_t;
typedef std::unordered_map<std::string, // stream name
                           ARDrone_DirectShow_UDPConnectionConfigurations_t> ARDrone_DirectShow_ConnectionConfigurations_t;
typedef ARDrone_DirectShow_ConnectionConfigurations_t::iterator ARDrone_DirectShow_ConnectionConfigurationIterator_t;

typedef std::unordered_map<std::string, // module name
                           ARDrone_MediaFoundation_UDPConnectionConfiguration_t> ARDrone_MediaFoundation_UDPConnectionConfigurations_t;
typedef ARDrone_MediaFoundation_UDPConnectionConfigurations_t::iterator ARDrone_MediaFoundation_UDPConnectionConfigurationIterator_t;
typedef std::unordered_map<std::string, // stream name
                           ARDrone_MediaFoundation_UDPConnectionConfigurations_t> ARDrone_MediaFoundation_ConnectionConfigurations_t;
typedef ARDrone_MediaFoundation_ConnectionConfigurations_t::iterator ARDrone_MediaFoundation_ConnectionConfigurationIterator_t;
#else
typedef Net_StreamConnectionConfiguration_T<ARDrone_StreamConfiguration_t,
                                            NET_TRANSPORTLAYER_UDP> ARDrone_UDPConnectionConfiguration_t;
typedef std::unordered_map<std::string, // stream name
                           Net_UDPConnectionConfigurations_t> ARDrone_ConnectionConfigurations_t;
typedef ARDrone_ConnectionConfigurations_t::iterator ARDrone_ConnectionConfigurationIterator_t;
#endif // ACE_WIN32 || ACE_WIN64

#endif // #ifndef ARDRONE_NETWORK_COMMON_H
