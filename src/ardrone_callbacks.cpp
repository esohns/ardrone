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
#else
#include <ifaddrs.h>
#endif

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <gl/GL.h>
#include <gl/GLU.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
//#include <GL/glut.h>
#include "glm/glm.hpp"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <cguid.h>
#include <PhysicalMonitorEnumerationAPI.h>
#include <mfapi.h>
#include <mfidl.h>
#include <wlanapi.h>
#endif

#ifdef __cplusplus
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavutil/pixfmt.h"
}
#endif /* __cplusplus */

#include "ace/Log_Msg.h"
#include "ace/Process.h"
#include "ace/Process_Manager.h"
#include "ace/Synch.h"

#include "gmodule.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "gdk/gdkwin32.h"
#else
#include "gdk/gdk.h"
#endif
#include "gdk/gdkkeysyms.h"
#include "gtk/gtk.h"
//#include "gtk/gtkgl.h"

#include "common_file_tools.h"

#include "common_gl_defines.h"
#include "common_gl_tools.h"

#include "common_image_tools.h"

#include "common_timer_manager.h"

#include "common_ui_common.h"
#include "common_ui_defines.h"
#include "common_ui_tools.h"

#include "stream_dec_tools.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "stream_dev_directshow_tools.h"
#include "stream_dev_tools.h"
#endif // ACE_WIN32 || ACE_WIN64

#include "stream_vis_common.h"

#include "net_common_tools.h"

#if defined (HAVE_CONFIG_H)
#include "ardrone_config.h"
#endif // HAVE_CONFIG_H

#include "ardrone_common.h"
#include "ardrone_configuration.h"
#include "ardrone_defines.h"
#include "ardrone_macros.h"
#include "ardrone_message.h"
#include "ardrone_stream.h"
#include "ardrone_opengl.h"

// global variables
bool un_toggling_connect = false;

bool
load_wlan_interfaces (GtkListStore* listStore_inout)
{
  ARDRONE_TRACE (ACE_TEXT ("::load_wlan_interfaces"));

  // initialize result
  gtk_list_store_clear (listStore_inout);

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#elif defined (ACE_LINUX)
#if defined (NL80211_USE)
  ARDrone_WLANMonitor_t* WLAN_monitor_p =
    ARDRONE_WLANMONITOR_SINGLETON::instance ();
  ACE_ASSERT (WLAN_monitor_p);
#endif // NL80211_USE
#endif // ACE_WIN32 || ACE_WIN64

  Net_InterfaceIdentifiers_t interface_identifiers_a =
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#if defined (WLANAPI_USE)
      Net_WLAN_Tools::getInterfaces (ACE_INVALID_HANDLE);
#endif // WLANAPI_USE
#elif defined (ACE_LINUX)
      Net_WLAN_Tools::getInterfaces (
#if defined (NL80211_USE)
                                     static_cast<struct nl_sock*> (NULL),
//                                     const_cast<struct nl_sock*> (WLAN_monitor_p->getP ()),
                                     WLAN_monitor_p->get_3 ());
#elif defined (WEXT_USE) || defined (DBUS_USE)
                                     AF_UNSPEC,
                                     0);
#endif // NL80211_USE
#endif // ACE_WIN32 || ACE_WIN64

  GtkTreeIter iterator;
  for (Net_InterfacesIdentifiersIterator_t iterator_2 = interface_identifiers_a.begin ();
       iterator_2 != interface_identifiers_a.end ();
       ++iterator_2)
  {
    gtk_list_store_append (listStore_inout, &iterator);
    gtk_list_store_set (listStore_inout, &iterator,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                        0, ACE_TEXT_ALWAYS_CHAR (Net_Common_Tools::interfaceToString (*iterator_2).c_str ()),
                        1, ACE_TEXT_ALWAYS_CHAR (Common_Tools::GUIDToString (*iterator_2).c_str ()),
#else
                        0, ACE_TEXT_ALWAYS_CHAR ((*iterator_2).c_str ()),
#endif // ACE_WIN32 || ACE_WIN64
                        -1);
  } // end FOR

  return true;
}

bool
load_display_devices (GtkListStore* listStore_inout)
{
  ARDRONE_TRACE (ACE_TEXT ("::load_display_devices"));

  // initialize result
  gtk_list_store_clear (listStore_inout);

  Common_UI_DisplayDevices_t devices_a;
  if (!Common_UI_Tools::getDisplayDevices (devices_a))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_UI_Tools::getDisplayDevices(), aborting\n")));
    return false;
  } // end IF

  GtkTreeIter iterator;
  for (Common_UI_DisplayDevicesIterator_t iterator_2 = devices_a.begin ();
       iterator_2 != devices_a.end ();
       ++iterator_2)
  {
    gtk_list_store_append (listStore_inout, &iterator);
    gtk_list_store_set (listStore_inout, &iterator,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                        0, (*iterator_2).description.c_str (),
                        1, (*iterator_2).device.c_str (),
#else
                        0, (*iterator_2).device.c_str (),
#endif // ACE_WIN32 || ACE_WIN64
                        -1);
  } // end FOR

  return true;
}

bool
load_display_formats (GtkListStore* listStore_inout)
{
  ARDRONE_TRACE (ACE_TEXT ("::load_display_formats"));

  // initialize result
  gtk_list_store_clear (listStore_inout);

  GtkTreeIter iterator;
  do
  { // *TODO*: this needs more work; support the device capabilities exposed
    //         through the API instead of static values
    gtk_list_store_append (listStore_inout, &iterator);
    gtk_list_store_set (listStore_inout, &iterator,
                        0, ACE_TEXT (ARDroneVideoModeToString (ARDRONE_VIDEOMODE_360P).c_str ()),
                        1, ARDRONE_VIDEOMODE_360P,
                        -1);

    gtk_list_store_append (listStore_inout, &iterator);
    gtk_list_store_set (listStore_inout, &iterator,
                        0, ACE_TEXT (ARDroneVideoModeToString (ARDRONE_VIDEOMODE_720P).c_str ()),
                        1, ARDRONE_VIDEOMODE_720P,
                        -1);

    break;
  } while (true);

  return true;
}

bool
load_save_formats (GtkListStore* listStore_inout)
{
  ARDRONE_TRACE (ACE_TEXT ("::load_save_formats"));

  // initialize result
  gtk_list_store_clear (listStore_inout);

  GtkTreeIter iterator;
  do
  { // *TODO*: this needs more work; support the device capabilities exposed
    //         through the API instead of static values
    // *TODO*: define/activate a 'save-to-file' subpipeline (use a multiplexer)
    //         and forward(/encapsulate) the byte-stream as default format
    gtk_list_store_append (listStore_inout, &iterator);
    gtk_list_store_set (listStore_inout, &iterator,
                        0, ACE_TEXT ("RGB AVI"),
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                        1, ACE_TEXT (Common_Tools::GUIDToString (MEDIASUBTYPE_RGB24).c_str ()),
                        2, 0,
#else
                        1, ACE_TEXT (""),
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

  Common_UI_GTK_BuildersIterator_t iterator;
  //ACE_SYNCH_MUTEX* lock_p = NULL;
  struct ARDrone_ThreadData* data_p =
      static_cast<struct ARDrone_ThreadData*> (arg_in);

  // sanity check(s)
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->CBData);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct ARDrone_DirectShow_GtkCBData* directshow_cb_data_p =
    NULL;
  struct ARDrone_DirectShow_Configuration* directshow_configuration_p =
    NULL;
  struct ARDrone_MediaFoundation_GtkCBData* mediafoundation_cb_data_p =
    NULL;
  struct ARDrone_MediaFoundation_Configuration* mediafoundation_configuration_p =
    NULL;
  switch (data_p->mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      struct ARDrone_DirectShow_GtkCBData* directshow_cb_data_p =
        static_cast<struct ARDrone_DirectShow_GtkCBData*> (data_p->CBData);
      directshow_configuration_p = directshow_cb_data_p->configuration;
      ACE_ASSERT (directshow_configuration_p);
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      struct ARDrone_MediaFoundation_GtkCBData* mediafoundation_cb_data_p =
        static_cast<struct ARDrone_MediaFoundation_GtkCBData*> (data_p->CBData);
      mediafoundation_configuration_p =
        mediafoundation_cb_data_p->configuration;
      ACE_ASSERT (mediafoundation_configuration_p);
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), aborting\n"),
                  data_p->mediaFramework));
      break;
    }
  } // end SWITCH
#else
  struct ARDrone_GtkCBData* cb_data_p =
    static_cast<struct ARDrone_GtkCBData*> (data_p->CBData);
  struct ARDrone_Configuration* configuration_p = cb_data_p->configuration;
  ACE_ASSERT (configuration_p);
#endif // ACE_WIN32 || ACE_WIN64

  iterator =
    data_p->CBData->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  //lock_p = &data_p->CBData->lock;

  // sanity check(s)
  ACE_ASSERT (iterator != data_p->CBData->builders.end ());

  GtkStatusbar* statusbar_p = NULL;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ARDrone_DirectShow_StreamConfiguration_t::ITERATOR_T directshow_iterator_2;
  ARDrone_MediaFoundation_StreamConfiguration_t::ITERATOR_T mediafoundation_iterator_2;
#else
  ARDrone_StreamConfiguration_t::ITERATOR_T iterator_2;
#endif
  //  struct ARDrone_ModuleHandlerConfiguration* configuration_p = NULL;
  Stream_IStreamControlBase* istream_base_p = NULL;
  std::ostringstream converter;
  Common_IGetR_T<ARDrone_SessionData_t>* iget_p = NULL;
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
//    ARDrone_ConnectionConfigurationIterator_t iterator_3;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ARDrone_DirectShow_StreamConfigurationsIterator_t directshow_iterator_4;
  ARDrone_MediaFoundation_StreamConfigurationsIterator_t mediafoundation_iterator_4;
  typename ARDrone_DirectShow_VideoStream_t::IINITIALIZE_T* iinitialize_p = NULL;
#else
  ARDrone_StreamConfigurationsIterator_t iterator_4;
  typename ARDrone_VideoStream_t::IINITIALIZE_T* iinitialize_p = NULL;
#endif // ACE_WIN32 || ACE_WIN64
  //    Stream_ISession* session_p = NULL;
  ACE_Time_Value session_start_timeout =
      COMMON_TIME_NOW + ACE_Time_Value (3, 0);
  Common_IGetP_T<ARDrone_IController>* iget_2 = NULL;
  ARDrone_StreamsIterator_t streams_iterator =
      data_p->CBData->streams.find (navdata_stream_name_string_);
  ACE_ASSERT (streams_iterator != data_p->CBData->streams.end ());

    // navdata
//    iinitialize_p = data_p->CBData->NavDataStream;
//    ACE_ASSERT (iinitialize_p);
    iget_2 =
      dynamic_cast<Common_IGetP_T<ARDrone_IController>*> ((*streams_iterator).second);
    ACE_ASSERT (iget_2);
    data_p->CBData->controller =
        const_cast<ARDrone_IController*> (iget_2->getP ());
    ACE_ASSERT (data_p->CBData->controller);
//    istream_base_p =
//        dynamic_cast<Stream_IStreamControlBase*> ((*streams_iterator).second);
//    ACE_ASSERT (istream_base_p);

    // control
    streams_iterator =
          data_p->CBData->streams.find (control_stream_name_string_);
    ACE_ASSERT (streams_iterator != data_p->CBData->streams.end ());
    iinitialize_p =
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      dynamic_cast<typename ARDrone_DirectShow_ControlStream_t::IINITIALIZE_T*> ((*streams_iterator).second);
#else
      dynamic_cast<typename ARDrone_ControlStream_t::IINITIALIZE_T*> ((*streams_iterator).second);
#endif // ACE_WIN32 || ACE_WIN64
    ACE_ASSERT (iinitialize_p);
    istream_base_p =
        dynamic_cast<Stream_IStreamControlBase*> ((*streams_iterator).second);
    ACE_ASSERT (istream_base_p);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    switch (data_p->mediaFramework)
    {
      case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
      {
        directshow_iterator_4 =
          directshow_configuration_p->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_CONTROL_STREAM_NAME_STRING));
        ACE_ASSERT (directshow_iterator_4 != directshow_configuration_p->streamConfigurations.end ());
        directshow_iterator_2 = (*directshow_iterator_4).second.find (ACE_TEXT_ALWAYS_CHAR (""));
        ACE_ASSERT (directshow_iterator_2 != (*directshow_iterator_4).second.end ());
        logfile_name_string = (*directshow_iterator_2).second.second.targetFileName;
        (*directshow_iterator_2).second.second.targetFileName =
          Common_File_Tools::getLogFilename (ACE_TEXT_ALWAYS_CHAR (ARDRONE_PACKAGE_NAME),
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_CONTROL_LOG_FILE_PREFIX));

        result_2 = iinitialize_p->initialize ((*directshow_iterator_4).second);
        if (!result_2)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to initialize control stream: \"%m\", aborting\n")));
          goto done;
        } // end IF
        (*directshow_iterator_2).second.second.targetFileName =
            logfile_name_string;
        //session_p = dynamic_cast<Stream_ISession*> (data_p->CBData->controlStream);
        //ACE_ASSERT (session_p);
        istream_base_p->start ();
        // *IMPORTANT NOTE*: race condition here --> add timeout
        //    session_p->wait (false,
        //                     &session_start_timeout);

        // mavlink
        directshow_iterator_4 =
          directshow_configuration_p->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_MAVLINK_STREAM_NAME_STRING));
        ACE_ASSERT (directshow_iterator_4 != directshow_configuration_p->streamConfigurations.end ());
        directshow_iterator_2 = (*directshow_iterator_4).second.find (ACE_TEXT_ALWAYS_CHAR (""));
        ACE_ASSERT (directshow_iterator_2 != (*directshow_iterator_4).second.end ());
        logfile_name_string =
            (*directshow_iterator_2).second.second.targetFileName;
        (*directshow_iterator_2).second.second.targetFileName =
          Common_File_Tools::getLogFilename (ACE_TEXT_ALWAYS_CHAR (ARDRONE_PACKAGE_NAME),
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_MAVLINK_LOG_FILE_PREFIX));

        streams_iterator =
              data_p->CBData->streams.find (mavlink_stream_name_string_);
        ACE_ASSERT (streams_iterator != data_p->CBData->streams.end ());
        iinitialize_p =
#if defined (ACE_WIN32) || defined (ACE_WIN64)
          dynamic_cast<typename ARDrone_DirectShow_MAVLinkStream_t::IINITIALIZE_T*> ((*streams_iterator).second);
#else
          dynamic_cast<typename ARDrone_MAVLinkStream_t::IINITIALIZE_T*> ((*streams_iterator).second);
#endif // ACE_WIN32 || ACE_WIN64
        ACE_ASSERT (iinitialize_p);
        istream_base_p =
            dynamic_cast<Stream_IStreamControlBase*> ((*streams_iterator).second);
        ACE_ASSERT (istream_base_p);

        result_2 = iinitialize_p->initialize ((*directshow_iterator_4).second);
        if (!result_2)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to initialize MAVLink stream: \"%m\", aborting\n")));
          goto done;
        } // end IF
        (*directshow_iterator_2).second.second.targetFileName =
            logfile_name_string;
        //    session_p = dynamic_cast<Stream_ISession*> (data_p->CBData->MAVLinkStream);
        //    ACE_ASSERT (session_p);
        istream_base_p->start ();
        //    (*iterator_2).second.targetFileName = logfile_name_string;

        // navdata
        directshow_iterator_4 =
          directshow_configuration_p->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_NAVDATA_STREAM_NAME_STRING));
        ACE_ASSERT (directshow_iterator_4 != directshow_configuration_p->streamConfigurations.end ());
        directshow_iterator_2 = (*directshow_iterator_4).second.find (ACE_TEXT_ALWAYS_CHAR (""));
        ACE_ASSERT (directshow_iterator_2 != (*directshow_iterator_4).second.end ());
        logfile_name_string =
            (*directshow_iterator_2).second.second.targetFileName;
        //    configuration_p =
        //      const_cast<struct ARDrone_ModuleHandlerConfiguration*> (static_cast<const struct ARDrone_ModuleHandlerConfiguration*> (&((*iterator_2).second)));
        //    ACE_ASSERT (configuration_p);
        //    configuration_p->targetFileName =
        (*directshow_iterator_2).second.second.targetFileName =
          Common_File_Tools::getLogFilename (ACE_TEXT_ALWAYS_CHAR (ARDRONE_PACKAGE_NAME),
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_NAVDATA_LOG_FILE_PREFIX));

        streams_iterator =
              data_p->CBData->streams.find (navdata_stream_name_string_);
        ACE_ASSERT (streams_iterator != data_p->CBData->streams.end ());
        iinitialize_p =
#if defined (ACE_WIN32) || defined (ACE_WIN64)
          dynamic_cast<typename ARDrone_DirectShow_NavDataStream_t::IINITIALIZE_T*> ((*streams_iterator).second);
#else
          dynamic_cast<typename ARDrone_NavDataStream_t::IINITIALIZE_T*> ((*streams_iterator).second);
#endif // ACE_WIN32 || ACE_WIN64
        ACE_ASSERT (iinitialize_p);
        istream_base_p =
            dynamic_cast<Stream_IStreamControlBase*> ((*streams_iterator).second);
        ACE_ASSERT (istream_base_p);

        result_2 = iinitialize_p->initialize ((*directshow_iterator_4).second);
        if (!result_2)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to initialize NavData stream: \"%m\", aborting\n")));
          goto done;
        } // end IF
        (*directshow_iterator_2).second.second.targetFileName =
            logfile_name_string;
        istream_base_p->start ();
        //    session_p = dynamic_cast<Stream_ISession*> (data_p->CBData->NavDataStream);
        //    ACE_ASSERT (session_p);
        //    // *IMPORTANT NOTE*: race condition here --> add timeout
        //    session_p->wait (false,
        //                     &session_start_timeout);

        // video
        if (!data_p->CBData->enableVideo)
          goto continue_;

        directshow_iterator_4 =
          directshow_configuration_p->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
        ACE_ASSERT (directshow_iterator_4 != directshow_configuration_p->streamConfigurations.end ());
        directshow_iterator_2 =
            (*directshow_iterator_4).second.find (ACE_TEXT_ALWAYS_CHAR (""));
        ACE_ASSERT (directshow_iterator_2 != (*directshow_iterator_4).second.end ());
        //    (*iterator_2).second.stream = data_p->CBData->videoStream;

        streams_iterator =
              data_p->CBData->streams.find (video_stream_name_string_);
        ACE_ASSERT (streams_iterator != data_p->CBData->streams.end ());
        iinitialize_p =
#if defined (ACE_WIN32) || defined (ACE_WIN64)
          dynamic_cast<typename ARDrone_DirectShow_VideoStream_t::IINITIALIZE_T*> ((*streams_iterator).second);
#else
          dynamic_cast<typename ARDrone_VideoStream_t::IINITIALIZE_T*> ((*streams_iterator).second);
#endif // ACE_WIN32 || ACE_WIN64
        ACE_ASSERT (iinitialize_p);
        istream_base_p =
            dynamic_cast<Stream_IStreamControlBase*> ((*streams_iterator).second);
        ACE_ASSERT (istream_base_p);

        result_2 = iinitialize_p->initialize ((*directshow_iterator_4).second);
        if (!result_2)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to initialize video stream: \"%m\", aborting\n")));
          goto done;
        } // end IF

        iget_p =
          dynamic_cast<Common_IGetR_T<ARDrone_SessionData_t>*> ((*streams_iterator).second);
        ACE_ASSERT (iget_p);

        session_data_container_p = &iget_p->getR ();
        ACE_ASSERT (session_data_container_p);
        session_data_p =
          &const_cast<struct ARDrone_SessionData&> (session_data_container_p->getR ());
        ACE_ASSERT (session_data_p);
        converter.clear ();
        converter.str (ACE_TEXT_ALWAYS_CHAR (""));
        converter << session_data_p->sessionId;

        // set context id
        gdk_threads_enter ();
        statusbar_p =
          GTK_STATUSBAR (gtk_builder_get_object ((*iterator).second.second,
                                                 ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_STATUSBAR)));
        ACE_ASSERT (statusbar_p);
        data_p->CBData->contextIds[COMMON_UI_GTK_STATUSCONTEXT_DATA] =
          gtk_statusbar_get_context_id (statusbar_p,
                                        converter.str ().c_str ());
        gdk_threads_leave ();
        //  } // end lock scope
        istream_base_p->start ();
        //    if (!istream_base_p->isRunning ())
        //    {
        //      ACE_DEBUG ((LM_ERROR,
        //                  ACE_TEXT ("failed to start stream, aborting\n")));
        //      return;
        //    } // end IF

        break;
      }
      case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
      {
        mediafoundation_iterator_4 =
          mediafoundation_configuration_p->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_CONTROL_STREAM_NAME_STRING));
        ACE_ASSERT (mediafoundation_iterator_4 != mediafoundation_configuration_p->streamConfigurations.end ());
        mediafoundation_iterator_2 = (*mediafoundation_iterator_4).second.find (ACE_TEXT_ALWAYS_CHAR (""));
        ACE_ASSERT (mediafoundation_iterator_2 != (*mediafoundation_iterator_4).second.end ());
        logfile_name_string = (*mediafoundation_iterator_2).second.second.targetFileName;
        (*mediafoundation_iterator_2).second.second.targetFileName =
          Common_File_Tools::getLogFilename (ACE_TEXT_ALWAYS_CHAR (ARDRONE_PACKAGE_NAME),
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_CONTROL_LOG_FILE_PREFIX));
        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown media framework (was: %d), aborting\n"),
                    data_p->mediaFramework));
        break;
      }
    } // end SWITCH
#else
    iterator_4 =
      configuration_p->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_CONTROL_STREAM_NAME_STRING));
    ACE_ASSERT (iterator_4 != configuration_p->streamConfigurations.end ());
    iterator_2 = (*iterator_4).second.find (ACE_TEXT_ALWAYS_CHAR (""));
    ACE_ASSERT (iterator_2 != (*iterator_4).second.end ());
    logfile_name_string = (*iterator_2).second.second.targetFileName;
    (*iterator_2).second.second.targetFileName =
        Common_File_Tools::getLogFilename (ACE_TEXT_ALWAYS_CHAR (ARDRONE_PACKAGE_NAME),
                                           ACE_TEXT_ALWAYS_CHAR (ARDRONE_CONTROL_LOG_FILE_PREFIX));

    result_2 = iinitialize_p->initialize ((*iterator_4).second);
    if (!result_2)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to initialize control stream: \"%m\", aborting\n")));
      goto done;
    } // end IF
    (*iterator_2).second.second.targetFileName = logfile_name_string;
    //session_p = dynamic_cast<Stream_ISession*> (data_p->CBData->controlStream);
    //ACE_ASSERT (session_p);
    istream_base_p->start ();
    // *IMPORTANT NOTE*: race condition here --> add timeout
//    session_p->wait (false,
//                     &session_start_timeout);

    // mavlink
    iterator_4 =
      configuration_p->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_MAVLINK_STREAM_NAME_STRING));
    ACE_ASSERT (iterator_4 != configuration_p->streamConfigurations.end ());
    iterator_2 = (*iterator_4).second.find (ACE_TEXT_ALWAYS_CHAR (""));
    ACE_ASSERT (iterator_2 != (*iterator_4).second.end ());
    logfile_name_string = (*iterator_2).second.second.targetFileName;
    (*iterator_2).second.second.targetFileName =
        Common_File_Tools::getLogFilename (ACE_TEXT_ALWAYS_CHAR (ARDRONE_PACKAGE_NAME),
                                           ACE_TEXT_ALWAYS_CHAR (ARDRONE_MAVLINK_LOG_FILE_PREFIX));

    streams_iterator =
          data_p->CBData->streams.find (mavlink_stream_name_string_);
    ACE_ASSERT (streams_iterator != data_p->CBData->streams.end ());
    iinitialize_p =
        dynamic_cast<typename ARDrone_VideoStream_t::IINITIALIZE_T*> ((*streams_iterator).second);
    ACE_ASSERT (iinitialize_p);
    istream_base_p =
        dynamic_cast<Stream_IStreamControlBase*> ((*streams_iterator).second);
    ACE_ASSERT (istream_base_p);
    result_2 = iinitialize_p->initialize ((*iterator_4).second);
    if (!result_2)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to initialize MAVLink stream: \"%m\", aborting\n")));
      goto done;
    } // end IF
    (*iterator_2).second.second.targetFileName = logfile_name_string;
//    session_p = dynamic_cast<Stream_ISession*> (data_p->CBData->MAVLinkStream);
//    ACE_ASSERT (session_p);
    istream_base_p->start ();
//    (*iterator_2).second.targetFileName = logfile_name_string;

    // navdata
    iterator_4 =
      configuration_p->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_NAVDATA_STREAM_NAME_STRING));
    ACE_ASSERT (iterator_4 != configuration_p->streamConfigurations.end ());
    iterator_2 = (*iterator_4).second.find (ACE_TEXT_ALWAYS_CHAR (""));
    ACE_ASSERT (iterator_2 != (*iterator_4).second.end ());
    logfile_name_string = (*iterator_2).second.second.targetFileName;
    (*iterator_2).second.second.targetFileName =
        Common_File_Tools::getLogFilename (ACE_TEXT_ALWAYS_CHAR (ARDRONE_PACKAGE_NAME),
                                           ACE_TEXT_ALWAYS_CHAR (ARDRONE_NAVDATA_LOG_FILE_PREFIX));

    streams_iterator =
          data_p->CBData->streams.find (navdata_stream_name_string_);
    ACE_ASSERT (streams_iterator != data_p->CBData->streams.end ());
    iinitialize_p =
        dynamic_cast<typename ARDrone_VideoStream_t::IINITIALIZE_T*> ((*streams_iterator).second);
    ACE_ASSERT (iinitialize_p);
    istream_base_p =
        dynamic_cast<Stream_IStreamControlBase*> ((*streams_iterator).second);
    ACE_ASSERT (istream_base_p);
    result_2 = iinitialize_p->initialize ((*iterator_4).second);
    if (!result_2)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to initialize NavData stream: \"%m\", aborting\n")));
      goto done;
    } // end IF
    (*iterator_2).second.second.targetFileName = logfile_name_string;
    istream_base_p->start ();
//    session_p = dynamic_cast<Stream_ISession*> (data_p->CBData->NavDataStream);
//    ACE_ASSERT (session_p);
//    // *IMPORTANT NOTE*: race condition here --> add timeout
//    session_p->wait (false,
//                     &session_start_timeout);

    // video
    if (!data_p->CBData->enableVideo)
      goto continue_;

    iterator_4 =
      configuration_p->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
    ACE_ASSERT (iterator_4 != configuration_p->streamConfigurations.end ());
    iterator_2 = (*iterator_4).second.find (ACE_TEXT_ALWAYS_CHAR (""));
    ACE_ASSERT (iterator_2 != (*iterator_4).second.end ());

    streams_iterator =
          data_p->CBData->streams.find (video_stream_name_string_);
    ACE_ASSERT (streams_iterator != data_p->CBData->streams.end ());
    iinitialize_p =
        dynamic_cast<typename ARDrone_VideoStream_t::IINITIALIZE_T*> ((*streams_iterator).second);
    ACE_ASSERT (iinitialize_p);
    istream_base_p =
        dynamic_cast<Stream_IStreamControlBase*> ((*streams_iterator).second);
    ACE_ASSERT (istream_base_p);
    result_2 = iinitialize_p->initialize ((*iterator_4).second);
    if (!result_2)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to initialize video stream: \"%m\", aborting\n")));
      goto done;
    } // end IF

    iget_p =
        dynamic_cast<Common_IGetR_T<ARDrone_SessionData_t>*> ((*streams_iterator).second);
    ACE_ASSERT (iget_p);
    session_data_container_p = &iget_p->getR ();
    ACE_ASSERT (session_data_container_p);
    session_data_p =
      &const_cast<struct ARDrone_SessionData&> (session_data_container_p->getR ());
    ACE_ASSERT (session_data_p);
    converter.clear ();
    converter.str (ACE_TEXT_ALWAYS_CHAR (""));
    converter << session_data_p->sessionId;

    // set context id
    gdk_threads_enter ();
    statusbar_p =
      GTK_STATUSBAR (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_STATUSBAR)));
    ACE_ASSERT (statusbar_p);
    data_p->CBData->contextIds[COMMON_UI_GTK_STATUSCONTEXT_DATA] =
        gtk_statusbar_get_context_id (statusbar_p,
                                      converter.str ().c_str ());
    gdk_threads_leave ();
//  } // end lock scope
    istream_base_p->start ();
  //    if (!data_p->CBData->videoStream->isRunning ())
  //    {
  //      ACE_DEBUG ((LM_ERROR,
  //                  ACE_TEXT ("failed to start stream, aborting\n")));
  //      return;
  //    } // end IF
#endif

continue_:
    streams_iterator =
        data_p->CBData->streams.find (control_stream_name_string_);
    ACE_ASSERT (streams_iterator != data_p->CBData->streams.end ());
    istream_base_p =
        dynamic_cast<Stream_IStreamControlBase*> ((*streams_iterator).second);
    ACE_ASSERT (istream_base_p);
    istream_base_p->wait (true, false, false);
    streams_iterator =
        data_p->CBData->streams.find (mavlink_stream_name_string_);
    ACE_ASSERT (streams_iterator != data_p->CBData->streams.end ());
    istream_base_p =
        dynamic_cast<Stream_IStreamControlBase*> ((*streams_iterator).second);
    ACE_ASSERT (istream_base_p);
    istream_base_p->wait (true, false, false);
    streams_iterator =
        data_p->CBData->streams.find (navdata_stream_name_string_);
    ACE_ASSERT (streams_iterator != data_p->CBData->streams.end ());
    istream_base_p =
        dynamic_cast<Stream_IStreamControlBase*> ((*streams_iterator).second);
    ACE_ASSERT (istream_base_p);
    istream_base_p->wait (true, false, false);
  if (data_p->CBData->enableVideo)
  {
    streams_iterator =
        data_p->CBData->streams.find (video_stream_name_string_);
    ACE_ASSERT (streams_iterator != data_p->CBData->streams.end ());
    istream_base_p =
        dynamic_cast<Stream_IStreamControlBase*> ((*streams_iterator).second);
    ACE_ASSERT (istream_base_p);
    istream_base_p->wait (true, false, false);
  } // end IF

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
    data_p->CBData->progressData.completedActions.insert (data_p->eventSourceId);
  } // end lock scope

  // clean up
  delete data_p;

  return result;
}

// -----------------------------------------------------------------------------

gboolean
idle_associated_SSID_cb (gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::idle_associated_SSID_cb"));

  struct ARDrone_GtkCBData_Base* cb_data_base_p =
    static_cast<struct ARDrone_GtkCBData_Base*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (cb_data_base_p);

  std::string SSID_string;
  bool auto_associate_b = false;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct ARDrone_DirectShow_GtkCBData* directshow_cb_data_p =
    NULL;
  struct ARDrone_DirectShow_Configuration* directshow_configuration_p =
    NULL;
  struct ARDrone_MediaFoundation_GtkCBData* mediafoundation_cb_data_p =
    NULL;
  struct ARDrone_MediaFoundation_Configuration* mediafoundation_configuration_p =
    NULL;
  switch (cb_data_base_p->mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      struct ARDrone_DirectShow_GtkCBData* directshow_cb_data_p =
        static_cast<struct ARDrone_DirectShow_GtkCBData*> (cb_data_base_p);
      directshow_configuration_p = directshow_cb_data_p->configuration;
      ACE_ASSERT (directshow_configuration_p);
      SSID_string = directshow_configuration_p->WLANMonitorConfiguration.SSID;
      auto_associate_b =
        directshow_configuration_p->WLANMonitorConfiguration.autoAssociate;
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      struct ARDrone_MediaFoundation_GtkCBData* mediafoundation_cb_data_p =
        static_cast<struct ARDrone_MediaFoundation_GtkCBData*> (cb_data_base_p);
      mediafoundation_configuration_p =
        mediafoundation_cb_data_p->configuration;
      ACE_ASSERT (mediafoundation_configuration_p);
      SSID_string =
        mediafoundation_configuration_p->WLANMonitorConfiguration.SSID;
      auto_associate_b =
        mediafoundation_configuration_p->WLANMonitorConfiguration.autoAssociate;
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), aborting\n"),
                  cb_data_base_p->mediaFramework));
      break;
    }
  } // end SWITCH
#else
  struct ARDrone_GtkCBData* cb_data_p =
    static_cast<struct ARDrone_GtkCBData*> (cb_data_base_p);
  struct ARDrone_Configuration* configuration_p = cb_data_p->configuration;
  ACE_ASSERT (configuration_p);
  SSID_string = configuration_p->WLANMonitorConfiguration.SSID;
  auto_associate_b =
    configuration_p->WLANMonitorConfiguration.autoAssociate;
