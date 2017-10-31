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

#include "ardrone_defines.h"
#include "ardrone_macros.h"

const char ardrone_default_handler_module_name_string[] =
  ACE_TEXT_ALWAYS_CHAR (ARDRONE_STREAM_MDOULE_HANDLER_NAME_STRING);

ARDrone_Module_EventHandler::ARDrone_Module_EventHandler (ISTREAM_T* stream_in)
 : inherited (stream_in)
 //, streams_ ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_EventHandler::ARDrone_Module_EventHandler"));

}

void
ARDrone_Module_EventHandler::handleDataMessage (ARDrone_Message*& message_inout,
                                                bool& passMessageDownstream_out)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_EventHandler::handleDataMessage"));

  // *NOTE*: messages traversing this module will be sent to the device
  //         --> filter inbound NavData here

  enum ARDrone_MessageType message_type_e = message_inout->type ();
  ARDrone_Message* message_p = message_inout;

  // the base class release()s all messages --> create duplicates
  if (message_type_e == ARDRONE_MESSAGE_ATCOMMAND)
  {
    message_p =
      dynamic_cast<ARDrone_Message*> (message_inout->duplicate ());
    ACE_ASSERT (message_p);
  } // end IF
  inherited::handleDataMessage (message_p,
                                passMessageDownstream_out);
  ACE_ASSERT (!message_p);
  ACE_ASSERT (!passMessageDownstream_out);

  if (message_type_e == ARDRONE_MESSAGE_ATCOMMAND)
  {
    // sanity check(s)
    ACE_ASSERT (message_inout);

    passMessageDownstream_out = true;
  } // end IF
}

void
ARDrone_Module_EventHandler::handleSessionMessage (ARDrone_SessionMessage*& message_inout,
                                                   bool& passMessageDownstream_out)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_EventHandler::handleSessionMessage"));

  Stream_SessionId_t session_id = message_inout->sessionId ();
  const ARDrone_SessionData_t& session_data_container_r =
    message_inout->getR ();
  struct ARDrone_SessionData& session_data_r =
    const_cast<struct ARDrone_SessionData&> (session_data_container_r.getR ());

  if (message_inout->type () == STREAM_SESSION_MESSAGE_STATISTIC)
  {
    // retain statistic data for each stream separately
    // *TODO*: move this into the base-class

    SESSION_DATA_ITERATOR_T iterator_2;
    struct ARDrone_SessionData* session_data_p = NULL;
    ARDroneStreamStatisticIterator_t iterator_3, iterator_4;
    struct ARDrone_Statistic statistic_s;
    { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, inherited::lock_);
      // step1: 'sink' this streams' data
      iterator_2 = inherited::sessionData_.find (session_id);
      if (iterator_2 == inherited::sessionData_.end ())
        goto continue_;
      ACE_ASSERT ((*iterator_2).second);
      session_data_p =
        &const_cast<struct ARDrone_SessionData&> ((*iterator_2).second->getR ());
      ACE_ASSERT (session_data_p->lock);
      ACE_ASSERT (session_data_p->state);
      { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard_2, *session_data_p->lock);
        iterator_3 =
            session_data_p->statistic.streamStatistic.find (session_data_p->state->type);
        ACE_ASSERT (iterator_3 != session_data_p->statistic.streamStatistic.end ());
        (*iterator_3).second = session_data_r.statistic;
      } // end lock scope

      // step2: merge everything
      for (iterator_2 = inherited::sessionData_.begin ();
           iterator_2 != inherited::sessionData_.end ();
           ++iterator_2)
      {
        session_data_p =
          &const_cast<struct ARDrone_SessionData&> ((*iterator_2).second->getR ());
        ACE_ASSERT (session_data_p->state);
        iterator_3 =
            session_data_p->statistic.streamStatistic.find (session_data_p->state->type);
        ACE_ASSERT (iterator_3 != session_data_p->statistic.streamStatistic.end ());
        iterator_4 =
            statistic_s.streamStatistic.find (session_data_p->state->type);
        ACE_ASSERT (iterator_4 != statistic_s.streamStatistic.end ());
        (*iterator_4).second = (*iterator_3).second;
      } // end FOR
    } // end lock scope
    // step3: combine everything
    +statistic_s;

    // update message session data
    ACE_ASSERT (session_data_r.lock);
    { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard_3, *session_data_r.lock);
      session_data_r.statistic = statistic_s;
    } // end lock scope
  } // end IF

continue_:
  inherited::handleSessionMessage (message_inout,
                                   passMessageDownstream_out);
  ACE_ASSERT (!message_inout);
  ACE_ASSERT (!passMessageDownstream_out);
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
