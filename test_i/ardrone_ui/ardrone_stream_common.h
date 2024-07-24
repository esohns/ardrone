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
#include <string>
#include <unordered_map>

#include "ace/config-lite.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
//#include <combaseapi.h>
#include <control.h>
#include <d3d9.h>
#include <evr.h>
#include <strmif.h>
#include <mfidl.h>
//#include <minwindef.h>
#else
//#include "linux/videodev2.h"
#endif // ACE_WIN32 || ACE_WIN64

#ifdef __cplusplus
extern "C"
{
#include "libavformat/avformat.h"
}
#endif /* __cplusplus */

#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
#include "gtk/gtk.h"
#elif defined (WXWIDGETS_USE)
#include "wx/apptrait.h"
#endif
#endif // GUI_SUPPORT

#include "ace/OS.h"

#if defined (GUI_SUPPORT)
#include "common_ui_common.h"
#include "common_ui_defines.h"
#endif // GUI_SUPPORT

#include "stream_base.h"
#include "stream_common.h"
#include "stream_control_message.h"
#include "stream_messageallocatorheap_base.h"
#include "stream_inotify.h"
#include "stream_isessionnotify.h"
#include "stream_session_data.h"

#include "stream_dec_defines.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "stream_dev_directshow_tools.h"

#include "stream_lib_directshow_tools.h"
#else
#include "stream_lib_ffmpeg_common.h"
#endif // ACE_WIN32 || ACE_WIN64

#include "stream_vis_common.h"
#include "stream_vis_defines.h"

#include "net_configuration.h"
#include "net_connection_configuration.h"
#include "net_defines.h"
#include "net_iconnection.h"
#include "net_iconnectionmanager.h"

#include "ardrone_common.h"
#include "ardrone_message.h"
#include "ardrone_tools.h"
#include "ardrone_types.h"

// forward declarations
//class ARDrone_Message;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
class ARDrone_DirectShow_SessionMessage;
class ARDrone_DirectShow_SessionData;
#else
class ARDrone_SessionMessage;
class ARDrone_SessionData;
#endif // ACE_WIN32 || ACE_WIN64

#if defined (GUI_SUPPORT)
struct ARDrone_UI_CBData_Base;
#endif // GUI_SUPPORT
struct ARDrone_UserData;
struct ARDrone_StreamState
 : Stream_State
{
  ARDrone_StreamState ()
   : CBData (NULL)
   , sessionData (NULL)
   , type (ARDRONE_STREAM_INVALID)
   , userData (NULL)
  {}

  struct ARDrone_StreamState operator+= (const struct ARDrone_StreamState& rhs_in)
  {
    // *NOTE*: the idea is to 'merge' the data
    Stream_State::operator+= (rhs_in);

    // *NOTE*: the idea is to 'merge' the data
    //*sessionData += *rhs_in.sessionData;
    type = (type != ARDRONE_STREAM_INVALID ? type : rhs_in.type);

    userData = (userData ? userData : rhs_in.userData);

    return *this;
  }

#if defined (GUI_SUPPORT)
  struct ARDrone_UI_CBData_Base*  CBData;
#endif // GUI_SUPPORT
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ARDrone_DirectShow_SessionData* sessionData;
#else
  ARDrone_SessionData*            sessionData;
#endif // ACE_WIN32 || ACE_WIN64
  enum ARDrone_StreamType         type;

  struct ARDrone_UserData*        userData;
};

struct ARDrone_ConnectionState;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
class ARDrone_DirectShow_SessionData
 : public Stream_SessionDataMediaBase_T<struct Stream_SessionData,
                                        struct _AMMediaType,
                                        struct ARDrone_StreamState,
                                        struct ARDrone_Statistic,
                                        struct ARDrone_UserData>
{
 public:
  ARDrone_DirectShow_SessionData ()
   : Stream_SessionDataMediaBase_T<struct Stream_SessionData,
                                   struct _AMMediaType,
                                   struct ARDrone_StreamState,
                                   struct ARDrone_Statistic,
                                   struct ARDrone_UserData> ()
   , builder (NULL)
   , direct3DDevice (NULL)
   , direct3DManagerResetToken (0)
   , session (NULL)
   , stream (NULL)
   , windowController (NULL)
   , targetFileName ()
  {}

