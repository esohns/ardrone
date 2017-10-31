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
#else
#include "linux/videodev2.h"
#endif

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

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "stream_dev_directshow_tools.h"
#endif

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

struct ARDrone_UserData;
struct ARDrone_SessionData;
struct ARDrone_StreamState
 : Stream_State
{
  ARDrone_StreamState ()
   : sessionData (NULL)
   , type (ARDRONE_STREAM_INVALID)
   , userData (NULL)
  {};

  struct ARDrone_StreamState operator+= (const struct ARDrone_StreamState& rhs_in)
  {
    // *NOTE*: the idea is to 'merge' the data
    Stream_State::operator+= (rhs_in);

    // *NOTE*: the idea is to 'merge' the data
    //*sessionData += *rhs_in.sessionData;
    type = (type != ARDRONE_STREAM_INVALID ? type : rhs_in.type);

    userData = (userData ? userData : rhs_in.userData);

    return *this;
  };

  struct ARDrone_SessionData* sessionData;
  enum ARDrone_StreamType     type;

  struct ARDrone_UserData*    userData;
};

struct ARDrone_ConnectionState;
struct ARDrone_SessionData
 : Stream_SessionData
{
  ARDrone_SessionData ()
   : Stream_SessionData ()
#if defined (ACE_WIN32) || defined (ACE_WIN64)
   , direct3DDevice (NULL)
   , direct3DManagerResetToken (0)
   , format (NULL)
   , graphBuilder (NULL)
   , session (NULL)
   , windowController (NULL)
#else
   , format (AV_PIX_FMT_RGBA)
   , height (0)
   , width (0)
#endif
   , state (NULL)
   , statistic ()
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
#else
    ARDroneVideoModeToResolution (ARDRONE_DEFAULT_VIDEO_MODE,
                                  width,
                                  height);
#endif
  };

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
    format = ((format != AV_PIX_FMT_NONE) ? format : rhs_in.format);
    height = (height ? height : rhs_in.height);
    width = (width ? width : rhs_in.width);
#endif
    state = (state ? state : rhs_in.state);
    targetFileName =
      (!targetFileName.empty () ? targetFileName : rhs_in.targetFileName);

    userData = (userData ? userData : rhs_in.userData);

    return *this;
  };

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  // *TODO*: mediafoundation only, remove ASAP
  IDirect3DDevice9Ex*             direct3DDevice;
  // *TODO*: mediafoundation only, remove ASAP
  UINT                            direct3DManagerResetToken; // direct 3D manager 'id'
  struct _AMMediaType*            format;
  IGraphBuilder*                  graphBuilder;
  IMFMediaSession*                session;
  IVideoWindow*                   windowController;
#else
  enum AVPixelFormat              format;
  unsigned int                    height;
  unsigned int                    width;
#endif

  struct ARDrone_StreamState*     state;
  struct ARDrone_Statistic        statistic;
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

struct ARDrone_ConnectionConfiguration;
struct ARDrone_ConnectionState;
typedef Net_IConnection_T<ACE_INET_Addr,
                          struct ARDrone_ConnectionConfiguration,
                          struct ARDrone_ConnectionState,
                          struct ARDrone_Statistic> ARDrone_IConnection_t;
typedef std::map<std::string,
                 struct ARDrone_ConnectionConfiguration> ARDrone_ConnectionConfigurations_t;
struct ARDrone_UserData;
typedef Net_IConnectionManager_T<ACE_MT_SYNCH,
                                 ACE_INET_Addr,
                                 struct ARDrone_ConnectionConfiguration,
                                 struct ARDrone_ConnectionState,
                                 struct ARDrone_Statistic,
                                 struct ARDrone_UserData> ARDrone_IConnectionManager_t;
struct ARDrone_DirectShow_FilterConfiguration;
struct ARDrone_StreamState;
//extern const char stream_name_string_[];
struct ARDrone_AllocatorConfiguration;
struct ARDrone_StreamConfiguration;
struct ARDrone_ModuleHandlerConfiguration;
typedef Stream_Configuration_T<//stream_name_string_,
                               struct ARDrone_AllocatorConfiguration,
                               struct ARDrone_StreamConfiguration,
                               struct Stream_ModuleConfiguration,
                               struct ARDrone_ModuleHandlerConfiguration> ARDrone_StreamConfiguration_t;
