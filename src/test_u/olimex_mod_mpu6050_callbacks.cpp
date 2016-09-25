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

#include <cmath>

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "math.h"
#endif

#include <ace/Log_Msg.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <glm/glm.hpp>

#include <gmodule.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtkgl.h>

#include "common_timer_manager.h"

#include "common_ui_defines.h"

#include "olimex_mod_mpu6050_common.h"
#include "olimex_mod_mpu6050_defines.h"
#include "olimex_mod_mpu6050_macros.h"
#include "olimex_mod_mpu6050_message.h"
#include "olimex_mod_mpu6050_opengl.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
G_MODULE_EXPORT gboolean
idle_initialize_ui_cb (gpointer userData_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("::idle_initialize_ui_cb"));

  Olimex_Mod_MPU6050_GtkCBData* cb_data_p =
    static_cast<Olimex_Mod_MPU6050_GtkCBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (cb_data_p);

  Common_UI_GladeXMLsIterator_t iterator =
    cb_data_p->gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != cb_data_p->gladeXML.end ());

  GtkWindow* main_window_p =
    GTK_WINDOW (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (OLIMEX_MOD_MPU6050_UI_WIDGET_NAME_WINDOW_MAIN)));
  ACE_ASSERT (main_window_p);
  gtk_window_set_default_size (main_window_p,
                               OLIMEX_MOD_MPU6050_DEFAULT_UI_WIDGET_WINDOW_MAIN_SIZE_WIDTH,
                               OLIMEX_MOD_MPU6050_DEFAULT_UI_WIDGET_WINDOW_MAIN_SIZE_HEIGHT);

  GtkDialog* about_dialog_p =
    GTK_DIALOG (glade_xml_get_widget ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (OLIMEX_MOD_MPU6050_UI_WIDGET_NAME_DIALOG_ABOUT)));
  ACE_ASSERT (about_dialog_p);

  //if (cb_data_p->clientMode)
  //{
  //  GtkImageMenuItem* menu_item_p =
  //    GTK_IMAGE_MENU_ITEM (glade_xml_get_widget ((*iterator).second.second,
  //                                               ACE_TEXT_ALWAYS_CHAR (OLIMEX_MOD_MPU6050_UI_WIDGET_NAME_MENU_VIEW_CALIBRATE)));
  //  ACE_ASSERT (menu_item_p);
  //  gtk_widget_set_sensitive (GTK_WIDGET (menu_item_p), FALSE);
  //} // end IF

  GtkWidget* drawing_area_p =
      glade_xml_get_widget ((*iterator).second.second,
                            ACE_TEXT_ALWAYS_CHAR (OLIMEX_MOD_MPU6050_UI_WIDGET_NAME_DRAWING_AREA));
  ACE_ASSERT (drawing_area_p);

  GtkCurve* curve_p =
    GTK_CURVE (glade_xml_get_widget ((*iterator).second.second,
                                     ACE_TEXT_ALWAYS_CHAR (OLIMEX_MOD_MPU6050_UI_WIDGET_NAME_CURVE)));
  ACE_ASSERT (curve_p);
  // *WARNING*: glade/gtk crashes when declaring the curve type to be anything
  //            other than GTK_CURVE_TYPE_SPLINE (default)
  //gtk_curve_set_curve_type (curve_p, GTK_CURVE_TYPE_FREE);
  gtk_curve_set_range (curve_p,
                       0.0F,
                       static_cast <gfloat> (OLIMEX_MOD_MPU6050_TEMPERATURE_BUFFER_SIZE - 1),
                       0.0F,
                       100.0F);
  gtk_curve_set_vector (curve_p,
                        OLIMEX_MOD_MPU6050_TEMPERATURE_BUFFER_SIZE,
                        cb_data_p->temperature);

  GtkStatusbar* status_bar_p =
      GTK_STATUSBAR (glade_xml_get_widget ((*iterator).second.second,
                                           ACE_TEXT_ALWAYS_CHAR (OLIMEX_MOD_MPU6050_UI_WIDGET_NAME_STATUS_BAR)));
  ACE_ASSERT (status_bar_p);
  cb_data_p->contextIdData =
      gtk_statusbar_get_context_id (status_bar_p,
                                    ACE_TEXT_ALWAYS_CHAR (OLIMEX_MOD_MPU6050_UI_WIDGET_STATUS_BAR_CONTEXT_DATA));
  cb_data_p->contextIdInformation =
    gtk_statusbar_get_context_id (status_bar_p,
                                  ACE_TEXT_ALWAYS_CHAR (OLIMEX_MOD_MPU6050_UI_WIDGET_STATUS_BAR_CONTEXT_INFORMATION));

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
                    G_CALLBACK (quit_clicked_gtk_cb),
                    cb_data_p);
