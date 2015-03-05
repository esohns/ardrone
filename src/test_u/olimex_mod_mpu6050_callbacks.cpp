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
#include "stdafx.h"

#include "olimex_mod_mpu6050_callbacks.h"

#include "ace/Log_Msg.h"

#include "GL/gl.h"
#include "GL/glu.h"
#include "GL/glut.h"

#include "gmodule.h"

#include "gdk/gdkkeysyms.h"
#include "gtk/gtkgl.h"

#include "common_timer_manager.h"

#include "olimex_mod_mpu6050_defines.h"
#include "olimex_mod_mpu6050_macros.h"
#include "olimex_mod_mpu6050_message.h"
#include "olimex_mod_mpu6050_opengl.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
G_MODULE_EXPORT gboolean
idle_initialize_UI_cb (gpointer userData_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("::idle_initialize_UI_cb"));

  Olimex_Mod_MPU6050_GtkCBData_t* cb_data_p =
      static_cast<Olimex_Mod_MPU6050_GtkCBData_t*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (cb_data_p);

  GtkWindow* main_window_p =
      GTK_WINDOW (glade_xml_get_widget (cb_data_p->XML,
                                        ACE_TEXT_ALWAYS_CHAR (OLIMEX_MOD_MPU6050_UI_WIDGET_NAME_WINDOW_MAIN)));
  ACE_ASSERT (main_window_p);
  gtk_window_set_default_size (main_window_p,
                               OLIMEX_MOD_MPU6050_DEFAULT_UI_WIDGET_WINDOW_MAIN_SIZE_WIDTH,
                               OLIMEX_MOD_MPU6050_DEFAULT_UI_WIDGET_WINDOW_MAIN_SIZE_HEIGHT);

  GtkDialog* about_dialog_p =
      GTK_DIALOG (glade_xml_get_widget (cb_data_p->XML,
                                        ACE_TEXT_ALWAYS_CHAR (OLIMEX_MOD_MPU6050_UI_WIDGET_NAME_DIALOG_ABOUT)));
  ACE_ASSERT (about_dialog_p);

  GtkWidget* drawing_area_p =
      glade_xml_get_widget (cb_data_p->XML,
                            ACE_TEXT_ALWAYS_CHAR (OLIMEX_MOD_MPU6050_UI_WIDGET_NAME_DRAWING_AREA));
  ACE_ASSERT (drawing_area_p);

////  GtkCBData_->drawingArea = gtk_drawing_area_new ();
////  ACE_ASSERT (GtkCBData_->drawingArea);
////  gtk_widget_set_name (GtkCBData_->drawingArea,
////                       ACE_TEXT_ALWAYS_CHAR (OLIMEX_MOD_MPU6050_UI_WIDGET_NAME_DRAWING_AREA));
//  gtk_widget_set_events (GtkCBData_->drawingArea,
//                         GDK_EXPOSURE_MASK);
//  gtk_container_add (GTK_CONTAINER (opengl_container),
//                     GtkCBData_->drawingArea);

  GtkStatusbar* status_bar_p =
      GTK_STATUSBAR (glade_xml_get_widget (cb_data_p->XML,
                                           ACE_TEXT_ALWAYS_CHAR (OLIMEX_MOD_MPU6050_UI_WIDGET_NAME_STATUS_BAR)));
  ACE_ASSERT (status_bar_p);
  cb_data_p->contextId =
      gtk_statusbar_get_context_id (status_bar_p,
                                    ACE_TEXT_ALWAYS_CHAR (OLIMEX_MOD_MPU6050_UI_WIDGET_STATUS_BAR_CONTEXT));

  // step2: (auto-)connect signals/slots
  // *NOTE*: glade_xml_signal_autoconnect doesn't work reliably
  //glade_xml_signal_autoconnect(xml);
  // step2a: connect default signals
  // *NOTE*: glade_xml_signal_connect_data doesn't work reliably
//  g_signal_connect_swapped (G_OBJECT (main_window_p),
//                            ACE_TEXT_ALWAYS_CHAR ("destroy"),
//                            G_CALLBACK (gtk_main_quit),
//                            NULL);
  g_signal_connect (G_OBJECT (main_window_p),
                    ACE_TEXT_ALWAYS_CHAR ("destroy"),
                    G_CALLBACK (quit_clicked_GTK_cb),
                    cb_data_p);