  ARDrone_DirectShow_SessionData& operator+= (const ARDrone_DirectShow_SessionData& rhs_in)
  {
    // *NOTE*: the idea is to 'merge' the data
    Stream_SessionDataMediaBase_T<struct Stream_SessionData,
                                  struct _AMMediaType,
                                  struct ARDrone_StreamState,
                                  struct ARDrone_Statistic,
                                  struct ARDrone_UserData>::operator+= (rhs_in);

    // *NOTE*: the idea is to 'merge' the data
    // *NOTE*: always use upstream data, if available
    ULONG reference_count = 0;
    if (rhs_in.direct3DDevice)
    {
      if (direct3DDevice)
      {
        direct3DDevice->Release (); direct3DDevice = NULL;
      } // end IF
      reference_count = rhs_in.direct3DDevice->AddRef ();
      direct3DDevice = rhs_in.direct3DDevice;
      direct3DManagerResetToken = rhs_in.direct3DManagerResetToken;
    } // end IF
    if (rhs_in.builder)
    {
      if (builder)
      {
        builder->Release (); builder = NULL;
      } // end IF
      reference_count = rhs_in.builder->AddRef ();
      builder = rhs_in.builder;
    } // end IF
    if (rhs_in.windowController)
    {
      if (windowController)
      {
        windowController->Release (); windowController = NULL;
      } // end IF
      reference_count = rhs_in.windowController->AddRef ();
      windowController = rhs_in.windowController;
    } // end IF
    targetFileName =
      (!targetFileName.empty () ? targetFileName : rhs_in.targetFileName);

    return *this;
  }

  IGraphBuilder*              builder;
  // *TODO*: mediafoundation only, remove ASAP
#if COMMON_OS_WIN32_TARGET_PLATFORM (0x0600) // _WIN32_WINNT_VISTA
  IDirect3DDevice9Ex*         direct3DDevice;
#else
  IDirect3DDevice9*           direct3DDevice;
#endif // COMMON_OS_WIN32_TARGET_PLATFORM (0x0600)
  // *TODO*: mediafoundation only, remove ASAP
  UINT                        direct3DManagerResetToken; // direct 3D manager 'id'
  IMFMediaSession*            session;
  Stream_Base_t*              stream;
  IVideoWindow*               windowController;

  std::string                 targetFileName;
};
typedef Stream_SessionData_T<ARDrone_DirectShow_SessionData> ARDrone_DirectShow_SessionData_t;
#else
class ARDrone_SessionData
 : public Stream_SessionDataMediaBase_T<struct Stream_SessionData,
                                        struct Stream_MediaFramework_FFMPEG_VideoMediaType,
                                        struct ARDrone_StreamState,
                                        struct ARDrone_Statistic,
                                        struct ARDrone_UserData>
{
 public:
  ARDrone_SessionData ()
   : Stream_SessionDataMediaBase_T<struct Stream_SessionData,
                                   struct Stream_MediaFramework_FFMPEG_VideoMediaType,
                                   struct ARDrone_StreamState,
                                   struct ARDrone_Statistic,
                                   struct ARDrone_UserData> ()
   , stream (NULL)
   , targetFileName ()
  {
    struct Stream_MediaFramework_FFMPEG_VideoMediaType format_s;
    format_s.format = AV_PIX_FMT_RGBA;
    ARDroneVideoModeToResolution (ARDRONE_DEFAULT_VIDEO_MODE,
                                  format_s.resolution);
    formats.push_back (format_s);
  }

  ARDrone_SessionData& operator+= (const ARDrone_SessionData& rhs_in)
  {
    // *NOTE*: the idea is to 'merge' the data
    Stream_SessionDataMediaBase_T<struct Stream_SessionData,
                                  struct Stream_MediaFramework_FFMPEG_MediaType,
                                  struct ARDrone_StreamState,
                                  struct ARDrone_Statistic,
                                  struct ARDrone_UserData>::operator+= (rhs_in);

    stream = rhs_in.stream;
    targetFileName =
      (!targetFileName.empty () ? targetFileName : rhs_in.targetFileName);

    return *this;
  }

  Stream_Base_t* stream;
  std::string    targetFileName;
};
typedef Stream_SessionData_T<ARDrone_SessionData> ARDrone_SessionData_t;
#endif // ACE_WIN32 || ACE_WIN64