#endif // ACE_WIN32 || ACE_WIN64

  Common_UI_GTK_BuildersIterator_t iterator =
    cb_data_base_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  ACE_ASSERT (iterator != cb_data_base_p->builders.end ());
  ARDrone_WLANMonitor_t* WLAN_monitor_p =
    ARDRONE_WLANMONITOR_SINGLETON::instance ();
  ACE_ASSERT (WLAN_monitor_p);
  if (ACE_OS::strcmp (SSID_string.c_str (),
                      WLAN_monitor_p->SSID ().c_str ()))
    return G_SOURCE_REMOVE; // nothing to do

  GtkToggleAction* toggle_action_p =
    GTK_TOGGLE_ACTION (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_TOGGLEACTION_CONNECT)));
  ACE_ASSERT (toggle_action_p);
  gtk_action_set_sensitive (GTK_ACTION (toggle_action_p),
                            TRUE);
  if (auto_associate_b)
  {
    GtkSpinner* spinner_p =
        GTK_SPINNER (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINNER)));
    ACE_ASSERT (spinner_p);
    gtk_spinner_stop (spinner_p);
    gtk_widget_set_sensitive (GTK_WIDGET (spinner_p),
                              FALSE);
    gtk_widget_set_visible (GTK_WIDGET (spinner_p),
                            FALSE);
  } // end IF

  return G_SOURCE_REMOVE;
}

gboolean
idle_disassociated_SSID_cb (gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::idle_disassociated_SSID_cb"));

  struct ARDrone_GtkCBData_Base* cb_data_base_p =
    static_cast<struct ARDrone_GtkCBData_Base*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (cb_data_base_p);

  std::string SSID_string;
  bool auto_associate_b = false;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct ARDrone_DirectShow_GtkCBData* directshow_cb_data_p =
    NULL;
  struct ARDrone_DirectShow_Configuration* directshow_configuration_p =
    NULL;
  struct ARDrone_MediaFoundation_GtkCBData* mediafoundation_cb_data_p =
    NULL;
  struct ARDrone_MediaFoundation_Configuration* mediafoundation_configuration_p =
    NULL;
  switch (cb_data_base_p->mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      struct ARDrone_DirectShow_GtkCBData* directshow_cb_data_p =
        static_cast<struct ARDrone_DirectShow_GtkCBData*> (cb_data_base_p);
      directshow_configuration_p = directshow_cb_data_p->configuration;
      ACE_ASSERT (directshow_configuration_p);
      SSID_string = directshow_configuration_p->WLANMonitorConfiguration.SSID;
      auto_associate_b =
        directshow_configuration_p->WLANMonitorConfiguration.autoAssociate;
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      struct ARDrone_MediaFoundation_GtkCBData* mediafoundation_cb_data_p =
        static_cast<struct ARDrone_MediaFoundation_GtkCBData*> (cb_data_base_p);
      mediafoundation_configuration_p =
        mediafoundation_cb_data_p->configuration;
      ACE_ASSERT (mediafoundation_configuration_p);
      SSID_string =
        mediafoundation_configuration_p->WLANMonitorConfiguration.SSID;
      auto_associate_b =
        mediafoundation_configuration_p->WLANMonitorConfiguration.autoAssociate;
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), aborting\n"),
                  cb_data_base_p->mediaFramework));
      break;
    }
  } // end SWITCH
#else
  struct ARDrone_GtkCBData* cb_data_p =
    static_cast<struct ARDrone_GtkCBData*> (cb_data_base_p);
  struct ARDrone_Configuration* configuration_p = cb_data_p->configuration;
  ACE_ASSERT (configuration_p);
  SSID_string = configuration_p->WLANMonitorConfiguration.SSID;
  auto_associate_b =
    configuration_p->WLANMonitorConfiguration.autoAssociate;
#endif // ACE_WIN32 || ACE_WIN64

    Common_UI_GTK_BuildersIterator_t iterator =
      cb_data_base_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  ACE_ASSERT (iterator != cb_data_base_p->builders.end ());

  ARDrone_WLANMonitor_t* WLAN_monitor_p =
      ARDRONE_WLANMONITOR_SINGLETON::instance ();
  ACE_ASSERT (WLAN_monitor_p);
  if (ACE_OS::strcmp (SSID_string.c_str (),
                      WLAN_monitor_p->SSID ().c_str ()))
    return G_SOURCE_REMOVE; // nothing to do

  GtkToggleAction* toggle_action_p =
    GTK_TOGGLE_ACTION (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_TOGGLEACTION_CONNECT)));
  ACE_ASSERT (toggle_action_p);
  gtk_action_set_sensitive (GTK_ACTION (toggle_action_p),
                            TRUE);
  if (auto_associate_b)
  {
    GtkSpinner* spinner_p =
        GTK_SPINNER (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINNER)));
    ACE_ASSERT (spinner_p);
    gtk_spinner_start (spinner_p);
    gtk_widget_set_sensitive (GTK_WIDGET (spinner_p),
                              TRUE);
    gtk_widget_set_visible (GTK_WIDGET (spinner_p),
                            TRUE);
  } // end IF

  return G_SOURCE_REMOVE;
}

gboolean
idle_initialize_ui_cb (gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::idle_initialize_ui_cb"));

  struct ARDrone_GtkCBData_Base* cb_data_base_p =
    static_cast<struct ARDrone_GtkCBData_Base*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (cb_data_base_p);

  bool auto_associate_b = false;
  bool use_proactor_b = false;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct ARDrone_DirectShow_GtkCBData* directshow_cb_data_p =
    NULL;
  struct ARDrone_DirectShow_Configuration* directshow_configuration_p =
    NULL;
  struct ARDrone_MediaFoundation_GtkCBData* mediafoundation_cb_data_p =
    NULL;
  struct ARDrone_MediaFoundation_Configuration* mediafoundation_configuration_p =
    NULL;
  switch (cb_data_base_p->mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      directshow_cb_data_p =
        static_cast<struct ARDrone_DirectShow_GtkCBData*> (cb_data_base_p);
      directshow_configuration_p = directshow_cb_data_p->configuration;
      ACE_ASSERT (directshow_configuration_p);
      auto_associate_b =
        directshow_configuration_p->WLANMonitorConfiguration.autoAssociate;
      use_proactor_b =
        (directshow_configuration_p->dispatchConfiguration.numberOfProactorThreads > 0);
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      mediafoundation_cb_data_p =
        static_cast<struct ARDrone_MediaFoundation_GtkCBData*> (cb_data_base_p);
      mediafoundation_configuration_p =
        mediafoundation_cb_data_p->configuration;
      ACE_ASSERT (mediafoundation_configuration_p);
      auto_associate_b =
        mediafoundation_configuration_p->WLANMonitorConfiguration.autoAssociate;
      use_proactor_b =
        (mediafoundation_configuration_p->dispatchConfiguration.numberOfProactorThreads > 0);
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), aborting\n"),
                  cb_data_base_p->mediaFramework));
      break;
    }
  } // end SWITCH
#else
  struct ARDrone_GtkCBData* cb_data_p =
    static_cast<struct ARDrone_GtkCBData*> (cb_data_base_p);
  struct ARDrone_Configuration* configuration_p = cb_data_p->configuration;
  ACE_ASSERT (configuration_p);
  auto_associate_b =
    configuration_p->WLANMonitorConfiguration.autoAssociate;
  use_proactor_b =
    (configuration_p->dispatchConfiguration.numberOfProactorThreads > 0);
#endif // ACE_WIN32 || ACE_WIN64

  Common_UI_GTK_BuildersIterator_t iterator =
    cb_data_base_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != cb_data_base_p->builders.end ());

  GtkDialog* dialog_p =
    GTK_DIALOG (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_DIALOG_MAIN)));
  ACE_ASSERT (dialog_p);

  GtkAboutDialog* about_dialog_p =
    GTK_ABOUT_DIALOG (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_DIALOG_ABOUT)));
  ACE_ASSERT (about_dialog_p);

  // configuration -----------------------
  GtkListStore* list_store_p =
    GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                            ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_LISTSTORE_WLAN_INTERFACE)));
  ACE_ASSERT (list_store_p);
  gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE (list_store_p),
                                        0, GTK_SORT_ASCENDING);
  if (!load_wlan_interfaces (list_store_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ::load_wlan_interfaces(), aborting\n")));
    return G_SOURCE_REMOVE;
  } // end IF
  GtkComboBox* combo_box_p =
    GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                           ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_COMBOBOX_WLAN_INTERFACE)));
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

  //GtkEntryBuffer* entrybuffer_p =
  //  GTK_ENTRY_BUFFER (gtk_builder_get_object ((*iterator).second.second,
  //                                            ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_ENTRYBUFFER_SSID)));
  //ACE_ASSERT (entrybuffer_p);
  //gtk_entry_buffer_set_text (entrybuffer_p,
  //                           ACE_TEXT_ALWAYS_CHAR (cb_data_p->configuration->WLANMonitorConfiguration.SSID.c_str ()),
  //                           cb_data_p->configuration->WLANMonitorConfiguration.SSID.size ());
  GtkEntry* entry_p =
    GTK_ENTRY (gtk_builder_get_object ((*iterator).second.second,
                                       ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_ENTRY_SSID)));
  ACE_ASSERT (entry_p);
  gtk_entry_set_text (entry_p,
                      ACE_TEXT_ALWAYS_CHAR (cb_data_p->configuration->WLANMonitorConfiguration.SSID.c_str ()));

  entry_p =
    GTK_ENTRY (gtk_builder_get_object ((*iterator).second.second,
                                       ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_ENTRY_ADDRESS)));
  ACE_ASSERT (entry_p);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ARDrone_DirectShow_ConnectionConfigurationIterator_t directshow_iterator_2;
  ARDrone_DirectShow_Stream_ConnectionConfigurationIterator_t directshow_iterator_2_2;
  ARDrone_MediaFoundation_ConnectionConfigurationIterator_t mediafoundation_iterator_2;
  ARDrone_MediaFoundation_Stream_ConnectionConfigurationIterator_t mediafoundation_iterator_2_2;
  switch (cb_data_base_p->mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      directshow_iterator_2 =
        directshow_configuration_p->connectionConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
      ACE_ASSERT (directshow_iterator_2 != directshow_configuration_p->connectionConfigurations.end ());
      //ARDrone_DirectShow_Stream_ConnectionConfigurationIterator_t directshow_iterator_2_2 ((*directshow_iterator_2).second,
      //                                                                                     0);
      directshow_iterator_2_2 =
        (*directshow_iterator_2).second.find (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING));
      //(*directshow_iterator_2).second.find (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING),
      //                                      directshow_iterator_2_2);
      ACE_ASSERT (directshow_iterator_2_2 != (*directshow_iterator_2).second.end ());
      gtk_entry_set_text (entry_p,
                          ACE_TEXT_ALWAYS_CHAR (Net_Common_Tools::IPAddressToString ((*directshow_iterator_2_2).second.socketHandlerConfiguration.socketConfiguration_2.address, true).c_str ());
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      mediafoundation_iterator_2 =
        mediafoundation_configuration_p->connectionConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
      ACE_ASSERT (mediafoundation_iterator_2 != mediafoundation_configuration_p->connectionConfigurations.end ());
      mediafoundation_iterator_2_2 =
        (*mediafoundation_iterator_2).second.find (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING));
      ACE_ASSERT (mediafoundation_iterator_2_2 != (*mediafoundation_iterator_2).second.end ());
      gtk_entry_set_text (entry_p,
                          ACE_TEXT_ALWAYS_CHAR (Net_Common_Tools::IPAddressToString ((*mediafoundation_iterator_2_2).second.socketHandlerConfiguration.socketConfiguration_2.address, true).c_str ());
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), aborting\n"),
                  cb_data_base_p->mediaFramework));
      return G_SOURCE_REMOVE;
    }
  } // end SWITCH
#else
  ARDrone_ConnectionConfigurationIterator_t iterator_2 =
    configuration_p->connectionConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
  ACE_ASSERT (iterator_2 != configuration_p->connectionConfigurations.end ());
  ARDrone_Stream_ConnectionConfigurationIterator_t iterator_2_2 =
    (*iterator_2).second.find (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING));
  ACE_ASSERT (iterator_2_2 != (*iterator_2).second.end ());
  gtk_entry_set_text (entry_p,
                      Net_Common_Tools::IPAddressToString ((*iterator_2_2).second.socketHandlerConfiguration.socketConfiguration_2.address, true).c_str ());
#endif // ACE_WIN32 || ACE_WIN64

  GtkSpinButton* spin_button_p =
    GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_BUFFERSIZE)));
  ACE_ASSERT (spin_button_p);
  gtk_spin_button_set_range (spin_button_p,
                             0.0,
                             std::numeric_limits<unsigned int>::max ());
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ARDrone_DirectShow_StreamConfigurationsIterator_t directshow_iterator_3;
  ARDrone_MediaFoundation_StreamConfigurationsIterator_t mediafoundation_iterator_3;
  switch (cb_data_base_p->mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      directshow_iterator_3 =
        directshow_configuration_p->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
      ACE_ASSERT (directshow_iterator_3 != directshow_configuration_p->streamConfigurations.end ());
      gtk_spin_button_set_value (spin_button_p,
                                 (*directshow_iterator_3).second.allocatorConfiguration_.defaultBufferSize);
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      mediafoundation_iterator_3 =
        mediafoundation_configuration_p->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
      ACE_ASSERT (mediafoundation_iterator_3 != mediafoundation_configuration_p->streamConfigurations.end ());
      gtk_spin_button_set_value (spin_button_p,
                                 (*mediafoundation_iterator_3).second.allocatorConfiguration_.defaultBufferSize);
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), aborting\n"),
                  cb_data_base_p->mediaFramework));
      return G_SOURCE_REMOVE;
    }
  } // end SWITCH
#else
  ARDrone_StreamConfigurationsIterator_t iterator_3 =
    configuration_p->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
  ACE_ASSERT (iterator_3 != configuration_p->streamConfigurations.end ());
  gtk_spin_button_set_value (spin_button_p,
    (*iterator_3).second.allocatorConfiguration_.defaultBufferSize);
#endif // ACE_WIN32 || ACE_WIN64

  list_store_p =
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
  combo_box_p =
    GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                           ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_COMBOBOX_DISPLAY_DEVICE)));
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
  std::string target_filename_string;
  bool is_fullscreen = false;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ARDrone_DirectShow_StreamConfiguration_t::ITERATOR_T directshow_iterator_4;
  ARDrone_MediaFoundation_StreamConfiguration_t::ITERATOR_T mediafoundation_iterator_4;
  switch (cb_data_base_p->mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      directshow_iterator_4 =
        (*directshow_iterator_3).second.find (ACE_TEXT_ALWAYS_CHAR (""));
      ACE_ASSERT (directshow_iterator_4 != (*directshow_iterator_3).second.end ());
      target_filename_string =
        (*directshow_iterator_4).second.second.targetFileName;
      is_fullscreen =
        (*directshow_iterator_4).second.second.fullScreen;
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      mediafoundation_iterator_4 =
        (*mediafoundation_iterator_3).second.find (ACE_TEXT_ALWAYS_CHAR (""));
      ACE_ASSERT (mediafoundation_iterator_4 != (*mediafoundation_iterator_3).second.end ());
      target_filename_string =
        (*mediafoundation_iterator_4).second.second.targetFileName;
      is_fullscreen =
        (*mediafoundation_iterator_4).second.second.fullScreen;
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), aborting\n"),
                  cb_data_base_p->mediaFramework));
      return G_SOURCE_REMOVE;
    }
  } // end SWITCH
#else
  ARDrone_StreamConfiguration_t::ITERATOR_T iterator_4 =
    (*iterator_3).second.find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (iterator_4 != (*iterator_3).second.end ());
  target_filename_string = (*iterator_4).second.second.targetFileName;
  is_fullscreen = (*iterator_4).second.second.fullScreen;
#endif // ACE_WIN32 || ACE_WIN64
  if (!target_filename_string.empty ())
  {
    // *NOTE*: gtk does not complain if the file doesn't exist, but the button
    //         will display "(None)" --> create empty file
    if (!Common_File_Tools::isReadable (target_filename_string))
      if (!Common_File_Tools::create (target_filename_string))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Common_File_Tools::create(\"%s\"): \"%m\", aborting\n"),
                    ACE_TEXT (target_filename_string.c_str ())));
        return G_SOURCE_REMOVE;
      } // end IF
    //file_p = g_file_new_for_path (target_filename_string.c_str ());
    //ACE_ASSERT (file_p);
    //ACE_ASSERT (g_file_query_exists (file_p, NULL));

    //std::string file_uri =
    //  ACE_TEXT_ALWAYS_CHAR ("file://") +
    //  target_filename_string;
    //if (!gtk_file_chooser_set_current_folder_uri (GTK_FILE_CHOOSER (file_chooser_button_p),
    //                                              file_uri.c_str ()))
    string_p = g_string_new (target_filename_string.c_str ());
    filename_p = string_p->str;
      //Common_UI_Tools::Locale2UTF8 (cb_data_p->configuration->moduleHandlerConfiguration.targetFileName);
    if (!gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (file_chooser_button_p),
                                        filename_p))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to gtk_file_chooser_set_filename(\"%s\"): \"%s\", aborting\n"),
                  ACE_TEXT (target_filename_string.c_str ())));

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
    //              ACE_TEXT (target_filename_string.c_str ()),
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
  default_folder_uri += target_filename_string;
  if (!gtk_file_chooser_set_current_folder_uri (GTK_FILE_CHOOSER (file_chooser_button_p),
                                                default_folder_uri.c_str ()))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_file_chooser_set_current_folder_uri(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (default_folder_uri.c_str ())));
    return G_SOURCE_REMOVE;
  } // end IF

  // orientation
#if defined (GTKGL_SUPPORT)
#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
//  GError* error_p = NULL;
  GtkGLArea* gl_area_p = GTK_GL_AREA (gtk_gl_area_new ());
  if (!gl_area_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to gtk_gl_area_new(), aborting\n")));
    return G_SOURCE_REMOVE;
  } // end IF
  gint major_version, minor_version;
  gtk_gl_area_get_required_version (gl_area_p,
                                    &major_version,
                                    &minor_version);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
//  if (data_p->useMediaFoundation)
//    (*mediafoundation_modulehandler_configuration_iterator).second.second.OpenGLWindow =
//    gl_area_p;
//  else
//    (*directshow_modulehandler_configuration_iterator).second.second.OpenGLWindow =
//    gl_area_p;
#else
//  (*iterator_4).second.second.OpenGLWindow = gl_area_p;
#endif // ACE_WIN32 || ACE_WIN64
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
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to ggla_area_new(), aborting\n")));
    return G_SOURCE_REMOVE;
  } // end IF
#endif /* GTK_CHECK_VERSION (3,16,0) */
  ACE_ASSERT (gl_area_p);
  gtk_widget_set_events (GTK_WIDGET (gl_area_p),
                         GDK_EXPOSURE_MASK |
                         GDK_BUTTON_PRESS_MASK);

#if GTK_CHECK_VERSION (3,16,0)
  // *NOTE*: (try to) enable legacy mode on Win32
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  gtk_gl_area_set_required_version (gl_area_p,
                                    2, 1);
#endif
  gtk_gl_area_set_use_es (gl_area_p,
                          FALSE);
  // *WARNING*: the 'renderbuffer' (in place of 'texture') image attachment
  //            concept appears to be broken; setting this to 'false' gives
  //            "fb setup not supported" (see: gtkglarea.c:734)
  // *TODO*: more specifically, glCheckFramebufferStatusEXT() returns
  //         GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT; find out what is
  //         going on
  // *TODO*: the depth buffer feature is broken on Win32
  gtk_gl_area_set_has_alpha (gl_area_p,
                             TRUE);
  gtk_gl_area_set_has_depth_buffer (gl_area_p,
                                    TRUE);
  gtk_gl_area_set_has_stencil_buffer (gl_area_p,
                                      FALSE);
  gtk_gl_area_set_auto_render (gl_area_p,
                               TRUE);
  gtk_widget_set_can_focus (GTK_WIDGET (gl_area_p),
                            FALSE);
  gtk_widget_set_hexpand (GTK_WIDGET (gl_area_p),
                          TRUE);
  gtk_widget_set_vexpand (GTK_WIDGET (gl_area_p),
                          TRUE);
  //gtk_widget_set_visible (GTK_WIDGET (gl_area_p),
  //                        TRUE);
#endif /* GTK_CHECK_VERSION (3,16,0) */
#else /* GTK_CHECK_VERSION (3,0,0) */
#if defined (GTKGLAREA_SUPPORT)
  /* Attribute list for gtkglarea widget. Specifies a
     list of Boolean attributes and enum/integer
     attribute/value pairs. The last attribute must be
     GGLA_NONE. See glXChooseVisual manpage for further
     explanation.
  */
  int attribute_list_a[] = {
    GDK_GL_RGBA,
    GDK_GL_RED_SIZE,   1,
    GDK_GL_GREEN_SIZE, 1,
    GDK_GL_BLUE_SIZE,  1,
    GDK_GL_DOUBLEBUFFER,
    GDK_GL_NONE
  };

  GtkGLArea* gl_area_p = GTK_GL_AREA (gtk_gl_area_new (attribute_list_a));
  if (!gl_area_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_gl_area_new(): \"%m\", aborting\n")));
    return G_SOURCE_REMOVE;
  } // end ELSE
#else
  GdkGLConfigMode features = static_cast<GdkGLConfigMode> (GDK_GL_MODE_DOUBLE |
                                                           GDK_GL_MODE_ALPHA |
                                                           GDK_GL_MODE_DEPTH |
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
#endif /* GTKGLAREA_SUPPORT */
//  /* Attribute list for gtkglarea widget. Specifies a
//     list of Boolean attributes and enum/integer
//     attribute/value pairs. The last attribute must be
//     GGLA_NONE. See glXChooseVisual manpage for further
//     explanation.
//  */
//  int attribute_list[] = {
//    GDK_GL_RGBA,
//    GDK_GL_RED_SIZE,   1,
//    GDK_GL_GREEN_SIZE, 1,
//    GDK_GL_BLUE_SIZE,  1,
////    GDK_GL_AUX_BUFFERS,
////    GDK_GL_BUFFER_SIZE,
//    GDK_GL_DOUBLEBUFFER,
//    GDK_GL_NONE
//  };

//  GtkGLArea* gl_area_p = GTK_GL_AREA (gtk_gl_area_new (attribute_list));
//  if (!gl_area_p)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to gtk_gl_area_new(): \"%m\", aborting\n")));
//    return G_SOURCE_REMOVE;
//  } // end ELSE
#endif /* GTK_CHECK_VERSION (3,0,0) */
  ACE_ASSERT (gl_area_p);
  GtkBox* box_p =
    GTK_BOX (gtk_builder_get_object ((*iterator).second.second,
                                     ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_BOX_ORIENTATION)));
  ACE_ASSERT (box_p);
  gtk_box_pack_start (box_p,
                      GTK_WIDGET (gl_area_p),
                      TRUE, // expand
                      TRUE, // fill
                      0);   // padding
#if GTK_CHECK_VERSION (3,8,0)
  gtk_builder_expose_object ((*iterator).second.second,
                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_DRAWINGAREA_OPENGL),
                             G_OBJECT (gl_area_p));
#endif /* GTK_CHECK_VERSION (3,8,0) */
#endif /* GTKGL_SUPPORT */

  // statistic ---------------------------
  spin_button_p =
    GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_SESSIONMESSAGES_CONTROL)));
  ACE_ASSERT (spin_button_p);
  gtk_spin_button_set_range (spin_button_p,
                             0.0,
                             std::numeric_limits<unsigned int>::max ());
  spin_button_p =
    GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_DATAMESSAGES_CONTROL)));
  ACE_ASSERT (spin_button_p);
  gtk_spin_button_set_range (spin_button_p,
                             0.0,
                             std::numeric_limits<unsigned int>::max ());
  spin_button_p =
    GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_SESSIONMESSAGES_NAVDATA)));
  ACE_ASSERT (spin_button_p);
  gtk_spin_button_set_range (spin_button_p,
                             0.0,
                             std::numeric_limits<unsigned int>::max ());
  spin_button_p =
    GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_DATAMESSAGES_NAVDATA)));
  ACE_ASSERT (spin_button_p);
  gtk_spin_button_set_range (spin_button_p,
                             0.0,
                             std::numeric_limits<unsigned int>::max ());
  spin_button_p =
    GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_SESSIONMESSAGES_MAVLINK)));
  ACE_ASSERT (spin_button_p);
  gtk_spin_button_set_range (spin_button_p,
                             0.0,
                             std::numeric_limits<unsigned int>::max ());
  spin_button_p =
    GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_DATAMESSAGES_MAVLINK)));
  ACE_ASSERT (spin_button_p);
  gtk_spin_button_set_range (spin_button_p,
                             0.0,
                             std::numeric_limits<unsigned int>::max ());
  spin_button_p =
    GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_SESSIONMESSAGES_VIDEO)));
  ACE_ASSERT (spin_button_p);
  gtk_spin_button_set_range (spin_button_p,
                             0.0,
                             std::numeric_limits<unsigned int>::max ());
  spin_button_p =
    GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_DATAMESSAGES_VIDEO)));
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
  spin_button_p =
    GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_DATA)));
  ACE_ASSERT (spin_button_p);
  gtk_spin_button_set_range (spin_button_p,
                             0.0,
                             std::numeric_limits<unsigned int>::max ());

  GtkDrawingArea* drawing_area_p =
      GTK_DRAWING_AREA (gtk_builder_get_object ((*iterator).second.second,
                                                ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_DRAWINGAREA_VIDEO)));
  ACE_ASSERT (drawing_area_p);

  GtkProgressBar* progress_bar_p =
    GTK_PROGRESS_BAR (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_PROGRESSBAR)));
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
  cb_data_base_p->contextIds[COMMON_UI_GTK_STATUSCONTEXT_DATA] =
      gtk_statusbar_get_context_id (statusbar_p,
                                    ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_STATUSBAR_CONTEXT_DATA));
  cb_data_base_p->contextIds[COMMON_UI_GTK_STATUSCONTEXT_INFORMATION] =
    gtk_statusbar_get_context_id (statusbar_p,
                                  ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_STATUSBAR_CONTEXT_INFORMATION));

  // step2: (auto-)connect signals/slots
  gtk_builder_connect_signals ((*iterator).second.second,
                               cb_data_base_p);

  gulong result =
    g_signal_connect (G_OBJECT (dialog_p),
                      ACE_TEXT_ALWAYS_CHAR ("destroy"),
                      G_CALLBACK (button_quit_clicked_cb),
                      cb_data_base_p);
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

#if defined (GTKGL_SUPPORT)
  result =
    g_signal_connect (G_OBJECT (gl_area_p),
                      ACE_TEXT_ALWAYS_CHAR ("realize"),
                      G_CALLBACK (glarea_realize_cb),
                      userData_in);
  ACE_ASSERT (result);
#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
  result =
    g_signal_connect (G_OBJECT (gl_area_p),
                      ACE_TEXT_ALWAYS_CHAR ("create-context"),
                      G_CALLBACK (glarea_create_context_cb),
                      userData_in);
  ACE_ASSERT (result);
  result =
    g_signal_connect (G_OBJECT (gl_area_p),
                      ACE_TEXT_ALWAYS_CHAR ("render"),
                      G_CALLBACK (glarea_render_cb),
                      userData_in);
  ACE_ASSERT (result);
  result =
    g_signal_connect (G_OBJECT (gl_area_p),
                      ACE_TEXT_ALWAYS_CHAR ("resize"),
                      G_CALLBACK (glarea_resize_cb),
                      userData_in);
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
#endif
#else
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

//  // step5: use correct screen
//  if (parentWidget_in)
//    gtk_window_set_screen (GTK_WINDOW (dialog),
//                           gtk_widget_get_screen (const_cast<GtkWidget*> (//parentWidget_in)));

  // step6: draw main window
  gtk_widget_show_all (GTK_WIDGET (dialog_p));

  // step7: initialize updates
  guint event_source_id = 0;
  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, cb_data_base_p->lock, G_SOURCE_REMOVE);
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
    event_source_id =
      g_timeout_add (COMMON_UI_GTK_INTERVAL_DEFAULT_WIDGET_REFRESH,
                     idle_update_info_display_cb,
                     cb_data_base_p);
    if (event_source_id > 0)
      cb_data_base_p->eventSourceIds.insert (event_source_id);
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
  switch (cb_data_base_p->mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      ACE_ASSERT (!(*directshow_iterator_4).second.second.window);
      //(*iterator_4).second.window =
      //  static_cast<HWND> (GDK_WINDOW_HWND (window_p));
      //gdk_win32_window_get_impl_hwnd (window_p);
      //ACE_ASSERT (cb_data_p->configuration->moduleHandlerConfiguration.window);
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      ACE_ASSERT (!(*mediafoundation_iterator_4).second.second.window);
      //(*iterator_4).second.window =
      //  static_cast<HWND> (GDK_WINDOW_HWND (window_p));
      //gdk_win32_window_get_impl_hwnd (window_p);
      //ACE_ASSERT (cb_data_p->configuration->moduleHandlerConfiguration.window);
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), aborting\n"),
                  cb_data_base_p->mediaFramework));
      return G_SOURCE_REMOVE;
    }
  } // end SWITCH
#else
  ACE_ASSERT (!(*iterator_4).second.second.window);
  (*iterator_4).second.second.window = window_p;
#endif
  //ACE_ASSERT ((*iterator_4).second.window);
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("drawing area display window (handle: %@)\n"),
//              (*iterator_4).second.window));

  // step9: activate some widgets
  gint n_rows = 0;
  list_store_p =
    GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                            ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_LISTSTORE_WLAN_INTERFACE)));
  ACE_ASSERT (list_store_p);
  n_rows = gtk_tree_model_iter_n_children (GTK_TREE_MODEL (list_store_p),
                                           NULL);
  gtk_widget_set_sensitive (GTK_WIDGET (combo_box_p), (n_rows > 0));
  bool activate_combobox_network_interface = (n_rows > 0);
  list_store_p =
    GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                            ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_LISTSTORE_DISPLAY_DEVICE)));
  ACE_ASSERT (list_store_p);
  n_rows = gtk_tree_model_iter_n_children (GTK_TREE_MODEL (list_store_p),
                                           NULL);
  gtk_widget_set_sensitive (GTK_WIDGET (combo_box_p), (n_rows > 0));
  bool activate_combobox_display_interface = (n_rows > 0);
  int primary_display_monitor_index = 0;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  HMONITOR monitor_h = NULL;
  if (!Stream_Module_Device_Tools::getDisplayDevice (ACE_TEXT_ALWAYS_CHAR (""),
                                                     monitor_h))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Stream_Module_Device_Tools::getDisplayDevice(\"\"), aborting\n")));
    return G_SOURCE_REMOVE;
  } // end IF
  ACE_ASSERT (monitor_h);
  MONITORINFOEX monitor_info;
  monitor_info.cbSize = sizeof (MONITORINFOEX);
  if (!GetMonitorInfo (monitor_h,
                       &monitor_info))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to GetMonitorInfo(): \"%s\", aborting\n"),
                ACE_TEXT (Common_Tools::errorToString (GetLastError ()).c_str ())));
    return G_SOURCE_REMOVE;
  } // end IF
  GtkTreeIter tree_iterator;
  GValue value;
  for (gtk_tree_model_get_iter_first (GTK_TREE_MODEL (list_store_p),
                                      &tree_iterator);
       gtk_tree_model_iter_next (GTK_TREE_MODEL (list_store_p),
                                 &tree_iterator);
       ++primary_display_monitor_index)
  {
#if GTK_CHECK_VERSION (3,0,0)
    value = G_VALUE_INIT;
#else
    g_value_init (&value, G_TYPE_STRING);
#endif
    gtk_tree_model_get_value (GTK_TREE_MODEL (list_store_p),
                              &tree_iterator,
                              0, &value);
    ACE_ASSERT (G_VALUE_TYPE (&value) == G_TYPE_STRING);
    if (!ACE_OS::strcmp (g_value_get_string (&value),
#if defined (UNICODE)
                         ACE_TEXT_ALWAYS_CHAR (ACE_TEXT_WCHAR_TO_TCHAR (monitor_info.szDevice))))
#else
                         monitor_info.szDevice))
#endif
    {
      g_value_unset (&value);
      break;
    } // end IF
    g_value_unset (&value);
  } // end FOR
#endif

  combo_box_p =
    GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                           ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_COMBOBOX_WLAN_INTERFACE)));
  ACE_ASSERT (combo_box_p);
  if (activate_combobox_network_interface)
    gtk_combo_box_set_active (combo_box_p, 0);
  combo_box_p =
    GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                           ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_COMBOBOX_DISPLAY_DEVICE)));
  ACE_ASSERT (combo_box_p);
  if (activate_combobox_display_interface)
    gtk_combo_box_set_active (combo_box_p,
                              primary_display_monitor_index);

  //list_store_p =
  //  GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
  //                                          ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_LISTSTORE_DISPLAY_FORMAT)));
  //ACE_ASSERT (list_store_p);
  //n_rows =
  //  gtk_tree_model_iter_n_children (GTK_TREE_MODEL (list_store_p), NULL);
  //gtk_widget_set_sensitive (GTK_WIDGET (combo_box_p), (n_rows > 0));
  //if (n_rows)
  //{
  //  combo_box_p =
  //    GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
  //                                           ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_COMBOBOX_DISPLAY_FORMAT)));
  //  ACE_ASSERT (combo_box_p);
  //  gtk_widget_set_sensitive (GTK_WIDGET (combo_box_p), true);
  //  gtk_combo_box_set_active (combo_box_p, 0);
  //} // end IF
  //else
  //  gtk_action_set_sensitive (GTK_ACTION (toggle_action_p), false);

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
    gtk_widget_set_sensitive (GTK_WIDGET (combo_box_p), TRUE);
    gtk_combo_box_set_active (combo_box_p, 0);
  } // end IF

  GtkCheckButton* check_button_p =
    GTK_CHECK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_CHECKBUTTON_VIDEO)));
  ACE_ASSERT (check_button_p);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_button_p),
                                cb_data_base_p->enableVideo);
  GtkToggleAction* toggle_action_p =
    GTK_TOGGLE_ACTION (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_TOGGLEACTION_FULLSCREEN)));
  ACE_ASSERT (toggle_action_p);
  gtk_toggle_action_set_active (toggle_action_p, is_fullscreen);
