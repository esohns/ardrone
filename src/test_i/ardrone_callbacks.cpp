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

#include "ardrone_callbacks.h"

#include <cmath>

#if defined (ACE_WIN32) || defined (ACE_WIN64)
//#include "math.h"
#endif

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <gl/GL.h>
#include <gl/GLU.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include <GL/glut.h>
#include <glm/glm.hpp>

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <mfapi.h>
#include <mfidl.h>
#endif

#ifdef __cplusplus
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavutil/pixfmt.h>
}
#endif /* __cplusplus */

#include "ace/Log_Msg.h"
#include "ace/Process.h"
#include "ace/Process_Manager.h"
#include "ace/Synch.h"

#include <gmodule.h>

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <gdk/gdkwin32.h>
#else
#include <gdk/gdk.h>
#endif
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
//#include <gtk/gtkgl.h>

#include "common_timer_manager.h"

#include "common_ui_defines.h"
#include "common_ui_tools.h"

#include "stream_dec_tools.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "stream_dev_directshow_tools.h"
#endif

#include "stream_vis_common.h"

#ifdef HAVE_CONFIG_H
#include "ardrone_config.h"
#endif

#include "ardrone_common.h"
#include "ardrone_configuration.h"
#include "ardrone_defines.h"
#include "ardrone_macros.h"
#include "ardrone_message.h"
#include "ardrone_opengl.h"

// global variables
bool un_toggling_connect = false;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
BOOL CALLBACK
monitor_enum_cb (HMONITOR monitor_in,
                 HDC      deviceContext_in,
                 LPRECT   clippingArea_in,
                 LPARAM   CBData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::monitor_enum_cb"));

  // sanity check(s)
  ACE_ASSERT (CBData_in);

  GtkListStore* list_store_p = reinterpret_cast<GtkListStore*> (CBData_in);

  // sanity check(s)
  ACE_ASSERT (list_store_p);
  MONITORINFOEX monitor_info;
  monitor_info.cbSize = sizeof (MONITORINFOEX);
  if (!GetMonitorInfo (monitor_in,
                       &monitor_info))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to GetMonitorInfo(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Tools::error2String (GetLastError ()).c_str ())));
    return FALSE;
  } // end IF

  GtkTreeIter iterator;
  gtk_list_store_append (list_store_p, &iterator);
  gtk_list_store_set (list_store_p, &iterator,
#if defined (ACE_USES_WCHAR)
                      0, ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_WCHAR_TO_TCHAR (monitor_info.szDevice)),
#else
                      0, monitor_info.szDevice,
#endif
                      -1);

  return TRUE;
};

BOOL CALLBACK
monitor_clip_cb (HMONITOR monitor_in,
                 HDC      deviceContext_in,
                 LPRECT   clippingArea_in,
                 LPARAM   userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::monitor_clip_cb"));

  ACE_UNUSED_ARG (deviceContext_in);

  // sanity check(s)
  ACE_ASSERT (clippingArea_in);
  ACE_ASSERT (userData_in);

  struct ARDrone_GtkCBData* cb_data_p =
    reinterpret_cast<struct ARDrone_GtkCBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (cb_data_p);
  ACE_ASSERT (cb_data_p->configuration);

  MONITORINFOEX monitor_info_ex_s;
  ACE_OS::memset (&monitor_info_ex_s, 0, sizeof (MONITORINFOEX));
  monitor_info_ex_s.cbSize = sizeof (MONITORINFOEX);
  if (!GetMonitorInfo (monitor_in,
                       &monitor_info_ex_s))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to GetMonitorInfo(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Tools::error2String (GetLastError ()).c_str ())));
    return FALSE;
  } // end IF

  if (ACE_OS::strcmp (monitor_info_ex_s.szDevice,
                      cb_data_p->configuration->moduleHandlerConfiguration.device.c_str ()))
    return TRUE;

  cb_data_p->configuration->moduleHandlerConfiguration.area =
    *clippingArea_in;

  return TRUE;
};
#endif

bool
load_display_devices (GtkListStore* listStore_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::load_display_devices"));

  // initialize result
  gtk_list_store_clear (listStore_in);

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (!EnumDisplayMonitors (NULL,                                     // hdc
                            NULL,                                     // lprcClip
                            monitor_enum_cb,
                            reinterpret_cast<LPARAM> (listStore_in)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to EnumDisplayMonitors(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Tools::error2String (GetLastError ()).c_str ())));
    return false;
  } // end IF
  //GdkDisplayManager* display_manager_p = gdk_display_manager_get ();
  //ACE_ASSERT (display_manager_p);
  //GSList* list_p = gdk_display_manager_list_displays (display_manager_p);
  //ACE_ASSERT (list_p);

  //GdkDisplay* display_p = NULL;
  //int number_of_monitors = 0;
  //GdkMonitor* monitor_p = NULL;
  //GtkTreeIter iterator;
  //for (GSList* list_2 = list_p;
  //     list_2;
  //     list_2 = list_2->next)
  //{
  //  display_p = GDK_DISPLAY (list_2->data);
  //  ACE_ASSERT (display_p);

  //  ACE_DEBUG ((LM_DEBUG,
  //              ACE_TEXT ("found display: \"%s\"...\n"),
  //              ACE_TEXT (gdk_display_get_name (display_p))));

  //  number_of_monitors = gdk_display_get_n_monitors (display_p);
  //  for (int i = 0;
  //       i < number_of_monitors;
  //       ++i)
  //  {
  //    monitor_p = gdk_display_get_monitor (display_p,
  //                                         i);
  //    ACE_ASSERT (monitor_p);

  //    ACE_DEBUG ((LM_DEBUG,
  //                ACE_TEXT ("found monitor: \"%s\"...\n"),
  //                ACE_TEXT (gdk_monitor_get_model (monitor_p))));

  //    gtk_list_store_append (listStore_in, &iterator);
  //    gtk_list_store_set (listStore_in, &iterator,
  //                        0, gdk_monitor_get_model (monitor_p),
  //                        -1);
  //  } // end FOR
  //} // end FOR

  //g_slist_free (list_p);
#else
  int result = -1;
  // *TODO*: this should work on most Xorg systems, but is really a bad idea:
  //         - relies on local 'xrandr' tool
  //         - temporary files
  //         - system(3) call
  //         --> extremely inefficient; remove ASAP
  std::string filename_string =
      Common_File_Tools::getTempFilename (ACE_TEXT_ALWAYS_CHAR (""));
  std::string command_line_string = ACE_TEXT_ALWAYS_CHAR ("xrandr >> ");
  command_line_string += filename_string;

//  ACE_Process_Options process_options (false);/*,                                             // inherit environment ?
//                                       ACE_Process_Options::DEFAULT_COMMAND_LINE_BUF_LEN, // command line buffer length
//                                       ACE_Process_Options::ENVIRONMENT_BUFFER,           // environment buffer
//                                       ACE_Process_Options::MAX_ENVIRONMENT_ARGS,         // #environment variables
//                                       ACE_Process_Options::MAX_COMMAND_LINE_OPTIONS);    // #command line options*/
//  result =
//      process_options.command_line (ACE_TEXT (command_line_string.c_str ()));
//  if (result == -1)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ACE_Process_Options::command_line(\"%s\"): \"%m\", aborting\n"),
//                ACE_TEXT (command_line_string.c_str ())));
//    return false;
//  } // end IF
//  ACE_Process_Manager* process_manager_p = ACE_Process_Manager::instance ();
//  ACE_ASSERT (process_manager_p);
//  pid_t process_pid = process_manager_p->spawn (process_options,
//                                                NULL);
//  if (process_pid == ACE_INVALID_PID)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ACE_Process_Manager::spawn(\"%s\"): \"%m\", aborting\n"),
//                ACE_TEXT (command_line_string.c_str ())));
//    return false;
//  } // end IF
//  ACE_exitcode exit_status = -1;
//  if (process_manager_p->wait (process_pid, &exit_status) == ACE_INVALID_PID)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ACE_Process_Manager::wait(): \"%m\", aborting\n")));
//    return false;
//  } // end IF

  std::string display_records_string;
#if defined (DEBUG_DEBUGGER)
  // *TODO*: cannot debug this (gdb crashes)
#else
  result = ACE_OS::system (ACE_TEXT (command_line_string.c_str ()));
//  result = execl ("/bin/sh", "sh", "-c", command, (char *) 0);
  if ((result == -1)      ||
      !WIFEXITED (result) ||
      WEXITSTATUS (result))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_OS::system(\"%s\"): \"%m\" (result was: %d), aborting\n"),
                ACE_TEXT (command_line_string.c_str ()),
                WEXITSTATUS (result)));
    return false;
  } // end IF
  unsigned char* data_p = NULL;
  if (!Common_File_Tools::load (filename_string,
                                data_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_File_Tools::load(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (filename_string.c_str ())));
    return false;
  } // end IF
  if (!Common_File_Tools::deleteFile (filename_string))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_File_Tools::deleteFile(\"%s\"), continuing\n"),
                ACE_TEXT (filename_string.c_str ())));

  display_records_string = reinterpret_cast<char*> (data_p);
  delete [] data_p;
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("xrandr data: \"%s\"\n"),
//              ACE_TEXT (display_record_string.c_str ())));
#endif

  std::istringstream converter;
  char buffer [BUFSIZ];
  std::string regex_string =
      ACE_TEXT_ALWAYS_CHAR ("^(.+) (?:connected)(?: primary)? (.+) \\((?:(.+)\\w*)+\\) ([[:digit:]]+)mm x ([[:digit:]]+)mm$");
  std::regex regex (regex_string);
  std::smatch match_results;
  converter.str (display_records_string);
  std::string buffer_string;
  GtkTreeIter iterator;
  do
  {
    converter.getline (buffer, sizeof (buffer));
    buffer_string = buffer;
    if (!std::regex_match (buffer_string,
                           match_results,
                           regex,
                           std::regex_constants::match_default))
      continue;
    ACE_ASSERT (match_results.ready () && !match_results.empty ());
    ACE_ASSERT (match_results[1].matched && !match_results[1].str ().empty ());

    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("found display device \"%s\"...\n"),
                ACE_TEXT (match_results[1].str ().c_str ())));

    gtk_list_store_append (listStore_in, &iterator);
    gtk_list_store_set (listStore_in, &iterator,
                        0, ACE_TEXT (match_results[1].str ().c_str ()),
                        -1);
  } while (!converter.fail ());
#endif

  return true;
}

bool
load_display_formats (GtkListStore* listStore_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::load_display_formats"));

  // initialize result
  gtk_list_store_clear (listStore_in);

  std::string format_string;
  GtkTreeIter iterator;
  do
  { // *TODO*: this needs more work; support the device capabilities exposed
    //         through the API, instead of static values
    format_string = ACE_TEXT_ALWAYS_CHAR ("H264 360p");
    gtk_list_store_append (listStore_in, &iterator);
    gtk_list_store_set (listStore_in, &iterator,
                        0, ACE_TEXT (format_string.c_str ()),
                        1, ARDRONE_VIDEOMODE_360P,
                        -1);

    format_string = ACE_TEXT_ALWAYS_CHAR ("H264 720p");
    gtk_list_store_append (listStore_in, &iterator);
    gtk_list_store_set (listStore_in, &iterator,
                        0, ACE_TEXT (format_string.c_str ()),
                        1, ARDRONE_VIDEOMODE_720P,
                        -1);

    break;
  } while (true);

  return true;
}

bool
load_save_formats (GtkListStore* listStore_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::load_save_formats"));

  // initialize result
  gtk_list_store_clear (listStore_in);

  std::string format_string;
  GtkTreeIter iterator;
  do
  { // *TODO*: this needs more work; support the device capabilities exposed
    //         through the API, instead of static values
    // *TODO*: define/activate a 'save-to-file' subpipeline (use a multiplexer)
    //         and forward(/encapsulate) the byte-stream as default format
    format_string = ACE_TEXT_ALWAYS_CHAR ("RGB");
    gtk_list_store_append (listStore_in, &iterator);
    gtk_list_store_set (listStore_in, &iterator,
                        0, ACE_TEXT (format_string.c_str ()),
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                        1, ACE_TEXT (Stream_Module_Decoder_Tools::GUIDToString (MEDIASUBTYPE_RGB24).c_str ()),
#else
                        2, AV_PIX_FMT_RGBA,
#endif
                        -1);
    break;
  } while (true);

  return true;
}

// -----------------------------------------------------------------------------

ACE_THR_FUNC_RETURN
stream_processing_function (void* arg_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::stream_processing_function"));

  ACE_THR_FUNC_RETURN result;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = std::numeric_limits<unsigned long>::max ();
#else
  result = arg_in;
#endif

  Common_UI_GTKBuildersIterator_t iterator;
  //ACE_SYNCH_MUTEX* lock_p = NULL;
  struct ARDrone_ThreadData* data_p =
      static_cast<struct ARDrone_ThreadData*> (arg_in);

  // sanity check(s)
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->CBData);
  ACE_ASSERT (data_p->CBData->configuration);

  iterator =
    data_p->CBData->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  //lock_p = &data_p->CBData->lock;

  // sanity check(s)
  ACE_ASSERT (iterator != data_p->CBData->builders.end ());

  GtkStatusbar* statusbar_p = NULL;
  ARDrone_ModuleHandlerConfigurationsIterator_t iterator_3;
  struct ARDrone_ModuleHandlerConfiguration* configuration_p = NULL;
  std::ostringstream converter;
  const ARDrone_SessionData_t* session_data_container_p = NULL;
  const struct ARDrone_SessionData* session_data_p = NULL;
  std::string logfile_name_string;
  bool result_2 = false;
//  guint context_id = 0;

//  {
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//    ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, *lock_p, -1);
//#else
//    ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, *lock_p, std::numeric_limits<void*>::max ());
//#endif

    // configure streams and retrieve stream handles
    Net_SocketConfigurationIterator_t iterator_2 =
        data_p->CBData->configuration->socketConfigurations.begin ();
    Stream_ISession* session_p = NULL;
    ACE_Time_Value session_start_timeout =
        COMMON_TIME_NOW + ACE_Time_Value (3, 0);

    // *TODO*: bind to a specific interface
    logfile_name_string =
        data_p->CBData->configuration->moduleHandlerConfiguration.targetFileName;
    data_p->CBData->configuration->moduleHandlerConfiguration.targetFileName =
        Common_File_Tools::getLogFilename (ACE_TEXT_ALWAYS_CHAR (ARDRONE_PACKAGE),
                                           ACE_TEXT_ALWAYS_CHAR (ARDRONE_CONTROL_LOG_FILE_PREFIX));
    data_p->CBData->configuration->moduleHandlerConfiguration.stream =
      data_p->CBData->controlStream;
    //result_2 =
    //  data_p->CBData->controlStream->initialize (data_p->CBData->configuration->streamConfiguration);
    //if (!result_2)
    //{
    //  ACE_DEBUG ((LM_ERROR,
    //              ACE_TEXT ("failed to initialize control stream: \"%m\", aborting\n")));
    //  goto done;
    //} // end IF
//    session_p = dynamic_cast<Stream_ISession*> (data_p->CBData->controlStream);
//    ACE_ASSERT (session_p);
    //data_p->CBData->controlStream->start ();
    // *IMPORTANT NOTE*: race condition here --> add timeout
