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

#include "ardrone_stream_common.h"

#include "ardrone_common.h"
#include "ardrone_callbacks.h"
#include "ardrone_configuration.h"
#include "ardrone_defines.h"
#include "ardrone_macros.h"
#include "ardrone_stream.h"

ARDrone_EventHandler::ARDrone_EventHandler (struct ARDrone_GtkCBData* CBData_in,
                                            bool consoleMode_in)
 : consoleMode_ (consoleMode_in)
 , CBData_ (CBData_in)
 , ControlNotify_ (NULL)
 , MAVLinkNotify_ (NULL)
 , NavDataNotify_ (NULL)
 , streams_ ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_EventHandler::ARDrone_EventHandler"));

  // sanity check(s)
  ACE_ASSERT (CBData_);
}

void
ARDrone_EventHandler::start (Stream_SessionId_t sessionId_in,
                             const struct ARDrone_SessionData& sessionData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_EventHandler::start"));

  // sanity check(s)
  ACE_ASSERT (CBData_);
  ACE_ASSERT (sessionData_in.state);

  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, CBData_->lock);
    streams_.insert (std::make_pair (sessionId_in, sessionData_in.state->type));
  } // end lock scope

  if (sessionData_in.state->type == ARDRONE_STREAM_NAVDATA)
  {
    { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, CBData_->lock);
      guint event_source_id = g_idle_add (idle_session_start_cb,
                                          CBData_);
      if (event_source_id == 0)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to g_idle_add(idle_session_start_cb): \"%m\", returning\n")));
        return;
      } // end IF
      CBData_->eventSourceIds.insert (event_source_id);
    } // end lock scope
  } // end IF
}

void
ARDrone_EventHandler::notify (Stream_SessionId_t sessionId_in,
                              const ARDrone_Message& message_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_EventHandler::notify"));

  ACE_UNUSED_ARG (sessionId_in);

  // sanity check(s)
  ACE_ASSERT (CBData_);

  bool message_event = true;
  enum ARDrone_StreamType stream_type_e = ARDRONE_STREAM_INVALID;
  ARDroneStreamStatisticIterator_t iterator;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  bool refresh_display = false;
#endif
  switch (message_in.type ())
  {
    case ARDRONE_MESSAGE_ATCOMMAND:
    {
      message_event = false; // do not register outbound messages
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

      stream_type_e = ARDRONE_STREAM_CONTROL;

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

      stream_type_e = ARDRONE_STREAM_MAVLINK;

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

      stream_type_e = ARDRONE_STREAM_NAVDATA;

      break;
    }
    case ARDRONE_MESSAGE_VIDEO:
    {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
      refresh_display = true;
#endif

      stream_type_e = ARDRONE_STREAM_VIDEO;

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

  if (message_event)
  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, CBData_->lock);
//    iterator =
//      CBData_->progressData->statistic.streamStatistic.find (stream_type_e);
//    ACE_ASSERT (iterator != CBData_->progressData->statistic.streamStatistic.end ());
//    (*iterator).second.bytes += message_in.total_length ();
//    ++(*iterator).second.dataMessages;
//    +CBData_->progressData->statistic;

    CBData_->eventStack.push_back (std::make_pair (stream_type_e,
                                                   ARDRONE_EVENT_MESSAGE_DATA));
  } // end lock scope

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  if (refresh_display)
  {
    guint event_source_id = g_idle_add (idle_update_video_display_cb,
                                        CBData_);
    if (event_source_id == 0)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to g_idle_add(idle_update_video_display_cb): \"%m\", returning\n")));
      return;
    } // end IF
  } // end IF
#endif
}

