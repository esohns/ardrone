/***************************************************************************
*   Copyright (C) 2010 by Erik Sohns   *
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

#ifndef ARDRONE_CONFIGURATION_H
#define ARDRONE_CONFIGURATION_H

#include <algorithm>
#include <deque>
#include <map>
#include <string>

#if defined (GTKGL_SUPPORT)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <gl/GL.h>
#else
#include <GL/gl.h>
#endif // ACE_WIN32 || ACE_WIN64
#endif /* GTKGL_SUPPORT */

#include "gtk/gtk.h"

#include "ace/Singleton.h"
#include "ace/Synch_Traits.h"

#include "common_gl_common.h"

#include "common_ui_gtk_builder_definition.h"
#include "common_ui_gtk_gl_common.h"
#include "common_ui_gtk_manager.h"

#include "stream_common.h"
#include "stream_configuration.h"

#include "stream_dec_defines.h"

#include "stream_lib_common.h"
#include "stream_lib_defines.h"

#include "net_iconnector.h"

#include "net_wlan_configuration.h"

#include "ardrone_defines.h"
#include "ardrone_network_common.h"
//#include "ardrone_stream.h"
#include "ardrone_stream_common.h"
#include "ardrone_types.h"

// forward declarations
class ARDrone_MAVLinkMessage;
class ARDrone_NavDataMessage;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
struct ARDrone_DirectShow_FilterConfiguration
 : Stream_MediaFramework_DirectShow_FilterConfiguration
{
  ARDrone_DirectShow_FilterConfiguration ()
   : Stream_MediaFramework_DirectShow_FilterConfiguration ()
   , module (NULL)
   , pinConfiguration (NULL)
  {}

  // *TODO*: specify this as part of the network protocol header/handshake
  Stream_Module_t*                                                module; // handle
  struct Stream_MediaFramework_DirectShow_FilterPinConfiguration* pinConfiguration; // handle
};
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
struct ARDrone_DirectShow_SocketHandlerConfiguration;
struct ARDrone_MediaFoundation_SocketHandlerConfiguration;
typedef Net_IConnector_T<ACE_INET_Addr,
                         struct ARDrone_DirectShow_SocketHandlerConfiguration> ARDrone_DirectShow_IConnector_t;
typedef Net_IConnector_T<ACE_INET_Addr,
                         struct ARDrone_MediaFoundation_SocketHandlerConfiguration> ARDrone_MediaFoundation_IConnector_t;
#else
struct ARDrone_SocketHandlerConfiguration;
typedef Net_IConnector_T<ACE_INET_Addr,
                         struct ARDrone_SocketHandlerConfiguration> ARDrone_IConnector_t;
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
struct ARDrone_DirectShow_SignalConfiguration
 : Stream_SignalHandlerConfiguration
{
  ARDrone_DirectShow_SignalConfiguration ()
   : Stream_SignalHandlerConfiguration ()
   , actionTimerId (-1)
   , connector (NULL)
   , peerAddress ()
  {}

  long                             actionTimerId;
  ARDrone_DirectShow_IConnector_t* connector;
  ACE_INET_Addr                    peerAddress;
};

struct ARDrone_MediaFoundation_SignalConfiguration
 : Stream_SignalHandlerConfiguration
{
  ARDrone_MediaFoundation_SignalConfiguration ()
   : Stream_SignalHandlerConfiguration ()
   , actionTimerId (-1)
   , connector (NULL)
   , peerAddress ()
  {}

  long                                  actionTimerId;
  ARDrone_MediaFoundation_IConnector_t* connector;
  ACE_INET_Addr                         peerAddress;
};
#else
struct ARDrone_SignalConfiguration
 : Stream_SignalHandlerConfiguration
{
  ARDrone_SignalConfiguration ()
   : Stream_SignalHandlerConfiguration ()
   , actionTimerId (-1)
   , connector (NULL)
   , peerAddress ()
  {}

  long                  actionTimerId;
  ARDrone_IConnector_t* connector;
  ACE_INET_Addr         peerAddress;
};
#endif // ACE_WIN32 || ACE_WIN64