//    session_p->wait (false,
//                     &session_start_timeout);
    data_p->CBData->configuration->moduleHandlerConfiguration.targetFileName =
        logfile_name_string;

    iterator_2++;
    if (data_p->CBData->configuration->streamConfiguration.useReactor)
      data_p->CBData->configuration->listenerConfiguration.address =
          (*iterator_2).address;
    logfile_name_string =
        data_p->CBData->configuration->moduleHandlerConfiguration.targetFileName;
    data_p->CBData->configuration->moduleHandlerConfiguration.targetFileName =
        Common_File_Tools::getLogFilename (ACE_TEXT_ALWAYS_CHAR (ARDRONE_PACKAGE),
                                           ACE_TEXT_ALWAYS_CHAR (ARDRONE_MAVLINK_LOG_FILE_PREFIX));
    data_p->CBData->configuration->moduleHandlerConfiguration.stream =
      data_p->CBData->MAVLinkStream;
    //result_2 =
    //  data_p->CBData->MAVLinkStream->initialize (data_p->CBData->configuration->streamConfiguration);
    //if (!result_2)
    //{
    //  ACE_DEBUG ((LM_ERROR,
    //              ACE_TEXT ("failed to initialize MAVLink stream: \"%m\", aborting\n")));
    //  goto done;
    //} // end IF
    //session_p = dynamic_cast<Stream_ISession*> (data_p->CBData->MAVLinkStream);
    //ACE_ASSERT (session_p);
    //data_p->CBData->MAVLinkStream->start ();
    // *IMPORTANT NOTE*: race condition here --> add timeout
    //session_p->wait (false,
    //                 &session_start_timeout);
    data_p->CBData->configuration->moduleHandlerConfiguration.targetFileName =
        logfile_name_string;

    iterator_2++;
    iterator_2++;
    // *TODO*: bind to a specific interface
    if (data_p->CBData->configuration->streamConfiguration.useReactor)
      data_p->CBData->configuration->listenerConfiguration.address =
          (*iterator_2).address;
    data_p->CBData->configuration->moduleHandlerConfiguration.stream =
      data_p->CBData->NavDataStream;
    iterator_3 =
        data_p->CBData->configuration->streamConfiguration.moduleHandlerConfigurations.find (ACE_TEXT_ALWAYS_CHAR ("Controller"));
    ACE_ASSERT (iterator_3 != data_p->CBData->configuration->streamConfiguration.moduleHandlerConfigurations.end ());
    configuration_p =
          const_cast<struct ARDrone_ModuleHandlerConfiguration*> (static_cast<const struct ARDrone_ModuleHandlerConfiguration*> ((*iterator_3).second));
    ACE_ASSERT (configuration_p);
    configuration_p->targetFileName =
        Common_File_Tools::getLogFilename (ACE_TEXT_ALWAYS_CHAR (ARDRONE_PACKAGE),
                                           ACE_TEXT_ALWAYS_CHAR (ARDRONE_NAVDATA_LOG_FILE_PREFIX));
    configuration_p->stream = data_p->CBData->NavDataStream;
    //result_2 =
    //  data_p->CBData->NavDataStream->initialize (data_p->CBData->configuration->streamConfiguration);
    //if (!result_2)
    //{
    //  ACE_DEBUG ((LM_ERROR,
    //              ACE_TEXT ("failed to initialize NavData stream: \"%m\", aborting\n")));
    //  goto done;
    //} // end IF
    //data_p->CBData->NavDataStream->start ();
    session_p = dynamic_cast<Stream_ISession*> (data_p->CBData->NavDataStream);
    ACE_ASSERT (session_p);
    // *IMPORTANT NOTE*: race condition here --> add timeout
    session_p->wait (false,
                     &session_start_timeout);

//    iterator_2++;
    data_p->CBData->configuration->moduleHandlerConfiguration.stream =
      data_p->CBData->liveVideoStream;
    result_2 =
      data_p->CBData->liveVideoStream->initialize (data_p->CBData->configuration->streamConfiguration);
    if (!result_2)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to initialize video stream: \"%m\", aborting\n")));
      goto done;
    } // end IF

    session_data_container_p = &data_p->CBData->liveVideoStream->get ();
    ACE_ASSERT (session_data_container_p);
    session_data_p =
      &const_cast<struct ARDrone_SessionData&> (session_data_container_p->get ());
    ACE_ASSERT (session_data_p);
    converter.clear ();
    converter.str (ACE_TEXT_ALWAYS_CHAR (""));
    converter << session_data_p->sessionID;

//    // set context ID
    gdk_threads_enter ();
    statusbar_p =
      GTK_STATUSBAR (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_STATUSBAR)));
    ACE_ASSERT (statusbar_p);
    data_p->CBData->configuration->moduleHandlerConfiguration.contextId =
        gtk_statusbar_get_context_id (statusbar_p,
                                      converter.str ().c_str ());
    gdk_threads_leave ();
//  } // end lock scope
    data_p->CBData->liveVideoStream->start ();

  //    if (!stream_p->isRunning ())
  //    {
  //      ACE_DEBUG ((LM_ERROR,
  //                  ACE_TEXT ("failed to start stream, aborting\n")));
  //      return;
  //    } // end IF
  data_p->CBData->controlStream->wait (true, false, false);
  data_p->CBData->MAVLinkStream->wait (true, false, false);
  data_p->CBData->NavDataStream->wait (true, false, false);
  data_p->CBData->liveVideoStream->wait (true, false, false);

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  result = 0;
#else
  result = NULL;
#endif

done:
  { // synch access
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, data_p->CBData->lock, -1);
#else
    ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, data_p->CBData->lock, std::numeric_limits<void*>::max ());
#endif

    data_p->CBData->progressData->completedActions.insert (data_p->eventSourceID);
  } // end lock scope

  // clean up
  delete data_p;

  return result;
}

// -----------------------------------------------------------------------------

gboolean
idle_initialize_ui_cb (gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::idle_initialize_ui_cb"));

  struct ARDrone_GtkCBData* cb_data_p =
    static_cast<struct ARDrone_GtkCBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (cb_data_p);
  ACE_ASSERT (cb_data_p->configuration);
  ACE_ASSERT (!cb_data_p->configuration->socketConfigurations.empty ());
  //ACE_ASSERT (cb_data_p->MAVLinkStream);
  //ACE_ASSERT (cb_data_p->liveVideoStream);

  Common_UI_GTKBuildersIterator_t iterator =
    cb_data_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != cb_data_p->builders.end ());

  GtkDialog* dialog_p =
    GTK_DIALOG (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_DIALOG_MAIN)));
  ACE_ASSERT (dialog_p);

  GtkAboutDialog* about_dialog_p =
    GTK_ABOUT_DIALOG (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_DIALOG_ABOUT)));
  ACE_ASSERT (about_dialog_p);

  GtkEntry* entry_p =
    GTK_ENTRY (gtk_builder_get_object ((*iterator).second.second,
                                       ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_ENTRY_ADDRESS)));
  ACE_ASSERT (entry_p);
  gtk_entry_set_text (entry_p,
                      Net_Common_Tools::IPAddressToString (cb_data_p->configuration->socketConfigurations.back ().address,
                                                           true).c_str ());

  GtkSpinButton* spin_button_p =
    GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_BUFFERSIZE)));
  ACE_ASSERT (spin_button_p);
  gtk_spin_button_set_range (spin_button_p,
                             0.0,
                             std::numeric_limits<unsigned int>::max ());
  gtk_spin_button_set_value (spin_button_p,
                             cb_data_p->configuration->allocatorConfiguration.defaultBufferSize);

  GtkListStore* list_store_p =
    GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                            ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_LISTSTORE_DISPLAY_DEVICE)));
  ACE_ASSERT (list_store_p);
  gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (list_store_p),
                                        1, GTK_SORT_DESCENDING);
  if (!load_display_devices (list_store_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::load_display_devices(), aborting\n")));
    return G_SOURCE_REMOVE;
  } // end IF
  GtkComboBox* combo_box_p =
    GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                           ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_COMBOBOX_DISPLAY_DEVICE)));
  ACE_ASSERT (combo_box_p);
  //gtk_combo_box_set_model (combo_box_p,
  //                         GTK_TREE_MODEL (list_store_p));
  GtkCellRenderer* cell_renderer_p = gtk_cell_renderer_text_new ();
  if (!cell_renderer_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to gtk_cell_renderer_text_new(), aborting\n")));
    return G_SOURCE_REMOVE;
  } // end IF
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo_box_p), cell_renderer_p,
                              true);
  // *NOTE*: cell_renderer_p does not need to be g_object_unref()ed because it
  //         is GInitiallyUnowned and the floating reference has been
  //         passed to combo_box_p by the gtk_cell_layout_pack_start() call
  gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo_box_p), cell_renderer_p,
                                  //"cell-background", 0,
                                  //"text", 1,
                                  "text", 0,
                                  NULL);
  list_store_p =
    GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                            ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_LISTSTORE_DISPLAY_FORMAT)));
  ACE_ASSERT (list_store_p);
  gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (list_store_p),
                                        1, GTK_SORT_ASCENDING);
  if (!load_display_formats (list_store_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::load_display_formats(), aborting\n")));
    return G_SOURCE_REMOVE;
  } // end IF
  combo_box_p =
    GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                           ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_COMBOBOX_DISPLAY_FORMAT)));
  ACE_ASSERT (combo_box_p);
  //gtk_combo_box_set_model (combo_box_p,
  //                         GTK_TREE_MODEL (list_store_p));
  cell_renderer_p = gtk_cell_renderer_text_new ();
  if (!cell_renderer_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to gtk_cell_renderer_text_new(), aborting\n")));
    return G_SOURCE_REMOVE;
  } // end IF
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo_box_p), cell_renderer_p,
                              true);
  // *NOTE*: cell_renderer_p does not need to be g_object_unref()ed because it
  //         is GInitiallyUnowned and the floating reference has been
  //         passed to combo_box_p by the gtk_cell_layout_pack_start() call
  gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo_box_p), cell_renderer_p,
                                  //"cell-background", 0,
                                  //"text", 1,
                                  "text", 0,
                                  NULL);

  list_store_p =
    GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                            ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_LISTSTORE_SAVE_FORMAT)));
  ACE_ASSERT (list_store_p);
  gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (list_store_p),
                                        1, GTK_SORT_ASCENDING);

  //Stream_Module_t* module_p =
  //  const_cast<Stream_Module_t*> (cb_data_p->stream->find (ACE_TEXT_ALWAYS_CHAR ("Display")));
  //if (!module_p)
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to retrieve \"%s\" module handle, aborting\n"),
  //              ACE_TEXT ("Display")));
  //  return G_SOURCE_REMOVE;
  //} // end IF
  //IMFMediaSource* media_source_p =
  //  dynamic_cast<IMFMediaSource*> (module_p->writer ());
  //if (!media_source_p)
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("dynamic_cast<ARDrone_Module_Display*> failed, aborting\n")));
  //  return G_SOURCE_REMOVE;
  //} // end IF
  if (!load_save_formats (//media_source_p,
                          list_store_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::load_save_formats(), aborting\n")));
    return G_SOURCE_REMOVE;
  } // end IF
  combo_box_p =
    GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                           ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_COMBOBOX_SAVE_FORMAT)));
  ACE_ASSERT (combo_box_p);
  //gtk_combo_box_set_model (combo_box_p,
  //                         GTK_TREE_MODEL (list_store_p));
  cell_renderer_p = gtk_cell_renderer_text_new ();
  if (!cell_renderer_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to gtk_cell_renderer_text_new(), aborting\n")));
    return G_SOURCE_REMOVE;
  } // end IF
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo_box_p), cell_renderer_p,
                              true);
  // *NOTE*: cell_renderer_p does not need to be g_object_unref()ed because it
  //         is GInitiallyUnowned and the floating reference has been
  //         passed to combo_box_p by the gtk_cell_layout_pack_start() call
  gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo_box_p), cell_renderer_p,
                                  //"cell-background", 0,
                                  //"text", 1,
                                  "text", 0,
                                  NULL);

  GtkFileChooserButton* file_chooser_button_p =
    GTK_FILE_CHOOSER_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                     ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_FILECHOOSERBUTTON_SAVE)));
  ACE_ASSERT (file_chooser_button_p);
  //struct _GValue property_s = G_VALUE_INIT;
  //g_value_init (&property_s,
  //              G_TYPE_POINTER);
  //g_object_get_property (G_OBJECT (file_chooser_button_p),
  //                       ACE_TEXT_ALWAYS_CHAR ("dialog"),
  //                       &property_s);
  //G_VALUE_HOLDS_POINTER (&property_s);
  //GtkFileChooser* file_chooser_p = NULL;
    //reinterpret_cast<GtkFileChooser*> (g_value_get_pointer (&property_s));
  //g_object_get (G_OBJECT (file_chooser_button_p),
  //              ACE_TEXT_ALWAYS_CHAR ("dialog"),
  //              &file_chooser_p, NULL);
  //ACE_ASSERT (file_chooser_p);
  //ACE_ASSERT (GTK_IS_FILE_CHOOSER_DIALOG (file_chooser_p));
  //GtkFileChooserDialog* file_chooser_dialog_p =
  //  GTK_FILE_CHOOSER_DIALOG (file_chooser_p);
  //ACE_ASSERT (file_chooser_dialog_p);
  //GtkPlacesSidebar* places_sidebar_p = NULL;
  //Common_UI_Tools::dump (GTK_WIDGET (file_chooser_dialog_p));
  //[0].get_children ()[0].get_children ([0].get_children ()[0]
  //  vbox.get_children ()[0].hide ()

  //GError* error_p = NULL;
  //GFile* file_p = NULL;
  struct _GString* string_p = NULL;
  gchar* filename_p = NULL;
  if (!cb_data_p->configuration->moduleHandlerConfiguration.targetFileName.empty ())
  {
    // *NOTE*: gtk does not complain if the file doesn't exist, but the button
    //         will display "(None)" --> create empty file
    if (!Common_File_Tools::isReadable (cb_data_p->configuration->moduleHandlerConfiguration.targetFileName))
      if (!Common_File_Tools::create (cb_data_p->configuration->moduleHandlerConfiguration.targetFileName))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Common_File_Tools::create(\"%s\"): \"%m\", aborting\n"),
                    ACE_TEXT (cb_data_p->configuration->moduleHandlerConfiguration.targetFileName.c_str ())));
        return G_SOURCE_REMOVE;
      } // end IF
    //file_p =
    //  g_file_new_for_path (data_p->configuration->moduleHandlerConfiguration.targetFileName.c_str ());
    //ACE_ASSERT (file_p);
    //ACE_ASSERT (g_file_query_exists (file_p, NULL));

    //std::string file_uri =
    //  ACE_TEXT_ALWAYS_CHAR ("file://") +
    //  data_p->configuration->moduleHandlerConfiguration.targetFileName;
    //if (!gtk_file_chooser_set_current_folder_uri (GTK_FILE_CHOOSER (file_chooser_button_p),
    //                                              file_uri.c_str ()))
    string_p =
      g_string_new (cb_data_p->configuration->moduleHandlerConfiguration.targetFileName.c_str ());
    filename_p = string_p->str;
      //Common_UI_Tools::Locale2UTF8 (cb_data_p->configuration->moduleHandlerConfiguration.targetFileName);
    if (!gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (file_chooser_button_p),
                                        filename_p))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to gtk_file_chooser_set_filename(\"%s\"): \"%s\", aborting\n"),
                  ACE_TEXT (cb_data_p->configuration->moduleHandlerConfiguration.targetFileName.c_str ())));

      // clean up
      g_string_free (string_p, FALSE);
      g_free (filename_p);

      return G_SOURCE_REMOVE;
    } // end IF
    g_string_free (string_p, FALSE);
    g_free (filename_p);

    //if (!gtk_file_chooser_select_file (GTK_FILE_CHOOSER (file_chooser_dialog_p),
    //                                   file_p,
    //                                   &error_p))
    //{
    //  ACE_DEBUG ((LM_ERROR,
    //              ACE_TEXT ("failed to gtk_file_chooser_select_file(\"%s\"): \"%s\", aborting\n"),
    //              ACE_TEXT (data_p->configuration->moduleHandlerConfiguration.targetFileName.c_str ()),
    //              ACE_TEXT (error_p->message)));

    //  // clean up
    //  g_error_free (error_p);
    //  g_object_unref (file_p);

    //  return G_SOURCE_REMOVE;
    //} // end IF
    //g_object_unref (file_p);
  } // end IF
  else
  {
    //file_p =
    //  g_file_new_for_path (Common_File_Tools::getTempDirectory ().c_str ());
    //ACE_ASSERT (file_p);

    string_p = g_string_new (Common_File_Tools::getTempDirectory ().c_str ());
    filename_p = string_p->str;
      //Common_UI_Tools::Locale2UTF8 (Common_File_Tools::getTempDirectory ());
    if (!gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (file_chooser_button_p),
                                        filename_p))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to gtk_file_chooser_set_filename(\"%s\"): \"%s\", aborting\n"),
                  ACE_TEXT (Common_File_Tools::getTempDirectory ().c_str ())));

      // clean up
      g_string_free (string_p, FALSE);
      g_free (filename_p);

      return G_SOURCE_REMOVE;
    } // end IF
    g_string_free (string_p, FALSE);
    g_free (filename_p);
    //g_object_unref (file_p);
  } // end ELSE

  std::string default_folder_uri = ACE_TEXT_ALWAYS_CHAR ("file://");
  default_folder_uri +=
    cb_data_p->configuration->moduleHandlerConfiguration.targetFileName;
  if (!gtk_file_chooser_set_current_folder_uri (GTK_FILE_CHOOSER (file_chooser_button_p),
                                                default_folder_uri.c_str ()))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_file_chooser_set_current_folder_uri(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (default_folder_uri.c_str ())));
    return G_SOURCE_REMOVE;
  } // end IF

  GtkCheckButton* check_button_p =
    GTK_CHECK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_CHECKBUTTON_FULLSCREEN)));
  ACE_ASSERT (check_button_p);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_button_p),
                                cb_data_p->configuration->moduleHandlerConfiguration.fullScreen);
  check_button_p =
    GTK_CHECK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_CHECKBUTTON_ASYNCH)));
  ACE_ASSERT (check_button_p);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_button_p),
                                !cb_data_p->configuration->signalHandlerConfiguration.useReactor);
  check_button_p =
    GTK_CHECK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_CHECKBUTTON_SAVE)));
  ACE_ASSERT (check_button_p);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_button_p),
                                !cb_data_p->configuration->moduleHandlerConfiguration.targetFileName.empty ());

  spin_button_p =
    GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_SESSIONMESSAGES)));
  ACE_ASSERT (spin_button_p);
  gtk_spin_button_set_range (spin_button_p,
                             0.0,
                             std::numeric_limits<unsigned int>::max ());
  spin_button_p =
    GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_DATAMESSAGES)));
  ACE_ASSERT (spin_button_p);
  gtk_spin_button_set_range (spin_button_p,
                             0.0,
                             std::numeric_limits<unsigned int>::max ());
  spin_button_p =
    GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_DATA)));
  ACE_ASSERT (spin_button_p);
  gtk_spin_button_set_range (spin_button_p,
                             0.0,
                             std::numeric_limits<unsigned int>::max ());
  spin_button_p =
    GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_CONNECTIONS)));
  ACE_ASSERT (spin_button_p);
  gtk_spin_button_set_range (spin_button_p,
                             0.0,
                             std::numeric_limits<unsigned int>::max ());

  // step4: initialize text view, setup auto-scrolling
  //GtkTextView* text_view_p =
  //  GTK_TEXT_VIEW (gtk_builder_get_object ((*iterator).second.second,
  //                                         ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_TEXTVIEW_LOG)));
  //ACE_ASSERT (text_view_p);
