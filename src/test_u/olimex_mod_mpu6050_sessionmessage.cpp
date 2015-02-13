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

#include "Olimex_Mod_MPU6050_SessionMessage.h"

#include "ace/Malloc_Base.h"

#include "olimex_mod_mpu6050_macros.h"

Olimex_Mod_MPU6050_SessionMessage::Olimex_Mod_MPU6050_SessionMessage (unsigned int sessionID_in,
                                                                      Stream_SessionMessageType& messageType_in,
                                                                      Olimex_Mod_MPU6050_SessionConfiguration*& configuration_inout)
 : inherited (sessionID_in,
              messageType_in,
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

  Olimex_Mod_MPU6050_SessionMessage* nb = NULL;

  // *NOTE*: create a new Olimex_Mod_MPU6050_SessionMessage that contains unique copies of
  // the message block fields, but a reference counted duplicate of
  // the ACE_Data_Block

  // if there is no allocator, use the standard new and delete calls.
  if (message_block_allocator_ == NULL)
  {
    // uses the copy ctor
    ACE_NEW_RETURN (nb,
                    Olimex_Mod_MPU6050_SessionMessage (*this),
                    NULL);
  } // end IF

  // *WARNING*:we tell the allocator to return a Olimex_Mod_MPU6050_SessionMessage
  // by passing a 0 as argument to malloc()...
  ACE_NEW_MALLOC_RETURN (nb,
                         static_cast<Olimex_Mod_MPU6050_SessionMessage*> (message_block_allocator_->malloc (0)),
                         Olimex_Mod_MPU6050_SessionMessage (*this),
                         NULL);

  // increment the reference counts of all the continuation messages
  if (cont_)
  {
    nb->cont_ = cont_->duplicate ();

    // If things go wrong, release all of our resources and return
    if (nb->cont_ == 0)
    {
      nb->release ();
      nb = NULL;
    } // end IF
  } // end IF

  // *NOTE*: if "this" is initialized, so is the "clone" (and vice-versa)...

  return nb;
}
