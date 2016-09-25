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

#include "ace/Guard_T.h"
#include "ace/Synch_Traits.h"

#include "olimex_mod_mpu6050_stream_common.h"

#include "olimex_mod_mpu6050_common.h"
#include "olimex_mod_mpu6050_macros.h"

Olimex_Mod_MPU6050_EventHandler::Olimex_Mod_MPU6050_EventHandler (Olimex_Mod_MPU6050_GtkCBData* GtkCBData_in,
                                                                  bool consoleMode_in)
 : consoleMode_ (consoleMode_in)
 , GtkCBData_ (GtkCBData_in)
 , sessionData_ (NULL)
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
Olimex_Mod_MPU6050_EventHandler::start (Stream_SessionId_t sessionID_in,
                                        const Olimex_Mod_MPU6050_SessionData& sessionData_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_EventHandler::start"));

  sessionData_ = &const_cast<Olimex_Mod_MPU6050_SessionData&> (sessionData_in);

  {
    ACE_Guard<ACE_SYNCH_MUTEX> aGuard (GtkCBData_->lock);

    GtkCBData_->eventQueue.push_back (OLIMEX_MOD_MPU6050_EVENT_CONNECT);
  } // end lock scope
}

void
Olimex_Mod_MPU6050_EventHandler::notify (Stream_SessionId_t sessionID_in,
                                         const Stream_SessionMessageType& sessionMessage_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_EventHandler::notify"));

}

void
Olimex_Mod_MPU6050_EventHandler::notify (Stream_SessionId_t sessionID_in,
                                         const Olimex_Mod_MPU6050_Message& message_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_EventHandler::notify"));

  // sanity check(s)
  ACE_ASSERT (sessionData_);

  if (consoleMode_)
  {
    float a_x, a_y, a_z;
    float t;
    float g_x, g_y, g_z;
    ::extract_data (message_in.rd_ptr (),
                    a_x, a_y, a_z,
                    t,
                    g_x, g_y, g_z);
    ACE_DEBUG ((LM_INFO,
                ACE_TEXT ("%6.3f,%6.3f,%6.3f,%6.2f,%8.3f,%8.3f,%8.3f\n"),
                a_x, a_y, a_z,
                t,
                g_x, g_y, g_z));
  } // end IF
  else
  {
    Olimex_Mod_MPU6050_Message* message_p =
      dynamic_cast<Olimex_Mod_MPU6050_Message*> (message_in.duplicate ());
    if (!message_p)
    {
      // no message buffer available --> discard message
      //ACE_DEBUG ((LM_DEBUG,
      //            ACE_TEXT ("Olimex_Mod_MPU6050_Message::duplicate() failed, continuing\n")));
      sessionData_->currentStatistic.droppedMessages++;

      return;
    } // end IF

    {
      ACE_Guard<ACE_SYNCH_MUTEX> aGuard (GtkCBData_->lock);

      GtkCBData_->eventQueue.push_back (OLIMEX_MOD_MPU6050_EVENT_MESSAGE);
      GtkCBData_->messageQueue.push_front (message_p);
    } // end lock scope
  } // end ELSE
}

void
Olimex_Mod_MPU6050_EventHandler::notify (Stream_SessionId_t sessionID_in,
                                         const Olimex_Mod_MPU6050_SessionMessage& message_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_EventHandler::notify"));

  {
    ACE_Guard<ACE_SYNCH_MUTEX> aGuard (GtkCBData_->lock);

    GtkCBData_->eventQueue.push_back (OLIMEX_MOD_MPU6050_EVENT_SESSION_MESSAGE);
  } // end lock scope
}

void
Olimex_Mod_MPU6050_EventHandler::end (Stream_SessionId_t sessionID_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_EventHandler::end"));

  {
    ACE_Guard<ACE_SYNCH_MUTEX> aGuard (GtkCBData_->lock);

    GtkCBData_->eventQueue.push_back (OLIMEX_MOD_MPU6050_EVENT_DISCONNECT);
  } // end lock scope
}