//  GtkTextBuffer* buffer_p =
////    gtk_text_buffer_new (NULL); // text tag table --> create new
//      gtk_text_view_get_buffer (view_p);
//  ACE_ASSERT (buffer_p);
////  gtk_text_view_set_buffer (view_p, buffer_p);

  //PangoFontDescription* font_description_p =
  //  pango_font_description_from_string (ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_GTK_PANGO_LOG_FONT_DESCRIPTION));
  //if (!font_description_p)
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to pango_font_description_from_string(\"%s\"): \"%m\", aborting\n"),
  //              ACE_TEXT (ARDRONE_UI_GTK_PANGO_LOG_FONT_DESCRIPTION)));
  //  return G_SOURCE_REMOVE;
  //} // end IF
  //// apply font
  //GtkRcStyle* rc_style_p = gtk_rc_style_new ();
  //if (!rc_style_p)
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to gtk_rc_style_new(): \"%m\", aborting\n")));
  //  return G_SOURCE_REMOVE;
  //} // end IF
  //rc_style_p->font_desc = font_description_p;
  //GdkColor base_colour, text_colour;
  //gdk_color_parse (ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_GTK_PANGO_LOG_COLOR_BASE),
  //                 &base_colour);
  //rc_style_p->base[GTK_STATE_NORMAL] = base_colour;
  //gdk_color_parse (ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_GTK_PANGO_LOG_COLOR_TEXT),
  //                 &text_colour);
  //rc_style_p->text[GTK_STATE_NORMAL] = text_colour;
  //rc_style_p->color_flags[GTK_STATE_NORMAL] =
  //  static_cast<GtkRcFlags> (GTK_RC_BASE |
  //                           GTK_RC_TEXT);
  //gtk_widget_modify_style (GTK_WIDGET (text_view_p),
  //                         rc_style_p);
  ////gtk_rc_style_unref (rc_style_p);
  //g_object_unref (rc_style_p);

  //  GtkTextIter iterator;
  //  gtk_text_buffer_get_end_iter (buffer_p,
  //                                &iterator);
  //  gtk_text_buffer_create_mark (buffer_p,
  //                               ACE_TEXT_ALWAYS_CHAR (NET_UI_SCROLLMARK_NAME),
  //                               &iterator,
  //                               TRUE);
  //  g_object_unref (buffer_p);

  GtkWidget* drawing_area_p =
      GTK_WIDGET (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_DRAWINGAREA_VIDEO)));
  ACE_ASSERT (drawing_area_p);

  GtkProgressBar* progress_bar_p =
    GTK_PROGRESS_BAR (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_PROGRESS_BAR)));
  ACE_ASSERT (progress_bar_p);
  gtk_progress_bar_set_text (progress_bar_p, ACE_TEXT_ALWAYS_CHAR (""));
  gint width, height;
  gtk_widget_get_size_request (GTK_WIDGET (progress_bar_p), &width, &height);
  gtk_progress_bar_set_pulse_step (progress_bar_p,
                                   1.0 / static_cast<double> (width));

  GtkStatusbar* statusbar_p =
      GTK_STATUSBAR (gtk_builder_get_object ((*iterator).second.second,
                                           ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_STATUSBAR)));
  ACE_ASSERT (statusbar_p);
  cb_data_p->contextIdData =
      gtk_statusbar_get_context_id (statusbar_p,
                                    ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_STATUSBAR_CONTEXT_DATA));
  cb_data_p->contextIdInformation =
    gtk_statusbar_get_context_id (statusbar_p,
                                  ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_STATUSBAR_CONTEXT_INFORMATION));

  // step2: (auto-)connect signals/slots
  gtk_builder_connect_signals ((*iterator).second.second,
                               cb_data_p);

  gulong result =
    g_signal_connect (G_OBJECT (dialog_p),
                      ACE_TEXT_ALWAYS_CHAR ("destroy"),
                      G_CALLBACK (button_quit_clicked_cb),
                      cb_data_p);
//                   G_CALLBACK(gtk_widget_destroyed),
//                   &main_dialog_p,
//  g_signal_connect (G_OBJECT (dialog_p),
//                    ACE_TEXT_ALWAYS_CHAR ("delete-event"),
//                    G_CALLBACK (delete_event_cb),
//                    NULL);
  ACE_ASSERT (result);

  result = g_signal_connect_swapped (G_OBJECT (about_dialog_p),
                                     ACE_TEXT_ALWAYS_CHAR ("response"),
                                     G_CALLBACK (gtk_widget_hide),
                                     about_dialog_p);
  ACE_ASSERT (result);

  // step3: initialize OpenGL
#if defined (GTKGL_SUPPORT)
#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
  GError* error_p = NULL;
  GtkGLArea* gl_area_p = GTK_GL_AREA (gtk_gl_area_new ());
  if (!gl_area_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to gtk_gl_area_new(), aborting\n")));
    return G_SOURCE_REMOVE;
  } // end ELSE

  //gint major_version, minor_version;
  //gtk_gl_area_get_required_version (gl_area_p, &major_version, &minor_version);
//  gtk_gl_area_set_required_version (gl_area_p, 2, 1);
#else
  /* Attribute list for gtkglarea widget. Specifies a
     list of Boolean attributes and enum/integer
     attribute/value pairs. The last attribute must be
     GGLA_NONE. See glXChooseVisual manpage for further
     explanation.
  */
  int attribute_list[] = {
    GGLA_RGBA,
    GGLA_RED_SIZE,   1,
    GGLA_GREEN_SIZE, 1,
    GGLA_BLUE_SIZE,  1,
    GGLA_DOUBLEBUFFER,
    GGLA_NONE
  };

  GglaArea* gl_area_p = GGLA_AREA (ggla_area_new (attribute_list));
  if (!gl_area_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ggla_area_new(): \"%m\", aborting\n")));
    return G_SOURCE_REMOVE;
  } // end ELSE
#endif

  //gtk_builder_expose_object ((*iterator).second.second,
  //                           ACE_TEXT_ALWAYS_CHAR (TEST_U_STREAM_UI_GTK_GLAREA_3D_NAME),
  //                           G_OBJECT (gl_area_p));
//  gtk_widget_set_size_request (GTK_WIDGET (gl_area_p),
//                               320, 240);
  //GtkBox* box_p =
  //  GTK_BOX (gtk_builder_get_object ((*iterator).second.second,
  //                                   ACE_TEXT_ALWAYS_CHAR (TEST_U_STREAM_UI_GTK_BOX_DISPLAY_NAME)));
  //ACE_ASSERT (box_p);
  //gtk_container_remove (GTK_CONTAINER (box_p),
  //                      GTK_WIDGET (drawing_area_2));
  //drawing_area_2 = NULL;
  //gtk_box_pack_start (box_p,
  //                    GTK_WIDGET (gl_area_p),
  //                    TRUE,
  //                    TRUE,
  //                    0);
#else
#if defined (GTKGLAREA_SUPPORT)
  /* Attribute list for gtkglarea widget. Specifies a
     list of Boolean attributes and enum/integer
     attribute/value pairs. The last attribute must be
     GGLA_NONE. See glXChooseVisual manpage for further
     explanation.
  */
  int attribute_list[] = {
    GDK_GL_RGBA,
    GDK_GL_RED_SIZE,   1,
    GDK_GL_GREEN_SIZE, 1,
    GDK_GL_BLUE_SIZE,  1,
//    GDK_GL_AUX_BUFFERS,
//    GDK_GL_BUFFER_SIZE,
    GDK_GL_DOUBLEBUFFER,
    GDK_GL_NONE
  };

  GtkGLArea* gl_area_p = GTK_GL_AREA (gtk_gl_area_new (attribute_list));
  if (!gl_area_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_gl_area_new(): \"%m\", aborting\n")));
    return G_SOURCE_REMOVE;
  } // end ELSE

//  gtk_builder_expose_object ((*iterator).second.second,
//                             ACE_TEXT_ALWAYS_CHAR (TEST_U_STREAM_UI_GTK_GLAREA_3D_NAME),
//                             G_OBJECT (gl_area_p));
//  gtk_widget_set_size_request (GTK_WIDGET (gl_area_p),
//                               320, 240);
  GtkBox* box_p =
    GTK_BOX (gtk_builder_get_object ((*iterator).second.second,
                                     ACE_TEXT_ALWAYS_CHAR (TEST_U_STREAM_UI_GTK_BOX_DISPLAY_NAME)));
  ACE_ASSERT (box_p);
  gtk_container_remove (GTK_CONTAINER (box_p),
                        GTK_WIDGET (drawing_area_2));
  drawing_area_2 = NULL;
  gtk_box_pack_start (box_p,
                      GTK_WIDGET (gl_area_p),
                      TRUE,
                      TRUE,
                      0);
#else
  GdkGLConfigMode features = static_cast<GdkGLConfigMode> (GDK_GL_MODE_DOUBLE  |
                                                           GDK_GL_MODE_ALPHA   |
                                                           GDK_GL_MODE_DEPTH   |
                                                           GDK_GL_MODE_STENCIL |
                                                           GDK_GL_MODE_ACCUM);
  GdkGLConfigMode configuration_mode =
      static_cast<GdkGLConfigMode> (GDK_GL_MODE_RGBA | features);
  GdkGLConfig* gl_config_p = gdk_gl_config_new_by_mode (configuration_mode);
  if (!gl_config_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gdk_gl_config_new_by_mode(): \"%m\", aborting\n")));
    return G_SOURCE_REMOVE;
  } // end IF

  if (!gtk_widget_set_gl_capability (GTK_WIDGET (drawing_area_2), // widget
                                     gl_config_p,                 // configuration
                                     NULL,                        // share list
                                     true,                        // direct
                                     GDK_GL_RGBA_TYPE))           // render type
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_widget_set_gl_capability(): \"%m\", aborting\n")));
    return G_SOURCE_REMOVE;
  } // end IF
#endif
#endif
#endif

  // step3b: initialize GLUT
  //glutInit (&cb_data_p->argc, cb_data_p->argv);

  // step4: connect custom signals
  //--------------------------------------

#if defined (GTKGL_SUPPORT)
#if GTK_CHECK_VERSION (3,0,0)
  //result =
  //  g_signal_connect (G_OBJECT (drawing_area_p),
  //                    ACE_TEXT_ALWAYS_CHAR ("size-allocate"),
  //                    G_CALLBACK (drawingarea_size_allocate_cb),
  //                    userData_in);
  //ACE_ASSERT (result);
  //result =
  //  g_signal_connect (G_OBJECT (drawing_area_p),
  //                    ACE_TEXT_ALWAYS_CHAR ("draw"),
  //                    G_CALLBACK (drawingarea_2d_draw_cb),
  //                    userData_in);
  //ACE_ASSERT (result);
#if GTK_CHECK_VERSION (3,16,0)
  //result =
  //  g_signal_connect (G_OBJECT (gl_area_p),
  //                    ACE_TEXT_ALWAYS_CHAR ("create-context"),
  //                    G_CALLBACK (glarea_create_context_cb),
  //                    userData_in);
  //ACE_ASSERT (result);
  //result =
  //  g_signal_connect (G_OBJECT (gl_area_p),
  //                    ACE_TEXT_ALWAYS_CHAR ("render"),
  //                    G_CALLBACK (glarea_render_cb),
  //                    userData_in);
  //ACE_ASSERT (result);
  //result =
  //  g_signal_connect (G_OBJECT (gl_area_p),
  //                    ACE_TEXT_ALWAYS_CHAR ("resize"),
  //                    G_CALLBACK (glarea_resize_cb),
  //                    userData_in);
#else
  result =
      g_signal_connect (G_OBJECT (gl_area_p),
                        ACE_TEXT_ALWAYS_CHAR ("size-allocate"),
                        G_CALLBACK (glarea_size_allocate_event_cb),
                        userData_in);
  ACE_ASSERT (result);
  result =
      g_signal_connect (G_OBJECT (gl_area_p),
                        ACE_TEXT_ALWAYS_CHAR ("draw"),
                        G_CALLBACK (glarea_draw_cb),
                        userData_in);
  ACE_ASSERT (result);
  result =
    g_signal_connect (G_OBJECT (gl_area_p),
                      ACE_TEXT_ALWAYS_CHAR ("realize"),
                      G_CALLBACK (glarea_realize_cb),
                      userData_in);
#endif
#else
  result =
      g_signal_connect (G_OBJECT (drawing_area_p),
                        ACE_TEXT_ALWAYS_CHAR ("configure-event"),
                        G_CALLBACK (drawingarea_configure_event_cb),
                        userData_in);
  ACE_ASSERT (result);
  result =
      g_signal_connect (G_OBJECT (drawing_area_p),
                        ACE_TEXT_ALWAYS_CHAR ("expose-event"),
                        G_CALLBACK (drawingarea_2d_expose_event_cb),
                        userData_in);
  ACE_ASSERT (result);
#if defined (GTKGLAREA_SUPPORT)
  result =
      g_signal_connect (G_OBJECT (gl_area_p),
                        ACE_TEXT_ALWAYS_CHAR ("configure-event"),
                        G_CALLBACK (glarea_configure_event_cb),
                        userData_in);
  ACE_ASSERT (result);
  result =
      g_signal_connect (G_OBJECT (gl_area_p),
                        ACE_TEXT_ALWAYS_CHAR ("expose-event"),
                        G_CALLBACK (glarea_expose_event_cb),
                        userData_in);
  ACE_ASSERT (result);
  result =
    g_signal_connect (G_OBJECT (gl_area_p),
                      ACE_TEXT_ALWAYS_CHAR ("realize"),
                      G_CALLBACK (glarea_realize_cb),
                      userData_in);
#else
  result =
      g_signal_connect (G_OBJECT (drawing_area_2),
                        ACE_TEXT_ALWAYS_CHAR ("configure-event"),
                        G_CALLBACK (drawingarea_configure_event_cb),
                        userData_in);
  ACE_ASSERT (result);
  result =
      g_signal_connect (G_OBJECT (drawing_area_2),
                        ACE_TEXT_ALWAYS_CHAR ("expose-event"),
                        G_CALLBACK (drawingarea_3d_expose_event_cb),
                        userData_in);
#endif
#endif
#endif
  ACE_ASSERT (result);

  //g_signal_connect (drawing_area_p,
  //                  ACE_TEXT_ALWAYS_CHAR ("key-press-event"),
  //                  G_CALLBACK (key_cb),
  //                  cb_data_p);
  //g_signal_connect (drawing_area_p,
  //                  ACE_TEXT_ALWAYS_CHAR ("motion-notify-event"),
  //                  G_CALLBACK (motion_cb),
  //                  cb_data_p);

