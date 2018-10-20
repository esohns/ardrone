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
#include "stdafx.h"

#include "ace/Synch.h"
#include "ardrone_eventhandler.h"

#include "ace/Guard_T.h"
#include "ace/Synch_Traits.h"

#include "ardrone_common.h"
#include "ardrone_callbacks.h"
#include "ardrone_configuration.h"
#include "ardrone_defines.h"
#include "ardrone_macros.h"
#include "ardrone_stream.h"
#include "ardrone_stream_common.h"

#if defined (GUI_SUPPORT)
ARDrone_EventHandler::ARDrone_EventHandler (struct ARDrone_UI_CBData_Base* CBData_in,
#else
ARDrone_EventHandler::ARDrone_EventHandler (
#endif // GUI_SUPPORT
                                            bool consoleMode_in)
 : consoleMode_ (consoleMode_in)
#if defined (GUI_SUPPORT)
 , CBData_ (CBData_in)
#endif // GUI_SUPPORT
 , ControlNotify_ (NULL)
 , MAVLinkNotify_ (NULL)
 , NavDataNotify_ (NULL)
 , streams_ ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_EventHandler::ARDrone_EventHandler"));

  // sanity check(s)
#if defined (GUI_SUPPORT)
  ACE_ASSERT (CBData_);
#endif // GUI_SUPPORT
}

void
ARDrone_EventHandler::start (Stream_SessionId_t sessionId_in,
                             const struct ARDrone_SessionData& sessionData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_EventHandler::start"));

  // sanity check(s)
#if defined (GUI_SUPPORT)
  ACE_ASSERT (CBData_);
#if defined (GTK_USE)
  struct ARDrone_UI_GTK_State& state_r =
    const_cast<struct ARDrone_UI_GTK_State&> (ARDRONE_UI_GTK_MANAGER_SINGLETON::instance ()->getR_2 ());
#endif // GTK_USE
#endif // GUI_SUPPORT
  ACE_ASSERT (sessionData_in.state);

#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, state_r.lock);
#endif // GTK_USE
#endif // GUI_SUPPORT
    streams_.insert (std::make_pair (sessionId_in,
                                     sessionData_in.state->type));
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
  } // end lock scope
#endif // GTK_USE
#endif // GUI_SUPPORT

  if (sessionData_in.state->type == ARDRONE_STREAM_NAVDATA)
  {
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
    { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, state_r.lock);
      guint event_source_id = g_idle_add (idle_session_start_cb,
                                          CBData_);
      if (event_source_id == 0)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to g_idle_add(idle_session_start_cb): \"%m\", returning\n")));
        return;
      } // end IF
      state_r.eventSourceIds.insert (event_source_id);
    } // end lock scope
#endif // GTK_USE
#endif // GUI_SUPPORT
  } // end IF
}