//                   G_CALLBACK(gtk_widget_destroyed),
//                   &main_dialog_p,
//  g_signal_connect (G_OBJECT (main_window_p),
//                    ACE_TEXT_ALWAYS_CHAR ("delete-event"),
//                    G_CALLBACK (delete_event_cb),
//                    NULL);
  GtkWidget* widget_p =
      GTK_WIDGET (glade_xml_get_widget (cb_data_p->XML,
                                        ACE_TEXT_ALWAYS_CHAR (OLIMEX_MOD_MPU6050_UI_WIDGET_NAME_MENU_FILE_QUIT)));
  ACE_ASSERT (widget_p);
  g_signal_connect (G_OBJECT (widget_p),
                    ACE_TEXT_ALWAYS_CHAR ("activate"),
                    G_CALLBACK (quit_clicked_GTK_cb),
                    cb_data_p);

  widget_p =
      GTK_WIDGET (glade_xml_get_widget (cb_data_p->XML,
                                        ACE_TEXT_ALWAYS_CHAR (OLIMEX_MOD_MPU6050_UI_WIDGET_NAME_MENU_HELP_ABOUT)));
  ACE_ASSERT (widget_p);
  g_signal_connect (G_OBJECT (widget_p),
                    ACE_TEXT_ALWAYS_CHAR ("activate"),
                    G_CALLBACK (about_clicked_GTK_cb),
                    cb_data_p);
  g_signal_connect_swapped (G_OBJECT (about_dialog_p),
                            ACE_TEXT_ALWAYS_CHAR ("response"),
                            G_CALLBACK (gtk_widget_hide),
                            about_dialog_p);

  // step3a: initialize OpenGL
  if (!gtk_gl_init_check (&cb_data_p->argc, &cb_data_p->argv))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_gl_init_check(): \"%m\", aborting\n")));
    return FALSE; // G_SOURCE_REMOVE
  } // end IF

  int mode = (GDK_GL_MODE_RGBA  |
              GDK_GL_MODE_DEPTH);
  if (cb_data_p->openGLDoubleBuffered)
    mode |= GDK_GL_MODE_DOUBLE;
  GdkGLConfig* configuration_p =
      gdk_gl_config_new_by_mode (static_cast<GdkGLConfigMode> (mode));
  if (!configuration_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gdk_gl_config_new_by_mode(): \"%m\", aborting\n")));
    return FALSE; // G_SOURCE_REMOVE
  } // end IF

  if (!gtk_widget_set_gl_capability (drawing_area_p,    // (container) widget
                                     configuration_p,   // GdkGLConfig: configuration
                                     NULL,              // GdkGLContext: share list
                                     TRUE,              // direct rendering ?
                                     GDK_GL_RGBA_TYPE)) // render_type
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_widget_set_gl_capability(): \"%m\", aborting\n")));

    // clean up
    g_free (configuration_p);

    return FALSE; // G_SOURCE_REMOVE
  } // end IF
  // *TODO*: free configuration_p ?

  // step3b: initialize GLUT
  glutInit (&cb_data_p->argc, cb_data_p->argv);

  // step4: connect custom signals
  g_signal_connect (drawing_area_p,
                    ACE_TEXT_ALWAYS_CHAR ("button-press-event"),
                    G_CALLBACK (button_cb),
                    cb_data_p);
  g_signal_connect (drawing_area_p,
                    ACE_TEXT_ALWAYS_CHAR ("configure-event"),
                    G_CALLBACK (configure_cb),
                    cb_data_p);
  g_signal_connect (drawing_area_p,
                    ACE_TEXT_ALWAYS_CHAR ("expose-event"),
                    G_CALLBACK (expose_cb),
                    cb_data_p);
  g_signal_connect (drawing_area_p,
                    ACE_TEXT_ALWAYS_CHAR ("key-press-event"),
                    G_CALLBACK (key_cb),
                    cb_data_p);

