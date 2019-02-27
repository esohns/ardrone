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
#else
#include "stream_lib_ffmpeg_common.h"
#endif // ACE_WIN32 || ACE_WIN64

#include "stream_dev_common.h"
#include "stream_dev_defines.h"

#include "stream_vis_common.h"
#include "stream_vis_defines.h"

#include "net_connection_manager.h"

//#include "test_u_video_ui_network.h"
#include "test_u_video_ui_network_common.h"

// forward declarations
#if defined (ACE_WIN32) || defined (ACE_WIN64)
struct ISampleGrabber;
#endif // ACE_WIN32 || ACE_WIN64
class Stream_IAllocator;
template <typename NotificationType,
          typename DataMessageType,
          typename SessionMessageType>
class Test_U_EventHandler_T;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
struct Test_U_DirectShow_MessageData
{
  Test_U_DirectShow_MessageData ()
   : sample (NULL)
   , sampleTime (0)
  {}

  IMediaSample* sample;
  double        sampleTime;
};
typedef Stream_DataBase_T<struct Test_U_DirectShow_MessageData> Test_U_DirectShow_MessageData_t;

struct Test_U_MediaFoundation_MessageData
{
  Test_U_MediaFoundation_MessageData ()
   : sample (NULL)
   , sampleTime (0)
  {}

  IMFSample* sample;
  LONGLONG   sampleTime;
};
typedef Stream_DataBase_T<struct Test_U_MediaFoundation_MessageData> Test_U_MediaFoundation_MessageData_t;
#else
struct Test_U_MessageData
{
  Test_U_MessageData ()
   : release (false)
  {}

  bool release;
};
#endif // ACE_WIN32 || ACE_WIN64

struct Test_U_StatisticData
 : Stream_Statistic
{
  Test_U_StatisticData ()
   : Stream_Statistic ()
#if defined (ACE_WIN32) || defined (ACE_WIN64)
   , capturedFrames (0)
#endif // ACE_WIN32 || ACE_WIN64
  {}

  struct Test_U_StatisticData operator+= (const struct Test_U_StatisticData& rhs_in)
  {
    Stream_Statistic::operator+= (rhs_in);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    capturedFrames += rhs_in.capturedFrames;
#endif // ACE_WIN32 || ACE_WIN64

    return *this;
  }

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  unsigned int capturedFrames;
#endif // ACE_WIN32 || ACE_WIN64
};
typedef Common_StatisticHandler_T<struct Test_U_StatisticData> Test_U_StatisticHandler_t;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
struct Test_U_DirectShow_StreamState;
class Test_U_DirectShow_SessionData
 : public Stream_SessionDataMediaBase_T<struct Test_U_SessionData,
                                        struct _AMMediaType,
                                        struct Test_U_DirectShow_StreamState,
                                        struct Test_U_StatisticData,
                                        struct Stream_UserData>
{
 public:
  Test_U_DirectShow_SessionData ()
   : Stream_SessionDataMediaBase_T<struct Test_U_SessionData,
                                   struct _AMMediaType,
                                   struct Test_U_DirectShow_StreamState,
                                   struct Test_U_StatisticData,
                                   struct Stream_UserData> ()
   , direct3DDevice (NULL)
   , direct3DManagerResetToken (0)
   , resetToken (0)
  {}

  Test_U_DirectShow_SessionData& operator+= (const Test_U_DirectShow_SessionData& rhs_in)
  {
    // *NOTE*: the idea is to 'merge' the data
    Stream_SessionDataMediaBase_T<struct Test_U_SessionData,
                                  struct _AMMediaType,
                                  struct Test_U_DirectShow_StreamState,
                                  struct Test_U_StatisticData,
                                  struct Stream_UserData>::operator+= (rhs_in);

    direct3DDevice = (direct3DDevice ? direct3DDevice : rhs_in.direct3DDevice);
    direct3DManagerResetToken =
      (direct3DManagerResetToken ? direct3DManagerResetToken
                                 : rhs_in.direct3DManagerResetToken);
    resetToken = (resetToken ? resetToken : rhs_in.resetToken);

    return *this;
  }

#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0600) // _WIN32_WINNT_VISTA
  IDirect3DDevice9Ex* direct3DDevice;
#else
  IDirect3DDevice9*   direct3DDevice;
#endif // COMMON_OS_WIN32_TARGET_PLATFORM(0x0600)
  UINT                direct3DManagerResetToken;
  UINT                resetToken;