void
ARDrone_EventHandler::notify (Stream_SessionId_t sessionId_in,
                              const ARDrone_Message& message_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_EventHandler::notify"));

  ACE_UNUSED_ARG (sessionId_in);

  // sanity check(s)
#if defined (GUI_SUPPORT)
  ACE_ASSERT (CBData_);
#if defined (GTK_USE)
  struct ARDrone_UI_GTK_State& state_r =
    const_cast<struct ARDrone_UI_GTK_State&> (ARDRONE_UI_GTK_MANAGER_SINGLETON::instance ()->getR_2 ());
#endif // GTK_USE
#endif // GUI_SUPPORT

  bool message_event_b = true;
  ARDrone_Event_t event_s =
    std::make_pair (ARDRONE_STREAM_INVALID,
                    static_cast<enum Net_WLAN_EventType> (COMMON_UI_EVENT_DATA));
  ARDroneStreamStatisticIterator_t iterator;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  bool refresh_display = false;
#endif // ACE_WIN32 || ACE_WIN64
  switch (message_in.type ())
  {
    case ARDRONE_MESSAGE_ATCOMMAND:
    {
      message_event_b = false; // do not register outbound messages
      //stream_type_e = ARDRONE_STREAM_NAVDATA;
      break;
    }
    case ARDRONE_MESSAGE_CONTROL:
    { ACE_ASSERT (ControlNotify_);
      const ARDrone_MessageData_t& data_container_r = message_in.getR ();
      const struct ARDrone_MessageData& data_r = data_container_r.getR ();
      try {
        // *TODO*: remove type inference
        ControlNotify_->messageCB (data_r.controlData);
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in ARDrone_IControlNotify::messageCB(), returning\n")));
      }

      event_s.first = ARDRONE_STREAM_CONTROL;

      break;
    }
    case ARDRONE_MESSAGE_MAVLINK:
    { ACE_ASSERT (MAVLinkNotify_);
      const ARDrone_MessageData_t& data_container_r = message_in.getR ();
      const struct ARDrone_MessageData& data_r = data_container_r.getR ();
      try {
        // *TODO*: remove type inference
        MAVLinkNotify_->messageCB (data_r.MAVLinkData,
                                   message_in.rd_ptr ());
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in ARDrone_IMAVLinkNotify::messageCB(), returning\n")));
      }

      event_s.first = ARDRONE_STREAM_MAVLINK;

      break;
    }
    case ARDRONE_MESSAGE_NAVDATA:
    { ACE_ASSERT (NavDataNotify_);
      const ARDrone_MessageData_t& data_container_r = message_in.getR ();
      const struct ARDrone_MessageData& data_r = data_container_r.getR ();

      try {
        // *TODO*: remove type inference
        NavDataNotify_->messageCB (data_r.NavData.NavData,
                                   data_r.NavData.NavDataOptionOffsets,
                                   message_in.rd_ptr ());
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in ARDrone_INavDataNotify::messageCB(), returning\n")));
      }

      event_s.first = ARDRONE_STREAM_NAVDATA;

      break;
    }
    case ARDRONE_MESSAGE_VIDEO:
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
      refresh_display = true;
#endif // ACE_WIN32 || ACE_WIN64

      event_s.first = ARDRONE_STREAM_VIDEO;

      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown message type (was: %d), returning\n"),
                  message_in.type ()));
      return;
    }
  } // end SWITCH

  if (message_event_b)
  { 
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
    { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, state_r.lock);
//    iterator =
//      CBData_.progressData->statistic.streamStatistic.find (stream_type_e);
//    ACE_ASSERT (iterator != CBData_.progressData->statistic.streamStatistic.end ());
//    (*iterator).second.bytes += message_in.total_length ();
//    ++(*iterator).second.dataMessages;
//    +CBData_.progressData->statistic;
      state_r.eventStack.push (event_s);
    } // end lock scope
#endif // GTK_USE
#endif // GUI_SUPPORT
  } // end IF

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  if (refresh_display)
  {
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
    guint event_source_id = g_idle_add (idle_update_video_display_cb,
                                        CBData_);
    if (event_source_id == 0)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to g_idle_add(idle_update_video_display_cb): \"%m\", returning\n")));
      return;
    } // end IF
#endif // GTK_USE
#endif // GUI_SUPPORT
  } // end IF
#endif // ACE_WIN32 || ACE_WIN64
}

