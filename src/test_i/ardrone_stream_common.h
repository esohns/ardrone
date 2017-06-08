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

#ifndef ARDRONE_STREAM_COMMON_H
#define ARDRONE_STREAM_COMMON_H

#include <list>
#include <map>
#include <string>

#include "ace/config-lite.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <d3d9.h>
#include <strmif.h>
#include <minwindef.h>
//#include <mfidl.h>
#endif

#ifdef __cplusplus
extern "C"
{
#include "libavformat/avformat.h"
}
#endif /* __cplusplus */

#include "common_ui_defines.h"

#include "stream_base.h"
#include "stream_common.h"
#include "stream_control_message.h"
#include "stream_messageallocatorheap_base.h"
#include "stream_inotify.h"
#include "stream_isessionnotify.h"
#include "stream_session_data.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "stream_dev_directshow_tools.h"
#endif

#include "net_iconnection.h"
#include "net_iconnectionmanager.h"

#include "ardrone_modules_common.h"
//#include "ardrone_stream.h"

// forward declarations
class ARDrone_MAVLinkMessage;
class ARDrone_NavDataMessage;
class ARDrone_LiveVideoMessage;
class ARDrone_SessionMessage;

struct ARDrone_UserData;
struct ARDrone_SessionData;
struct ARDrone_StreamState
 : Stream_State
{
  inline ARDrone_StreamState ()
   : currentSessionData (NULL)
   , userData (NULL)
  {};

  struct ARDrone_SessionData* currentSessionData;
  struct ARDrone_UserData*    userData;
};

struct ARDrone_ConnectionState;
typedef Stream_Statistic ARDrone_RuntimeStatistic_t;
struct ARDrone_SessionData
 : Stream_SessionData
{
  inline ARDrone_SessionData ()
   : Stream_SessionData ()
   , connectionState (NULL)
   , currentStatistic ()
#if defined (ACE_WIN32) || defined (ACE_WIN64)
   , direct3DDevice (NULL)
   , format (NULL)
   , graphBuilder (NULL)
   //, rendererNodeId (0)
   , resetToken (0)
   , session (NULL)
   , windowController (NULL)
#else
   , format (AV_PIX_FMT_RGBA)
//   , height (ARDRONE_DEFAULT_VIDEO_HEIGHT)
//   , width (ARDRONE_DEFAULT_VIDEO_WIDTH)
#endif
   , isNavData (false)
   , state (NULL)
   , targetFileName ()
   , userData (NULL)
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    format =
      static_cast<struct _AMMediaType*> (CoTaskMemAlloc (sizeof (struct _AMMediaType)));
    if (!format)
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory, continuing\n")));
    else
      ACE_OS::memset (format, 0, sizeof (struct _AMMediaType));
#endif
  };

  inline struct ARDrone_SessionData operator+= (const struct ARDrone_SessionData& rhs_in)
  {
    // *NOTE*: the idea is to 'merge' the data
    Stream_SessionData::operator+= (rhs_in);

    // *NOTE*: the idea is to 'merge' the data
    currentStatistic += rhs_in.currentStatistic;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    // *NOTE*: always use upstream data, if available
    ULONG reference_count = 0;
    if (rhs_in.direct3DDevice)
    {
      if (direct3DDevice)
      {
        direct3DDevice->Release ();
        direct3DDevice = NULL;
      } // end IF
      reference_count = rhs_in.direct3DDevice->AddRef ();
      direct3DDevice = rhs_in.direct3DDevice;
      resetToken = rhs_in.resetToken;
    } // end IF
    bool format_is_empty = (!format || (format->majortype == GUID_NULL));
    if (format_is_empty && rhs_in.format)
      Stream_Module_Device_DirectShow_Tools::copyMediaType (*rhs_in.format,
                                                            format);
    if (rhs_in.graphBuilder)
    {
      if (graphBuilder)
      {
        graphBuilder->Release ();
        graphBuilder = NULL;
      } // end IF
      reference_count = rhs_in.graphBuilder->AddRef ();
      graphBuilder = rhs_in.graphBuilder;
    } // end IF
    //rendererNodeId = (rendererNodeId ? rendererNodeId : rhs_in.rendererNodeId);
    //session = (session ? session : rhs_in.session);
    if (rhs_in.windowController)
    {
      if (windowController)
      {
        windowController->Release ();
        windowController = NULL;
      } // end IF
      reference_count = rhs_in.windowController->AddRef ();
      windowController = rhs_in.windowController;
    } // end IF
#else
#endif
    state = (state ? state : rhs_in.state);
    targetFileName =
      (!targetFileName.empty () ? targetFileName : rhs_in.targetFileName);
    userData = (userData ? userData : rhs_in.userData);

    return *this;
  };

  struct ARDrone_ConnectionState* connectionState;
  ARDrone_RuntimeStatistic_t      currentStatistic;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  IDirect3DDevice9Ex*             direct3DDevice;
  struct _AMMediaType*            format;
  IGraphBuilder*                  graphBuilder;
  //TOPOID                          rendererNodeId;
  UINT                            resetToken; // direct 3D manager 'id'
  IMFMediaSession*                session;
  IVideoWindow*                   windowController;