//  // step5: use correct screen
//  if (parentWidget_in)
//    gtk_window_set_screen (GTK_WINDOW (dialog),
//                           gtk_widget_get_screen (const_cast<GtkWidget*> (//parentWidget_in)));

  // step6: draw main window
  gtk_widget_show_all (GTK_WIDGET (dialog_p));

  // step7: initialize updates
  guint event_source_id = 0;
  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, cb_data_p->lock, G_SOURCE_REMOVE);

    // schedule asynchronous updates of the log view
    //event_source_id = g_timeout_add_seconds (1,
    //                                         idle_update_log_display_cb,
    //                                         cb_data_p);
    //if (event_source_id > 0)
    //  cb_data_p->eventSourceIds.insert (event_source_id);
    //else
    //{
    //  ACE_DEBUG ((LM_ERROR,
    //              ACE_TEXT ("failed to g_timeout_add_seconds(): \"%m\", aborting\n")));
    //  return G_SOURCE_REMOVE;
    //} // end ELSE
    // schedule asynchronous updates of the info view
    event_source_id = g_timeout_add (COMMON_UI_GTK_WIDGET_UPDATE_INTERVAL,
                                     idle_update_info_display_cb,
                                     cb_data_p);
    if (event_source_id > 0)
      cb_data_p->eventSourceIds.insert (event_source_id);
    else
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to g_timeout_add(): \"%m\", aborting\n")));
      return G_SOURCE_REMOVE;
    } // end ELSE
  } // end lock scope

  // step7: initialize fps, schedule refresh
  //cb_data_p->timeStamp = COMMON_TIME_POLICY ();
  //guint opengl_refresh_rate =
  //    static_cast<guint> (ARDRONE_UI_WIDGET_GL_REFRESH_INTERVAL);
  //cb_data_p->openGLRefreshId = g_timeout_add (opengl_refresh_rate,
  //                                            process_cb,
  //                                            cb_data_p);
  //if (!cb_data_p->openGLRefreshId)
  //{
  //  ACE_DEBUG ((LM_ERROR,
  //              ACE_TEXT ("failed to g_timeout_add(): \"%m\", aborting\n")));
  //  return G_SOURCE_REMOVE;
  //} // end IF
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
  //else
  //  cb_data_p->eventSourceIds.insert (cb_data_p->openGLRefreshId);

  // step8: retrieve canvas coordinates, window handle and pixel buffer
  GdkWindow* window_p = gtk_widget_get_window (GTK_WIDGET (drawing_area_p));
  ACE_ASSERT (window_p);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_ASSERT (gdk_win32_window_is_win32 (window_p));
  //  static_cast<HWND> (GDK_WINDOW_HWND (GDK_DRAWABLE (window_p)));
  ACE_ASSERT (!cb_data_p->configuration->moduleHandlerConfiguration.window);
  cb_data_p->configuration->moduleHandlerConfiguration.window = NULL;
    //static_cast<HWND> (GDK_WINDOW_HWND (window_p));
    //gdk_win32_window_get_impl_hwnd (window_p);
  //ACE_ASSERT (cb_data_p->configuration->moduleHandlerConfiguration.window);
#else
  ACE_ASSERT (!cb_data_p->configuration->moduleHandlerConfiguration.window);
  cb_data_p->configuration->moduleHandlerConfiguration.window = window_p;
  ACE_ASSERT (cb_data_p->configuration->moduleHandlerConfiguration.window);
#endif

  // step9: activate some widgets
  GtkToggleAction* toggle_action_p =
      GTK_TOGGLE_ACTION (gtk_builder_get_object ((*iterator).second.second,
                                                  ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_TOGGLEACTION_CONNECT)));
  ACE_ASSERT (toggle_action_p);

  gint n_rows = 0;
  list_store_p =
    GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                            ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_LISTSTORE_DISPLAY_DEVICE)));
  ACE_ASSERT (list_store_p);
  n_rows = 1;
//    gtk_tree_model_iter_n_children (GTK_TREE_MODEL (list_store_p), NULL);
  if (n_rows)
  {
    combo_box_p =
      GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_COMBOBOX_DISPLAY_DEVICE)));
    ACE_ASSERT (combo_box_p);
    gtk_widget_set_sensitive (GTK_WIDGET (combo_box_p), true);
    gtk_combo_box_set_active (combo_box_p, 0);
  } // end IF
  else
    gtk_action_set_sensitive (GTK_ACTION (toggle_action_p), false);
  list_store_p =
    GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                            ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_LISTSTORE_DISPLAY_FORMAT)));
  ACE_ASSERT (list_store_p);
  n_rows = 1;
//    gtk_tree_model_iter_n_children (GTK_TREE_MODEL (list_store_p), NULL);
  if (n_rows)
  {
    combo_box_p =
      GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_COMBOBOX_DISPLAY_FORMAT)));
    ACE_ASSERT (combo_box_p);
    gtk_widget_set_sensitive (GTK_WIDGET (combo_box_p), true);
    gtk_combo_box_set_active (combo_box_p, 0);
  } // end IF
  else
    gtk_action_set_sensitive (GTK_ACTION (toggle_action_p), false);

  GtkAction* action_p =
      GTK_ACTION (gtk_builder_get_object ((*iterator).second.second,
                                          ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_ACTION_CUT)));
  ACE_ASSERT (action_p);
  gtk_action_set_sensitive (action_p, false);

  list_store_p =
    GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                            ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_LISTSTORE_SAVE_FORMAT)));
  ACE_ASSERT (list_store_p);
  n_rows =
    gtk_tree_model_iter_n_children (GTK_TREE_MODEL (list_store_p), NULL);
  if (n_rows)
  {
    combo_box_p =
      GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_COMBOBOX_SAVE_FORMAT)));
    ACE_ASSERT (combo_box_p);
    gtk_widget_set_sensitive (GTK_WIDGET (combo_box_p), true);
    gtk_combo_box_set_active (combo_box_p, 0);
  } // end IF

  bool is_active =
    !cb_data_p->configuration->moduleHandlerConfiguration.targetFileName.empty ();
  if (is_active)
  {
    GtkToggleButton* toggle_button_p =
          GTK_TOGGLE_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                     ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_CHECKBUTTON_SAVE)));
    ACE_ASSERT (toggle_button_p);
    gtk_toggle_button_set_active (toggle_button_p,
                                  true);
    GtkFrame* frame_p =
      GTK_FRAME (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_FRAME_OPTIONS_SAVE)));
    ACE_ASSERT (frame_p);
    gtk_widget_set_sensitive (GTK_WIDGET (frame_p),
                              true);
  } // end IF

  ///* Get Icons shown on buttons */
  //GtkSettings* settings_p = gtk_settings_get_default ();
  //ACE_ASSERT (settings_p);
  //gtk_settings_set_long_property (settings_p,
  //                                ACE_TEXT_ALWAYS_CHAR ("gtk-button-images"),
  //                                TRUE,
  //                                ACE_TEXT_ALWAYS_CHAR ("main"));

  // one-shot action
  return G_SOURCE_REMOVE;
}

gboolean
idle_finalize_ui_cb (gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::idle_finalize_ui_cb"));

  struct ARDrone_GtkCBData* cb_data_p =
      static_cast<struct ARDrone_GtkCBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (cb_data_p);

  // synch access
  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, cb_data_p->lock, G_SOURCE_REMOVE);

    unsigned int num_messages = cb_data_p->messages.size ();
    while (!cb_data_p->messages.empty ())
    {
      cb_data_p->messages.front ()->release ();
      cb_data_p->messages.pop_front ();
    } // end WHILE
    if (num_messages)
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("flushed %u message(s)\n"),
                  num_messages));
  } // end lock scope

  //if (cb_data_p->openGLRefreshId)
  //{
  //  g_source_remove (cb_data_p->openGLRefreshId);
  //  cb_data_p->openGLRefreshId = 0;
  //} // end iF
  cb_data_p->eventSourceIds.clear ();

  //if (glIsList (cb_data_p->openGLAxesListId))
  //{
  //  glDeleteLists (cb_data_p->openGLAxesListId, 1);
  //  cb_data_p->openGLAxesListId = 0;
  //} // end IF

  gtk_main_quit ();

  // one-shot action
  return G_SOURCE_REMOVE;
}

gboolean
idle_session_end_cb (gpointer userData_in)
{
  STREAM_TRACE (ACE_TEXT ("::idle_session_end_cb"));

  struct ARDrone_GtkCBData* data_p =
    static_cast<struct ARDrone_GtkCBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->progressData);

  Common_UI_GTKBuildersIterator_t iterator =
    data_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->builders.end ());

  // *IMPORTANT NOTE*: there are two major reasons for being here that are not
  //                   mutually exclusive, so there could be a race:
  //                   - user pressed stop
  //                   - there was an asynchronous error on the stream
  GtkToggleAction* toggle_action_p =
    GTK_TOGGLE_ACTION (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_TOGGLEACTION_CONNECT)));
  ACE_ASSERT (toggle_action_p);

  GtkFrame* frame_p =
    GTK_FRAME (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_FRAME_CONFIGURATION)));
  ACE_ASSERT (frame_p);
  gtk_widget_set_sensitive (GTK_WIDGET (frame_p), true);
  frame_p =
    GTK_FRAME (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_FRAME_OPTIONS)));
  ACE_ASSERT (frame_p);
  gtk_widget_set_sensitive (GTK_WIDGET (frame_p), true);

  // stop progress reporting ?
  ACE_ASSERT (data_p->progressData->eventSourceID);
  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, data_p->lock, G_SOURCE_REMOVE);

    if (!g_source_remove (data_p->progressData->eventSourceID))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to g_source_remove(%u), continuing\n"),
                  data_p->progressData->eventSourceID));
    data_p->eventSourceIds.erase (data_p->progressData->eventSourceID);
    data_p->progressData->eventSourceID = 0;
  } // end lock scope

  GtkProgressBar* progress_bar_p =
    GTK_PROGRESS_BAR (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_PROGRESS_BAR)));
  ACE_ASSERT (progress_bar_p);
  // *NOTE*: this disables "activity mode" (in Gtk2)
  gtk_progress_bar_set_fraction (progress_bar_p, 0.0);
  //gtk_progress_bar_set_text (progress_bar_p, ACE_TEXT_ALWAYS_CHAR (""));
  gtk_widget_set_sensitive (GTK_WIDGET (progress_bar_p), false);

  // update widgets
  un_toggling_connect = true;
  gtk_action_activate (GTK_ACTION (toggle_action_p));

  GtkAction* action_p =
    GTK_ACTION (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_ACTION_TRIM)));
  ACE_ASSERT (toggle_action_p);
  gtk_action_set_sensitive (action_p, false);
  action_p =
    GTK_ACTION (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_ACTION_CALIBRATE)));
  ACE_ASSERT (action_p);
  gtk_action_set_sensitive (action_p, false);

  return G_SOURCE_REMOVE;
}
gboolean
idle_session_start_cb (gpointer userData_in)
{
  STREAM_TRACE (ACE_TEXT ("::idle_session_start_cb"));

  struct ARDrone_GtkCBData* data_p =
    static_cast<struct ARDrone_GtkCBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  Common_UI_GTKBuildersIterator_t iterator =
    data_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->builders.end ());

  // update widgets
  GtkToggleAction* toggle_action_p =
    GTK_TOGGLE_ACTION (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_TOGGLEACTION_CONNECT)));
  ACE_ASSERT (toggle_action_p);
  gtk_action_set_sensitive (GTK_ACTION (toggle_action_p), true);
  GtkAction* action_p =
    GTK_ACTION (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_ACTION_TRIM)));
  ACE_ASSERT (toggle_action_p);
  gtk_action_set_sensitive (action_p, true);
  action_p =
    GTK_ACTION (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_ACTION_CALIBRATE)));
  ACE_ASSERT (action_p);
  gtk_action_set_sensitive (action_p, true);

  return G_SOURCE_REMOVE;
}

gboolean
idle_reset_ui_cb (gpointer userData_in)
{
  STREAM_TRACE (ACE_TEXT ("::idle_reset_ui_cb"));

  struct ARDrone_GtkCBData* data_p =
    static_cast<struct ARDrone_GtkCBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->progressData);

  Common_UI_GTKBuildersIterator_t iterator =
    data_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->builders.end ());

  GtkSpinButton* spin_button_p =
    GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_SESSIONMESSAGES)));
  ACE_ASSERT (spin_button_p);
  gtk_spin_button_set_value (spin_button_p, 0.0);
  spin_button_p =
    GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_DATAMESSAGES)));
  ACE_ASSERT (spin_button_p);
  gtk_spin_button_set_value (spin_button_p, 0.0);
  spin_button_p =
    GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_DATA)));
  ACE_ASSERT (spin_button_p);
  gtk_spin_button_set_value (spin_button_p, 0.0);

  GtkProgressBar* progress_bar_p =
    GTK_PROGRESS_BAR (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_PROGRESS_BAR)));
  ACE_ASSERT (progress_bar_p);
  gtk_progress_bar_set_text (progress_bar_p, ACE_TEXT_ALWAYS_CHAR (""));

  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, data_p->lock, G_SOURCE_REMOVE);

    data_p->progressData->statistic.bytes = 0.0F;
  } // end lock scope

  return G_SOURCE_REMOVE;
}

gboolean
idle_update_info_display_cb (gpointer userData_in)
{
  STREAM_TRACE (ACE_TEXT ("::idle_update_info_display_cb"));

  struct ARDrone_GtkCBData* data_p =
    static_cast<struct ARDrone_GtkCBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, data_p->lock, G_SOURCE_REMOVE);

  // sanity check(s)
  if (data_p->eventStack.empty ())
    return G_SOURCE_CONTINUE;

  Common_UI_GTKBuildersIterator_t iterator =
    data_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->builders.end ());

  GtkSpinButton* spin_button_p = NULL;
  bool is_session_message = false;
  for (ARDrone_EventsIterator_t iterator_2 = data_p->eventStack.begin ();
       iterator_2 != data_p->eventStack.end ();
       ++iterator_2)
  {
    switch (*iterator_2)
    {
      case ARDRONE_EVENT_CONNECT:
      {
        spin_button_p =
            GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                     ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_CONNECTIONS)));
        if (spin_button_p) // target ?
          gtk_spin_button_spin (spin_button_p,
                                GTK_SPIN_STEP_FORWARD,
                                1.0);

        spin_button_p =
          GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                   ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_SESSIONMESSAGES)));
        ACE_ASSERT (spin_button_p);

        is_session_message = true;
        break;
      }
      case ARDRONE_EVENT_DISCONNECT:
      {
        spin_button_p =
            GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                     ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_CONNECTIONS)));
        if (spin_button_p) // target ?
          gtk_spin_button_spin (spin_button_p,
                                GTK_SPIN_STEP_BACKWARD,
                                1.0);

        spin_button_p =
          GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                   ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_SESSIONMESSAGES)));
        ACE_ASSERT (spin_button_p);

        is_session_message = true;
        break;
      }
      case ARDRONE_EVENT_MESSAGE:
      {
        spin_button_p =
          GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                   ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_DATA)));
        ACE_ASSERT (spin_button_p);
        gtk_spin_button_set_value (spin_button_p,
                                   static_cast<gdouble> (data_p->progressData->statistic.bytes));

        spin_button_p =
            GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                     ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_DATAMESSAGES)));
        ACE_ASSERT (spin_button_p);

        break;
      }
      case ARDRONE_EVENT_RESIZE:
      {
        spin_button_p =
            GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                     ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_SESSIONMESSAGES)));
        ACE_ASSERT (spin_button_p);

        GtkDrawingArea* drawing_area_p =
            GTK_DRAWING_AREA (gtk_builder_get_object ((*iterator).second.second,
                                                      ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_DRAWINGAREA_VIDEO)));
        ACE_ASSERT (drawing_area_p);
        unsigned int height, width;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
        // sanity check(s)
        ACE_ASSERT (data_p->configuration->moduleHandlerConfiguration.filterConfiguration);
        ACE_ASSERT (data_p->configuration->moduleHandlerConfiguration.filterConfiguration->pinConfiguration);
        ACE_ASSERT (data_p->configuration->moduleHandlerConfiguration.filterConfiguration->pinConfiguration->format);

        ACE_ASSERT (data_p->configuration->moduleHandlerConfiguration.filterConfiguration->pinConfiguration->format->formattype == FORMAT_VideoInfo);
        ACE_ASSERT (data_p->configuration->moduleHandlerConfiguration.filterConfiguration->pinConfiguration->format->cbFormat == sizeof (struct tagVIDEOINFOHEADER));
        struct tagVIDEOINFOHEADER* video_info_header_p =
          reinterpret_cast<struct tagVIDEOINFOHEADER*> (data_p->configuration->moduleHandlerConfiguration.filterConfiguration->pinConfiguration->format->pbFormat);
        height = video_info_header_p->bmiHeader.biHeight;
        width = video_info_header_p->bmiHeader.biWidth;
#else
        height =
          data_p->configuration->moduleHandlerConfiguration.sourceFormat.height;
        width =
          data_p->configuration->moduleHandlerConfiguration.sourceFormat.width;
#endif
        gtk_widget_set_size_request (GTK_WIDGET (drawing_area_p),
                                     width, height);

        is_session_message = true;
        break;
      }
      case ARDRONE_EVENT_SESSION_MESSAGE:
      {
        spin_button_p =
            GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                     ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_SESSIONMESSAGES)));
        ACE_ASSERT (spin_button_p);

        is_session_message = true;
        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown event type (was: %d), continuing\n"),
                    *iterator_2));
        break;
      }
    } // end SWITCH
    ACE_UNUSED_ARG (is_session_message);
    gtk_spin_button_spin (spin_button_p,
                          GTK_SPIN_STEP_FORWARD,
                          1.0);
  } // end FOR
  data_p->eventStack.clear ();

  return G_SOURCE_CONTINUE;
}