//  // step5: use correct screen
//  if (parentWidget_in)
//    gtk_window_set_screen (GTK_WINDOW (dialog),
//                           gtk_widget_get_screen (const_cast<GtkWidget*> (//parentWidget_in)));

  // step6: draw main window
  gtk_widget_show_all (GTK_WIDGET (main_window_p));

  // step7: init fps, schedule refresh
  cb_data_p->timestamp = COMMON_TIME_POLICY ();
  guint opengl_refresh_rate =
      static_cast<guint> (OLIMEX_MOD_MPU6050_UI_WIDGET_GL_REFRESH_INTERVAL);

  {
    cb_data_p->openGLRefreshTimerId = g_timeout_add (opengl_refresh_rate,
                                                     process_cb,
                                                     cb_data_p);
  } // end lock scope
  if (!cb_data_p->openGLRefreshTimerId)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_widget_set_gl_capability(): \"%m\", aborting\n")));
    return FALSE; // G_SOURCE_REMOVE
  } // end IF

  // one-shot action
  return FALSE; // G_SOURCE_REMOVE
}

G_MODULE_EXPORT gboolean
idle_finalize_UI_cb (gpointer userData_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("::idle_finalize_UI_cb"));

  Olimex_Mod_MPU6050_GtkCBData_t* cb_data_p =
      static_cast<Olimex_Mod_MPU6050_GtkCBData_t*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (cb_data_p);

  if (cb_data_p->openGLRefreshTimerId)
  {
    g_source_remove (cb_data_p->openGLRefreshTimerId);
    cb_data_p->openGLRefreshTimerId = 0;
  } // end iF

  if (glIsList (cb_data_p->openGLAxesListId))
  {
    glDeleteLists (cb_data_p->openGLAxesListId, 1);
    cb_data_p->openGLAxesListId = 0;
  } // end IF

  gtk_main_quit ();

  // one-shot action
  return FALSE; // G_SOURCE_REMOVE
}

/////////////////////////////////////////

G_MODULE_EXPORT gboolean
button_cb (GtkWidget* widget_in,
           GdkEventButton* event_in,
           gpointer userData_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("::button_cb"));

  ACE_UNUSED_ARG (event_in);
  ACE_UNUSED_ARG (userData_in);
//  Olimex_Mod_MPU6050_GtkCBData_t* cb_data_p =
//      reinterpret_cast<Olimex_Mod_MPU6050_GtkCBData_t*> (userData_in);

//  // sanity check(s)
//  ACE_ASSERT (cb_data_p);

  gtk_widget_grab_focus (widget_in);

  return TRUE; // done (do not propagate further)
}

G_MODULE_EXPORT gboolean
configure_cb (GtkWidget* widget_in,
              GdkEventConfigure* event_in,
              gpointer userData_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("::configure_cb"));

  Olimex_Mod_MPU6050_GtkCBData_t* cb_data_p =
      reinterpret_cast<Olimex_Mod_MPU6050_GtkCBData_t*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (widget_in);
  ACE_ASSERT (cb_data_p);
//  ACE_ASSERT (!cb_data_p->openGLContext);
//  ACE_ASSERT (!cb_data_p->openGLDrawable);

  cb_data_p->openGLContext = gtk_widget_get_gl_context (widget_in);
  ACE_ASSERT (cb_data_p->openGLContext);
  cb_data_p->openGLDrawable = gtk_widget_get_gl_drawable (widget_in);
  ACE_ASSERT (cb_data_p->openGLDrawable);

  if (!gdk_gl_drawable_gl_begin (cb_data_p->openGLDrawable,
                                 cb_data_p->openGLContext))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gdk_gl_drawable_gl_begin(), aborting\n")));
    return FALSE; // propagate
  } // end IF

  if (!cb_data_p->openGLAxesListId)
  {
    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard (cb_data_p->lock);

    cb_data_p->openGLAxesListId = ::axes ();
    if (!glIsList (cb_data_p->openGLAxesListId))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ::axes (): \"%m\", aborting\n")));
      return FALSE; // G_SOURCE_REMOVE
    } // end IF
  } // end lock scope

  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();

  /* specify the lower left corner, as well as width/height of the viewport */
  GtkAllocation allocation;
  gtk_widget_get_allocation (widget_in, &allocation);
  glViewport (0, 0, allocation.width, allocation.height);

  gluPerspective (60.0,
                  (allocation.width / allocation.height),
                  1.0,
                  100.0); // setup a perspective projection

  glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();

  // set up the camera
  // *NOTE*: standard "right-hand" coordinate system (RHCS) is:
  //         x (thumb): right, y (index): up, z (middle finger): towards you