//                   G_CALLBACK(gtk_widget_destroyed),
//                   &main_dialog_p,
//  g_signal_connect (G_OBJECT (main_window_p),
//                    ACE_TEXT_ALWAYS_CHAR ("delete-event"),
//                    G_CALLBACK (delete_event_cb),
//                    NULL);
  GtkWidget* widget_p =
      GTK_WIDGET (glade_xml_get_widget ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (OLIMEX_MOD_MPU6050_UI_WIDGET_NAME_MENU_FILE_QUIT)));
  ACE_ASSERT (widget_p);
  g_signal_connect (G_OBJECT (widget_p),
                    ACE_TEXT_ALWAYS_CHAR ("activate"),
                    G_CALLBACK (quit_clicked_gtk_cb),
                    cb_data_p);

  widget_p =
    GTK_WIDGET (glade_xml_get_widget ((*iterator).second.second,
                                      ACE_TEXT_ALWAYS_CHAR (OLIMEX_MOD_MPU6050_UI_WIDGET_NAME_MENU_VIEW_CALIBRATE)));
  ACE_ASSERT (widget_p);
  g_signal_connect (G_OBJECT (widget_p),
                    ACE_TEXT_ALWAYS_CHAR ("activate"),
                    G_CALLBACK (calibrate_clicked_gtk_cb),
                    cb_data_p);

  widget_p =
      GTK_WIDGET (glade_xml_get_widget ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (OLIMEX_MOD_MPU6050_UI_WIDGET_NAME_MENU_HELP_ABOUT)));
  ACE_ASSERT (widget_p);
  g_signal_connect (G_OBJECT (widget_p),
                    ACE_TEXT_ALWAYS_CHAR ("activate"),
                    G_CALLBACK (about_clicked_gtk_cb),
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
    return G_SOURCE_REMOVE;
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
    return G_SOURCE_REMOVE;
  } // end IF

  if (gtk_widget_get_realized (drawing_area_p))
    gtk_widget_unrealize (drawing_area_p);
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

    return G_SOURCE_REMOVE;
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
  g_signal_connect (drawing_area_p,
                    ACE_TEXT_ALWAYS_CHAR ("motion-notify-event"),
                    G_CALLBACK (motion_cb),
                    cb_data_p);

//  // step5: use correct screen
//  if (parentWidget_in)
//    gtk_window_set_screen (GTK_WINDOW (dialog),
//                           gtk_widget_get_screen (const_cast<GtkWidget*> (//parentWidget_in)));

  // step6: draw main window
  gtk_widget_show_all (GTK_WIDGET (main_window_p));

  // step7: initialize fps, schedule refresh
  cb_data_p->timestamp = COMMON_TIME_POLICY ();
  guint opengl_refresh_rate =
      static_cast<guint> (OLIMEX_MOD_MPU6050_UI_WIDGET_GL_REFRESH_INTERVAL);
  cb_data_p->openGLRefreshId = g_timeout_add (opengl_refresh_rate,
                                              process_cb,
                                              cb_data_p);
  if (!cb_data_p->openGLRefreshId)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to g_timeout_add(): \"%m\", aborting\n")));
    return G_SOURCE_REMOVE;
  } // end IF
  //cb_data_p->openGLRefreshId = g_idle_add_full (10000,
  //                                              process_cb,
  //                                              cb_data_p,
  //                                              NULL);
  //if (cb_data_p->openGLRefreshId == 0)
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to g_idle_add_full(): \"%m\", aborting\n")));
  //  return G_SOURCE_REMOVE;
  //} // end IF
  else
    cb_data_p->eventSourceIds.insert (cb_data_p->openGLRefreshId);

  // one-shot action
  return G_SOURCE_REMOVE;
}