gboolean
idle_update_log_display_cb (gpointer userData_in)
{
  STREAM_TRACE (ACE_TEXT ("::idle_update_log_display_cb"));

  struct ARDrone_GtkCBData* data_p =
    static_cast<struct ARDrone_GtkCBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, data_p->logStackLock, G_SOURCE_REMOVE);

  Common_UI_GTKBuildersIterator_t iterator =
      data_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->builders.end ());

  GtkTextView* view_p =
    GTK_TEXT_VIEW (gtk_builder_get_object ((*iterator).second.second,
                                           ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_TEXTVIEW_LOG)));
  ACE_ASSERT (view_p);
  GtkTextBuffer* buffer_p = gtk_text_view_get_buffer (view_p);
  ACE_ASSERT (buffer_p);

  GtkTextIter text_iterator;
  gtk_text_buffer_get_end_iter (buffer_p,
                                &text_iterator);

  gchar* string_p = NULL;
  // sanity check
  if (data_p->logStack.empty ())
    return G_SOURCE_CONTINUE;

  // step1: convert text
  for (Common_MessageStackConstIterator_t iterator_2 = data_p->logStack.begin ();
       iterator_2 != data_p->logStack.end ();
       ++iterator_2)
  {
    string_p = Common_UI_Tools::Locale2UTF8 (*iterator_2);
    if (!string_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to convert message text (was: \"%s\"), aborting\n"),
                  ACE_TEXT ((*iterator_2).c_str ())));
      return G_SOURCE_REMOVE;
    } // end IF

    // step2: display text
    gtk_text_buffer_insert (buffer_p,
                            &text_iterator,
                            string_p,
                            -1);

    // clean up
    g_free (string_p);
  } // end FOR

  data_p->logStack.clear ();

  // step3: scroll the view accordingly
//  // move the iterator to the beginning of line, so it doesn't scroll
//  // in horizontal direction
//  gtk_text_iter_set_line_offset (&text_iterator, 0);

//  // ...and place the mark at iter. The mark will stay there after insertion
//  // because it has "right" gravity
//  GtkTextMark* text_mark_p =
//      gtk_text_buffer_get_mark (buffer_p,
//                                ACE_TEXT_ALWAYS_CHAR (TEST_I_STREAM_UI_GTK_SCROLLMARK_NAME));
////  gtk_text_buffer_move_mark (buffer_p,
////                             text_mark_p,
////                             &text_iterator);

//  // scroll the mark onscreen
//  gtk_text_view_scroll_mark_onscreen (view_p,
//                                      text_mark_p);
  //GtkAdjustment* adjustment_p =
  //    GTK_ADJUSTMENT (gtk_builder_get_object ((*iterator).second.second,
  //                                            ACE_TEXT_ALWAYS_CHAR (TEST_I_STREAM_UI_GTK_ADJUSTMENT_NAME)));
  //ACE_ASSERT (adjustment_p);
  //gtk_adjustment_set_value (adjustment_p,
  //                          adjustment_p->upper - adjustment_p->page_size));

  return G_SOURCE_CONTINUE;
}

gboolean
idle_update_video_display_cb (gpointer userData_in)
{
  STREAM_TRACE (ACE_TEXT ("::idle_update_video_display_cb"));

  struct ARDrone_GtkCBData* data_p =
    static_cast<struct ARDrone_GtkCBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  Common_UI_GTKBuildersIterator_t iterator =
    data_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  ACE_ASSERT (iterator != data_p->builders.end ());

  GtkDrawingArea* drawing_area_p =
    GTK_DRAWING_AREA (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_DRAWINGAREA_VIDEO)));
  ACE_ASSERT (drawing_area_p);

  gdk_window_invalidate_rect (gtk_widget_get_window (GTK_WIDGET (drawing_area_p)),
                              NULL,
                              false);

  return G_SOURCE_REMOVE;
}

gboolean
idle_update_progress_cb (gpointer userData_in)
{
  STREAM_TRACE (ACE_TEXT ("::idle_update_progress_cb"));

  struct ARDrone_GtkProgressData* data_p =
    static_cast<struct ARDrone_GtkProgressData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->GTKState);

  Common_UI_GTKBuildersIterator_t iterator =
    data_p->GTKState->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->GTKState->builders.end ());

  GtkProgressBar* progress_bar_p =
    GTK_PROGRESS_BAR (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_PROGRESS_BAR)));
  ACE_ASSERT (progress_bar_p);

  ACE_TCHAR buffer[BUFSIZ];
  ACE_OS::memset (buffer, 0, sizeof (buffer));
  int result = -1;
  float fps, speed = 0.0F;
  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, data_p->GTKState->lock, G_SOURCE_REMOVE);
    fps   = data_p->statistic.messagesPerSecond;
    speed = data_p->statistic.bytesPerSecond;
  } // end lock scope
  std::string magnitude_string = ACE_TEXT_ALWAYS_CHAR ("byte(s)/s");
  if (speed)
  {
    if (speed >= 1024.0F)
    {
      speed /= 1024.0F;
      magnitude_string = ACE_TEXT_ALWAYS_CHAR ("kbyte(s)/s");
    } // end IF
    if (speed >= 1024.0F)
    {
      speed /= 1024.0F;
      magnitude_string = ACE_TEXT_ALWAYS_CHAR ("mbyte(s)/s");
    } // end IF
    result = ACE_OS::sprintf (buffer, ACE_TEXT ("%.0f fps | %.2f %s"),
                              fps, speed, magnitude_string.c_str ());
    if (result < 0)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::sprintf(): \"%m\", continuing\n")));
  } // end IF
  gtk_progress_bar_set_text (progress_bar_p,
                             ACE_TEXT_ALWAYS_CHAR (buffer));
  gtk_progress_bar_pulse (progress_bar_p);

  // --> reschedule
  return G_SOURCE_CONTINUE;
}

/////////////////////////////////////////

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
G_MODULE_EXPORT void
toggleaction_connect_toggled_cb (GtkToggleAction* toggleAction_in,
                                 gpointer userData_in)
{
  STREAM_TRACE (ACE_TEXT ("::toggleaction_connect_toggled_cb"));

  // --> user pressed connect/disconnect

  struct ARDrone_GtkCBData* cb_data_p =
    static_cast<struct ARDrone_GtkCBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (cb_data_p);
  ACE_ASSERT (cb_data_p->configuration);
  ACE_ASSERT (cb_data_p->progressData);
  ACE_ASSERT (cb_data_p->controlStream);
  ACE_ASSERT (cb_data_p->MAVLinkStream);
  ACE_ASSERT (cb_data_p->NavDataStream);
  ACE_ASSERT (cb_data_p->liveVideoStream);

  Common_UI_GTKBuildersIterator_t iterator =
    cb_data_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != cb_data_p->builders.end ());

  // handle untoggle --> PLAY
  if (un_toggling_connect)
  {
    un_toggling_connect = false;

    gtk_action_set_stock_id (GTK_ACTION (toggleAction_in), GTK_STOCK_CONNECT);
    gtk_action_set_sensitive (GTK_ACTION (toggleAction_in), true);

    // re-toggle
    guint result =
        g_signal_handlers_block_by_func (G_OBJECT (toggleAction_in),
                                         (gpointer)toggleaction_connect_toggled_cb,
                                         userData_in);
    ACE_ASSERT (result);
    GtkToggleButton* toggle_button_p =
        GTK_TOGGLE_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                   ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_TOGGLEBUTTON_CONNECT)));
    ACE_ASSERT (toggle_button_p);
    gtk_toggle_button_set_active (toggle_button_p, FALSE);
    g_signal_handlers_unblock_by_func (G_OBJECT (toggleAction_in),
                                       (gpointer)toggleaction_connect_toggled_cb,
                                       userData_in);

    return; // done
  } // end IF

  if (!gtk_toggle_action_get_active (toggleAction_in))
  {
    // stop stream
    cb_data_p->controlStream->stop (false, true);
    cb_data_p->MAVLinkStream->stop (false, true);
    cb_data_p->NavDataStream->stop (false, true);
    cb_data_p->liveVideoStream->stop (false, true);

    return;
  } // end IF

  GtkFileChooserButton* file_chooser_button_p = NULL;
  char* URI_p = NULL;
  GError* error_p = NULL;
  gchar* hostname_p = NULL;
  gchar* directory_p = NULL;
  GtkComboBox* combo_box_p = NULL;
  GtkTreeIter iterator_2;
  GtkFrame* frame_p = NULL;
  GtkProgressBar* progress_bar_p = NULL;

  bool stop_progress_reporting = false;

  struct ARDrone_ThreadData* thread_data_p = NULL;
  ACE_thread_t thread_id = -1;
  ACE_hthread_t thread_handle;
  ACE_TCHAR thread_name[BUFSIZ];
  const char* thread_name_p = NULL;
  ACE_Thread_Manager* thread_manager_p = NULL;
  int result = -1;
//  Stream_IStreamControlBase* stream_p = NULL;
  std::string interface_identifier_string;
  GdkDisplayManager* display_manager_p = NULL;
  GSList* list_p = NULL;
  GdkDisplay* display_p = NULL;
  int number_of_monitors = 0;
  int monitor_number = -1;
  int number_of_screens = 0;
  GdkScreen* screen_p = NULL;
  bool device_found = false;
  GValue value;
#if GTK_CHECK_VERSION (3,0,0)
  value = G_VALUE_INIT;
#else
  g_value_init (&value, G_TYPE_NONE);
#endif
  GtkListStore* list_store_p = NULL;
  GtkDrawingArea* drawing_area_p = NULL;
  GtkCheckButton* check_button_p = NULL;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#if GTK_CHECK_VERSION (3,0,0)
#else
  GdkPixbuf* pixbuf_p = NULL;
#endif
#endif

  // update configuration

  // retrieve address
  GtkEntry* entry_p =
      GTK_ENTRY (gtk_builder_get_object ((*iterator).second.second,
                                         ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_ENTRY_ADDRESS)));
  ACE_ASSERT (entry_p);
  GtkSpinButton* spin_button_p =
      GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_PORT)));
  ACE_ASSERT (spin_button_p);
  std::string address_string = gtk_entry_get_text (entry_p);
  address_string += ACE_TEXT_ALWAYS_CHAR (':');
  std::ostringstream converter;
  converter <<
    static_cast<unsigned short> (gtk_spin_button_get_value_as_int (spin_button_p));
  address_string += converter.str ();
  result =
      cb_data_p->configuration->socketConfigurations.back ().address.set (address_string.c_str (),
                                                                          AF_INET);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_INET_Addr::set(\"%s\"): \"%m\", returning\n"),
                ACE_TEXT (address_string.c_str ())));
    goto error;
  } // end IF
  // *TODO*: verify the given address
  if (!Net_Common_Tools::IPAddressToInterface (cb_data_p->configuration->socketConfigurations.back ().address,
                                               interface_identifier_string))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_Common_Tools::IPAddressToInterface(%s), returning\n"),
                ACE_TEXT (Net_Common_Tools::IPAddressToString (cb_data_p->configuration->socketConfigurations.back ().address).c_str ())));
    goto error;
  } // end IF
  if (!Net_Common_Tools::interfaceToIPAddress (interface_identifier_string,
                                               cb_data_p->localSAP))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_Common_Tools::interfaceToIPAddress(%s), returning\n"),
                ACE_TEXT (interface_identifier_string.c_str ())));
    goto error;
  } // end IF
  ACE_DEBUG ((LM_ERROR,
              ACE_TEXT ("set local SAP: %s...\n"),
              ACE_TEXT (Net_Common_Tools::IPAddressToString (cb_data_p->localSAP).c_str ())));

  // retrieve buffer
  spin_button_p =
      GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_BUFFERSIZE)));
  ACE_ASSERT (spin_button_p);
  cb_data_p->configuration->allocatorConfiguration.defaultBufferSize =
      static_cast<unsigned int> (gtk_spin_button_get_value_as_int (spin_button_p));

  // set fullscreen ?
  check_button_p =
      GTK_CHECK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_CHECKBUTTON_FULLSCREEN)));
  ACE_ASSERT (check_button_p);
  cb_data_p->configuration->moduleHandlerConfiguration.fullScreen =
    gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (check_button_p));

  // retrieve filename ?
  check_button_p =
      GTK_CHECK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_CHECKBUTTON_SAVE)));
  ACE_ASSERT (check_button_p);
  if (!gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (check_button_p)))
  {
    cb_data_p->configuration->moduleHandlerConfiguration.targetFileName.clear ();

    goto continue_;
  } // end IF

  file_chooser_button_p =
      GTK_FILE_CHOOSER_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                       ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_FILECHOOSERBUTTON_SAVE)));
  ACE_ASSERT (file_chooser_button_p);
  URI_p =
      gtk_file_chooser_get_uri (GTK_FILE_CHOOSER (file_chooser_button_p));
  if (!URI_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_file_chooser_get_uri(), returning\n")));
    goto error;
  } // end IF
  directory_p = g_filename_from_uri (URI_p,
                                     &hostname_p,
                                     &error_p);
  g_free (URI_p);
  if (!directory_p)
  { ACE_ASSERT (error_p);
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to g_filename_from_uri(): \"%s\", returning\n"),
                ACE_TEXT (error_p->message)));

    // clean up
    g_error_free (error_p);

    goto error;
  } // end IF
  ACE_ASSERT (!hostname_p);
  cb_data_p->configuration->moduleHandlerConfiguration.targetFileName =
      directory_p;
  g_free (directory_p);
  ACE_ASSERT (Common_File_Tools::isDirectory (cb_data_p->configuration->moduleHandlerConfiguration.targetFileName));
  cb_data_p->configuration->moduleHandlerConfiguration.targetFileName +=
      ACE_DIRECTORY_SEPARATOR_STR;
  cb_data_p->configuration->moduleHandlerConfiguration.targetFileName +=
      ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_FILE_NAME);

continue_:
  // retrieve save format
  combo_box_p =
      GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_COMBOBOX_SAVE_FORMAT)));
  ACE_ASSERT (combo_box_p);
  if (gtk_combo_box_get_active_iter (combo_box_p,
                                     &iterator_2))
  {
    GtkListStore* list_store_p =
        GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                                ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_LISTSTORE_SAVE_FORMAT)));
    ACE_ASSERT (list_store_p);
    GValue value = {0,};
    gtk_tree_model_get_value (GTK_TREE_MODEL (list_store_p),
                              &iterator_2,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                              1, &value);
#else
                              2, &value);
#endif
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_ASSERT (G_VALUE_TYPE (&value) == G_TYPE_STRING);
    std::string format_string = g_value_get_string (&value);
    g_value_unset (&value);
    HRESULT result = E_FAIL;
#if defined (OLE2ANSI)
    result = CLSIDFromString (format_string.c_str (),
#else
    result = CLSIDFromString (ACE_TEXT_ALWAYS_WCHAR (format_string.c_str ()),
#endif
                              &cb_data_p->configuration->moduleHandlerConfiguration.format->subtype);
    if (FAILED (result))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to CLSIDFromString(): \"%s\", returning\n"),
                  ACE_TEXT (Common_Tools::error2String (result).c_str ())));
      goto error;
    } // end IF
#else
    ACE_ASSERT (G_VALUE_TYPE (&value) == G_TYPE_INT);
    cb_data_p->configuration->moduleHandlerConfiguration.format =
      static_cast<enum AVPixelFormat> (g_value_get_int (&value));
    g_value_unset (&value);
#endif
  } // end IF

  // retrieve display settings
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  // sanity check(s)
  ACE_ASSERT (cb_data_p->configuration->moduleHandlerConfiguration.filterConfiguration);
  ACE_ASSERT (cb_data_p->configuration->moduleHandlerConfiguration.filterConfiguration->pinConfiguration);
  ACE_ASSERT (cb_data_p->configuration->moduleHandlerConfiguration.filterConfiguration->pinConfiguration->format);
  ACE_ASSERT (cb_data_p->configuration->moduleHandlerConfiguration.format);

  ACE_ASSERT (cb_data_p->configuration->moduleHandlerConfiguration.filterConfiguration->pinConfiguration->format->formattype == FORMAT_VideoInfo);
  ACE_ASSERT (cb_data_p->configuration->moduleHandlerConfiguration.filterConfiguration->pinConfiguration->format->cbFormat == sizeof (struct tagVIDEOINFOHEADER));
  struct tagVIDEOINFOHEADER* video_info_header_p =
    reinterpret_cast<struct tagVIDEOINFOHEADER*> (cb_data_p->configuration->moduleHandlerConfiguration.filterConfiguration->pinConfiguration->format->pbFormat);
  ACE_ASSERT (cb_data_p->configuration->moduleHandlerConfiguration.format->formattype == FORMAT_VideoInfo);
  ACE_ASSERT (cb_data_p->configuration->moduleHandlerConfiguration.format->cbFormat == sizeof (struct tagVIDEOINFOHEADER));
  struct tagVIDEOINFOHEADER* video_info_header_2 =
    reinterpret_cast<struct tagVIDEOINFOHEADER*> (cb_data_p->configuration->moduleHandlerConfiguration.format->pbFormat);