//  // top-down (RHCS: x --> up [y --> towards you, z --> right]
  gluLookAt (0.0, 10.0, 0.0, // eye position (*NOTE*: relative to )
             0.0, 0.0, 0.0,  // looking-at position (RHCS notation)
             1.0, 0.0, 0.0); // up direction (RHCS notation, relative to eye
                             // position and looking-at direction)
  // behind (RHCS: -z --> up [y --> right,
//  gluLookAt (-10.0, 0.0, 0.0, // eye position (*NOTE*: relative to standard
//                              //                       "right-hand" coordinate
//                              //                       system [RHCS])
//             0.0, 0.0, 0.0,   // looking-at position (RHCS notation)
//             0.0, 0.0, -1.0); // up direction (RHCS notation, relative to eye
//                              // position and looking-at direction)
  // *NOTE*: in order for this to work with the device, the modelview CS needs
  //         to be switched to "left-hand" coordinate system as well
//  glScalef (1.0F, 1.0F, -1.0F);

  GLfloat light_ambient[] = {1.0F, 1.0F, 1.0F, 1.0F};
//  GLfloat light_diffuse[] = {1.0F, 1.0F, 1.0F, 1.0F};
  GLfloat light0_position[] = {5.0F, 5.0F, 5.0F, 0.0F};
  glLightfv (GL_LIGHT0, GL_AMBIENT, light_ambient);
//  glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv (GL_LIGHT0, GL_POSITION, light0_position);
  glEnable (GL_LIGHTING);
  glEnable (GL_LIGHT0);

  glEnable (GL_DEPTH_TEST);
//  glDepthFunc (GL_LEQUAL);
  //  glEnable (GL_COLOR_MATERIAL);

//  glShadeModel (GL_FLAT); // flat shading
  glShadeModel (GL_SMOOTH); // smooth shading

  glClearColor (0.0F, 0.0F, 0.0F, 0.0F); // background color
  glClearDepth (1.0); // background depth value
//  glColor3f (1.0F, 1.0F, 1.0F); // white

  gdk_gl_drawable_gl_end (cb_data_p->openGLDrawable);

  return TRUE; // done (do not propagate further)
}


//G_MODULE_EXPORT gboolean
//delete_event_cb (GtkWidget* widget_in,
//                 GdkEvent* event_in,
//                 gpointer userData_in)
//{
//  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("::delete_event_cb"));

//  ACE_UNUSED_ARG (widget_in);
//  ACE_UNUSED_ARG (event_in);
//  ACE_UNUSED_ARG (userData_in);

//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("received delete event...\n")));

//  return FALSE; // propagate (*NOTE*: emits "destroy" signal)
//}

G_MODULE_EXPORT gboolean
expose_cb (GtkWidget* widget_in,
           GdkEventExpose* event_in,
           gpointer userData_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("::expose_cb"));

  ACE_UNUSED_ARG (event_in);
  Olimex_Mod_MPU6050_GtkCBData_t* cb_data_p =
      reinterpret_cast<Olimex_Mod_MPU6050_GtkCBData_t*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (widget_in);
  ACE_ASSERT (cb_data_p);

//  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard (cb_data_p->lock);
  cb_data_p->frameCounter++;

  ACE_Time_Value elapsed;
  const static ACE_Time_Value one_second (1, 0);
  if (!gdk_gl_drawable_gl_begin (cb_data_p->openGLDrawable,
                                 cb_data_p->openGLContext))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gdk_gl_drawable_gl_begin(), aborting\n")));
    return FALSE; // propagate
  } // end IF

  // step1: draw object(s)
//  glClearColor (0.0F, 0.0F, 0.0F, 0.0F);
  glClear (GL_COLOR_BUFFER_BIT |
           GL_DEPTH_BUFFER_BIT);