//  gtk_widget_set_sensitive (GTK_WIDGET (toggle_button_p),
//                            cb_data_p->enableVideo);
  toggle_action_p =
        GTK_TOGGLE_ACTION (gtk_builder_get_object ((*iterator).second.second,
                                                   ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_TOGGLEACTION_ORIENTATION)));
  ACE_ASSERT (toggle_action_p);
  gtk_toggle_action_set_active (toggle_action_p, TRUE);

  bool is_active = !target_filename_string.empty ();
  if (is_active)
  {
    GtkToggleButton* toggle_button_p =
          GTK_TOGGLE_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                     ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_CHECKBUTTON_SAVE)));
    ACE_ASSERT (toggle_button_p);
    gtk_toggle_button_set_active (toggle_button_p, TRUE);
  } // end IF

  check_button_p =
    GTK_CHECK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_CHECKBUTTON_ASYNCH)));
  ACE_ASSERT (check_button_p);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_button_p),
                                use_proactor_b);
  check_button_p =
    GTK_CHECK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_CHECKBUTTON_ASSOCIATE)));
  ACE_ASSERT (check_button_p);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (check_button_p),
                                auto_associate_b);

  ///* Get Icons shown on buttons */
  //GtkSettings* settings_p = gtk_settings_get_default ();
  //ACE_ASSERT (settings_p);
  //gtk_settings_set_long_property (settings_p,
  //                                ACE_TEXT_ALWAYS_CHAR ("gtk-button-images"),
  //                                TRUE,
  //                                ACE_TEXT_ALWAYS_CHAR ("main"));

#if defined (GTKGL_SUPPORT)
  window_p = gtk_widget_get_window (GTK_WIDGET (gl_area_p));
  ACE_ASSERT (window_p);
  GdkFrameClock* frame_clock_p = gdk_window_get_frame_clock (window_p);
  ACE_ASSERT (frame_clock_p);
  gdk_frame_clock_end_updating (frame_clock_p);

  ACE_ASSERT (!cb_data_base_p->openGLRefreshId);
  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard,  cb_data_base_p->lock, G_SOURCE_REMOVE);
    // schedule asynchronous updates of the state
    cb_data_base_p->openGLRefreshId =
      g_timeout_add (COMMON_UI_GTK_INTERVAL_DEFAULT_OPENGL_REFRESH,
                     idle_update_orientation_display_cb,
                     userData_in);
    if (cb_data_base_p->openGLRefreshId > 0)
      cb_data_base_p->eventSourceIds.insert (cb_data_base_p->openGLRefreshId);
    else
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to g_timeout_add(): \"%m\", aborting\n")));
      return G_SOURCE_REMOVE;
    } // end ELSE
  } // end lock scope
#endif // GTKGL_SUPPORT

  return G_SOURCE_REMOVE;
}

gboolean
idle_finalize_ui_cb (gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::idle_finalize_ui_cb"));

  struct ARDrone_GtkCBData_Base* cb_data_base_p =
      static_cast<struct ARDrone_GtkCBData_Base*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (cb_data_base_p);

  unsigned int num_messages = 0;
  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, cb_data_base_p->lock, G_SOURCE_REMOVE);
    num_messages = cb_data_base_p->messages.size ();
    while (!cb_data_base_p->messages.empty ())
    {
      cb_data_base_p->messages.front ()->release ();
      cb_data_base_p->messages.pop_front ();
    } // end WHILE

    //if (cb_data_base_p->openGLRefreshId)
    //{
    //  g_source_remove (cb_data_base_p->openGLRefreshId);
    //  cb_data_base_p->openGLRefreshId = 0;
    //} // end iF
    cb_data_base_p->eventSourceIds.clear ();

    //if (glIsList (cb_data_p->openGLAxesListId))
    //{
    //  glDeleteLists (cb_data_p->openGLAxesListId, 1);
    //  cb_data_p->openGLAxesListId = 0;
    //} // end IF
  } // end lock scope
  if (num_messages)
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("flushed %u message(s)\n"),
                num_messages));

  // leave GTK
  gtk_main_quit ();

  // one-shot action
  return G_SOURCE_REMOVE;
}

gboolean
idle_session_end_cb (gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::idle_session_end_cb"));

  struct ARDrone_GtkCBData_Base* cb_data_base_p =
    static_cast<struct ARDrone_GtkCBData_Base*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (cb_data_base_p);

  Common_UI_GTK_BuildersIterator_t iterator =
    cb_data_base_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != cb_data_base_p->builders.end ());

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  struct ARDrone_GtkCBData* cb_data_p =
    static_cast<struct ARDrone_GtkCBData*> (userData_in);
  ACE_ASSERT (cb_data_p);

  ARDrone_StreamConfigurationsIterator_t iterator_2 =
      cb_data_p->configuration->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
  ACE_ASSERT (iterator_2 != cb_data_p->configuration->streamConfigurations.end ());
  ARDrone_StreamConfiguration_t::ITERATOR_T iterator_3 =
    (*iterator_2).second.find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (iterator_3 != (*iterator_2).second.end ());
#endif

  // *IMPORTANT NOTE*: there are two major reasons for being here that are not
  //                   mutually exclusive, so there could be a race:
  //                   - user pressed stop
  //                   - there was a serious error on some stream
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

  //// stop progress reporting ?
  //if (data_p->progressData->eventSourceId)
  //{ ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, data_p->lock, G_SOURCE_REMOVE);
  //  if (!g_source_remove (data_p->progressData->eventSourceId))
  //    ACE_DEBUG ((LM_ERROR,
  //                ACE_TEXT ("failed to g_source_remove(%u), continuing\n"),
  //                data_p->progressData->eventSourceId));
  //  data_p->eventSourceIds.erase (data_p->progressData->eventSourceId);
  //  data_p->progressData->eventSourceId = 0;

  //  GtkProgressBar* progress_bar_p =
  //    GTK_PROGRESS_BAR (gtk_builder_get_object ((*iterator).second.second,
  //                                              ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_PROGRESSBAR)));
  //  ACE_ASSERT (progress_bar_p);
  //  // *NOTE*: this disables "activity mode" (in Gtk2)
  //  gtk_progress_bar_set_fraction (progress_bar_p, 0.0);
  //  //gtk_progress_bar_set_text (progress_bar_p, ACE_TEXT_ALWAYS_CHAR (""));
  //  gtk_widget_set_sensitive (GTK_WIDGET (progress_bar_p), false);
  //} // end IF

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

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  if ((*iterator_3).second.second.fullScreen)
  {
    GtkWindow* window_p =
        GTK_WINDOW (gtk_builder_get_object ((*iterator).second.second,
                                            ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_WINDOW_FULLSCREEN)));
    ACE_ASSERT (window_p);
    gtk_widget_hide (GTK_WIDGET (window_p));
  } // end IF
#endif

  if (cb_data_base_p->stateEventId)
  {
    if (!g_source_remove (cb_data_base_p->stateEventId))
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to g_source_remove(%u): \"%m\", continuing\n"),
                  cb_data_base_p->stateEventId));
    cb_data_base_p->stateEventId = 0;
  } // end IF

  return G_SOURCE_REMOVE;
}
gboolean
idle_session_start_cb (gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::idle_session_start_cb"));

  struct ARDrone_GtkCBData_Base* data_p =
    static_cast<struct ARDrone_GtkCBData_Base*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  Common_UI_GTK_BuildersIterator_t iterator =
    data_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->builders.end ());

  // update widgets
  GtkToggleAction* toggle_action_p =
    GTK_TOGGLE_ACTION (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_TOGGLEACTION_CONNECT)));
  ACE_ASSERT (toggle_action_p);
  // re-toggle
  un_toggling_connect = true;
  guint result =
      g_signal_handlers_block_by_func (G_OBJECT (toggle_action_p),
                                       (gpointer)toggleaction_connect_toggled_cb,
                                       userData_in);
  ACE_ASSERT (result);
  gtk_toggle_action_set_active (toggle_action_p, FALSE);
  result =
    g_signal_handlers_unblock_by_func (G_OBJECT (toggle_action_p),
                                       (gpointer)toggleaction_connect_toggled_cb,
                                       userData_in);
  ACE_ASSERT (result);
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

  ACE_ASSERT (!data_p->stateEventId);
  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, data_p->lock, G_SOURCE_REMOVE);
    // schedule asynchronous updates of the state
    data_p->stateEventId =
      g_timeout_add (COMMON_UI_GTK_INTERVAL_DEFAULT_WIDGET_REFRESH,
                     idle_update_state_cb,
                     userData_in);
    if (data_p->stateEventId > 0)
      data_p->eventSourceIds.insert (data_p->stateEventId);
    else
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to g_timeout_add(): \"%m\", aborting\n")));
      return G_SOURCE_REMOVE;
    } // end ELSE
  } // end lock scope

  return G_SOURCE_REMOVE;
}

gboolean
idle_reset_ui_cb (gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::idle_reset_ui_cb"));

  struct ARDrone_GtkCBData_Base* data_base_p =
    static_cast<struct ARDrone_GtkCBData_Base*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_base_p);

  Common_UI_GTK_BuildersIterator_t iterator =
    data_base_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_base_p->builders.end ());

  GtkSpinButton* spin_button_p =
    GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_SESSIONMESSAGES_CONTROL)));
  ACE_ASSERT (spin_button_p);
  gtk_spin_button_set_value (spin_button_p, 0.0);
  spin_button_p =
    GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_DATAMESSAGES_CONTROL)));
  ACE_ASSERT (spin_button_p);
  gtk_spin_button_set_value (spin_button_p, 0.0);
  GtkProgressBar* progress_bar_p =
    GTK_PROGRESS_BAR (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_PROGRESSBAR_CONTROL)));
  ACE_ASSERT (progress_bar_p);
  gtk_progress_bar_set_text (progress_bar_p, ACE_TEXT_ALWAYS_CHAR (""));
  spin_button_p =
    GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_SESSIONMESSAGES_NAVDATA)));
  ACE_ASSERT (spin_button_p);
  gtk_spin_button_set_value (spin_button_p, 0.0);
  spin_button_p =
    GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_DATAMESSAGES_NAVDATA)));
  ACE_ASSERT (spin_button_p);
  gtk_spin_button_set_value (spin_button_p, 0.0);
  progress_bar_p =
    GTK_PROGRESS_BAR (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_PROGRESSBAR_NAVDATA)));
  ACE_ASSERT (progress_bar_p);
  gtk_progress_bar_set_text (progress_bar_p, ACE_TEXT_ALWAYS_CHAR (""));
  spin_button_p =
    GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_SESSIONMESSAGES_MAVLINK)));
  ACE_ASSERT (spin_button_p);
  gtk_spin_button_set_value (spin_button_p, 0.0);
  spin_button_p =
    GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_DATAMESSAGES_MAVLINK)));
  ACE_ASSERT (spin_button_p);
  gtk_spin_button_set_value (spin_button_p, 0.0);
  progress_bar_p =
    GTK_PROGRESS_BAR (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_PROGRESSBAR_MAVLINK)));
  ACE_ASSERT (progress_bar_p);
  gtk_progress_bar_set_text (progress_bar_p, ACE_TEXT_ALWAYS_CHAR (""));
  spin_button_p =
    GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_SESSIONMESSAGES_VIDEO)));
  ACE_ASSERT (spin_button_p);
  gtk_spin_button_set_value (spin_button_p, 0.0);
  spin_button_p =
    GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_DATAMESSAGES_VIDEO)));
  ACE_ASSERT (spin_button_p);
  gtk_spin_button_set_value (spin_button_p, 0.0);
  progress_bar_p =
    GTK_PROGRESS_BAR (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_PROGRESSBAR_VIDEO)));
  ACE_ASSERT (progress_bar_p);
  gtk_progress_bar_set_text (progress_bar_p, ACE_TEXT_ALWAYS_CHAR (""));

  spin_button_p =
    GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_DATA)));
  ACE_ASSERT (spin_button_p);
  gtk_spin_button_set_value (spin_button_p, 0.0);

  progress_bar_p =
    GTK_PROGRESS_BAR (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_PROGRESSBAR)));
  ACE_ASSERT (progress_bar_p);
  gtk_progress_bar_set_text (progress_bar_p, ACE_TEXT_ALWAYS_CHAR (""));

  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, data_base_p->lock, G_SOURCE_REMOVE);
    data_base_p->progressData.statistic.bytes = 0.0F;
  } // end lock scope

  return G_SOURCE_REMOVE;
}

gboolean
idle_update_info_display_cb (gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::idle_update_info_display_cb"));

  struct ARDrone_GtkCBData_Base* cb_data_base_p =
    static_cast<struct ARDrone_GtkCBData_Base*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (cb_data_base_p);

  Common_UI_GTK_BuildersIterator_t iterator =
    cb_data_base_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != cb_data_base_p->builders.end ());

  GtkProgressBar* progress_bar_p = NULL;
  GtkSpinButton* spin_button_p = NULL;
  bool is_session_message = false;

  ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, cb_data_base_p->lock, G_SOURCE_REMOVE);

  // sanity check(s)
  if (cb_data_base_p->eventStack.empty ())
    return G_SOURCE_CONTINUE;

  for (ARDrone_EventsIterator_t iterator_2 = cb_data_base_p->eventStack.begin ();
       iterator_2 != cb_data_base_p->eventStack.end ();
       ++iterator_2)
  { // step1: process event
    is_session_message = false;
    switch ((*iterator_2).second)
    {
      case ARDRONE_EVENT_CONNECT:
      {
        spin_button_p =
            GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                     ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_CONNECTIONS)));
        ACE_ASSERT (spin_button_p);
        gtk_spin_button_spin (spin_button_p,
                              GTK_SPIN_STEP_FORWARD,
                              1.0);

        is_session_message = true;

        break;
      }
      case ARDRONE_EVENT_DISCONNECT:
      {
        spin_button_p =
            GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                     ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_CONNECTIONS)));
        ACE_ASSERT (spin_button_p);
        gtk_spin_button_spin (spin_button_p,
                              GTK_SPIN_STEP_BACKWARD,
                              1.0);

        is_session_message = true;

        break;
      }
      case ARDRONE_EVENT_MESSAGE_DATA:
      {
        spin_button_p =
          GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                   ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_DATA)));
        ACE_ASSERT (spin_button_p);
        gtk_spin_button_set_value (spin_button_p,
                                   static_cast<gdouble> (cb_data_base_p->progressData.statistic.bytes));

        break;
      }
      case ARDRONE_EVENT_MESSAGE_SESSION:
      {
        is_session_message = true;
        break;
      }
      case ARDRONE_EVENT_RESIZE:
      {
        GtkDrawingArea* drawing_area_p =
            GTK_DRAWING_AREA (gtk_builder_get_object ((*iterator).second.second,
                                                      ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_DRAWINGAREA_VIDEO)));
        ACE_ASSERT (drawing_area_p);

        unsigned int height, width;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
        ARDrone_DirectShow_StreamConfigurationsIterator_t directshow_video_streamconfiguration_iterator;
        ARDrone_MediaFoundation_StreamConfigurationsIterator_t mediafoundation_video_streamconfiguration_iterator;
        switch (cb_data_base_p->mediaFramework)
        {
          case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
          {
            struct ARDrone_DirectShow_GtkCBData* directshow_cb_data_p =
              static_cast<struct ARDrone_DirectShow_GtkCBData*> (userData_in);
            struct ARDrone_DirectShow_Configuration* directshow_configuration_p =
              directshow_cb_data_p->configuration;
            ACE_ASSERT (directshow_configuration_p);
            directshow_video_streamconfiguration_iterator =
              directshow_configuration_p->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
            ACE_ASSERT (directshow_video_streamconfiguration_iterator != directshow_configuration_p->streamConfigurations.end ());
            break;
          }
          case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
          {
            struct ARDrone_MediaFoundation_GtkCBData* mediafoundation_cb_data_p =
              static_cast<struct ARDrone_MediaFoundation_GtkCBData*> (userData_in);
            struct ARDrone_MediaFoundation_Configuration* mediafoundation_configuration_p =
              mediafoundation_cb_data_p->configuration;
            mediafoundation_video_streamconfiguration_iterator =
              mediafoundation_configuration_p->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
            ACE_ASSERT (mediafoundation_video_streamconfiguration_iterator != mediafoundation_configuration_p->streamConfigurations.end ());
            break;
          }
          default:
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("invalid/unknown media framework (was: %d), aborting\n"),
                        cb_data_base_p->mediaFramework));
            return G_SOURCE_REMOVE;
          }
        } // end SWITCH
#else
        struct ARDrone_GtkCBData* cb_data_p =
          static_cast<struct ARDrone_GtkCBData*> (userData_in);
        ACE_ASSERT (cb_data_p);
        struct ARDrone_Configuration* configuration_p =
          cb_data_p->configuration;
        ACE_ASSERT (configuration_p);
        ARDrone_StreamConfigurationsIterator_t video_streamconfiguration_iterator =
            configuration_p->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
        ACE_ASSERT (video_streamconfiguration_iterator != configuration_p->streamConfigurations.end ());
#endif

#if defined (ACE_WIN32) || defined (ACE_WIN64)
        ARDrone_DirectShow_StreamConfiguration_t::ITERATOR_T directshow_iterator_3;
        ARDrone_MediaFoundation_StreamConfiguration_t::ITERATOR_T mediafoundation_iterator_3;
        switch (cb_data_base_p->mediaFramework)
        {
          case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
          {
            directshow_iterator_3 =
              (*directshow_video_streamconfiguration_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (""));
            ACE_ASSERT (directshow_iterator_3 != (*directshow_video_streamconfiguration_iterator).second.end ());

            // sanity check(s)
            ACE_ASSERT ((*directshow_iterator_3).second.second.filterConfiguration);
            ACE_ASSERT ((*directshow_iterator_3).second.second.filterConfiguration->pinConfiguration);
            ACE_ASSERT ((*directshow_iterator_3).second.second.filterConfiguration->pinConfiguration->format);

            ACE_ASSERT ((*directshow_iterator_3).second.second.filterConfiguration->pinConfiguration->format->formattype == FORMAT_VideoInfo);
            ACE_ASSERT ((*directshow_iterator_3).second.second.filterConfiguration->pinConfiguration->format->cbFormat == sizeof (struct tagVIDEOINFOHEADER));
            struct tagVIDEOINFOHEADER* video_info_header_p =
              reinterpret_cast<struct tagVIDEOINFOHEADER*> ((*directshow_iterator_3).second.second.filterConfiguration->pinConfiguration->format->pbFormat);
            height = video_info_header_p->bmiHeader.biHeight;
            width = video_info_header_p->bmiHeader.biWidth;
            break;
          }
          case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
          {
            ACE_ASSERT (false); // *TODO*
            break;
          }
          default:
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("invalid/unknown media framework (was: %d), aborting\n"),
                        cb_data_base_p->mediaFramework));
            return G_SOURCE_REMOVE;
          }
        } // end SWITCH
#else
        ARDrone_StreamConfiguration_t::ITERATOR_T iterator_4 =
          (*video_streamconfiguration_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (MODULE_DEC_DECODER_LIBAV_DECODER_DEFAULT_NAME_STRING));
        ACE_ASSERT (iterator_4 != (*video_streamconfiguration_iterator).second.end ());

        height = (*iterator_4).second.second.sourceFormat.height;
        width = (*iterator_4).second.second.sourceFormat.width;
#endif
        gtk_widget_set_size_request (GTK_WIDGET (drawing_area_p),
                                     width, height);

        is_session_message = true;

        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown event type (was: %d), continuing\n"),
                    (*iterator_2).second));
        break;
      }
    } // end SWITCH

    // step2: update message counter
    switch ((*iterator_2).first)
    {
      case ARDRONE_STREAM_CONTROL:
      {
        progress_bar_p =
          GTK_PROGRESS_BAR (gtk_builder_get_object ((*iterator).second.second,
                                                    ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_PROGRESSBAR_CONTROL)));
        spin_button_p =
          GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                   (is_session_message ? ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_SESSIONMESSAGES_CONTROL)
                                                                       : ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_DATAMESSAGES_CONTROL))));
        break;
      }
      case ARDRONE_STREAM_NAVDATA:
      {
        progress_bar_p =
          GTK_PROGRESS_BAR (gtk_builder_get_object ((*iterator).second.second,
                                                    ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_PROGRESSBAR_NAVDATA)));
        spin_button_p =
          GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                   (is_session_message ? ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_SESSIONMESSAGES_NAVDATA)
                                                                       : ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_DATAMESSAGES_NAVDATA))));
        break;
      }
      case ARDRONE_STREAM_MAVLINK:
      {
        progress_bar_p =
          GTK_PROGRESS_BAR (gtk_builder_get_object ((*iterator).second.second,
                                                    ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_PROGRESSBAR_MAVLINK)));
        spin_button_p =
          GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                   (is_session_message ? ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_SESSIONMESSAGES_MAVLINK)
                                                                       : ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_DATAMESSAGES_MAVLINK))));
        break;
      }
      case ARDRONE_STREAM_VIDEO:
      {
        progress_bar_p =
          GTK_PROGRESS_BAR (gtk_builder_get_object ((*iterator).second.second,
                                                    ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_PROGRESSBAR_VIDEO)));
        spin_button_p =
          GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                   (is_session_message ? ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_SESSIONMESSAGES_VIDEO)
                                                                       : ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_DATAMESSAGES_VIDEO))));
        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown stream type (was: %d), continuing\n"),
                    (*iterator_2).first));
        continue;
      }
    } // end SWITCH
    ACE_ASSERT (spin_button_p);
    gtk_spin_button_spin (spin_button_p,
                          GTK_SPIN_STEP_FORWARD,
                          1.0);
    ACE_ASSERT (progress_bar_p);
    if (!is_session_message)
      gtk_progress_bar_pulse (progress_bar_p);
  } // end FOR
  cb_data_base_p->eventStack.clear ();

  return G_SOURCE_CONTINUE;
}

//gboolean
//idle_update_log_display_cb (gpointer userData_in)
//{
//  ARDRONE_TRACE (ACE_TEXT ("::idle_update_log_display_cb"));
//
//  struct ARDrone_GtkCBData_Base* data_p =
//    static_cast<struct ARDrone_GtkCBData_Base*> (userData_in);
//
//  // sanity check(s)
//  ACE_ASSERT (data_p);
//
//  ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, data_p->logStackLock, G_SOURCE_REMOVE);
//
//  Common_UI_GTK_BuildersIterator_t iterator =
//      data_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
//  // sanity check(s)
//  ACE_ASSERT (iterator != data_p->builders.end ());
//
//  GtkTextView* view_p =
//    GTK_TEXT_VIEW (gtk_builder_get_object ((*iterator).second.second,
//                                           ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_TEXTVIEW_LOG)));
//  ACE_ASSERT (view_p);
//  GtkTextBuffer* buffer_p = gtk_text_view_get_buffer (view_p);
//  ACE_ASSERT (buffer_p);
//
//  GtkTextIter text_iterator;
//  gtk_text_buffer_get_end_iter (buffer_p,
//                                &text_iterator);
//
//  gchar* string_p = NULL;
//  // sanity check
//  if (data_p->logStack.empty ())
//    return G_SOURCE_CONTINUE;
//
//  // step1: convert text
//  for (Common_MessageStackConstIterator_t iterator_2 = data_p->logStack.begin ();
//       iterator_2 != data_p->logStack.end ();
//       ++iterator_2)
//  {
//    string_p = Common_UI_Tools::Locale2UTF8 (*iterator_2);
//    if (!string_p)
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to convert message text (was: \"%s\"), aborting\n"),
//                  ACE_TEXT ((*iterator_2).c_str ())));
//      return G_SOURCE_REMOVE;
//    } // end IF
//
//    // step2: display text
//    gtk_text_buffer_insert (buffer_p,
//                            &text_iterator,
//                            string_p,
//                            -1);
//
//    // clean up
//    g_free (string_p);
//  } // end FOR
//
//  data_p->logStack.clear ();
//
//  // step3: scroll the view accordingly
////  // move the iterator to the beginning of line, so it doesn't scroll
////  // in horizontal direction
////  gtk_text_iter_set_line_offset (&text_iterator, 0);
//
////  // ...and place the mark at iter. The mark will stay there after insertion
////  // because it has "right" gravity
////  GtkTextMark* text_mark_p =
////      gtk_text_buffer_get_mark (buffer_p,
////                                ACE_TEXT_ALWAYS_CHAR (TEST_I_STREAM_UI_GTK_SCROLLMARK_NAME));
//////  gtk_text_buffer_move_mark (buffer_p,
//////                             text_mark_p,
//////                             &text_iterator);
//
////  // scroll the mark onscreen
////  gtk_text_view_scroll_mark_onscreen (view_p,
////                                      text_mark_p);
//  //GtkAdjustment* adjustment_p =
//  //    GTK_ADJUSTMENT (gtk_builder_get_object ((*iterator).second.second,
//  //                                            ACE_TEXT_ALWAYS_CHAR (TEST_I_STREAM_UI_GTK_ADJUSTMENT_NAME)));
//  //ACE_ASSERT (adjustment_p);
//  //gtk_adjustment_set_value (adjustment_p,
//  //                          adjustment_p->upper - adjustment_p->page_size));
//
//  return G_SOURCE_CONTINUE;
//}

gboolean
idle_update_state_cb (gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::idle_update_state_cb"));

  struct ARDrone_GtkCBData_Base* data_p =
    static_cast<struct ARDrone_GtkCBData_Base*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->controller);

  Common_UI_GTK_BuildersIterator_t iterator =
    data_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  ACE_ASSERT (iterator != data_p->builders.end ());

//  uint32_t device_state = data_p->controller->get ();
  struct _navdata_demo_t device_state_2 = data_p->controller->get_2 ();
  std::ostringstream converter;

  GtkLabel* label_p =
    GTK_LABEL (gtk_builder_get_object ((*iterator).second.second,
                                       ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_LABEL_ROLL)));
  ACE_ASSERT (label_p);
  converter << device_state_2.phi;
  gtk_label_set_text (label_p,
                      converter.str ().c_str ());
  label_p =
    GTK_LABEL (gtk_builder_get_object ((*iterator).second.second,
                                       ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_LABEL_PITCH)));
  ACE_ASSERT (label_p);
  converter.clear ();
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter << device_state_2.theta;
  gtk_label_set_text (label_p,
                      converter.str ().c_str ());
  label_p =
    GTK_LABEL (gtk_builder_get_object ((*iterator).second.second,
                                       ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_LABEL_YAW)));
  ACE_ASSERT (label_p);
  converter.clear ();
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter << device_state_2.psi;
  gtk_label_set_text (label_p,
                      converter.str ().c_str ());

  return G_SOURCE_REMOVE;
}

gboolean
idle_update_orientation_display_cb (gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::idle_update_orientation_display_cb"));

  struct ARDrone_GtkCBData_Base* data_p =
    static_cast<struct ARDrone_GtkCBData_Base*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  Common_UI_GTK_BuildersIterator_t iterator =
    data_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  ACE_ASSERT (iterator != data_p->builders.end ());

  std::ostringstream converter;
  converter.precision (ARDRONE_OPENGL_MODEL_ORIENTATION_PRECISION);
  GtkLabel* label_p =
    GTK_LABEL (gtk_builder_get_object ((*iterator).second.second,
                                       ARDRONE_UI_WIDGET_NAME_LABEL_ROLL));
  ACE_ASSERT (label_p);
  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, data_p->lock, G_SOURCE_REMOVE);
    converter << data_p->openGLScene.orientation.x;
    gtk_label_set_text (label_p,
                        ACE_TEXT_ALWAYS_CHAR (converter.str ().c_str ()));

    label_p =
        GTK_LABEL (gtk_builder_get_object ((*iterator).second.second,
                                           ARDRONE_UI_WIDGET_NAME_LABEL_PITCH));
    ACE_ASSERT (label_p);
    converter.clear ();
    converter.str (ACE_TEXT_ALWAYS_CHAR (""));
    converter << data_p->openGLScene.orientation.y;
    gtk_label_set_text (label_p,
                        ACE_TEXT_ALWAYS_CHAR (converter.str ().c_str ()));

    label_p =
        GTK_LABEL (gtk_builder_get_object ((*iterator).second.second,
                                           ARDRONE_UI_WIDGET_NAME_LABEL_YAW));
    ACE_ASSERT (label_p);
    converter.clear ();
    converter.str (ACE_TEXT_ALWAYS_CHAR (""));
    converter << data_p->openGLScene.orientation.z;
    gtk_label_set_text (label_p,
                        ACE_TEXT_ALWAYS_CHAR (converter.str ().c_str ()));
  } // end .lock scope
  GtkGLArea* gl_area_p =
    GTK_GL_AREA (gtk_builder_get_object ((*iterator).second.second,
                                         ARDRONE_UI_WIDGET_NAME_DRAWINGAREA_OPENGL));
  ACE_ASSERT (gl_area_p);

  gtk_gl_area_queue_render (gl_area_p);
//  gdk_window_invalidate_rect (gtk_widget_get_window (GTK_WIDGET (gl_area_p)),
//                              NULL,
//                              false);

  return G_SOURCE_CONTINUE;
}
gboolean
idle_update_video_display_cb (gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::idle_update_video_display_cb"));

  struct ARDrone_GtkCBData_Base* data_p =
    static_cast<struct ARDrone_GtkCBData_Base*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  Common_UI_GTK_BuildersIterator_t iterator =
    data_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  ACE_ASSERT (iterator != data_p->builders.end ());

  GtkToggleAction* toggle_action_p =
      GTK_TOGGLE_ACTION (gtk_builder_get_object ((*iterator).second.second,
                                                 ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_TOGGLEACTION_CONNECT)));
  ACE_ASSERT (toggle_action_p);

  GtkDrawingArea* drawing_area_p =
    GTK_DRAWING_AREA (gtk_builder_get_object ((*iterator).second.second,
                                              gtk_toggle_action_get_active (toggle_action_p) ? ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_DRAWINGAREA_FULLSCREEN)
                                                                                             : ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_DRAWINGAREA_VIDEO)));
  ACE_ASSERT (drawing_area_p);

  gdk_window_invalidate_rect (gtk_widget_get_window (GTK_WIDGET (drawing_area_p)),
                              NULL,
                              false);

  return G_SOURCE_REMOVE;
}

gboolean
idle_update_progress_cb (gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::idle_update_progress_cb"));

  struct ARDrone_GtkProgressData* data_p =
    static_cast<struct ARDrone_GtkProgressData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->state);

  // done ?
  Common_UI_GTK_PendingActionsIterator_t iterator_2;
  int result = -1;
  ACE_THR_FUNC_RETURN exit_status;
  for (Common_UI_GTK_CompletedActionsIterator_t iterator_3 = data_p->completedActions.begin ();
        iterator_3 != data_p->completedActions.end ();
        ++iterator_3)
  {
    iterator_2 = data_p->pendingActions.find (*iterator_3);
    ACE_ASSERT (iterator_2 != data_p->pendingActions.end ());
    result =
      ACE_Thread_Manager::instance ()->join ((*iterator_2).second.id (),
                                             &exit_status);
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Thread_Manager::join(%u): \"%m\", continuing\n"),
                  (*iterator_2).second.id ()));
    else if (exit_status)
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("processing thread (id: %u) has joined (status was: %u)\n"),
                  (*iterator_2).second.id (),
                  exit_status));
#else
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("processing thread (id: %u) has joined (status was: %@)\n"),
                  (*iterator_2).second.id (),
                  exit_status));