#else
  enum AVPixelFormat              format;
//  unsigned int                    height;
//  unsigned int                    width;
#endif

  bool                            isNavData;
  struct ARDrone_StreamState*     state;
  std::string                     targetFileName;

  struct ARDrone_UserData*        userData;
};
typedef Stream_SessionData_T<struct ARDrone_SessionData> ARDrone_SessionData_t;

typedef Stream_ControlMessage_T<enum Stream_ControlType,
                                enum Stream_ControlMessageType,
                                struct ARDrone_AllocatorConfiguration> ARDrone_ControlMessage_t;

typedef Stream_MessageAllocatorHeapBase_T<ACE_MT_SYNCH,
                                          struct ARDrone_AllocatorConfiguration,
                                          ARDrone_ControlMessage_t,
                                          ARDrone_Message,
                                          ARDrone_SessionMessage> ARDrone_MessageAllocator_t;

//typedef Stream_INotify_T<enum Stream_SessionMessageType> ARDrone_IStreamNotify_t;

typedef Stream_ISessionDataNotify_T<Stream_SessionId_t,
                                    struct ARDrone_SessionData,
                                    enum Stream_SessionMessageType,
                                    ARDrone_Message,
                                    ARDrone_SessionMessage> ARDrone_Notification_t;
typedef std::list<ARDrone_Notification_t*> ARDrone_Subscribers_t;
typedef ARDrone_Subscribers_t::iterator ARDrone_SubscribersIterator_t;

//struct ARDrone_ConnectionConfiguration;
//typedef std::deque<struct ARDrone_ConnectionConfiguration> ARDrone_ConnectionConfigurations_t;
//typedef ARDrone_ConnectionConfigurations_t::iterator ARDrone_ConnectionConfigurationIterator_t;
struct ARDrone_ConnectionState;
typedef Net_IConnection_T<ACE_INET_Addr,
                          struct ARDrone_ConnectionConfiguration,
                          struct ARDrone_ConnectionState,
                          ARDrone_RuntimeStatistic_t> ARDrone_IConnection_t;
struct ARDrone_UserData;
typedef Net_IConnectionManager_T<ACE_INET_Addr,
                                 struct ARDrone_ConnectionConfiguration,
                                 struct ARDrone_ConnectionState,
                                 ARDrone_RuntimeStatistic_t,
                                 struct ARDrone_UserData> ARDrone_IConnectionManager_t;