struct ARDrone_WLANMonitorConfiguration
 : Net_WLAN_MonitorConfiguration
{
  ARDrone_WLANMonitorConfiguration ()
   : Net_WLAN_MonitorConfiguration ()
   , userData (NULL)
  {
    autoAssociate = ARDRONE_DEFAULT_WLAN_SSID_AUTOASSOCIATE;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    enableMediaStreamingMode = ARDRONE_DEFAULT_WLAN_ENABLE_MEDIASTREAMING;
#endif // ACE_WIN32 || ACE_WIN64
  }

  struct ARDrone_UserData* userData;
};

#if defined (ACE_WIN32) || defined (ACE_WIN64)
struct ARDrone_DirectShow_Configuration
{
  ARDrone_DirectShow_Configuration ()
   : allocatorConfiguration ()
   , dispatchConfiguration ()
   , signalConfiguration ()
   , WLANMonitorConfiguration ()
   , connectionConfigurations ()
   , parserConfiguration ()
   , filterConfiguration ()
   , pinConfiguration ()
   , streamConfigurations ()
   , streamSubscribers ()
   , streamSubscribersLock ()
   , userData (NULL)
  {
    filterConfiguration.pinConfiguration = &pinConfiguration;
  }

  struct ARDrone_AllocatorConfiguration                          allocatorConfiguration;
  struct Common_EventDispatchConfiguration                       dispatchConfiguration;

  struct ARDrone_DirectShow_SignalConfiguration                  signalConfiguration;

  struct ARDrone_WLANMonitorConfiguration                        WLANMonitorConfiguration;
  ARDrone_DirectShow_ConnectionConfigurations_t                  connectionConfigurations;
  struct Common_ParserConfiguration                              parserConfiguration;
  struct ARDrone_DirectShow_FilterConfiguration                  filterConfiguration;
  struct Stream_MediaFramework_DirectShow_FilterPinConfiguration pinConfiguration;
  ARDrone_DirectShow_StreamConfigurations_t                      streamConfigurations;
  ARDrone_Subscribers_t                                          streamSubscribers;
  ACE_SYNCH_RECURSIVE_MUTEX                                      streamSubscribersLock;

  struct ARDrone_UserData*                                       userData;
};

struct ARDrone_MediaFoundation_Configuration
{
  ARDrone_MediaFoundation_Configuration ()
   : allocatorConfiguration ()
   , dispatchConfiguration ()
   , signalConfiguration ()
   , WLANMonitorConfiguration ()
   , connectionConfigurations ()
   , parserConfiguration ()
   , streamConfigurations ()
   , streamSubscribers ()
   , streamSubscribersLock ()
   , userData (NULL)
  {}

  struct ARDrone_AllocatorConfiguration              allocatorConfiguration;
  struct Common_EventDispatchConfiguration           dispatchConfiguration;

  struct ARDrone_MediaFoundation_SignalConfiguration signalConfiguration;

  struct ARDrone_WLANMonitorConfiguration            WLANMonitorConfiguration;
  ARDrone_MediaFoundation_ConnectionConfigurations_t connectionConfigurations;
  struct Common_ParserConfiguration                  parserConfiguration;
  ARDrone_MediaFoundation_StreamConfigurations_t     streamConfigurations;
  ARDrone_Subscribers_t                              streamSubscribers;
  ACE_SYNCH_RECURSIVE_MUTEX                          streamSubscribersLock;

  struct ARDrone_UserData*                           userData;
};
#else
struct ARDrone_Configuration
{
  ARDrone_Configuration ()
   : allocatorConfiguration ()
   , dispatchConfiguration ()
   , signalConfiguration ()
   , WLANMonitorConfiguration ()
   , connectionConfigurations ()
   , parserConfiguration ()
   , streamConfigurations ()
   , streamSubscribers ()
   , streamSubscribersLock ()
   , userData (NULL)
  {}

  struct ARDrone_AllocatorConfiguration    allocatorConfiguration;
  struct Common_EventDispatchConfiguration dispatchConfiguration;

  struct ARDrone_SignalConfiguration       signalConfiguration;