#endif
    } // end ELSE IF

    data_p->state->eventSourceIds.erase (*iterator_3);
    data_p->pendingActions.erase (iterator_2);
  } // end FOR
  data_p->completedActions.clear ();

  bool done = false;
  if (data_p->pendingActions.empty ())
  {
    //if (data_p->cursorType != GDK_LAST_CURSOR)
    //{
    //  GdkCursor* cursor_p = gdk_cursor_new (data_p->cursorType);
    //  if (!cursor_p)
    //  {
    //    ACE_DEBUG ((LM_ERROR,
    //                ACE_TEXT ("failed to gdk_cursor_new(%d): \"%m\", continuing\n"),
    //                data_p->cursorType));
    //    return G_SOURCE_REMOVE;
    //  } // end IF
    //  GtkWindow* window_p =
    //    GTK_WINDOW (gtk_builder_get_object ((*iterator).second.second,
    //                                        ACE_TEXT_ALWAYS_CHAR (IRC_CLIENT_GUI_GTK_WINDOW_MAIN)));
    //  ACE_ASSERT (window_p);
    //  GdkWindow* window_2 = gtk_widget_get_window (GTK_WIDGET (window_p));
    //  ACE_ASSERT (window_2);
    //  gdk_window_set_cursor (window_2, cursor_p);
    //  data_p->cursorType = GDK_LAST_CURSOR;
    //} // end IF

    done = true;
  } // end IF

  Common_UI_GTK_BuildersIterator_t iterator =
    data_p->state->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != data_p->state->builders.end ());

  GtkProgressBar* progress_bar_p =
    GTK_PROGRESS_BAR (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_PROGRESSBAR)));
  ACE_ASSERT (progress_bar_p);

  if (done)
  {
    // *NOTE*: this disables "activity mode" (in Gtk2)
    gtk_progress_bar_set_fraction (progress_bar_p, 0.0);
    //gtk_progress_bar_set_text (progress_bar_p, ACE_TEXT_ALWAYS_CHAR (""));
    gtk_widget_set_sensitive (GTK_WIDGET (progress_bar_p), false);

    data_p->eventSourceId = 0;

    return G_SOURCE_REMOVE; // done
  } // end IF

  // update progress bars
  ACE_TCHAR buffer[BUFSIZ];
  ACE_OS::memset (buffer, 0, sizeof (buffer));
  float fps, speed = 0.0F;
  std::string magnitude_string = ACE_TEXT_ALWAYS_CHAR ("byte(s)/s");

  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, data_p->state->lock, G_SOURCE_REMOVE);
    fps   = data_p->statistic.messagesPerSecond;
    speed = data_p->statistic.bytesPerSecond;
  } // end lock scope
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
    //ACE_OS::memset (buffer, 0, sizeof (buffer));
    result = ACE_OS::sprintf (buffer, ACE_TEXT ("%.0f 1/s | %.2f %s"),
                              fps, speed, magnitude_string.c_str ());
    if (result < 0)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_OS::sprintf(): \"%m\", continuing\n")));
  } // end IF
  gtk_progress_bar_set_text (progress_bar_p,
                              ACE_TEXT_ALWAYS_CHAR (buffer));
  gtk_progress_bar_pulse (progress_bar_p);

  ARDroneStreamStatisticConstIterator_t iterator_4;
  ACE_OS::memset (buffer, 0, sizeof (buffer));
  magnitude_string = ACE_TEXT_ALWAYS_CHAR ("byte(s)/s");

  progress_bar_p =
    GTK_PROGRESS_BAR (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_PROGRESSBAR_CONTROL)));
  ACE_ASSERT (progress_bar_p);
  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, data_p->state->lock, G_SOURCE_REMOVE);
    iterator_4 = data_p->statistic.streamStatistic.find (ARDRONE_STREAM_CONTROL);
    ACE_ASSERT (iterator_4 != data_p->statistic.streamStatistic.end ());
    fps = (*iterator_4).second.messagesPerSecond;
    speed = (*iterator_4).second.bytesPerSecond;
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
      //ACE_OS::memset (buffer, 0, sizeof (buffer));
      result = ACE_OS::sprintf (buffer, ACE_TEXT ("%.0f 1/s | %.2f %s"),
                                fps, speed, magnitude_string.c_str ());
      if (result < 0)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_OS::sprintf(): \"%m\", continuing\n")));
    } // end IF
    gtk_progress_bar_set_text (progress_bar_p,
                               ACE_TEXT_ALWAYS_CHAR (buffer));

    ACE_OS::memset (buffer, 0, sizeof (buffer));
    magnitude_string = ACE_TEXT_ALWAYS_CHAR ("byte(s)/s");

    progress_bar_p =
      GTK_PROGRESS_BAR (gtk_builder_get_object ((*iterator).second.second,
                                                ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_PROGRESSBAR_NAVDATA)));
    ACE_ASSERT (progress_bar_p);
    iterator_4 = data_p->statistic.streamStatistic.find (ARDRONE_STREAM_NAVDATA);
    ACE_ASSERT (iterator_4 != data_p->statistic.streamStatistic.end ());
    fps = (*iterator_4).second.messagesPerSecond;
    speed = (*iterator_4).second.bytesPerSecond;
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
      //ACE_OS::memset (buffer, 0, sizeof (buffer));
      result = ACE_OS::sprintf (buffer, ACE_TEXT ("%.0f 1/s | %.2f %s"),
                                fps, speed, magnitude_string.c_str ());
      if (result < 0)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_OS::sprintf(): \"%m\", continuing\n")));
    } // end IF
    gtk_progress_bar_set_text (progress_bar_p,
                               ACE_TEXT_ALWAYS_CHAR (buffer));

    ACE_OS::memset (buffer, 0, sizeof (buffer));
    magnitude_string = ACE_TEXT_ALWAYS_CHAR ("byte(s)/s");

    progress_bar_p =
      GTK_PROGRESS_BAR (gtk_builder_get_object ((*iterator).second.second,
                                                ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_PROGRESSBAR_MAVLINK)));
    ACE_ASSERT (progress_bar_p);
    iterator_4 = data_p->statistic.streamStatistic.find (ARDRONE_STREAM_MAVLINK);
    ACE_ASSERT (iterator_4 != data_p->statistic.streamStatistic.end ());
    fps = (*iterator_4).second.messagesPerSecond;
    speed = (*iterator_4).second.bytesPerSecond;
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
      //ACE_OS::memset (buffer, 0, sizeof (buffer));
      result = ACE_OS::sprintf (buffer, ACE_TEXT ("%.0f 1/s | %.2f %s"),
                                fps, speed, magnitude_string.c_str ());
      if (result < 0)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_OS::sprintf(): \"%m\", continuing\n")));
    } // end IF
    gtk_progress_bar_set_text (progress_bar_p,
                               ACE_TEXT_ALWAYS_CHAR (buffer));

    ACE_OS::memset (buffer, 0, sizeof (buffer));
    magnitude_string = ACE_TEXT_ALWAYS_CHAR ("byte(s)/s");

    progress_bar_p =
      GTK_PROGRESS_BAR (gtk_builder_get_object ((*iterator).second.second,
                                                ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_PROGRESSBAR_VIDEO)));
    ACE_ASSERT (progress_bar_p);
    iterator_4 = data_p->statistic.streamStatistic.find (ARDRONE_STREAM_VIDEO);
    ACE_ASSERT (iterator_4 != data_p->statistic.streamStatistic.end ());
    fps = (*iterator_4).second.messagesPerSecond;
    speed = (*iterator_4).second.bytesPerSecond;
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
      //ACE_OS::memset (buffer, 0, sizeof (buffer));
      result = ACE_OS::sprintf (buffer, ACE_TEXT ("%.0f 1/s | %.2f %s"),
                                fps, speed, magnitude_string.c_str ());
      if (result < 0)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_OS::sprintf(): \"%m\", continuing\n")));
    } // end IF
    gtk_progress_bar_set_text (progress_bar_p,
                               ACE_TEXT_ALWAYS_CHAR (buffer));
  } // end lock scope

  return G_SOURCE_CONTINUE; // --> reschedule
}

/////////////////////////////////////////

#if defined (__cplusplus)
extern "C"
{
#endif /* __cplusplus */
void
toggleaction_connect_toggled_cb (GtkToggleAction* toggleAction_in,
                                 gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::toggleaction_connect_toggled_cb"));

  // --> user pressed connect/disconnect

  struct ARDrone_GtkCBData_Base* cb_data_base_p =
    static_cast<struct ARDrone_GtkCBData_Base*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (cb_data_base_p);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct ARDrone_DirectShow_GtkCBData* directshow_cb_data_p =
    NULL;
  struct ARDrone_DirectShow_Configuration* directshow_configuration_p =
    NULL;
  struct ARDrone_MediaFoundation_GtkCBData* mediafoundation_cb_data_p =
    NULL;
  struct ARDrone_MediaFoundation_Configuration* mediafoundation_configuration_p =
    NULL;
  switch (cb_data_base_p->mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      directshow_cb_data_p =
        static_cast<struct ARDrone_DirectShow_GtkCBData*> (cb_data_base_p);
      directshow_configuration_p = directshow_cb_data_p->configuration;
      ACE_ASSERT (directshow_configuration_p);
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      mediafoundation_cb_data_p =
        static_cast<struct ARDrone_MediaFoundation_GtkCBData*> (cb_data_base_p);
      mediafoundation_configuration_p =
        mediafoundation_cb_data_p->configuration;
      ACE_ASSERT (mediafoundation_configuration_p);
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), aborting\n"),
                  cb_data_base_p->mediaFramework));
      break;
    }
  } // end SWITCH
#else
  struct ARDrone_GtkCBData* cb_data_p =
    static_cast<struct ARDrone_GtkCBData*> (cb_data_base_p);
  struct ARDrone_Configuration* configuration_p = cb_data_p->configuration;
  ACE_ASSERT (configuration_p);
#endif // ACE_WIN32 || ACE_WIN64

  Common_UI_GTK_BuildersIterator_t iterator =
    cb_data_base_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != cb_data_base_p->builders.end ());

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
    GtkToggleAction* toggle_action_p =
        GTK_TOGGLE_ACTION (gtk_builder_get_object ((*iterator).second.second,
                                                   ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_TOGGLEACTION_CONNECT)));
    ACE_ASSERT (toggle_action_p);
    gtk_toggle_action_set_active (toggle_action_p, FALSE);
    g_signal_handlers_unblock_by_func (G_OBJECT (toggleAction_in),
                                       (gpointer)toggleaction_connect_toggled_cb,
                                       userData_in);

    return; // done
  } // end IF

  if (!gtk_toggle_action_get_active (toggleAction_in))
  {
    // stop streams
    Stream_IStreamControlBase* istream_base_p = NULL;
    ARDrone_StreamsIterator_t streams_iterator =
      cb_data_base_p->streams.find (control_stream_name_string_);
    ACE_ASSERT (streams_iterator != cb_data_base_p->streams.end ());
    istream_base_p =
        dynamic_cast<Stream_IStreamControlBase*> ((*streams_iterator).second);
    ACE_ASSERT (istream_base_p);
    istream_base_p->stop (false, false, true);
    streams_iterator =
      cb_data_base_p->streams.find (mavlink_stream_name_string_);
    ACE_ASSERT (streams_iterator != cb_data_base_p->streams.end ());
    istream_base_p =
        dynamic_cast<Stream_IStreamControlBase*> ((*streams_iterator).second);
    ACE_ASSERT (istream_base_p);
    istream_base_p->stop (false, false, true);
    streams_iterator =
      cb_data_base_p->streams.find (navdata_stream_name_string_);
    ACE_ASSERT (streams_iterator != cb_data_base_p->streams.end ());
    istream_base_p =
        dynamic_cast<Stream_IStreamControlBase*> ((*streams_iterator).second);
    ACE_ASSERT (istream_base_p);
    istream_base_p->stop (false, false, true);
    streams_iterator =
      cb_data_base_p->streams.find (video_stream_name_string_);
    ACE_ASSERT (streams_iterator != cb_data_base_p->streams.end ());
    istream_base_p =
        dynamic_cast<Stream_IStreamControlBase*> ((*streams_iterator).second);
    ACE_ASSERT (istream_base_p);
    istream_base_p->stop (false, false, true);

    return;
  } // end IF

//  GtkFileChooserButton* file_chooser_button_p = NULL;
//  char* URI_p = NULL;
//  GError* error_p = NULL;
//  gchar* hostname_p = NULL;
//  gchar* directory_p = NULL;
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
//  int number_of_screens = 0;
  GdkScreen* screen_p = NULL;
//  bool device_found = false;
  GValue value;
#if GTK_CHECK_VERSION (3,0,0)
  value = G_VALUE_INIT;
#if GTK_CHECK_VERSION (3,22,0)
  GdkMonitor* monitor_p = NULL;
#endif
#else
  g_value_init (&value, G_TYPE_NONE);
#endif
  GtkListStore* list_store_p = NULL;
  GtkDrawingArea* drawing_area_p = NULL;
  GtkCheckButton* check_button_p = NULL;
  GtkToggleAction* toggle_action_p = NULL;
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
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ARDrone_DirectShow_ConnectionConfigurationIterator_t directshow_iterator_3;
  ARDrone_DirectShow_Stream_ConnectionConfigurationIterator_t directshow_iterator_3_2;
  ARDrone_DirectShow_StreamConfigurationsIterator_t directshow_iterator_4;
  ARDrone_DirectShow_StreamConfiguration_t::ITERATOR_T directshow_iterator_5, directshow_iterator_6;
  ARDrone_MediaFoundation_ConnectionConfigurationIterator_t mediafoundation_iterator_3;
  ARDrone_MediaFoundation_Stream_ConnectionConfigurationIterator_t mediafoundation_iterator_3_2;
  ARDrone_MediaFoundation_StreamConfigurationsIterator_t mediafoundation_iterator_4;
  ARDrone_MediaFoundation_StreamConfiguration_t::ITERATOR_T mediafoundation_iterator_5, mediafoundation_iterator_6;
  switch (cb_data_base_p->mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      directshow_iterator_3 =
        directshow_configuration_p->connectionConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
      ACE_ASSERT (directshow_iterator_3 != directshow_configuration_p->connectionConfigurations.end ());
      directshow_iterator_3_2 =
        (*directshow_iterator_3).second.find (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING));
      //ARDrone_DirectShow_Stream_ConnectionConfigurationIterator_t directshow_iterator_3_2 ((*directshow_iterator_3).second,
      //                                                                                     0);
      //(*directshow_iterator_3).second.find (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING),
      //                                      directshow_iterator_3_2);
      ACE_ASSERT (directshow_iterator_3_2 != (*directshow_iterator_3).second.end ());
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      mediafoundation_iterator_3 =
        mediafoundation_configuration_p->connectionConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
      ACE_ASSERT (mediafoundation_iterator_3 != mediafoundation_configuration_p->connectionConfigurations.end ());
      mediafoundation_iterator_3_2 =
        (*mediafoundation_iterator_3).second.find (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING));
      ACE_ASSERT (mediafoundation_iterator_3_2 != (*mediafoundation_iterator_3).second.end ());
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  cb_data_base_p->mediaFramework));
      return;
    }
  } // end SWITCH
#else
  ARDrone_ConnectionConfigurationIterator_t iterator_3 =
    configuration_p->connectionConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
  ACE_ASSERT (iterator_3 != configuration_p->connectionConfigurations.end ());
  ARDrone_Stream_ConnectionConfigurationIterator_t iterator_3_2 =
    (*iterator_3).second.find (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING));
  ACE_ASSERT (iterator_3_2 != (*iterator_3).second.end ());
  ARDrone_StreamConfigurationsIterator_t iterator_4;
  ARDrone_StreamConfiguration_t::ITERATOR_T iterator_5, iterator_6;
#endif
  std::string address_string =
    ACE_TEXT_ALWAYS_CHAR (gtk_entry_get_text (entry_p));
  address_string += ACE_TEXT_ALWAYS_CHAR (':');
  std::ostringstream converter;
  converter <<
    static_cast<unsigned short> (gtk_spin_button_get_value_as_int (spin_button_p));
  address_string += converter.str ();
  bool is_fullscreen = false;
  Common_UI_DisplayDevices_t devices_a;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  switch (cb_data_base_p->mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      result =
        (*directshow_iterator_3_2).second.socketHandlerConfiguration.socketConfiguration_2.address.set (address_string.c_str (),
                                                                                                        AF_INET);
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      result =
        (*mediafoundation_iterator_3_2).second.socketHandlerConfiguration.socketConfiguration_2.address.set (address_string.c_str (),
                                                                                                             AF_INET);
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  cb_data_base_p->mediaFramework));
      goto error;
    }
  } // end SWITCH
#else
  result =
    (*iterator_3_2).second.socketHandlerConfiguration.socketConfiguration_2.address.set (address_string.c_str (),
                                                                                         AF_INET);
#endif
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_INET_Addr::set(%s): \"%m\", returning\n"),
                ACE_TEXT (address_string.c_str ())));
    goto error;
  } // end IF

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  switch (cb_data_base_p->mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      directshow_iterator_4 =
        directshow_configuration_p->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
      ACE_ASSERT (directshow_iterator_4 != directshow_configuration_p->streamConfigurations.end ());
      directshow_iterator_5 = (*directshow_iterator_4).second.find (ACE_TEXT_ALWAYS_CHAR (""));
      ACE_ASSERT (directshow_iterator_5 != (*directshow_iterator_4).second.end ());
      directshow_iterator_6 =
        (*directshow_iterator_4).second.find (ACE_TEXT_ALWAYS_CHAR (MODULE_DEC_DECODER_LIBAV_DECODER_DEFAULT_NAME_STRING));
      ACE_ASSERT (directshow_iterator_6 != (*directshow_iterator_4).second.end ());

      is_fullscreen = (*directshow_iterator_5).second.second.fullScreen;
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      mediafoundation_iterator_4 =
        mediafoundation_configuration_p->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
      ACE_ASSERT (mediafoundation_iterator_4 != mediafoundation_configuration_p->streamConfigurations.end ());
      mediafoundation_iterator_5 = (*mediafoundation_iterator_4).second.find (ACE_TEXT_ALWAYS_CHAR (""));
      ACE_ASSERT (mediafoundation_iterator_5 != (*mediafoundation_iterator_4).second.end ());
      mediafoundation_iterator_6 =
        (*mediafoundation_iterator_4).second.find (ACE_TEXT_ALWAYS_CHAR (MODULE_DEC_DECODER_LIBAV_DECODER_DEFAULT_NAME_STRING));
      ACE_ASSERT (mediafoundation_iterator_6 != (*mediafoundation_iterator_4).second.end ());

      is_fullscreen = (*mediafoundation_iterator_5).second.second.fullScreen;
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  cb_data_base_p->mediaFramework));
      goto error;
    }
  } // end SWITCH
#else
  iterator_4 =
      configuration_p->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
  ACE_ASSERT (iterator_4 != configuration_p->streamConfigurations.end ());
  iterator_5 = (*iterator_4).second.find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (iterator_5 != (*iterator_4).second.end ());
  iterator_6 =
      (*iterator_4).second.find (ACE_TEXT_ALWAYS_CHAR (MODULE_DEC_DECODER_LIBAV_DECODER_DEFAULT_NAME_STRING));
  ACE_ASSERT (iterator_6 != (*iterator_4).second.end ());
  is_fullscreen = (*iterator_5).second.second.fullScreen;
#endif
  // retrieve buffer
  spin_button_p =
      GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINBUTTON_BUFFERSIZE)));
  ACE_ASSERT (spin_button_p);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  switch (cb_data_base_p->mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      (*directshow_iterator_4).second.allocatorConfiguration_.defaultBufferSize =
        static_cast<unsigned int> (gtk_spin_button_get_value_as_int (spin_button_p));
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      (*mediafoundation_iterator_4).second.allocatorConfiguration_.defaultBufferSize =
        static_cast<unsigned int> (gtk_spin_button_get_value_as_int (spin_button_p));
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  cb_data_base_p->mediaFramework));
      goto error;
    }
  } // end SWITCH
#else
  (*iterator_4).second.allocatorConfiguration_.defaultBufferSize =
      static_cast<unsigned int> (gtk_spin_button_get_value_as_int (spin_button_p));
#endif
  // set fullscreen ?
  toggle_action_p =
      GTK_TOGGLE_ACTION (gtk_builder_get_object ((*iterator).second.second,
                                                 ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_TOGGLEACTION_FULLSCREEN)));
  ACE_ASSERT (toggle_action_p);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  switch (cb_data_base_p->mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      (*directshow_iterator_5).second.second.fullScreen =
        gtk_toggle_action_get_active (toggle_action_p);
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      (*mediafoundation_iterator_5).second.second.fullScreen =
        gtk_toggle_action_get_active (toggle_action_p);
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  cb_data_base_p->mediaFramework));
      goto error;
    }
  } // end SWITCH
#else
  (*iterator_5).second.second.fullScreen =
    gtk_toggle_action_get_active (toggle_action_p);
#endif
  // save streams ?
  check_button_p =
      GTK_CHECK_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_CHECKBUTTON_SAVE)));
  ACE_ASSERT (check_button_p);
  if (!gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (check_button_p)))
    goto continue_;

  // retrieve save format ?
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
#if GTK_CHECK_VERSION (3,0,0)
    GValue value = G_VALUE_INIT;
#else
    GValue value;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    g_value_init (&value, G_TYPE_STRING);
#else
    g_value_init (&value, G_TYPE_INT);
#endif
#endif
    gtk_tree_model_get_value (GTK_TREE_MODEL (list_store_p),
                              &iterator_2,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                              1, &value);
    ACE_ASSERT (G_VALUE_TYPE (&value) == G_TYPE_STRING);
    switch (cb_data_base_p->mediaFramework)
    {
      case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
      {
        (*directshow_iterator_5).second.second.inputFormat->subtype =
          Common_Tools::StringToGUID (g_value_get_string (&value));
        break;
  }
      case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
      {
        (*mediafoundation_iterator_5).second.second.inputFormat->subtype =
          Common_Tools::StringToGUID (g_value_get_string (&value));
        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                    cb_data_base_p->mediaFramework));
        goto error;
      }
    } // end SWITCH
#else
                              2, &value);
    ACE_ASSERT (G_VALUE_TYPE (&value) == G_TYPE_INT);
    (*iterator_5).second.second.inputFormat =
      static_cast<enum AVPixelFormat> (g_value_get_int (&value));
#endif
    g_value_unset (&value);
  } // end IF

continue_:
  // retrieve display settings ?
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct tagVIDEOINFOHEADER* video_info_header_p = NULL;
  struct tagVIDEOINFOHEADER* video_info_header_2 = NULL;
  switch (cb_data_base_p->mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      // sanity check(s)
      ACE_ASSERT ((*directshow_iterator_5).second.second.filterConfiguration);
      ACE_ASSERT ((*directshow_iterator_5).second.second.filterConfiguration->pinConfiguration);
      ACE_ASSERT ((*directshow_iterator_5).second.second.filterConfiguration->pinConfiguration->format);
      ACE_ASSERT ((*directshow_iterator_5).second.second.filterConfiguration->pinConfiguration->format->formattype == FORMAT_VideoInfo);
      ACE_ASSERT ((*directshow_iterator_5).second.second.filterConfiguration->pinConfiguration->format->cbFormat == sizeof (struct tagVIDEOINFOHEADER));
      video_info_header_p =
        reinterpret_cast<struct tagVIDEOINFOHEADER*> ((*directshow_iterator_5).second.second.filterConfiguration->pinConfiguration->format->pbFormat);

      ACE_ASSERT ((*directshow_iterator_5).second.second.inputFormat);
      ACE_ASSERT ((*directshow_iterator_5).second.second.inputFormat->formattype == FORMAT_VideoInfo);
      ACE_ASSERT ((*directshow_iterator_5).second.second.inputFormat->cbFormat == sizeof (struct tagVIDEOINFOHEADER));
      video_info_header_2 =
        reinterpret_cast<struct tagVIDEOINFOHEADER*> ((*directshow_iterator_5).second.second.inputFormat->pbFormat);
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      ACE_ASSERT (false); // *TODO*
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  cb_data_base_p->mediaFramework));
      goto error;
    }
  } // end SWITCH
#endif
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
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                            1, &value);
#else
                            0, &value);
#endif
  ACE_ASSERT (G_VALUE_TYPE (&value) == G_TYPE_STRING);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  switch (cb_data_base_p->mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      (*directshow_iterator_5).second.second.deviceIdentifier =
        g_value_get_string (&value);
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      (*mediafoundation_iterator_5).second.second.deviceIdentifier =
        g_value_get_string (&value);
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  cb_data_base_p->mediaFramework));
      goto error;
    }
  } // end SWITCH
#else
  (*iterator_5).second.second.displayDeviceIdentifier =
      g_value_get_string (&value);
#endif
  g_value_unset (&value);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (!Common_UI_Tools::getDisplayDevices (devices_a))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_UI_Tools::getDisplayDevices(), returning\n")));
    goto error;
  } // end IF
#else
  display_manager_p = gdk_display_manager_get ();
  ACE_ASSERT (display_manager_p);
  list_p = gdk_display_manager_list_displays (display_manager_p);
  ACE_ASSERT (list_p);
#if GTK_CHECK_VERSION (3,22,0)
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
                           (*iterator_5).second.second.displayDeviceIdentifier.c_str ()))
        break;
    } // end FOR
  } // end FOR
  if (!monitor_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("device not found (was: \"%s\"), returning\n"),
                ACE_TEXT ((*iterator_5).second.second.displayDeviceIdentifier.c_str ())));
    goto error;
  } // end IF
  display_p = gdk_monitor_get_display (monitor_p);
  ACE_ASSERT (display_p);
  screen_p = gdk_display_get_default_screen (display_p);
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
                             ACE_TEXT ((*iterator_5).second.second.device.c_str ())))
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
                ACE_TEXT ((*iterator_5).second.second.displayDeviceIdentifier.c_str ())));
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
#if GTK_CHECK_VERSION (3,0,0)
  value = G_VALUE_INIT;
#else
  g_value_init (&value, G_TYPE_INT);
#endif
  gtk_tree_model_get_value (GTK_TREE_MODEL (list_store_p),
                            &iterator_2,
                            1, &value);
  ACE_ASSERT (G_VALUE_TYPE (&value) == G_TYPE_INT);

  switch (static_cast<enum ARDrone_VideoMode> (g_value_get_int (&value)))
  {
    case ARDRONE_VIDEOMODE_360P:
    { // *TODO*: use ffmpeg to determine the format resolution
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      ACE_ASSERT (video_info_header_p);
      video_info_header_p->bmiHeader.biWidth = ARDRONE_H264_360P_VIDEO_WIDTH;
      video_info_header_p->bmiHeader.biHeight = -ARDRONE_H264_360P_VIDEO_HEIGHT;
      video_info_header_p->bmiHeader.biSizeImage =
        DIBSIZE (video_info_header_p->bmiHeader);
#else
      (*iterator_5).second.second.sourceFormat.height =
          ARDRONE_H264_360P_VIDEO_HEIGHT;
      (*iterator_5).second.second.sourceFormat.width =
          ARDRONE_H264_360P_VIDEO_WIDTH;
      (*iterator_6).second.second.sourceFormat.height =
          ARDRONE_H264_360P_VIDEO_HEIGHT;
      (*iterator_6).second.second.sourceFormat.width =
          ARDRONE_H264_360P_VIDEO_WIDTH;
#endif
      break;
    }
    case ARDRONE_VIDEOMODE_720P:
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      ACE_ASSERT (video_info_header_p);
      video_info_header_p->bmiHeader.biWidth = ARDRONE_H264_720P_VIDEO_WIDTH;
      video_info_header_p->bmiHeader.biHeight = -ARDRONE_H264_720P_VIDEO_HEIGHT;
      video_info_header_p->bmiHeader.biSizeImage =
        DIBSIZE (video_info_header_p->bmiHeader);
#else
      (*iterator_5).second.second.sourceFormat.height =
          ARDRONE_H264_720P_VIDEO_HEIGHT;
      (*iterator_5).second.second.sourceFormat.width =
          ARDRONE_H264_720P_VIDEO_WIDTH;
      (*iterator_6).second.second.sourceFormat.height =
          ARDRONE_H264_720P_VIDEO_HEIGHT;
      (*iterator_6).second.second.sourceFormat.width =
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

  drawing_area_p =
    GTK_DRAWING_AREA (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_DRAWINGAREA_VIDEO)));
  ACE_ASSERT (drawing_area_p);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  switch (cb_data_base_p->mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    { ACE_ASSERT (video_info_header_p);
      video_info_header_p->bmiHeader.biSizeImage =
        DIBSIZE (video_info_header_p->bmiHeader);
      video_info_header_p->dwBitRate =
        (video_info_header_p->bmiHeader.biWidth         *
         abs (video_info_header_p->bmiHeader.biHeight)) * 4 * 30 * 8;
      (*directshow_iterator_5).second.second.filterConfiguration->pinConfiguration->format->lSampleSize =
        video_info_header_p->bmiHeader.biSizeImage;
      directshow_configuration_p->filterConfiguration.allocatorProperties.cbBuffer =
        video_info_header_p->bmiHeader.biSizeImage;

      (*directshow_iterator_5).second.second.window =
        static_cast<HWND> (GDK_WINDOW_HWND (gtk_widget_get_window (GTK_WIDGET (drawing_area_p))));

      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      (*mediafoundation_iterator_5).second.second.window =
        static_cast<HWND> (GDK_WINDOW_HWND (gtk_widget_get_window (GTK_WIDGET (drawing_area_p))));
      ACE_ASSERT (false); // *TODO*
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  cb_data_base_p->mediaFramework));
      goto error;
    }
  } // end SWITCH
#endif

  //GtkWindow* window_p = NULL;
#if defined (GTK3_SUPPORT)
  struct _cairo_rectangle_int rectangle_s;
#else
  GtkAllocation rectangle_s;
#endif
  gtk_widget_get_allocation (GTK_WIDGET (drawing_area_p),
                             &rectangle_s);
  if (is_fullscreen)
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    switch (cb_data_base_p->mediaFramework)
    {
      case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
      { ACE_ASSERT (video_info_header_p);
        ACE_ASSERT (video_info_header_2);
        video_info_header_2->bmiHeader.biHeight =
          -((*directshow_iterator_5).second.second.area.bottom -
          (*directshow_iterator_5).second.second.area.top);
        video_info_header_2->bmiHeader.biWidth =
          ((*directshow_iterator_5).second.second.area.right -
          (*directshow_iterator_5).second.second.area.left);
        video_info_header_2->bmiHeader.biSizeImage =
          DIBSIZE (video_info_header_2->bmiHeader);

        unsigned int source_buffer_size =
          av_image_get_buffer_size (Stream_Module_Decoder_Tools::mediaSubTypeToAVPixelFormat ((*directshow_iterator_5).second.second.inputFormat->subtype),
                                    video_info_header_p->bmiHeader.biWidth,
                                    ::abs (video_info_header_p->bmiHeader.biHeight),
                                    1); // *TODO*: linesize alignment
        video_info_header_2->bmiHeader.biSizeImage =
          std::max (video_info_header_2->bmiHeader.biSizeImage,
                    static_cast<ULONG> (source_buffer_size));

        (*directshow_iterator_5).second.second.inputFormat->lSampleSize =
          video_info_header_2->bmiHeader.biSizeImage;
        directshow_configuration_p->filterConfiguration.allocatorProperties.cbBuffer =
          video_info_header_2->bmiHeader.biSizeImage;
        break;
      }
      case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
      {
        ACE_ASSERT (false); // *TODO*
        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                    cb_data_base_p->mediaFramework));
        goto error;
      }
    } // end SWITCH
#else
    gdk_screen_get_monitor_geometry (screen_p,
                                     monitor_number,
                                     &rectangle_s);
    GtkWindow* window_p =
        GTK_WINDOW (gtk_builder_get_object ((*iterator).second.second,
                                            ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_WINDOW_FULLSCREEN)));
    ACE_ASSERT (window_p);
//    gtk_widget_set_colormap (GTK_WIDGET ((*iterator_5).second.window),
//                             gdk_screen_get_rgba_colormap (screen_p));
    GdkVisual *visual_p = gdk_screen_get_rgba_visual (screen_p);
    ACE_ASSERT (visual_p);
    gtk_widget_set_visual (GTK_WIDGET (window_p),
                           visual_p);
    gtk_widget_set_size_request (GTK_WIDGET (window_p),
                                 rectangle_s.width, rectangle_s.height);
    gtk_window_set_screen (window_p,
                           screen_p);
    gtk_window_set_keep_above (window_p,
                               TRUE);
//    gtk_window_present (window_p);
    gtk_widget_show_all (GTK_WIDGET (window_p));
    gtk_window_move (window_p,
                     rectangle_s.x, rectangle_s.y);
    gtk_window_resize (window_p,
                       rectangle_s.width, rectangle_s.height);
    gtk_window_fullscreen (window_p);
//    gtk_window_fullscreen_on_monitor ();
//#if defined (_DEBUG)
//    gtk_window_set_interactive_debugging (TRUE);
//#endif
    (*iterator_5).second.second.window =
        gtk_widget_get_window (GTK_WIDGET (window_p));
//    gdk_window_set_override_redirect ((*iterator_5).second.window,
//                                      TRUE);
    drawing_area_p =
        GTK_DRAWING_AREA (gtk_builder_get_object ((*iterator).second.second,
                                                  ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_DRAWINGAREA_FULLSCREEN)));
    ACE_ASSERT (drawing_area_p);
