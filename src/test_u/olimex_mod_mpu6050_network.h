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

#ifndef OLIMEX_MOD_MPU6050_NETWORK_H
#define OLIMEX_MOD_MPU6050_NETWORK_H

#include "ace/Global_Macros.h"
#include "ace/INET_Addr.h"
//#include "ace/Netlink_Addr.h"
#include "ace/Singleton.h"
#include "ace/Synch_Traits.h"
//#include "ace/SOCK_Dgram.h"
//#include "ace/SOCK_Netlink.h"

#include "stream_common.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#include "net_asynch_netlinksockethandler.h"
#endif
#include "net_asynch_udpsockethandler.h"
#include "net_common.h"
#include "net_connection_manager.h"
#include "net_itransportlayer.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#include "net_netlinkconnection.h"
#include "net_netlinksockethandler.h"
#endif
#include "net_stream_asynch_udpsocket_base.h"
#include "net_stream_udpsocket_base.h"
#include "net_udpconnection_base.h"
#include "net_udpsockethandler.h"

#include "net_client_asynchconnector.h"
#include "net_client_connector.h"
#include "net_client_iconnector.h"

#include "olimex_mod_mpu6050_types.h"
//#include "olimex_mod_mpu6050_stream.h"

// forward declarations
class Olimex_Mod_MPU6050_Stream;

typedef Net_StreamUDPSocketBase_T<Net_UDPSocketHandler_T<Net_SOCK_Dgram,
                                                         Net_SocketConfiguration>,
                                  ///////
                                  ACE_INET_Addr,
                                  Olimex_Mod_MPU6050_Configuration,
                                  Stream_State,
                                  Olimex_Mod_MPU6050_RuntimeStatistic_t,
                                  Olimex_Mod_MPU6050_Stream,
                                  ///////
                                  Olimex_Mod_MPU6050_UserData,
                                  ///////
                                  Stream_ModuleConfiguration,
                                  Stream_ModuleHandlerConfiguration> Olimex_Mod_MPU6050_UDPHandler_t;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
typedef Net_StreamUDPSocketBase_T<Net_NetlinkSocketHandler_T<Net_SOCK_Dgram,
                                                             Net_SocketConfiguration>,
                                  ///////
                                  ACE_INET_Addr,
                                  Olimex_Mod_MPU6050_Configuration,
                                  Stream_State,
                                  Olimex_Mod_MPU6050_RuntimeStatistic_t,
                                  Olimex_Mod_MPU6050_Stream,
                                  ///////
                                  Olimex_Mod_MPU6050_UserData,
                                  ///////
                                  Stream_ModuleConfiguration,
                                  Stream_ModuleHandlerConfiguration> Olimex_Mod_MPU6050_NetlinkHandler_t;
#endif
typedef Net_StreamAsynchUDPSocketBase_T<Net_AsynchUDPSocketHandler_T<Net_SocketConfiguration>,
                                        Net_SOCK_Dgram,

                                        ACE_INET_Addr,
                                        Olimex_Mod_MPU6050_Configuration,
                                        Stream_State,
                                        Olimex_Mod_MPU6050_RuntimeStatistic_t,
                                        Olimex_Mod_MPU6050_Stream,
                                        
                                        Olimex_Mod_MPU6050_UserData,
                                        
                                        Stream_ModuleConfiguration,
                                        Stream_ModuleHandlerConfiguration> Olimex_Mod_MPU6050_AsynchUDPHandler_t;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
typedef Net_StreamAsynchUDPSocketBase_T<Net_AsynchNetlinkSocketHandler,
                                        
                                        ACE_INET_Addr,
                                        Olimex_Mod_MPU6050_Configuration,
                                        Stream_State,
                                        Olimex_Mod_MPU6050_RuntimeStatistic_t,
                                        Olimex_Mod_MPU6050_Stream,
                                        
                                        Olimex_Mod_MPU6050_UserData,
                                        
                                        Stream_ModuleConfiguration,
                                        Stream_ModuleHandlerConfiguration> Olimex_Mod_MPU6050_AsynchNetlinkHandler_t;
#endif

typedef Net_Client_IConnector_T<ACE_INET_Addr,
                                Net_SocketHandlerConfiguration> Olimex_Mod_MPU6050_IConnector_t;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
typedef Net_Client_IConnector_T<ACE_Netlink_Addr,
                                Net_SocketHandlerConfiguration_t> Olimex_Mod_MPU6050_INetlinkConnector_t;