  struct ARDrone_WLANMonitorConfiguration  WLANMonitorConfiguration;
  ARDrone_ConnectionConfigurations_t       connectionConfigurations;
  struct Common_ParserConfiguration        parserConfiguration;
  ARDrone_StreamConfigurations_t           streamConfigurations;
  ARDrone_Subscribers_t                    streamSubscribers;
  ACE_SYNCH_RECURSIVE_MUTEX                streamSubscribersLock;

  struct ARDrone_UserData*                 userData;
};
#endif // ACE_WIN32 || ACE_WIN64

//////////////////////////////////////////

typedef std::deque<ARDrone_Message*> ARDrone_Messages_t;
typedef ARDrone_Messages_t::const_iterator ARDrone_MessagesIterator_t;

struct ARDrone_GtkProgressData
 : Common_UI_GTK_ProgressData
{
  ARDrone_GtkProgressData ()
   : Common_UI_GTK_ProgressData ()
   , statistic ()
  {}

  struct ARDrone_Statistic statistic;
};

typedef std::map<std::string,
                 Stream_Base_t*> ARDrone_Streams_t;
typedef ARDrone_Streams_t::const_iterator ARDrone_StreamsIterator_t;
#if defined (_MSC_VER)
#pragma pack (push, 1)
#endif // _MSC_VER
struct ARDrone_GtkCBData_Base
#if defined (GTKGL_SUPPORT)
  : Common_UI_GTK_GLState
#else
  : Common_UI_GTK_State
#endif // GTKGL_SUPPORT
{
  ARDrone_GtkCBData_Base ()
#if defined (GTKGL_SUPPORT)
   : Common_UI_GTK_GLState ()
#else
   : Common_UI_GTK_State ()
#endif // GTKGL_SUPPORT
   , controller (NULL)
   , enableVideo (ARDRONE_DEFAULT_VIDEO_DISPLAY)
   , eventStack ()
   , frameCounter (0)
   , localSAP ()
#if defined (ACE_WIN32) || defined (ACE_WIN64)
   , mediaFramework (MODULE_LIB_DEFAULT_MEDIAFRAMEWORK)
#endif // ACE_WIN32 || ACE_WIN64
   , messages ()
   , messageAllocator (NULL)
#if defined (GTKGL_SUPPORT)
   , openGLModelListId (0)
   , openGLRefreshId (0)
   , openGLScene ()
#endif // GTKGL_SUPPORT
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
   , pixelBuffer (NULL)
#endif // ACE_WIN32 || ACE_WIN64
   , progressData ()
   , stateEventId (0)
   , streams ()
   , timeStamp (ACE_Time_Value::zero)
   , videoMode (ARDRONE_DEFAULT_VIDEO_MODE)
  {
#if defined (GTKGL_SUPPORT)
//    resetCamera ();
#endif // GTKGL_SUPPORT
  }

#if defined (GTKGL_SUPPORT)
//  void resetCamera ()
//  {
//    ACE_OS::memset (&openGLScene, 0, sizeof (openGLScene));
//    openGLScene.camera.zoom = ARDRONE_OPENGL_CAMERA_DEFAULT_ZOOM;
//  };
#endif // GTKGL_SUPPORT

  ARDrone_IController*            controller;
  // *NOTE*: on the host ("server"), use the device bias registers instead !
  // *TODO*: implement a client->server protocol to do this
  //struct ARDrone_SensorBias clientSensorBias; // client side ONLY (!)
  bool                            enableVideo;
  ARDrone_Events_t                eventStack;
  unsigned int                    frameCounter;
  ACE_INET_Addr                   localSAP;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  enum Stream_MediaFramework_Type mediaFramework;
#endif // ACE_WIN32 || ACE_WIN64
  ARDrone_Messages_t              messages;
  ARDrone_MessageAllocator_t*     messageAllocator;
#if defined (GTKGL_SUPPORT)
  GLuint                          openGLModelListId;
  guint                           openGLRefreshId;
  struct Common_GL_Scene          openGLScene;
#endif // GTKGL_SUPPORT
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  GdkPixbuf*                      pixelBuffer;
#endif // ACE_WIN32 || ACE_WIN64
  struct ARDrone_GtkProgressData  progressData;
  guint                           stateEventId;
  ARDrone_Streams_t               streams;
  ACE_Time_Value                  timeStamp;
  enum ARDrone_VideoMode          videoMode;
};
#if defined (_MSC_VER)
#pragma pack (pop)
#endif // _MSC_VER

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#if defined (_MSC_VER)
#pragma pack (push, 1)
#endif // _MSC_VER
struct ARDrone_DirectShow_GtkCBData
 : ARDrone_GtkCBData_Base
{
  ARDrone_DirectShow_GtkCBData ()
   : ARDrone_GtkCBData_Base ()
   , configuration (NULL)
  {}

  struct ARDrone_DirectShow_Configuration* configuration;
};
#if defined (_MSC_VER)
#pragma pack (pop)
#endif // _MSC_VER