void
ARDrone_EventHandler::notify (Stream_SessionId_t sessionId_in,
                              const ARDrone_SessionMessage& message_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_EventHandler::notify"));

  int result = -1;
  ARDrone_Event_t event_s =
    std::make_pair (ARDRONE_STREAM_INVALID,
                    static_cast<enum Net_WLAN_EventType> (COMMON_UI_EVENT_SESSION));
  SESSIONID_TO_STREAM_MAP_ITERATOR_T iterator;
  ARDroneStreamStatisticIterator_t iterator_2;

  // sanity check(s)
#if defined (GUI_SUPPORT)
  ACE_ASSERT (CBData_);
#if defined (GTK_USE)
  struct ARDrone_UI_GTK_State& state_r =
    const_cast<struct ARDrone_UI_GTK_State&> (ARDRONE_UI_GTK_MANAGER_SINGLETON::instance ()->getR_2 ());
#endif // GTK_USE
#endif // GUI_SUPPORT

  // update session id <-> stream mapping ?
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, state_r.lock);
#endif // GTK_USE
#endif // GUI_SUPPORT
    if (message_in.type () == STREAM_SESSION_MESSAGE_LINK)
    {
      const ARDrone_SessionData_t& session_data_container_r =
        message_in.getR ();
      struct ARDrone_SessionData& session_data_r =
        const_cast<struct ARDrone_SessionData&> (session_data_container_r.getR ());
      iterator =
        std::find_if (streams_.begin (), streams_.end (),
                      std::bind2nd (SESSIONID_TO_STREAM_MAP_FIND_S (),
                                    session_data_r.state->type));
      ACE_ASSERT (iterator != streams_.end ());
      if ((*iterator).first != sessionId_in)
      {
        //ACE_DEBUG ((LM_DEBUG,
        //            ACE_TEXT ("updating session id (was: %u) --> %u\n"),
        //            (*iterator).first,
        //            sessionId_in));
        streams_.erase (iterator);
        streams_.insert (std::make_pair (sessionId_in,
                                         session_data_r.state->type));
        //(*iterator).first = sessionId_in;
      } // end IF
    } // end IF
    iterator = streams_.find (sessionId_in);
    // sanity check(s)
    if (iterator == streams_.end ())
    { // most probable reason: statistic messages arriving out-of-session
      //ACE_DEBUG ((LM_DEBUG,
      //            ACE_TEXT ("session message (type: %d): invalid session id (was: %u), returning\n"),
      //            message_in.type (),
      //            sessionId_in));
      return;
    } // end IF
    event_s.first = (*iterator).second;
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
  } // end lock scope
#endif // GTK_USE
#endif // GUI_SUPPORT

  switch (message_in.type ())
  {
    case STREAM_SESSION_MESSAGE_CONNECT:
      event_s.second =
        static_cast<enum Net_WLAN_EventType> (COMMON_UI_EVENT_CONNECT);
      break;
    case STREAM_SESSION_MESSAGE_DISCONNECT:
      event_s.second =
        static_cast<enum Net_WLAN_EventType> (COMMON_UI_EVENT_DISCONNECT);
      break;
    case STREAM_SESSION_MESSAGE_ABORT:
    case STREAM_SESSION_MESSAGE_LINK:
      break;
    case STREAM_SESSION_MESSAGE_RESIZE:
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      struct ARDrone_DirectShow_Configuration* directshow_configuration_p =
        NULL;
      struct ARDrone_MediaFoundation_Configuration* mediafoundation_configuration_p =
        NULL;
#if defined (GUI_SUPPORT)
      switch (CBData_->mediaFramework)
      {
        case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
        {
          struct ARDrone_DirectShow_UI_CBData* cb_data_p =
            static_cast<struct ARDrone_DirectShow_UI_CBData*> (CBData_);
          directshow_configuration_p = cb_data_p->configuration;
          break;
        }
        case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
        {
          struct ARDrone_MediaFoundation_UI_CBData* cb_data_p =
            static_cast<struct ARDrone_MediaFoundation_UI_CBData*> (CBData_);
          mediafoundation_configuration_p = cb_data_p->configuration;
          break;
        }
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                      CBData_->mediaFramework));
          return;
        }
      } // end SWITCH
#endif // GUI_SUPPORT
#else
#if defined (GUI_SUPPORT)
      struct ARDrone_UI_CBData* cb_data_p =
        static_cast<struct ARDrone_UI_CBData*> (CBData_);
      struct ARDrone_Configuration* configuration_p = cb_data_p->configuration;
#endif // GUI_SUPPORT
#endif // ACE_WIN32 || ACE_WIN64
      event_s.second =
        static_cast<Net_WLAN_EventType> (COMMON_UI_EVENT_RESIZE);

      // update configuration (reused by gtk callback(s))
      const ARDrone_SessionData_t& session_data_container_r =
        message_in.getR ();
      struct ARDrone_SessionData& session_data_r =
        const_cast<struct ARDrone_SessionData&> (session_data_container_r.getR ());

