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

#ifndef TEST_U_STREAM_COMMON_H
#define TEST_U_STREAM_COMMON_H

#include "ace/Synch_Traits.h"

#include "mavlink/v2.0/mavlink_types.h"

#include "common_iinitialize.h"
#include "common_isubscribe.h"
#include "common_statistic_handler.h"
#include "common_tools.h"

#include "common_ui_common.h"

#include "stream_common.h"
#include "stream_configuration.h"
#include "stream_control_message.h"
#include "stream_data_base.h"
#include "stream_inotify.h"
#include "stream_isessionnotify.h"
#include "stream_istreamcontrol.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "stream_messageallocatorheap_base.h"
#else
#include "stream_messageallocatorheap_base.h"
#endif // ACE_WIN32 || ACE_WIN64
#include "stream_session_data.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "stream_lib_directdraw_common.h"
#include "stream_lib_directshow_tools.h"
#endif // ACE_WIN32 || ACE_WIN64

#include "stream_dev_common.h"
#include "stream_dev_defines.h"

#include "stream_lib_ffmpeg_common.h"

#include "stream_vis_common.h"
#include "stream_vis_defines.h"

#include "net_connection_manager.h"

#include "ardrone_common.h"
#include "ardrone_types.h"

//#include "test_u_network.h"
#include "test_u_network_common.h"

// forward declarations
class Stream_IAllocator;
template <typename NotificationType,
          typename DataMessageType,
          typename SessionMessageType>
class Test_U_EventHandler_T;

struct Test_U_MessageData
{
  Test_U_MessageData ()
   : messageType (ARDRONE_MESSAGE_INVALID)
   , controlData ()
   , NavData ()
  {}
  inline void operator+= (struct Test_U_MessageData rhs_in) { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }

  enum ARDrone_MessageType      messageType;

  ARDrone_DeviceConfiguration_t controlData;
  struct ARDrone_NavData        NavData;
};
typedef Stream_DataBase_T<struct Test_U_MessageData> Test_U_MessageData_t;

//struct Stream_Statistic
// : Stream_Statistic
//{
//  Stream_Statistic ()
//   : Stream_Statistic ()
//  {}
//
//  struct Stream_Statistic operator+= (const struct Stream_Statistic& rhs_in)
//  {
//    Stream_Statistic::operator+= (rhs_in);
//
//    return *this;
//  }
//};
//typedef Common_StatisticHandler_T<struct Stream_Statistic> Test_U_StatisticHandler_t;

struct Test_U_StreamState;
class Test_U_SessionData
 : public Stream_SessionDataMediaBase_T<struct Stream_SessionData,
                                        struct Stream_MediaFramework_FFMPEG_VideoMediaType,
                                        struct Test_U_StreamState,
                                        struct Stream_Statistic,
                                        struct Stream_UserData>
{
 public:
  Test_U_SessionData ()
   : Stream_SessionDataMediaBase_T<struct Stream_SessionData,
                                   struct Stream_MediaFramework_FFMPEG_VideoMediaType,
                                   struct Test_U_StreamState,
                                   struct Stream_Statistic,
                                   struct Stream_UserData> ()
   , connection (NULL)
   , connectionStates ()
  {}

  Test_U_SessionData& operator= (const Test_U_SessionData& rhs_in)
  {
    Stream_SessionDataMediaBase_T<struct Stream_SessionData,
                                  struct Stream_MediaFramework_FFMPEG_VideoMediaType,
                                  struct Test_U_StreamState,
                                  struct Stream_Statistic,
                                  struct Stream_UserData>::operator= (rhs_in);

    connection = rhs_in.connection;
    connectionStates = rhs_in.connectionStates;

    return *this;
  }

  Test_U_SessionData& operator+= (const Test_U_SessionData& rhs_in)
  {
    // *NOTE*: the idea is to 'merge' the data
    Stream_SessionDataMediaBase_T<struct Stream_SessionData,
                                  struct Stream_MediaFramework_FFMPEG_VideoMediaType,
                                  struct Test_U_StreamState,
                                  struct Stream_Statistic,
                                  struct Stream_UserData>::operator+= (rhs_in);

    connection = rhs_in.connection;
    connectionStates = rhs_in.connectionStates;

    return *this;
  }

  Test_U_IConnection_t*         connection;
  Stream_Net_ConnectionStates_t connectionStates;

