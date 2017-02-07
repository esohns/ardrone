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
                             const ARDrone_SessionData& sessionData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_EventHandler::start"));

  // sanity check(s)
  ACE_ASSERT (GtkCBData_);

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

    GtkCBData_->eventStack.push_back (ARDRONE_EVENT_CONNECT);
  } // end lock scope
}

void
ARDrone_EventHandler::notify (Stream_SessionId_t sessionID_in,
                              const Stream_SessionMessageType& sessionMessage_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_EventHandler::notify"));

}

void
ARDrone_EventHandler::notify (Stream_SessionId_t sessionID_in,
                              const ARDrone_Message& message_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_EventHandler::notify"));

  // sanity check(s)
  ACE_ASSERT (GtkCBData_);
  //ACE_ASSERT (GtkCBData_->progressData);

  ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, GtkCBData_->lock);

  //GtkCBData_->progressData->statistic.bytes += message_in.total_length ();
  GtkCBData_->eventStack.push_back (ARDRONE_EVENT_MESSAGE);

  guint event_source_id = g_idle_add (idle_update_video_display_cb,
                                      GtkCBData_);
  if (event_source_id == 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to g_idle_add(idle_update_video_display_cb): \"%m\", returning\n")));
    return;
  } // end IF
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
    case STREAM_SESSION_MESSAGE_ABORT:
    case STREAM_SESSION_MESSAGE_DISCONNECT:
    case STREAM_SESSION_MESSAGE_LINK:
    {
      event = ARDRONE_EVENT_SESSION_MESSAGE;
      break;
    }
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
