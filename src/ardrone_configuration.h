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

#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
#if defined (GTKGL_SUPPORT)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <gl/GL.h>
#else
#include <GL/gl.h>
#endif // ACE_WIN32 || ACE_WIN64
#endif /* GTKGL_SUPPORT */
#endif /* GTK_USE */
#endif /* GUI_SUPPORT */

#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
#include "gtk/gtk.h"
#endif // GTK_USE
#endif /* GUI_SUPPORT */

#include "ace/Singleton.h"
#include "ace/Synch_Traits.h"

#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
#if defined (GTKGL_SUPPORT)
#include "common_gl_common.h"
#endif /* GTKGL_SUPPORT */
#endif /* GTK_USE */
#endif /* GUI_SUPPORT */

#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
#include "common_ui_gtk_builder_definition.h"
#include "common_ui_gtk_manager.h"

#if defined (GTKGL_SUPPORT)
#include "common_ui_gtk_gl_common.h"
#endif /* GTKGL_SUPPORT */
#elif defined (WXWIDGETS_USE)
#include "common_ui_wxwidgets_common.h"
#include "common_ui_wxwidgets_iapplication.h"
#endif
#endif /* GUI_SUPPORT */

#include "common_test_u_common.h"

#include "stream_common.h"
#include "stream_configuration.h"

#include "stream_dec_defines.h"

#include "stream_lib_common.h"
#include "stream_lib_defines.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "stream_lib_directdraw_common.h"
#endif // ACE_WIN32 || ACE_WIN64

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

struct ARDrone_Configuration_Base
{
  ARDrone_Configuration_Base ()
   : allocatorConfiguration ()
#if defined (ACE_WIN32) || defined (ACE_WIN64)
   , direct3DConfiguration ()
#endif // ACE_WIN32 || ACE_WIN64
   , dispatchConfiguration ()
   , parserConfiguration ()
   , streamSubscribers ()
   , streamSubscribersLock ()
   , WLANMonitorConfiguration ()
   , userData (NULL)
  {}

  struct ARDrone_AllocatorConfiguration               allocatorConfiguration;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct Stream_MediaFramework_Direct3D_Configuration direct3DConfiguration;
#endif // ACE_WIN32 || ACE_WIN64
  struct Common_EventDispatchConfiguration            dispatchConfiguration;
  struct Common_ParserConfiguration                   parserConfiguration;
  ARDrone_Subscribers_t                               streamSubscribers;
  ACE_SYNCH_RECURSIVE_MUTEX                           streamSubscribersLock;
  struct Net_WLAN_MonitorConfiguration                WLANMonitorConfiguration;

  struct ARDrone_UserData*                            userData;
};

#if defined (ACE_WIN32) || defined (ACE_WIN64)
struct ARDrone_DirectShow_Configuration
 : ARDrone_Configuration_Base
{
  ARDrone_DirectShow_Configuration ()
   : ARDrone_Configuration_Base ()
   , connectionConfigurations ()
   , filterConfiguration ()
   , pinConfiguration ()
   , signalConfiguration ()
   , streamConfigurations ()
  {
    filterConfiguration.pinConfiguration = &pinConfiguration;
  }

  ARDrone_DirectShow_ConnectionConfigurations_t                  connectionConfigurations;
  struct ARDrone_DirectShow_FilterConfiguration                  filterConfiguration;
  struct Stream_MediaFramework_DirectShow_FilterPinConfiguration pinConfiguration;
  struct ARDrone_DirectShow_SignalConfiguration                  signalConfiguration;
  ARDrone_DirectShow_StreamConfigurations_t                      streamConfigurations;
};

struct ARDrone_MediaFoundation_Configuration
 : ARDrone_Configuration_Base
{
  ARDrone_MediaFoundation_Configuration ()
   : ARDrone_Configuration_Base ()
   , connectionConfigurations ()
   , signalConfiguration ()
   , streamConfigurations ()
  {}

  ARDrone_MediaFoundation_ConnectionConfigurations_t connectionConfigurations;
  struct ARDrone_MediaFoundation_SignalConfiguration signalConfiguration;
  ARDrone_MediaFoundation_StreamConfigurations_t     streamConfigurations;
};
#else
struct ARDrone_Configuration
 : ARDrone_Configuration_Base
{
  ARDrone_Configuration ()
   : ARDrone_Configuration_Base ()
   , connectionConfigurations ()
   , signalConfiguration ()
   , streamConfigurations ()
  {}

  ARDrone_ConnectionConfigurations_t connectionConfigurations;
  struct ARDrone_SignalConfiguration signalConfiguration;
  ARDrone_StreamConfigurations_t     streamConfigurations;
};
#endif // ACE_WIN32 || ACE_WIN64

typedef std::deque<ARDrone_Message*> ARDrone_Messages_t;
typedef ARDrone_Messages_t::const_iterator ARDrone_MessagesIterator_t;

