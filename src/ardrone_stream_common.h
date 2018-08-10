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
#include "linux/videodev2.h"
#endif // ACE_WIN32 || ACE_WIN64

#ifdef __cplusplus
extern "C"
{
#include "libavformat/avformat.h"
}
#endif /* __cplusplus */

#include "gtk/gtk.h"

#include "ace/OS.h"

#include "common_ui_defines.h"

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
#endif // ACE_WIN32 || ACE_WIN64

#include "net_configuration.h"
#include "net_defines.h"
#include "net_iconnection.h"
#include "net_iconnectionmanager.h"

#include "ardrone_common.h"
#include "ardrone_types.h"

// *IMPORTANT NOTE*: these are defined in ardrone_stream.cpp
std::string ARDroneStreamTypeToString (const enum ARDrone_StreamType);
std::string ARDroneVideoModeToString (const enum ARDrone_VideoMode);
// *TODO*: use libav here
void ARDroneVideoModeToResolution (const enum ARDrone_VideoMode,
                                   unsigned int&,  // return value: width
                                   unsigned int&); // return value: height

// forward declarations
class ARDrone_Message;
class ARDrone_SessionMessage;

struct ARDrone_GtkCBData_Base;
struct ARDrone_SessionData;
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

  struct ARDrone_GtkCBData_Base* CBData;
  struct ARDrone_SessionData*    sessionData;
  enum ARDrone_StreamType        type;

  struct ARDrone_UserData*       userData;
};

struct ARDrone_ConnectionState;
struct ARDrone_SessionData
 : Stream_SessionData
{
  ARDrone_SessionData ()
   : Stream_SessionData ()
#if defined (ACE_WIN32) || defined (ACE_WIN64)
   , builder (NULL)
   , direct3DDevice (NULL)
   , direct3DManagerResetToken (0)
   , inputFormat (NULL)
   , session (NULL)
   , windowController (NULL)
#else
   , height (0)
   , inputFormat (AV_PIX_FMT_RGBA)
   , width (0)
#endif // ACE_WIN32 || ACE_WIN64
   , state (NULL)
   , statistic ()
   , targetFileName ()
   , userData (NULL)
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    inputFormat =
      static_cast<struct _AMMediaType*> (CoTaskMemAlloc (sizeof (struct _AMMediaType)));
    if (!inputFormat)
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate memory, continuing\n")));
    else
      ACE_OS::memset (inputFormat, 0, sizeof (struct _AMMediaType));
#else
    ARDroneVideoModeToResolution (ARDRONE_DEFAULT_VIDEO_MODE,
                                  width,
                                  height);
#endif // ACE_WIN32 || ACE_WIN64
  }

  struct ARDrone_SessionData operator+= (const struct ARDrone_SessionData& rhs_in)
  {
    // *NOTE*: the idea is to 'merge' the data
    Stream_SessionData::operator+= (rhs_in);

    // *NOTE*: the idea is to 'merge' the data
    statistic += rhs_in.statistic;
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
      direct3DManagerResetToken = rhs_in.direct3DManagerResetToken;
    } // end IF
    bool format_is_empty =
      (!inputFormat || (InlineIsEqualGUID (inputFormat->majortype, GUID_NULL)));
    if (format_is_empty && rhs_in.inputFormat)
      Stream_MediaFramework_DirectShow_Tools::copyMediaType (*rhs_in.inputFormat,
                                                             inputFormat);
    if (rhs_in.builder)
    {
      if (builder)
      {
        builder->Release ();
        builder = NULL;
      } // end IF
      reference_count = rhs_in.builder->AddRef ();
      builder = rhs_in.builder;
    } // end IF
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
    inputFormat =
        ((inputFormat != AV_PIX_FMT_NONE) ? inputFormat
                                          : rhs_in.inputFormat);
    height = (height ? height : rhs_in.height);
    width = (width ? width : rhs_in.width);
#endif // ACE_WIN32 || ACE_WIN64
    state = (state ? state : rhs_in.state);
    targetFileName =
      (!targetFileName.empty () ? targetFileName : rhs_in.targetFileName);

    userData = (userData ? userData : rhs_in.userData);

    return *this;
  }

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  IGraphBuilder*                  builder;
  // *TODO*: mediafoundation only, remove ASAP
  IDirect3DDevice9Ex*             direct3DDevice;
  // *TODO*: mediafoundation only, remove ASAP
  UINT                            direct3DManagerResetToken; // direct 3D manager 'id'
  struct _AMMediaType*            inputFormat;
  IMFMediaSession*                session;
  IVideoWindow*                   windowController;
#else
  unsigned int                    height;
  enum AVPixelFormat              inputFormat;
  unsigned int                    width;
