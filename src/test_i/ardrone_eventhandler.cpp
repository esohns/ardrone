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
#include "ardrone_macros.h"

ARDrone_EventHandler::ARDrone_EventHandler (struct ARDrone_GtkCBData* GtkCBData_in,
                                            bool consoleMode_in)
 : consoleMode_ (consoleMode_in)
 , GtkCBData_ (GtkCBData_in)
 , MAVLinkNotify_ (NULL)
 , NavDataNotify_ (NULL)
 , streams_ ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_EventHandler::ARDrone_EventHandler"));

  // sanity check(s)
  ACE_ASSERT (GtkCBData_);
}

void
ARDrone_EventHandler::start (Stream_SessionId_t sessionID_in,
                             const struct ARDrone_SessionData& sessionData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_EventHandler::start"));

  // sanity check(s)
  ACE_ASSERT (GtkCBData_);
  ACE_ASSERT (sessionData_in.state);

  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, GtkCBData_->lock);
    streams_.insert (std::make_pair (sessionID_in, sessionData_in.state->type));
  } // end lock scope

  if (sessionData_in.state->type == ARDRONE_STREAM_NAVDATA)
  {
    { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, GtkCBData_->lock);
      guint event_source_id = g_idle_add (idle_session_start_cb,
                                          GtkCBData_);
      if (event_source_id == 0)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to g_idle_add(idle_session_start_cb): \"%m\", returning\n")));
        return;
      } // end IF
      GtkCBData_->eventSourceIds.insert (event_source_id);
    } // end lock scope
  } // end IF
}

void
ARDrone_EventHandler::notify (Stream_SessionId_t sessionID_in,
                              const ARDrone_Message& message_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_EventHandler::notify"));

  ACE_UNUSED_ARG (sessionID_in);

  // sanity check(s)
  ACE_ASSERT (GtkCBData_);

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
    {
      stream_type_e = ARDRONE_STREAM_CONTROL;
      break;
    }
    case ARDRONE_MESSAGE_MAVLINK:
    { ACE_ASSERT (MAVLinkNotify_);

      const ARDrone_MessageData_t& data_container_r = message_in.get ();
      const struct ARDrone_MessageData& data_r = data_container_r.get ();

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

      const ARDrone_MessageData_t& data_container_r = message_in.get ();
      const struct ARDrone_MessageData& data_r = data_container_r.get ();

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
  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, GtkCBData_->lock);
    iterator =
      GtkCBData_->progressData->statistic.streamStatistic.find (stream_type_e);
    ACE_ASSERT (iterator != GtkCBData_->progressData->statistic.streamStatistic.end ());
    (*iterator).second.bytes += message_in.total_length ();
    GtkCBData_->progressData->statistic.bytes += message_in.total_length ();

    GtkCBData_->eventStack.push_back (std::make_pair (stream_type_e,
                                                      ARDRONE_EVENT_MESSAGE_DATA));
  } // end lock scope

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  if (refresh_display)
  {
    guint event_source_id = g_idle_add (idle_update_video_display_cb,
                                        GtkCBData_);
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
ARDrone_EventHandler::notify (Stream_SessionId_t sessionID_in,
                              const ARDrone_SessionMessage& message_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_EventHandler::notify"));

  int result = -1;
  enum ARDrone_StreamType stream_type_e = ARDRONE_STREAM_INVALID;
  enum ARDrone_EventType event_e = ARDRONE_EVENT_MESSAGE_SESSION;
  SESSIONID_TO_STREAM_MAP_ITERATOR_T iterator;
  ARDroneStreamStatisticIterator_t iterator_2;

  // sanity check(s)
  ACE_ASSERT (GtkCBData_);

  // update mapping ?
  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, GtkCBData_->lock);
    if (message_in.type () == STREAM_SESSION_MESSAGE_LINK)
    {
      const ARDrone_StreamSessionData_t& session_data_container_r =
        message_in.get ();
      struct ARDrone_SessionData& session_data_r =
        const_cast<struct ARDrone_SessionData&> (session_data_container_r.get ());
      iterator =
        std::find_if (streams_.begin (), streams_.end (),
                      std::bind2nd (SESSIONID_TO_STREAM_MAP_FIND_S (),
                                    session_data_r.state->type));
      ACE_ASSERT (iterator != streams_.end ());
      if ((*iterator).first != sessionID_in)
      {
        //ACE_DEBUG ((LM_DEBUG,
        //            ACE_TEXT ("updating session id (was: %u) --> %u\n"),
        //            (*iterator).first,
        //            sessionID_in));
        streams_.erase (iterator);
        streams_.insert (std::make_pair (sessionID_in,
                                         session_data_r.state->type));
        //(*iterator).first = sessionID_in;
      } // end IF
    } // end IF
    iterator = streams_.find (sessionID_in);
    // sanity check(s)
    if (iterator == streams_.end ())
    { // most probable reason: statistic messages arriving out-of-session
      //ACE_DEBUG ((LM_DEBUG,
      //            ACE_TEXT ("session message (type: %d): invalid session id (was: %u), returning\n"),
      //            message_in.type (),
      //            sessionID_in));
      return;
    } // end IF
    stream_type_e = (*iterator).second;
  } // end lock scope

  // sanity check(s)
  ACE_ASSERT (GtkCBData_->progressData);

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
      event_e = ARDRONE_EVENT_RESIZE;
      break;
    case STREAM_SESSION_MESSAGE_UNLINK:
      break;
    case STREAM_SESSION_MESSAGE_STATISTIC:
    {
      const ARDrone_StreamSessionData_t& session_data_container_r =
        message_in.get ();
      struct ARDrone_SessionData& session_data_r =
        const_cast<struct ARDrone_SessionData&> (session_data_container_r.get ());

      if (session_data_r.lock)
      {
        result = session_data_r.lock->acquire ();
        if (result == -1)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_SYNCH_MUTEX::acquire(): \"%m\", continuing\n")));
      } // end IF

      { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, GtkCBData_->lock);
        // *NOTE*: this merges and combines the statistic data
        GtkCBData_->progressData->statistic += session_data_r.statistic;
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

  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, GtkCBData_->lock);
    GtkCBData_->eventStack.push_back (std::make_pair (stream_type_e,
                                                      event_e));
  } // end lock scope
}

void
ARDrone_EventHandler::end (Stream_SessionId_t sessionID_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_EventHandler::end"));

  // sanity check(s)
  ACE_ASSERT (GtkCBData_);

  enum ARDrone_StreamType stream_type_e = ARDRONE_STREAM_INVALID;

  SESSIONID_TO_STREAM_MAP_ITERATOR_T iterator;
  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, GtkCBData_->lock);
    iterator = streams_.find (sessionID_in);
    ACE_ASSERT (iterator != streams_.end ());
    stream_type_e = (*iterator).second;
    streams_.erase (iterator);
  } // end lock scope

  if (stream_type_e == ARDRONE_STREAM_NAVDATA)
  {
    { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, GtkCBData_->lock);
      guint event_source_id = g_idle_add (idle_session_end_cb,
                                          GtkCBData_);
      if (event_source_id == 0)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to g_idle_add(idle_session_end_cb): \"%m\", returning\n")));
        return;
      } // end IF
      GtkCBData_->eventSourceIds.insert (event_source_id);
    } // end lock scope
  } // end IF
}