G_MODULE_EXPORT gboolean
idle_finalize_ui_cb (gpointer userData_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("::idle_finalize_ui_cb"));

  Olimex_Mod_MPU6050_GtkCBData* cb_data_p =
      static_cast<Olimex_Mod_MPU6050_GtkCBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (cb_data_p);

  // synch access
  {
    ACE_Guard<ACE_SYNCH_MUTEX> aGuard (cb_data_p->lock);

    unsigned int num_messages = cb_data_p->messageQueue.size ();
    while (!cb_data_p->messageQueue.empty ())
    {
      cb_data_p->messageQueue.front ()->release ();
      cb_data_p->messageQueue.pop_front ();
    } // end WHILE
    if (num_messages)
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("flushed %u messages\n"),
                  num_messages));
  } // end lock scope

  if (cb_data_p->openGLRefreshId)
  {
    g_source_remove (cb_data_p->openGLRefreshId);
    cb_data_p->openGLRefreshId = 0;
  } // end iF
  cb_data_p->eventSourceIds.clear ();

  if (glIsList (cb_data_p->openGLAxesListId))
  {
    glDeleteLists (cb_data_p->openGLAxesListId, 1);
    cb_data_p->openGLAxesListId = 0;
  } // end IF

  gtk_main_quit ();

  // one-shot action
  return G_SOURCE_REMOVE;
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
//  Olimex_Mod_MPU6050_GtkCBData* cb_data_p =
//      reinterpret_cast<Olimex_Mod_MPU6050_GtkCBData*> (userData_in);

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

  static bool is_first_invokation = true;

  Olimex_Mod_MPU6050_GtkCBData* cb_data_p =
      reinterpret_cast<Olimex_Mod_MPU6050_GtkCBData*> (userData_in);

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
    is_first_invokation = false;
    return FALSE; // propagate
  } // end IF

  if (!cb_data_p->openGLAxesListId)
  {
    ACE_Guard<ACE_SYNCH_MUTEX> aGuard (cb_data_p->lock);

    cb_data_p->openGLAxesListId = ::axes ();
    if (!glIsList (cb_data_p->openGLAxesListId))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ::axes (): \"%m\", aborting\n")));
      is_first_invokation = false;
      return G_SOURCE_REMOVE;
    } // end IF
  } // end lock scope

  glMatrixMode (GL_PROJECTION);
  // specify the lower left corner, as well as width/height of the viewport
  glViewport (0, 0,
              widget_in->allocation.width,
              widget_in->allocation.height);

  // setup a perspective projection
  // *NOTE*: initially, the "camera" points down the negative z-axis (y is up)
  glLoadIdentity ();
  //gluPerspective (OLIMEX_MOD_MPU6050_OPENGL_PERSPECTIVE_FOVY,
  //                (widget_in->allocation.width / widget_in->allocation.height),
  //                OLIMEX_MOD_MPU6050_OPENGL_PERSPECTIVE_ZNEAR,
  //                OLIMEX_MOD_MPU6050_OPENGL_PERSPECTIVE_ZFAR); // setup a perspective projection
  GLdouble fW, fH;
  fH =
   ::tan (OLIMEX_MOD_MPU6050_OPENGL_PERSPECTIVE_FOVY / 360.0 * M_PI) *
   OLIMEX_MOD_MPU6050_OPENGL_PERSPECTIVE_ZNEAR;
  fW = fH * (widget_in->allocation.width / widget_in->allocation.height);
  glFrustum (-fW, fW,
             -fH, fH,
             OLIMEX_MOD_MPU6050_OPENGL_PERSPECTIVE_ZNEAR,
             OLIMEX_MOD_MPU6050_OPENGL_PERSPECTIVE_ZFAR);

  glMatrixMode (GL_MODELVIEW);

  // set up the camera ?
  if (is_first_invokation)
    cb_data_p->resetCamera ();

  // *NOTE*: standard "right-hand" coordinate system (RHCS) is:
  //         x (thumb): right, y (index): up, z (middle): out of the screen
