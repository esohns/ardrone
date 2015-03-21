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

#include "olimex_mod_mpu6050_sessionmessage.h"

#include "ace/Malloc_Base.h"

#include "olimex_mod_mpu6050_macros.h"

Olimex_Mod_MPU6050_SessionMessage::Olimex_Mod_MPU6050_SessionMessage (Stream_SessionMessageType_t messageType_in,
                                                                      Stream_State_t* streamState_in,
                                                                      Net_StreamSessionData_t*& configuration_inout)
                                                                      //Olimex_Mod_MPU6050_StreamSessionData_t*& configuration_inout)
 : inherited (messageType_in,
              streamState_in,
              configuration_inout)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_SessionMessage::Olimex_Mod_MPU6050_SessionMessage"));

}

Olimex_Mod_MPU6050_SessionMessage::Olimex_Mod_MPU6050_SessionMessage (const Olimex_Mod_MPU6050_SessionMessage& message_in)
 : inherited (message_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_SessionMessage::Olimex_Mod_MPU6050_SessionMessage"));

}

Olimex_Mod_MPU6050_SessionMessage::Olimex_Mod_MPU6050_SessionMessage (ACE_Allocator* messageAllocator_in)
 : inherited (messageAllocator_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_SessionMessage::Olimex_Mod_MPU6050_SessionMessage"));

}

Olimex_Mod_MPU6050_SessionMessage::Olimex_Mod_MPU6050_SessionMessage (ACE_Data_Block* dataBlock_in,
                                                                      ACE_Allocator* messageAllocator_in)
 : inherited (dataBlock_in,
              messageAllocator_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_SessionMessage::Olimex_Mod_MPU6050_SessionMessage"));

}

Olimex_Mod_MPU6050_SessionMessage::~Olimex_Mod_MPU6050_SessionMessage ()
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_SessionMessage::~Olimex_Mod_MPU6050_SessionMessage"));

}

ACE_Message_Block*
Olimex_Mod_MPU6050_SessionMessage::duplicate (void) const
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_SessionMessage::duplicate"));

  Olimex_Mod_MPU6050_SessionMessage* message_p = NULL;

  // *NOTE*: create a new Olimex_Mod_MPU6050_SessionMessage that contains
  //         unique copies of the message block fields, but a reference
  //         counted duplicate of the ACE_Data_Block

  // if there is no allocator, use the standard new and delete calls.
  if (inherited::message_block_allocator_ == NULL)
  {
    // uses the copy ctor
    ACE_NEW_NORETURN (message_p,
                      Olimex_Mod_MPU6050_SessionMessage (*this));
  } // end IF
  else
  {
    // *NOTE*: instruct the allocator to return a session message by passing 0 as
    //         argument to malloc()...
    ACE_NEW_MALLOC_NORETURN (message_p,
                             static_cast<Olimex_Mod_MPU6050_SessionMessage*> (inherited::message_block_allocator_->malloc (0)),
                             Olimex_Mod_MPU6050_SessionMessage (*this));
  } // end IF
  if (!message_p)
  {
    Stream_IAllocator* allocator_p =
      dynamic_cast<Stream_IAllocator*> (inherited::message_block_allocator_);
    ACE_ASSERT (allocator_p);
    if (allocator_p->block ())
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate Olimex_Mod_MPU6050_SessionMessage: \"%m\", aborting\n")));
    return NULL;
  } // end IF

  // *NOTE*: if "this" is initialized, so is the "clone" (and vice-versa)...

  return message_p;
}
