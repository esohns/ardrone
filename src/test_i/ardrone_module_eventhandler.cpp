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
#include "ardrone_module_eventhandler.h"

#include "ace/Log_Msg.h"

#include "ardrone_macros.h"

ARDrone_Module_EventHandler::ARDrone_Module_EventHandler (ISTREAM_T* stream_in)
 : inherited (stream_in)
 , streams_ ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_EventHandler::ARDrone_Module_EventHandler"));

}

void
ARDrone_Module_EventHandler::handleDataMessage (ARDrone_Message*& message_inout,
                                                bool& passMessageDownstream_out)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_EventHandler::handleDataMessage"));

  inherited::handleDataMessage (message_inout,
                                passMessageDownstream_out);
  if (!passMessageDownstream_out)
    return;

  //enum ARDrone_StreamType stream_type_e = ARDRONE_STREAM_INVALID;
  //switch (message_inout->type ())
  //{
  //  case ARDRONE_MESSAGE_ATCOMMAND:
  //    stream_type_e = ARDRONE_STREAM_NAVDATA; break;
  //  case ARDRONE_MESSAGE_CONTROL:
  //    stream_type_e = ARDRONE_STREAM_CONTROL; break;
  //  case ARDRONE_MESSAGE_MAVLINK:
  //    stream_type_e = ARDRONE_STREAM_MAVLINK; break;
  //  case ARDRONE_MESSAGE_NAVDATA:
  //    stream_type_e = ARDRONE_STREAM_NAVDATA; break;
  //  case ARDRONE_MESSAGE_VIDEO:
  //    stream_type_e = ARDRONE_STREAM_VIDEO; break;
  //  default:
  //  {
  //    ACE_DEBUG ((LM_ERROR,
  //                ACE_TEXT ("%s: invalid/unknown message type (was: %d), returning\n"),
  //                inherited::mod_->name (),
  //                message_inout->type ()));
  //    return;
  //  }
  //} // end SWITCH

  if (message_inout->type () != ARDRONE_MESSAGE_ATCOMMAND)
  {
    message_inout->release ();
    message_inout = NULL;
    passMessageDownstream_out = false;
  } // end IF
}
void
ARDrone_Module_EventHandler::handleSessionMessage (ARDrone_SessionMessage*& message_inout,
                                                   bool& passMessageDownstream_out)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_EventHandler::handleSessionMessage"));

  Stream_SessionId_t session_id = message_inout->sessionId ();
  SESSIONID_TO_STREAM_MAP_ITERATOR_T iterator = streams_.find (session_id);
  const ARDrone_StreamSessionData_t& session_data_container_r =
    message_inout->getR ();
  struct ARDrone_SessionData& session_data_r =
    const_cast<struct ARDrone_SessionData&> (session_data_container_r.getR ());
  if (iterator == streams_.end ())
    streams_.insert (std::make_pair (session_id,
                                     session_data_r.state->type));

  if (message_inout->type () == STREAM_SESSION_MESSAGE_STATISTIC)
  {
    // retain statistic data for each stream separately
    // *TODO*: consider moving this into a base-class
    SESSION_DATA_ITERATOR_T iterator_2;
    struct ARDrone_SessionData* session_data_p = NULL;
    { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, inherited::lock_);
      iterator_2 = inherited::sessionData_.find (session_id);
      if (iterator_2 == inherited::sessionData_.end ())
        goto continue_;
      ACE_ASSERT ((*iterator_2).second);
      session_data_p =
        &const_cast<struct ARDrone_SessionData&> ((*iterator_2).second->getR ());
      ACE_ASSERT (session_data_p->lock);
      { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, *session_data_p->lock);
        // *NOTE*: this merges and combines the statistic data
        session_data_p->statistic += session_data_r.statistic;
      } // end lock scope
    } // end lock scope

    // update message session data
    ACE_ASSERT (iterator != streams_.end ());
    ARDroneStreamStatisticIterator_t iterator_3;
    ACE_ASSERT (session_data_r.lock);
    { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, *session_data_r.lock);
      // *TODO*: consider locking session_data_p here
      session_data_r.statistic = session_data_p->statistic;
    } // end lock scope
  } // end IF

continue_:
  inherited::handleSessionMessage (message_inout,
                                   passMessageDownstream_out);
  if (!passMessageDownstream_out)
    return;
}

ACE_Task<ACE_MT_SYNCH,
         Common_TimePolicy_t>*
ARDrone_Module_EventHandler:: clone ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_EventHandler::clone"));

  // initialize return value(s)
  Stream_Task_t* task_p = NULL;

  Stream_Module_t* module_p = NULL;
  ACE_NEW_NORETURN (module_p,
                    ARDrone_Module_EventHandler_Module (NULL,
                                                        ACE_TEXT_ALWAYS_CHAR (inherited::name ())));
  if (!module_p)
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("%s: failed to allocate memory(%u): %m, aborting\n"),
                inherited::mod_->name (),
                sizeof (ARDrone_Module_EventHandler_Module)));
  else
  {
    task_p = module_p->writer ();
    ACE_ASSERT (task_p);

    ARDrone_Module_EventHandler* eventHandler_impl =
        dynamic_cast<ARDrone_Module_EventHandler*> (task_p);
    if (!eventHandler_impl)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: dynamic_cast<ARDrone_Module_EventHandler> failed, aborting\n"),
                  inherited::mod_->name ()));

      // clean up
      delete module_p;

      return NULL;
    } // end IF
    eventHandler_impl->initialize (*inherited::configuration_,
                                   inherited::allocator_);
  } // end ELSE

  return task_p;
}