#if defined (ACE_WIN32) || defined (ACE_WIN64)
      ARDrone_DirectShow_StreamConfigurationsIterator_t directshow_video_streamconfiguration_iterator;
      ARDrone_DirectShow_StreamConfiguration_t::ITERATOR_T directshow_iterator_3;
      ARDrone_DirectShow_StreamConfiguration_t::ITERATOR_T directshow_iterator_4;
      ARDrone_MediaFoundation_StreamConfigurationsIterator_t mediafoundation_video_streamconfiguration_iterator;
      ARDrone_MediaFoundation_StreamConfiguration_t::ITERATOR_T mediafoundation_iterator_3;
      ARDrone_MediaFoundation_StreamConfiguration_t::ITERATOR_T mediafoundation_iterator_4;
#if defined (GUI_SUPPORT)
      switch (CBData_->mediaFramework)
      {
        case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
        { ACE_ASSERT (directshow_configuration_p);
          directshow_video_streamconfiguration_iterator =
            directshow_configuration_p->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
          ACE_ASSERT (directshow_video_streamconfiguration_iterator != directshow_configuration_p->streamConfigurations.end ());
          directshow_iterator_3 =
            (*directshow_video_streamconfiguration_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (""));
          ACE_ASSERT (directshow_iterator_3 != (*directshow_video_streamconfiguration_iterator).second.end ());
          directshow_iterator_4 =
            (*directshow_video_streamconfiguration_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (STREAM_DEC_DECODER_LIBAV_DECODER_DEFAULT_NAME_STRING));
          ACE_ASSERT (directshow_iterator_4 != (*directshow_video_streamconfiguration_iterator).second.end ());
          break;
        }
        case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
        { ACE_ASSERT (mediafoundation_configuration_p);
          mediafoundation_video_streamconfiguration_iterator =
            mediafoundation_configuration_p->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
          ACE_ASSERT (mediafoundation_video_streamconfiguration_iterator != mediafoundation_configuration_p->streamConfigurations.end ());
          mediafoundation_iterator_3 =
            (*mediafoundation_video_streamconfiguration_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (""));
          ACE_ASSERT (mediafoundation_iterator_3 != (*mediafoundation_video_streamconfiguration_iterator).second.end ());
          mediafoundation_iterator_4 =
            (*mediafoundation_video_streamconfiguration_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (STREAM_DEC_DECODER_LIBAV_DECODER_DEFAULT_NAME_STRING));
          ACE_ASSERT (mediafoundation_iterator_4 != (*mediafoundation_video_streamconfiguration_iterator).second.end ());
          break;
        }
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("invalid/unknown media framework (was: %d), returning\n"),
                      CBData_->mediaFramework));
          return;
        }
    } // end SWITCH
#endif // GUI_SUPPORT
#else
      ACE_ASSERT (configuration_p);
      ARDrone_StreamConfigurationsIterator_t video_streamconfiguration_iterator =
        configuration_p->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
      ACE_ASSERT (video_streamconfiguration_iterator != configuration_p->streamConfigurations.end ());
      ARDrone_StreamConfiguration_t::ITERATOR_T iterator_3 =
        (*video_streamconfiguration_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (""));
      ACE_ASSERT (iterator_3 != (*video_streamconfiguration_iterator).second.end ());
      ARDrone_StreamConfiguration_t::ITERATOR_T iterator_4 =
        (*video_streamconfiguration_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (STREAM_DEC_DECODER_LIBAV_DECODER_DEFAULT_NAME_STRING));
      ACE_ASSERT (iterator_4 != (*video_streamconfiguration_iterator).second.end ());
#endif // ACE_WIN32 || ACE_WIN64

      if (session_data_r.lock)
      {
        result = session_data_r.lock->acquire ();
        if (result == -1)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_SYNCH_MUTEX::acquire(): \"%m\", continuing\n")));
      } // end IF

#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
      { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, state_r.lock);