#endif
  drawing_area_p =
    GTK_DRAWING_AREA (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_DRAWINGAREA_VIDEO)));
  ACE_ASSERT (drawing_area_p);

  combo_box_p =
      GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_COMBOBOX_DISPLAY_DEVICE)));
  ACE_ASSERT (combo_box_p);
  if (!gtk_combo_box_get_active_iter (combo_box_p,
                                      &iterator_2))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("no available display device, returning\n")));
    goto error;
  } // end IF
  list_store_p =
      GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_LISTSTORE_DISPLAY_DEVICE)));
  ACE_ASSERT (list_store_p);
  gtk_tree_model_get_value (GTK_TREE_MODEL (list_store_p),
                            &iterator_2,
                            0, &value);
  ACE_ASSERT (G_VALUE_TYPE (&value) == G_TYPE_STRING);
  cb_data_p->configuration->moduleHandlerConfiguration.device =
    g_value_get_string (&value);
  g_value_unset (&value);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (!EnumDisplayMonitors (NULL,                                     // hdc
                            NULL,                                     // lprcClip
                            monitor_clip_cb,
                            reinterpret_cast<LPARAM> (userData_in)))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to EnumDisplayMonitors(): \"%s\", returning\n"),
                ACE_TEXT (Common_Tools::error2String (GetLastError ()).c_str ())));
    goto error;
  } // end IF
#else
  display_manager_p = gdk_display_manager_get ();
  ACE_ASSERT (display_manager_p);
  list_p = gdk_display_manager_list_displays (display_manager_p);
  ACE_ASSERT (list_p);
#if GTK_CHECK_VERSION (3,22,0)
  GdkMonitor* monitor_p = NULL;
  for (GSList* list_2 = list_p;
       list_2;
       list_2 = list_2->next)
  {
    display_p = GDK_DISPLAY (list_2->data);
    ACE_ASSERT (display_p);
    number_of_monitors = gdk_display_get_n_monitors (display_p);
    for (int i = 0;
         i < number_of_monitors;
         ++i)
    {
      monitor_p = gdk_display_get_monitor (display_p,
                                           i);
      ACE_ASSERT (monitor_p);

      if (!ACE_OS::strcmp (gdk_monitor_get_model (monitor_p),
                           cb_data_p->configuration->moduleHandlerConfiguration.device.c_str ()))
        break;
    } // end FOR
  } // end FOR
  if (!monitor_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("device not found (was: \"%s\"), returning\n"),
                ACE_TEXT (cb_data_p->configuration->moduleHandlerConfiguration.device.c_str ())));
    goto error;
  } // end IF
  display_p = gdk_monitor_get_display (monitor_p);
  ACE_ASSERT (display_p);
  screen_p = gdk_display_get_default_screen (display_p);
  ACE_ASSERT (screen_p);
#else
  for (GSList* list_2 = list_p;
       list_2;
       list_2 = list_2->next)
  {
#if defined (GTK3_SUPPORT)
    display_p = GDK_DISPLAY (list_2->data);
#else
    display_p = GDK_DISPLAY_OBJECT (list_2->data);
#endif
    ACE_ASSERT (display_p);
    number_of_screens = gdk_display_get_n_screens (display_p);
    for (int i = 0;
         i < number_of_screens;
         ++i)
    {
      screen_p = gdk_display_get_screen (display_p,
                                         i);
      ACE_ASSERT (screen_p);
      number_of_monitors = gdk_screen_get_n_monitors (screen_p);
      for (monitor_number = 0;
           monitor_number < number_of_monitors;
           ++monitor_number)
      {
        if (!ACE_OS::strcmp (ACE_TEXT (gdk_screen_get_monitor_plug_name (screen_p, monitor_number)),
                             ACE_TEXT (cb_data_p->configuration->moduleHandlerConfiguration.device.c_str ())))
        {
          device_found = true;
          break;
        } // end IF
      } // end FOR
      if (device_found)
        break;
      screen_p = NULL;
    } // end FOR
  } // end FOR
  g_slist_free (list_p);
#endif /* GTK_CHECK_VERSION (3,22,0) */
  if (!screen_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("device not found (was: \"%s\"), returning\n"),
                ACE_TEXT (cb_data_p->configuration->moduleHandlerConfiguration.device.c_str ())));
    goto error;
  } // end IF
#endif

  combo_box_p =
      GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_COMBOBOX_DISPLAY_FORMAT)));
  ACE_ASSERT (combo_box_p);
  if (!gtk_combo_box_get_active_iter (combo_box_p,
                                      &iterator_2))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("no available display format, returning\n")));
    goto error;
  } // end IF
  list_store_p =
      GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_LISTSTORE_DISPLAY_FORMAT)));
  ACE_ASSERT (list_store_p);
  value = {0,};
  gtk_tree_model_get_value (GTK_TREE_MODEL (list_store_p),
                            &iterator_2,
                            1, &value);
  ACE_ASSERT (G_VALUE_TYPE (&value) == G_TYPE_INT);

  switch (static_cast<enum ARDrone_VideoMode> (g_value_get_int (&value)))
  {
    case ARDRONE_VIDEOMODE_360P:
    { // *TODO*: use ffmpeg to determine the format resolution
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      video_info_header_p->bmiHeader.biWidth = ARDRONE_H264_360P_VIDEO_WIDTH;
      video_info_header_p->bmiHeader.biHeight = -ARDRONE_H264_360P_VIDEO_HEIGHT;
#else
      cb_data_p->configuration->moduleHandlerConfiguration.sourceFormat.height =
        ARDRONE_H264_360P_VIDEO_HEIGHT;
      cb_data_p->configuration->moduleHandlerConfiguration.sourceFormat.width =
        ARDRONE_H264_360P_VIDEO_WIDTH;
#endif
      break;
    }
    case ARDRONE_VIDEOMODE_720P:
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      video_info_header_p->bmiHeader.biWidth = ARDRONE_H264_720P_VIDEO_WIDTH;
      video_info_header_p->bmiHeader.biHeight = -ARDRONE_H264_720P_VIDEO_HEIGHT;
#else
      cb_data_p->configuration->moduleHandlerConfiguration.sourceFormat.height =
        ARDRONE_H264_720P_VIDEO_HEIGHT;
      cb_data_p->configuration->moduleHandlerConfiguration.sourceFormat.width =
        ARDRONE_H264_720P_VIDEO_WIDTH;
#endif
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown display format (was: %d), returning\n"),
                  g_value_get_int (&value)));

      // clean up
      g_value_unset (&value);

      goto error;
    }
  } // end SWITCH
  g_value_unset (&value);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  video_info_header_p->bmiHeader.biSizeImage =
    DIBSIZE (video_info_header_p->bmiHeader);
  video_info_header_p->dwBitRate =
    (video_info_header_p->bmiHeader.biWidth * abs (video_info_header_p->bmiHeader.biHeight)) * 4 * 30 * 8;
  cb_data_p->configuration->moduleHandlerConfiguration.filterConfiguration->pinConfiguration->format->lSampleSize =
    video_info_header_p->bmiHeader.biSizeImage;
  cb_data_p->configuration->directShowFilterConfiguration.allocatorProperties.cbBuffer =
    video_info_header_p->bmiHeader.biSizeImage;
#endif

#if defined (GTK3_SUPPORT)
  struct _cairo_rectangle_int rectangle_s;
#else
  GtkAllocation rectangle_s;
#endif
  gtk_widget_get_allocation (GTK_WIDGET (drawing_area_p),
                             &rectangle_s);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#endif
  if (cb_data_p->configuration->moduleHandlerConfiguration.fullScreen)
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    video_info_header_2->bmiHeader.biHeight =
      -(cb_data_p->configuration->moduleHandlerConfiguration.area.bottom -
        cb_data_p->configuration->moduleHandlerConfiguration.area.top);
    video_info_header_2->bmiHeader.biWidth =
      (cb_data_p->configuration->moduleHandlerConfiguration.area.right -
       cb_data_p->configuration->moduleHandlerConfiguration.area.left);
    video_info_header_2->bmiHeader.biSizeImage =
      DIBSIZE (video_info_header_2->bmiHeader);

    unsigned int source_buffer_size =
      av_image_get_buffer_size (Stream_Module_Decoder_Tools::mediaTypeSubTypeToAVPixelFormat (cb_data_p->configuration->moduleHandlerConfiguration.format->subtype),
                                video_info_header_p->bmiHeader.biWidth,
                                abs (video_info_header_p->bmiHeader.biHeight),
                                1); // *TODO*: linesize alignment
    video_info_header_2->bmiHeader.biSizeImage =
      std::max (video_info_header_2->bmiHeader.biSizeImage,
                static_cast<ULONG> (source_buffer_size));

    cb_data_p->configuration->moduleHandlerConfiguration.format->lSampleSize =
      video_info_header_2->bmiHeader.biSizeImage;
    cb_data_p->configuration->directShowFilterConfiguration.allocatorProperties.cbBuffer =
      video_info_header_2->bmiHeader.biSizeImage;
#else
    GtkWidget* widget_p = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    if (!widget_p)
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to gtk_window_new(), returning\n")));
      goto error;
    } // end IF
    gtk_window_set_decorated (GTK_WINDOW (widget_p), FALSE);
//        gdk_screen_get_root_window (screen_p);
//    gtk_container_set_border_width (GTK_CONTAINER (widget_p), 10);
    gtk_widget_realize (widget_p);
    gtk_widget_show (widget_p);
//    gtk_window_set_screen (GTK_WINDOW (widget_p), screen_p);
    // *NOTE*: gtk_widget_get_allocation() does not work on GtkWindows for some
    //         reason
//    gtk_widget_get_allocation (widget_p,
//                               &rectangle_s);
//    gtk_window_get_size (GTK_WINDOW (widget_p),
//                         &rectangle_s.width,
//                         &rectangle_s.height);
//    rectangle_s.width = gdk_screen_get_width (screen_p);
//    rectangle_s.height = gdk_screen_get_height (screen_p);
    gdk_screen_get_monitor_geometry (screen_p,
                                     monitor_number,
                                     &rectangle_s);
    gtk_window_move (GTK_WINDOW (widget_p),
                     rectangle_s.x, rectangle_s.y);
    gtk_window_fullscreen (GTK_WINDOW (widget_p));
    cb_data_p->configuration->moduleHandlerConfiguration.window =
        gtk_widget_get_window (widget_p);
#endif
  } // end IF
  else
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (!cb_data_p->configuration->moduleHandlerConfiguration.window)
    {
      cb_data_p->configuration->moduleHandlerConfiguration.area.left = 0;
      cb_data_p->configuration->moduleHandlerConfiguration.area.right =
        video_info_header_p->bmiHeader.biWidth;
      cb_data_p->configuration->moduleHandlerConfiguration.area.top = 0;
      cb_data_p->configuration->moduleHandlerConfiguration.area.bottom =
        abs (video_info_header_p->bmiHeader.biHeight);
      goto create_window;
    } // end IF

    // *NOTE*: if the chosen display device screen area contains the drawing
    //         area, use it; otherwise open a new window
    DWORD flags = MONITOR_DEFAULTTONULL;
    HMONITOR monitor_h =
      MonitorFromWindow (cb_data_p->configuration->moduleHandlerConfiguration.window,
                         flags);
    if (!monitor_h)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to MonitorFromWindow(): \"%s\", returning\n"),
                  ACE_TEXT (Common_Tools::error2String (GetLastError ()).c_str ())));
      goto error;
    } // end IF
    MONITORINFOEX monitor_info_ex_s;
    ACE_OS::memset (&monitor_info_ex_s, 0, sizeof (MONITORINFOEX));
    monitor_info_ex_s.cbSize = sizeof (MONITORINFOEX);
    if (!GetMonitorInfo (monitor_h,
                         reinterpret_cast<struct tagMONITORINFO*> (&monitor_info_ex_s)))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to GetMonitorInfo(): \"%s\", returning\n"),
                  ACE_TEXT (Common_Tools::error2String (GetLastError ()).c_str ())));
      goto error;
    } // end IF
    if (ACE_OS::strcmp (ACE_TEXT (cb_data_p->configuration->moduleHandlerConfiguration.device.c_str ()),
                        ACE_TEXT (monitor_info_ex_s.szDevice)))
    {
      // *NOTE*: center the window on the display device
      unsigned int delta_x =
       abs (abs (cb_data_p->configuration->moduleHandlerConfiguration.area.left) -
            rectangle_s.width) / 2;
      cb_data_p->configuration->moduleHandlerConfiguration.area.left =
        (cb_data_p->configuration->moduleHandlerConfiguration.area.left +
         delta_x);
      cb_data_p->configuration->moduleHandlerConfiguration.area.right =
        (cb_data_p->configuration->moduleHandlerConfiguration.area.left +
         rectangle_s.width);
      unsigned int delta_y =
        abs (abs (cb_data_p->configuration->moduleHandlerConfiguration.area.top) -
             rectangle_s.height) / 2;
      cb_data_p->configuration->moduleHandlerConfiguration.area.top =
        (cb_data_p->configuration->moduleHandlerConfiguration.area.top +
         delta_y);
      cb_data_p->configuration->moduleHandlerConfiguration.area.bottom =
        (cb_data_p->configuration->moduleHandlerConfiguration.area.top +
         rectangle_s.height);

create_window:
      DWORD window_style = (WS_OVERLAPPED     |
                            WS_CAPTION        |
                            (WS_CLIPSIBLINGS  |
                             WS_CLIPCHILDREN) |
                            WS_SYSMENU        |
                            //WS_THICKFRAME     |
                            WS_MINIMIZEBOX    |
                            WS_VISIBLE/*
                            WS_MAXIMIZEBOX*/);
      DWORD window_style_ex = (WS_EX_APPWINDOW |
                               WS_EX_WINDOWEDGE);

      cb_data_p->configuration->moduleHandlerConfiguration.window =
        CreateWindowEx (window_style_ex,               // dwExStyle
                        ACE_TEXT_ALWAYS_CHAR ("EDIT"), // lpClassName
                        ACE_TEXT_ALWAYS_CHAR ("EDIT"), // lpWindowName
                        window_style,                  // dwStyle
                        cb_data_p->configuration->moduleHandlerConfiguration.area.left, // x
                        cb_data_p->configuration->moduleHandlerConfiguration.area.top,  // y
                        //rectangle_s.width,             // nWidth
                        //rectangle_s.height,            // nHeight
                        video_info_header_p->bmiHeader.biWidth,
                        abs (video_info_header_p->bmiHeader.biHeight),
                        //parent_window_handle,          // hWndParent
                        NULL,
                        NULL,                          // hMenu
                        GetModuleHandle (NULL),        // hInstance
                        NULL);                         // lpParam
      if (!cb_data_p->configuration->moduleHandlerConfiguration.window)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to CreateWindow(): \"%s\", returning\n"),
                    ACE_TEXT (Common_Tools::error2String (::GetLastError ()).c_str ())));
        goto error;
      } // end IF
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("opened window (size: %ux%u, handle: 0x%@)...\n"),
                  rectangle_s.width, rectangle_s.height,
                  cb_data_p->configuration->moduleHandlerConfiguration.window));
    } // end IF
    else
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      cb_data_p->configuration->moduleHandlerConfiguration.area.bottom =
        rectangle_s.y + rectangle_s.height;
      cb_data_p->configuration->moduleHandlerConfiguration.area.left =
        rectangle_s.x;
      cb_data_p->configuration->moduleHandlerConfiguration.area.right =
        rectangle_s.x + rectangle_s.width;
      cb_data_p->configuration->moduleHandlerConfiguration.area.top =
        rectangle_s.y;
#else
      cb_data_p->configuration->moduleHandlerConfiguration.area = rectangle_s;
#endif
    } // end ELSE

    video_info_header_2->bmiHeader.biHeight = rectangle_s.height;
    video_info_header_2->bmiHeader.biWidth = rectangle_s.width;
    video_info_header_2->bmiHeader.biSizeImage =
      DIBSIZE (video_info_header_2->bmiHeader);
    video_info_header_2->dwBitRate =
      (video_info_header_2->bmiHeader.biWidth * abs (video_info_header_2->bmiHeader.biHeight)) * 4 * 30 * 8;
    cb_data_p->configuration->moduleHandlerConfiguration.format->lSampleSize =
      video_info_header_2->bmiHeader.biSizeImage;
    cb_data_p->configuration->directShowFilterConfiguration.allocatorProperties.cbBuffer =
      std::max (cb_data_p->configuration->directShowFilterConfiguration.allocatorProperties.cbBuffer,
                static_cast<long> (video_info_header_2->bmiHeader.biSizeImage));