typedef std::map<std::string,
                 Stream_Base_t*> ARDrone_Streams_t;
typedef ARDrone_Streams_t::const_iterator ARDrone_StreamsIterator_t;

//////////////////////////////////////////

#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
struct ARDrone_UI_GTK_State
 : Common_UI_GTK_State_t
{
  ARDrone_UI_GTK_State ()
   : Common_UI_GTK_State_t ()
   , eventStack ()
  {}

  ARDrone_Events_t eventStack;
};
#elif (WXWIDGETS_USE)
struct ARDrone_UI_wxWidgets_State
 : Common_UI_wxWidgets_State
{
  ARDrone_UI_wxWidgets_State ()
   : Common_UI_wxWidgets_State ()
   , eventStack ()
  {}

  ARDrone_Events_t eventStack;
};
#endif

struct ARDrone_UI_ProgressData
#if defined (GTK_USE)
 : Common_UI_GTK_ProgressData
#elif defined (WXWIDGETS_USE)
 : Common_UI_wxWidgets_ProgressData
#endif
{
  ARDrone_UI_ProgressData ()
#if defined (GTK_USE)
   : Common_UI_GTK_ProgressData ()
#elif defined (WXWIDGETS_USE)
   : Common_UI_wxWidgets_ProgressData ()
#endif
   , state (NULL)
   , statistic ()
  {}

#if defined (GTK_USE)
  struct ARDrone_UI_GTK_State*       state;
#elif defined (WXWIDGETS_USE)
  struct ARDrone_UI_wxWidgets_State* state;
#endif // WXWIDGETS_USE
  struct ARDrone_Statistic           statistic;
};

//#if defined (_MSC_VER)
//#pragma pack (push, 1)
//#endif // _MSC_VER
struct ARDrone_UI_CBData_Base
 : Common_Test_U_UI_CBData
{
  ARDrone_UI_CBData_Base ()
   : Common_Test_U_UI_CBData ()
   , controller (NULL)
   , enableVideo (ARDRONE_DEFAULT_VIDEO_DISPLAY)
   , frameCounter (0)
   , localSAP ()
#if defined (ACE_WIN32) || defined (ACE_WIN64)
   , mediaFramework (STREAM_LIB_DEFAULT_MEDIAFRAMEWORK)
#endif // ACE_WIN32 || ACE_WIN64
   , messages ()
   , messageAllocator (NULL)
#if ((defined (GTK_USE) && defined (GTKGL_SUPPORT)) || (defined (WXWIDGETS_USE) && defined (WXWIDGETS_GL_SUPPORT)))
   , openGLModelListId (0)
   , openGLRefreshId (0)
   , openGLScene ()
#endif
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
//#if defined (GTK_USE)
//#if defined (GTKGL_SUPPORT)
//    resetCamera ();
//#endif // GTKGL_SUPPORT
//#endif // GTK_USE
  }

//#if defined (GTK_USE)
//#if defined (GTKGL_SUPPORT)
//  void resetCamera ()
//  {
//    ACE_OS::memset (&openGLScene, 0, sizeof (openGLScene));
//    openGLScene.camera.zoom = ARDRONE_OPENGL_CAMERA_DEFAULT_ZOOM;
//  };
//#endif // GTKGL_SUPPORT
//#endif // GTK_USE

  ARDrone_IController*            controller;
  // *NOTE*: on the host ("server"), use the device bias registers instead !
  // *TODO*: implement a client->server protocol to do this
  //struct ARDrone_SensorBias clientSensorBias; // client side ONLY (!)
  bool                            enableVideo;
  unsigned int                    frameCounter;
  ACE_INET_Addr                   localSAP;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  enum Stream_MediaFramework_Type mediaFramework;
#endif // ACE_WIN32 || ACE_WIN64
  ARDrone_Messages_t              messages;
  ARDrone_MessageAllocator_t*     messageAllocator;
#if ((defined (GTK_USE) && defined (GTKGL_SUPPORT)) || (defined (WXWIDGETS_USE) && defined (WXWIDGETS_GL_SUPPORT)))
  GLuint                          openGLModelListId;
  guint                           openGLRefreshId;
  struct Common_GL_Scene          openGLScene;
#endif // GTKGL_SUPPORT
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  GdkPixbuf*                      pixelBuffer;
#endif // ACE_WIN32 || ACE_WIN64
  struct ARDrone_UI_ProgressData  progressData;
  guint                           stateEventId;
  ARDrone_Streams_t               streams;
  ACE_Time_Value                  timeStamp;
  enum ARDrone_VideoMode          videoMode;
};
//#if defined (_MSC_VER)
//#pragma pack (pop)
//#endif // _MSC_VER