 private:
  ACE_UNIMPLEMENTED_FUNC (Test_U_DirectShow_SessionData (const Test_U_DirectShow_SessionData&))
  ACE_UNIMPLEMENTED_FUNC (Test_U_DirectShow_SessionData& operator= (const Test_U_DirectShow_SessionData&))
};
typedef Stream_SessionData_T<Test_U_DirectShow_SessionData> Test_U_DirectShow_SessionData_t;

struct Test_U_MediaFoundation_StreamState;
class Test_U_MediaFoundation_SessionData
 : public Stream_SessionDataMediaBase_T<struct Test_U_SessionData,
                                        IMFMediaType*,
                                        struct Test_U_MediaFoundation_StreamState,
                                        struct Test_U_StatisticData,
                                        struct Stream_UserData>
{
 public:
  Test_U_MediaFoundation_SessionData ()
   : Stream_SessionDataMediaBase_T<struct Test_U_SessionData,
                                   IMFMediaType*,
                                   struct Test_U_MediaFoundation_StreamState,
                                   struct Test_U_StatisticData,
                                   struct Stream_UserData> ()
   , direct3DDevice (NULL)
   , direct3DManagerResetToken (0)
   , rendererNodeId (0)
   , resetToken (0)
   , session (NULL)
  {}

  Test_U_MediaFoundation_SessionData& operator+= (const Test_U_MediaFoundation_SessionData& rhs_in)
  {
    // *NOTE*: the idea is to 'merge' the data
    Stream_SessionDataMediaBase_T<struct Test_U_SessionData,
                                  IMFMediaType*,
                                  struct Test_U_MediaFoundation_StreamState,
                                  struct Test_U_StatisticData,
                                  struct Stream_UserData>::operator+= (rhs_in);

    direct3DDevice = (direct3DDevice ? direct3DDevice : rhs_in.direct3DDevice);
    direct3DManagerResetToken =
      (direct3DManagerResetToken ? direct3DManagerResetToken
                                 : rhs_in.direct3DManagerResetToken);
    rendererNodeId = (rendererNodeId ? rendererNodeId : rhs_in.rendererNodeId);
    resetToken = (resetToken ? resetToken : rhs_in.resetToken);
    session = (session ? session : rhs_in.session);

    return *this;
  }

#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0600) // _WIN32_WINNT_VISTA
  IDirect3DDevice9Ex*                 direct3DDevice;
#else
  IDirect3DDevice9*                   direct3DDevice;
#endif // COMMON_OS_WIN32_TARGET_PLATFORM(0x0600)
  UINT                                direct3DManagerResetToken;
  TOPOID                              rendererNodeId;
  UINT                                resetToken;
  IMFMediaSession*                    session;

 private:
  ACE_UNIMPLEMENTED_FUNC (Test_U_MediaFoundation_SessionData (const Test_U_MediaFoundation_SessionData&))
  ACE_UNIMPLEMENTED_FUNC (Test_U_MediaFoundation_SessionData& operator= (const Test_U_MediaFoundation_SessionData&))
};
typedef Stream_SessionData_T<Test_U_MediaFoundation_SessionData> Test_U_MediaFoundation_SessionData_t;
#else
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
#endif // ACE_WIN32 || ACE_WIN64

template <typename DataType,
          typename SessionDataType>
class Test_U_Message_T;
template <typename DataMessageType,
          typename SessionDataType>
class Test_U_SessionMessage_T;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Test_U_Message_T<struct Test_U_DirectShow_MessageData,
                                      Test_U_DirectShow_SessionData_t> Test_U_DirectShow_Message_t;
typedef Test_U_SessionMessage_T<Test_U_DirectShow_Message_t,
                                             Test_U_DirectShow_SessionData_t> Test_U_DirectShow_SessionMessage_t;
typedef Test_U_Message_T<struct Test_U_MediaFoundation_MessageData,
                                      Test_U_MediaFoundation_SessionData_t> Test_U_MediaFoundation_Message_t;
typedef Test_U_SessionMessage_T<Test_U_MediaFoundation_Message_t,
                                             Test_U_MediaFoundation_SessionData_t> Test_U_MediaFoundation_SessionMessage_t;