struct ARDrone_AllocatorConfiguration;
//typedef Stream_ControlMessage_T<enum Stream_ControlType,
//                                enum Stream_ControlMessageType,
//                                struct ARDrone_AllocatorConfiguration> ARDrone_ControlMessage_t;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Stream_MessageAllocatorHeapBase_T<ACE_MT_SYNCH,
                                          struct ARDrone_AllocatorConfiguration,
                                          Stream_ControlMessage_t,
                                          ARDrone_Message,
                                          ARDrone_DirectShow_SessionMessage> ARDrone_MessageAllocator_t;

typedef Stream_ISessionDataNotify_T<ARDrone_DirectShow_SessionData,
                                    enum Stream_SessionMessageType,
                                    ARDrone_Message,
                                    ARDrone_DirectShow_SessionMessage> ARDrone_Notification_t;
#else
typedef Stream_MessageAllocatorHeapBase_T<ACE_MT_SYNCH,
                                          struct ARDrone_AllocatorConfiguration,
                                          Stream_ControlMessage_t,
                                          ARDrone_Message,
                                          ARDrone_SessionMessage> ARDrone_MessageAllocator_t;

typedef Stream_ISessionDataNotify_T<struct ARDrone_SessionData,
                                    enum Stream_SessionMessageType,
                                    ARDrone_Message,
                                    ARDrone_SessionMessage> ARDrone_Notification_t;
#endif // ACE_WIN32 || ACE_WIN64
typedef std::list<ARDrone_Notification_t*> ARDrone_Subscribers_t;
typedef ARDrone_Subscribers_t::iterator ARDrone_SubscribersIterator_t;

struct ARDrone_UI_CBData_Base;
struct ARDrone_ModuleHandlerConfigurationBase
 //: Stream_ModuleHandlerConfiguration
{
  ARDrone_ModuleHandlerConfigurationBase ()
   : /*Stream_ModuleHandlerConfiguration ()
   ,  */ block (false)
#if defined (GUI_SUPPORT)
   , CBData (NULL)
#endif // GUI_SUPPORT
   , codecConfiguration (NULL)
#if defined (GUI_SUPPORT)
   , display ()
#if defined (ACE_WIN32) || defined (ACE_WIN64)
   , direct3DConfiguration (NULL)
#endif // ACE_WIN32 || ACE_WIN64
   , fullScreen (ARDRONE_DEFAULT_VIDEO_FULLSCREEN)
#endif // GUI_SUPPORT
   , finishOnDisconnect (true)
   , interfaceIdentifier ()
   , outboundStreamName (ACE_TEXT_ALWAYS_CHAR (ARDRONE_NAVDATA_STREAM_NAME_STRING))
   , printProgressDot (false)
   , subscriber (NULL)
   , subscribers (NULL)
   , targetFileName ()
  {}

  bool                                                 block; // H264 decoder module
#if defined (GUI_SUPPORT)
  struct ARDrone_UI_CBData_Base*                       CBData; // controller module
#endif // GUI_SUPPORT
  struct Stream_MediaFramework_FFMPEG_CodecConfiguration* codecConfiguration; // H264 decoder module
#if defined (GUI_SUPPORT)
  struct Common_UI_DisplayDevice                       display; // display module
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct Stream_MediaFramework_Direct3D_Configuration* direct3DConfiguration; // display module
#endif // ACE_WIN32 || ACE_WIN64
  bool                                                 fullScreen; // display module
#endif // GUI_SUPPORT
  bool                                                 finishOnDisconnect;
  std::string                                          interfaceIdentifier; // wireless-/display-
  std::string                                          outboundStreamName;  // event handler module
  bool                                                 printProgressDot;    // file writer module
  ARDrone_Notification_t*                              subscriber;          // event handler module
  ARDrone_Subscribers_t*                               subscribers;         // event handler module
  std::string                                          targetFileName;      // file sink module
};
#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Stream_Configuration_T<//stream_name_string_,
                               struct ARDrone_StreamConfiguration,
                               struct ARDrone_DirectShow_ModuleHandlerConfiguration> ARDrone_DirectShow_StreamConfiguration_t;
typedef Net_StreamConnectionConfiguration_T<ARDrone_DirectShow_StreamConfiguration_t,
                                            NET_TRANSPORTLAYER_TCP> ARDrone_DirectShow_TCPConnectionConfiguration_t;