#endif // ACE_WIN32 || ACE_WIN64

  struct ARDrone_StreamState*     state;
  struct ARDrone_Statistic        statistic;
  std::string                     targetFileName;

  struct ARDrone_UserData*        userData;
};
typedef Stream_SessionData_T<struct ARDrone_SessionData> ARDrone_SessionData_t;

struct ARDrone_AllocatorConfiguration;
typedef Stream_ControlMessage_T<enum Stream_ControlType,
                                enum Stream_ControlMessageType,
                                struct ARDrone_AllocatorConfiguration> ARDrone_ControlMessage_t;

typedef Stream_MessageAllocatorHeapBase_T<ACE_MT_SYNCH,
                                          struct ARDrone_AllocatorConfiguration,
                                          ARDrone_ControlMessage_t,
                                          ARDrone_Message,
                                          ARDrone_SessionMessage> ARDrone_MessageAllocator_t;

typedef Stream_ISessionDataNotify_T<Stream_SessionId_t,
                                    struct ARDrone_SessionData,
                                    enum Stream_SessionMessageType,
                                    ARDrone_Message,
                                    ARDrone_SessionMessage> ARDrone_Notification_t;
typedef std::list<ARDrone_Notification_t*> ARDrone_Subscribers_t;
typedef ARDrone_Subscribers_t::iterator ARDrone_SubscribersIterator_t;

struct ARDrone_GtkCBData_Base;
struct ARDrone_ModuleHandlerConfigurationBase
 : Stream_ModuleHandlerConfiguration
{
  ARDrone_ModuleHandlerConfigurationBase ()
   : Stream_ModuleHandlerConfiguration ()
   , block (false)
   , CBData (NULL)
   , codecId (AV_CODEC_ID_H264)
   , fullScreen (ARDRONE_DEFAULT_VIDEO_FULLSCREEN)
   , outboundStreamName (ACE_TEXT_ALWAYS_CHAR (ARDRONE_NAVDATA_STREAM_NAME_STRING))
   , printProgressDot (false)
   , pushStatisticMessages (true)
   , subscriber (NULL)
   , subscribers (NULL)
   , targetFileName ()
  {
    concurrency = STREAM_HEADMODULECONCURRENCY_CONCURRENT;

    passive = false;
  }

  bool                           block;                 // H264 decoder module
  struct ARDrone_GtkCBData_Base* CBData;                // controller module
  enum AVCodecID                 codecId;               // H264 decoder module
  bool                           fullScreen;            // display module
  std::string                    outboundStreamName;    // event handler module
  bool                           printProgressDot;      // file writer module
  bool                           pushStatisticMessages; // statistic module
  ARDrone_Notification_t*        subscriber;            // event handler module
  ARDrone_Subscribers_t*         subscribers;           // event handler module
  std::string                    targetFileName;        // file sink module
};
#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Stream_Configuration_T<//stream_name_string_,
                               struct ARDrone_AllocatorConfiguration,
                               struct ARDrone_StreamConfiguration,
                               struct Stream_ModuleConfiguration,
                               struct ARDrone_DirectShow_ModuleHandlerConfiguration> ARDrone_DirectShow_StreamConfiguration_t;
typedef Net_ConnectionConfiguration_T<struct ARDrone_DirectShow_ConnectionConfiguration,
                                      struct ARDrone_AllocatorConfiguration,
                                      ARDrone_DirectShow_StreamConfiguration_t> ARDrone_DirectShow_ConnectionConfiguration_t;
typedef Net_IConnection_T<ACE_INET_Addr,
                          ARDrone_DirectShow_ConnectionConfiguration_t,
                          struct ARDrone_ConnectionState,
                          struct ARDrone_Statistic> ARDrone_DirectShow_IConnection_t;
typedef std::unordered_map<std::string, // module name
                           ARDrone_DirectShow_ConnectionConfiguration_t> ARDrone_DirectShow_Stream_ConnectionConfigurations_t;
typedef Net_IConnectionManager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 ARDrone_DirectShow_ConnectionConfiguration_t,
                                 struct ARDrone_ConnectionState,
                                 struct ARDrone_Statistic,
                                 struct ARDrone_UserData> ARDrone_DirectShow_IConnectionManager_t;
