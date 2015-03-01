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

#include "olimex_mod_mpu6050_signalhandler.h"

#include "ace/Log_Msg.h"

#include "common_tools.h"

#include "common_ui_gtk_manager.h"

#include "olimex_mod_mpu6050_macros.h"

Olimex_Mod_MPU6050_SignalHandler::Olimex_Mod_MPU6050_SignalHandler (const ACE_INET_Addr& peerAddress_in,
                                                                    Olimex_Mod_MPU6050_IConnector_t* interfaceHandle_in,
                                                                    // ---------
                                                                    bool useReactor_in)
 : inherited (this,          // event handler handle
              useReactor_in) // use reactor ?
 , interfaceHandle_ (interfaceHandle_in)
 , peerAddress_ (peerAddress_in)
 , useReactor_ (useReactor_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_SignalHandler::Olimex_Mod_MPU6050_SignalHandler"));

}

Olimex_Mod_MPU6050_SignalHandler::~Olimex_Mod_MPU6050_SignalHandler ()
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_SignalHandler::~Olimex_Mod_MPU6050_SignalHandler"));

}

bool
Olimex_Mod_MPU6050_SignalHandler::handleSignal (int signal_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_SignalHandler::handleSignal"));

  bool stop_event_dispatching = false;
  bool connect = false;
  bool abort = false;
  switch (signal_in)
  {
    case SIGINT:
// *PORTABILITY*: on Windows SIGQUIT is not defined
#if !defined (ACE_WIN32) && !defined (ACE_WIN64)
    case SIGQUIT:
#endif
    {
      //ACE_DEBUG ((LM_DEBUG,
      //            ACE_TEXT ("shutting down...\n")));

      // shutdown...
      stop_event_dispatching = true;

      break;
    }
// *PORTABILITY*: on Windows SIGUSRx are not defined
// --> use SIGBREAK (21) and SIGTERM (15) instead...
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
      // (try to) abort a connection...
      abort = true;

      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("received invalid/unknown signal: \"%S\", aborting\n"),
                  signal_in));

      return false;
    }
  } // end SWITCH

  // ...abort ?
  if (abort)
  {
    // release an existing connection...
    CONNECTIONMANAGER_SINGLETON::instance ()->abortOldestConnection ();
  } // end IF

  // ...connect ?
  if (connect &&
      interfaceHandle_)
  {
    try
    {
      interfaceHandle_->connect (peerAddress_);
    }
    catch (...)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in Olimex_Mod_MPU6050_IConnector_t::connect(), aborting\n")));

      return false;
    }
  } // end IF

  // ...shutdown ?
  if (stop_event_dispatching)
  {
    // stop everything, i.e.
    // - leave reactor event loop handling signals, sockets, (maintenance) timers...
    // --> (try to) terminate in a well-behaved manner

    // step1: close open connection(s)
    if (interfaceHandle_)
    {
      try
      {
        interfaceHandle_->abort ();
      }
      catch (...)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in Olimex_Mod_MPU6050_IConnector_t::abort(), aborting\n")));

        return false;
      }
    } // end IF
    CONNECTIONMANAGER_SINGLETON::instance ()->stop ();
    CONNECTIONMANAGER_SINGLETON::instance ()->abortConnections ();
    // *IMPORTANT NOTE*: as long as connections are inactive (i.e. events are
    // dispatched by reactor thread(s), there is no real reason to wait here)
    //CONNECTIONMANAGER_SINGLETON::instance ()->waitConnections ();

    // step2: stop GTK event dispatch
    COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->stop ();

    // step3: stop reactor (&& proactor, if applicable)
    Common_Tools::finalizeEventDispatch (true,         // stop reactor ?
                                         !useReactor_, // stop proactor ?
                                         -1);          // group ID (--> don't block !)
  } // end IF

  return true;
}
