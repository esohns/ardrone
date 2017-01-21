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

#include "ardrone_network.h"
#include "ardrone_stream_common.h"
#include "ardrone_types.h"

// forward declarations
class ARDrone_Message;

struct _AMMediaType;
class ACE_Message_Queue_Base;
struct ARDrone_DirectShow_PinConfiguration
{
  inline ARDrone_DirectShow_PinConfiguration ()
    : bufferSize (ARDRONE_STREAM_BUFFER_SIZE)
    , format (NULL)
    , queue (NULL)
  {};

  size_t                  bufferSize; // medial sample-
  struct _AMMediaType*    format; // (preferred) media type handle
  ACE_Message_Queue_Base* queue;  // (inbound) buffer queue handle
};
struct ARDrone_DirectShow_FilterConfiguration
{
  inline ARDrone_DirectShow_FilterConfiguration ()
    : format (NULL)
    , module (NULL)
    , pinConfiguration (NULL)
  {};

  // *TODO*: specify this as part of the network protocol header/handshake
  struct _AMMediaType*                        format; // handle
  Stream_Module_t*                            module; // handle
  struct ARDrone_DirectShow_PinConfiguration* pinConfiguration; // handle
};

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
   , socketConfiguration ()
   , socketHandlerConfiguration ()
   , connectionConfiguration ()
   , allocatorConfiguration ()
   , moduleConfiguration ()
   , moduleHandlerConfiguration ()
   , directShowFilterConfiguration ()
   , directShowPinConfiguration ()
   , streamConfiguration ()
   , userData (NULL)
  {};

  struct ARDrone_SignalHandlerConfiguration     signalHandlerConfiguration;

  struct Net_SocketConfiguration                socketConfiguration;
  struct ARDrone_SocketHandlerConfiguration     socketHandlerConfiguration;
  struct ARDrone_ConnectionConfiguration        connectionConfiguration;

  struct Stream_AllocatorConfiguration          allocatorConfiguration;
  struct Stream_ModuleConfiguration             moduleConfiguration;
  struct ARDrone_ModuleHandlerConfiguration     moduleHandlerConfiguration;
  struct ARDrone_DirectShow_FilterConfiguration directShowFilterConfiguration;
  struct ARDrone_DirectShow_PinConfiguration    directShowPinConfiguration;
  struct ARDrone_StreamConfiguration            streamConfiguration;

  struct ARDrone_UserData*                      userData;
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
  , stream (NULL)
  //, temperature ()
  //, temperatureIndex (-1)
  , timeStamp (ACE_Time_Value::zero)
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
 ARDrone_StreamBase_t*           stream;
 //gfloat                temperature[ARDRONE_TEMPERATURE_BUFFER_SIZE * 2];
 //int                   temperatureIndex;
 ACE_Time_Value                  timeStamp;
};

struct ARDrone_ThreadData
{
  inline ARDrone_ThreadData ()
   : CBData (NULL)
   , eventSourceID (0)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
   , useMediaFoundation (COMMON_UI_DEFAULT_WIN32_USE_MEDIAFOUNDATION)
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