typedef Net_IConnection_T<ACE_INET_Addr,
                          struct ARDrone_ConnectionState,
                          struct ARDrone_Statistic> ARDrone_IConnection_t;
typedef std::unordered_map<std::string, // module name
                           ARDrone_DirectShow_TCPConnectionConfiguration_t> ARDrone_DirectShow_Stream_ConnectionConfigurations_t;
typedef Net_IConnectionManager_T<ACE_INET_Addr,
                                 ARDrone_DirectShow_TCPConnectionConfiguration_t,
                                 struct ARDrone_ConnectionState,
                                 struct ARDrone_Statistic,
                                 struct Net_UserData> ARDrone_DirectShow_ITCPConnectionManager_t;
struct ARDrone_DirectShow_FilterConfiguration;
struct ARDrone_DirectShow_ModuleHandlerConfiguration
 : Stream_DirectShow_ModuleHandlerConfiguration
 , ARDrone_ModuleHandlerConfigurationBase
{
  ARDrone_DirectShow_ModuleHandlerConfiguration ()
   : Stream_DirectShow_ModuleHandlerConfiguration ()
   , ARDrone_ModuleHandlerConfigurationBase ()
   , area ()
   , builder (NULL)
   , connection (NULL)
   , connectionConfigurations (NULL)
   , connectionManager (NULL)
   , filterCLSID (GUID_NULL)
   , filterConfiguration (NULL)
   , push (STREAM_LIB_DIRECTSHOW_FILTER_SOURCE_DEFAULT_PUSH)
   , window (NULL)
   , windowController (NULL)
   , windowController2 (NULL)
  {
    concurrency = STREAM_HEADMODULECONCURRENCY_CONCURRENT;
    passive = false;

    push = true;
  }

  struct tagRECT                                        area;                     // display module
  IGraphBuilder*                                        builder;                  // display module
  ARDrone_IConnection_t*                                connection;               // net source/IO module
  ARDrone_DirectShow_Stream_ConnectionConfigurations_t* connectionConfigurations; // net source/target modules
  ARDrone_DirectShow_ITCPConnectionManager_t*           connectionManager;        // IO module
  struct _GUID                                          filterCLSID;              // display module
  struct ARDrone_DirectShow_FilterConfiguration*        filterConfiguration;      // display module
  bool                                                  push;                     // display module
  HWND                                                  window;                   // display module
  IVideoWindow*                                         windowController;         // display module
  IMFVideoDisplayControl*                               windowController2;        // display module (EVR)
};

typedef Stream_Configuration_T<//stream_name_string_,
                               struct ARDrone_StreamConfiguration,
                               struct ARDrone_MediaFoundation_ModuleHandlerConfiguration> ARDrone_MediaFoundation_StreamConfiguration_t;
typedef Net_StreamConnectionConfiguration_T<ARDrone_MediaFoundation_StreamConfiguration_t,
                                            NET_TRANSPORTLAYER_TCP> ARDrone_MediaFoundation_TCPConnectionConfiguration_t;
//typedef Net_IConnection_T<ACE_INET_Addr,
//                          struct ARDrone_ConnectionState,
//                          struct ARDrone_Statistic> ARDrone_MediaFoundation_IConnection_t;
typedef std::unordered_map<std::string, // module name
                           ARDrone_MediaFoundation_TCPConnectionConfiguration_t> ARDrone_MediaFoundation_Stream_ConnectionConfigurations_t;
typedef Net_IConnectionManager_T<ACE_INET_Addr,
                                 ARDrone_MediaFoundation_TCPConnectionConfiguration_t,
                                 struct ARDrone_ConnectionState,
                                 struct ARDrone_Statistic,
                                 struct ARDrone_UserData> ARDrone_MediaFoundation_ITCPConnectionManager_t;