//  // top-down (RHCS: x --> up [y --> towards you, z --> right]
  //cb_data_p->openGLCamera.position[0] = 0.0F;  // eye position (*NOTE*: relative to standard
  //cb_data_p->openGLCamera.position[1] = 10.0F; //                       "right-hand" coordinate
  //cb_data_p->openGLCamera.position[2] = 0.0F;  //                       system [RHCS])
  //cb_data_p->openGLCamera.looking_at[0] = 0.0F; // looking-at position (RHCS notation)
  //cb_data_p->openGLCamera.looking_at[1] = 0.0F;
  //cb_data_p->openGLCamera.looking_at[2] = 0.0F;
  //cb_data_p->openGLCamera.up[0] = 1.0F; // up direction (RHCS notation, relative to "eye"
  //cb_data_p->openGLCamera.up[1] = 0.0F; // position and looking-at direction)
  //cb_data_p->openGLCamera.up[2] = 0.0F;

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
  // *TODO*: do this without glu, i.e. render the complete scene using only
  //         translations, rotations and scaling
  //gluLookAt (cb_data_p->openGLCamera.position[0],
  //           cb_data_p->openGLCamera.position[1],
  //           cb_data_p->openGLCamera.position[2],
  //           cb_data_p->openGLCamera.looking_at[0],
  //           cb_data_p->openGLCamera.looking_at[1],
  //           cb_data_p->openGLCamera.looking_at[2],
  //           cb_data_p->openGLCamera.up[0],
  //           cb_data_p->openGLCamera.up[1],
  //           cb_data_p->openGLCamera.up[2]);

  // set up features ?
  if (is_first_invokation)
  {
    glShadeModel (GL_SMOOTH); // shading mathod: GL_SMOOTH or GL_FLAT
    //glPixelStorei (GL_UNPACK_ALIGNMENT, 4); // 4-byte pixel alignment

    // enable/disable features
    glEnable (GL_DEPTH_TEST);
    glEnable (GL_LIGHTING);
    //glEnable (GL_TEXTURE_2D);
    //glEnable (GL_CULL_FACE);

    glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    //glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glEnable (GL_BLEND);

    //glEnable (GL_SCISSOR_TEST);

    // track material ambient and diffuse from surface color, call it before
    // glEnable(GL_COLOR_MATERIAL)
    glColorMaterial (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable (GL_COLOR_MATERIAL);

    //glLineWidth (1.0F);
    //glEnable (GL_LINE_SMOOTH);

    //glClearColor (0.0F, 0.0F, 0.0F, 0.0F); // background color
    //glClearStencil (0); // clear stencil buffer
    glClearDepth (1.0F); // 0 is near, 1 is far
    //glDepthFunc (GL_LEQUAL);
    //glColor3f (1.0F, 1.0F, 1.0F); // white
  } // end IF

  // set up lighting ?
  if (is_first_invokation)
  {
    // set up light colors (ambient, diffuse, specular)
    GLfloat light_ambient[] = {0.0F, 0.0F, 0.0F, 1.0F};
    glLightfv (GL_LIGHT0, GL_AMBIENT, light_ambient);
    GLfloat light_diffuse[] = {0.9F, 0.9F, 0.9F, 1.0F};
    glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    GLfloat light_specular[] = {1.0F, 1.0F, 1.0F, 1.0F};
    glLightfv (GL_LIGHT0, GL_SPECULAR, light_specular);

    // position the light in eye space
    GLfloat light0_position[] = {0.0F,
                                 OLIMEX_MOD_MPU6050_OPENGL_CAMERA_DEFAULT_ZOOM * 2,
                                 OLIMEX_MOD_MPU6050_OPENGL_CAMERA_DEFAULT_ZOOM * 2,
                                 0.0F}; // --> directional light
    glLightfv (GL_LIGHT0, GL_POSITION, light0_position);

    glEnable (GL_LIGHT0);
  } // end IF

  gdk_gl_drawable_gl_end (cb_data_p->openGLDrawable);

  is_first_invokation = false;

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
  Olimex_Mod_MPU6050_GtkCBData* cb_data_p =
      reinterpret_cast<Olimex_Mod_MPU6050_GtkCBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (widget_in);
  ACE_ASSERT (cb_data_p);

//  ACE_Guard<ACE_SYNCH_RECURSIVE_MUTEX> aGuard (cb_data_p->lock);

  if (!gdk_gl_drawable_gl_begin (cb_data_p->openGLDrawable,
                                 cb_data_p->openGLContext))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gdk_gl_drawable_gl_begin(), aborting\n")));
    return FALSE; // propagate
  } // end IF

  // step1: clear screen
  glClear (GL_COLOR_BUFFER_BIT |
           GL_DEPTH_BUFFER_BIT);

  // step2: set camera
  glPushMatrix ();

  // step2a: apply model transformation(s)
  //glTranslatef (cb_data_p->openGLCamera.translation[0],
  //              cb_data_p->openGLCamera.translation[1],
  //              cb_data_p->openGLCamera.translation[2]);
  glTranslatef (0.0F, 0.0F, -cb_data_p->openGLCamera.zoom);
  glRotatef (cb_data_p->openGLCamera.rotation[0], 1.0F, 0.0F, 0.0F);
  glRotatef (cb_data_p->openGLCamera.rotation[1], 0.0F, 1.0F, 0.0F);
  glRotatef (cb_data_p->openGLCamera.rotation[2], 0.0F, 0.0F, 1.0F);

  // step2b: apply view transformation(s)
  // *NOTE*: corrections so the teapot orients the same as the chip
  // *NOTE*: OpenGL uses RHCS, positive rotation is counter-clockwise (CCW)
  //         (that is, CCW looking TOWARDS the origin)
  glRotatef (90.0F, 0.0F, 0.0F, 1.0F);
  glRotatef (90.0F, 1.0F, 0.0F, 0.0F);

  // step3: draw object(s)
  // step3a: draw model
  const static bool solid = TRUE; // wireframe
  const static float scale = 1.0;
  draw_teapot (solid, scale);

  // step3b: draw axes
  // restrict viewport to upper left corner
  static GLsizei axes_height;
  axes_height =
    static_cast<GLsizei> (widget_in->allocation.height * OLIMEX_MOD_MPU6050_OPENGL_AXES_SIZE);
  glViewport (0, widget_in->allocation.height - axes_height,
              static_cast<GLsizei> (widget_in->allocation.width * OLIMEX_MOD_MPU6050_OPENGL_AXES_SIZE),
              axes_height);
  //glScissor (0, allocation.height - axes_height,
  //           static_cast<GLsizei> (allocation.width * OLIMEX_MOD_MPU6050_OPENGL_AXES_SIZE),
  //           axes_height);
  //glEnable (GL_SCISSOR_TEST);
  //static GLfloat clear_color[4];
  //glGetFloatv (GL_COLOR_CLEAR_VALUE, clear_color);
  //glClearColor (1.0F, 1.0F, 1.0F, 1.0F);
  //glClear (GL_COLOR_BUFFER_BIT |
  //         GL_DEPTH_BUFFER_BIT);

  // set specific features
  static GLfloat color[4];
  glGetFloatv (GL_CURRENT_COLOR, color);
  //glEnable (GL_COLOR_MATERIAL);
  glDisable (GL_LIGHTING);

  // step1: axes don't translate/zoom
  static GLfloat modelview_matrix[16];
  glGetFloatv (GL_MODELVIEW_MATRIX, modelview_matrix);
  glPushMatrix ();
  modelview_matrix[12] = 0.0F;
  modelview_matrix[13] = 0.0F;
  modelview_matrix[14] = -OLIMEX_MOD_MPU6050_OPENGL_CAMERA_DEFAULT_ZOOM;
  glLoadMatrixf (modelview_matrix);

  // draw
  glCallList (cb_data_p->openGLAxesListId);

  // restore standard camera perspective
  glPopMatrix ();

  // reset specific features
  glColor3f (color[0], color[1], color[2]);
  //glDisable (GL_COLOR_MATERIAL);
  glEnable (GL_LIGHTING);

  // restore viewport
  //glClearColor (clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
  //glDisable (GL_SCISSOR_TEST);
  glViewport (0, 0,
              widget_in->allocation.width,
              widget_in->allocation.height);

  // return to 3D-projection
  glPopMatrix ();

  // step3c: draw fps
  // switch to 2D-projection (i.e. "HUD"-mode)
  glMatrixMode (GL_PROJECTION);
  glPushMatrix ();
  glLoadIdentity ();
  glOrtho (0.0, widget_in->allocation.width,
           0.0, widget_in->allocation.height,
           OLIMEX_MOD_MPU6050_OPENGL_ORTHO_ZNEAR,
           OLIMEX_MOD_MPU6050_OPENGL_ORTHO_ZFAR);
  glMatrixMode (GL_MODELVIEW);
  glPushMatrix ();
  glLoadIdentity ();

  cb_data_p->frameCounter++;
  const static ACE_Time_Value one_second (1, 0);
  ACE_Time_Value elapsed = COMMON_TIME_POLICY () - cb_data_p->timestamp;
  ACE_UINT64 elapsed_usec;
  elapsed.to_usec (elapsed_usec);
  if (elapsed > one_second)
    elapsed_usec -= 1000000;
  ::frames_per_second (static_cast<float> (cb_data_p->frameCounter) *
                       (1000000.0F / static_cast<float> (elapsed_usec)));

  // return to 3D-projection
  glPopMatrix ();
  glMatrixMode (GL_PROJECTION);
  glPopMatrix ();
  glMatrixMode (GL_MODELVIEW);

  if (elapsed >= one_second)
  {
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

  Olimex_Mod_MPU6050_GtkCBData* cb_data_p =
      reinterpret_cast<Olimex_Mod_MPU6050_GtkCBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (cb_data_p);

  Common_UI_GladeXMLsIterator_t iterator =
    cb_data_p->gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != cb_data_p->gladeXML.end ());

  // step0: process event queue
  GtkStatusbar* status_bar_p =
      GTK_STATUSBAR (glade_xml_get_widget ((*iterator).second.second,
                                           ACE_TEXT_ALWAYS_CHAR (OLIMEX_MOD_MPU6050_UI_WIDGET_NAME_STATUS_BAR)));
  ACE_ASSERT (status_bar_p);
  Olimex_Mod_MPU6050_Message* message_p = NULL;
  {
    ACE_Guard<ACE_SYNCH_MUTEX> aGuard (cb_data_p->lock);
    if (cb_data_p->eventQueue.empty ())
    {
      return TRUE; // --> continue processing
      //goto expose;
    } // end IF
    switch (cb_data_p->eventQueue.front ())
    {
      case OLIMEX_MOD_MPU6050_EVENT_CONNECT:
      {
        cb_data_p->eventQueue.pop_front ();
        gtk_statusbar_push (status_bar_p,
                            cb_data_p->contextIdInformation,
                            ACE_TEXT_ALWAYS_CHAR ("connected"));
        return TRUE; // continue processing
      }
      case OLIMEX_MOD_MPU6050_EVENT_DISCONNECT:
      {
        cb_data_p->eventQueue.pop_front ();
        gtk_statusbar_push (status_bar_p,
                            cb_data_p->contextIdInformation,
                            ACE_TEXT_ALWAYS_CHAR ("disconnected"));
        return TRUE; // continue processing
      }
      case OLIMEX_MOD_MPU6050_EVENT_MESSAGE:
      {
        cb_data_p->eventQueue.pop_front ();
        message_p = cb_data_p->messageQueue.front ();
        cb_data_p->messageQueue.pop_front ();
        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid event (was: %d), aborting\n"),
                    cb_data_p->eventQueue.front ()));
        cb_data_p->eventQueue.pop_front ();
        return FALSE; // --> stop processing
      }
    } // end SWITCH
  } // end lock scope
  ACE_ASSERT (message_p);
  ACE_ASSERT (message_p->size () == OLIMEX_MOD_MPU6050_STREAM_BUFFER_SIZE);

  // step1a: extract sensor data
  float ax, ay, az, t, gx, gy, gz;
  extract_data (message_p->rd_ptr (),
                ax, ay, az,
                t,
                gx, gy, gz);

  // step1b: correct data with bias information
  if (cb_data_p->clientMode)
  {
    ax -= cb_data_p->clientSensorBias.ax_bias;
    ay -= cb_data_p->clientSensorBias.ay_bias;
    az -= cb_data_p->clientSensorBias.az_bias;
    gx -= cb_data_p->clientSensorBias.gx_bias;
    gy -= cb_data_p->clientSensorBias.gy_bias;
    gz -= cb_data_p->clientSensorBias.gz_bias;
  } // end IF

  // step1c: update status bar
  gchar buffer[BUFSIZ];
  ACE_OS::memset (buffer, 0, sizeof (buffer));
  int result =
    ACE_OS::sprintf (buffer,
                     ACE_TEXT_ALWAYS_CHAR ("[accelerometer (g/s): %6.3f,%6.3f,%6.3f], [gyrometer (°/s): %8.3f,%8.3f,%8.3f], [temperature (°C): %6.2f]"),
                     ax, ay, az,
                     gx, gy, gz,
                     t);
  if (result < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to sprintf(): \"%m\", aborting\n")));

    // clean up
    message_p->release ();

    return FALSE; // --> stop processing
  } // end IF
  gtk_statusbar_pop (status_bar_p,
                     cb_data_p->contextIdData);
  gtk_statusbar_push (status_bar_p,
                      cb_data_p->contextIdData,
                      buffer);

  // step2: process sensor data
  static float angle_x, angle_y, angle_z;
  // step2a: acceleration / rotation
  //cb_data_p->openGLCamera.translation[0] +=
  //  OLIMEX_MOD_MPU6050_OPENGL_CAMERA_TRANSLATION_FACTOR * diff_x;
  //cb_data_p->openGLCamera.translation[1] -=
  //  OLIMEX_MOD_MPU6050_OPENGL_CAMERA_TRANSLATION_FACTOR * diff_y;
  //cb_data_p->openGLCamera.translation[2] -=
  //  OLIMEX_MOD_MPU6050_OPENGL_CAMERA_TRANSLATION_FACTOR * diff_y;
  // *TODO*: x,y rotation directions seem inverted for some reason...
  angle_x = -gx * (1.0F / static_cast<float> (OLIMEX_MOD_MPU6050_DATA_RATE));
  angle_y = -gy * (1.0F / static_cast<float> (OLIMEX_MOD_MPU6050_DATA_RATE));
  angle_z =  gz * (1.0F / static_cast<float> (OLIMEX_MOD_MPU6050_DATA_RATE));
  cb_data_p->openGLCamera.rotation[0] += angle_x;
  cb_data_p->openGLCamera.rotation[1] += angle_y;
  cb_data_p->openGLCamera.rotation[2] += angle_z;

  // step2b: temperature
  GtkCurve* curve_p =
    GTK_CURVE (glade_xml_get_widget ((*iterator).second.second,
                                     ACE_TEXT_ALWAYS_CHAR (OLIMEX_MOD_MPU6050_UI_WIDGET_NAME_CURVE)));
  ACE_ASSERT (curve_p);
  cb_data_p->temperatureIndex++;
  if (cb_data_p->temperatureIndex == OLIMEX_MOD_MPU6050_TEMPERATURE_BUFFER_SIZE)
  {
    cb_data_p->temperatureIndex = 0;
    //ACE_OS::memset (cb_data_p->temperature,
    //                0,
    //                sizeof (cb_data_p->temperature));
    gtk_curve_reset (curve_p);
  } // end IF
  cb_data_p->temperature[cb_data_p->temperatureIndex * 2] =
    static_cast<gfloat> (cb_data_p->temperatureIndex);
  cb_data_p->temperature[(cb_data_p->temperatureIndex * 2) + 1] =
    ((t + OLIMEX_MOD_MPU6050_THERMOMETER_OFFSET) /
    OLIMEX_MOD_MPU6050_THERMOMETER_RANGE) *
    OLIMEX_MOD_MPU6050_UI_WIDGET_CURVE_MAXIMUM_Y;
  //gtk_curve_set_range (curve_p,
  //                     0.0F,
  //                     static_cast<gfloat> (cb_data_p->temperatureIndex),
  //                     0.0F,
  //                     100.0F);
  gtk_curve_set_vector (curve_p,
                        OLIMEX_MOD_MPU6050_TEMPERATURE_BUFFER_SIZE,
                        //cb_data_p->temperatureIndex + 1,
                        cb_data_p->temperature);

