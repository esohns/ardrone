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

#if defined (GTKGL_SUPPORT)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <gl/GL.h>
#else
#include <GL/gl.h>
#endif
#endif /* GTKGL_SUPPORT */

#include <ace/Singleton.h>
#include <ace/Synch_Traits.h>

#include "common_ui_gtk_builder_definition.h"
#include "common_ui_gtk_common.h"
#include "common_ui_gtk_manager.h"

#include "stream_common.h"

#include "net_configuration.h"

#include "ardrone_defines.h"
#include "ardrone_network.h"
#include "ardrone_stream_common.h"
#include "ardrone_types.h"

// forward declarations
class ARDrone_Message;

struct ARDrone_AllocatorConfiguration
 : Stream_AllocatorConfiguration
{
  inline ARDrone_AllocatorConfiguration ()
   : Stream_AllocatorConfiguration ()
  {
    defaultBufferSize = ARDRONE_MESSAGE_BUFFER_SIZE;

    // *NOTE*: facilitate (message block) data buffers to be scanned with
    //         (f)lexs' yy_scan_buffer() method, and support 'padding' in ffmpeg
    paddingBytes =
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      AV_INPUT_BUFFER_PADDING_SIZE;
#else
      FF_INPUT_BUFFER_PADDING_SIZE;
#endif
    paddingBytes =
      std::max (static_cast<unsigned int> (STREAM_DECODER_FLEX_BUFFER_BOUNDARY_SIZE),
                paddingBytes);
  };
};

#if defined (ACE_WIN32) || defined (ACE_WIN64)
struct _AMMediaType;
//class ACE_Message_Queue_Base;
struct ARDrone_DirectShow_FilterConfiguration
 : Stream_Miscellaneous_DirectShow_FilterConfiguration
{
  inline ARDrone_DirectShow_FilterConfiguration ()
    : Stream_Miscellaneous_DirectShow_FilterConfiguration ()
    //, format (NULL)
    , module (NULL)
    , pinConfiguration (NULL)
  {};

  // *TODO*: specify this as part of the network protocol header/handshake
  //struct _AMMediaType*                                           format; // handle
  Stream_Module_t*                                               module; // handle
  struct Stream_Miscellaneous_DirectShow_FilterPinConfiguration* pinConfiguration; // handle
};
#endif

struct ARDrone_SocketHandlerConfiguration;
typedef Net_IConnector_T<ACE_INET_Addr,
                         struct ARDrone_SocketHandlerConfiguration> ARDrone_IConnector_t;
struct ARDrone_SignalHandlerConfiguration
 : Common_SignalHandlerConfiguration
{
 inline ARDrone_SignalHandlerConfiguration ()
  : Common_SignalHandlerConfiguration ()
  , actionTimerID (-1)
  , consoleMode (false)
  , interfaceHandle (NULL)
  , peerAddress ()
  {};

  long                  actionTimerID;
  bool                  consoleMode;
  ARDrone_IConnector_t* interfaceHandle;
  ACE_INET_Addr         peerAddress;
};

struct ARDrone_Configuration
{
  inline ARDrone_Configuration ()
   : signalHandlerConfiguration ()
   , listenerConfiguration ()
   , socketConfigurations ()
   , socketHandlerConfiguration ()
   , connectionConfiguration ()
   , allocatorConfiguration ()
   , parserConfiguration ()
   , moduleConfiguration ()
   , moduleHandlerConfiguration ()
#if defined (ACE_WIN32) || defined (ACE_WIN64)
   , directShowFilterConfiguration ()
   , directShowPinConfiguration ()
#endif
   , streamConfiguration ()
   , streamSubscribers ()
   , streamSubscribersLock ()
   , userData (NULL)
  {};

  struct ARDrone_SignalHandlerConfiguration                     signalHandlerConfiguration;

  struct Net_ListenerConfiguration                              listenerConfiguration;
  Net_SocketConfigurationStack_t                                socketConfigurations;
  struct ARDrone_SocketHandlerConfiguration                     socketHandlerConfiguration;
  struct ARDrone_ConnectionConfiguration                        connectionConfiguration;

  struct ARDrone_AllocatorConfiguration                         allocatorConfiguration;
  struct Common_ParserConfiguration                             parserConfiguration;
  struct Stream_ModuleConfiguration                             moduleConfiguration;
  struct ARDrone_ModuleHandlerConfiguration                     moduleHandlerConfiguration;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct ARDrone_DirectShow_FilterConfiguration                 directShowFilterConfiguration;
  struct Stream_Miscellaneous_DirectShow_FilterPinConfiguration directShowPinConfiguration;
#endif
  struct ARDrone_StreamConfiguration                            streamConfiguration;
  ARDrone_Subscribers_t                                         streamSubscribers;
  ACE_SYNCH_RECURSIVE_MUTEX                                     streamSubscribersLock;