#endif
  } // end IF
  else
  { // --> not fullscreen
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    HWND window_h;
#if defined (UNICODE)
    std::wstring device_name_string;
#else
    std::string device_name_string;
#endif
    switch (cb_data_base_p->mediaFramework)
    {
      case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
      { ACE_ASSERT (video_info_header_p);
        (*directshow_iterator_5).second.second.area.left = 0;
        (*directshow_iterator_5).second.second.area.right =
          video_info_header_p->bmiHeader.biWidth;
        (*directshow_iterator_5).second.second.area.top = 0;
        (*directshow_iterator_5).second.second.area.bottom =
          std::abs (video_info_header_p->bmiHeader.biHeight);

        window_h = (*directshow_iterator_5).second.second.window;
        device_name_string =
          (*directshow_iterator_5).second.second.deviceIdentifier;
        break;
      }
      case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
      {
        ACE_ASSERT (false); // *TODO*
        window_h = (*mediafoundation_iterator_5).second.second.window;
        device_name_string =
          (*mediafoundation_iterator_5).second.second.deviceIdentifier;
        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                    cb_data_base_p->mediaFramework));
        goto error;
      }
    } // end SWITCH
    // *NOTE*: if the chosen display device screen area contains the drawing
    //         area, use it[; otherwise open a new window]
    DWORD flags = MONITOR_DEFAULTTONULL;
    HMONITOR monitor_h = MonitorFromWindow (window_h,
                                            flags);
    if (!monitor_h)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to MonitorFromWindow(%@): \"%s\", returning\n"),
                  window_h,
                  ACE_TEXT (Common_Tools::errorToString (GetLastError ()).c_str ())));
      goto error;
    } // end IF
    MONITORINFOEX monitor_info_ex_s;
    ACE_OS::memset (&monitor_info_ex_s, 0, sizeof (MONITORINFOEX));
    monitor_info_ex_s.cbSize = sizeof (MONITORINFOEX);
    if (!GetMonitorInfo (monitor_h,
                         reinterpret_cast<struct tagMONITORINFO*> (&monitor_info_ex_s)))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to GetMonitorInfo(%@): \"%s\", returning\n"),
                  monitor_h,
                  ACE_TEXT (Common_Tools::errorToString (GetLastError ()).c_str ())));
      goto error;
    } // end IF
    if (ACE_OS::strcmp (device_name_string.c_str (),
                        monitor_info_ex_s.szDevice))
    { // --> the drawing area is NOT currently displayed on the selected monitor
      // *NOTE*: center the window on the display device
      //unsigned int delta_x =
      // abs (abs ((*iterator_5).second.area.left) -
      //           rectangle_s.width) / 2;
      //(*iterator_5).second.area.left =
      //  ((*iterator_5).second.area.left +
      //   delta_x);
      //(*iterator_5).second.area.right =
      //  ((*iterator_5).second.area.left +
      //   rectangle_s.width);
      //unsigned int delta_y =
      //  abs (abs ((*iterator_5).second.area.top) -
      //       rectangle_s.height) / 2;
      //(*iterator_5).second.area.top =
      //  ((*iterator_5).second.area.top +
      //   delta_y);
      //(*iterator_5).second.area.bottom =
      //  ((*iterator_5).second.area.top +
      //   rectangle_s.height);
    } // end IF
    else
    { // --> the drawing area is currently displayed on the selected monitor
      switch (cb_data_base_p->mediaFramework)
      {
        case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
        {
          (*directshow_iterator_5).second.second.area.bottom =
            rectangle_s.y + rectangle_s.height;
          (*directshow_iterator_5).second.second.area.left = rectangle_s.x;
          (*directshow_iterator_5).second.second.area.right =
            rectangle_s.x + rectangle_s.width;
          (*directshow_iterator_5).second.second.area.top = rectangle_s.y;
          break;
        }
        case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
        {
          (*mediafoundation_iterator_5).second.second.area.bottom =
            rectangle_s.y + rectangle_s.height;
          (*mediafoundation_iterator_5).second.second.area.left = rectangle_s.x;
          (*mediafoundation_iterator_5).second.second.area.right =
            rectangle_s.x + rectangle_s.width;
          (*mediafoundation_iterator_5).second.second.area.top = rectangle_s.y;
          break;
        }
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                      cb_data_base_p->mediaFramework));
          goto error;
        }
      } // end SWITCH
    } // end ELSE

    switch (cb_data_base_p->mediaFramework)
    {
      case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
      { ACE_ASSERT (video_info_header_2);
        video_info_header_2->bmiHeader.biHeight = rectangle_s.height;
        video_info_header_2->bmiHeader.biWidth = rectangle_s.width;
        video_info_header_2->bmiHeader.biSizeImage =
          DIBSIZE (video_info_header_2->bmiHeader);
        video_info_header_2->dwBitRate =
          (video_info_header_2->bmiHeader.biWidth * std::abs (video_info_header_2->bmiHeader.biHeight)) * 4 * 30 * 8;
        (*directshow_iterator_5).second.second.inputFormat->lSampleSize =
          video_info_header_2->bmiHeader.biSizeImage;
        directshow_configuration_p->filterConfiguration.allocatorProperties.cbBuffer =
          std::max (directshow_configuration_p->filterConfiguration.allocatorProperties.cbBuffer,
                    static_cast<long> (video_info_header_2->bmiHeader.biSizeImage));

        // *TODO*: for some reason DirectShow fails to paint into the drawing area
        //         --> open a dedicated window
        //(*iterator_5).second.window = NULL;
        break;
      }
      case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
      {
        ACE_ASSERT (false); // *TODO*
        break;
      }
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                    cb_data_base_p->mediaFramework));
        goto error;
      }
    } // end SWITCH
#else
    (*iterator_5).second.second.area = rectangle_s;
#endif
  } // end ELSE
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  switch (cb_data_base_p->mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("using display device \"%s\" [lrtb: %d/%d/%d/%d]: %dx%d\n"),
                  ACE_TEXT ((*directshow_iterator_5).second.second.deviceIdentifier.c_str ()),
                  (*directshow_iterator_5).second.second.area.left,
                  (*directshow_iterator_5).second.second.area.right,
                  (*directshow_iterator_5).second.second.area.top,
                  (*directshow_iterator_5).second.second.area.bottom,
                  ((*directshow_iterator_5).second.second.area.right -
                  (*directshow_iterator_5).second.second.area.left),
                   ((*directshow_iterator_5).second.second.area.bottom -
                  (*directshow_iterator_5).second.second.area.top)));
      ACE_ASSERT ((*directshow_iterator_5).second.second.window);
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("using display device \"%s\" [lrtb: %d/%d/%d/%d]: %dx%d\n"),
                  ACE_TEXT ((*mediafoundation_iterator_5).second.second.deviceIdentifier.c_str ()),
                  (*mediafoundation_iterator_5).second.second.area.left,
                  (*mediafoundation_iterator_5).second.second.area.right,
                  (*mediafoundation_iterator_5).second.second.area.top,
                  (*mediafoundation_iterator_5).second.second.area.bottom,
                  ((*mediafoundation_iterator_5).second.second.area.right -
                  (*mediafoundation_iterator_5).second.second.area.left),
                   ((*mediafoundation_iterator_5).second.second.area.bottom -
                  (*mediafoundation_iterator_5).second.second.area.top)));
      ACE_ASSERT ((*mediafoundation_iterator_5).second.second.window);
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  cb_data_base_p->mediaFramework));
      goto error;
    }
  } // end SWITCH
#else
  (*iterator_5).second.second.area = rectangle_s;
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("using display device \"%s\" (display: \"%s\", monitor: %d) [%d/%d/%d/%d]: %dx%d\n"),
              ACE_TEXT ((*iterator_5).second.second.displayDeviceIdentifier.c_str ()),
              ACE_TEXT (gdk_display_get_name (display_p)),
              monitor_number,
              (*iterator_5).second.second.area.x,
              (*iterator_5).second.second.area.y,
              (*iterator_5).second.second.area.width,
              (*iterator_5).second.second.area.height,
              (*iterator_5).second.second.area.width,
              (*iterator_5).second.second.area.height));
  ACE_ASSERT ((*iterator_5).second.second.window);
#endif

  // *NOTE*: the surface / pixel buffer haven't been created yet, as the window
  //         wasn't 'viewable' during the first 'configure' event
  //         --> create it now
#if GTK_CHECK_VERSION (3,0,0)
  g_signal_emit_by_name (G_OBJECT (drawing_area_p),
                         ACE_TEXT_ALWAYS_CHAR ("size-allocate"),
                         &rectangle_s,
                         userData_in,
                         &result);
#else
  GdkEventConfigure event_s;
  event_s.type = GDK_CONFIGURE;
  event_s.window = (*iterator_5).second.window;
  event_s.send_event = TRUE;
  event_s.x = rectangle_s.x; event_s.y = rectangle_s.y;
  event_s.width = rectangle_s.width; event_s.height = rectangle_s.height;
  g_signal_emit_by_name (G_OBJECT (drawing_area_p),
                         ACE_TEXT_ALWAYS_CHAR ("configure-event"),
                         &event_s,
                         userData_in,
                         &result);
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
                                                ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_PROGRESSBAR)));
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
  thread_data_p->CBData = cb_data_base_p;

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
      thread_manager_p->spawn (::stream_processing_function,       // function
                               thread_data_p,                      // argument
                               (THR_NEW_LWP      |
                                THR_JOINABLE     |
                                THR_INHERIT_SCHED),                // flags
                               &thread_id,                         // id
                               &thread_handle,                     // handle
                               ACE_DEFAULT_THREAD_PRIORITY,        // priority
                               COMMON_APPLICATION_THREAD_GROUP_ID, // group id
                               NULL,                               // stack
                               0,                                  // stack size
                               &thread_name_p);                    // name
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Thread_Manager::spawn(): \"%m\", returning\n")));
    goto error;
  } // end IF
  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("spawned processing thread (id: %u)\n"),
  //            thread_id));

  // start progress reporting
  // *TODO*: there is a race condition here if the processing thread returns
  //         early
  ACE_ASSERT (!cb_data_base_p->progressData.eventSourceId);
  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, cb_data_base_p->lock);
    cb_data_base_p->progressData.eventSourceId =
        //g_idle_add_full (G_PRIORITY_DEFAULT_IDLE, // _LOW doesn't work (on Win32)
        //                 idle_update_progress_cb,
        //                 &data_p->progressData,
        //                 NULL);
        g_timeout_add_full (G_PRIORITY_DEFAULT_IDLE,                            // _LOW doesn't work (on Win32)
                            COMMON_UI_GTK_INTERVAL_DEFAULT_PROGRESSBAR_REFRESH, // ms (?)
                            idle_update_progress_cb,
                            &cb_data_base_p->progressData,
                            NULL);
    if (cb_data_base_p->progressData.eventSourceId > 0)
    {
      thread_data_p->eventSourceId = cb_data_base_p->progressData.eventSourceId;
      cb_data_base_p->progressData.pendingActions[cb_data_base_p->progressData.eventSourceId] =
          ACE_Thread_ID (thread_id, thread_handle);
      cb_data_base_p->eventSourceIds.insert (cb_data_base_p->progressData.eventSourceId);
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
                                                ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_PROGRESSBAR)));
  ACE_ASSERT (progress_bar_p);
  gtk_widget_set_sensitive (GTK_WIDGET (progress_bar_p), false);

  if (stop_progress_reporting)
  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, cb_data_base_p->lock);
    cb_data_base_p->progressData.completedActions.insert (cb_data_base_p->progressData.eventSourceId);
  } // end IF

  if (thread_data_p)
    delete thread_data_p;

  un_toggling_connect = true;
  gtk_action_activate (GTK_ACTION (toggleAction_in));
}

void
action_calibrate_activate_cb (GtkAction* action_in,
                              gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::action_calibrate_activate_cb"));

  struct ARDrone_GtkCBData_Base* cb_data_p =
      static_cast<struct ARDrone_GtkCBData_Base*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (cb_data_p);
  ACE_ASSERT (cb_data_p->controller);

  try {
    cb_data_p->controller->calibrate ();
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in ARDrone_IController::calibrate(), continuing\n")));
  }
}

void
action_dump_activate_cb (GtkAction* action_in,
                         gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::action_dump_activate_cb"));

  struct ARDrone_GtkCBData_Base* cb_data_p =
      static_cast<struct ARDrone_GtkCBData_Base*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (cb_data_p);
  ACE_ASSERT (cb_data_p->controller);

  try {
    cb_data_p->controller->dump ();
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in ARDrone_IController::dump(), continuing\n")));
  }
}

void
combobox_wlan_interface_changed_cb (GtkComboBox* comboBox_in,
                                    gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::combobox_wlan_interface_changed_cb"));

  struct ARDrone_GtkCBData_Base* cb_data_base_p =
      static_cast<struct ARDrone_GtkCBData_Base*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (cb_data_base_p);
  struct ARDrone_WLANMonitorConfiguration* wlan_monitor_configuration_p = NULL;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct ARDrone_DirectShow_GtkCBData* directshow_cb_data_p =
    NULL;
  struct ARDrone_DirectShow_Configuration* directshow_configuration_p =
    NULL;
  struct ARDrone_MediaFoundation_GtkCBData* mediafoundation_cb_data_p =
    NULL;
  struct ARDrone_MediaFoundation_Configuration* mediafoundation_configuration_p =
    NULL;
  switch (cb_data_base_p->mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      directshow_cb_data_p =
        static_cast<struct ARDrone_DirectShow_GtkCBData*> (cb_data_base_p);
      directshow_configuration_p = directshow_cb_data_p->configuration;
      ACE_ASSERT (directshow_configuration_p);
      wlan_monitor_configuration_p =
        &directshow_configuration_p->WLANMonitorConfiguration;
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      mediafoundation_cb_data_p =
        static_cast<struct ARDrone_MediaFoundation_GtkCBData*> (cb_data_base_p);
      mediafoundation_configuration_p =
        mediafoundation_cb_data_p->configuration;
      ACE_ASSERT (mediafoundation_configuration_p);
      wlan_monitor_configuration_p =
        &mediafoundation_configuration_p->WLANMonitorConfiguration;
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), aborting\n"),
                  cb_data_base_p->mediaFramework));
      break;
    }
  } // end SWITCH
#else
  struct ARDrone_GtkCBData* cb_data_p =
    static_cast<struct ARDrone_GtkCBData*> (cb_data_base_p);
  struct ARDrone_Configuration* configuration_p = cb_data_p->configuration;
  ACE_ASSERT (configuration_p);
  wlan_monitor_configuration_p = &configuration_p->WLANMonitorConfiguration;
#endif // ACE_WIN32 || ACE_WIN64
  ACE_ASSERT (wlan_monitor_configuration_p);

  Common_UI_GTK_BuildersIterator_t iterator =
    cb_data_base_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != cb_data_base_p->builders.end ());

  GtkTreeIter iterator_2;
  if (!gtk_combo_box_get_active_iter (comboBox_in,
                                      &iterator_2))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_combo_box_get_active_iter(), returning\n")));
    return;
  } // end IF
  GtkListStore* list_store_p =
      GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                              ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_LISTSTORE_WLAN_INTERFACE)));
  ACE_ASSERT (list_store_p);
  GValue value;
#if GTK_CHECK_VERSION (3,0,0)
  value = G_VALUE_INIT;
#else
  g_value_init (&value, G_TYPE_STRING);
#endif
  gtk_tree_model_get_value (GTK_TREE_MODEL (list_store_p),
                            &iterator_2,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                            1, &value);
#else
                            0, &value);
#endif
  ACE_ASSERT (G_VALUE_TYPE (&value) == G_TYPE_STRING);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  wlan_monitor_configuration_p->interfaceIdentifier =
    Common_Tools::StringToGUID (ACE_TEXT_ALWAYS_CHAR (g_value_get_string (&value)));
#else
  wlan_monitor_configuration_p->interfaceIdentifier =
    ACE_TEXT_ALWAYS_CHAR (g_value_get_string (&value));
#endif // ACE_WIN32 || ACE_WIN64
  g_value_unset (&value);

  ARDrone_WLANMonitor_t* WLAN_monitor_p =
    ARDRONE_WLANMONITOR_SINGLETON::instance ();
  ACE_ASSERT (WLAN_monitor_p);
  std::string SSID_string =
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#if defined (WLANAPI_USE)
    Net_WLAN_Tools::associatedSSID (WLAN_monitor_p->get (),
                                    wlan_monitor_configuration_p->interfaceIdentifier);
#else
    ACE_TEXT_ALWAYS_CHAR ("");
  ACE_ASSERT (false);
  ACE_NOTSUP;

  ACE_NOTREACHED (return;)
#endif // WLANAPI_USE
#elif defined (ACE_LINUX)
#if defined (WEXT_USE)
    Net_WLAN_Tools::associatedSSID (wlan_monitor_configuration_p->interfaceIdentifier,
                                    ACE_INVALID_HANDLE);
#elif defined (NL80211_USE)
    Net_WLAN_Tools::associatedSSID (wlan_monitor_configuration_p->interfaceIdentifier,
                                    NULL,
                                    WLAN_monitor_p->get_3 ());
#elif defined (DBUS_USE)
    Net_WLAN_Tools::associatedSSID (WLAN_monitor_p->getP (),
                                    wlan_monitor_configuration_p->interfaceIdentifier);
#else
    ACE_TEXT_ALWAYS_CHAR ("");
  ACE_ASSERT (false);
  ACE_NOTSUP;

  ACE_NOTREACHED (return;)
#endif // WEXT_USE
#endif // ACE_WIN32 || ACE_WIN64
  if ((!wlan_monitor_configuration_p->SSID.empty () &&
       ACE_OS::strcmp (wlan_monitor_configuration_p->SSID.c_str (),
                       SSID_string.c_str ())) &&
      wlan_monitor_configuration_p->autoAssociate)
  {
    GtkSpinner* spinner_p =
      GTK_SPINNER (gtk_builder_get_object ((*iterator).second.second,
                                           ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINNER)));
    ACE_ASSERT (spinner_p);
    gtk_widget_set_sensitive (GTK_WIDGET (spinner_p),
                              TRUE);
    gtk_spinner_start (spinner_p);
  } // end IF
  else
  {
    GtkEntry* entry_p =
      GTK_ENTRY (gtk_builder_get_object ((*iterator).second.second,
                                         ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_ENTRY_ADDRESS)));
    ACE_ASSERT (entry_p);
    ACE_INET_Addr interface_address, gateway_address;
    if (!Net_Common_Tools::interfaceToIPAddress (wlan_monitor_configuration_p->interfaceIdentifier,
                                                 interface_address,
                                                 gateway_address))
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Net_Common_Tools::interfaceToIPAddress(\"%s\"), returning\n"),
                  ACE_TEXT (Net_Common_Tools::interfaceToString (wlan_monitor_configuration_p->interfaceIdentifier).c_str ())));
#else
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Net_Common_Tools::interfaceToIPAddress(\"%s\"), returning\n"),
                  ACE_TEXT (wlan_monitor_configuration_p->interfaceIdentifier.c_str ())));
#endif // ACE_WIN32 || ACE_WIN64
      return;
    } // end IF
    if (gateway_address.is_any ())
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("\"%s\" does not currently have any gateway address, returning\n"),
                  ACE_TEXT (Net_Common_Tools::interfaceToString (wlan_monitor_configuration_p->interfaceIdentifier).c_str ())));
#else
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("\"%s\" does not currently have any gateway address, returning\n"),
                  ACE_TEXT (wlan_monitor_configuration_p->interfaceIdentifier.c_str ())));
#endif // ACE_WIN32 || ACE_WIN64
      return;
    } // end IF
    gtk_entry_set_text (entry_p,
                        ACE_TEXT_ALWAYS_CHAR (Net_Common_Tools::IPAddressToString (gateway_address, true).c_str ()));

    GtkToggleAction* toggle_action_p =
      GTK_TOGGLE_ACTION (gtk_builder_get_object ((*iterator).second.second,
                                                 ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_TOGGLEACTION_CONNECT)));
    ACE_ASSERT (toggle_action_p);
    gtk_action_set_sensitive (GTK_ACTION (toggle_action_p),
                              TRUE);
  } // end ELSE
}

void
combobox_display_device_changed_cb (GtkComboBox* comboBox_in,
                                    gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::combobox_display_device_changed_cb"));

  struct ARDrone_GtkCBData_Base* cb_data_p =
      static_cast<struct ARDrone_GtkCBData_Base*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (cb_data_p);

  Common_UI_GTK_BuildersIterator_t iterator =
    cb_data_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != cb_data_p->builders.end ());

  GtkComboBox* combo_box_p =
    GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                           ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_COMBOBOX_DISPLAY_DEVICE)));
  ACE_ASSERT (combo_box_p);
  GtkListStore* list_store_p =
    GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                            ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_LISTSTORE_DISPLAY_DEVICE)));
  ACE_ASSERT (list_store_p);
  gint n_rows =
    gtk_tree_model_iter_n_children (GTK_TREE_MODEL (list_store_p), NULL);
  combo_box_p =
    GTK_COMBO_BOX (gtk_builder_get_object ((*iterator).second.second,
                                           ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_COMBOBOX_DISPLAY_FORMAT)));
  ACE_ASSERT (combo_box_p);
  gtk_widget_set_sensitive (GTK_WIDGET (combo_box_p), (n_rows > 0));
  if (n_rows > 0)
    gtk_combo_box_set_active (combo_box_p, 0);
}

void
combobox_display_format_changed_cb (GtkComboBox* comboBox_in,
                                    gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::combobox_display_format_changed_cb"));

  struct ARDrone_GtkCBData_Base* cb_data_p =
      static_cast<struct ARDrone_GtkCBData_Base*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (cb_data_p);
  //ACE_ASSERT (cb_data_p->controller);

  Common_UI_GTK_BuildersIterator_t iterator =
    cb_data_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != cb_data_p->builders.end ());

  GtkTreeIter iterator_2;
  if (!gtk_combo_box_get_active_iter (comboBox_in,
                                      &iterator_2))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_combo_box_get_active_iter(), returning\n")));
    return;
  } // end IF
  GtkListStore* list_store_p =
    GTK_LIST_STORE (gtk_builder_get_object ((*iterator).second.second,
                                            ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_LISTSTORE_DISPLAY_FORMAT)));
  ACE_ASSERT (list_store_p);
  GValue value;
#if GTK_CHECK_VERSION (3,0,0)
  value = G_VALUE_INIT;
#else
  g_value_init (&value, G_TYPE_INT);
#endif
  gtk_tree_model_get_value (GTK_TREE_MODEL (list_store_p),
                            &iterator_2,
                            1, &value);
  ACE_ASSERT (G_VALUE_TYPE (&value) == G_TYPE_INT);
  cb_data_p->videoMode =
    static_cast<enum ARDrone_VideoMode> (g_value_get_int (&value));

  if (cb_data_p->controller)
  {
    try {
      cb_data_p->controller->set (cb_data_p->videoMode);
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in ARDrone_IController::set(%d), returning\n"),
                  cb_data_p->videoMode));
      return;
    }
  } // end IF
}

void
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
void
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
void
places_save_mount_cb (GtkPlacesSidebar* placesSidebar_in,
                      GMountOperation* mountOperation_in,
                      gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::places_save_mount_cb"));

//  struct ARDrone_GtkCBData_Base* cb_data_p =
//      static_cast<struct ARDrone_GtkCBData_Base*> (userData_in);

}
#endif

// ---------------------------------------
#if defined (GTKGL_SUPPORT)
void
glarea_realize_cb (GtkWidget* widget_in,
                   gpointer   userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::glarea_realize_cb"));

  // sanity check(s)
  ACE_ASSERT (widget_in);
  ACE_ASSERT (userData_in);

  struct ARDrone_GtkCBData_Base* data_p = NULL;
  GLuint* model_list_id_p = NULL;
  GtkAllocation allocation;

  GtkGLArea* gl_area_p = GTK_GL_AREA (widget_in);
  ACE_ASSERT (gl_area_p);
  // NOTE*: the OpenGL context has been created at this point
  GdkGLContext* context_p = gtk_gl_area_get_context (gl_area_p);
  if (unlikely (!context_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_gl_area_get_context(%@), returning\n"),
                gl_area_p));
    goto error;
  } // end IF
  data_p = static_cast<struct ARDrone_GtkCBData_Base*> (userData_in);
  ACE_ASSERT (data_p);
  //ACE_ASSERT (data_p->configuration);

  // load the texture
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//  //struct ARDrone_DirectShow_GtkCBData* directshow_data_p = NULL;
//  //struct ARDrone_MediaFoundation_GtkCBData* mediafoundation_data_p =
//  //  NULL;
//  ARDrone_DirectShow_StreamConfigurationsIterator_t directshow_streams_iterator;
//  ARDrone_MediaFoundation_StreamConfigurationsIterator_t mediafoundation_streams_iterator;
//  ARDrone_DirectShow_StreamConfiguration_t::ITERATOR_T directshow_modulehandler_configuration_iterator;
//  ARDrone_MediaFoundation_StreamConfiguration_t::ITERATOR_T mediafoundation_modulehandler_configuration_iterator;
//  switch (data_p->mediaFramework)
//  {
//    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
//    {
//      //directshow_data_p =
//      //  static_cast<struct ARDrone_DirectShow_GtkCBData*> (userData_in);
//      //// sanity check(s)
//      //ACE_ASSERT (directshow_data_p);
//      //ACE_ASSERT (directshow_data_p->configuration);
//
//      directshow_streams_iterator =
//        data_p->configuration->directShowStreamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_NAVDATA_STREAM_NAME_STRING));
//      ACE_ASSERT (directshow_streams_iterator != data_p->configuration->directShowStreamConfigurations.end ());
//      directshow_modulehandler_configuration_iterator =
//        //directshow_data_p->configuration->streamConfiguration_.find (ACE_TEXT_ALWAYS_CHAR (""));
//        (*directshow_streams_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (""));
//        //ACE_ASSERT (directshow_modulehandler_configuration_iterator != directshow_data_p->configuration->directShowStreamConfigurations.end ());
//        ACE_ASSERT (directshow_modulehandler_configuration_iterator != (*directshow_streams_iterator).second.end ());
//      break;
//    }
//    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
//    {
//      //mediafoundation_data_p =
//      //  static_cast<struct ARDrone_MediaFoundation_GtkCBData*> (userData_in);
//      //// sanity check(s)
//      //ACE_ASSERT (mediafoundation_data_p);
//      //ACE_ASSERT (mediafoundation_data_p->configuration);
//
//      mediafoundation_streams_iterator =
//        data_p->configuration->mediaFoundationStreamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_NAVDATA_STREAM_NAME_STRING));
//      ACE_ASSERT (mediafoundation_streams_iterator != data_p->configuration->mediaFoundationStreamConfigurations.end ());
//      mediafoundation_modulehandler_configuration_iterator =
//        //mediafoundation_data_p->configuration->streamConfiguration_.find (ACE_TEXT_ALWAYS_CHAR (""));
//        (*mediafoundation_streams_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (""));
//      //ACE_ASSERT (mediafoundation_modulehandler_configuration_iterator != mediafoundation_data_p->configuration->directShowStreamConfigurations.end ());
//      ACE_ASSERT (mediafoundation_modulehandler_configuration_iterator != (*mediafoundation_streams_iterator).second.end ());
//      break;
//    }
//    default:
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
//                  data_p->mediaFramework));
//      goto error;
//    }
//  } // end SWITCH
//#else
//  ARDrone_StreamConfigurationsIterator_t streams_iterator =
//      data_p->configuration->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_NAVDATA_STREAM_NAME_STRING));
//  ACE_ASSERT (streams_iterator != data_p->configuration->streamConfigurations.end ());
//  ARDrone_StreamConfiguration_t::ITERATOR_T modulehandler_configuration_iterator =
//    (*streams_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (""));
//  ACE_ASSERT (modulehandler_configuration_iterator != (*streams_iterator).second.end ());
//#endif

  gtk_gl_area_attach_buffers (gl_area_p);
  gdk_gl_context_make_current (context_p);

//#if GTK_CHECK_VERSION (3,0,0)
//#if GTK_CHECK_VERSION (3,16,0)
//  gtk_gl_area_make_current (gl_area_p);
//#else
//  ggla_area_make_current (gl_area_p);
//#endif
//#else
//#if defined (GTKGLAREA_SUPPORT)
//  gtk_gl_area_make_current (GTK_GL_AREA (gl_area_p));
//#else
//  gdk_gl_drawable_make_current (drawable_p,
//                                context_p);
//#endif
//#endif

  // sanity check(s)
  ACE_ASSERT (gtk_gl_area_get_has_depth_buffer (gl_area_p));

  // load model mesh
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//  switch (data_p->mediaFramework)
//  {
//    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
//      //model_list_id_p = &directshow_data_p->openGLModelListId;
//      break;
//    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
//      //model_list_id_p = &mediafoundation_data_p->openGLModelListId;
//      break;
//    default:
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
//                  data_p->mediaFramework));
//      goto error;
//    }
//  } // end SWITCH
//#else
  model_list_id_p = &data_p->openGLModelListId;
//#endif
  ACE_ASSERT (model_list_id_p);
  if (*model_list_id_p > 0)
  {
    glDeleteLists (*model_list_id_p, 1);
//    glDeleteTextures (1, model_list_id_p);
    COMMON_GL_ASSERT;
    *model_list_id_p = 0;
  } // end IF

  if (!*model_list_id_p)
  {
    std::string filename = Common_File_Tools::getWorkingDirectory ();
    filename += ACE_DIRECTORY_SEPARATOR_CHAR;
    filename += ACE_TEXT_ALWAYS_CHAR (ARDRONE_CONFIGURATION_DIRECTORY);
    filename += ACE_DIRECTORY_SEPARATOR_CHAR;
    filename +=
      ACE_TEXT_ALWAYS_CHAR (ARDRONE_OPENGL_MODEL_DEFAULT_FILE);
//        ACE_TEXT_ALWAYS_CHAR (ARDRONE_OPENGL_TEXTURE_DEFAULT_FILE);
    *model_list_id_p =
        Common_GL_Tools::loadModel (filename,
                                    data_p->openGLScene.boundingBox,
                                    data_p->openGLScene.center);
//        Common_GL_Tools::loadTexture (filename);
    if (!*model_list_id_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_GL_Tools::loadModel(\"%s\"), aborting\n"),
                  ACE_TEXT (filename.c_str ())));
      goto error;
    } // end IF
//    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    COMMON_GL_ASSERT;
//    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    COMMON_GL_ASSERT;

    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("OpenGL model list id: %u\n"),
                *model_list_id_p));
  } // end IF

  // initialize perspective
  gtk_widget_get_allocation (widget_in,
                             &allocation);

  glMatrixMode (GL_PROJECTION);
  COMMON_GL_ASSERT;

  // reset the projection matrix
  glLoadIdentity ();
  COMMON_GL_ASSERT;

  gluPerspective (ARDRONE_OPENGL_PERSPECTIVE_FOVY,
                  static_cast<GLdouble> (allocation.width) / static_cast<GLdouble> (allocation.height),
                  ARDRONE_OPENGL_PERSPECTIVE_ZNEAR,
                  ARDRONE_OPENGL_PERSPECTIVE_ZFAR);
  COMMON_GL_ASSERT;

  glViewport (0, 0,
              static_cast<GLsizei> (allocation.width), static_cast<GLsizei> (allocation.height));
  COMMON_GL_ASSERT;

  glMatrixMode (GL_MODELVIEW);
  COMMON_GL_ASSERT;

  // reset the projection matrix
  glLoadIdentity ();
  COMMON_GL_ASSERT;

  return;

error:
  return;
} // glarea_realize_cb
#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
GdkGLContext*
glarea_create_context_cb (GtkGLArea* GLArea_in,
                          gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::glarea_create_context_cb"));

  // sanity check(s)
  ACE_ASSERT (GLArea_in);
  ACE_ASSERT (userData_in);
  ACE_ASSERT (!gtk_gl_area_get_context (GLArea_in));

  GdkGLContext* result_p = NULL;

  GdkWindow* window_p = gtk_widget_get_window (GTK_WIDGET (GLArea_in));

  // sanity check(s)
  ACE_ASSERT (window_p);

  GError* error_p = NULL;
  // *TODO*: this currently fails on Wayland (Gnome >= 3.22.24; Intel Ironlake
  //         HD Graphics (ThinkPad T410))
  // *WORKAROUND*: set GDK_BACKEND=x11 environment to force XWayland
  result_p = gdk_window_create_gl_context (window_p,
                                           &error_p);
  if (!result_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gdk_window_create_gl_context(): \"%s\", aborting\n"),
                ACE_TEXT (error_p->message)));

    gtk_gl_area_set_error (GLArea_in, error_p);
    g_error_free (error_p);

    return NULL;
  } // end IF

  gdk_gl_context_set_required_version (result_p,
                                       2, 1);
#if defined (_DEBUG)
  gdk_gl_context_set_debug_enabled (result_p,
                                    TRUE);
#endif
  //gdk_gl_context_set_forward_compatible (result_p,
  //                                       FALSE);
  gdk_gl_context_set_use_es (result_p,
                             -1); // auto-detect

  if (!gdk_gl_context_realize (result_p,
                               &error_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to realize OpenGL context: \"%s\", continuing\n"),
                ACE_TEXT (error_p->message)));

    gtk_gl_area_set_error (GLArea_in, error_p);
    g_error_free (error_p);

    return NULL;
  } // end IF

  gdk_gl_context_make_current (result_p);

  // initialize options
  glClearColor (0.0f, 0.0f, 0.0f, 1.0f);              // Black Background
//  glClearColor (0.1f, 0.1f, 0.1f, 1.0f);              // Black Background
  COMMON_GL_ASSERT;
  //glClearDepth (1.0);                                 // Depth Buffer Setup
  //COMMON_GL_ASSERT;
  /* speedups */
  //  glDisable (GL_CULL_FACE);
  //  glEnable (GL_DITHER);