//expose:
  // invalidate drawing area
  // *NOTE*: the drawing area is not refreshed automatically unless the window
  //         is resized
  GtkWidget* drawing_area_p =
      glade_xml_get_widget ((*iterator).second.second,
                            ACE_TEXT_ALWAYS_CHAR (OLIMEX_MOD_MPU6050_UI_WIDGET_NAME_DRAWING_AREA));
  ACE_ASSERT (drawing_area_p);
  gtk_widget_queue_draw (drawing_area_p);
  //gtk_widget_queue_draw (GTK_WIDGET (curve_p));
  //gdk_window_invalidate_rect (//gtk_widget_get_root_window (drawing_area),
  //                            drawing_area_p->window,
  //                            &drawing_area_p->allocation,
  //                            FALSE);
  //gdk_window_process_updates (drawing_area_p->window, FALSE);

  // clean up
  message_p->release ();
  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("buffer pool: %d/%d [%.0f%%] --> %u byte(s)\n"),
  //            cb_data_p->allocator->cache_size (),
  //            OLIMEX_MOD_MPU6050_MAXIMUM_NUMBER_OF_INFLIGHT_MESSAGES,
  //            ((static_cast<float> (cb_data_p->allocator->cache_size ()) /
  //              static_cast<float> (OLIMEX_MOD_MPU6050_MAXIMUM_NUMBER_OF_INFLIGHT_MESSAGES)) * 100.0F),
  //            cb_data_p->allocator->cache_depth ()));

  return TRUE; // --> continue processing
}