//  glColor3f (1.0F, 1.0F, 1.0F); // white

  const static gboolean solid = FALSE; // wireframe
  const static gdouble scale = 1.0;
  gdk_gl_draw_teapot (solid, scale);

  // step2: draw axes
  //  ::axes (1.0F);
  glCallList (cb_data_p->openGLAxesListId);

  // step3: draw fps
  elapsed = COMMON_TIME_POLICY () - cb_data_p->timestamp;
  if (elapsed >= one_second)
  {
    // switch to 2D-projection (HUD-mode)
    glMatrixMode (GL_PROJECTION);
    glPushMatrix ();
    glLoadIdentity ();
    glOrtho (0.0, widget_in->allocation.width,
             0.0, widget_in->allocation.height,
             -1.0, 10.0);
    glMatrixMode (GL_MODELVIEW);
    glPushMatrix ();
    glLoadIdentity ();

//    glClear (GL_DEPTH_BUFFER_BIT);
    ::frames_per_second (static_cast<float> (cb_data_p->frameCounter));

    // return to 3D-projection (3D-mode)
    glPopMatrix ();
    glMatrixMode (GL_PROJECTION);
    glPopMatrix ();
    glMatrixMode (GL_MODELVIEW);

    cb_data_p->frameCounter = 0;
    cb_data_p->timestamp += one_second;
  } // end IF

  // step4: swap buffers / flush
  if (cb_data_p->openGLDoubleBuffered)
    gdk_gl_drawable_swap_buffers (cb_data_p->openGLDrawable);
  else
    glFlush ();

  gdk_gl_drawable_gl_end (cb_data_p->openGLDrawable);

  return TRUE; // done (do not propagate further)
}

