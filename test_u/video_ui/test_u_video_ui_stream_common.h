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

#ifndef TEST_U_VIDEO_UI_STREAM_COMMON_H
#define TEST_U_VIDEO_UI_STREAM_COMMON_H

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <control.h>
#endif // ACE_WIN32 || ACE_WIN64

#include "ace/Synch_Traits.h"

#include "common_isubscribe.h"
#include "common_statistic_handler.h"
#include "common_tools.h"

#include "common_ui_common.h"

#include "stream_common.h"
#include "stream_configuration.h"
#include "stream_control_message.h"
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
#include "stream_lib_ffmpeg_common.h"

#include "stream_dev_common.h"
#include "stream_dev_defines.h"

#include "stream_vis_common.h"
#include "stream_vis_defines.h"

#include "net_connection_manager.h"

//#include "test_u_video_ui_network.h"
#include "test_u_video_ui_network_common.h"

// forward declarations
class Stream_IAllocator;
template <typename NotificationType,
          typename DataMessageType,
          typename SessionMessageType>
class Test_U_EventHandler_T;

struct Test_U_MessageData
{
  Test_U_MessageData ()
   : release (false)
  {}

  bool release;
};

struct Test_U_StatisticData
 : Stream_Statistic
{
  Test_U_StatisticData ()
   : Stream_Statistic ()
   , capturedFrames (0)
  {}

  struct Test_U_StatisticData operator+= (const struct Test_U_StatisticData& rhs_in)
  {
    Stream_Statistic::operator+= (rhs_in);
    capturedFrames += rhs_in.capturedFrames;

    return *this;
  }

  unsigned int capturedFrames;
};
typedef Common_StatisticHandler_T<struct Test_U_StatisticData> Test_U_StatisticHandler_t;

struct Test_U_StreamState;
class Test_U_SessionData
 : public Stream_SessionDataMediaBase_T<struct Stream_SessionData,
                                        struct Stream_MediaFramework_FFMPEG_MediaType,
                                        struct Test_U_StreamState,
                                        struct Test_U_StatisticData,
                                        struct Stream_UserData>
{
 public:
  Test_U_SessionData ()
   : Stream_SessionDataMediaBase_T<struct Stream_SessionData,
                                   struct Stream_MediaFramework_FFMPEG_MediaType,
                                   struct Test_U_StreamState,
                                   struct Test_U_StatisticData,
                                   struct Stream_UserData> ()
  {}

//  Test_U_SessionData& operator+= (const Test_U_SessionData& rhs_in)
//  {
//    // *NOTE*: the idea is to 'merge' the data
//    Stream_SessionDataMediaBase_T<struct Test_U_SessionData,
//                                  struct Stream_MediaFramework_FFMPEG_MediaType,
//                                  struct Test_U_StreamState,
//                                  struct Test_U_StatisticData,
//                                  struct Stream_UserData>::operator+= (rhs_in);

//    return *this;
//  }

 private:
//  ACE_UNIMPLEMENTED_FUNC (Test_U_SessionData (const Test_U_SessionData&))
  ACE_UNIMPLEMENTED_FUNC (Test_U_SessionData& operator= (const Test_U_SessionData&))
};
typedef Stream_SessionData_T<Test_U_SessionData> Test_U_SessionData_t;

template <typename DataType,
          typename SessionDataType>
class Test_U_Message_T;
template <typename DataMessageType,
          typename SessionDataType>
class Test_U_SessionMessage_T;
typedef Test_U_Message_T<struct Test_U_MessageData,
                         Test_U_SessionData_t> Test_U_Message_t;
typedef Test_U_SessionMessage_T<Test_U_Message_t,
                                Test_U_SessionData_t> Test_U_SessionMessage_t;

typedef Stream_ISessionDataNotify_T<Stream_SessionId_t,
                                    Test_U_SessionData,
                                    enum Stream_SessionMessageType,
                                    Test_U_Message_t,
                                    Test_U_SessionMessage_t> Test_U_ISessionNotify_t;
