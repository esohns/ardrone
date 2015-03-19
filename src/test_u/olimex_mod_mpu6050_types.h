/*  I2C kernel module driver for the Olimex MOD-MPU6050 UEXT module
    (see https://www.olimex.com/Products/Modules/Sensors/MOD-MPU6050/open-source-hardware,
         http://www.invensense.com/mems/gyro/mpu6050.html)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef OLIMEX_MOD_MPU6050_TYPES_H
#define OLIMEX_MOD_MPU6050_TYPES_H

#include <deque>
#include <list>

#include "GL/gl.h"
#include "glm/glm.hpp"

#include "ace/Global_Macros.h"
#include "ace/OS.h"
#include "ace/Synch.h"
#include "ace/Time_Value.h"

#include "gtk/gtk.h"
#include "gtk/gtkgl.h"
#include "glade/glade.h"

#include "common.h"
#include "common_inotify.h"

#include "common_ui_types.h"

#include "olimex_mod_mpu6050_defines.h"

// forward declarations
class Olimex_Mod_MPU6050_Message;

enum Olimex_Mod_MPU6050_MessageType_t
{
  OLIMEX_MOD_MPU6050_MESSAGE_INVALID = -1,
  OLIMEX_MOD_MPU6050_MESSAGE_SENSOR_DATA,
  ///////////////////////////////////////
  OLIMEX_MOD_MPU6050_MESSAGE_MAX
};

enum Olimex_Mod_MPU6050_Event_t
{
  OLIMEX_MOD_MPU6050_EVENT_INVALID = -1,
  OLIMEX_MOD_MPU6050_EVENT_CONNECT,
  OLIMEX_MOD_MPU6050_EVENT_DISCONNECT,
  OLIMEX_MOD_MPU6050_EVENT_MESSAGE,
  ///////////////////////////////////////
  OLIMEX_MOD_MPU6050_EVENT_MAX
};
typedef std::deque<Olimex_Mod_MPU6050_Event_t> Olimex_Mod_MPU6050_Events_t;
typedef Olimex_Mod_MPU6050_Events_t::const_iterator Olimex_Mod_MPU6050_EventsIterator_t;

typedef std::deque<Olimex_Mod_MPU6050_Message*> Olimex_Mod_MPU6050_Messages_t;
typedef Olimex_Mod_MPU6050_Messages_t::const_iterator Olimex_Mod_MPU6050_MessagesIterator_t;

typedef Common_INotify_T<Olimex_Mod_MPU6050_Message> Olimex_Mod_MPU6050_Notification_t;
typedef std::list<Olimex_Mod_MPU6050_Notification_t*> Olimex_Mod_MPU6050_Subscribers_t;
typedef Olimex_Mod_MPU6050_Subscribers_t::iterator Olimex_Mod_MPU6050_SubscribersIterator_t;

struct camera_t
{
  //glm::vec3 position;
  //glm::vec3 looking_at;
  //glm::vec3 up;

  float zoom;
  glm::vec3 rotation;
  glm::vec3 translation;
  int last[2];
};

struct Olimex_Mod_MPU6050_GtkCBData_t
{
 inline Olimex_Mod_MPU6050_GtkCBData_t ()
 : argc (0)
 , argv (NULL)
 , contextId (0)
//  , eventQueue ()
//  , eventSourceIds ()
 , frameCounter (0)
 , lock (NULL, NULL)
//  , messageQueue ()
 , openGLAxesListId (0)
//  , openGLCamera ()
 , openGLContext (NULL)
 , openGLDrawable (NULL)
 , openGLRefreshTimerId (0)
 , openGLDoubleBuffered (OLIMEX_MOD_MPU6050_OPENGL_DOUBLE_BUFFERED)
//  , subscribers ()
 , timestamp (ACE_Time_Value::zero)
 , XML (NULL)
 {
   resetCamera ();
 };

 inline void resetCamera ()
 {
   ACE_OS::memset (&openGLCamera, 0, sizeof (openGLCamera));
   openGLCamera.zoom = OLIMEX_MOD_MPU6050_OPENGL_CAMERA_DEFAULT_ZOOM;
 };

 int                                argc;
 ACE_TCHAR**                        argv;
 guint                              contextId; // status bar context
 Olimex_Mod_MPU6050_Events_t        eventQueue;
 Common_UI_GTK_EventSourceIDs_t     eventSourceIds;
 unsigned int                       frameCounter;
 mutable ACE_Recursive_Thread_Mutex lock;
 Olimex_Mod_MPU6050_Messages_t      messageQueue;
 GLuint                             openGLAxesListId;
 camera_t                           openGLCamera;
 GdkGLContext*                      openGLContext;
 GdkGLDrawable*                     openGLDrawable;
 guint                              openGLRefreshTimerId;
 bool                               openGLDoubleBuffered;
 Olimex_Mod_MPU6050_Subscribers_t   subscribers;
 ACE_Time_Value                     timestamp;
 GladeXML*                          XML;
};

#endif // #ifndef OLIMEX_MOD_MPU6050_TYPES_H