//  COMMON_GL_ASSERT;
//  glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
  glColorMaterial (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  COMMON_GL_ASSERT;
  glEnable (GL_COLOR_MATERIAL);
  COMMON_GL_ASSERT;
  glEnable (GL_LIGHTING);
  COMMON_GL_ASSERT;
  glEnable (GL_LIGHT0);    /* Uses default lighting parameters */
  COMMON_GL_ASSERT;
  glLightModeli (GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
  COMMON_GL_ASSERT;
  //  glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
  glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Really Nice Perspective
  COMMON_GL_ASSERT;
  glDepthFunc (GL_LESS);                              // The Type Of Depth Testing To Do
  COMMON_GL_ASSERT;
  glDepthMask (GL_TRUE);
  COMMON_GL_ASSERT;
//  glEnable (GL_TEXTURE_2D);                           // Enable Texture Mapping
//  COMMON_GL_ASSERT;
//  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//  COMMON_GL_ASSERT;
//  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//  COMMON_GL_ASSERT;
  glShadeModel (GL_SMOOTH);                           // Enable Smooth Shading
  COMMON_GL_ASSERT;
//  // XXX docs say all polygons are emitted CCW, but tests show that some aren't
//  glFrontFace (GL_CW);
  //  glHint (GL_POLYGON_SMOOTH_HINT, GL_FASTEST);
  glHint (GL_POLYGON_SMOOTH_HINT, GL_NICEST);
  COMMON_GL_ASSERT;
  glEnable (GL_BLEND);                                // Enable Semi-Transparency
  COMMON_GL_ASSERT;
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  COMMON_GL_ASSERT;
  glEnable (GL_DEPTH_TEST);                           // Enables Depth Testing
  COMMON_GL_ASSERT;
  glEnable (GL_NORMALIZE);
  COMMON_GL_ASSERT;

  return result_p;
}
gboolean
glarea_render_cb (GtkGLArea* GLArea_in,
                  GdkGLContext* context_in,
                  gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::glarea_render_cb"));

  ACE_UNUSED_ARG (context_in);

  // sanity check(s)
  ACE_ASSERT (GLArea_in);
  ACE_ASSERT (userData_in);

  struct ARDrone_GtkCBData_Base* data_p =
    static_cast<struct ARDrone_GtkCBData_Base*> (userData_in);
//  struct Stream_Module_Visualization_OpenGLInstruction* instruction_p = NULL;

  // sanity check(s)
  ACE_ASSERT (data_p);

//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//  struct ARDrone_DirectShow_GtkCBData* directshow_data_p = NULL;
//  struct ARDrone_MediaFoundation_GtkCBData* mediafoundation_data_p =
//    NULL;
//  ARDrone_DirectShow_StreamConfiguration_t::ITERATOR_T directshow_modulehandler_configuration_iterator;
//  ARDrone_MediaFoundation_StreamConfiguration_t::ITERATOR_T mediafoundation_modulehandler_configuration_iterator;
//  switch (data_p->mediaFramework)
//  {
//    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
//    {
//      directshow_data_p =
//        static_cast<struct ARDrone_DirectShow_GtkCBData*> (userData_in);
//      // sanity check(s)
//      ACE_ASSERT (directshow_data_p);
//      ACE_ASSERT (directshow_data_p->configuration);
//
//      directshow_modulehandler_configuration_iterator =
//        directshow_data_p->configuration->streamConfiguration.find (ACE_TEXT_ALWAYS_CHAR (""));
//      ACE_ASSERT (directshow_modulehandler_configuration_iterator != directshow_data_p->configuration->streamConfiguration.end ());
//      break;
//    }
//    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
//    {
//      mediafoundation_data_p =
//        static_cast<struct ARDrone_MediaFoundation_GtkCBData*> (userData_in);
//      // sanity check(s)
//      ACE_ASSERT (mediafoundation_data_p);
//      ACE_ASSERT (mediafoundation_data_p->configuration);
//
//      mediafoundation_modulehandler_configuration_iterator =
//        mediafoundation_data_p->configuration->streamConfiguration.find (ACE_TEXT_ALWAYS_CHAR (""));
//      ACE_ASSERT (mediafoundation_modulehandler_configuration_iterator != mediafoundation_data_p->configuration->streamConfiguration.end ());
//      break;
//    }
//    default:
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
//                  data_p->mediaFramework));
//      goto error;
//    }
//  } // end SWITCH
//#else
//  ACE_ASSERT (data_p->configuration);
//
//  ARDrone_StreamConfigurationsIterator_t streams_iterator =
//      data_p->configuration->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_NAVDATA_STREAM_NAME_STRING));
//  ACE_ASSERT (streams_iterator != data_p->configuration->streamConfigurations.end ());
//  ARDrone_StreamConfiguration_t::ITERATOR_T modulehandler_configuration_iterator =
//    (*streams_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (""));
//  ACE_ASSERT (modulehandler_configuration_iterator != (*streams_iterator).second.end ());
//#endif

  GLuint* model_list_id_p = NULL;
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//  if (data_p->useMediaFoundation)
//    model_list_id_p = &mediafoundation_data_p->openGLModelListId;
//  else
//    model_list_id_p = &directshow_data_p->openGLModelListId;
//#else
  model_list_id_p = &data_p->openGLModelListId;
//#endif
  ACE_ASSERT (model_list_id_p);
  ACE_ASSERT (*model_list_id_p);

//  glBindTexture (GL_TEXTURE_2D, *model_list_id_p);
//  COMMON_GL_ASSERT;

  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  COMMON_GL_ASSERT;

  glLoadIdentity ();
  COMMON_GL_ASSERT;
  // left
  //  gluLookAt (0.0f,0.0f,3.0f,  // eye xyz
//             0.0f,0.0f,0.0f,  // center xyz
//             0.0f,1.0f,0.0f); // up xyz
  // top
//  gluLookAt (0.0f,3.0f,0.0f,  // eye xyz
//             0.0f,0.0f,0.0f,  // center xyz
//             -1.0f,0.0f,0.0f); // up xyz
  // behind
  gluLookAt (3.0f,0.0f,0.0f,  // eye xyz
             0.0f,0.0f,0.0f,  // center xyz
             0.0f,1.0f,0.0f); // up xyz
  COMMON_GL_ASSERT;

//  glTranslatef (0.0f, 0.0f, ARDRONE_OPENGL_CAMERA_DEFAULT_Z);
//  COMMON_GL_ASSERT;

  // rotate it around the all axes
  static GLfloat rotation_angle = 0.0f;
  glRotatef (rotation_angle, 0.0f, 1.0f, 0.0f);
//  glRotatef (rotation_angle, 1.0f, 1.0f, 1.0f);
  COMMON_GL_ASSERT;

  // scale the whole asset to fit into the view frustum
  static GLfloat scale_factor = 0.0f;
  scale_factor = (data_p->openGLScene.boundingBox.second.x -
                  data_p->openGLScene.boundingBox.first.x);
  scale_factor =
      COMMON_GL_ASSIMP_MAX (data_p->openGLScene.boundingBox.second.y - data_p->openGLScene.boundingBox.first.y, scale_factor);
  scale_factor =
      COMMON_GL_ASSIMP_MAX (data_p->openGLScene.boundingBox.second.z - data_p->openGLScene.boundingBox.first.z, scale_factor);
  scale_factor = (1.0f / scale_factor) * 2.0f;
  glScalef (scale_factor, scale_factor, scale_factor);
  COMMON_GL_ASSERT;

  // center the model
  glTranslatef (-data_p->openGLScene.center.x,
                -data_p->openGLScene.center.y,
                -data_p->openGLScene.center.z);
  COMMON_GL_ASSERT;

  glCallList (data_p->openGLModelListId);
  COMMON_GL_ASSERT;

//  Common_GL_Tools::drawCube (true);
//  COMMON_GL_ASSERT;

//  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, data_p->lock, FALSE);
//    if (data_p->OpenGLInstructions.empty ())
//      goto continue_;

//    do
//    {
//      instruction_p = &data_p->OpenGLInstructions.front ();
//      switch (instruction_p->type)
//      {
//        case STREAM_MODULE_VIS_OPENGLINSTRUCTION_SET_COLOR_BG:
//        {
//          glClearColor (static_cast<GLclampf> (instruction_p->color.red),
//                        static_cast<GLclampf> (instruction_p->color.green),
//                        static_cast<GLclampf> (instruction_p->color.blue),
//                        1.0F);
//          break;
//        }
//        case STREAM_MODULE_VIS_OPENGLINSTRUCTION_SET_COLOR_FG:
//        {
//          glColor4f (static_cast<GLclampf> (instruction_p->color.red),
//                     static_cast<GLclampf> (instruction_p->color.green),
//                     static_cast<GLclampf> (instruction_p->color.blue),
//                     1.0F);
//          break;
//        }
//        default:
//        {
//          ACE_DEBUG ((LM_ERROR,
//                      ACE_TEXT ("invalid/unknown OpenGL effect (was: %d), continuing\n"),
//                      ACE_TEXT (instruction_p->type)));
//          break;
//        }
//      } // end SWITCH
//      data_p->OpenGLInstructions.pop_front ();
//    } while (!data_p->OpenGLInstructions.empty ());
//  } // end lock scope

//  rotation_angle -= 1.0f;

//continue_:
  return TRUE;
}
void
glarea_resize_cb (GtkGLArea* GLArea_in,
                  gint width_in,
                  gint height_in,
                  gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::glarea_resize_cb"));

  // sanity check(s)
  ACE_ASSERT (GLArea_in);
  ACE_ASSERT (userData_in);

  struct ARDrone_GtkCBData_Base* data_p =
    static_cast<struct ARDrone_GtkCBData_Base*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//  struct ARDrone_DirectShow_GtkCBData* directshow_data_p = NULL;
//  struct ARDrone_MediaFoundation_GtkCBData* mediafoundation_data_p =
//    NULL;
//  ARDrone_DirectShow_StreamConfiguration_t::ITERATOR_T directshow_modulehandler_configuration_iterator;
//  ARDrone_MediaFoundation_StreamConfiguration_t::ITERATOR_T mediafoundation_modulehandler_configuration_iterator;
//  if (data_p->useMediaFoundation)
//  {
//    mediafoundation_data_p =
//      static_cast<struct ARDrone_MediaFoundation_GtkCBData*> (userData_in);
//    // sanity check(s)
//    ACE_ASSERT (mediafoundation_data_p);
//    ACE_ASSERT (mediafoundation_data_p->configuration);
//
//    mediafoundation_modulehandler_configuration_iterator =
//      mediafoundation_data_p->configuration->streamConfiguration.find (ACE_TEXT_ALWAYS_CHAR (""));
//    ACE_ASSERT (mediafoundation_modulehandler_configuration_iterator != mediafoundation_data_p->configuration->streamConfiguration.end ());
//  } // end IF
//  else
//  {
//    directshow_data_p =
//      static_cast<struct ARDrone_DirectShow_GtkCBData*> (userData_in);
//    // sanity check(s)
//    ACE_ASSERT (directshow_data_p);
//    ACE_ASSERT (directshow_data_p->configuration);
//
//    directshow_modulehandler_configuration_iterator =
//      directshow_data_p->configuration->streamConfiguration.find (ACE_TEXT_ALWAYS_CHAR (""));
//    ACE_ASSERT (directshow_modulehandler_configuration_iterator != directshow_data_p->configuration->streamConfiguration.end ());
//  } // end ELSE
//#else
//  ACE_ASSERT (data_p->configuration);
//
//  ARDrone_StreamConfigurationsIterator_t streams_iterator =
//      data_p->configuration->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_NAVDATA_STREAM_NAME_STRING));
//  ACE_ASSERT (streams_iterator != data_p->configuration->streamConfigurations.end ());
//  ARDrone_StreamConfiguration_t::ITERATOR_T modulehandler_configuration_iterator =
//    (*streams_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (""));
//  ACE_ASSERT (modulehandler_configuration_iterator != (*streams_iterator).second.end ());
//#endif

  //gtk_gl_area_make_current (GLArea_in);

  glViewport (0, 0,
              static_cast<GLsizei> (width_in), static_cast<GLsizei> (height_in));
  // *TODO*: find out why this reports GL_INVALID_OPERATION
  COMMON_GL_PRINT_ERROR;

  glMatrixMode (GL_PROJECTION);
  COMMON_GL_ASSERT;

  glLoadIdentity ();
  COMMON_GL_ASSERT;

  gluPerspective (ARDRONE_OPENGL_PERSPECTIVE_FOVY,
                  static_cast<GLdouble> (width_in) / static_cast<GLdouble> (height_in),
                  ARDRONE_OPENGL_PERSPECTIVE_ZNEAR,
                  ARDRONE_OPENGL_PERSPECTIVE_ZFAR);
  COMMON_GL_ASSERT;

  glMatrixMode (GL_MODELVIEW);
  COMMON_GL_ASSERT;
}
#else
void
glarea_size_allocate_event_cb (GtkWidget* widget_in,
                               GdkRectangle* allocation_in,
                               gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::glarea_size_allocate_event_cb"));

  struct ARDrone_GtkCBData_Base* data_p =
    static_cast<struct ARDrone_GtkCBData_Base*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct Test_U_AudioEffect_DirectShow_GTK_CBData* directshow_data_p = NULL;
  struct Test_U_AudioEffect_MediaFoundation_GTK_CBData* mediafoundation_data_p =
    NULL;
  if (data_p->useMediaFoundation)
  {
    mediafoundation_data_p =
      static_cast<struct Test_U_AudioEffect_MediaFoundation_GTK_CBData*> (userData_in);
    // sanity check(s)
    ACE_ASSERT (mediafoundation_data_p);
    ACE_ASSERT (mediafoundation_data_p->configuration);
  } // end IF
  else
  {
    directshow_data_p =
      static_cast<struct Test_U_AudioEffect_DirectShow_GTK_CBData*> (userData_in);
    // sanity check(s)
    ACE_ASSERT (directshow_data_p);
    ACE_ASSERT (directshow_data_p->configuration);
  } // end ELSE
#else
  struct Test_U_AudioEffect_GTK_CBData* data_p =
    static_cast<struct Test_U_AudioEffect_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->configuration);

#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
#else
  // sanity check(s)
  ACE_ASSERT (widget_in);
#endif
#else
#if defined (GTKGLAREA_SUPPORT)
  // sanity check(s)
  ACE_ASSERT (widget_in);
#else
  GdkGLDrawable* drawable_p =
    (*modulehandler_configuration_iterator).second.GdkWindow3D;
  GdkGLContext* context_p =
    (*modulehandler_configuration_iterator).second.OpenGLContext;

  // sanity check(s)
  ACE_ASSERT (drawable_p);
  ACE_ASSERT (context_p);
#endif
#endif
#endif

#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
#else
  if (!ggla_area_make_current (GGLA_AREA (widget_in)))
#endif
#else
#if defined (GTKGLAREA_SUPPORT)
  if (!gtk_gl_area_make_current (GTK_GL_AREA (widget_in)))
#else
  if (!gdk_gl_drawable_make_current (drawable_p,
                                     context_p))
#endif
#endif
    return;

  glViewport (0, 0,
              allocation_in->width, allocation_in->height);
  ACE_ASSERT (glGetError () == GL_NO_ERROR);

  glMatrixMode (GL_PROJECTION);
  ACE_ASSERT (glGetError () == GL_NO_ERROR);
  glLoadIdentity (); // Reset The Projection Matrix
  ACE_ASSERT (glGetError () == GL_NO_ERROR);

  gluPerspective (45.0,
                  allocation_in->width / (GLdouble)allocation_in->height,
                  0.1,
                  100.0); // Calculate The Aspect Ratio Of The Window
  ACE_ASSERT (glGetError () == GL_NO_ERROR);

  glMatrixMode (GL_MODELVIEW);
  ACE_ASSERT (glGetError () == GL_NO_ERROR);
}
gboolean
glarea_draw_cb (GtkWidget* widget_in,
                cairo_t* context_in,
                gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::glarea_draw_cb"));

  ACE_UNUSED_ARG (context_in);

  // sanity check(s)
  ACE_ASSERT (widget_in);
  ACE_ASSERT (userData_in);

  struct Test_U_AudioEffect_GTK_CBData* data_p =
    static_cast<struct Test_U_AudioEffect_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  GLuint* texture_id_p = NULL;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct Test_U_AudioEffect_DirectShow_GTK_CBData* directshow_data_p = NULL;
  struct Test_U_AudioEffect_MediaFoundation_GTK_CBData* mediafoundation_data_p = NULL;
  if (data_p->useMediaFoundation)
  {
    mediafoundation_data_p =
      static_cast<struct Test_U_AudioEffect_MediaFoundation_GTK_CBData*> (userData_in);
    // sanity check(s)
    ACE_ASSERT (mediafoundation_data_p);
    ACE_ASSERT (mediafoundation_data_p->configuration);

    texture_id_p =
      &(*mediafoundation_modulehandler_configuration_iterator).second.OpenGLTextureId;
  } // end IF
  else
  {
    directshow_data_p =
      static_cast<struct Test_U_AudioEffect_DirectShow_GTK_CBData*> (userData_in);
    // sanity check(s)
    ACE_ASSERT (directshow_data_p);
    ACE_ASSERT (directshow_data_p->configuration);

    texture_id_p =
      &(*directshow_modulehandler_configuration_iterator).second.OpenGLTextureId;
  } // end ELSE
#else
  struct Test_U_AudioEffect_GTK_CBData* data_p =
    static_cast<struct Test_U_AudioEffect_GTK_CBData*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);
  ACE_ASSERT (data_p->configuration);

#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
#else
  // sanity check(s)
  ACE_ASSERT (widget_in);

  Test_U_AudioEffect_StreamConfiguration_t::ITERATOR_T modulehandler_configuration_iterator =
    data_p->configuration->streamConfiguration.find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (modulehandler_configuration_iterator != data_p->configuration->streamConfiguration.end ());
#endif
#else
#if defined (GTKGLAREA_SUPPORT)
  // sanity check(s)
  ACE_ASSERT (widget_in);
#else
  GdkGLDrawable* drawable_p =
    (*modulehandler_configuration_iterator).second.GdkWindow3D;
  GdkGLContext* context_p =
    (*modulehandler_configuration_iterator).second.OpenGLContext;

  // sanity check(s)
  ACE_ASSERT (drawable_p);
  ACE_ASSERT (context_p);
#endif
#endif

  texture_id_p =
    &(*modulehandler_configuration_iterator).second.OpenGLTextureId;
#endif
  ACE_ASSERT (texture_id_p);

#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
#else
  if (!ggla_area_make_current (GGLA_AREA (widget_in)))
#endif
#else
#if defined (GTKGLAREA_SUPPORT)
  if (!gtk_gl_area_make_current (GTK_GL_AREA (widget_in)))
#else
  bool result = gdk_gl_drawable_make_current (drawable_p,
                                              context_p);
  if (!result)
#endif
#endif
    return FALSE;

#if GTK_CHECK_VERSION (3,0,0)
#else
#if defined (GTKGLAREA_SUPPORT)
#else
  result = gdk_gl_drawable_gl_begin (drawable_p,
                                     context_p);
  if (!result)
    return FALSE;
#endif
#endif

  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  ACE_ASSERT (glGetError () == GL_NO_ERROR);
  glLoadIdentity (); // Reset the transformation matrix.
  ACE_ASSERT (glGetError () == GL_NO_ERROR);
  glTranslatef (0.0F, 0.0F, -5.0F); // Move back into the screen 5 units
  ACE_ASSERT (glGetError () == GL_NO_ERROR);

  glBindTexture (GL_TEXTURE_2D, *texture_id_p);
  ACE_ASSERT (glGetError () == GL_NO_ERROR);

//  static GLfloat rot_x = 0.0f;
//  static GLfloat rot_y = 0.0f;
//  static GLfloat rot_z = 0.0f;
//  glRotatef (rot_x, 1.0f, 0.0f, 0.0f); // Rotate On The X Axis
//  glRotatef (rot_y, 0.0f, 1.0f, 0.0f); // Rotate On The Y Axis
//  glRotatef (rot_z, 0.0f, 0.0f, 1.0f); // Rotate On The Z Axis
  static GLfloat rotation = 0.0F;
  glRotatef (rotation, 1.0F, 1.0F, 1.0F); // Rotate On The X,Y,Z Axis
  ACE_ASSERT (glGetError () == GL_NO_ERROR);

//  glBegin (GL_QUADS);

//  glTexCoord2i (0, 0); glVertex3f (  0.0f,   0.0f, 0.0f);
//  glTexCoord2i (0, 1); glVertex3f (  0.0f, 100.0f, 0.0f);
//  glTexCoord2i (1, 1); glVertex3f (100.0f, 100.0f, 0.0f);
//  glTexCoord2i (1, 0); glVertex3f (100.0f,   0.0f, 0.0f);

  static GLfloat vertices[] = {
    -0.5f, 0.0f, 0.5f,   0.5f, 0.0f, 0.5f,   0.5f, 1.0f, 0.5f,  -0.5f, 1.0f, 0.5f,
    -0.5f, 1.0f, -0.5f,  0.5f, 1.0f, -0.5f,  0.5f, 0.0f, -0.5f, -0.5f, 0.0f, -0.5f,
    0.5f, 0.0f, 0.5f,   0.5f, 0.0f, -0.5f,  0.5f, 1.0f, -0.5f,  0.5f, 1.0f, 0.5f,
    -0.5f, 0.0f, -0.5f,  -0.5f, 0.0f, 0.5f,  -0.5f, 1.0f, 0.5f, -0.5f, 1.0f, -0.5f};
  static GLfloat texture_coordinates[] = {
    0.0,0.0, 1.0,0.0, 1.0,1.0, 0.0,1.0,
    0.0,0.0, 1.0,0.0, 1.0,1.0, 0.0,1.0,
    0.0,0.0, 1.0,0.0, 1.0,1.0, 0.0,1.0,
    0.0,0.0, 1.0,0.0, 1.0,1.0, 0.0,1.0 };
  static GLubyte cube_indices[24] = {
    0,1,2,3, 4,5,6,7, 3,2,5,4, 7,6,1,0,
    8,9,10,11, 12,13,14,15};

  glTexCoordPointer (2, GL_FLOAT, 0, texture_coordinates);
  ACE_ASSERT (glGetError () == GL_NO_ERROR);
  glVertexPointer (3, GL_FLOAT, 0, vertices);
  ACE_ASSERT (glGetError () == GL_NO_ERROR);
  glDrawElements (GL_QUADS, 24, GL_UNSIGNED_BYTE, cube_indices);
  ACE_ASSERT (glGetError () == GL_NO_ERROR);

//  rot_x += 0.3f;
//  rot_y += 0.20f;
//  rot_z += 0.4f;
  rotation -= 1.0f; // Decrease The Rotation Variable For The Cube

  //GLuint vertex_array_id = 0;
  //glGenVertexArrays (1, &vertex_array_id);
  //glBindVertexArray (vertex_array_id);

  //static const GLfloat vertex_buffer_data[] = {
  //  -1.0f, -1.0f, 0.0f,
  //  1.0f, -1.0f, 0.0f,
  //  -1.0f,  1.0f, 0.0f,
  //  -1.0f,  1.0f, 0.0f,
  //  1.0f, -1.0f, 0.0f,
  //  1.0f,  1.0f, 0.0f,
  //};

  //GLuint vertex_buffer;
  //glGenBuffers (1, &vertex_buffer);
  //glBindBuffer (GL_ARRAY_BUFFER, vertex_buffer);
  //glBufferData (GL_ARRAY_BUFFER,
  //              sizeof (vertex_buffer_data), vertex_buffer_data,
  //              GL_STATIC_DRAW);

  ////GLuint program_id = LoadShaders ("Passthrough.vertexshader",
  ////                                 "SimpleTexture.fragmentshader");
  ////GLuint tex_id = glGetUniformLocation (program_id, "renderedTexture");
  ////GLuint time_id = glGetUniformLocation (program_id, "time");

  //glBindFramebuffer (GL_FRAMEBUFFER, 0);
  //glViewport (0, 0,
  //            data_p->area3D.width, data_p->area3D.height);

#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
#else
  ggla_area_swap_buffers (GGLA_AREA (widget_in));
#endif
#else
#if defined (GTKGLAREA_SUPPORT)
  gtk_gl_area_swap_buffers (GTK_GL_AREA (widget_in));
#else
  gdk_gl_drawable_gl_end (drawable_p);
  gdk_gl_drawable_swap_buffers (drawable_p);