typedef std::list<Test_U_ISessionNotify_t*> Test_U_Subscribers_t;
typedef Test_U_Subscribers_t::iterator Test_U_SubscribersIterator_t;
typedef Net_Connection_Manager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 Test_U_ConnectionConfiguration_t,
                                 struct Test_U_ConnectionState,
                                 struct Test_U_StatisticData,
                                 struct Net_UserData> Test_U_ConnectionManager_t;
struct Test_U_ModuleHandlerConfiguration
 : Stream_ModuleHandlerConfiguration
{
  Test_U_ModuleHandlerConfiguration ()
   : Stream_ModuleHandlerConfiguration ()
#if defined (ACE_WIN32) || defined (ACE_WIN64)
   , builder (NULL)
#endif // ACE_WIN32 || ACE_WIN64
   , codecFormat (AV_PIX_FMT_NONE)
   , codecId (AV_CODEC_ID_H264)
   , connection (NULL)
   , connectionConfigurations (NULL)
   , connectionManager (NULL)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
   , deviceIdentifier ()
   , direct3DConfiguration (NULL)
   , direct3DDevice (NULL)
   , filterConfiguration (NULL)
   , filterCLSID (GUID_NULL)
   , push (false)
   , windowController (NULL)
   , windowController2 (NULL)
#endif // ACE_WIN32 || ACE_WIN64
   , display ()
   , display_2 ()
   , fullScreen (false)
   , outputFormat ()
   , subscriber (NULL)
   , subscribers (NULL)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
   , window (NULL)
#else
   , window (0)
#endif // ACE_WIN32 || ACE_WIN64
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    concurrency = STREAM_HEADMODULECONCURRENCY_CONCURRENT;
#else
    concurrency = STREAM_HEADMODULECONCURRENCY_ACTIVE;
#endif // ACE_WIN32 || ACE_WIN64
    hasHeader = true;
  }

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  IGraphBuilder*                                builder;
#endif // ACE_WIN32 || ACE_WIN64
  enum AVPixelFormat                            codecFormat; // preferred output-
  enum AVCodecID                                codecId;
  Test_U_IConnection_t*                         connection;
  Test_U_Stream_ConnectionConfigurations_t*     connectionConfigurations;
  Test_U_ConnectionManager_t*                   connectionManager;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct Stream_Device_Identifier               deviceIdentifier; // target module
  struct Stream_MediaFramework_Direct3D_Configuration* direct3DConfiguration;
  IDirect3DDevice9*                             direct3DDevice;
  struct Test_U_DirectShow_FilterConfiguration* filterConfiguration;
  CLSID                                         filterCLSID;
  bool                                          push;
  IVideoWindow*                                 windowController;
  IMFVideoDisplayControl*                       windowController2;
#endif // ACE_WIN32 || ACE_WIN64
  struct Common_UI_DisplayDevice                display; // display module
  struct Common_UI_Display                      display_2; // display module
  bool                                          fullScreen;
  struct Stream_MediaFramework_FFMPEG_MediaType outputFormat;
  Test_U_ISessionNotify_t*                      subscriber;
  Test_U_Subscribers_t*                         subscribers;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  HWND                                          window;
#else
  XID                                           window;
#endif // ACE_WIN32 || ACE_WIN64
};
//extern const char stream_name_string_[];
struct Test_U_StreamConfiguration;
struct Test_U_ModuleHandlerConfiguration;
typedef Stream_Configuration_T<//stream_name_string_,
                               struct Stream_AllocatorConfiguration,
                               struct Test_U_StreamConfiguration,
                               struct Stream_ModuleConfiguration,
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
   , format ()
   , renderer (STREAM_VIS_RENDERER_VIDEO_DEFAULT)
  {
    printFinalReport = true;
  }

  struct Stream_MediaFramework_FFMPEG_MediaType format; // session data-
  enum Stream_Visualization_VideoRenderer       renderer;
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