struct ARDrone_MediaFoundation_ModuleHandlerConfiguration
 : Stream_MediaFoundation_ModuleHandlerConfiguration
 , ARDrone_ModuleHandlerConfigurationBase
{
  ARDrone_MediaFoundation_ModuleHandlerConfiguration ()
   : Stream_MediaFoundation_ModuleHandlerConfiguration ()
   , ARDrone_ModuleHandlerConfigurationBase ()
   , area ()
   , connection (NULL)
   , connectionConfigurations (NULL)
   , connectionManager (NULL)
   , rendererNodeId (0)
   , session (NULL)
   , window (NULL)
   , windowController (NULL)
  {
    concurrency = STREAM_HEADMODULECONCURRENCY_CONCURRENT;
    passive = false;
  }

  struct tagRECT                                             area;                     // display module
  ARDrone_IConnection_t*                                     connection;               // net source/IO module
  ARDrone_MediaFoundation_Stream_ConnectionConfigurations_t* connectionConfigurations; // net source/target modules
  ARDrone_MediaFoundation_ITCPConnectionManager_t*           connectionManager;        // IO module
  TOPOID                                                     rendererNodeId;
  IMFMediaSession*                                           session;
  HWND                                                       window;                   // display module
  IMFVideoDisplayControl*                                    windowController;
};
#else
struct ARDrone_ConnectionConfiguration;
struct ARDrone_AllocatorConfiguration;
struct ARDrone_StreamConfiguration;
struct ARDrone_ModuleHandlerConfiguration;
typedef Stream_Configuration_T<//stream_name_string_,
                               struct ARDrone_AllocatorConfiguration,
                               struct ARDrone_StreamConfiguration,
                               struct Stream_ModuleConfiguration,
                               struct ARDrone_ModuleHandlerConfiguration> ARDrone_StreamConfiguration_t;
typedef Net_ConnectionConfiguration_T<struct ARDrone_ConnectionConfiguration,
                                      struct ARDrone_AllocatorConfiguration,
                                      ARDrone_StreamConfiguration_t> ARDrone_ConnectionConfiguration_t;
typedef Net_IConnection_T<ACE_INET_Addr,
                          ARDrone_ConnectionConfiguration_t,
                          struct ARDrone_ConnectionState,
                          struct ARDrone_Statistic> ARDrone_IConnection_t;
typedef std::unordered_map<std::string, // module name
                           ARDrone_ConnectionConfiguration_t> ARDrone_Stream_ConnectionConfigurations_t;
typedef Net_IConnectionManager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 ARDrone_ConnectionConfiguration_t,
                                 struct ARDrone_ConnectionState,
                                 struct ARDrone_Statistic,
                                 struct ARDrone_UserData> ARDrone_IConnectionManager_t;
struct ARDrone_ModuleHandlerConfiguration
 : Stream_ModuleHandlerConfiguration
 , ARDrone_ModuleHandlerConfigurationBase
{
  ARDrone_ModuleHandlerConfiguration ()
   : Stream_ModuleHandlerConfiguration ()
   , ARDrone_ModuleHandlerConfigurationBase ()
#if defined (GUI_SUPPORT)
   , area ()
#endif // GUI_SUPPORT
   , connection (NULL)
   , connectionConfigurations (NULL)
   , connectionManager (NULL)
   , display ()
   , frameRate ()
#if defined (GUI_SUPPORT)
   , fullScreen (false)
   , outputFormat ()
#if defined (GTK_USE)
   , pixelBuffer (NULL)
   , pixelBufferLock (NULL)
#endif // GTK_USE
   , window (NULL)
#endif // GUI_SUPPORT
  {
    concurrency = STREAM_HEADMODULECONCURRENCY_CONCURRENT;
    passive = false;
  }

#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
  GdkRectangle                                  area;
#elif defined (WXWIDGETS_USE)
  wxRect                                        area;
#endif
#endif // GUI_SUPPORT
  ARDrone_IConnection_t*                        connection;               // net source/IO module
  ARDrone_Stream_ConnectionConfigurations_t*    connectionConfigurations; // net source/target modules
  ARDrone_IConnectionManager_t*                 connectionManager;        // IO module
  struct Common_UI_DisplayDevice                display;
  struct AVRational                             frameRate;                // AVI encoder module
#if defined (GUI_SUPPORT)
  bool                                          fullScreen;
  struct Stream_MediaFramework_FFMPEG_MediaType outputFormat;
#if defined (GTK_USE)
  GdkPixbuf*                                    pixelBuffer;              // display module
  ACE_SYNCH_MUTEX*                              pixelBufferLock;          // display module
#endif // GTK_USE
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  HWND                                          window;
#else
#if defined (GTK_USE)
  GdkWindow*                                    window;
#endif // GTK_USE
#endif // ACE_WIN32 || ACE_WIN64
#endif // GUI_SUPPORT
};
#endif // ACE_WIN32 || ACE_WIN64
typedef Common_IInitializeP_T<ARDrone_IControlNotify> ARDrone_IControlInitialize_t;
typedef Common_IInitializeP_T<ARDrone_IMAVLinkNotify> ARDrone_IMAVLinkInitialize_t;
typedef Common_IInitializeP_T<ARDrone_INavDataNotify> ARDrone_INavDataInitialize_t;
struct ARDrone_UserData;
struct ARDrone_StreamConfiguration
 : Stream_Configuration
{
  ARDrone_StreamConfiguration ()
   : Stream_Configuration ()
#if defined (GUI_SUPPORT)
   , CBData (NULL)
#endif // GUI_SUPPORT
   , deviceConfiguration (NULL)
   , dispatch (COMMON_EVENT_DEFAULT_DISPATCH)
   , format ()
   , initializeControl (NULL)
   , initializeMAVLink (NULL)
   , initializeNavData (NULL)
   , renderer (STREAM_VIS_RENDERER_VIDEO_DEFAULT)
   , userData (NULL)
  {}

#if defined (GUI_SUPPORT)
  struct ARDrone_UI_CBData_Base*                CBData;
#endif // GUI_SUPPORT
  ARDrone_IDeviceConfiguration*                 deviceConfiguration;
  enum Common_EventDispatchType                 dispatch;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct _AMMediaType                           format;
#else
  struct Stream_MediaFramework_FFMPEG_MediaType format;
#endif // ACE_WIN32 || ACE_WIN64
  ARDrone_IControlInitialize_t*                 initializeControl;
  ARDrone_IMAVLinkInitialize_t*                 initializeMAVLink;
  ARDrone_INavDataInitialize_t*                 initializeNavData;
  enum Stream_Visualization_VideoRenderer       renderer;

  struct ARDrone_UserData*                      userData;
};