typedef Stream_ISessionDataNotify_T<Stream_SessionId_t,
                                    Test_U_DirectShow_SessionData,
                                    enum Stream_SessionMessageType,
                                    Test_U_DirectShow_Message_t,
                                    Test_U_DirectShow_SessionMessage_t> Test_U_DirectShow_ISessionNotify_t;
typedef std::list<Test_U_DirectShow_ISessionNotify_t*> Test_U_DirectShow_Subscribers_t;
typedef Test_U_DirectShow_Subscribers_t::iterator Test_U_DirectShow_SubscribersIterator_t;

typedef Stream_ISessionDataNotify_T<Stream_SessionId_t,
                                    Test_U_MediaFoundation_SessionData,
                                    enum Stream_SessionMessageType,
                                    Test_U_MediaFoundation_Message_t,
                                    Test_U_MediaFoundation_SessionMessage_t> Test_U_MediaFoundation_ISessionNotify_t;
typedef std::list<Test_U_MediaFoundation_ISessionNotify_t*> Test_U_MediaFoundation_Subscribers_t;
typedef Test_U_MediaFoundation_Subscribers_t::iterator Test_U_MediaFoundation_SubscribersIterator_t;
#else
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
#endif // ACE_WIN32 || ACE_WIN64
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
   , codecFormat (AV_PIX_FMT_NONE)
   , codecId (AV_CODEC_ID_H264)
   , connection (NULL)
   , connectionConfigurations (NULL)
   , connectionManager (NULL)
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

  enum AVPixelFormat                            codecFormat; // preferred output-
  enum AVCodecID                                codecId;
  Test_U_IConnection_t*                         connection;
  Test_U_Stream_ConnectionConfigurations_t*     connectionConfigurations;
  Test_U_ConnectionManager_t*                   connectionManager;
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
#if defined (ACE_WIN32) || defined (ACE_WIN64)
struct Test_U_DirectShow_StreamConfiguration;
struct Test_U_DirectShow_ModuleHandlerConfiguration;
typedef Stream_Configuration_T<//stream_name_string_,
                               struct Stream_AllocatorConfiguration,
                               struct Test_U_DirectShow_StreamConfiguration,
                               struct Stream_ModuleConfiguration,
                               struct Test_U_DirectShow_ModuleHandlerConfiguration> Test_U_DirectShow_StreamConfiguration_t;
struct Test_U_DirectShow_ModuleHandlerConfiguration
 : Test_U_ModuleHandlerConfiguration
{
  Test_U_DirectShow_ModuleHandlerConfiguration ()
   : Test_U_ModuleHandlerConfiguration ()
   , area ()
   , builder (NULL)
   , direct3DConfiguration (NULL)
   , filterConfiguration (NULL)
   , filterCLSID (GUID_NULL)
   , outputFormat ()
   , push (STREAM_LIB_DIRECTSHOW_FILTER_SOURCE_DEFAULT_PUSH)
   //, sourceFormat ()
   , subscriber (NULL)
   , subscribers (NULL)
   , windowController (NULL)
   , windowController2 (NULL)
  {
    mediaFramework = STREAM_MEDIAFRAMEWORK_DIRECTSHOW;
  }

  struct Test_U_DirectShow_ModuleHandlerConfiguration operator= (const struct Test_U_DirectShow_ModuleHandlerConfiguration& rhs_in)
  {
    area = rhs_in.area;
    if (builder)
    {
      builder->Release (); builder = NULL;
    } // end IF
    if (rhs_in.builder)
    {
      rhs_in.builder->AddRef ();
      builder = rhs_in.builder;
    } // end IF
    direct3DConfiguration = rhs_in.direct3DConfiguration;
    filterConfiguration = rhs_in.filterConfiguration;
    filterCLSID = rhs_in.filterCLSID;
//    if (outputFormat)
//      Stream_MediaFramework_DirectShow_Tools::delete_ (outputFormat);
    push = rhs_in.push;
    subscriber = rhs_in.subscriber;
    subscribers = rhs_in.subscribers;
    if (windowController)
    {
      windowController->Release (); windowController = NULL;
    } // end IF
    if (rhs_in.windowController)
    {
      rhs_in.windowController->AddRef ();
      windowController = rhs_in.windowController;
    } // end IF
    if (windowController2)
    {
      windowController2->Release (); windowController2 = NULL;
    } // end IF
    if (rhs_in.windowController2)
    {
      rhs_in.windowController2->AddRef ();
      windowController2 = rhs_in.windowController2;
    } // end IF

    return *this;
  }

  struct tagRECT                                        area;
  IGraphBuilder*                                        builder;
  struct Stream_MediaFramework_Direct3D_Configuration*  direct3DConfiguration;
  struct Test_U_DirectShow_FilterConfiguration* filterConfiguration;
  CLSID                                                 filterCLSID;
  struct _AMMediaType                                   outputFormat;
  bool                                                  push;
  //struct _AMMediaType                                   sourceFormat;
  Test_U_DirectShow_ISessionNotify_t*           subscriber;
  Test_U_DirectShow_Subscribers_t*              subscribers;
  IVideoWindow*                                         windowController;
  IMFVideoDisplayControl*                               windowController2; // EVR
};