G_MODULE_EXPORT gboolean
process_cb (gpointer userData_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("::process_cb"));

  Olimex_Mod_MPU6050_GtkCBData_t* cb_data_p =
      reinterpret_cast<Olimex_Mod_MPU6050_GtkCBData_t*> (userData_in);
  int result = -1;
  unsigned short* data_p = NULL;

  // sanity check(s)
  ACE_ASSERT (cb_data_p);

  // step0: process event queue
  Olimex_Mod_MPU6050_Event_t event = OLIMEX_MOD_MPU6050_EVENT_INVALID;
  Olimex_Mod_MPU6050_Message* message_p = NULL;
  GtkStatusbar* status_bar_p =
      GTK_STATUSBAR (glade_xml_get_widget (cb_data_p->XML,
                                           ACE_TEXT_ALWAYS_CHAR (OLIMEX_MOD_MPU6050_UI_WIDGET_NAME_STATUS_BAR)));
  ACE_ASSERT (status_bar_p);
  {
    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard (cb_data_p->lock);
    if (cb_data_p->eventQueue.empty ())
      goto expose;
    event = cb_data_p->eventQueue.front ();
    cb_data_p->eventQueue.pop_front ();
    switch (event)
    {
      case OLIMEX_MOD_MPU6050_EVENT_CONNECT:
      {
        gtk_statusbar_push (status_bar_p,
                            cb_data_p->contextId,
                            ACE_TEXT_ALWAYS_CHAR ("CONNECT"));
        return TRUE; // done (do not propagate further)
      }
      case OLIMEX_MOD_MPU6050_EVENT_DISCONNECT:
      {
        gtk_statusbar_push (status_bar_p,
                            cb_data_p->contextId,
                            ACE_TEXT_ALWAYS_CHAR ("DISCONNECT"));
        return TRUE; // done (do not propagate further)
      }
      case OLIMEX_MOD_MPU6050_EVENT_MESSAGE:
      {
        message_p = cb_data_p->messageQueue.front ();
        cb_data_p->messageQueue.pop_front ();
        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid event (was: %d), aborting\n"),
                    event));
        return FALSE; // --> stop processing timer
      }
    } // end SWITCH
  } // end lock scope
  ACE_ASSERT (message_p);

  ACE_ASSERT (message_p->size () == OLIMEX_MOD_MPU6050_STREAM_BUFFER_SIZE);
  data_p = reinterpret_cast<unsigned short*> (message_p->rd_ptr ());
  unsigned short a_x, a_y, a_z;
  unsigned short g_x, g_y, g_z;
  short t;
  // *NOTE*: i2c uses a big-endian transfer syntax
  a_x = ((ACE_BYTE_ORDER == ACE_LITTLE_ENDIAN) ? ACE_SWAP_WORD (*data_p)
                                               : *data_p);
  // invert two's complement representation
  a_x = (a_x < 0) ? -(~(a_x - 1)) : ~(a_x - 1);
  data_p++;
  a_y = ((ACE_BYTE_ORDER == ACE_LITTLE_ENDIAN) ? ACE_SWAP_WORD (*data_p)
                                               : *data_p);
  a_y = (a_y < 0) ? -(~(a_y - 1)) : ~(a_y - 1);
  data_p++;
  a_z = ((ACE_BYTE_ORDER == ACE_LITTLE_ENDIAN) ? ACE_SWAP_WORD (*data_p)
                                               : *data_p);
  a_z = (a_z < 0) ? -(~(a_z - 1)) : ~(a_z - 1);
  data_p++;
  t = ((ACE_BYTE_ORDER == ACE_LITTLE_ENDIAN) ? ACE_SWAP_WORD (*data_p)
                                             : *data_p);
  data_p++;
  g_x = ((ACE_BYTE_ORDER == ACE_LITTLE_ENDIAN) ? ACE_SWAP_WORD (*data_p)
                                               : *data_p);
  g_x = (g_x < 0) ? -(~(g_x - 1)) : ~(g_x - 1);
  data_p++;
  g_y = ((ACE_BYTE_ORDER == ACE_LITTLE_ENDIAN) ? ACE_SWAP_WORD (*data_p)
                                               : *data_p);
  g_y = (g_y < 0) ? -(~(g_y - 1)) : ~(g_y - 1);
  data_p++;
  g_z = ((ACE_BYTE_ORDER == ACE_LITTLE_ENDIAN) ? ACE_SWAP_WORD (*data_p)
                                               : *data_p);
  g_z = (g_z < 0) ? -(~(g_z - 1)) : ~(g_z - 1);
  // translate quantities into absolute values
  GLfloat ax, ay, az, tp, gx, gy, gz;
  ax = a_x / static_cast<GLfloat> (OLIMEX_MOD_MPU6050_ACCELEROMETER_LSB_FACTOR_2);
  ay = a_y / static_cast<GLfloat> (OLIMEX_MOD_MPU6050_ACCELEROMETER_LSB_FACTOR_2);
  az = a_z / static_cast<GLfloat> (OLIMEX_MOD_MPU6050_ACCELEROMETER_LSB_FACTOR_2);
  tp = (t / static_cast<GLfloat> (OLIMEX_MOD_MPU6050_THERMOMETER_LSB_FACTOR)) +
       OLIMEX_MOD_MPU6050_THERMOMETER_OFFSET;
  gx = g_x / static_cast<GLfloat> (OLIMEX_MOD_MPU6050_GYROSCOPE_LSB_FACTOR_250);
  gy = g_y / static_cast<GLfloat> (OLIMEX_MOD_MPU6050_GYROSCOPE_LSB_FACTOR_250);
  gz = g_z / static_cast<GLfloat> (OLIMEX_MOD_MPU6050_GYROSCOPE_LSB_FACTOR_250);
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("%.3f,%.3f,%.3f,%.2f,%.3f,%.3f,%.3f\n"),
//              ax, ay, az,
//              tp,
//              gx, gy, gz));
  gchar buffer[BUFSIZ];
  ACE_OS::memset (buffer, 0, sizeof (buffer));
  result = ACE_OS::sprintf (buffer,
                            ACE_TEXT_ALWAYS_CHAR ("#%u: [accelerometer (g): %.3f,%.3f,%.3f], [gyrometer (°): %.3f,%.3f,%.3f], [temperature (°C): %.2f]"),
                            message_p->getID (),
                            ax, ay, az,
                            gx, gy, gz,
                            tp);
  if (result < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to sprintf(): \"%m\", aborting\n")));
    return FALSE; // --> stop processing timer
  } // end IF
  gtk_statusbar_push (status_bar_p,
                      cb_data_p->contextId,
                      buffer);

  static GLfloat angle_x, angle_y, angle_z;
//  angle_x = gx;
//  angle_y = gy;
//  angle_z = gz;

  // clean up
  message_p->release ();

  // step1: update control data/params
  if (!gdk_gl_drawable_gl_begin (cb_data_p->openGLDrawable,
                                 cb_data_p->openGLContext))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gdk_gl_drawable_gl_begin(), aborting\n")));
    return FALSE; // --> stop processing timer
  } // end IF

  glPushMatrix ();
