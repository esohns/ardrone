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
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_EventHandler::ARDrone_Module_EventHandler"));

}

ARDrone_Module_EventHandler::~ARDrone_Module_EventHandler ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_EventHandler::~ARDrone_Module_EventHandler"));

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
                                                        ACE_TEXT_ALWAYS_CHAR (inherited::name ()),
                                                        NULL,
                                                        false));
  if (!module_p)
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory(%u): %m, aborting\n"),
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
                  ACE_TEXT ("dynamic_cast<ARDrone_Module_EventHandler> failed, aborting\n")));

      // clean up
      delete module_p;

      return NULL;
    } // end IF
    eventHandler_impl->initialize (*inherited::configuration_,
                                   inherited::allocator_);
  } // end ELSE

  return task_p;
}