//extern const char stream_name_string_[];
#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Stream_Configuration_T<//stream_name_string_,
                               struct ARDrone_StreamConfiguration,
                               struct ARDrone_DirectShow_ModuleHandlerConfiguration> ARDrone_DirectShow_StreamConfiguration_t;
typedef ARDrone_DirectShow_StreamConfiguration_t::ITERATOR_T ARDrone_DirectShow_StreamConfigurationIterator_t;
typedef std::unordered_map<std::string,
                           ARDrone_DirectShow_StreamConfiguration_t> ARDrone_DirectShow_StreamConfigurations_t;
typedef ARDrone_DirectShow_StreamConfigurations_t::iterator ARDrone_DirectShow_StreamConfigurationsIterator_t;

typedef Stream_Configuration_T<//stream_name_string_,
                               struct ARDrone_StreamConfiguration,
                               struct ARDrone_MediaFoundation_ModuleHandlerConfiguration> ARDrone_MediaFoundation_StreamConfiguration_t;
typedef ARDrone_MediaFoundation_StreamConfiguration_t::ITERATOR_T ARDrone_MediaFoundation_StreamConfigurationIterator_t;
typedef std::unordered_map<std::string,
                           ARDrone_MediaFoundation_StreamConfiguration_t> ARDrone_MediaFoundation_StreamConfigurations_t;
typedef ARDrone_MediaFoundation_StreamConfigurations_t::iterator ARDrone_MediaFoundation_StreamConfigurationsIterator_t;
#else
struct ARDrone_AllocatorConfiguration;
typedef Stream_Configuration_T<//stream_name_string_,
                               struct ARDrone_StreamConfiguration,
                               struct ARDrone_ModuleHandlerConfiguration> ARDrone_StreamConfiguration_t;
typedef std::unordered_map<std::string,
                           ARDrone_StreamConfiguration_t> ARDrone_StreamConfigurations_t;
typedef ARDrone_StreamConfigurations_t::iterator ARDrone_StreamConfigurationsIterator_t;
#endif // ACE_WIN32 || ACE_WIN64

//////////////////////////////////////////

typedef Stream_IStreamControl_T<enum Stream_ControlType,
                                enum Stream_SessionMessageType,
                                enum Stream_StateMachine_ControlState,
                                struct ARDrone_StreamState> ARDrone_IStreamControl_t;

#endif // #ifndef ARDRONE_STREAM_COMMON_H