#endif
#endif

  return TRUE;
}
//void
//glarea_realize_cb (GtkWidget* widget_in,
//                   gpointer userData_in)
//{
//  ARDRONE_TRACE (ACE_TEXT ("::glarea_realize_cb"));
//
//  // sanity check(s)
//  ACE_ASSERT (widget_in);
//  ACE_ASSERT (userData_in);
//
//  struct ARDrone_GtkCBData_Base* data_p =
//    static_cast<struct ARDrone_GtkCBData_Base*> (userData_in);
//
//  // sanity check(s)
//  ACE_ASSERT (data_p);
//
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//  struct Test_U_AudioEffect_DirectShow_GTK_CBData* directshow_data_p = NULL;
//  struct Test_U_AudioEffect_MediaFoundation_GTK_CBData* mediafoundation_data_p =
//    NULL;
//  if (data_p->useMediaFoundation)
//  {
//    mediafoundation_data_p =
//      static_cast<struct Test_U_AudioEffect_MediaFoundation_GTK_CBData*> (userData_in);
//    // sanity check(s)
//    ACE_ASSERT (mediafoundation_data_p);
//    ACE_ASSERT (mediafoundation_data_p->configuration);
//  } // end IF
//  else
//  {
//    directshow_data_p =
//      static_cast<struct Test_U_AudioEffect_DirectShow_GTK_CBData*> (userData_in);
//    // sanity check(s)
//    ACE_ASSERT (directshow_data_p);
//    ACE_ASSERT (directshow_data_p->configuration);
//  } // end ELSE
//#else
//  struct Test_U_AudioEffect_GTK_CBData* data_p =
//    static_cast<struct Test_U_AudioEffect_GTK_CBData*> (userData_in);
//
//  // sanity check(s)
//  ACE_ASSERT (data_p);
//  ACE_ASSERT (data_p->configuration);
//
//#if GTK_CHECK_VERSION (3,0,0)
//#if GTK_CHECK_VERSION (3,16,0)
//#else
//  // sanity check(s)
//  ACE_ASSERT (widget_in);
//
//  Test_U_AudioEffect_StreamConfiguration_t::ITERATOR_T modulehandler_configuration_iterator =
//    data_p->configuration->streamConfiguration.find (ACE_TEXT_ALWAYS_CHAR (""));
//  ACE_ASSERT (modulehandler_configuration_iterator != data_p->configuration->streamConfiguration.end ());
//#endif
//#else
//#if defined (GTKGLAREA_SUPPORT)
//  // sanity check(s)
//  ACE_ASSERT (widget_in);
//#else
//  GdkGLDrawable* drawable_p =
//    (*modulehandler_configuration_iterator).second.GdkWindow3D;
//  GdkGLContext* context_p =
//    (*modulehandler_configuration_iterator).second.OpenGLContext;
//
//  // sanity check(s)
//  ACE_ASSERT (drawable_p);
//  ACE_ASSERT (context_p);
//#endif
//#endif
//#endif
//
//#if GTK_CHECK_VERSION (3,0,0)
//#if GTK_CHECK_VERSION (3,16,0)
//#else
//  if (!ggla_area_make_current (GGLA_AREA (widget_in)))
//#endif
//#else
//#if defined (GTKGLAREA_SUPPORT)
//  if (!gtk_gl_area_make_current (GTK_GL_AREA (widget_in)))
//#else
//  bool result = gdk_gl_drawable_make_current (drawable_p,
//                                              context_p);
//  if (!result)
//#endif
//#endif
//    return;
//
//#if GTK_CHECK_VERSION (3,0,0)
//#else
//#if defined (GTKGLAREA_SUPPORT)
//#else
//  result = gdk_gl_drawable_gl_begin (drawable_p,
//                                     context_p);
//  if (!result)
//    return;
//#endif
//#endif
//
//  GtkAllocation allocation;
//  gtk_widget_get_allocation (widget_in, &allocation);
//
//  GLuint* texture_id_p = NULL;
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//  if (data_p->useMediaFoundation)
//    texture_id_p =
//      &(*mediafoundation_modulehandler_configuration_iterator).second.OpenGLTextureId;
//  else
//    texture_id_p =
//      &(*directshow_modulehandler_configuration_iterator).second.OpenGLTextureId;
//#else
//  texture_id_p =
//    &(*modulehandler_configuration_iterator).second.OpenGLTextureId;
//#endif
//  ACE_ASSERT (texture_id_p);
//
//  static GLubyte* image_p = NULL;
//  if (!image_p)
//  {
//    std::string filename = Common_File_Tools::getWorkingDirectory ();
//    filename += ACE_DIRECTORY_SEPARATOR_CHAR;
//    filename += ACE_TEXT_ALWAYS_CHAR (TEST_U_STREAM_CONFIGURATION_DIRECTORY);
//    filename += ACE_DIRECTORY_SEPARATOR_CHAR;
//    filename +=
//      ACE_TEXT_ALWAYS_CHAR (TEST_U_STREAM_AUDIOEFFECT_DEFAULT_IMAGE_FILE);
//    unsigned int width, height;
//    bool has_alpha = false;
//    if (!Common_Image_Tools::loadPNG2OpenGL (filename,
//                                             width, height,
//                                             has_alpha,
//                                             image_p))
//    {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("failed to Common_Image_Tools::loadPNG2OpenGL(\"%s\"): \"%m\", returning\n"),
//                  ACE_TEXT (filename.c_str ())));
//      return;
//    } // end IF
//    ACE_ASSERT (image_p);
//    ACE_DEBUG ((LM_DEBUG,
//                ACE_TEXT ("loaded \"%s\"...\n"),
//                ACE_TEXT (filename.c_str ())));
//
//    glGenTextures (1, texture_id_p);
//    ACE_ASSERT (glGetError () == GL_NO_ERROR);
//    glBindTexture (GL_TEXTURE_2D, *texture_id_p);
//    ACE_ASSERT (glGetError () == GL_NO_ERROR);
//    glEnableClientState (GL_TEXTURE_COORD_ARRAY);
//    ACE_ASSERT (glGetError () == GL_NO_ERROR);
//    glEnableClientState (GL_VERTEX_ARRAY);
//    ACE_ASSERT (glGetError () == GL_NO_ERROR);
//    glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
//    ACE_ASSERT (glGetError () == GL_NO_ERROR);
//    // select modulate to mix texture with color for shading
////    glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
////    ACE_ASSERT (glGetError () == GL_NO_ERROR);
//    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    ACE_ASSERT (glGetError () == GL_NO_ERROR);
//    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    ACE_ASSERT (glGetError () == GL_NO_ERROR);
//    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA,
//                  width, height,
//                  0, (has_alpha ? GL_RGBA : GL_RGB),
//                  GL_UNSIGNED_BYTE, image_p);
//    ACE_ASSERT (glGetError () == GL_NO_ERROR);
//    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    ACE_ASSERT (glGetError () == GL_NO_ERROR);
//    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
//    ACE_ASSERT (glGetError () == GL_NO_ERROR);
//    glTexParameteri (GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
//    ACE_ASSERT (glGetError () == GL_NO_ERROR);
//    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
//    ACE_ASSERT (glGetError () == GL_NO_ERROR);
//    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
//    ACE_ASSERT (glGetError () == GL_NO_ERROR);
//
//    ACE_DEBUG ((LM_DEBUG,
//                ACE_TEXT ("OpenGL texture ID: %u...\n"),
//                *texture_id_p));
//
//    // clean up (do NOT reset the pointer)
//    free (image_p);
//  } // end IF
//
//  glViewport (0, 0,
//              allocation.width, allocation.height);
//  ACE_ASSERT (glGetError () == GL_NO_ERROR);
//
//  glMatrixMode (GL_PROJECTION);
//  ACE_ASSERT (glGetError () == GL_NO_ERROR);
//////  glOrtho (0.0, allocation.width,
//////           0.0, allocation.height,
//////           -1.0, 1.0);
//////  glMatrixMode (GL_MODELVIEW);
//  glLoadIdentity ();
//  ACE_ASSERT (glGetError () == GL_NO_ERROR);
//  gluPerspective (45.0,
//                  (allocation.width / (GLdouble)allocation.height),
//                  0.1,
//                  100.0); // setup a perspective projection
//  ACE_ASSERT (glGetError () == GL_NO_ERROR);
////  GLdouble fW, fH;
////  fH =
////   ::tan (60.0 / 360.0 * M_PI) *
////   -1.0;
////  fW = fH * (allocation.width / allocation.height);
////  glFrustum (-fW, fW,
////             -fH, fH,
////             -1.0,
////             100.0);
////  gluLookAt (-10.0, 0.0, 0.0, // eye position (*NOTE*: relative to standard
////             //                       "right-hand" coordinate
////             //                       system [RHCS])
////             0.0, 0.0, 0.0,   // looking-at position (RHCS notation)
////             0.0, 0.0, -1.0); // up direction (RHCS notation, relative to eye
//  // position and looking-at direction)
//  glMatrixMode (GL_MODELVIEW);
//  ACE_ASSERT (glGetError () == GL_NO_ERROR);
//
//  /* light */
////  GLfloat light_positions[2][4]   = { 50.0, 50.0, 0.0, 0.0,
////                                     -50.0, 50.0, 0.0, 0.0 };
////  GLfloat light_colors[2][4] = { .6, .6,  .6, 1.0,   /* white light */
////                                 .4, .4, 1.0, 1.0 }; /* cold blue light */
////  glLightfv (GL_LIGHT0, GL_POSITION, light_positions[0]);
////  glLightfv (GL_LIGHT0, GL_DIFFUSE,  light_colors[0]);
////  glLightfv (GL_LIGHT1, GL_POSITION, light_positions[1]);
////  glLightfv (GL_LIGHT1, GL_DIFFUSE,  light_colors[1]);
////  glEnable (GL_LIGHT0);
////  glEnable (GL_LIGHT1);
////  glEnable (GL_LIGHTING);
//
//  // set up light colors (ambient, diffuse, specular)
//  GLfloat light_ambient[] = {1.0F, 1.0F, 1.0F, 1.0F};
//  glLightfv (GL_LIGHT0, GL_AMBIENT, light_ambient);
//  ACE_ASSERT (glGetError () == GL_NO_ERROR);
//  GLfloat light_diffuse[] = {0.3F, 0.3F, 0.3F, 1.0F};
//  glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);
//  ACE_ASSERT (glGetError () == GL_NO_ERROR);
//  GLfloat light_specular[] = {1.0F, 1.0F, 1.0F, 1.0F};
//  glLightfv (GL_LIGHT0, GL_SPECULAR, light_specular);
//  ACE_ASSERT (glGetError () == GL_NO_ERROR);
//  // position the light in eye space
//  GLfloat light0_position[] = {0.0F,
//                               5.0F * 2,
//                               5.0F * 2,
//                               0.0F}; // --> directional light
//  glLightfv (GL_LIGHT0, GL_POSITION, light0_position);
//  ACE_ASSERT (glGetError () == GL_NO_ERROR);
//  glEnable (GL_LIGHT0);
//  ACE_ASSERT (glGetError () == GL_NO_ERROR);
//
//#if GTK_CHECK_VERSION (3,0,0)
//#else
//#if defined (GTKGLAREA_SUPPORT)
//#else
//  gdk_gl_drawable_gl_end (drawable_p);
//#endif
//#endif
//} // glarea_realize_cb
#endif
#else
#if defined (GTKGLAREA_SUPPORT)
void
glarea_configure_event_cb (GtkWidget* widget_in,
                           GdkEvent* event_in,
                           gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::glarea_configure_event_cb"));

  struct ARDrone_GtkCBData_Base* data_p =
    static_cast<struct ARDrone_GtkCBData_Base*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct Test_U_AudioEffect_DirectShow_GTK_CBData* directshow_data_p = NULL;
  struct Test_U_AudioEffect_MediaFoundation_GTK_CBData* mediafoundation_data_p =
    NULL;
  if (data_p->useMediaFoundation)
  {
    mediafoundation_data_p =
      static_cast<struct Test_U_AudioEffect_MediaFoundation_GTK_CBData*> (userData_in);
    // sanity check(s)
    ACE_ASSERT (mediafoundation_data_p);
    ACE_ASSERT (mediafoundation_data_p->configuration);
  } // end IF
  else
  {
    directshow_data_p =
      static_cast<struct Test_U_AudioEffect_DirectShow_GTK_CBData*> (userData_in);
    // sanity check(s)
    ACE_ASSERT (directshow_data_p);
    ACE_ASSERT (directshow_data_p->configuration);
  } // end ELSE
#else
  // sanity check(s)
  ACE_ASSERT (data_p->configuration);

#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
#else
  // sanity check(s)
  ACE_ASSERT (widget_in);
#endif
#else
#if defined (GTKGLAREA_SUPPORT)
  // sanity check(s)
  ACE_ASSERT (widget_in);
#else
  GdkGLDrawable* drawable_p =
    (*modulehandler_configuration_iterator).second.GdkWindow3D;
  GdkGLContext* context_p =
    (*modulehandler_configuration_iterator).second.OpenGLContext;

  // sanity check(s)
  ACE_ASSERT (drawable_p);
  ACE_ASSERT (context_p);
#endif
#endif
#endif

#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
#else
  if (!ggla_area_make_current (GGLA_AREA (widget_in)))
#endif
#else
#if defined (GTKGLAREA_SUPPORT)
  if (!gtk_gl_area_make_current (GTK_GL_AREA (widget_in)))
#else
  if (!gdk_gl_drawable_make_current (drawable_p,
                                     context_p))
#endif
#endif
    return;

  glViewport (0, 0,
              event_in->configure.width, event_in->configure.height);
  ACE_ASSERT (glGetError () == GL_NO_ERROR);

  glMatrixMode (GL_PROJECTION);
  ACE_ASSERT (glGetError () == GL_NO_ERROR);
  glLoadIdentity (); // Reset The Projection Matrix
  ACE_ASSERT (glGetError () == GL_NO_ERROR);

  gluPerspective (45.0,
                  event_in->configure.width / (GLdouble)event_in->configure.height,
                  0.1,
                  100.0); // Calculate The Aspect Ratio Of The Window
  ACE_ASSERT (glGetError () == GL_NO_ERROR);

  glMatrixMode (GL_MODELVIEW);
  ACE_ASSERT (glGetError () == GL_NO_ERROR);
}
gboolean
glarea_expose_event_cb (GtkWidget* widget_in,
                        cairo_t* context_in,
                        gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::glarea_expose_event_cb"));

  ACE_UNUSED_ARG (context_in);

  // sanity check(s)
  ACE_ASSERT (widget_in);
  ACE_ASSERT (userData_in);

  struct ARDrone_GtkCBData_Base* data_p =
    static_cast<struct ARDrone_GtkCBData_Base*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

  GLuint* texture_id_p = NULL;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct Test_U_AudioEffect_DirectShow_GTK_CBData* directshow_data_p = NULL;
  struct Test_U_AudioEffect_MediaFoundation_GTK_CBData* mediafoundation_data_p =
    NULL;
  if (data_p->useMediaFoundation)
  {
    mediafoundation_data_p =
      static_cast<struct Test_U_AudioEffect_MediaFoundation_GTK_CBData*> (userData_in);
    // sanity check(s)
    ACE_ASSERT (mediafoundation_data_p);
    ACE_ASSERT (mediafoundation_data_p->configuration);

    texture_id_p =
      &(*mediafoundation_modulehandler_configuration_iterator).second.OpenGLTextureId;
  } // end IF
  else
  {
    directshow_data_p =
      static_cast<struct Test_U_AudioEffect_DirectShow_GTK_CBData*> (userData_in);
    // sanity check(s)
    ACE_ASSERT (directshow_data_p);
    ACE_ASSERT (directshow_data_p->configuration);

    texture_id_p =
      &(*directshow_modulehandler_configuration_iterator).second.OpenGLTextureId;
  } // end ELSE
#else
  // sanity check(s)
  ACE_ASSERT (data_p->configuration);

#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
#else
  // sanity check(s)
  ACE_ASSERT (widget_in);
#endif
#else
#if defined (GTKGLAREA_SUPPORT)
  // sanity check(s)
  ACE_ASSERT (widget_in);
#else
  GdkGLDrawable* drawable_p =
    (*modulehandler_configuration_iterator).second.GdkWindow3D;
  GdkGLContext* context_p =
    (*modulehandler_configuration_iterator).second.OpenGLContext;

  // sanity check(s)
  ACE_ASSERT (drawable_p);
  ACE_ASSERT (context_p);
#endif
#endif

  texture_id_p =
    &((*modulehandler_configuration_iterator).second.OpenGLTextureId);
#endif
  ACE_ASSERT (texture_id_p);

#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
#else
  if (!ggla_area_make_current (GGLA_AREA (widget_in)))
#endif
#else
#if defined (GTKGLAREA_SUPPORT)
  if (!gtk_gl_area_make_current (GTK_GL_AREA (widget_in)))
#else
  bool result = gdk_gl_drawable_make_current (drawable_p,
                                              context_p);
  if (!result)
#endif
#endif
    return FALSE;

#if GTK_CHECK_VERSION (3,0,0)
#else
#if defined (GTKGLAREA_SUPPORT)
#else
  result = gdk_gl_drawable_gl_begin (drawable_p,
                                     context_p);
  if (!result)
    return FALSE;
#endif
#endif

  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  ACE_ASSERT (glGetError () == GL_NO_ERROR);
  glLoadIdentity (); // Reset the transformation matrix.
  ACE_ASSERT (glGetError () == GL_NO_ERROR);
  glTranslatef (0.0F, 0.0F, -5.0F); // Move back into the screen 5 units
  ACE_ASSERT (glGetError () == GL_NO_ERROR);

  glBindTexture (GL_TEXTURE_2D, *texture_id_p);
  ACE_ASSERT (glGetError () == GL_NO_ERROR);

//  static GLfloat rot_x = 0.0f;
//  static GLfloat rot_y = 0.0f;
//  static GLfloat rot_z = 0.0f;
//  glRotatef (rot_x, 1.0f, 0.0f, 0.0f); // Rotate On The X Axis
//  glRotatef (rot_y, 0.0f, 1.0f, 0.0f); // Rotate On The Y Axis
//  glRotatef (rot_z, 0.0f, 0.0f, 1.0f); // Rotate On The Z Axis
  static GLfloat rotation = 0.0F;
  glRotatef (rotation, 1.0F, 1.0F, 1.0F); // Rotate On The X,Y,Z Axis
  ACE_ASSERT (glGetError () == GL_NO_ERROR);

//  glBegin (GL_QUADS);

//  glTexCoord2i (0, 0); glVertex3f (  0.0f,   0.0f, 0.0f);
//  glTexCoord2i (0, 1); glVertex3f (  0.0f, 100.0f, 0.0f);
//  glTexCoord2i (1, 1); glVertex3f (100.0f, 100.0f, 0.0f);
//  glTexCoord2i (1, 0); glVertex3f (100.0f,   0.0f, 0.0f);

  static GLfloat vertices[] = {
    -0.5f, 0.0f, 0.5f,   0.5f, 0.0f, 0.5f,   0.5f, 1.0f, 0.5f,  -0.5f, 1.0f, 0.5f,
    -0.5f, 1.0f, -0.5f,  0.5f, 1.0f, -0.5f,  0.5f, 0.0f, -0.5f, -0.5f, 0.0f, -0.5f,
    0.5f, 0.0f, 0.5f,   0.5f, 0.0f, -0.5f,  0.5f, 1.0f, -0.5f,  0.5f, 1.0f, 0.5f,
    -0.5f, 0.0f, -0.5f,  -0.5f, 0.0f, 0.5f,  -0.5f, 1.0f, 0.5f, -0.5f, 1.0f, -0.5f};
  static GLfloat texture_coordinates[] = {
    0.0,0.0, 1.0,0.0, 1.0,1.0, 0.0,1.0,
    0.0,0.0, 1.0,0.0, 1.0,1.0, 0.0,1.0,
    0.0,0.0, 1.0,0.0, 1.0,1.0, 0.0,1.0,
    0.0,0.0, 1.0,0.0, 1.0,1.0, 0.0,1.0 };
  static GLubyte cube_indices[24] = {
    0,1,2,3, 4,5,6,7, 3,2,5,4, 7,6,1,0,
    8,9,10,11, 12,13,14,15};

  glTexCoordPointer (2, GL_FLOAT, 0, texture_coordinates);
  ACE_ASSERT (glGetError () == GL_NO_ERROR);
  glVertexPointer (3, GL_FLOAT, 0, vertices);
  ACE_ASSERT (glGetError () == GL_NO_ERROR);
  glDrawElements (GL_QUADS, 24, GL_UNSIGNED_BYTE, cube_indices);
  ACE_ASSERT (glGetError () == GL_NO_ERROR);

//  rot_x += 0.3f;
//  rot_y += 0.20f;
//  rot_z += 0.4f;
  rotation -= 1.0f; // Decrease The Rotation Variable For The Cube

  //GLuint vertex_array_id = 0;
  //glGenVertexArrays (1, &vertex_array_id);
  //glBindVertexArray (vertex_array_id);

  //static const GLfloat vertex_buffer_data[] = {
  //  -1.0f, -1.0f, 0.0f,
  //  1.0f, -1.0f, 0.0f,
  //  -1.0f,  1.0f, 0.0f,
  //  -1.0f,  1.0f, 0.0f,
  //  1.0f, -1.0f, 0.0f,
  //  1.0f,  1.0f, 0.0f,
  //};

  //GLuint vertex_buffer;
  //glGenBuffers (1, &vertex_buffer);
  //glBindBuffer (GL_ARRAY_BUFFER, vertex_buffer);
  //glBufferData (GL_ARRAY_BUFFER,
  //              sizeof (vertex_buffer_data), vertex_buffer_data,
  //              GL_STATIC_DRAW);

  ////GLuint program_id = LoadShaders ("Passthrough.vertexshader",
  ////                                 "SimpleTexture.fragmentshader");
  ////GLuint tex_id = glGetUniformLocation (program_id, "renderedTexture");
  ////GLuint time_id = glGetUniformLocation (program_id, "time");

  //glBindFramebuffer (GL_FRAMEBUFFER, 0);
  //glViewport (0, 0,
  //            data_p->area3D.width, data_p->area3D.height);

#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
#else
  ggla_area_swap_buffers (GGLA_AREA (widget_in));
#endif
#else
#if defined (GTKGLAREA_SUPPORT)
  gtk_gl_area_swap_buffers (GTK_GL_AREA (widget_in));
#else
  gdk_gl_drawable_gl_end (drawable_p);
  gdk_gl_drawable_swap_buffers (drawable_p);
#endif
#endif

  return TRUE;
}
void
glarea_realize_cb (GtkWidget* widget_in,
                   gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::glarea_realize_cb"));

  // sanity check(s)
  ACE_ASSERT (widget_in);
  ACE_ASSERT (userData_in);

  struct ARDrone_GtkCBData_Base* data_p =
    static_cast<struct ARDrone_GtkCBData_Base*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (data_p);

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct Test_U_AudioEffect_DirectShow_GTK_CBData* directshow_data_p = NULL;
  struct Test_U_AudioEffect_MediaFoundation_GTK_CBData* mediafoundation_data_p =
    NULL;
  if (data_p->useMediaFoundation)
  {
    mediafoundation_data_p =
      static_cast<struct Test_U_AudioEffect_MediaFoundation_GTK_CBData*> (userData_in);
    // sanity check(s)
    ACE_ASSERT (mediafoundation_data_p);
    ACE_ASSERT (mediafoundation_data_p->configuration);
  } // end IF
  else
  {
    directshow_data_p =
      static_cast<struct Test_U_AudioEffect_DirectShow_GTK_CBData*> (userData_in);
    // sanity check(s)
    ACE_ASSERT (directshow_data_p);
    ACE_ASSERT (directshow_data_p->configuration);
  } // end ELSE
#else
  // sanity check(s)
  ACE_ASSERT (data_p->configuration);

#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
#else
  // sanity check(s)
  ACE_ASSERT (widget_in);
#endif
#else
#if defined (GTKGLAREA_SUPPORT)
  // sanity check(s)
  ACE_ASSERT (widget_in);
#else
  GdkGLDrawable* drawable_p =
    (*modulehandler_configuration_iterator).second.GdkWindow3D;
  GdkGLContext* context_p =
    (*modulehandler_configuration_iterator).second.OpenGLContext;

  // sanity check(s)
  ACE_ASSERT (drawable_p);
  ACE_ASSERT (context_p);
#endif
#endif
#endif

#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
#else
  if (!ggla_area_make_current (GGLA_AREA (widget_in)))
#endif
#else
#if defined (GTKGLAREA_SUPPORT)
  if (!gtk_gl_area_make_current (GTK_GL_AREA (widget_in)))
#else
  bool result = gdk_gl_drawable_make_current (drawable_p,
                                              context_p);
  if (!result)
#endif
#endif
    return;

#if GTK_CHECK_VERSION (3,0,0)
#else
#if defined (GTKGLAREA_SUPPORT)
#else
  result = gdk_gl_drawable_gl_begin (drawable_p,
                                     context_p);
  if (!result)
    return;
#endif
#endif

  GtkAllocation allocation;
  gtk_widget_get_allocation (widget_in, &allocation);

  GLuint* texture_id_p = NULL;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (data_p->useMediaFoundation)
    texture_id_p =
      &(*mediafoundation_modulehandler_configuration_iterator).second.OpenGLTextureId;
  else
    texture_id_p =
      &(*directshow_modulehandler_configuration_iterator).second.OpenGLTextureId;
#else
  texture_id_p =
    &(*modulehandler_configuration_iterator).second.OpenGLTextureId;
#endif
  ACE_ASSERT (texture_id_p);

  static GLubyte* image_p = NULL;
  if (!image_p)
  {
    std::string filename = Common_File_Tools::getWorkingDirectory ();
    filename += ACE_DIRECTORY_SEPARATOR_CHAR;
    filename += ACE_TEXT_ALWAYS_CHAR (TEST_U_STREAM_CONFIGURATION_DIRECTORY);
    filename += ACE_DIRECTORY_SEPARATOR_CHAR;
    filename +=
      ACE_TEXT_ALWAYS_CHAR (TEST_U_STREAM_AUDIOEFFECT_DEFAULT_IMAGE_FILE);
    unsigned int width, height;
    bool has_alpha = false;
    if (!Common_Image_Tools::loadPNG2OpenGL (filename,
                                             width, height,
                                             has_alpha,
                                             image_p))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Common_Image_Tools::loadPNG2OpenGL(\"%s\"): \"%m\", returning\n"),
                  ACE_TEXT (filename.c_str ())));
      return;
    } // end IF
    ACE_ASSERT (image_p);
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("loaded \"%s\"...\n"),
                ACE_TEXT (filename.c_str ())));

    glGenTextures (1, texture_id_p);
    ACE_ASSERT (glGetError () == GL_NO_ERROR);
    glBindTexture (GL_TEXTURE_2D, *texture_id_p);
    ACE_ASSERT (glGetError () == GL_NO_ERROR);
    glEnableClientState (GL_TEXTURE_COORD_ARRAY);
    ACE_ASSERT (glGetError () == GL_NO_ERROR);
    glEnableClientState (GL_VERTEX_ARRAY);
    ACE_ASSERT (glGetError () == GL_NO_ERROR);
    glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    ACE_ASSERT (glGetError () == GL_NO_ERROR);
    // select modulate to mix texture with color for shading
//    glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
//    ACE_ASSERT (glGetError () == GL_NO_ERROR);

#if defined (GL_VERSION_1_1)
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); // GL_CLAMP_TO_EDGE
#else
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
#endif
    ACE_ASSERT (glGetError () == GL_NO_ERROR);
#if defined (GL_VERSION_1_1)
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP); // GL_CLAMP_TO_EDGE
#else
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#endif
    ACE_ASSERT (glGetError () == GL_NO_ERROR);
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA,
                  width, height,
                  0, (has_alpha ? GL_RGBA : GL_RGB),
                  GL_UNSIGNED_BYTE, image_p);
    ACE_ASSERT (glGetError () == GL_NO_ERROR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    ACE_ASSERT (glGetError () == GL_NO_ERROR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    ACE_ASSERT (glGetError () == GL_NO_ERROR);
#if defined (GL_VERSION_1_4)
    glTexParameteri (GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    ACE_ASSERT (glGetError () == GL_NO_ERROR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    ACE_ASSERT (glGetError () == GL_NO_ERROR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 1000);
    ACE_ASSERT (glGetError () == GL_NO_ERROR);
#endif

    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("\"%s\" --> OpenGL texture ID: %u...\n"),
                ACE::basename (ACE_TEXT (filename.c_str ()), ACE_DIRECTORY_SEPARATOR_CHAR),
                *texture_id_p));

    // clean up (do NOT reset the pointer)
    free (image_p);
  } // end IF

  glViewport (0, 0,
              allocation.width, allocation.height);
  ACE_ASSERT (glGetError () == GL_NO_ERROR);

  glMatrixMode (GL_PROJECTION);
  ACE_ASSERT (glGetError () == GL_NO_ERROR);
////  glOrtho (0.0, allocation.width,
////           0.0, allocation.height,
////           -1.0, 1.0);
////  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();
  ACE_ASSERT (glGetError () == GL_NO_ERROR);
  gluPerspective (45.0,
                  (allocation.width / (GLdouble)allocation.height),
                  0.1,
                  100.0); // setup a perspective projection
  ACE_ASSERT (glGetError () == GL_NO_ERROR);
//  GLdouble fW, fH;
//  fH =
//   ::tan (60.0 / 360.0 * M_PI) *
//   -1.0;
//  fW = fH * (allocation.width / allocation.height);
//  glFrustum (-fW, fW,
//             -fH, fH,
//             -1.0,
//             100.0);
//  gluLookAt (-10.0, 0.0, 0.0, // eye position (*NOTE*: relative to standard
//             //                       "right-hand" coordinate
//             //                       system [RHCS])
//             0.0, 0.0, 0.0,   // looking-at position (RHCS notation)
//             0.0, 0.0, -1.0); // up direction (RHCS notation, relative to eye
  // position and looking-at direction)
  glMatrixMode (GL_MODELVIEW);
  ACE_ASSERT (glGetError () == GL_NO_ERROR);

  /* light */
//  GLfloat light_positions[2][4]   = { 50.0, 50.0, 0.0, 0.0,
//                                     -50.0, 50.0, 0.0, 0.0 };
//  GLfloat light_colors[2][4] = { .6, .6,  .6, 1.0,   /* white light */
//                                 .4, .4, 1.0, 1.0 }; /* cold blue light */
//  glLightfv (GL_LIGHT0, GL_POSITION, light_positions[0]);
//  glLightfv (GL_LIGHT0, GL_DIFFUSE,  light_colors[0]);
//  glLightfv (GL_LIGHT1, GL_POSITION, light_positions[1]);
//  glLightfv (GL_LIGHT1, GL_DIFFUSE,  light_colors[1]);
//  glEnable (GL_LIGHT0);
//  glEnable (GL_LIGHT1);
//  glEnable (GL_LIGHTING);

  // set up light colors (ambient, diffuse, specular)
  GLfloat light_ambient[] = {1.0F, 1.0F, 1.0F, 1.0F};
  glLightfv (GL_LIGHT0, GL_AMBIENT, light_ambient);
  ACE_ASSERT (glGetError () == GL_NO_ERROR);
  GLfloat light_diffuse[] = {0.3F, 0.3F, 0.3F, 1.0F};
  glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  ACE_ASSERT (glGetError () == GL_NO_ERROR);
  GLfloat light_specular[] = {1.0F, 1.0F, 1.0F, 1.0F};
  glLightfv (GL_LIGHT0, GL_SPECULAR, light_specular);
  ACE_ASSERT (glGetError () == GL_NO_ERROR);
  // position the light in eye space
  GLfloat light0_position[] = {0.0F,
                               5.0F * 2,
                               5.0F * 2,
                               0.0F}; // --> directional light
  glLightfv (GL_LIGHT0, GL_POSITION, light0_position);
  ACE_ASSERT (glGetError () == GL_NO_ERROR);
  glEnable (GL_LIGHT0);
  ACE_ASSERT (glGetError () == GL_NO_ERROR);

#if GTK_CHECK_VERSION (3,0,0)
#else
#if defined (GTKGLAREA_SUPPORT)
#else
  gdk_gl_drawable_gl_end (drawable_p);
#endif
#endif
} // glarea_realize_cb
#else
gboolean
drawingarea_3d_expose_event_cb (GtkWidget* widget_in,
                                cairo_t* context_in,
                                gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::drawingarea_3d_expose_event_cb"));

  ACE_UNUSED_ARG (context_in);

  // sanity check(s)
  ACE_ASSERT (widget_in);
  //ACE_ASSERT (context_in);
  ACE_ASSERT (userData_in);

  struct ARDrone_GtkCBData_Base* data_p =
    static_cast<struct ARDrone_GtkCBData_Base*> (userData_in);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct Test_U_AudioEffect_DirectShow_GTK_CBData* directshow_data_p = NULL;
  struct Test_U_AudioEffect_MediaFoundation_GTK_CBData* mediafoundation_data_p =
    NULL;
  if (data_p->useMediaFoundation)
  {
    mediafoundation_data_p =
      static_cast<struct Test_U_AudioEffect_MediaFoundation_GTK_CBData*> (userData_in);
    // sanity check(s)
    ACE_ASSERT (mediafoundation_data_p);
  } // end IF
  else
  {
    directshow_data_p =
      static_cast<struct Test_U_AudioEffect_DirectShow_GTK_CBData*> (userData_in);
    // sanity check(s)
    ACE_ASSERT (directshow_data_p);
  } // end ELSE
#else
  // sanity check(s)
  ACE_ASSERT (data_p);
#endif

  GLuint texture_id = 0;
#if defined (GTKGL_SUPPORT)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  if (data_p->useMediaFoundation)
    texture_id =
    (*mediafoundation_modulehandler_configuration_iterator).second.OpenGLTextureId;
  else
    texture_id =
    (*directshow_modulehandler_configuration_iterator).second.OpenGLTextureId;
#else
  texture_id =
    (*modulehandler_configuration_iterator).second.OpenGLTextureId;
#endif
  // sanity check(s)
  if (texture_id == 0)
    return FALSE; // --> still waiting for the first frame
#endif

  GdkWindow* window_p = gtk_widget_get_window (widget_in);
  ACE_ASSERT (window_p);
#if defined (GTKGL_SUPPORT)
#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
  gdk_cairo_draw_from_gl (context_in,
                          window_p,
                          texture_id,
                          GL_TEXTURE,
                          1,
                          0, 0,
                          data_p->area3D.width, data_p->area3D.height);
#else
#endif
#else
#if defined (GTKGLAREA_SUPPORT)
#else
  GdkGLDrawable* gl_drawable_p = gtk_widget_get_gl_drawable (widget_in);
  ACE_ASSERT (gl_drawable_p);
  GdkGLContext* gl_context_p = gtk_widget_get_gl_context (widget_in);
  ACE_ASSERT (gl_context_p);

  gdk_gl_drawable_gl_begin (gl_drawable_p, gl_context_p);

//  gdk_gl_drawable_swap_buffers (gl_drawable_p);
  gdk_gl_drawable_gl_end (gl_drawable_p);
#endif
#endif
#endif

#if GTK_CHECK_VERSION (3,0,0)
#if GTK_CHECK_VERSION (3,16,0)
  {
//    ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, data_p->cairoSurfaceLock, FALSE);

    cairo_paint (context_in);
  } // end lock scope
#endif
#endif

  return TRUE;
} // drawingarea_3d_draw_cb
#endif
#endif
#endif
// ---------------------------------------

void
toggleaction_video_toggled_cb (GtkToggleAction* toggleAction_in,
                               gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::toggleaction_video_toggled_cb"));

  struct ARDrone_GtkCBData_Base* cb_data_p =
      static_cast<struct ARDrone_GtkCBData_Base*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (toggleAction_in);
  ACE_ASSERT (cb_data_p);

  cb_data_p->enableVideo = gtk_toggle_action_get_active (toggleAction_in);

  Common_UI_GTK_BuildersIterator_t iterator =
    cb_data_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != cb_data_p->builders.end ());

  GtkFrame* frame_p =
    GTK_FRAME (gtk_builder_get_object ((*iterator).second.second,
                                       ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_FRAME_DISPLAY)));
  ACE_ASSERT (frame_p);
  gtk_widget_set_sensitive (GTK_WIDGET (frame_p),
                            cb_data_p->enableVideo);

  GtkToggleAction* toggle_action_p =
    GTK_TOGGLE_ACTION (gtk_builder_get_object ((*iterator).second.second,
                                               ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_TOGGLEACTION_FULLSCREEN)));
  ACE_ASSERT (toggle_action_p);
//  gtk_widget_set_sensitive (GTK_WIDGET (toggle_action_p),
//                            cb_data_p->enableVideo);
  GtkWidget* widget_p =
    GTK_WIDGET (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_CHECKBUTTON_SAVE)));
  ACE_ASSERT (widget_p);
  gtk_widget_set_sensitive (widget_p,
                            cb_data_p->enableVideo);
}

void
toggleaction_fullscreen_toggled_cb (GtkToggleAction* toggleAction_in,
                                    gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::toggleaction_fullscreen_toggled_cb"));

  struct ARDrone_GtkCBData_Base* cb_data_p =
      static_cast<struct ARDrone_GtkCBData_Base*> (userData_in);

  // sanity check(s)
  ACE_UNUSED_ARG (toggleAction_in);
  ACE_ASSERT (cb_data_p);

  Common_UI_GTK_BuildersIterator_t iterator =
    cb_data_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != cb_data_p->builders.end ());

  // sanity check(s)
  Stream_IStreamControlBase* istream_base_p = NULL;
  ARDrone_StreamsIterator_t streams_iterator =
      cb_data_p->streams.find (video_stream_name_string_);
  ACE_ASSERT (streams_iterator != cb_data_p->streams.end ());
  istream_base_p =
      dynamic_cast<Stream_IStreamControlBase*> ((*streams_iterator).second);
  ACE_ASSERT (istream_base_p);
  if (!cb_data_p->enableVideo ||
      !istream_base_p->isRunning ())
    return;

  Stream_IStream_t* istream_p =
      dynamic_cast<Stream_IStream_t*> (istream_base_p);
  ACE_ASSERT (istream_p);
  const Stream_Module_t* module_p = NULL;
  std::string stream_name_string;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  switch (cb_data_p->mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      module_p =
          istream_p->find (ACE_TEXT_ALWAYS_CHAR (MODULE_VIS_DIRECTSHOW_DEFAULT_NAME_STRING));
      stream_name_string = istream_p->name ();
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      module_p =
          istream_p->find (ACE_TEXT_ALWAYS_CHAR (MODULE_VIS_MEDIAFOUNDATION_DEFAULT_NAME_STRING));
      stream_name_string = istream_p->name ();
      break;
    } // end ELSE
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  cb_data_p->mediaFramework));
      return;
    }
  } // end SWITCH
#else
  module_p =
      istream_p->find (ACE_TEXT_ALWAYS_CHAR (MODULE_VIS_GTK_PIXBUF_DEFAULT_NAME_STRING));
  stream_name_string = istream_p->name ();
#endif
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_IStream::find(\"Display\"), returning\n"),
                ACE_TEXT (stream_name_string.c_str ())));
    return;
  } // end IF
  Stream_Module_Visualization_IFullscreen* ifullscreen_p =
    dynamic_cast<Stream_Module_Visualization_IFullscreen*> (const_cast<Stream_Module_t*> (module_p)->writer ());
  if (!ifullscreen_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("Display: failed to dynamic_cast<Stream_Module_Visualization_IFullscreen*>(0x%@), returning\n"),
                const_cast<Stream_Module_t*> (module_p)->writer ()));
    return;
  } // end IF
  try {
    ifullscreen_p->toggle ();
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("Display: failed to Stream_Module_Visualization_IFullscreen::toggle(), returning\n")));
    return;
  }
}

void
toggleaction_save_toggled_cb (GtkToggleAction* toggleAction_in,
                              gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::toggleaction_save_toggled_cb"));

  struct ARDrone_GtkCBData_Base* cb_data_base_p =
      static_cast<struct ARDrone_GtkCBData_Base*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (toggleAction_in);
  ACE_ASSERT (cb_data_base_p);
  if (!cb_data_base_p->enableVideo)
    return;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct ARDrone_DirectShow_GtkCBData* directshow_cb_data_p =
    NULL;
  struct ARDrone_DirectShow_Configuration* directshow_configuration_p =
    NULL;
  struct ARDrone_MediaFoundation_GtkCBData* mediafoundation_cb_data_p =
    NULL;
  struct ARDrone_MediaFoundation_Configuration* mediafoundation_configuration_p =
    NULL;
  switch (cb_data_base_p->mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      directshow_cb_data_p =
        static_cast<struct ARDrone_DirectShow_GtkCBData*> (cb_data_base_p);
      directshow_configuration_p = directshow_cb_data_p->configuration;
      ACE_ASSERT (directshow_configuration_p);
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      mediafoundation_cb_data_p =
        static_cast<struct ARDrone_MediaFoundation_GtkCBData*> (cb_data_base_p);
      mediafoundation_configuration_p =
        mediafoundation_cb_data_p->configuration;
      ACE_ASSERT (mediafoundation_configuration_p);
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), aborting\n"),
                  cb_data_base_p->mediaFramework));
      break;
    }
  } // end SWITCH
#else
  struct ARDrone_GtkCBData* cb_data_p =
    static_cast<struct ARDrone_GtkCBData*> (cb_data_base_p);
  struct ARDrone_Configuration* configuration_p = cb_data_p->configuration;
  ACE_ASSERT (configuration_p);