//  glRotatef (angle_x, 1.0F, 0.0F, 0.0F);
//  glRotatef (angle_y, 0.0F, 1.0F, 0.0F);
//  glRotatef (angle_z, 0.0F, 0.0F, 1.0F);
  glPopMatrix ();

expose:
  gdk_gl_drawable_gl_end (cb_data_p->openGLDrawable);

  // step2: invalidate drawing area, marking it "dirty" and to be redrawn when
  // main loop signals expose-events (which it would do anyway (as needed), when
  // this returns and the main loop idles -- trigger immediate processing
  // instead, see below)
  GtkWidget* drawing_area =
      glade_xml_get_widget (cb_data_p->XML,
                            ACE_TEXT_ALWAYS_CHAR (OLIMEX_MOD_MPU6050_UI_WIDGET_NAME_DRAWING_AREA));
  ACE_ASSERT (drawing_area);
//  GtkAllocation allocation;
//  gtk_widget_get_allocation (drawing_area, &allocation);
  gdk_window_invalidate_rect (//gtk_widget_get_root_window (drawing_area),
                              drawing_area->window,
                              &drawing_area->allocation,
                              FALSE);
  gdk_window_process_updates (drawing_area->window, FALSE);

  return TRUE; // --> continue processing timer
}

G_MODULE_EXPORT gboolean
key_cb (GtkWidget* widget_in,
        GdkEventKey* event_in,
        gpointer userData_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("::key_cb"));

  Olimex_Mod_MPU6050_GtkCBData_t* cb_data_p =
      reinterpret_cast<Olimex_Mod_MPU6050_GtkCBData_t*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (event_in);
  ACE_ASSERT (cb_data_p);

  switch (event_in->keyval)
  {
//    case GDK_KEY_Left:
//      break;
//    case GDK_KEY_Right:
//      break;
    case GDK_KEY_Up:
      break;
    case GDK_KEY_Down:
      break;
    case GDK_KEY_c:
    case GDK_KEY_C:
      break;
    case GDK_KEY_d:
    case GDK_KEY_D:
      break;
    case GDK_KEY_r:
    case GDK_KEY_R:
      break;
    default:
      return FALSE; // propagate
  } // end SWITCH

  return TRUE; // done (do not propagate further)
}

/////////////////////////////////////////

G_MODULE_EXPORT gint
about_clicked_GTK_cb (GtkWidget* widget_in,
                      gpointer userData_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("::about_clicked_GTK_cb"));

  ACE_UNUSED_ARG (widget_in);
  Olimex_Mod_MPU6050_GtkCBData_t* cb_data_p =
      static_cast<Olimex_Mod_MPU6050_GtkCBData_t*> (userData_in);
  ACE_ASSERT (cb_data_p);

  // sanity check(s)
  ACE_ASSERT (cb_data_p->XML);

  // retrieve about dialog handle
  GtkWidget* about_dialog =
      GTK_WIDGET (glade_xml_get_widget (cb_data_p->XML,
                                        ACE_TEXT_ALWAYS_CHAR (OLIMEX_MOD_MPU6050_UI_WIDGET_NAME_DIALOG_ABOUT)));
  ACE_ASSERT (about_dialog);
  if (!about_dialog)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to glade_xml_get_widget(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT_ALWAYS_CHAR (OLIMEX_MOD_MPU6050_UI_WIDGET_NAME_DIALOG_ABOUT)));
    return FALSE; // propagate
  } // end IF

  // draw it
  if (!GTK_WIDGET_VISIBLE (about_dialog))
    gtk_widget_show_all (about_dialog);

  return TRUE; // done (do not propagate further)
}

G_MODULE_EXPORT gint
quit_clicked_GTK_cb (GtkWidget* widget_in,
                     gpointer userData_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("::quit_clicked_GTK_cb"));

  ACE_UNUSED_ARG (widget_in);
  ACE_UNUSED_ARG (userData_in);

  // this is the "delete-event" / "destroy" handler
  // --> destroy the main dialog widget
  pid_t pid = ACE_OS::getpid ();
  int result = -1;
  result = ACE_OS::kill (pid, SIGINT);
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to kill(%d, SIGINT): \"%m\", continuing\n"),
                pid));

//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("leaving GTK...\n")));

  return TRUE; // done (do not propagate further)
}
#ifdef __cplusplus
}
#endif /* __cplusplus */