#endif // GTK_USE
#endif // GUI_SUPPORT
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      switch (CBData_->mediaFramework)
      {
        case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
        {
          // sanity check(s)
          ACE_ASSERT (!session_data_r.formats.empty ());
          struct _AMMediaType& media_type_r = session_data_r.formats.front ();
          ACE_ASSERT (InlineIsEqualGUID (media_type_r.formattype, FORMAT_VideoInfo));
          ACE_ASSERT (media_type_r.cbFormat == sizeof (struct tagVIDEOINFOHEADER));

          struct tagVIDEOINFOHEADER* video_info_header_p =
            reinterpret_cast<struct tagVIDEOINFOHEADER*> (media_type_r.pbFormat);
          Common_UI_Resolution_t resolution_s;
          resolution_s.cx = video_info_header_p->bmiHeader.biWidth;
          resolution_s.cy = video_info_header_p->bmiHeader.biHeight;

          // sanity check(s)
          ACE_ASSERT ((*directshow_iterator_3).second.second.filterConfiguration);
          ACE_ASSERT ((*directshow_iterator_3).second.second.filterConfiguration->pinConfiguration);
          ACE_ASSERT ((*directshow_iterator_3).second.second.filterConfiguration->pinConfiguration->format);
          Stream_MediaFramework_DirectShow_Tools::resize (resolution_s,
                                                          *(*directshow_iterator_3).second.second.filterConfiguration->pinConfiguration->format);
          ACE_ASSERT ((*directshow_iterator_3).second.second.outputFormat);

          Stream_MediaFramework_DirectShow_Tools::resize (resolution_s,
                                                          *(*directshow_iterator_3).second.second.outputFormat);

          // sanity check(s)
          ACE_ASSERT ((*directshow_iterator_4).second.second.filterConfiguration);
          ACE_ASSERT ((*directshow_iterator_4).second.second.filterConfiguration->pinConfiguration);
          ACE_ASSERT ((*directshow_iterator_4).second.second.filterConfiguration->pinConfiguration->format);
          Stream_MediaFramework_DirectShow_Tools::resize (resolution_s,
                                                          *(*directshow_iterator_4).second.second.filterConfiguration->pinConfiguration->format);
          ACE_ASSERT ((*directshow_iterator_4).second.second.outputFormat);

          Stream_MediaFramework_DirectShow_Tools::resize (resolution_s,
                                                          *(*directshow_iterator_4).second.second.outputFormat);
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
                      CBData_->mediaFramework));
          return;
        }
      } // end SWITCH
#else
        (*iterator_3).second.second.sourceFormat.height = session_data_r.height;
        (*iterator_3).second.second.sourceFormat.width = session_data_r.width;
        (*iterator_4).second.second.sourceFormat.height = session_data_r.height;
        (*iterator_4).second.second.sourceFormat.width = session_data_r.width;
#endif // ACE_WIN32 || ACE_WIN64
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
      } // end lock scope
#endif // GTK_USE
#endif // GUI_SUPPORT

      if (session_data_r.lock)
      {
        result = session_data_r.lock->release ();
        if (result == -1)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_SYNCH_MUTEX::release(): \"%m\", continuing\n")));
      } // end IF

      break;
    }
    case STREAM_SESSION_MESSAGE_UNLINK:
      break;
    case STREAM_SESSION_MESSAGE_STATISTIC:
    {
      const ARDrone_SessionData_t& session_data_container_r =
        message_in.getR ();
      struct ARDrone_SessionData& session_data_r =
        const_cast<struct ARDrone_SessionData&> (session_data_container_r.getR ());

      if (session_data_r.lock)
      {
        result = session_data_r.lock->acquire ();
        if (result == -1)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_SYNCH_MUTEX::acquire(): \"%m\", continuing\n")));
      } // end IF

#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
      { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, state_r.lock);
#endif // GTK_USE
        CBData_->progressData.statistic = session_data_r.statistic;
#if defined (GTK_USE)
      } // end lock scope