struct ARDrone_DirectShow_FilterConfiguration;
struct ARDrone_DirectShow_ModuleHandlerConfiguration
 : ARDrone_ModuleHandlerConfigurationBase
{
  ARDrone_DirectShow_ModuleHandlerConfiguration ()
   : ARDrone_ModuleHandlerConfigurationBase ()
   , area ()
   , builder (NULL)
   , connection (NULL)
   , connectionConfigurations (NULL)
   , connectionManager (NULL)
   , deviceIdentifier ()
   , direct3DDevice (NULL)
   , filterCLSID (GUID_NULL)
   , filterConfiguration (NULL)
   , inputFormat (NULL)
   , outputFormat (NULL)
   , push (MODULE_LIB_DIRECTSHOW_FILTER_SOURCE_DEFAULT_PUSH)
   , window (NULL)
   , windowController (NULL)
   , windowController2 (NULL)
  {
    push = true;
  }

  struct tagRECT                                        area;                     // display module
  IGraphBuilder*                                        builder;                  // display module
  ARDrone_DirectShow_IConnection_t*                     connection;               // net source/IO module
  ARDrone_DirectShow_Stream_ConnectionConfigurations_t* connectionConfigurations; // net source/target modules
  ARDrone_DirectShow_IConnectionManager_t*              connectionManager;        // IO module
#if defined (UNICODE)
  std::wstring                                          deviceIdentifier;
#else
  std::string                                           deviceIdentifier;
#endif // UNICODE
  IDirect3DDevice9Ex*                                   direct3DDevice;           // display module
  struct _GUID                                          filterCLSID;              // display module
  struct ARDrone_DirectShow_FilterConfiguration*        filterConfiguration;      // display module
  struct _AMMediaType*                                  inputFormat;              // H264 decoder/display module
  struct _AMMediaType*                                  outputFormat;             // H264 decoder/display module
  bool                                                  push;                     // display module
  HWND                                                  window;                   // display module
  IVideoWindow*                                         windowController;         // display module
  IMFVideoDisplayControl*                               windowController2;        // display module (EVR)
};

typedef Stream_Configuration_T<//stream_name_string_,
                               struct ARDrone_AllocatorConfiguration,
                               struct ARDrone_StreamConfiguration,
                               struct Stream_ModuleConfiguration,
                               struct ARDrone_MediaFoundation_ModuleHandlerConfiguration> ARDrone_MediaFoundation_StreamConfiguration_t;
typedef Net_ConnectionConfiguration_T<struct ARDrone_MediaFoundation_ConnectionConfiguration,
                                      struct ARDrone_AllocatorConfiguration,
                                      ARDrone_MediaFoundation_StreamConfiguration_t> ARDrone_MediaFoundation_ConnectionConfiguration_t;
typedef Net_IConnection_T<ACE_INET_Addr,
                          ARDrone_MediaFoundation_ConnectionConfiguration_t,
                          struct ARDrone_ConnectionState,
                          struct ARDrone_Statistic> ARDrone_MediaFoundation_IConnection_t;
typedef std::unordered_map<std::string, // module name
                           ARDrone_MediaFoundation_ConnectionConfiguration_t> ARDrone_MediaFoundation_Stream_ConnectionConfigurations_t;
typedef Net_IConnectionManager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 ARDrone_MediaFoundation_ConnectionConfiguration_t,
                                 struct ARDrone_ConnectionState,
                                 struct ARDrone_Statistic,
                                 struct ARDrone_UserData> ARDrone_MediaFoundation_IConnectionManager_t;