#endif

typedef Net_Client_Connector_T<Net_UDPConnectionBase_T<Olimex_Mod_MPU6050_UDPHandler_t,
                                                       
                                                       Olimex_Mod_MPU6050_Configuration,
                                                       Stream_State,
                                                       Olimex_Mod_MPU6050_RuntimeStatistic_t,
                                                       Olimex_Mod_MPU6050_Stream,
                                                       
                                                       Net_SocketHandlerConfiguration,
                                                       
                                                       Net_UserData>,
                               //////////
                               ACE_INET_Addr,
                               Olimex_Mod_MPU6050_Configuration,
                               Stream_State,
                               Olimex_Mod_MPU6050_RuntimeStatistic_t,
                               Olimex_Mod_MPU6050_Stream,
                               //////////
                               Net_SocketHandlerConfiguration,
                               //////////
                               Net_UserData> Olimex_Mod_MPU6050_Connector_t;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
typedef Net_Client_Connector_T<Net_NetlinkConnection_T<Olimex_Mod_MPU6050_UDPHandler_t,

                                                       Olimex_Mod_MPU6050_Configuration,
                                                       Stream_State,
                                                       Olimex_Mod_MPU6050_RuntimeStatistic_t,
                                                       Olimex_Mod_MPU6050_Stream,

                                                       Net_SocketHandlerConfiguration,

                                                       Net_UserData>,
                               //////////
                               ACE_INET_Addr,
                               Olimex_Mod_MPU6050_Configuration,
                               Stream_State,
                               Olimex_Mod_MPU6050_RuntimeStatistic_t,
                               Olimex_Mod_MPU6050_Stream,
                               //////////
                               Net_SocketHandlerConfiguration,
                               //////////
                               Net_UserData> > Olimex_Mod_MPU6050_NetlinkConnector_t;
#endif

typedef Net_Client_AsynchConnector_T<Net_AsynchUDPConnectionBase_T<Olimex_Mod_MPU6050_UDPHandler_t,

                                                                   Olimex_Mod_MPU6050_Configuration,
                                                                   Stream_State,
                                                                   Olimex_Mod_MPU6050_RuntimeStatistic_t,
                                                                   Olimex_Mod_MPU6050_Stream,

                                                                   Net_SocketHandlerConfiguration,

                                                                   Net_UserData>,
                                     ////
                                     ACE_INET_Addr,
                                     Olimex_Mod_MPU6050_Configuration,
                                     Stream_State,
                                     Olimex_Mod_MPU6050_RuntimeStatistic_t,
                                     ////
                                     Olimex_Mod_MPU6050_Stream,
                                     ////
                                     Net_SocketHandlerConfiguration,
                                     ////
                                     Net_UserData> Olimex_Mod_MPU6050_AsynchConnector_t;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
typedef Net_Client_AsynchConnector_T<Net_AsynchNetlinkConnection_T<Olimex_Mod_MPU6050_UDPHandler_t,

                                                                   Olimex_Mod_MPU6050_Configuration,
                                                                   Stream_State,
                                                                   Olimex_Mod_MPU6050_RuntimeStatistic_t,
                                                                   Olimex_Mod_MPU6050_Stream,

                                                                   Net_SocketHandlerConfiguration,

                                                                   Net_UserData>,
                                     ////
                                     ACE_INET_Addr,
                                     Olimex_Mod_MPU6050_Configuration,
                                     Stream_State,
                                     Olimex_Mod_MPU6050_RuntimeStatistic_t,
                                     ////
                                     Olimex_Mod_MPU6050_Stream,
                                     ////
                                     Net_SocketHandlerConfiguration,
                                     ////
                                     Net_UserData> Olimex_Mod_MPU6050_AsynchNetlinkConnector_t;
#endif

typedef Net_Connection_Manager_T<ACE_INET_Addr,
                                 Olimex_Mod_MPU6050_Configuration,
                                 Stream_State,
                                 Olimex_Mod_MPU6050_RuntimeStatistic_t,
                                 ////////
                                 Net_UserData> Olimex_Mod_MPU6050_ConnectionManager_t;
typedef ACE_Singleton<Olimex_Mod_MPU6050_ConnectionManager_t,
                      ACE_SYNCH_MUTEX> OLIMEX_MOD_MPU6050_CONNECTIONMANAGER_SINGLETON;

#endif // #ifndef OLIMEX_MOD_MPU6050_NETWORK_H