void
ARDrone_EventHandler::notify (Stream_SessionId_t sessionId_in,
                              const ARDrone_SessionMessage& message_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_EventHandler::notify"));

  int result = -1;
  enum ARDrone_StreamType stream_type_e = ARDRONE_STREAM_INVALID;
  enum ARDrone_EventType event_e = ARDRONE_EVENT_MESSAGE_SESSION;
  SESSIONID_TO_STREAM_MAP_ITERATOR_T iterator;
  ARDroneStreamStatisticIterator_t iterator_2;

  // sanity check(s)
  ACE_ASSERT (CBData_);

  // update session id <-> stream mapping ?
  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, CBData_->lock);
    if (message_in.type () == STREAM_SESSION_MESSAGE_LINK)
    {
      const ARDrone_StreamSessionData_t& session_data_container_r =
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
    stream_type_e = (*iterator).second;
  } // end lock scope

  switch (message_in.type ())
  {
    case STREAM_SESSION_MESSAGE_CONNECT:
      event_e = ARDRONE_EVENT_CONNECT;
      break;
    case STREAM_SESSION_MESSAGE_DISCONNECT:
      event_e = ARDRONE_EVENT_DISCONNECT;
      break;
    case STREAM_SESSION_MESSAGE_ABORT:
    case STREAM_SESSION_MESSAGE_LINK:
      break;
    case STREAM_SESSION_MESSAGE_RESIZE:
    {
      // sanity check(s)
      ACE_ASSERT (CBData_->configuration);

      event_e = ARDRONE_EVENT_RESIZE;

      // update configuration (reused by gtk callback(s))
      const ARDrone_StreamSessionData_t& session_data_container_r =
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
      switch (CBData_->mediaFramework)
      {
        case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
        {
          directshow_video_streamconfiguration_iterator =
            CBData_->configuration->directShowStreamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
          ACE_ASSERT (directshow_video_streamconfiguration_iterator != CBData_->configuration->directShowStreamConfigurations.end ());
          directshow_iterator_3 =
            (*directshow_video_streamconfiguration_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (""));
          ACE_ASSERT (directshow_iterator_3 != (*directshow_video_streamconfiguration_iterator).second.end ());
          directshow_iterator_4 =
            (*directshow_video_streamconfiguration_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (MODULE_DEC_DECODER_LIBAV_DECODER_DEFAULT_NAME_STRING));
          ACE_ASSERT (directshow_iterator_4 != (*directshow_video_streamconfiguration_iterator).second.end ());
          break;
        }
        case STREAM_MEDIAFRAMEWORK_MEDIAFOUNDATION:
        {
          mediafoundation_video_streamconfiguration_iterator =
            CBData_->configuration->mediaFoundationStreamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
          ACE_ASSERT (mediafoundation_video_streamconfiguration_iterator != CBData_->configuration->mediaFoundationStreamConfigurations.end ());
          mediafoundation_iterator_3 =
            (*mediafoundation_video_streamconfiguration_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (""));
          ACE_ASSERT (mediafoundation_iterator_3 != (*mediafoundation_video_streamconfiguration_iterator).second.end ());
          mediafoundation_iterator_4 =
            (*mediafoundation_video_streamconfiguration_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (MODULE_DEC_DECODER_LIBAV_DECODER_DEFAULT_NAME_STRING));
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
#else
      ARDrone_StreamConfigurationsIterator_t video_streamconfiguration_iterator =
          CBData_->configuration->streamConfigurations.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING));
      ACE_ASSERT (video_streamconfiguration_iterator != CBData_->configuration->streamConfigurations.end ());
      ARDrone_StreamConfiguration_t::ITERATOR_T iterator_3 =
        (*video_streamconfiguration_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (""));
      ACE_ASSERT (iterator_3 != (*video_streamconfiguration_iterator).second.end ());
      ARDrone_StreamConfiguration_t::ITERATOR_T iterator_4 =
        (*video_streamconfiguration_iterator).second.find (ACE_TEXT_ALWAYS_CHAR (MODULE_DEC_DECODER_LIBAV_DECODER_DEFAULT_NAME_STRING));
      ACE_ASSERT (iterator_4 != (*video_streamconfiguration_iterator).second.end ());
#endif

      if (session_data_r.lock)
      {
        result = session_data_r.lock->acquire ();
        if (result == -1)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_SYNCH_MUTEX::acquire(): \"%m\", continuing\n")));
      } // end IF

      { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, CBData_->lock);
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      switch (CBData_->mediaFramework)
      {
        case STREAM_MEDIAFRAMEWORK_DIRECTSHOW:
        {
          // sanity check(s)
          ACE_ASSERT ((*directshow_iterator_3).second.second.filterConfiguration);
          ACE_ASSERT ((*directshow_iterator_3).second.second.filterConfiguration->pinConfiguration);
          ACE_ASSERT ((*directshow_iterator_3).second.second.filterConfiguration->pinConfiguration->format);
          ACE_ASSERT ((*directshow_iterator_3).second.second.filterConfiguration->pinConfiguration->format->formattype == FORMAT_VideoInfo);
          ACE_ASSERT ((*directshow_iterator_3).second.second.filterConfiguration->pinConfiguration->format->cbFormat == sizeof (struct tagVIDEOINFOHEADER));
          struct tagVIDEOINFOHEADER* video_info_header_p =
            reinterpret_cast<struct tagVIDEOINFOHEADER*> ((*directshow_iterator_3).second.second.filterConfiguration->pinConfiguration->format->pbFormat);

          ACE_ASSERT ((*directshow_iterator_3).second.second.inputFormat);
          ACE_ASSERT ((*directshow_iterator_3).second.second.inputFormat->formattype == FORMAT_VideoInfo);
          ACE_ASSERT ((*directshow_iterator_3).second.second.inputFormat->cbFormat == sizeof (struct tagVIDEOINFOHEADER));
          struct tagVIDEOINFOHEADER* video_info_header_2 =
            reinterpret_cast<struct tagVIDEOINFOHEADER*> ((*directshow_iterator_3).second.second.inputFormat->pbFormat);

          ACE_ASSERT ((*directshow_iterator_4).second.second.filterConfiguration);
          ACE_ASSERT ((*directshow_iterator_4).second.second.filterConfiguration->pinConfiguration);
          ACE_ASSERT ((*directshow_iterator_4).second.second.filterConfiguration->pinConfiguration->format);
          ACE_ASSERT ((*directshow_iterator_4).second.second.filterConfiguration->pinConfiguration->format->formattype == FORMAT_VideoInfo);
          ACE_ASSERT ((*directshow_iterator_4).second.second.filterConfiguration->pinConfiguration->format->cbFormat == sizeof (struct tagVIDEOINFOHEADER));
          struct tagVIDEOINFOHEADER* video_info_header_3 =
            reinterpret_cast<struct tagVIDEOINFOHEADER*> ((*directshow_iterator_4).second.second.filterConfiguration->pinConfiguration->format->pbFormat);

          ACE_ASSERT ((*directshow_iterator_4).second.second.inputFormat);
          ACE_ASSERT ((*directshow_iterator_4).second.second.inputFormat->formattype == FORMAT_VideoInfo);
          ACE_ASSERT ((*directshow_iterator_4).second.second.inputFormat->cbFormat == sizeof (struct tagVIDEOINFOHEADER));
          struct tagVIDEOINFOHEADER* video_info_header_4 =
            reinterpret_cast<struct tagVIDEOINFOHEADER*> ((*directshow_iterator_4).second.second.inputFormat->pbFormat);

          ACE_ASSERT (session_data_r.inputFormat);
          ACE_ASSERT (session_data_r.inputFormat->formattype == FORMAT_VideoInfo);
          ACE_ASSERT (session_data_r.inputFormat->cbFormat == sizeof (struct tagVIDEOINFOHEADER));
          struct tagVIDEOINFOHEADER* video_info_header_5 =
            reinterpret_cast<struct tagVIDEOINFOHEADER*> (session_data_r.inputFormat->pbFormat);

          video_info_header_p->bmiHeader.biHeight = video_info_header_5->bmiHeader.biHeight;
          video_info_header_p->bmiHeader.biWidth = video_info_header_5->bmiHeader.biWidth;
          video_info_header_p->bmiHeader.biSizeImage =
            DIBSIZE (video_info_header_p->bmiHeader);
          video_info_header_2->bmiHeader.biHeight = video_info_header_5->bmiHeader.biHeight;
          video_info_header_2->bmiHeader.biWidth = video_info_header_5->bmiHeader.biWidth;
          video_info_header_2->bmiHeader.biSizeImage =
            DIBSIZE (video_info_header_2->bmiHeader);
          video_info_header_3->bmiHeader.biHeight = video_info_header_5->bmiHeader.biHeight;
          video_info_header_3->bmiHeader.biWidth = video_info_header_5->bmiHeader.biWidth;
          video_info_header_3->bmiHeader.biSizeImage =
            DIBSIZE (video_info_header_3->bmiHeader);
          video_info_header_4->bmiHeader.biHeight = video_info_header_5->bmiHeader.biHeight;
          video_info_header_4->bmiHeader.biWidth = video_info_header_5->bmiHeader.biWidth;
          video_info_header_4->bmiHeader.biSizeImage =
            DIBSIZE (video_info_header_4->bmiHeader);
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
#endif
      } // end lock scope

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
      // sanity check(s)
      ACE_ASSERT (CBData_->progressData);

      const ARDrone_StreamSessionData_t& session_data_container_r =
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

      { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, CBData_->lock);
        CBData_->progressData->statistic = session_data_r.statistic;
      } // end lock scope

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

  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, CBData_->lock);
    CBData_->eventStack.push_back (std::make_pair (stream_type_e,
                                                      event_e));
  } // end lock scope
}