struct ARDrone_GtkCBData;
struct ARDrone_ModuleHandlerConfiguration
 : Stream_ModuleHandlerConfiguration
{
  ARDrone_ModuleHandlerConfiguration ()
   : Stream_ModuleHandlerConfiguration ()
   , block (false)
   , CBData (NULL)
   , codecFormat (AV_PIX_FMT_YUV420P) // codec output-
   , codecId (AV_CODEC_ID_H264)
   , connection (NULL)
   , connectionConfigurations ()
   , connectionManager (NULL)
   , device ()
#if defined (ACE_WIN32) || defined (ACE_WIN64)
   , area ()
   , consoleMode (false)
   , direct3DDevice (NULL)
   , filterCLSID (GUID_NULL)
   , filterConfiguration (NULL)
   , format (NULL)
   , graphBuilder (NULL)
   , push (MODULE_LIB_DIRECTSHOW_FILTER_SOURCE_DEFAULT_PUSH)
   , rendererNodeId (0)
   , session (NULL)
   , window (NULL)
   , windowController (NULL)
#else
   , area ()
   // *NOTE*: GtkPixbuf native format is RGBA
   , format (AV_PIX_FMT_RGBA)
   , frameRate ()
   , pixelBuffer (NULL)
   , pixelBufferLock (NULL)
   , sourceFormat ()
   , window (NULL)
#endif
   , fullScreen (ARDRONE_DEFAULT_VIDEO_FULLSCREEN)
   , inbound (true)
   , outboundStreamName (ACE_TEXT_ALWAYS_CHAR (ARDRONE_NAVDATA_STREAM_NAME_STRING))
   , printProgressDot (false)
   , pushStatisticMessages (true)
   , streamConfiguration (NULL)
   , subscriber (NULL)
   , subscribers (NULL)
   , targetFileName ()
  {
    concurrency = STREAM_HEADMODULECONCURRENCY_CONCURRENT;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
    //format =
    //  static_cast<struct _AMMediaType*> (CoTaskMemAlloc (sizeof (struct _AMMediaType)));
    //if (!format)
    //  ACE_DEBUG ((LM_CRITICAL,
    //              ACE_TEXT ("failed to allocate memory, continuing\n")));
    //else
    //  ACE_OS::memset (format, 0, sizeof (struct _AMMediaType));
#else
    ACE_OS::memset (&sourceFormat, 0, sizeof (struct _cairo_rectangle_int));
#endif

    passive = false;
  };

  bool                                block;                    // H264 decoder module
  struct ARDrone_GtkCBData*           CBData;                   // controller module
  enum AVPixelFormat                  codecFormat;              // H264 decoder module
  enum AVCodecID                      codecId;                  // H264 decoder module
  ARDrone_IConnection_t*              connection;               // net source/IO module
  ARDrone_ConnectionConfigurations_t* connectionConfigurations; // net source/target modules
  ARDrone_IConnectionManager_t*       connectionManager;        // IO module
  std::string                         device;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct tagRECT                                 area;                // display module
  bool                                           consoleMode;
  IDirect3DDevice9Ex*                            direct3DDevice;      // display module
  struct _GUID                                   filterCLSID;         // display module
  struct ARDrone_DirectShow_FilterConfiguration* filterConfiguration; // display module
  struct _AMMediaType*                           format;              // H264 decoder/display module
  IGraphBuilder*                                 graphBuilder;        // display module
  bool                                           push;                // display module
  TOPOID                                         rendererNodeId;
  IMFMediaSession*                               session;
  HWND                                           window;              // display module
  IVideoWindow*                                  windowController;    // display module
  //IMFVideoDisplayControl*                    windowController;
#else
  GdkRectangle                        area;            // display module
  enum AVPixelFormat                  format;          // H264 decoder/display module
  struct AVRational                   frameRate;       // AVI encoder module
  GdkPixbuf*                          pixelBuffer;     // display module
  ACE_SYNCH_MUTEX*                    pixelBufferLock; // display module
  struct _cairo_rectangle_int         sourceFormat;    // H264 decoder module
  GdkWindow*                          window;          // display module
#endif
  bool                                fullScreen;            // display module
  bool                                inbound;               // statistic/IO module
  std::string                         outboundStreamName;    // event handler module
  bool                                printProgressDot;      // file writer module
  bool                                pushStatisticMessages; // statistic module
  ARDrone_StreamConfiguration_t*      streamConfiguration;   // net source/target modules
  ARDrone_Notification_t*             subscriber; // event handler module
  ARDrone_Subscribers_t*              subscribers; // event handler module
  std::string                         targetFileName;
};

typedef Common_IInitializeP_T<ARDrone_IControlNotify> ARDrone_IControlInitialize_t;
typedef Common_IInitializeP_T<ARDrone_IMAVLinkNotify> ARDrone_IMAVLinkInitialize_t;
typedef Common_IInitializeP_T<ARDrone_INavDataNotify> ARDrone_INavDataInitialize_t;
struct ARDrone_StreamConfiguration
 : Stream_Configuration
{
  ARDrone_StreamConfiguration ()
   : Stream_Configuration ()
   , CBData (NULL)
   , initializeControl (NULL)
   , initializeMAVLink (NULL)
   , initializeNavData (NULL)
   , useReactor (NET_EVENT_USE_REACTOR)
   , userData (NULL)
  {};

  struct ARDrone_GtkCBData*     CBData;
  ARDrone_IControlInitialize_t* initializeControl;
  ARDrone_IMAVLinkInitialize_t* initializeMAVLink;
  ARDrone_INavDataInitialize_t* initializeNavData;
  bool                          useReactor;

  struct ARDrone_UserData*      userData;
};
typedef std::map<std::string,
                 ARDrone_StreamConfiguration_t> ARDrone_StreamConfigurations_t;
typedef ARDrone_StreamConfigurations_t::iterator ARDrone_StreamConfigurationsIterator_t;

typedef Stream_StatisticHandler_T<struct ARDrone_Statistic> ARDrone_StatisticHandler_t;

#endif // #ifndef ARDRONE_STREAM_COMMON_H
