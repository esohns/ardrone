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
 , controller_ (NULL)
 , GtkCBData_ (GtkCBData_in)
 , navDataSessionStarted_ (false)
 , videoModeSet_ (false)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_EventHandler::ARDrone_EventHandler"));

  // sanity check(s)
  ACE_ASSERT (GtkCBData_);
}

ARDrone_EventHandler::~ARDrone_EventHandler ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_EventHandler::~ARDrone_EventHandler"));

}

void
ARDrone_EventHandler::start (Stream_SessionId_t sessionID_in,
                             const struct ARDrone_SessionData& sessionData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_EventHandler::start"));

  // sanity check(s)
  ACE_ASSERT (GtkCBData_);

  if (sessionData_in.isNavData)
    navDataSessionStarted_ = true;

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
}

void
ARDrone_EventHandler::notify (Stream_SessionId_t sessionID_in,
                              const enum Stream_SessionMessageType& sessionMessage_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_EventHandler::notify"));

}

//void
//ARDrone_EventHandler::notify (Stream_SessionId_t sessionID_in,
//                              const ARDrone_MAVLinkMessage& message_in)
//{
//  ARDRONE_TRACE (ACE_TEXT ("ARDrone_EventHandler::notify"));

//  // sanity check(s)
//  ACE_ASSERT (GtkCBData_);
//  //ACE_ASSERT (GtkCBData_->progressData);

//  ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, GtkCBData_->lock);

//  //GtkCBData_->progressData->statistic.bytes += message_in.total_length ();
//  GtkCBData_->eventStack.push_back (ARDRONE_EVENT_MESSAGE);
//}
//void
//ARDrone_EventHandler::notify (Stream_SessionId_t sessionID_in,
//                              const ARDrone_NavDataMessage& message_in)
//{
//  ARDRONE_TRACE (ACE_TEXT ("ARDrone_EventHandler::notify"));

//  // sanity check(s)
//  ACE_ASSERT (GtkCBData_);
//  //ACE_ASSERT (GtkCBData_->progressData);

//  ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, GtkCBData_->lock);

//  //GtkCBData_->progressData->statistic.bytes += message_in.total_length ();
//  GtkCBData_->eventStack.push_back (ARDRONE_EVENT_MESSAGE);
//}
void
ARDrone_EventHandler::notify (Stream_SessionId_t sessionID_in,
                              const ARDrone_Message& message_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_EventHandler::notify"));

  // sanity check(s)
  ACE_ASSERT (GtkCBData_);

  bool refresh_display = false;

  switch (message_in.type ())
  {
    case ARDRONE_MESSAGE_ATCOMMANDMESSAGE:
      break; // do not count outbound messages
    case ARDRONE_MESSAGE_LIVEVIDEOFRAME:
    {
      refresh_display = true;

      break;
    }
    case ARDRONE_MESSAGE_MAVLINKMESSAGE:
    {
      const ARDrone_MessageData_t& data_container_r =
          message_in.get ();
      const struct ARDrone_MessageData& data_r =
          data_container_r.get ();

      try {
        // *TODO*: remove type inference
        messageCB (data_r.MAVLinkMessage,
                   message_in.rd_ptr ());
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in ARDrone_INotify::messageCB(), returning\n")));
      }

      break;
    }
    case ARDRONE_MESSAGE_NAVDATAMESSAGE:
    {
      const ARDrone_MessageData_t& data_container_r =
          message_in.get ();
      const struct ARDrone_MessageData& data_r =
          data_container_r.get ();

      try {
        messageCB (data_r.NavDataMessage,
                   data_r.NavDataMessageOptionOffsets,
                   message_in.rd_ptr ());
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in ARDrone_INotify::messageCB(), returning\n")));
      }

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

  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, GtkCBData_->lock);

    GtkCBData_->eventStack.push_back (ARDRONE_EVENT_MESSAGE);
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

  // sanity check(s)
  ACE_ASSERT (GtkCBData_);
  ACE_ASSERT (GtkCBData_->progressData);

  ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, GtkCBData_->lock);

  int result = -1;
  enum ARDrone_Event event = ARDRONE_EVENT_INVALID;
  switch (message_in.type ())
  {
    case STREAM_SESSION_MESSAGE_CONNECT:
      event = ARDRONE_EVENT_CONNECT; break;
    case STREAM_SESSION_MESSAGE_DISCONNECT:
      event = ARDRONE_EVENT_DISCONNECT; break;
    case STREAM_SESSION_MESSAGE_ABORT:
    case STREAM_SESSION_MESSAGE_LINK:
      event = ARDRONE_EVENT_SESSION_MESSAGE; break;
    case STREAM_SESSION_MESSAGE_RESIZE:
      event = ARDRONE_EVENT_RESIZE; break;
    case STREAM_SESSION_MESSAGE_UNLINK:
      event = ARDRONE_EVENT_SESSION_MESSAGE; break;
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

      GtkCBData_->progressData->statistic = session_data_r.currentStatistic;

      if (session_data_r.lock)
      {
        result = session_data_r.lock->release ();
        if (result == -1)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("failed to ACE_SYNCH_MUTEX::release(): \"%m\", continuing\n")));
      } // end IF

      event = ARDRONE_EVENT_SESSION_MESSAGE;
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
  GtkCBData_->eventStack.push_back (event);
}

void
ARDrone_EventHandler::end (Stream_SessionId_t sessionID_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_EventHandler::end"));

  navDataSessionStarted_ = false;
  videoModeSet_ = false;

  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, GtkCBData_->lock);

    GtkCBData_->eventStack.push_back (ARDRONE_EVENT_DISCONNECT);

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
}

void
ARDrone_EventHandler::messageCB (const struct __mavlink_message& record_in,
                                 void* payload_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_EventHandler::messageCB"));

  ACE_UNUSED_ARG (record_in);
  ACE_UNUSED_ARG (payload_in);

  if (navDataSessionStarted_ && !videoModeSet_)
  {
    // sanity check(s)
    ACE_ASSERT (controller_);

    try {
      controller_->set (ARDRONE_VIDEOMODE_720P);
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in ARDrone_IController::set(%d), returning\n"),
                  ARDRONE_VIDEOMODE_720P));
      return;
    }

    videoModeSet_ = true;
  } // end IF
}
void
ARDrone_EventHandler::messageCB (const struct _navdata_t& record_in,
                                 const ARDrone_NavDataMessageOptionOffsets_t& offsets_in,
                                 void* payload_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_EventHandler::messageCB"));

  ACE_ASSERT (false);
  ACE_NOTSUP;
  ACE_NOTREACHED (return;)
}