//  wlan_monitor_configuration_p = &configuration_p->WLANMonitorConfiguration;
#endif // ACE_WIN32 || ACE_WIN64

  Common_UI_GTK_BuildersIterator_t iterator =
    cb_data_base_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != cb_data_base_p->builders.end ());

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ARDrone_DirectShow_StreamConfigurationsIterator_t directshow_stream_configurations_iterator;
  ARDrone_DirectShow_StreamConfiguration_t::ITERATOR_T directshow_stream_configuration_iterator;
  ARDrone_MediaFoundation_StreamConfigurationsIterator_t mediafoundation_stream_configurations_iterator;
  ARDrone_MediaFoundation_StreamConfiguration_t::ITERATOR_T mediafoundation_stream_configuration_iterator;
  switch (cb_data_base_p->mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      directshow_stream_configurations_iterator =
        directshow_configuration_p->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
      ACE_ASSERT (directshow_stream_configurations_iterator != directshow_configuration_p->streamConfigurations.end ());
      directshow_stream_configuration_iterator =
        (*directshow_stream_configurations_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (""));
      ACE_ASSERT (directshow_stream_configuration_iterator != (*directshow_stream_configurations_iterator).second.end ());
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      mediafoundation_stream_configurations_iterator =
        mediafoundation_configuration_p->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
      ACE_ASSERT (mediafoundation_stream_configurations_iterator != mediafoundation_configuration_p->streamConfigurations.end ());
      mediafoundation_stream_configuration_iterator =
        (*mediafoundation_stream_configurations_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (""));
      ACE_ASSERT (mediafoundation_stream_configuration_iterator != (*mediafoundation_stream_configurations_iterator).second.end ());
      break;
    } // end ELSE
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  cb_data_base_p->mediaFramework));
      return;
    }
  } // end SWITCH
#else
  ARDrone_StreamConfigurationsIterator_t stream_configurations_iterator =
      configuration_p->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
  ACE_ASSERT (stream_configurations_iterator != configuration_p->streamConfigurations.end ());
  ARDrone_StreamConfiguration_t::ITERATOR_T stream_configuration_iterator =
    (*stream_configurations_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (stream_configuration_iterator != (*stream_configurations_iterator).second.end ());
#endif

  bool save_streams = false;
  GtkFileChooserButton* file_chooser_button_p = NULL;
  gchar* URI_p = NULL;
  gchar* directory_p = NULL;
  gchar* hostname_p = NULL;
  GError* error_p = NULL;

  save_streams = gtk_toggle_action_get_active (toggleAction_in);
  if (!save_streams)
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    switch (cb_data_base_p->mediaFramework)
    {
      case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
        (*directshow_stream_configuration_iterator).second.second.targetFileName.clear ();
        break;
      case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
        (*mediafoundation_stream_configuration_iterator).second.second.targetFileName.clear ();
        break;
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown media framework (was: %d), continuing\n"),
                    cb_data_base_p->mediaFramework));
        break;
      }
    } // end SWITCH
#else
    (*stream_configuration_iterator).second.second.targetFileName.clear ();
#endif
    goto continue_;
  } // end IF

  file_chooser_button_p =
      GTK_FILE_CHOOSER_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                       ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_FILECHOOSERBUTTON_SAVE)));
  ACE_ASSERT (file_chooser_button_p);
  URI_p = gtk_file_chooser_get_uri (GTK_FILE_CHOOSER (file_chooser_button_p));
  if (!URI_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_file_chooser_get_uri(), returning\n")));
    goto continue_;
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

    goto continue_;
  } // end IF
  ACE_ASSERT (!hostname_p);

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  switch (cb_data_base_p->mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      (*directshow_stream_configuration_iterator).second.second.targetFileName =
          directory_p;
      ACE_ASSERT (Common_File_Tools::isDirectory ((*directshow_stream_configuration_iterator).second.second.targetFileName));
      (*directshow_stream_configuration_iterator).second.second.targetFileName +=
          ACE_DIRECTORY_SEPARATOR_STR;
      (*directshow_stream_configuration_iterator).second.second.targetFileName +=
        ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_FILE_NAME);
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      (*mediafoundation_stream_configuration_iterator).second.second.targetFileName =
          directory_p;
      ACE_ASSERT (Common_File_Tools::isDirectory ((*mediafoundation_stream_configuration_iterator).second.second.targetFileName));
      (*mediafoundation_stream_configuration_iterator).second.second.targetFileName +=
          ACE_DIRECTORY_SEPARATOR_STR;
      (*mediafoundation_stream_configuration_iterator).second.second.targetFileName +=
        ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_FILE_NAME);
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  cb_data_base_p->mediaFramework));
      g_free (directory_p);
      goto continue_;
    }
  } // end SWITCH
#else
  (*stream_configuration_iterator).second.second.targetFileName = directory_p;
  ACE_ASSERT (Common_File_Tools::isDirectory ((*stream_configuration_iterator).second.second.targetFileName));
  (*stream_configuration_iterator).second.second.targetFileName +=
      ACE_DIRECTORY_SEPARATOR_STR;
  (*stream_configuration_iterator).second.second.targetFileName +=
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_FILE_NAME);
#endif
  g_free (directory_p);

continue_:
  GtkFrame* frame_p =
    GTK_FRAME (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_FRAME_OPTIONS_SAVE)));
  ACE_ASSERT (frame_p);
  gtk_widget_set_sensitive (GTK_WIDGET (frame_p),
                            save_streams);
}

void
action_cut_activate_cb (GtkAction* action_in,
                        gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::action_cut_activate_cb"));

  struct ARDrone_GtkCBData_Base* cb_data_p =
    static_cast<struct ARDrone_GtkCBData_Base*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (cb_data_p);
}

void
toggleaction_associate_toggled_cb (GtkToggleAction* toggleAction_in,
                                   gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::toggleaction_associate_toggled_cb"));

  struct ARDrone_GtkCBData_Base* cb_data_base_p =
      static_cast<struct ARDrone_GtkCBData_Base*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (toggleAction_in);
  ACE_ASSERT (cb_data_base_p);
  struct ARDrone_WLANMonitorConfiguration* wlan_monitor_configuration_p = NULL;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct ARDrone_DirectShow_GtkCBData* directshow_cb_data_p =
    NULL;
  struct ARDrone_DirectShow_Configuration* directshow_configuration_p =
    NULL;
  struct ARDrone_MediaFoundation_GtkCBData* mediafoundation_cb_data_p =
    NULL;
  struct ARDrone_MediaFoundation_Configuration* mediafoundation_configuration_p =
    NULL;
  switch (cb_data_base_p->mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      directshow_cb_data_p =
        static_cast<struct ARDrone_DirectShow_GtkCBData*> (cb_data_base_p);
      directshow_configuration_p = directshow_cb_data_p->configuration;
      ACE_ASSERT (directshow_configuration_p);
      wlan_monitor_configuration_p =
        &directshow_configuration_p->WLANMonitorConfiguration;
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      mediafoundation_cb_data_p =
        static_cast<struct ARDrone_MediaFoundation_GtkCBData*> (cb_data_base_p);
      mediafoundation_configuration_p =
        mediafoundation_cb_data_p->configuration;
      ACE_ASSERT (mediafoundation_configuration_p);
      wlan_monitor_configuration_p =
        &mediafoundation_configuration_p->WLANMonitorConfiguration;
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), aborting\n"),
                  cb_data_base_p->mediaFramework));
      break;
    }
  } // end SWITCH
#else
  struct ARDrone_GtkCBData* cb_data_p =
    static_cast<struct ARDrone_GtkCBData*> (cb_data_base_p);
  struct ARDrone_Configuration* configuration_p = cb_data_p->configuration;
  ACE_ASSERT (configuration_p);
  wlan_monitor_configuration_p = &configuration_p->WLANMonitorConfiguration;
#endif // ACE_WIN32 || ACE_WIN64
  ACE_ASSERT (wlan_monitor_configuration_p);

  Common_UI_GTK_BuildersIterator_t iterator =
    cb_data_base_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != cb_data_base_p->builders.end ());

  GtkSpinner* spinner_p =
    GTK_SPINNER (gtk_builder_get_object ((*iterator).second.second,
                                         ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_SPINNER)));
  ACE_ASSERT (spinner_p);

  // update configuration
  wlan_monitor_configuration_p->autoAssociate =
    gtk_toggle_action_get_active (toggleAction_in);

  ARDrone_WLANMonitor_t* WLAN_monitor_p =
    ARDRONE_WLANMONITOR_SINGLETON::instance ();
  ACE_ASSERT (WLAN_monitor_p);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  struct ether_addr ap_mac_address;
  ACE_OS::memset (&ap_mac_address, 0, sizeof (struct ether_addr));
#endif // ACE_WIN32 || ACE_WIN64

  if (ACE_OS::strcmp (WLAN_monitor_p->SSID ().c_str (),
                      wlan_monitor_configuration_p->SSID.c_str ()) &&
      wlan_monitor_configuration_p->autoAssociate)
  {
    gtk_spinner_start (spinner_p);
    gtk_widget_set_sensitive (GTK_WIDGET (spinner_p),
                              TRUE);
    gtk_widget_set_visible (GTK_WIDGET (spinner_p),
                            TRUE);

    if (!WLAN_monitor_p->associate (wlan_monitor_configuration_p->interfaceIdentifier,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
                                    ap_mac_address,
#endif // ACE_WIN32 || ACE_WIN64
                                    wlan_monitor_configuration_p->SSID))
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Net_IWLANMonitor_T::associate(\"%s\",%s), returning\n"),
                  ACE_TEXT (Net_Common_Tools::interfaceToString (wlan_monitor_configuration_p->interfaceIdentifier).c_str ()),
                  ACE_TEXT (wlan_monitor_configuration_p->SSID.c_str ())));
#else
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Net_IWLANMonitor_T::associate(\"%s\",%s,%s), returning\n"),
                  ACE_TEXT (wlan_monitor_configuration_p->interfaceIdentifier.c_str ()),
                  ACE_TEXT (Net_Common_Tools::LinkLayerAddressToString (reinterpret_cast<unsigned char*> (&ap_mac_address)).c_str ()),
                  ACE_TEXT (wlan_monitor_configuration_p->SSID.c_str ())));
#endif // ACE_WIN32 || ACE_WIN64
    } // end IF

    return;
  } // end IF

  gtk_widget_set_sensitive (GTK_WIDGET (spinner_p),
                            FALSE);
  gtk_widget_set_visible (GTK_WIDGET (spinner_p),
                          FALSE);
}

//------------------------------------------------------------------------------

#if GTK_CHECK_VERSION (3,0,0)
void
drawingarea_video_size_allocate_cb (GtkWidget* widget_in,
                                    GdkRectangle* allocation_in,
                                    gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::drawingarea_video_size_allocate_cb"));

  // sanity check(s)
  ACE_ASSERT (widget_in);
  ACE_ASSERT (allocation_in);
  ACE_ASSERT (userData_in);

  struct ARDrone_GtkCBData_Base* cb_data_base_p =
      reinterpret_cast<struct ARDrone_GtkCBData_Base*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (cb_data_base_p);

  Common_UI_GTK_BuildersIterator_t iterator =
    cb_data_base_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != cb_data_base_p->builders.end ());

  GtkToggleAction* toggle_action_p =
      GTK_TOGGLE_ACTION (gtk_builder_get_object ((*iterator).second.second,
                                                 ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_TOGGLEACTION_FULLSCREEN)));
  ACE_ASSERT (toggle_action_p);
  GdkWindow* window_p = gtk_widget_get_window (widget_in);

  // sanity check(s)
  if (!window_p)
    return; // window is not (yet) realized, nothing to do
  if (!gdk_window_is_viewable (window_p))
    return; // window is not (yet) mapped, nothing to do
  if (gtk_toggle_action_get_active (toggle_action_p) &&
      ACE_OS::strcmp (gtk_buildable_get_name (GTK_BUILDABLE (widget_in)),
                      ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_DRAWINGAREA_FULLSCREEN)))
    return; // use the fullscreen window, not the applications'

  // sanity check(s)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct ARDrone_DirectShow_GtkCBData* directshow_cb_data_p =
    NULL;
  struct ARDrone_DirectShow_Configuration* directshow_configuration_p =
    NULL;
  struct ARDrone_MediaFoundation_GtkCBData* mediafoundation_cb_data_p =
    NULL;
  struct ARDrone_MediaFoundation_Configuration* mediafoundation_configuration_p =
    NULL;
  switch (cb_data_base_p->mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      directshow_cb_data_p =
        static_cast<struct ARDrone_DirectShow_GtkCBData*> (cb_data_base_p);
      directshow_configuration_p = directshow_cb_data_p->configuration;
      ACE_ASSERT (directshow_configuration_p);
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      mediafoundation_cb_data_p =
        static_cast<struct ARDrone_MediaFoundation_GtkCBData*> (cb_data_base_p);
      mediafoundation_configuration_p =
        mediafoundation_cb_data_p->configuration;
      ACE_ASSERT (mediafoundation_configuration_p);
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), aborting\n"),
                  cb_data_base_p->mediaFramework));
      break;
    }
  } // end SWITCH
#else
  struct ARDrone_GtkCBData* cb_data_p =
    static_cast<struct ARDrone_GtkCBData*> (cb_data_base_p);
  struct ARDrone_Configuration* configuration_p = cb_data_p->configuration;
  ACE_ASSERT (configuration_p);
//  wlan_monitor_configuration_p = &configuration_p->WLANMonitorConfiguration;
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ARDrone_DirectShow_StreamConfigurationsIterator_t directshow_iterator_2;
  ARDrone_DirectShow_StreamConfiguration_t::ITERATOR_T directshow_iterator_3;
  ARDrone_MediaFoundation_StreamConfigurationsIterator_t mediafoundation_iterator_2;
  ARDrone_MediaFoundation_StreamConfiguration_t::ITERATOR_T mediafoundation_iterator_3;
  switch (cb_data_base_p->mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      directshow_iterator_2 =
        directshow_configuration_p->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
      ACE_ASSERT (directshow_iterator_2 != directshow_configuration_p->streamConfigurations.end ());
      directshow_iterator_3 =
        (*directshow_iterator_2).second.find (ACE_TEXT_ALWAYS_CHAR (""));
      ACE_ASSERT (directshow_iterator_3 != (*directshow_iterator_2).second.end ());
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      mediafoundation_iterator_2 =
        mediafoundation_configuration_p->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
      ACE_ASSERT (mediafoundation_iterator_2 != mediafoundation_configuration_p->streamConfigurations.end ());
      mediafoundation_iterator_3 =
        (*mediafoundation_iterator_2).second.find (ACE_TEXT_ALWAYS_CHAR (""));
      ACE_ASSERT (mediafoundation_iterator_3 != (*mediafoundation_iterator_2).second.end ());
      break;
    } // end ELSE
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  cb_data_base_p->mediaFramework));
      return;
    }
  } // end SWITCH
#else
  ARDrone_StreamConfigurationsIterator_t iterator_2 =
      configuration_p->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
  ACE_ASSERT (iterator_2 != configuration_p->streamConfigurations.end ());
  ARDrone_StreamConfiguration_t::ITERATOR_T iterator_3 =
    (*iterator_2).second.find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (iterator_3 != (*iterator_2).second.end ());
#endif

  // *NOTE*: x,y members are relative to the parent window
  //         --> no need to translate
  //  gdk_window_get_origin (gtk_widget_get_window (widget_in),
  //                         &(*iterator_2).second.area.x,
  //                         &(*iterator_2).second.area.y);
  //  gtk_widget_translate_coordinates (widget_in,
  //                                    gtk_widget_get_toplevel (widget_in),
  //                                    0, 0,
  //                                    &(*iterator_2).second.area.x,
  //                                    &(*iterator_2).second.area.y);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  switch (cb_data_base_p->mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
      (*directshow_iterator_3).second.second.area.left = allocation_in->x;
      (*directshow_iterator_3).second.second.area.right =
        allocation_in->x + allocation_in->width;
      (*directshow_iterator_3).second.second.area.top = allocation_in->y;
      (*directshow_iterator_3).second.second.area.bottom =
        allocation_in->y + allocation_in->height;
      break;
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
      (*mediafoundation_iterator_3).second.second.area.left = allocation_in->x;
      (*mediafoundation_iterator_3).second.second.area.right =
        allocation_in->x + allocation_in->width;
      (*mediafoundation_iterator_3).second.second.area.top = allocation_in->y;
      (*mediafoundation_iterator_3).second.second.area.bottom =
        allocation_in->y + allocation_in->height;
      break;
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  cb_data_base_p->mediaFramework));
      return;
    }
  } // end SWITCH
#else
  (*iterator_3).second.second.area = *allocation_in;
#endif

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#if GTK_CHECK_VERSION (3,0,0)
#else
  GdkPixbuf* pixbuf_p =
      gdk_pixbuf_new (GDK_COLORSPACE_RGB,
                      TRUE,
                      8,
                      (*iterator_3).second.area.width, (*iterator_3).second.area.height);
  if (!pixbuf_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gdk_pixbuf_new(), returning\n")));
    return;
  } // end IF
#endif

  ACE_ASSERT (&cb_data_p->lock == (*iterator_3).second.second.pixelBufferLock);
  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, cb_data_p->lock);
    if (cb_data_p->pixelBuffer)
    {
      g_object_unref (cb_data_p->pixelBuffer);
      cb_data_p->pixelBuffer = NULL;
    } // end IF
//    (*iterator_3).second.second.pixelBuffer = NULL;
    cb_data_p->pixelBuffer =
#if GTK_CHECK_VERSION (3,0,0)
        gdk_pixbuf_get_from_window (window_p,
                                    0, 0,
                                    allocation_in->width, allocation_in->height);
#else
        gdk_pixbuf_get_from_drawable (pixbuf_p,
                                      GDK_DRAWABLE (window_p),
                                      NULL,
                                      0, 0,
                                      0, 0, allocation_in->width, allocation_in->height);
#endif
    if (!cb_data_p->pixelBuffer)
    {
#if GTK_CHECK_VERSION (3,0,0)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to gdk_pixbuf_get_from_window(%@), returning\n"),
                  window_p));
#else
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to gdk_pixbuf_get_from_drawable(%@), returning\n"),
                  GDK_DRAWABLE (window_p)));

      gdk_pixbuf_unref (pixbuf_p);
#endif
      return;
    } // end IF

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

    // sanity check(s)
    ACE_ASSERT (gdk_pixbuf_get_colorspace (cb_data_p->pixelBuffer) == GDK_COLORSPACE_RGB);
    ACE_ASSERT (gdk_pixbuf_get_bits_per_sample (cb_data_p->pixelBuffer) == 8);

    if (!gdk_pixbuf_get_has_alpha (cb_data_p->pixelBuffer))
    { ACE_ASSERT (gdk_pixbuf_get_n_channels (cb_data_p->pixelBuffer) == 3);
      GdkPixbuf* pixbuf_p =
          gdk_pixbuf_add_alpha (cb_data_p->pixelBuffer,
                                FALSE, 0, 0, 0);
      ACE_ASSERT (pixbuf_p);
      gdk_pixbuf_unref (cb_data_p->pixelBuffer);
      cb_data_p->pixelBuffer = pixbuf_p;
    } // end IF
    // sanity check(s)
    ACE_ASSERT (gdk_pixbuf_get_has_alpha (cb_data_p->pixelBuffer));
    ACE_ASSERT (gdk_pixbuf_get_n_channels (cb_data_p->pixelBuffer) == 4);

    (*iterator_3).second.second.pixelBuffer = cb_data_p->pixelBuffer;
  } // end lock scope
#endif
}
#else
gboolean
drawingarea_video_configure_cb (GtkWidget* widget_in,
                                GdkEvent* event_in,
                                gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::drawingarea_video_configure_cb"));

  // sanity check(s)
  ACE_ASSERT (widget_in);
  ACE_ASSERT (event_in);
  ACE_ASSERT (userData_in);

  struct ARDrone_GtkCBData_Base* cb_data_p =
      reinterpret_cast<struct ARDrone_GtkCBData_Base*> (userData_in);

  // sanity check(s)
#if defined (GTK3_SUPPORT)
  ACE_ASSERT (gdk_event_get_event_type (event_in) == GDK_CONFIGURE);
#else
  if (event_in->type != GDK_CONFIGURE)
    return FALSE;
#endif
  ACE_ASSERT (cb_data_p);
  ACE_ASSERT (cb_data_p->configuration);

  ARDrone_StreamConfigurationsIterator_t iterator =
      cb_data_p->configuration->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
  ACE_ASSERT (iterator != cb_data_p->configuration->streamConfigurations.end ());
  ARDrone_StreamConfiguration_t::ITERATOR_T iterator_2 =
    (*iterator).second.find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (iterator_2 != (*iterator).second.end ());

  GdkWindow* window_p = gtk_widget_get_window (widget_in);
  if (!window_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_widget_get_window(%@), aborting\n"),
                widget_in));
    return TRUE;
  } // end IF

  // sanity check(s)
  if (!gdk_window_is_viewable (window_p))
    return TRUE; // window is not (yet) mapped, nothing to do

  // *NOTE*: x,y members are relative to the parent window
  //         --> no need to translate
  //  gdk_window_get_origin (gtk_widget_get_window (widget_in),
  //                         &(*iterator_2).second.area.x,
  //                         &(*iterator_2).second.area.y);
  //  gtk_widget_translate_coordinates (widget_in,
  //                                    gtk_widget_get_toplevel (widget_in),
  //                                    0, 0,
  //                                    &(*iterator_2).second.area.x,
  //                                    &(*iterator_2).second.area.y);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  (*iterator_2).second.area.left = event_in->configure.x;
  (*iterator_2).second.area.right =
    event_in->configure.x + event_in->configure.width;
  (*iterator_2).second.area.top = event_in->configure.y;
  (*iterator_2).second.area.bottom =
    event_in->configure.y + event_in->configure.height;
#else
  (*iterator_2).second.area.x = event_in->configure.x;
  (*iterator_2).second.area.y = event_in->configure.y;
  (*iterator_2).second.area.height = event_in->configure.height;
  (*iterator_2).second.area.width = event_in->configure.width;
#endif

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#if GTK_CHECK_VERSION (3,0,0)
#else
  GdkPixbuf* pixbuf_p =
      gdk_pixbuf_new (GDK_COLORSPACE_RGB,
                      TRUE,
                      8,
                      (*iterator_2).second.area.width, (*iterator_2).second.area.height);
  if (!pixbuf_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gdk_pixbuf_new(), aborting\n")));
    return TRUE;
  } // end IF
#endif

  { ACE_ASSERT (&cb_data_p->lock == (*iterator_2).second.pixelBufferLock);
    ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, cb_data_p->lock, TRUE);
    if (cb_data_p->pixelBuffer)
    {
      g_object_unref (cb_data_p->pixelBuffer);
      cb_data_p->pixelBuffer = NULL;
    } // end IF
//    (*iterator_2).second.pixelBuffer = NULL;
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
    {
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
      return TRUE;
    } // end IF
    (*iterator_2).second.pixelBuffer = cb_data_p->pixelBuffer;

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
#endif
gboolean
drawingarea_video_draw_cb (GtkWidget* widget_in,
                           cairo_t* context_in,
                           gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::drawingarea_video_draw_cb"));

  struct ARDrone_GtkCBData_Base* cb_data_p =
      reinterpret_cast<struct ARDrone_GtkCBData_Base*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (cb_data_p);

  Common_UI_GTK_BuildersIterator_t iterator =
    cb_data_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != cb_data_p->builders.end ());

  GtkToggleAction* toggle_action_p =
      GTK_TOGGLE_ACTION (gtk_builder_get_object ((*iterator).second.second,
                                                 ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_TOGGLEACTION_FULLSCREEN)));
  ACE_ASSERT (toggle_action_p);

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  if (!cb_data_p->pixelBuffer)
    return TRUE; // --> widget has not been realized yet
  if (gtk_toggle_action_get_active (toggle_action_p) &&
      ACE_OS::strcmp (gtk_buildable_get_name (GTK_BUILDABLE (widget_in)),
                      ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_DRAWINGAREA_FULLSCREEN)))
    return TRUE; // use the fullscreen window, not the applications'

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
void
drawingarea_video_realize_cb (GtkWidget* widget_in,
                              gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::drawingarea_video_realize_cb"));

  // sanity check(s)
  ACE_ASSERT (widget_in);
  ACE_ASSERT (userData_in);

  struct ARDrone_GtkCBData_Base* cb_data_p =
      reinterpret_cast<struct ARDrone_GtkCBData_Base*> (userData_in);

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

gboolean
key_cb (GtkWidget* widget_in,
        GdkEventKey* event_in,
        gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::key_cb"));

  ACE_UNUSED_ARG (widget_in);

  struct ARDrone_GtkCBData_Base* cb_data_base_p =
      reinterpret_cast<struct ARDrone_GtkCBData_Base*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (event_in);
  ACE_ASSERT (cb_data_base_p);

  Common_UI_GTK_BuildersIterator_t iterator =
    cb_data_base_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != cb_data_base_p->builders.end ());

  // sanity check(s)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct ARDrone_DirectShow_GtkCBData* directshow_cb_data_p =
    NULL;
  struct ARDrone_DirectShow_Configuration* directshow_configuration_p =
    NULL;
  struct ARDrone_MediaFoundation_GtkCBData* mediafoundation_cb_data_p =
    NULL;
  struct ARDrone_MediaFoundation_Configuration* mediafoundation_configuration_p =
    NULL;
  switch (cb_data_base_p->mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      directshow_cb_data_p =
        static_cast<struct ARDrone_DirectShow_GtkCBData*> (cb_data_base_p);
      directshow_configuration_p = directshow_cb_data_p->configuration;
      ACE_ASSERT (directshow_configuration_p);
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      mediafoundation_cb_data_p =
        static_cast<struct ARDrone_MediaFoundation_GtkCBData*> (cb_data_base_p);
      mediafoundation_configuration_p =
        mediafoundation_cb_data_p->configuration;
      ACE_ASSERT (mediafoundation_configuration_p);
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), aborting\n"),
                  cb_data_base_p->mediaFramework));
      break;
    }
  } // end SWITCH
#else
  struct ARDrone_GtkCBData* cb_data_p =
    static_cast<struct ARDrone_GtkCBData*> (cb_data_base_p);
  struct ARDrone_Configuration* configuration_p = cb_data_p->configuration;
  ACE_ASSERT (configuration_p);
//  wlan_monitor_configuration_p = &configuration_p->WLANMonitorConfiguration;
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ARDrone_DirectShow_StreamConfigurationsIterator_t directshow_iterator_2;
  ARDrone_DirectShow_StreamConfiguration_t::ITERATOR_T directshow_iterator_3;
  ARDrone_MediaFoundation_StreamConfigurationsIterator_t mediafoundation_iterator_2;
  ARDrone_MediaFoundation_StreamConfiguration_t::ITERATOR_T mediafoundation_iterator_3;
  switch (cb_data_base_p->mediaFramework)
  {
    case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
    {
      directshow_iterator_2 =
        directshow_configuration_p->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
      ACE_ASSERT (directshow_iterator_2 != directshow_configuration_p->streamConfigurations.end ());
      directshow_iterator_3 =
        (*directshow_iterator_2).second.find (ACE_TEXT_ALWAYS_CHAR (""));
      ACE_ASSERT (directshow_iterator_3 != (*directshow_iterator_2).second.end ());
      break;
    }
    case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
    {
      mediafoundation_iterator_2 =
        mediafoundation_configuration_p->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
      ACE_ASSERT (mediafoundation_iterator_2 != mediafoundation_configuration_p->streamConfigurations.end ());
      mediafoundation_iterator_3 =
        (*mediafoundation_iterator_2).second.find (ACE_TEXT_ALWAYS_CHAR (""));
      ACE_ASSERT (mediafoundation_iterator_3 != (*mediafoundation_iterator_2).second.end ());
      break;
    } // end ELSE
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                  cb_data_base_p->mediaFramework));
      return TRUE; // done (do not propagate further)
    }
  } // end SWITCH
#else
  ARDrone_StreamConfigurationsIterator_t iterator_2 =
      configuration_p->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
  ACE_ASSERT (iterator_2 != configuration_p->streamConfigurations.end ());
  ARDrone_StreamConfiguration_t::ITERATOR_T iterator_3 =
    (*iterator_2).second.find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (iterator_3 != (*iterator_2).second.end ());
#endif

  switch (event_in->keyval)
  {
    case GDK_KEY_Escape:
    case GDK_KEY_f:
    case GDK_KEY_F:
    {
      GtkToggleAction* toggle_action_p =
        GTK_TOGGLE_ACTION (gtk_builder_get_object ((*iterator).second.second,
                                                   ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_TOGGLEACTION_FULLSCREEN)));
      ACE_ASSERT (toggle_action_p);

      bool is_fullscreen = gtk_toggle_action_get_active (toggle_action_p);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      switch (cb_data_base_p->mediaFramework)
      {
        case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
          (*directshow_iterator_3).second.second.fullScreen = is_fullscreen;
          break;
        case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
          (*mediafoundation_iterator_3).second.second.fullScreen =
            is_fullscreen;
          break;
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                      cb_data_base_p->mediaFramework));
          return TRUE; // done (do not propagate further)
        }
      } // end SWITCH
#else
      (*iterator_3).second.second.fullScreen = is_fullscreen;
#endif

      // sanity check(s)
      if ((event_in->keyval == GDK_KEY_Escape) &&
          !is_fullscreen)
        break; // <-- not in fullscreen mode, nothing to do

      gtk_toggle_action_set_active (toggle_action_p,
                                    !is_fullscreen);

      break;
    }
//    case GDK_KEY_o:
//    case GDK_KEY_O:
//    {
//      GtkToggleAction* toggle_action_p =
//        GTK_TOGGLE_ACTION (gtk_builder_get_object ((*iterator).second.second,
//                                                   ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_TOGGLEACTION_FULLSCREEN)));
//      ACE_ASSERT (toggle_action_p);

//      ARDrone_StreamConfigurationsIterator_t iterator =
//        cb_data_p->configuration->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR ("Video_In"));
//      ACE_ASSERT (iterator != cb_data_p->configuration->streamConfigurations.end ());
//      ARDrone_StreamConfiguration_t::ITERATOR_T iterator_2 =
//        (*iterator).second.find (ACE_TEXT_ALWAYS_CHAR (""));
//      ACE_ASSERT (iterator_2 != (*iterator).second.end ());
//      (*iterator_2).second.second.fullScreen =
//        gtk_toggle_button_get_active (toggle_button_p);

//      // sanity check(s)
//      if ((event_in->keyval == GDK_KEY_Escape) &&
//          !(*iterator_2).second.second.fullScreen)
//        break; // <-- not in fullscreen mode, nothing to do

//      gtk_toggle_action_set_active (toggle_action_p,
//                                    !(*iterator_2).second.second.fullScreen);

//      break;
//    }
    default:
      return FALSE; // propagate
  } // end SWITCH

  return TRUE; // done (do not propagate further)
}

gboolean
drawingarea_video_key_press_event_cb (GtkWidget* widget_in,
                                      GdkEventKey* event_in,
                                      gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::drawingarea_video_key_press_event_cb"));

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

//gboolean
//motion_cb (GtkWidget* widget_in,
//           GdkEventMotion* event_in,
//           gpointer userData_in)
//{
//  ARDRONE_TRACE (ACE_TEXT ("::motion_cb"));
//
//  struct ARDrone_GtkCBData_Base* cb_data_p =
//    reinterpret_cast<struct ARDrone_GtkCBData_Base*> (userData_in);
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

//gint
//button_clear_clicked_cb (GtkWidget* widget_in,
//                         gpointer userData_in)
//{
//  ARDRONE_TRACE (ACE_TEXT ("::button_clear_clicked_cb"));
//
//  ACE_UNUSED_ARG (widget_in);
//  struct ARDrone_GtkCBData_Base* cb_data_p =
//    static_cast<struct ARDrone_GtkCBData_Base*> (userData_in);
//
//  // sanity check(s)
//  ACE_ASSERT (cb_data_p);
//
//  Common_UI_GTK_BuildersIterator_t iterator =
//    cb_data_p->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
//  // sanity check(s)
//  ACE_ASSERT (iterator != cb_data_p->builders.end ());
//
////  GtkTextView* view_p =
////    GTK_TEXT_VIEW (gtk_builder_get_object ((*iterator).second.second,
////                                           ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_TEXTVIEW_LOG)));
////  ACE_ASSERT (view_p);
////  GtkTextBuffer* buffer_p =
//////    gtk_text_buffer_new (NULL); // text tag table --> create new
////    gtk_text_view_get_buffer (view_p);
////  ACE_ASSERT (buffer_p);
////  gtk_text_buffer_set_text (buffer_p,
////                            ACE_TEXT_ALWAYS_CHAR (""), 0);
//
//  return TRUE; // done (do not propagate further)
//}

gint
button_about_clicked_cb (GtkWidget* widget_in,
                         gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::button_about_clicked_cb"));

  ACE_UNUSED_ARG (widget_in);
  struct ARDrone_GtkCBData_Base* cb_data_p =
      static_cast<struct ARDrone_GtkCBData_Base*> (userData_in);

  // sanity check(s)
  ACE_ASSERT (cb_data_p);

  Common_UI_GTK_BuildersIterator_t iterator =
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

gint
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
#if defined (__cplusplus)
}
#endif /* __cplusplus */