#else
    cb_data_p->configuration->moduleHandlerConfiguration.window =
        gtk_widget_get_window (GTK_WIDGET (drawing_area_p));
#endif
  } // end ELSE
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("using display device \"%s\" [%d/%d/%d/%d]: %dx%d...\n"),
              ACE_TEXT (cb_data_p->configuration->moduleHandlerConfiguration.device.c_str ()),
              cb_data_p->configuration->moduleHandlerConfiguration.area.left,
              cb_data_p->configuration->moduleHandlerConfiguration.area.right,
              cb_data_p->configuration->moduleHandlerConfiguration.area.top,
              cb_data_p->configuration->moduleHandlerConfiguration.area.bottom,
              (cb_data_p->configuration->moduleHandlerConfiguration.area.right -
               cb_data_p->configuration->moduleHandlerConfiguration.area.left),
              (cb_data_p->configuration->moduleHandlerConfiguration.area.bottom -
               cb_data_p->configuration->moduleHandlerConfiguration.area.top)));
#else
  cb_data_p->configuration->moduleHandlerConfiguration.area = rectangle_s;
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("using display device \"%s\" (display: \"%s\", monitor: %d) [%d/%d/%d/%d]: %dx%d...\n"),
              ACE_TEXT (cb_data_p->configuration->moduleHandlerConfiguration.device.c_str ()),
              ACE_TEXT (gdk_display_get_name (display_p)),
              monitor_number,
              cb_data_p->configuration->moduleHandlerConfiguration.area.x,
              cb_data_p->configuration->moduleHandlerConfiguration.area.y,
              cb_data_p->configuration->moduleHandlerConfiguration.area.width,
              cb_data_p->configuration->moduleHandlerConfiguration.area.height,
              cb_data_p->configuration->moduleHandlerConfiguration.area.width,
              cb_data_p->configuration->moduleHandlerConfiguration.area.height));
#endif

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#if GTK_CHECK_VERSION (3,0,0)
#else
  pixbuf_p =
      gdk_pixbuf_new (GDK_COLORSPACE_RGB,
                      TRUE,
                      8,
                      cb_data_p->configuration->moduleHandlerConfiguration.area.width, cb_data_p->configuration->moduleHandlerConfiguration.area.height);
  if (!pixbuf_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gdk_pixbuf_new(), returning\n")));
    goto error;
  } // end IF
#endif
  if (cb_data_p->pixelBuffer)
  {
    g_object_unref (cb_data_p->pixelBuffer);
    cb_data_p->pixelBuffer = NULL;
  } // end IF
  cb_data_p->pixelBuffer =
#if GTK_CHECK_VERSION (3,0,0)
      gdk_pixbuf_get_from_window (cb_data_p->configuration->moduleHandlerConfiguration.window,
                                  0, 0,
                                  cb_data_p->configuration->moduleHandlerConfiguration.area.width, cb_data_p->configuration->moduleHandlerConfiguration.area.height);
#else
      gdk_pixbuf_get_from_drawable (pixbuf_p,
                                    GDK_DRAWABLE (cb_data_p->configuration->moduleHandlerConfiguration.window),
                                    NULL,
                                    0, 0,
                                    0, 0, cb_data_p->configuration->moduleHandlerConfiguration.area.width, cb_data_p->configuration->moduleHandlerConfiguration.area.height);
#endif
  if (!cb_data_p->pixelBuffer)
  { // *NOTE*: most probable reason: window is not mapped
#if GTK_CHECK_VERSION (3,0,0)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gdk_pixbuf_get_from_window(), returning\n")));
#else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gdk_pixbuf_get_from_drawable(), returning\n")));

    gdk_pixbuf_unref (pixbuf_p);
#endif
    goto error;
  } // end IF
  cb_data_p->configuration->moduleHandlerConfiguration.pixelBuffer =
    cb_data_p->pixelBuffer;
#endif

  // update widgets
  gtk_action_set_stock_id (GTK_ACTION (toggleAction_in),
                           GTK_STOCK_DISCONNECT);
  gtk_action_set_sensitive (GTK_ACTION (toggleAction_in), false);

  frame_p =
      GTK_FRAME (gtk_builder_get_object ((*iterator).second.second,
                                         ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_FRAME_CONFIGURATION)));
  ACE_ASSERT (frame_p);
  gtk_widget_set_sensitive (GTK_WIDGET (frame_p), false);
  frame_p =
      GTK_FRAME (gtk_builder_get_object ((*iterator).second.second,
                                         ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_FRAME_OPTIONS)));
  ACE_ASSERT (frame_p);
  gtk_widget_set_sensitive (GTK_WIDGET (frame_p), false);

  progress_bar_p =
      GTK_PROGRESS_BAR (gtk_builder_get_object ((*iterator).second.second,
                                                ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_PROGRESS_BAR)));
  ACE_ASSERT (progress_bar_p);
  gtk_widget_set_sensitive (GTK_WIDGET (progress_bar_p), true);

  // reset info
  idle_reset_ui_cb (userData_in);

  // start processing thread
  ACE_NEW_NORETURN (thread_data_p,
                    struct ARDrone_ThreadData ());
  if (!thread_data_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory: \"%m\", returning\n")));
    goto error;
  } // end IF
  thread_data_p->CBData = cb_data_p;

  ACE_OS::memset (thread_name, 0, sizeof (thread_name));
  //  char* thread_name_p = NULL;
  //  ACE_NEW_NORETURN (thread_name_p,
  //                    ACE_TCHAR[BUFSIZ]);
  //  if (!thread_name_p)
  //  {
  //    ACE_DEBUG ((LM_CRITICAL,
  //                ACE_TEXT ("failed to allocate memory: \"%m\", returning\n")));

  //    // clean up
  //    delete thread_data_p;

  //    return;
  //  } // end IF
  //  ACE_OS::memset (thread_name_p, 0, sizeof (thread_name_p));
  //  ACE_OS::strcpy (thread_name_p,
  //                  ACE_TEXT (TEST_I_STREAM_FILECOPY_THREAD_NAME));
  //  const char* thread_name_2 = thread_name_p;
  ACE_OS::strcpy (thread_name,
                  ACE_TEXT (ARDRONE_UI_PROCESSING_THREAD_NAME));
  thread_name_p = thread_name;
  thread_manager_p = ACE_Thread_Manager::instance ();
  ACE_ASSERT (thread_manager_p);
  result =
      thread_manager_p->spawn (::stream_processing_function,     // function
                               thread_data_p,                    // argument
                               (THR_NEW_LWP      |
                                THR_JOINABLE     |
                                THR_INHERIT_SCHED),              // flags
                               &thread_id,                       // id
                               &thread_handle,                   // handle
                               ACE_DEFAULT_THREAD_PRIORITY,      // priority
                               COMMON_EVENT_THREAD_GROUP_ID + 2, // *TODO*: group id
                               NULL,                             // stack
                               0,                                // stack size
                               &thread_name_p);                  // name
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Thread_Manager::spawn(): \"%m\", returning\n")));
    goto error;
  } // end IF

  // start progress reporting
  // *TODO*: there is a race condition here if the processing thread returns
  //         early
  ACE_ASSERT (!cb_data_p->progressData->eventSourceID);
  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, cb_data_p->lock);

    cb_data_p->progressData->eventSourceID =
        //g_idle_add_full (G_PRIORITY_DEFAULT_IDLE, // _LOW doesn't work (on Win32)
        //                 idle_update_progress_cb,
        //                 &data_p->progressData,
        //                 NULL);
        g_timeout_add_full (G_PRIORITY_DEFAULT_IDLE,                   // _LOW doesn't work (on Win32)
                            COMMON_UI_GTK_PROGRESSBAR_UPDATE_INTERVAL, // ms (?)
                            idle_update_progress_cb,
                            cb_data_p->progressData,
                            NULL);
    if (cb_data_p->progressData->eventSourceID > 0)
    {
      thread_data_p->eventSourceID = cb_data_p->progressData->eventSourceID;
      cb_data_p->progressData->pendingActions[cb_data_p->progressData->eventSourceID] =
          ACE_Thread_ID (thread_id, thread_handle);
      cb_data_p->eventSourceIds.insert (cb_data_p->progressData->eventSourceID);
    } // end IF
    else
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to g_timeout_add_full(idle_update_progress_cb): \"%m\", continuing\n")));
    thread_data_p = NULL;
  } // end lock scope
  stop_progress_reporting = true;

  return;

error:
  // update widgets
  gtk_action_set_stock_id (GTK_ACTION (toggleAction_in),
                           GTK_STOCK_CONNECT);
  gtk_action_set_sensitive (GTK_ACTION (toggleAction_in), true);

  frame_p =
      GTK_FRAME (gtk_builder_get_object ((*iterator).second.second,
                                         ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_FRAME_CONFIGURATION)));
  ACE_ASSERT (frame_p);
  gtk_widget_set_sensitive (GTK_WIDGET (frame_p), true);
  frame_p =
      GTK_FRAME (gtk_builder_get_object ((*iterator).second.second,
                                         ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_FRAME_OPTIONS)));
  ACE_ASSERT (frame_p);
  gtk_widget_set_sensitive (GTK_WIDGET (frame_p), true);

  progress_bar_p =
      GTK_PROGRESS_BAR (gtk_builder_get_object ((*iterator).second.second,
                                                ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_PROGRESS_BAR)));
  ACE_ASSERT (progress_bar_p);
  gtk_widget_set_sensitive (GTK_WIDGET (progress_bar_p), false);

  if (stop_progress_reporting)
  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, cb_data_p->lock);

    cb_data_p->progressData->completedActions.insert (cb_data_p->progressData->eventSourceID);
  } // end IF

  if (thread_data_p)
    delete thread_data_p;

  un_toggling_connect = true;
  gtk_action_activate (GTK_ACTION (toggleAction_in));
}

G_MODULE_EXPORT void
action_trim_activate_cb (GtkAction* action_in,
                         gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::action_trim_activate_cb"));

  struct ARDrone_GtkCBData* cb_data_p =
      static_cast<struct ARDrone_GtkCBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (cb_data_p);
}

G_MODULE_EXPORT void
action_calibrate_activate_cb (GtkAction* action_in,
                              gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::action_calibrate_activate_cb"));

  struct ARDrone_GtkCBData* cb_data_p =
      static_cast<struct ARDrone_GtkCBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (cb_data_p);
}

G_MODULE_EXPORT void
action_cut_activate_cb (GtkAction* action_in,
                        gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::action_cut_activate_cb"));

  struct ARDrone_GtkCBData* cb_data_p =
      static_cast<struct ARDrone_GtkCBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (cb_data_p);
}

//G_MODULE_EXPORT void
//combobox_destination_changed_cb (GtkComboBox* comboBox_in,
//                                 gpointer userData_in)
//{
//  ARDRONE_TRACE (ACE_TEXT ("::combobox_destination_changed_cb"));
//
//  struct ARDrone_GtkCBData* cb_data_p =
//      static_cast<struct ARDrone_GtkCBData*> (userData_in);
//
//  // sanity check(s)
//  ACE_ASSERT (cb_data_p);
//}

G_MODULE_EXPORT void
entry_address_delete_text_cb (GtkEditable* editable_in,
                              gint startPosition_in,
                              gint endPosition_in,
                              gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::entry_address_delete_text_cb"));

  ACE_UNUSED_ARG (userData_in);

  gchar* string_p = NULL;
  std::string entry_string;
  std::string address_string;
  guint num_handlers = 0;

  string_p = gtk_editable_get_chars (editable_in, 0, -1);
  if (!string_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_editable_get_chars(), returning\n")));
    goto refuse;
  } // end IF
  entry_string = string_p;
  g_free (string_p);

  // validate edited string as a whole
  address_string = entry_string;
  if (endPosition_in == -1)
    address_string.erase (startPosition_in, std::string::npos);
  else
    address_string.erase (startPosition_in,
                          (endPosition_in - startPosition_in));

  if (!Net_Common_Tools::matchIPAddress (address_string))
    goto refuse;

  // delete
  num_handlers =
    g_signal_handlers_block_by_func (editable_in,
                                     (gpointer)entry_address_delete_text_cb,
                                     userData_in);
  ACE_ASSERT (num_handlers == 1);
  gtk_editable_delete_text (editable_in,
                            startPosition_in,
                            endPosition_in);
  num_handlers =
    g_signal_handlers_unblock_by_func (editable_in,
                                       (gpointer)entry_address_delete_text_cb,
                                       userData_in);
  ACE_ASSERT (num_handlers == 1);

refuse:
  g_signal_stop_emission_by_name (editable_in,
                                  ACE_TEXT_ALWAYS_CHAR ("delete-text"));
}
G_MODULE_EXPORT void
entry_address_insert_text_cb (GtkEditable* editable_in,
                              gchar* newText_in,
                              gint newTextLength_in,
                              gpointer position_inout,
                              gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::entry_address_insert_text_cb"));

  ACE_UNUSED_ARG (userData_in);

  gchar* string_p = NULL;
  std::string entry_string;
  std::stringstream converter;
  std::string group_string;
  unsigned int group = 0;
  std::string::size_type start_position = std::string::npos;
  std::string::size_type insert_position =
    *static_cast<gint*> (position_inout);
  bool is_digit = false;
  guint number_of_handlers = 0;

  string_p = gtk_editable_get_chars (editable_in, 0, -1);
  if (!string_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_editable_get_chars(), returning\n")));
    goto refuse;
  } // end IF
  entry_string = string_p;
  g_free (string_p);

  // sanity check(s)
  ACE_ASSERT (newTextLength_in);
  if ((newTextLength_in != 1) ||
      ((newTextLength_in == -1) &&
       (ACE_OS::strlen (newText_in) > 1)))
  {
    // validate string as a whole
    // *TODO*: support partial inserts...
    std::string address_string = entry_string;
    if (newTextLength_in == -1)
      address_string.insert (*(gint*)position_inout,
                             newText_in);
    else
      address_string.insert (*(gint*)position_inout,
                             newText_in,
                             newTextLength_in);

    if (!Net_Common_Tools::matchIPAddress (address_string))
      goto refuse;

    goto accept;
  } // end IF

  // validate single character
  is_digit = !!::isdigit (*newText_in);
  if (!is_digit && !(*newText_in == '.'))
  {
    //ACE_DEBUG ((LM_DEBUG,
    //            ACE_TEXT ("invalid input (was: '%c'), returning\n"),
    //            *newText_in));
    goto refuse;
  } // end IF

  if (is_digit)
  { // rules:
    // - group must not have leading 0s, unless group IS 0
    // - group must be <= 255
    std::string::size_type end_position = std::string::npos;

    // find group
    start_position = entry_string.rfind ('.', insert_position);
    if (start_position == std::string::npos) start_position = 0;
    //else ++start_position;
    end_position = entry_string.find ('.', insert_position);
    group_string =
      entry_string.substr (start_position,
                           ((end_position == std::string::npos) ? std::string::npos
                                                                : (end_position - start_position)));

    if (group_string.empty ())
      goto accept; // group (currently) empty
    converter << group_string;
    converter >> group;
    insert_position = *(gint*)position_inout - start_position;
    if (*newText_in == '0')
      if (!group || !insert_position)
        goto refuse; // refuse leading 0s

    group_string.insert (insert_position, 1, *newText_in);
    converter.str (ACE_TEXT_ALWAYS_CHAR (""));
    converter.clear ();
    converter << group_string;
    converter >> group;
    if (group > 255)
      goto refuse; // refuse groups > 255
  } // end IF
  else
  {
    // rules:
    // - any preceding/trailing digits must form a group
    // - total number of dots ('.') <= 3
    std::string::size_type position = insert_position;
    while (position)
    {
      if (::isdigit (entry_string[--position]))
        group_string.insert (0, 1, entry_string[position]);
      else
        break;
    } // end WHILE
    converter << group_string;
    converter >> group;
    if (group > 255)
      goto refuse; // refuse groups > 255
    group_string.clear ();
    position = insert_position;
    do
    {
      if (::isdigit (entry_string[position]))
        group_string.push_back (entry_string[position]);
      else
        break;
      ++position;
    } while (true);
    converter.str (ACE_TEXT_ALWAYS_CHAR (""));
    converter.clear ();
    converter << group_string;
    converter >> group;
    if (group > 255)
      goto refuse; // refuse groups > 255

    if (std::count (entry_string.begin (), entry_string.end (), '.') > 3)
      goto refuse; // refuse more than 3 '.'
  } // end ELSE

accept:
  // insert
  number_of_handlers =
    g_signal_handlers_block_by_func (editable_in,
                                     (gpointer)entry_address_insert_text_cb,
                                     userData_in);
  ACE_ASSERT (number_of_handlers == 1);
  gtk_editable_insert_text (editable_in,
                            newText_in,
                            newTextLength_in,
                            (gint*)position_inout);
  number_of_handlers =
    g_signal_handlers_unblock_by_func (editable_in,
                                       (gpointer)entry_address_insert_text_cb,
                                       userData_in);
  ACE_ASSERT (number_of_handlers == 1);

refuse:
  g_signal_stop_emission_by_name (editable_in,
                                  ACE_TEXT_ALWAYS_CHAR ("insert-text"));
}