struct Test_U_MediaFoundation_StreamConfiguration;
struct Test_U_MediaFoundation_ModuleHandlerConfiguration;
typedef Stream_Configuration_T<//stream_name_string_,
                               struct Stream_AllocatorConfiguration,
                               struct Test_U_MediaFoundation_StreamConfiguration,
                               struct Stream_ModuleConfiguration,
                               struct Test_U_MediaFoundation_ModuleHandlerConfiguration> Test_U_MediaFoundation_StreamConfiguration_t;
struct Test_U_MediaFoundation_ModuleHandlerConfiguration
 : Test_U_ModuleHandlerConfiguration
{
  Test_U_MediaFoundation_ModuleHandlerConfiguration ()
   : Test_U_ModuleHandlerConfiguration ()
   , area ()
   , direct3DConfiguration (NULL)
   , outputFormat (NULL)
   , rendererNodeId (0)
   , sampleGrabberNodeId (0)
   , session (NULL)
   //, sourceFormat (NULL)
   , subscriber (NULL)
   , subscribers (NULL)
   , windowController (NULL)
  {
    mediaFramework = STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION;
  }

  struct tagRECT                                       area;
  struct Stream_MediaFramework_Direct3D_Configuration* direct3DConfiguration;
  IMFMediaType*                                        outputFormat;
  TOPOID                                               rendererNodeId;
  TOPOID                                               sampleGrabberNodeId;
  IMFMediaSession*                                     session;
  //IMFMediaType*                                        sourceFormat;
  Test_U_MediaFoundation_ISessionNotify_t*     subscriber;
  Test_U_MediaFoundation_Subscribers_t*        subscribers;
  IMFVideoDisplayControl*                              windowController;
};
#else
struct Test_U_StreamConfiguration;
struct Test_U_ModuleHandlerConfiguration;
typedef Stream_Configuration_T<//stream_name_string_,
                               struct Stream_AllocatorConfiguration,
                               struct Test_U_StreamConfiguration,
                               struct Stream_ModuleConfiguration,
                               struct Test_U_ModuleHandlerConfiguration> Test_U_StreamConfiguration_t;
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
struct Test_U_DirectShow_StreamState
 : Stream_State
{
  Test_U_DirectShow_StreamState ()
   : Stream_State ()
   , sessionData (NULL)
   , userData (NULL)
  {}

  Test_U_DirectShow_SessionData* sessionData;

  struct Stream_UserData*        userData;
};

struct Test_U_MediaFoundation_StreamState
 : Stream_State
{
  Test_U_MediaFoundation_StreamState ()
   : Stream_State ()
   , sessionData (NULL)
   , userData (NULL)
  {}

  Test_U_MediaFoundation_SessionData* sessionData;

  struct Stream_UserData*             userData;
};
#else
struct Test_U_StreamState
 : Stream_State
{
  Test_U_StreamState ()
   : Stream_State ()
   , sessionData (NULL)
  {}

  Test_U_SessionData* sessionData;
};
#endif // ACE_WIN32 || ACE_WIN64

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
#if defined (ACE_WIN32) || defined (ACE_WIN64)
struct Test_U_DirectShow_StreamConfiguration
 : Test_U_StreamConfiguration
{
  Test_U_DirectShow_StreamConfiguration ()
   : Test_U_StreamConfiguration ()
   , format ()
  {}

  struct _AMMediaType format;
};

