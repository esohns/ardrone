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
#include "ardrone_signalhandler.h"

#include "ace/Log_Msg.h"

#include "common_tools.h"

#include "common_ui_gtk_manager_common.h"

#include "ardrone_configuration.h"
#include "ardrone_macros.h"

ARDrone_SignalHandler::ARDrone_SignalHandler ()
 : inherited (NULL)
 , connector_ (NULL)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_SignalHandler::ARDrone_SignalHandler"));

}

bool
ARDrone_SignalHandler::initialize (const struct ARDrone_SignalHandlerConfiguration& configuration_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_SignalHandler::initialize"));

  // *TODO*: remove type inference
  connector_ = configuration_in.connector;

  return inherited::initialize (configuration_in);
}

void
ARDrone_SignalHandler::handle (int signal_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_SignalHandler::handle"));

  // sanity check(s)
  ACE_ASSERT (inherited::isInitialized_);

  bool stop_event_dispatching = false;
  bool connect = false;
//  bool abort = false;
  switch (signal_in)
  {
    case SIGINT:
// *PORTABILITY*: on Windows SIGQUIT is not defined
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
    case SIGQUIT:
#endif
    {
      //ACE_DEBUG ((LM_DEBUG,
      //            ACE_TEXT ("shutting down\n")));

      // shutdown...
      stop_event_dispatching = true;

      break;
    }
// *PORTABILITY*: on Windows SIGUSRx are not defined
// --> use SIGBREAK (21) and SIGTERM (15) instead
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
    case SIGUSR1:
#else
    case SIGBREAK:
#endif
    {
      // (try to) connect...
      connect = true;

      break;
    }
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
    case SIGHUP:
    case SIGUSR2:
#endif
    case SIGTERM:
    {
//      // (try to) abort a connection
//      abort = true;

      break;
    }
    case SIGABRT:
    case SIGCHLD:
      break;
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("received invalid/unknown signal: \"%S\", returning\n"),
                  signal_in));
      return;
    }
  } // end SWITCH

//  // ...abort ?
//  if (abort)
//  {
//    // close any connections
//    CONNECTIONMANAGER_SINGLETON::instance ()->abortConnections ();
//  } // end IF

  // ...connect ?
  if (connect &&
      connector_)
  {
    try {
      connector_->connect (inherited::configuration_->peerAddress);
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in ARDrone_IConnector_t::connect(), returning\n")));
      return;
    }
  } // end IF

  // ...shutdown ?
  if (stop_event_dispatching)
  {
    // stop everything, i.e.
    // - leave reactor event loop handling signals, sockets, (maintenance) timers
    // --> (try to) terminate in a well-behaved manner

    // step1: close open connection attempt(s)
    if (connector_ &&
        !connector_->useReactor ())
    {
      ARDrone_IAsynchConnector_t* iasynch_connector_p =
        dynamic_cast<ARDrone_IAsynchConnector_t*> (connector_);
      ACE_ASSERT (iasynch_connector_p);
      try {
        iasynch_connector_p->abort ();
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in ARDrone_IAsynchConnector_t::abort(), returning\n")));
        return;
      }
    } // end IF

    // step2: stop GTK event dispatch ?
    if (inherited::configuration_->hasUI)
      ARDRONE_UI_GTK_MANAGER_SINGLETON::instance ()->stop (false,  // wait ?
                                                           false); // N/A
    else
    {
      // step3: stop reactor (&& proactor, if applicable)
      Common_Tools::finalizeEventDispatch (inherited::configuration_->useReactor,  // stop reactor ?
                                           !inherited::configuration_->useReactor, // stop proactor ?
                                           -1);                                    // group ID (--> don't block !)
    } // end ELSE
  } // end IF
}