#if defined (ACE_WIN32) || defined (ACE_WIN64)
//#if defined (_MSC_VER)
//#pragma pack (push, 1)
//#endif // _MSC_VER
struct ARDrone_DirectShow_UI_CBData
 : ARDrone_UI_CBData_Base
{
  ARDrone_DirectShow_UI_CBData ()
   : ARDrone_UI_CBData_Base ()
   , configuration (NULL)
  {}

  struct ARDrone_DirectShow_Configuration* configuration;
};
//#if defined (_MSC_VER)
//#pragma pack (pop)
//#endif // _MSC_VER

struct ARDrone_MediaFoundation_UI_CBData
 : ARDrone_UI_CBData_Base
{
  ARDrone_MediaFoundation_UI_CBData ()
   : ARDrone_UI_CBData_Base ()
   , configuration (NULL)
  {}

  struct ARDrone_MediaFoundation_Configuration* configuration;
};
#else
struct ARDrone_UI_CBData
 : ARDrone_UI_CBData_Base
{
  ARDrone_UI_CBData ()
   : ARDrone_UI_CBData_Base ()
   , configuration (NULL)
  {}

  struct ARDrone_Configuration* configuration;
};
#endif // ACE_WIN32 || ACE_WIN64
#endif // GUI_SUPPORT

struct ARDrone_ThreadData
 : Common_Test_U_ThreadData
{
  ARDrone_ThreadData ()
   : Common_Test_U_ThreadData ()
#if defined (GUI_SUPPORT)
   , CBData (NULL)
#endif // GUI_SUPPORT
#if defined (ACE_WIN32) || defined (ACE_WIN64)
   , mediaFramework (STREAM_LIB_DEFAULT_MEDIAFRAMEWORK)
#endif // ACE_WIN32 || ACE_WIN64
  {}

#if defined (GUI_SUPPORT)
  struct ARDrone_UI_CBData_Base*  CBData;
#endif // GUI_SUPPORT
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  enum Stream_MediaFramework_Type mediaFramework;
#endif // ACE_WIN32 || ACE_WIN64
};

//////////////////////////////////////////

#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
typedef Common_UI_GtkBuilderDefinition_T<struct ARDrone_UI_GTK_State> ARDrone_GtkBuilderDefinition_t;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Common_UI_GTK_Manager_T<ACE_MT_SYNCH,
                                Common_UI_GTK_Configuration_t,
                                struct ARDrone_UI_GTK_State,
                                struct ARDrone_DirectShow_UI_CBData> ARDrone_DirectShow_GTK_Manager_t;
typedef ACE_Singleton<ARDrone_DirectShow_GTK_Manager_t,
                      ACE_MT_SYNCH::MUTEX> ARDRONE_DIRECTSHOW_GTK_MANAGER_SINGLETON;
typedef Common_UI_GTK_Manager_T<ACE_MT_SYNCH,
                                Common_UI_GTK_Configuration_t,
                                struct ARDrone_UI_GTK_State,
                                struct ARDrone_MediaFoundation_UI_CBData> ARDrone_MediaFoundation_GTK_Manager_t;
typedef ACE_Singleton<ARDrone_MediaFoundation_GTK_Manager_t,
                      ACE_MT_SYNCH::MUTEX> ARDRONE_MEDIAFOUNDATION_GTK_MANAGER_SINGLETON;
#else
typedef Common_UI_GTK_Manager_T<ACE_MT_SYNCH,
                                Common_UI_GTK_Configuration_t,
                                struct ARDrone_UI_GTK_State,
                                struct ARDrone_UI_CBData> ARDrone_UI_GTK_Manager_t;
typedef ACE_Singleton<ARDrone_UI_GTK_Manager_t,
                      ACE_MT_SYNCH::MUTEX> ARDRONE_GTK_MANAGER_SINGLETON;
#endif // ACE_WIN32 || ACE_WIN64

//////////////////////////////////////////


#elif defined (WXWIDGETS_USE)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Common_UI_wxWidgets_IApplicationBase_T<struct ARDrone_UI_wxWidgets_State> ARDrone_UI_wxWidgets_IApplicationBase_t;
typedef Common_UI_wxWidgets_IApplication_T<struct ARDrone_UI_wxWidgets_State,
                                           struct ARDrone_DirectShow_UI_CBData> ARDrone_DirectShow_WxWidgetsIApplication_t;
typedef Common_UI_wxWidgets_IApplication_T<struct ARDrone_UI_wxWidgets_State,
                                           struct ARDrone_MediaFoundation_UI_CBData> ARDrone_MediaFoundation_WxWidgetsIApplication_t;
#else
typedef Common_UI_wxWidgets_IApplication_T<struct ARDrone_UI_wxWidgets_State,
                                           struct ARDrone_UI_CBData> ARDrone_WxWidgetsIApplication_t;
#endif // ACE_WIN32 || ACE_WIN64
#endif // WXWIDGETS_USE
#endif // GUI_SUPPORT

#endif // #ifndef ARDRONE_CONFIGURATION_H