struct ARDrone_MediaFoundation_ModuleHandlerConfiguration
 : ARDrone_ModuleHandlerConfigurationBase
{
  ARDrone_MediaFoundation_ModuleHandlerConfiguration ()
   : ARDrone_ModuleHandlerConfigurationBase ()
   , area ()
   , connection (NULL)
   , connectionConfigurations (NULL)
   , connectionManager (NULL)
   , deviceIdentifier ()
   , direct3DDevice (NULL)
   , inputFormat (NULL)
   , outputFormat (NULL)
   , rendererNodeId (0)
   , session (NULL)
   , window (NULL)
   , windowController (NULL)
  {}

  struct tagRECT                                             area;                     // display module
  ARDrone_MediaFoundation_IConnection_t*                     connection;               // net source/IO module
  ARDrone_MediaFoundation_Stream_ConnectionConfigurations_t* connectionConfigurations; // net source/target modules
  ARDrone_MediaFoundation_IConnectionManager_t*              connectionManager;        // IO module
#if defined (UNICODE)
  std::wstring                                               deviceIdentifier;         // device path (display-)
#else
  std::string                                                deviceIdentifier;         // device path (display-)
#endif // UNICODE
  IDirect3DDevice9Ex*                                        direct3DDevice;           // display module
  struct _AMMediaType*                                       inputFormat;              // H264 decoder/display module
  struct _AMMediaType*                                       outputFormat;             // H264 decoder/display module
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
 : ARDrone_ModuleHandlerConfigurationBase
{
  ARDrone_ModuleHandlerConfiguration ()
   : ARDrone_ModuleHandlerConfigurationBase ()
   , area ()
   , connection (NULL)
   , connectionConfigurations (NULL)
   , connectionManager (NULL)
   , displayDeviceIdentifier ()
   // *NOTE*: the GtkPixbuf native format appears to be RGBA
   , format (AV_PIX_FMT_RGBA)
   , frameRate ()
   , inputFormat (AV_PIX_FMT_RGBA)
   , outputFormat (AV_PIX_FMT_YUV420P)
   , pixelBuffer (NULL)
   , pixelBufferLock (NULL)
   , sourceFormat ()
   , window (NULL)
  {
    ACE_OS::memset (&sourceFormat, 0, sizeof (struct _cairo_rectangle_int));
  }

  GdkRectangle                               area;                     // display module
  ARDrone_IConnection_t*                     connection;               // net source/IO module
  ARDrone_Stream_ConnectionConfigurations_t* connectionConfigurations; // net source/target modules
  ARDrone_IConnectionManager_t*              connectionManager;        // IO module
  std::string                                displayDeviceIdentifier;
  enum AVPixelFormat                         format;                   // H264 decoder/display module
  struct AVRational                          frameRate;                // AVI encoder module
  enum AVPixelFormat                         inputFormat;              // display module
  enum AVPixelFormat                         outputFormat;             // H264 decoder module
  GdkPixbuf*                                 pixelBuffer;              // display module
  ACE_SYNCH_MUTEX*                           pixelBufferLock;          // display module
  struct _cairo_rectangle_int                sourceFormat;             // H264 decoder module
  GdkWindow*                                 window;                   // display module
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
   , CBData (NULL)
   , dispatch (NET_EVENT_DEFAULT_DISPATCH)
   , initializeControl (NULL)
   , initializeMAVLink (NULL)
   , initializeNavData (NULL)
   , userData (NULL)
  {}

  struct ARDrone_GtkCBData_Base* CBData;
  enum Common_EventDispatchType  dispatch;
  ARDrone_IControlInitialize_t*  initializeControl;
  ARDrone_IMAVLinkInitialize_t*  initializeMAVLink;
  ARDrone_INavDataInitialize_t*  initializeNavData;

  struct ARDrone_UserData*       userData;
};

//extern const char stream_name_string_[];
#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Stream_Configuration_T<//stream_name_string_,
                               struct ARDrone_AllocatorConfiguration,
                               struct ARDrone_StreamConfiguration,
                               struct Stream_ModuleConfiguration,
                               struct ARDrone_DirectShow_ModuleHandlerConfiguration> ARDrone_DirectShow_StreamConfiguration_t;
typedef ARDrone_DirectShow_StreamConfiguration_t::ITERATOR_T ARDrone_DirectShow_StreamConfigurationIterator_t;
typedef std::unordered_map<std::string,
                           ARDrone_DirectShow_StreamConfiguration_t> ARDrone_DirectShow_StreamConfigurations_t;
typedef ARDrone_DirectShow_StreamConfigurations_t::iterator ARDrone_DirectShow_StreamConfigurationsIterator_t;

typedef Stream_Configuration_T<//stream_name_string_,
                               struct ARDrone_AllocatorConfiguration,
                               struct ARDrone_StreamConfiguration,
                               struct Stream_ModuleConfiguration,
                               struct ARDrone_MediaFoundation_ModuleHandlerConfiguration> ARDrone_MediaFoundation_StreamConfiguration_t;
typedef ARDrone_MediaFoundation_StreamConfiguration_t::ITERATOR_T ARDrone_MediaFoundation_StreamConfigurationIterator_t;
typedef std::unordered_map<std::string,
                           ARDrone_MediaFoundation_StreamConfiguration_t> ARDrone_MediaFoundation_StreamConfigurations_t;
typedef ARDrone_MediaFoundation_StreamConfigurations_t::iterator ARDrone_MediaFoundation_StreamConfigurationsIterator_t;
#else
struct ARDrone_AllocatorConfiguration;
typedef Stream_Configuration_T<//stream_name_string_,
                               struct ARDrone_AllocatorConfiguration,
                               struct ARDrone_StreamConfiguration,
                               struct Stream_ModuleConfiguration,
                               struct ARDrone_ModuleHandlerConfiguration> ARDrone_StreamConfiguration_t;
typedef std::unordered_map<std::string,
                           ARDrone_StreamConfiguration_t> ARDrone_StreamConfigurations_t;
typedef ARDrone_StreamConfigurations_t::iterator ARDrone_StreamConfigurationsIterator_t;
#endif

#endif // #ifndef ARDRONE_STREAM_COMMON_H
