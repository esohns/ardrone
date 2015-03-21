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

#include "olimex_mod_mpu6050_eventhandler.h"

#include "ace/Synch.h"

#include "stream_common.h"

#include "olimex_mod_mpu6050_macros.h"
#include "olimex_mod_mpu6050_types.h"

Olimex_Mod_MPU6050_EventHandler::Olimex_Mod_MPU6050_EventHandler (Olimex_Mod_MPU6050_GtkCBData_t* GtkCBData_in)
 : GtkCBData_ (GtkCBData_in)
 , streamState_ (NULL)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_EventHandler::Olimex_Mod_MPU6050_EventHandler"));

  // sanity check(s)
  ACE_ASSERT (GtkCBData_);
}

Olimex_Mod_MPU6050_EventHandler::~Olimex_Mod_MPU6050_EventHandler ()
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_EventHandler::~Olimex_Mod_MPU6050_EventHandler"));

}

void
Olimex_Mod_MPU6050_EventHandler::start (const Stream_State_t& streamState_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_EventHandler::start"));

  streamState_ = &const_cast<Stream_State_t&> (streamState_in);

  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard (GtkCBData_->lock);

  GtkCBData_->eventQueue.push_back (OLIMEX_MOD_MPU6050_EVENT_CONNECT);
}

void
Olimex_Mod_MPU6050_EventHandler::notify (const Olimex_Mod_MPU6050_Message& message_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_EventHandler::notify"));

  Olimex_Mod_MPU6050_Message* message_p =
      dynamic_cast<Olimex_Mod_MPU6050_Message*> (message_in.duplicate ());
  if (!message_p)
  {
    // no message buffer available --> discard message
    //ACE_DEBUG ((LM_DEBUG,
    //            ACE_TEXT ("Olimex_Mod_MPU6050_Message::duplicate() failed, continuing\n")));
    if (streamState_)
      streamState_->currentStatistics.numDroppedMessages++;

    return;
  } // end IF

  {
    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard (GtkCBData_->lock);

    GtkCBData_->eventQueue.push_back (OLIMEX_MOD_MPU6050_EVENT_MESSAGE);
    GtkCBData_->messageQueue.push_front (message_p);
  } // end lock scope
}

void
Olimex_Mod_MPU6050_EventHandler::end ()
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_EventHandler::end"));

  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard (GtkCBData_->lock);

  GtkCBData_->eventQueue.push_back (OLIMEX_MOD_MPU6050_EVENT_DISCONNECT);
}

void
Olimex_Mod_MPU6050_EventHandler::initialize (Stream_State_t* streamState_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_EventHandler::initialize"));

  streamState_ = streamState_in;
}