 private:
//  ACE_UNIMPLEMENTED_FUNC (Test_U_SessionData (const Test_U_SessionData&))
//  ACE_UNIMPLEMENTED_FUNC (Test_U_SessionData& operator= (const Test_U_SessionData&))
};
typedef Stream_SessionData_T<Test_U_SessionData> Test_U_SessionData_t;

template <typename DataType,
          typename SessionDataType>
class Test_U_Message_T;
template <typename DataMessageType,
          typename SessionDataType>
class Test_U_SessionMessage_T;
typedef Test_U_Message_T<Test_U_MessageData_t,
                         Test_U_SessionData_t> Test_U_Message_t;
typedef Test_U_SessionMessage_T<Test_U_Message_t,
                                Test_U_SessionData_t> Test_U_SessionMessage_t;

typedef Stream_ISessionDataNotify_T<Test_U_SessionData,
                                    enum Stream_SessionMessageType,
                                    Test_U_Message_t,
                                    Test_U_SessionMessage_t> Test_U_ISessionNotify_t;
typedef std::list<Test_U_ISessionNotify_t*> Test_U_Subscribers_t;
typedef Test_U_Subscribers_t::iterator Test_U_SubscribersIterator_t;
//typedef Net_Connection_Manager_T<ACE_MT_SYNCH,
//                                 ACE_INET_Addr,
//                                 Test_U_ConnectionConfiguration_t,
//                                 struct Net_StreamConnectionState,
//                                 Net_StreamStatistic_t,
//                                 struct Net_UserData> Test_U_ConnectionManager_t;
struct Test_U_ModuleHandlerConfiguration
 : Stream_ModuleHandlerConfiguration
{
  Test_U_ModuleHandlerConfiguration ()
   : Stream_ModuleHandlerConfiguration ()
   , CBData (NULL)
   , connection (NULL)
   , connectionConfigurations (NULL)
   , deviceConfiguration (NULL)
   , parserConfiguration (NULL)
   , subscriber (NULL)
   , subscribers (NULL)
  {}

  struct Test_U_UI_CBData*                    CBData;
  Test_U_IConnection_t*                       connection;
  Net_ConnectionConfigurations_t*             connectionConfigurations;
  ARDrone_IDeviceConfiguration*               deviceConfiguration;
  struct Common_FlexBisonParserConfiguration* parserConfiguration; // parser module(s)
  Test_U_ISessionNotify_t*                    subscriber;
  Test_U_Subscribers_t*                       subscribers;
};
//extern const char stream_name_string_[];
struct Test_U_StreamConfiguration;
struct Test_U_ModuleHandlerConfiguration;
typedef Stream_Configuration_T<//stream_name_string_,
                               struct Test_U_StreamConfiguration,
                               struct Test_U_ModuleHandlerConfiguration> Test_U_StreamConfiguration_t;

struct Test_U_StreamState
 : Stream_State
{
  Test_U_StreamState ()
   : Stream_State ()
   , sessionData (NULL)
  {}

  Test_U_SessionData* sessionData;
};

struct Test_U_StreamConfiguration
 : Stream_Configuration
{
  Test_U_StreamConfiguration ()
   : Stream_Configuration ()
   , CBData (NULL)
   , deviceConfiguration (NULL)
   , initializeControl (NULL)
   , initializeNavData (NULL)
   , userData (NULL)
  {
    printFinalReport = true;
  }

  struct Test_U_UI_CBData*                       CBData;
  ARDrone_IDeviceConfiguration*                  deviceConfiguration;
  Common_IInitializeP_T<ARDrone_IControlNotify>* initializeControl;
  Common_IInitializeP_T<ARDrone_INavDataNotify>* initializeNavData;

  struct Stream_UserData*                        userData;
};
typedef Stream_IStreamControl_T<enum Stream_ControlType,
                                enum Stream_SessionMessageType,
                                enum Stream_StateMachine_ControlState,
                                struct Test_U_StreamState> Test_U_IStreamControl_t;

typedef Stream_MessageAllocatorHeapBase_T<ACE_MT_SYNCH,
                                          struct Stream_AllocatorConfiguration,
                                          Stream_ControlMessage_t,
                                          Test_U_Message_t,
                                          Test_U_SessionMessage_t> Test_U_MessageAllocator_t;

typedef Common_ISubscribe_T<Test_U_ISessionNotify_t> Test_U_ISubscribe_t;

typedef Test_U_EventHandler_T<Test_U_ISessionNotify_t,
                              Test_U_Message_t,
                              Test_U_SessionMessage_t> Test_U_EventHandler_t;

#endif
