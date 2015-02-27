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

#include "olimex_mod_mpu6050_module_eventhandler.h"

#include "ace/Guard_T.h"
#include "ace/Log_Msg.h"
#include "ace/Synch.h"

#include "olimex_mod_mpu6050_macros.h"
#include "olimex_mod_mpu6050_message.h"
#include "olimex_mod_mpu6050_sessionmessage.h"

Olimex_Mod_MPU6050_Module_EventHandler::Olimex_Mod_MPU6050_Module_EventHandler ()
 : //inherited (),
   lock_ (NULL)
 , subscribers_ (NULL)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_Module_EventHandler::Olimex_Mod_MPU6050_Module_EventHandler"));

}

Olimex_Mod_MPU6050_Module_EventHandler::~Olimex_Mod_MPU6050_Module_EventHandler ()
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_Module_EventHandler::~Olimex_Mod_MPU6050_Module_EventHandler"));

}

void
Olimex_Mod_MPU6050_Module_EventHandler::initialize (Olimex_Mod_MPU6050_Subscribers_t* subscribers_in,
                                                    ACE_Recursive_Thread_Mutex* lock_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_Module_EventHandler::initialize"));

  // sanity check(s)
  ACE_ASSERT (subscribers_in);
  ACE_ASSERT (lock_in);

  lock_ = lock_in;
  subscribers_ = subscribers_in;
}

void
Olimex_Mod_MPU6050_Module_EventHandler::handleDataMessage (Olimex_Mod_MPU6050_Message*& message_inout,
                                                           bool& passMessageDownstream_out)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_Module_EventHandler::handleDataMessage"));

  // don't care (implies yes per default, if we're part of a stream)
  ACE_UNUSED_ARG (passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT (message_inout);

//   try
//   {
//     message_inout->getData ()->dump_state ();
//   }
//   catch (...)
//   {
//     ACE_DEBUG ((LM_ERROR,
//                 ACE_TEXT ("caught exception in Common_IDumpState::dump_state(), continuing\n")));
//   }

  // refer the data back to any subscriber(s)

  // synch access
  {
    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard (*lock_);

    // *WARNING* if users unsubscribe() within the callback Bad Things (TM)
    // would happen, as the current iter would be invalidated
    // --> use a slightly modified for-loop (advance first and THEN invoke the
    // callback (*NOTE*: works for MOST containers...)
    // *NOTE*: this works due to the ACE_RECURSIVE_Thread_Mutex used as a lock...
    for (Olimex_Mod_MPU6050_SubscribersIterator_t iterator = subscribers_->begin ();
         iterator != subscribers_->end ();)
    {
      try
      {
        (*iterator++)->notify (*message_inout);
      }
      catch (...)
      {
        ACE_DEBUG((LM_ERROR,
                   ACE_TEXT ("caught exception in Common_INotify::notify (), continuing\n")));
      }
    } // end FOR
  } // end lock scope
}

void
Olimex_Mod_MPU6050_Module_EventHandler::handleSessionMessage (Olimex_Mod_MPU6050_SessionMessage*& message_inout,
                                                              bool& passMessageDownstream_out)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_Module_EventHandler::handleSessionMessage"));

  // don't care (implies yes per default, if we're part of a stream)
  ACE_UNUSED_ARG (passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT (message_inout);

  switch (message_inout->getType ())
  {
    case SESSION_BEGIN:
    {
      // refer the data back to any subscriber(s)

      // synch access
      {
        ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard (*lock_);

        // *WARNING* if users unsubscribe() within the callback Bad Things (TM)
        // would happen, as the current iter would be invalidated
        // --> use a slightly modified for-loop (advance first and THEN invoke the
        // callback (*NOTE*: works for MOST containers...)
        // *NOTE*: this works due to the ACE_RECURSIVE_Thread_Mutex used as a lock...
        for (Olimex_Mod_MPU6050_SubscribersIterator_t iterator = subscribers_->begin ();
             iterator != subscribers_->end ();)
        {
          try
          {
            (*iterator++)->start ();
          }
          catch (...)
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("caught exception in Common_INotify::start(), continuing\n")));
          }
        } // end FOR
      } // end lock scope

      break;
    }
    case SESSION_END:
    {
      // refer the data back to any subscriber(s)

      // synch access
      {
        ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard (*lock_);

        // *WARNING* if users unsubscribe() within the callback Bad Things (TM)
        // would happen, as the current iter would be invalidated
        // --> use a slightly modified for-loop (advance first and THEN invoke the
        // callback (*NOTE*: works for MOST containers...)
        // *NOTE*: this works due to the ACE_RECURSIVE_Thread_Mutex used as a lock...
        for (Olimex_Mod_MPU6050_SubscribersIterator_t iterator = subscribers_->begin ();
             iterator != subscribers_->end ();)
        {
          try
          {
            (*(iterator++))->end ();
          }
          catch (...)
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("caught exception in Common_INotify::end(), continuing\n")));
          }
        } // end FOR
      } // end lock scope

      break;
    }
    default:
      break;
  } // end SWITCH
}

void
Olimex_Mod_MPU6050_Module_EventHandler::subscribe (Olimex_Mod_MPU6050_Notification_t* interfaceHandle_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_Module_EventHandler::subscribe"));

  // sanity check(s)
  ACE_ASSERT (interfaceHandle_in);

  // synch access to subscribers
  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard (*lock_);

  subscribers_->push_back (interfaceHandle_in);
}

void
Olimex_Mod_MPU6050_Module_EventHandler::unsubscribe (Olimex_Mod_MPU6050_Notification_t* interfaceHandle_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_Module_EventHandler::unsubscribe"));

  // sanity check(s)
  ACE_ASSERT (interfaceHandle_in);

  // synch access to subscribers
  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard (*lock_);

  Olimex_Mod_MPU6050_SubscribersIterator_t iterator = subscribers_->begin ();
  for (;
       iterator != subscribers_->end ();
       iterator++)
    if ((*iterator) == interfaceHandle_in)
      break;

  if (iterator != subscribers_->end ())
    subscribers_->erase (iterator);
  else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("invalid argument (was: %@), aborting\n"),
                interfaceHandle_in));
}

Common_Module_t*
Olimex_Mod_MPU6050_Module_EventHandler:: clone ()
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_Module_EventHandler::clone"));

  // init return value(s)
  Common_Module_t* result = NULL;

  ACE_NEW_NORETURN (result,
                    Olimex_Mod_MPU6050_Module_EventHandler_Module (ACE_TEXT_ALWAYS_CHAR (inherited::name ()),
                                                                   NULL));
  if (!result)
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate memory(%u): %m, aborting\n"),
                sizeof (Olimex_Mod_MPU6050_Module_EventHandler_Module)));
  else
  {
    Olimex_Mod_MPU6050_Module_EventHandler* eventHandler_impl = NULL;
    eventHandler_impl = dynamic_cast<Olimex_Mod_MPU6050_Module_EventHandler*> (result->writer ());
    if (!eventHandler_impl)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("dynamic_cast<Olimex_Mod_MPU6050_Module_EventHandler> failed, aborting\n")));

      // clean up
      delete result;

      return NULL;
    } // end IF
    eventHandler_impl->initialize (subscribers_,
                                   lock_);
  } // end ELSE

  return result;
}