G_MODULE_EXPORT gboolean
key_cb (GtkWidget* widget_in,
        GdkEventKey* event_in,
        gpointer userData_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("::key_cb"));

  Olimex_Mod_MPU6050_GtkCBData* cb_data_p =
      reinterpret_cast<Olimex_Mod_MPU6050_GtkCBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (event_in);
  ACE_ASSERT (cb_data_p);

  switch (event_in->keyval)
  {
    case GDK_KEY_Left:
    {
      // *NOTE*: the camera moves on a circle in a plane perpendicular to the
      //         plane between the "view" and "up" directions. Therefore, the
      //         distance to the "target" remains constant
      // *NOTE*: camera positions are (discrete) positions on the circle
      //         perimeter

      //// compute movement plane
      //glm::vec3 unit_vector = glm::cross (cb_data_p->openGLCamera.looking_at,
      //                                    cb_data_p->openGLCamera.up);
      ////unit_vector = glm::normalize (unit_vector);

      //// compute distance factor
      //glm::vec3 direction =
      // cb_data_p->openGLCamera.looking_at - cb_data_p->openGLCamera.position;
      //GLfloat factor = glm::length (direction);

      //// compute position on circle perimeter
      //cb_data_p->openGLCamera.angle -= OLIMEX_MOD_MPU6050_OPENGL_CAMERA_ROTATION_STEP;
      //GLfloat angle = cb_data_p->openGLCamera.angle * 360.0F;

      //// compute camera position
      //glm::vec3 p, q, r;
      //p = cb_data_p->openGLCamera.position;
      //r = unit_vector;
      //r[0] = ::cos (angle);
      //r[1] = ::sin (angle);
      //r[2] = 0.0F;
      //r = glm::normalize (cb_data_p->openGLCamera.looking_at + r);
      //q = (r - p) * factor;
      //cb_data_p->openGLCamera.position = q;

      //gluLookAt (cb_data_p->openGLCamera.position[0],
      //           cb_data_p->openGLCamera.position[1],
      //           cb_data_p->openGLCamera.position[2],
      //           cb_data_p->openGLCamera.looking_at[0],
      //           cb_data_p->openGLCamera.looking_at[1],
      //           cb_data_p->openGLCamera.looking_at[2],
      //           cb_data_p->openGLCamera.up[0],
      //           cb_data_p->openGLCamera.up[1],
      //           cb_data_p->openGLCamera.up[2]);

      break;
    }
    case GDK_KEY_Right:
      break;
    //case GDK_KEY_Up:
    //  break;
    //case GDK_KEY_Down:
    //  break;
    //case GDK_KEY_c:
    //case GDK_KEY_C:
    //  break;
    //case GDK_KEY_d:
    //case GDK_KEY_D:
    //  break;
    case GDK_KEY_r:
    case GDK_KEY_R:
    {
      cb_data_p->resetCamera ();

      gtk_widget_queue_draw (widget_in);

      break;
    }
    default:
      return FALSE; // propagate
  } // end SWITCH

  return TRUE; // done (do not propagate further)
}