struct ARDrone_DirectShow_FilterConfiguration;
struct ARDrone_StreamState;
struct ARDrone_StreamConfiguration;
struct ARDrone_ModuleHandlerConfiguration;
struct ARDrone_SocketHandlerConfiguration;
//typedef Stream_Base_T<ACE_MT_SYNCH,
//                      Common_TimePolicy_t,
//                      enum Stream_ControlType,
//                      enum Stream_SessionMessageType,
//                      enum Stream_StateMachine_ControlState,
//                      struct ARDrone_StreamState,
//                      struct ARDrone_StreamConfiguration,
//                      ARDrone_RuntimeStatistic_t,
//                      struct Stream_ModuleConfiguration,
//                      struct ARDrone_ModuleHandlerConfiguration,
//                      struct ARDrone_SessionData,
//                      ARDrone_StreamSessionData_t,
//                      ARDrone_ControlMessage_t,
//                      ARDrone_LiveVideoMessage,
//                      ARDrone_SessionMessage> ARDrone_LiveVideoStreamBase_t;
struct ARDrone_ModuleHandlerConfiguration
 : Stream_ModuleHandlerConfiguration
{
  inline ARDrone_ModuleHandlerConfiguration ()
   : Stream_ModuleHandlerConfiguration ()
   , block (false)
   , codecFormat (AV_PIX_FMT_YUV420P) // codec output-
   , codecId (AV_CODEC_ID_H264)
   , connection (NULL)
   , connectionConfigurations ()
   , connectionManager (NULL)
   , debugScanner (COMMON_PARSER_DEFAULT_LEX_TRACE)
   , device ()
#if defined (ACE_WIN32) || defined (ACE_WIN64)
   , area ()
   , consoleMode (false)
   , direct3DDevice (NULL)
   , filterCLSID (GUID_NULL)
   , filterConfiguration (NULL)
   , format (NULL)
   , graphBuilder (NULL)
   , push (COMMON_DEFAULT_WIN32_DIRECTSHOW_USE_PUSH_SOURCE_FILTER)
   , rendererNodeId (0)
   , session (NULL)
   , window (NULL)
   , windowController (NULL)
#else
   , area ()
   // *NOTE*: GtkPixbuf native format is RGBA
   , format (AV_PIX_FMT_RGBA)
//   , height (ARDRONE_DEFAULT_VIDEO_HEIGHT)
   , pixelBuffer (NULL)
   , pixelBufferLock (NULL)
   , sourceFormat ()
//   , width (ARDRONE_DEFAULT_VIDEO_WIDTH)
   , window (NULL)
#endif
   , fullScreen (ARDRONE_DEFAULT_VIDEO_FULLSCREEN)
   , inbound (true)
   , printProgressDot (false)
   , pushStatisticMessages (true)
   //, socketConfigurations (NULL)
   //, socketHandlerConfiguration (NULL)
   , streamConfiguration (NULL)
   , subscriber (NULL)
   , subscribers (NULL)
   , targetFileName ()
   , useYYScanBuffer (STREAM_DECODER_DEFAULT_FLEX_USE_YY_SCAN_BUFFER)
  {
    concurrency = STREAM_HEADMODULECONCURRENCY_CONCURRENT;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
    format =
      static_cast<struct _AMMediaType*> (CoTaskMemAlloc (sizeof (struct _AMMediaType)));
    if (!format)
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory, continuing\n")));
    else
      ACE_OS::memset (format, 0, sizeof (struct _AMMediaType));
#else
    ACE_OS::memset (&sourceFormat, 0, sizeof (struct _cairo_rectangle_int));
#endif

    passive = false;
  };

  bool                                           block;               // H264 decoder module
  enum AVPixelFormat                             codecFormat;         // H264 decoder module
  enum AVCodecID                                 codecId;             // H264 decoder module
  ARDrone_IConnection_t*                         connection;          // net source/IO module
  ARDrone_ConnectionConfigurations_t*            connectionConfigurations; // net source/target modules
  ARDrone_IConnectionManager_t*                  connectionManager;   // IO module
  bool                                           debugScanner;        // H264 decoder module
  std::string                                    device;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct tagRECT                                 area;                // display module
  bool                                           consoleMode;
  IDirect3DDevice9Ex*                            direct3DDevice;      // display module
  struct _GUID                                   filterCLSID;         // display module
  struct ARDrone_DirectShow_FilterConfiguration* filterConfiguration; // display module
  struct _AMMediaType*                           format;              // display module
  IGraphBuilder*                                 graphBuilder;        // display module
  bool                                           push;                // display module
  TOPOID                                         rendererNodeId;
  IMFMediaSession*                               session;
  HWND                                           window;              // display module
  IVideoWindow*                                  windowController;    // display module
  //IMFVideoDisplayControl*                    windowController;
#else
  GdkRectangle                                   area;            // display module
  enum AVPixelFormat                             format;          // display module
//  unsigned int                                   height;          // display module
  GdkPixbuf*                                     pixelBuffer;     // display module
  ACE_SYNCH_MUTEX*                               pixelBufferLock; // display module
  struct _cairo_rectangle_int                    sourceFormat;    // H264 decoder module
//  unsigned int                                   width;           // display module
  GdkWindow*                                     window;          // display module
#endif
  bool                                           fullScreen;          // display module
  bool                                           inbound;          // statistic/IO module
  bool                                           printProgressDot; // file writer module
  bool                                           pushStatisticMessages; // statistic module
  struct ARDrone_StreamConfiguration*            streamConfiguration; // net source/target modules
  ARDrone_Notification_t*                        subscriber; // event handler module
  ARDrone_Subscribers_t*                         subscribers; // event handler module
  std::string                                    targetFileName;
  bool                                           useYYScanBuffer; // H264 decoder module
};

typedef Common_IInitializeP_T<ARDrone_IMAVLinkNotify> ARDrone_IMAVLinkInitialize_t;
typedef Common_IInitializeP_T<ARDrone_INavDataNotify> ARDrone_INavDataInitialize_t;
typedef std::map<std::string,
                 struct ARDrone_ModuleHandlerConfiguration> ARDrone_ModuleHandlerConfigurations_t;
typedef ARDrone_ModuleHandlerConfigurations_t::iterator ARDrone_ModuleHandlerConfigurationsIterator_t;
struct ARDrone_StreamConfiguration
 : Stream_Configuration
{
  inline ARDrone_StreamConfiguration ()
   : Stream_Configuration ()
   , initializeMAVLink (NULL)
   , initializeNavData (NULL)
   , moduleConfiguration_2 ()
   , moduleHandlerConfigurations ()
   , useReactor (NET_EVENT_USE_REACTOR)
   , userData (NULL)
  {};

  ARDrone_IMAVLinkInitialize_t*         initializeMAVLink;
  ARDrone_INavDataInitialize_t*         initializeNavData;
  struct Stream_ModuleConfiguration     moduleConfiguration_2;
  ARDrone_ModuleHandlerConfigurations_t moduleHandlerConfigurations;
  bool                                  useReactor;

  struct ARDrone_UserData*              userData;
};

#endif // #ifndef ARDRONE_STREAM_COMMON_H