#if defined (GTK3_SUPPORT)
G_MODULE_EXPORT void
places_save_mount_cb (GtkPlacesSidebar* placesSidebar_in,
                      GMountOperation* mountOperation_in,
                      gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::places_save_mount_cb"));

//  struct ARDrone_GtkCBData* cb_data_p =
//      static_cast<struct ARDrone_GtkCBData*> (userData_in);

}
#endif

G_MODULE_EXPORT void
togglebutton_save_toggled_cb (GtkToggleButton* toggleButton_in,
                              gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::togglebutton_save_toggled_cb"));

  struct ARDrone_GtkCBData* cb_data_p =
      static_cast<struct ARDrone_GtkCBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (toggleButton_in);
  ACE_ASSERT (cb_data_p);

  Common_UI_GTKBuildersIterator_t iterator =
    cb_data_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != cb_data_p->builders.end ());

  GtkFrame* frame_p =
    GTK_FRAME (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_FRAME_OPTIONS_SAVE)));
  ACE_ASSERT (frame_p);

  bool is_active = gtk_toggle_button_get_active (toggleButton_in);
  gtk_widget_set_sensitive (GTK_WIDGET (frame_p),
                            is_active);
}

//------------------------------------------------------------------------------

G_MODULE_EXPORT gboolean
drawingarea_configure_cb (GtkWidget* widget_in,
                          GdkEvent* event_in,
                          gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::drawingarea_configure_cb"));

  // sanity check(s)
  ACE_ASSERT (widget_in);
  ACE_ASSERT (event_in);
  ACE_ASSERT (userData_in);

  struct ARDrone_GtkCBData* cb_data_p =
      reinterpret_cast<struct ARDrone_GtkCBData*> (userData_in);

  // sanity check(s)
#if defined (GTK3_SUPPORT)
  ACE_ASSERT (gdk_event_get_event_type (event_in) == GDK_CONFIGURE);
#else
  ACE_ASSERT (event_in->type == GDK_CONFIGURE);
#endif
  ACE_ASSERT (cb_data_p);
  ACE_ASSERT (cb_data_p->configuration);
  if (cb_data_p->configuration->moduleHandlerConfiguration.fullScreen)
    return TRUE; // nothing to do

  // *NOTE*: x,y members are relative to the parent window
  //         --> no need to translate
  //  gdk_window_get_origin (gtk_widget_get_window (widget_in),
  //                         &cb_data_p->configuration->moduleHandlerConfiguration.area.x,
  //                         &cb_data_p->configuration->moduleHandlerConfiguration.area.y);
  //  gtk_widget_translate_coordinates (widget_in,
  //                                    gtk_widget_get_toplevel (widget_in),
  //                                    0, 0,
  //                                    &cb_data_p->configuration->moduleHandlerConfiguration.area.x,
  //                                    &cb_data_p->configuration->moduleHandlerConfiguration.area.y);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  //cb_data_p->configuration->moduleHandlerConfiguration.area.left =
  //  event_in->configure.x;
  //cb_data_p->configuration->moduleHandlerConfiguration.area.right =
  //  event_in->configure.x + event_in->configure.width;
  //cb_data_p->configuration->moduleHandlerConfiguration.area.top =
  //  event_in->configure.y;
  //cb_data_p->configuration->moduleHandlerConfiguration.area.bottom =
  //  event_in->configure.y + event_in->configure.height;
#else
  cb_data_p->configuration->moduleHandlerConfiguration.area.x =
    event_in->configure.x;
  cb_data_p->configuration->moduleHandlerConfiguration.area.y =
    event_in->configure.y;
  cb_data_p->configuration->moduleHandlerConfiguration.area.height =
    event_in->configure.height;
  cb_data_p->configuration->moduleHandlerConfiguration.area.width =
    event_in->configure.width;
#endif

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#if GTK_CHECK_VERSION (3,0,0)
#else
  GdkPixbuf* pixbuf_p =
      gdk_pixbuf_new (GDK_COLORSPACE_RGB,
                      TRUE,
                      8,
                      cb_data_p->configuration->moduleHandlerConfiguration.area.width, cb_data_p->configuration->moduleHandlerConfiguration.area.height);
  if (!pixbuf_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gdk_pixbuf_new(), aborting\n")));
    return FALSE;
  } // end IF
#endif
  GdkWindow* window_p = gtk_widget_get_window (widget_in);
  if (!window_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_widget_get_window(0x%@), aborting\n"),
                widget_in));

#if GTK_CHECK_VERSION (3,0,0)
#else
    gdk_pixbuf_unref (pixbuf_p);
#endif

    return FALSE;
  } // end IF

  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, cb_data_p->lock, FALSE);
    if (cb_data_p->pixelBuffer)
    {
      g_object_unref (cb_data_p->pixelBuffer);
      cb_data_p->pixelBuffer = NULL;
    } // end IF
    cb_data_p->pixelBuffer =
#if GTK_CHECK_VERSION (3,0,0)
        gdk_pixbuf_get_from_window (window_p,
                                    0, 0,
                                    event_in->configure.width, event_in->configure.height);
#else
        gdk_pixbuf_get_from_drawable (pixbuf_p,
                                      GDK_DRAWABLE (window_p),
                                      NULL,
                                      0, 0,
                                      0, 0, event_in->configure.width, event_in->configure.height);
#endif
    if (!cb_data_p->pixelBuffer)
    { // *NOTE*: most probable reason: window is not (yet) mapped
#if GTK_CHECK_VERSION (3,0,0)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to gdk_pixbuf_get_from_window(%@), aborting\n"),
                  window_p));
#else
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to gdk_pixbuf_get_from_drawable(%@), aborting\n"),
                  GDK_DRAWABLE (window_p)));

    gdk_pixbuf_unref (pixbuf_p);
#endif
      return FALSE;
    } // end IF
    cb_data_p->configuration->moduleHandlerConfiguration.pixelBuffer =
        cb_data_p->pixelBuffer;

//    GHashTable* hash_table_p = gdk_pixbuf_get_options (cb_data_p->pixelBuffer);
//    GHashTableIter iterator;
//    g_hash_table_iter_init (&iterator, hash_table_p);
//    gpointer key, value;
//    for (unsigned int i = 0;
//         g_hash_table_iter_next (iterator, &key, &value);
//         ++i)
//      ACE_DEBUG ((LM_DEBUG,
//                  ACE_TEXT ("%u: \"\" --> \"\"\n"),
//                  i,
//                  static_cast<gchar*> (key),
//                  static_cast<gchar*> (value)));
  } // end lock scope
#endif

  return TRUE;
}
G_MODULE_EXPORT gboolean
drawingarea_draw_cb (GtkWidget* widget_in,
                     cairo_t* context_in,
                     gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::drawingarea_draw_cb"));

  struct ARDrone_GtkCBData* cb_data_p =
      reinterpret_cast<struct ARDrone_GtkCBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (cb_data_p);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  if (!cb_data_p->pixelBuffer)
    return FALSE; // --> widget has not been realized yet

  //GdkWindow* window_p = gtk_widget_get_window (widget_in);
  //ACE_ASSERT (window_p);
  //GtkAllocation allocation;
  //gtk_widget_get_allocation (widget_in,
  //                           &allocation);
  gdk_cairo_set_source_pixbuf (context_in,
                               cb_data_p->pixelBuffer,
                               0.0, 0.0);

  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, cb_data_p->lock, FALSE);

    // *IMPORTANT NOTE*: potentially, this involves tranfer of image data to an
    //                   X server running on a different host
    //gdk_draw_pixbuf (GDK_DRAWABLE (window_p), NULL,
    //                 data_p->pixelBuffer,
    //                 0, 0, 0, 0, allocation.width, allocation.height,
    //                 GDK_RGB_DITHER_NONE, 0, 0);
    cairo_paint (context_in);
  } // end lock scope
#endif

  return TRUE;
}
G_MODULE_EXPORT void
drawingarea_realize_cb (GtkWidget* widget_in,
                        gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::drawingarea_realize_cb"));

  // sanity check(s)
  ACE_ASSERT (widget_in);
  ACE_ASSERT (userData_in);

  struct ARDrone_GtkCBData* cb_data_p =
      reinterpret_cast<struct ARDrone_GtkCBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (cb_data_p);

  GdkWindow* window_p = gtk_widget_get_window (widget_in);
  ACE_ASSERT (window_p);
  if (!gdk_window_ensure_native (window_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gdk_window_ensure_native(), returning\n")));
    return;
  } // end IF
}

G_MODULE_EXPORT gboolean
key_cb (GtkWidget* widget_in,
        GdkEventKey* event_in,
        gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::key_cb"));

  ACE_UNUSED_ARG (widget_in);

  struct ARDrone_GtkCBData* cb_data_p =
      reinterpret_cast<struct ARDrone_GtkCBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (event_in);
  ACE_ASSERT (cb_data_p);

  Common_UI_GTKBuildersIterator_t iterator =
    cb_data_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != cb_data_p->builders.end ());

  switch (event_in->keyval)
  {
    case GDK_KEY_Escape:
    case GDK_KEY_f:
    case GDK_KEY_F:
    {
      GtkCheckButton* check_button_p =
        GTK_CHECK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                  ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_CHECKBUTTON_FULLSCREEN)));
      ACE_ASSERT (check_button_p);
      gboolean is_active_b =
        gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (check_button_p));

      if (event_in->keyval == GDK_KEY_Escape)
      {
        if (is_active_b)
          goto continue_;
        break; // <-- not in fullscreen mode, nothing to do
      } // end IF

      gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_button_p),
                                    !is_active_b);

continue_:
      // sanity check(s)
      ACE_ASSERT (cb_data_p->liveVideoStream);
      if (!cb_data_p->liveVideoStream->isRunning ())
        break;

      const Stream_Module_t* module_p =
        cb_data_p->liveVideoStream->find (ACE_TEXT_ALWAYS_CHAR ("Display"));
      if (!module_p)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to Stream_IStream::find(\"Display\"), aborting\n"),
                    ACE_TEXT (cb_data_p->liveVideoStream->name ().c_str ())));
        return FALSE;
      } // end IF
      Stream_Module_Visualization_IFullscreen* ifullscreen_p =
        dynamic_cast<Stream_Module_Visualization_IFullscreen*> (const_cast<Stream_Module_t*> (module_p)->writer ());
      if (!ifullscreen_p)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("Display: failed to dynamic_cast<Stream_Module_Visualization_IFullscreen*>(0x%@), aborting\n"),
                    const_cast<Stream_Module_t*> (module_p)->writer ()));
        return FALSE;
      } // end IF
      try {
        ifullscreen_p->toggle ();
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("Display: failed to Stream_Module_Visualization_IFullscreen::toggle(), aborting\n")));
        return FALSE;
      }

      break;
    }
    default:
      return FALSE; // propagate
  } // end SWITCH

  return TRUE; // done (do not propagate further)
}
gboolean
drawingarea_key_press_event_cb (GtkWidget* widget_in,
                                GdkEventKey* event_in,
                                gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::drawingarea_key_press_event_cb"));

  return key_cb (widget_in, event_in, userData_in);
};
gboolean
dialog_main_key_press_event_cb (GtkWidget* widget_in,
                                GdkEventKey* event_in,
                                gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::dialog_main_key_press_event_cb"));

  return key_cb (widget_in, event_in, userData_in);
};

//G_MODULE_EXPORT gboolean
//motion_cb (GtkWidget* widget_in,
//           GdkEventMotion* event_in,
//           gpointer userData_in)
//{
//  ARDRONE_TRACE (ACE_TEXT ("::motion_cb"));
//
//  struct ARDrone_GtkCBData* cb_data_p =
//    reinterpret_cast<struct ARDrone_GtkCBData*> (userData_in);
//
//  // sanity check(s)
//  ACE_ASSERT (event_in);
//  ACE_ASSERT (cb_data_p);
//  if (event_in->is_hint) return FALSE; // propagate
//
//  int diff_x =
//   static_cast<int> (event_in->x) - cb_data_p->openGLCamera.last[0];
//  int diff_y =
//   static_cast<int> (event_in->y) - cb_data_p->openGLCamera.last[1];
//  cb_data_p->openGLCamera.last[0] = static_cast<int> (event_in->x);
//  cb_data_p->openGLCamera.last[1] = static_cast<int> (event_in->y);
//
//  bool is_dirty = false;
//  if (event_in->state & GDK_BUTTON1_MASK)
//  {
//    cb_data_p->openGLCamera.rotation[0] +=
//      ARDRONE_OPENGL_CAMERA_ROTATION_FACTOR * diff_y;
//    cb_data_p->openGLCamera.rotation[1] +=
//      ARDRONE_OPENGL_CAMERA_ROTATION_FACTOR * diff_x;
//    is_dirty = true;
//  } // end IF
//  if (event_in->state & GDK_BUTTON2_MASK)
//  {
//    cb_data_p->openGLCamera.translation[0] +=
//      ARDRONE_OPENGL_CAMERA_TRANSLATION_FACTOR * diff_x;
//    cb_data_p->openGLCamera.translation[1] -=
//      ARDRONE_OPENGL_CAMERA_TRANSLATION_FACTOR * diff_y;
//    is_dirty = true;
//  } // end IF
//  if (event_in->state & GDK_BUTTON3_MASK)
//  {
//    cb_data_p->openGLCamera.zoom -=
//      ARDRONE_OPENGL_CAMERA_ZOOM_FACTOR * diff_x;
//    is_dirty = true;
//  } // end IF
//
//  // invalidate drawing area
//  // *NOTE*: the drawing area is not refreshed automatically unless the window
//  //         is resized
//  if (is_dirty)
//    gtk_widget_queue_draw (widget_in);
//
//  return TRUE; // done (do not propagate further)
//}

//------------------------------------------------------------------------------

G_MODULE_EXPORT gint
button_clear_clicked_cb (GtkWidget* widget_in,
                         gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::button_clear_clicked_cb"));

  ACE_UNUSED_ARG (widget_in);
  struct ARDrone_GtkCBData* cb_data_p =
    static_cast<struct ARDrone_GtkCBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (cb_data_p);

  Common_UI_GTKBuildersIterator_t iterator =
    cb_data_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != cb_data_p->builders.end ());

//  GtkTextView* view_p =
//    GTK_TEXT_VIEW (gtk_builder_get_object ((*iterator).second.second,
//                                           ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_TEXTVIEW_LOG)));
//  ACE_ASSERT (view_p);
//  GtkTextBuffer* buffer_p =
////    gtk_text_buffer_new (NULL); // text tag table --> create new
//    gtk_text_view_get_buffer (view_p);
//  ACE_ASSERT (buffer_p);
//  gtk_text_buffer_set_text (buffer_p,
//                            ACE_TEXT_ALWAYS_CHAR (""), 0);

  return TRUE; // done (do not propagate further)
}

G_MODULE_EXPORT gint
button_about_clicked_cb (GtkWidget* widget_in,
                         gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::button_about_clicked_cb"));

  ACE_UNUSED_ARG (widget_in);
  struct ARDrone_GtkCBData* cb_data_p =
      static_cast<struct ARDrone_GtkCBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (cb_data_p);

  Common_UI_GTKBuildersIterator_t iterator =
    cb_data_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != cb_data_p->builders.end ());

  // retrieve about dialog handle
  GtkWidget* about_dialog =
      GTK_WIDGET (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_DIALOG_ABOUT)));
  ACE_ASSERT (about_dialog);
  if (!about_dialog)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_builder_get_object(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (ARDRONE_UI_WIDGET_NAME_DIALOG_ABOUT)));
    return FALSE; // propagate
  } // end IF

  // draw it
#if defined (GTK3_SUPPORT)
  if (!gtk_widget_is_visible (about_dialog))
#else
  if (!gtk_widget_get_visible (about_dialog))
#endif
    gtk_widget_show_all (about_dialog);

  return TRUE; // done (do not propagate further)
}

G_MODULE_EXPORT gint
button_quit_clicked_cb (GtkWidget* widget_in,
                        gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::button_quit_clicked_cb"));

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