G_MODULE_EXPORT gboolean
motion_cb (GtkWidget* widget_in,
           GdkEventMotion* event_in,
           gpointer userData_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("::motion_cb"));

  Olimex_Mod_MPU6050_GtkCBData* cb_data_p =
    reinterpret_cast<Olimex_Mod_MPU6050_GtkCBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (event_in);
  ACE_ASSERT (cb_data_p);
  if (event_in->is_hint) return FALSE; // propagate

  int diff_x =
   static_cast<int> (event_in->x) - cb_data_p->openGLCamera.last[0];
  int diff_y =
   static_cast<int> (event_in->y) - cb_data_p->openGLCamera.last[1];
  cb_data_p->openGLCamera.last[0] = static_cast<int> (event_in->x);
  cb_data_p->openGLCamera.last[1] = static_cast<int> (event_in->y);

  bool is_dirty = false;
  if (event_in->state & GDK_BUTTON1_MASK)
  {
    cb_data_p->openGLCamera.rotation[0] +=
      OLIMEX_MOD_MPU6050_OPENGL_CAMERA_ROTATION_FACTOR * diff_y;
    cb_data_p->openGLCamera.rotation[1] +=
      OLIMEX_MOD_MPU6050_OPENGL_CAMERA_ROTATION_FACTOR * diff_x;
    is_dirty = true;
  } // end IF
  if (event_in->state & GDK_BUTTON2_MASK)
  {
    cb_data_p->openGLCamera.translation[0] +=
      OLIMEX_MOD_MPU6050_OPENGL_CAMERA_TRANSLATION_FACTOR * diff_x;
    cb_data_p->openGLCamera.translation[1] -=
      OLIMEX_MOD_MPU6050_OPENGL_CAMERA_TRANSLATION_FACTOR * diff_y;
    is_dirty = true;
  } // end IF
  if (event_in->state & GDK_BUTTON3_MASK)
  {
    cb_data_p->openGLCamera.zoom -=
      OLIMEX_MOD_MPU6050_OPENGL_CAMERA_ZOOM_FACTOR * diff_x;
    is_dirty = true;
  } // end IF

  // invalidate drawing area
  // *NOTE*: the drawing area is not refreshed automatically unless the window
  //         is resized
  if (is_dirty)
    gtk_widget_queue_draw (widget_in);

  return TRUE; // done (do not propagate further)
}

/////////////////////////////////////////