  struct ARDrone_UserData*                                      userData;
};

//////////////////////////////////////////

typedef std::deque<ARDrone_Message*> ARDrone_Messages_t;
typedef ARDrone_Messages_t::const_iterator ARDrone_MessagesIterator_t;

struct ARDrone_GtkProgressData
 : Common_UI_GTK_ProgressData
{
  inline ARDrone_GtkProgressData ()
   : Common_UI_GTK_ProgressData ()
   , statistic ()
  {};

  Stream_Statistic statistic;
};

struct ARDrone_GtkCBData
 : Common_UI_GTKState
{
 inline ARDrone_GtkCBData ()
  : Common_UI_GTKState ()
  , configuration (NULL)
  , contextIdData (0)
  , contextIdInformation (0)
  , eventStack ()
  , frameCounter (0)
  , localSAP ()
  , MAVLinkStream (NULL)
  , NavDataStream (NULL)
  , messageQueue ()
#if defined (GTKGL_SUPPORT)
  , openGLAxesListId (0)
  , openGLCamera ()
  , openGLRefreshId (0)
  , openGLDoubleBuffered (ARDRONE_OPENGL_DOUBLE_BUFFERED)
#endif
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  , pixelBuffer (NULL)
#endif
  , progressData (NULL)
  //, temperature ()
  //, temperatureIndex (-1)
  , timeStamp (ACE_Time_Value::zero)
  , videoStream (NULL)
 {
#if defined (GTKGL_SUPPORT)
   resetCamera ();
#endif
 };

#if defined (GTKGL_SUPPORT)
 inline void resetCamera ()
 {
   ACE_OS::memset (&openGLCamera, 0, sizeof (openGLCamera));
   openGLCamera.zoom = ARDRONE_OPENGL_CAMERA_DEFAULT_ZOOM;
 };
#endif

 struct ARDrone_Configuration*   configuration;
 // *NOTE*: on the host ("server"), use the device bias registers instead !
 // *TODO*: implement a client->server protocol to do this
 //struct ARDrone_SensorBias clientSensorBias; // client side ONLY (!)
 guint                           contextIdData; // status bar context
 guint                           contextIdInformation; // status bar context
 ARDrone_Events_t                eventStack;
 unsigned int                    frameCounter;
 // *TODO*: let the user choose a NIC instead
 ACE_INET_Addr                   localSAP;
 ARDrone_StreamBase_t*           MAVLinkStream;
 ARDrone_StreamBase_t*           NavDataStream;
 ARDrone_Messages_t              messageQueue;
#if defined (GTKGL_SUPPORT)
 GLuint                          openGLAxesListId;
 struct ARDrone_Camera           openGLCamera;
 guint                           openGLRefreshId;
 bool                            openGLDoubleBuffered;
#endif
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
 GdkPixbuf*                      pixelBuffer;
#endif
 struct ARDrone_GtkProgressData* progressData;
 //gfloat                temperature[ARDRONE_TEMPERATURE_BUFFER_SIZE * 2];
 //int                   temperatureIndex;
 ACE_Time_Value                  timeStamp;
 ARDrone_StreamBase_t*           videoStream;
};

struct ARDrone_ThreadData
{
  inline ARDrone_ThreadData ()
   : CBData (NULL)
   , eventSourceID (0)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
   , useMediaFoundation (COMMON_DEFAULT_WIN32_MEDIA_FRAMEWORK == COMMON_WIN32_FRAMEWORK_MEDIAFOUNDATION)
#endif
  {};

  struct ARDrone_GtkCBData* CBData;
  guint                     eventSourceID;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  bool                      useMediaFoundation;
#endif
};

typedef Common_UI_GtkBuilderDefinition_T<struct ARDrone_GtkCBData> ARDrone_GtkBuilderDefinition_t;

typedef Common_UI_GTK_Manager_T<struct ARDrone_GtkCBData> ARDrone_GTK_Manager_t;
typedef ACE_Singleton<ARDrone_GTK_Manager_t,
                      typename ACE_MT_SYNCH::RECURSIVE_MUTEX> ARDRONE_UI_GTK_MANAGER_SINGLETON;

#endif // #ifndef ARDRONE_CONFIGURATION_H