void
ARDrone_EventHandler::end (Stream_SessionId_t sessionId_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_EventHandler::end"));

  // sanity check(s)
  ACE_ASSERT (CBData_);

  enum ARDrone_StreamType stream_type_e = ARDRONE_STREAM_INVALID;
  Stream_IStreamControlBase* istream_base_p = NULL;

  SESSIONID_TO_STREAM_MAP_ITERATOR_T iterator;
  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, CBData_->lock);
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
  } // end lock scope

  switch (stream_type_e)
  {
    case ARDRONE_STREAM_CONTROL:
    {
      // *NOTE*: the device closes the control connection after transmitting
      //         configuration data
      //         --> reconnect automatically ?
      Common_UI_GTK_BuildersIterator_t iterator =
        CBData_->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_GTK_DEFINITION_DESCRIPTOR_MAIN));
      // sanity check(s)
      ACE_ASSERT (iterator != CBData_->builders.end ());

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

      ARDrone_StreamsIterator_t iterator_2 =
          CBData_->streams.find (control_stream_name_string_);
      ACE_ASSERT (iterator_2 != CBData_->streams.end ());
      istream_base_p =
          dynamic_cast<Stream_IStreamControlBase*> ((*iterator_2).second);
      ACE_ASSERT (istream_base_p);
      istream_base_p->start ();
      break;
    }
    case ARDRONE_STREAM_MAVLINK:
      break;
    case ARDRONE_STREAM_NAVDATA:
    {
      { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, CBData_->lock);
        guint event_source_id = g_idle_add (idle_session_end_cb,
                                            CBData_);
        if (event_source_id == 0)
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to g_idle_add(idle_session_end_cb): \"%m\", returning\n")));
          return;
        } // end IF
        CBData_->eventSourceIds.insert (event_source_id);
      } // end lock scope

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