struct ARDrone_MediaFoundation_GtkCBData
 : ARDrone_GtkCBData_Base
{
  ARDrone_MediaFoundation_GtkCBData ()
   : ARDrone_GtkCBData_Base ()
   , configuration (NULL)
  {}

  struct ARDrone_MediaFoundation_Configuration* configuration;
};
#else
struct ARDrone_GtkCBData
 : ARDrone_GtkCBData_Base
{
  ARDrone_GtkCBData ()
   : ARDrone_GtkCBData_Base ()
   , configuration (NULL)
  {}

  struct ARDrone_Configuration* configuration;
};
#endif // ACE_WIN32 || ACE_WIN64

struct ARDrone_ThreadData
{
  ARDrone_ThreadData ()
   : CBData (NULL)
   , eventSourceId (0)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
   , mediaFramework (MODULE_LIB_DEFAULT_MEDIAFRAMEWORK)
#endif // ACE_WIN32 || ACE_WIN64
  {}

  struct ARDrone_GtkCBData_Base*  CBData;
  guint                           eventSourceId;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  enum Stream_MediaFramework_Type mediaFramework;
#endif // ACE_WIN32 || ACE_WIN64
};

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Common_UI_GtkBuilderDefinition_T<struct ARDrone_DirectShow_GtkCBData> ARDrone_DirectShow_GtkBuilderDefinition_t;
typedef Common_UI_GTK_Manager_T<ACE_MT_SYNCH,
                                struct ARDrone_DirectShow_GtkCBData> ARDrone_DirectShow_GTK_Manager_t;
typedef ACE_Singleton<ARDrone_DirectShow_GTK_Manager_t,
                      typename ACE_MT_SYNCH::MUTEX> ARDRONE_UI_DIRECTSHOW_GTK_MANAGER_SINGLETON;

typedef Common_UI_GtkBuilderDefinition_T<struct ARDrone_MediaFoundation_GtkCBData> ARDrone_MediaFoundation_GtkBuilderDefinition_t;
typedef Common_UI_GTK_Manager_T<ACE_MT_SYNCH,
                                struct ARDrone_MediaFoundation_GtkCBData> ARDrone_MediaFoundation_GTK_Manager_t;
typedef ACE_Singleton<ARDrone_MediaFoundation_GTK_Manager_t,
                      typename ACE_MT_SYNCH::MUTEX> ARDRONE_UI_MEDIAFOUNDATION_GTK_MANAGER_SINGLETON;
#else
typedef Common_UI_GtkBuilderDefinition_T<struct ARDrone_GtkCBData> ARDrone_GtkBuilderDefinition_t;
typedef Common_UI_GTK_Manager_T<ACE_MT_SYNCH,
                                struct ARDrone_GtkCBData> ARDrone_GTK_Manager_t;
typedef ACE_Singleton<ARDrone_GTK_Manager_t,
                      typename ACE_MT_SYNCH::MUTEX> ARDRONE_UI_GTK_MANAGER_SINGLETON;
#endif // ACE_WIN32 || ACE_WIN64
typedef Common_TaskBase_T<ACE_MT_SYNCH,
                          Common_TimePolicy_t,
                          Common_ILock_T<ACE_MT_SYNCH> > ARDrone_IGTK_Manager_t;

#endif // #ifndef ARDRONE_CONFIGURATION_H