G_MODULE_EXPORT gint
about_clicked_gtk_cb (GtkWidget* widget_in,
                      gpointer userData_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("::about_clicked_gtk_cb"));

  ACE_UNUSED_ARG (widget_in);
  Olimex_Mod_MPU6050_GtkCBData* cb_data_p =
      static_cast<Olimex_Mod_MPU6050_GtkCBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (cb_data_p);

  Common_UI_GladeXMLsIterator_t iterator =
    cb_data_p->gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != cb_data_p->gladeXML.end ());

  // retrieve about dialog handle
  GtkWidget* about_dialog =
      GTK_WIDGET (glade_xml_get_widget ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (OLIMEX_MOD_MPU6050_UI_WIDGET_NAME_DIALOG_ABOUT)));
  ACE_ASSERT (about_dialog);
  if (!about_dialog)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to glade_xml_get_widget(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (OLIMEX_MOD_MPU6050_UI_WIDGET_NAME_DIALOG_ABOUT)));
    return FALSE; // propagate
  } // end IF

  // draw it
  if (!GTK_WIDGET_VISIBLE (about_dialog))
    gtk_widget_show_all (about_dialog);

  return TRUE; // done (do not propagate further)
}

G_MODULE_EXPORT gint
calibrate_clicked_gtk_cb (GtkWidget* widget_in,
                          gpointer userData_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("::calibrate_clicked_gtk_cb"));

  ACE_UNUSED_ARG (widget_in);
  Olimex_Mod_MPU6050_GtkCBData* cb_data_p =
    static_cast<Olimex_Mod_MPU6050_GtkCBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (cb_data_p);

  Common_UI_GladeXMLsIterator_t iterator =
    cb_data_p->gladeXML.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != cb_data_p->gladeXML.end ());

  if (cb_data_p->clientMode)
  {
    ACE_OS::memset (&cb_data_p->clientSensorBias,
                    0,
                    sizeof (cb_data_p->clientSensorBias));

    ACE_Guard<ACE_SYNCH_MUTEX> aGuard (cb_data_p->lock);
    if (cb_data_p->eventQueue.empty ())
    {
      ACE_DEBUG ((LM_WARNING,
                  ACE_TEXT ("no data, returning\n")));
      return FALSE; // propagate
    } // end IF
    Olimex_Mod_MPU6050_Message* message_p = NULL;
    message_p = cb_data_p->messageQueue.front ();
    ACE_ASSERT (message_p);
    float dummy;
    extract_data (message_p->rd_ptr (),
                  cb_data_p->clientSensorBias.ax_bias,
                  cb_data_p->clientSensorBias.ay_bias,
                  cb_data_p->clientSensorBias.az_bias,
                  dummy,
                  cb_data_p->clientSensorBias.gx_bias,
                  cb_data_p->clientSensorBias.gy_bias,
                  cb_data_p->clientSensorBias.gz_bias);
    cb_data_p->clientSensorBias.az_bias += 1.0F;

    // update status bar
    gchar buffer[BUFSIZ];
    ACE_OS::memset (buffer, 0, sizeof (buffer));
    int result = ACE_OS::sprintf (buffer,
                                  ACE_TEXT_ALWAYS_CHAR ("updated bias data: [accelerometer (g/s): %6.3f,%6.3f,%6.3f], [gyrometer (°/s): %8.3f,%8.3f,%8.3f]"),
                                  cb_data_p->clientSensorBias.ax_bias,
                                  cb_data_p->clientSensorBias.ay_bias,
                                  cb_data_p->clientSensorBias.az_bias,
                                  cb_data_p->clientSensorBias.gx_bias,
                                  cb_data_p->clientSensorBias.gy_bias,
                                  cb_data_p->clientSensorBias.gz_bias);
    if (result < 0)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to sprintf(): \"%m\", continuing\n")));
      return FALSE; // propagate
    } // end IF
    GtkStatusbar* status_bar_p =
      GTK_STATUSBAR (glade_xml_get_widget ((*iterator).second.second,
                                           ACE_TEXT_ALWAYS_CHAR (OLIMEX_MOD_MPU6050_UI_WIDGET_NAME_STATUS_BAR)));
    ACE_ASSERT (status_bar_p);
    gtk_statusbar_push (status_bar_p,
                        cb_data_p->contextIdInformation,
                        buffer);
  } // end IF
  else
  {
    // *TODO*: implement a client->server protocol to do this
    ACE_ASSERT (false);
    ACE_NOTSUP_RETURN (TRUE);
  } // end IF

  return TRUE; // done (do not propagate further)
}

G_MODULE_EXPORT gint
quit_clicked_gtk_cb (GtkWidget* widget_in,
                     gpointer userData_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("::quit_clicked_gtk_cb"));

  ACE_UNUSED_ARG (widget_in);
  ACE_UNUSED_ARG (userData_in);

  // this is the "delete-event" / "destroy" handler
  // --> destroy the main dialog widget
  int result = -1;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = ACE_OS::raise (SIGINT);
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to raise(SIGINT): \"%m\", continuing\n")));
#else
  pid_t pid = ACE_OS::getpid ();
  result = ACE_OS::kill (pid, SIGINT);
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to kill(%d, SIGINT): \"%m\", continuing\n"),
                pid));
#endif

//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("leaving GTK...\n")));

  return TRUE; // done (do not propagate further)
}
#ifdef __cplusplus
}
#endif /* __cplusplus */