#endif // GTK_USE
#endif // GUI_SUPPORT

      if (session_data_r.lock)
      {
        result = session_data_r.lock->release ();
        if (result == -1)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_SYNCH_MUTEX::release(): \"%m\", continuing\n")));
      } // end IF

      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown session message type (was: %d), returning\n"),
                  message_in.type ()));
      return;
    }
  } // end SWITCH

#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, state_r.lock);
    state_r.eventStack.push (event_s);
  } // end lock scope
#endif // GTK_USE
#endif // GUI_SUPPORT
}

void
ARDrone_EventHandler::end (Stream_SessionId_t sessionId_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_EventHandler::end"));

  // sanity check(s)
#if defined (GUI_SUPPORT)
  ACE_ASSERT (CBData_);
#if defined (GTK_USE)
  struct ARDrone_UI_GTK_State& state_r =
    const_cast<struct ARDrone_UI_GTK_State&> (ARDRONE_UI_GTK_MANAGER_SINGLETON::instance ()->getR_2 ());
#endif // GTK_USE
#endif // GUI_SUPPORT

  enum ARDrone_StreamType stream_type_e = ARDRONE_STREAM_INVALID;
  Stream_IStreamControlBase* istream_base_p = NULL;

  SESSIONID_TO_STREAM_MAP_ITERATOR_T iterator;
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, state_r.lock);
#endif // GTK_USE
#endif // GUI_SUPPORT
    iterator = streams_.find (sessionId_in);
    if (iterator != streams_.end ())
    {
      stream_type_e = (*iterator).second;
      streams_.erase (iterator);
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("%s: session id %d has ended\n"),
                  ACE_TEXT (ARDroneStreamTypeToString (stream_type_e).c_str ()),
                  sessionId_in));
    } // end IF
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
  } // end lock scope
#endif // GTK_USE
#endif // GUI_SUPPORT

  switch (stream_type_e)
  {
    case ARDRONE_STREAM_CONTROL:
    {
      // *NOTE*: the device closes the control connection after transmitting
      //         configuration data
      //         --> reconnect automatically ?
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
      Common_UI_GTK_BuildersIterator_t iterator =
        state_r.builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN));
      // sanity check(s)
      ACE_ASSERT (iterator != state_r.builders.end ());

      // disconnecting ?
      GtkToggleAction* toggle_action_p =
          GTK_TOGGLE_ACTION (gtk_builder_get_object ((*iterator).second.second,
                                                     ACE_TEXT_ALWAYS_CHAR (ARDRONE_UI_WIDGET_NAME_TOGGLEACTION_CONNECT)));
      ACE_ASSERT (toggle_action_p);
      gdk_threads_enter ();
      if (!gtk_toggle_action_get_active (toggle_action_p))
      {
        gdk_threads_leave ();
        break;
      } // end IF
      gdk_threads_leave ();
#endif // GTK_USE

      ARDrone_StreamsIterator_t iterator_2 =
          CBData_->streams.find (control_stream_name_string_);
      ACE_ASSERT (iterator_2 != CBData_->streams.end ());
      istream_base_p =
          dynamic_cast<Stream_IStreamControlBase*> ((*iterator_2).second);
      ACE_ASSERT (istream_base_p);
      istream_base_p->start ();
#endif // GUI_SUPPORT // *TODO*
      break;
    }
    case ARDRONE_STREAM_MAVLINK:
      break;
    case ARDRONE_STREAM_NAVDATA:
    {
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
      { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, state_r.lock);
        guint event_source_id = g_idle_add (idle_session_end_cb,
                                            CBData_);
        if (event_source_id == 0)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to g_idle_add(idle_session_end_cb): \"%m\", returning\n")));
          return;
        } // end IF
        state_r.eventSourceIds.insert (event_source_id);
      } // end lock scope
#endif // GTK_USE
#endif // GUI_SUPPORT

      break;
    }
    case ARDRONE_STREAM_VIDEO:
      break;
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown stream type (was: %d), returning\n"),
                  stream_type_e));
      return;
    }
  } // end SWITCH
}
