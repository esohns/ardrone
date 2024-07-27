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

#include "ace/Log_Msg.h"

#include "common_tools.h"

#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
#include "common_ui_gtk_manager_common.h"
#endif // GTK_USE
#endif // GUI_SUPPORT

//#include "ardrone_configuration.h"
#include "ardrone_macros.h"
//#include "ardrone_network.h"

template <typename ConfigurationType>
Test_U_SignalHandler_T<ConfigurationType>::Test_U_SignalHandler_T ()
 : inherited (this)
{
  ARDRONE_TRACE (ACE_TEXT ("Test_U_SignalHandler_T::Test_U_SignalHandler_T"));

}

template <typename ConfigurationType>
bool
Test_U_SignalHandler_T<ConfigurationType>::initialize (const ConfigurationType& configuration_in)
{
  ARDRONE_TRACE (ACE_TEXT ("Test_U_SignalHandler_T::initialize"));

  // *TODO*: remove type inference
//  connector_ = configuration_in.connector;

  return inherited::initialize (configuration_in);
}

template <typename ConfigurationType>
void
Test_U_SignalHandler_T<ConfigurationType>::handle (const struct Common_Signal& signal_in)
{
  ARDRONE_TRACE (ACE_TEXT ("Test_U_SignalHandler_T::handle"));

  // sanity check(s)
  ACE_ASSERT (inherited::isInitialized_);

  bool stop_event_dispatching = false;
  bool connect = false;
//  bool abort = false;
  switch (signal_in.signal)
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
                  signal_in.signal));
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
//  if (connect &&
//      connector_)
//  {
//    try {
//      connector_->connect (inherited::configuration_->peerAddress);
//    } catch (...) {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("caught exception in ARDrone_IConnector_t::connect(), returning\n")));
//      return;
//    }
//  } // end IF

  // ...shutdown ?
  if (stop_event_dispatching)
  { ACE_ASSERT (inherited::configuration_->dispatchState);
    // stop everything, i.e.
    // - leave reactor event loop handling signals, sockets, (maintenance) timers
    // --> (try to) terminate in a well-behaved manner

    // step1: close open connection attempt(s)
//    if (connector_ &&
//        !connector_->useReactor ())
//    {
//      typename ConnectorType::IASYNCH_CONNECTOR_T* iasynch_connector_p =
//        dynamic_cast<typename ConnectorType::IASYNCH_CONNECTOR_T*> (connector_);
//      ACE_ASSERT (iasynch_connector_p);
//      try {
//        iasynch_connector_p->abort ();
//      } catch (...) {
//        ACE_DEBUG ((LM_ERROR,
//                    ACE_TEXT ("caught exception in Net_IAsynchConnector_T::abort(), returning\n")));
//        return;
//      }
//    } // end IF

//    ARDRONE_WLANMONITOR_SINGLETON::instance ()->stop (true,   // wait ?
//                                                      false); // N/A

    // step2: stop UI event dispatch ?
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
//      ARDRONE_GTK_MANAGER_SINGLETON::instance ()->stop (true,   // wait ?
//                                                        false); // N/A
#endif // GTK_USE
#endif // GUI_SUPPORT

    // step3: stop reactor (&& proactor, if applicable)
    Common_Event_Tools::finalizeEventDispatch (*inherited::configuration_->dispatchState,
                                               false,  // don't block
                                               false); // delete singletons ?
  } // end IF
}