typedef Stream_IStreamControl_T<enum Stream_ControlType,
                                enum Stream_SessionMessageType,
                                enum Stream_StateMachine_ControlState,
                                struct Test_U_DirectShow_StreamState> Test_U_DirectShow_IStreamControl_t;

struct Test_U_MediaFoundation_StreamConfiguration
 : Test_U_StreamConfiguration
{
  Test_U_MediaFoundation_StreamConfiguration ()
   : Test_U_StreamConfiguration ()
   , format (NULL)
  {}

  IMFMediaType* format;
};

typedef Stream_IStreamControl_T<enum Stream_ControlType,
                                enum Stream_SessionMessageType,
                                enum Stream_StateMachine_ControlState,
                                struct Test_U_MediaFoundation_StreamState> Test_U_MediaFoundation_IStreamControl_t;
#else
typedef Stream_IStreamControl_T<enum Stream_ControlType,
                                enum Stream_SessionMessageType,
                                enum Stream_StateMachine_ControlState,
                                struct Test_U_StreamState> Test_U_IStreamControl_t;
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
//typedef Stream_DirectShowAllocatorBase_T<struct Stream_AllocatorConfiguration,
//                                         Stream_ControlMessage_t,
//                                         Test_U_Message,
//                                         Test_U_SessionMessage> Test_U_MessageAllocator_t;
typedef Stream_MessageAllocatorHeapBase_T<ACE_MT_SYNCH,
                                          struct Stream_AllocatorConfiguration,
                                          Stream_ControlMessage_t,
                                          Test_U_DirectShow_Message_t,
                                          Test_U_DirectShow_SessionMessage_t> Test_U_DirectShow_MessageAllocator_t;
typedef Stream_MessageAllocatorHeapBase_T<ACE_MT_SYNCH,
                                          struct Stream_AllocatorConfiguration,
                                          Stream_ControlMessage_t,
                                          Test_U_MediaFoundation_Message_t,
                                          Test_U_MediaFoundation_SessionMessage_t> Test_U_MediaFoundation_MessageAllocator_t;
#else
typedef Stream_MessageAllocatorHeapBase_T<ACE_MT_SYNCH,
                                          struct Stream_AllocatorConfiguration,
                                          Stream_ControlMessage_t,
                                          Test_U_Message_t,
                                          Test_U_SessionMessage_t> Test_U_MessageAllocator_t;
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Common_ISubscribe_T<Test_U_DirectShow_ISessionNotify_t> Test_U_DirectShow_ISubscribe_t;
typedef Common_ISubscribe_T<Test_U_MediaFoundation_ISessionNotify_t> Test_U_MediaFoundation_ISubscribe_t;

typedef Test_U_EventHandler_T<Test_U_DirectShow_ISessionNotify_t,
                                      Test_U_DirectShow_Message_t,
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
                                      Common_UI_GTK_State_t,
#elif defined (WXWIDGETS_USE)
                                      struct Common_UI_wxWidgets_State,
                                      Common_UI_wxWidgets_IApplicationBase_t,
#endif
#endif // GUI_SUPPORT
                                      Test_U_DirectShow_SessionMessage_t> Test_U_DirectShow_EventHandler_t;
typedef Test_U_EventHandler_T<Test_U_MediaFoundation_ISessionNotify_t,
                                      Test_U_MediaFoundation_Message_t,
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
                                      Common_UI_GTK_State_t,
#elif defined (WXWIDGETS_USE)
                                      struct Common_UI_wxWidgets_State,
                                      Common_UI_wxWidgets_IApplicationBase_t,
#endif
#endif // GUI_SUPPORT
                                      Test_U_MediaFoundation_SessionMessage_t> Test_U_MediaFoundation_EventHandler_t;
#else
typedef Common_ISubscribe_T<Test_U_ISessionNotify_t> Test_U_ISubscribe_t;

typedef Test_U_EventHandler_T<Test_U_ISessionNotify_t,
                              Test_U_Message_t,
                              Test_U_SessionMessage_t> Test_U_EventHandler_t;
#endif // ACE_WIN32 || ACE_WIN64

#endif
