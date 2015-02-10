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

#include "olimex_mod_mpu6050_message.h"

#include "olimex_mod_mpu6050_macros.h"

// *NOTE*: this is implicitly invoked by duplicate()...
Olimex_Mod_MPU6050_Message::Olimex_Mod_MPU6050_Message (const Olimex_Mod_MPU6050_Message& message_in)
 : inherited (message_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_Message::Olimex_Mod_MPU6050_Message"));

}

Olimex_Mod_MPU6050_Message::Olimex_Mod_MPU6050_Message (ACE_Data_Block* dataBlock_in,
                                                        ACE_Allocator* messageAllocator_in)
 : inherited (dataBlock_in,        // use (don't own !) this data block
              messageAllocator_in, // use this when destruction is imminent...
              true)                // increment message counter
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_Message::Olimex_Mod_MPU6050_Message"));

}

Olimex_Mod_MPU6050_Message::~Olimex_Mod_MPU6050_Message ()
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_Message::~Olimex_Mod_MPU6050_Message"));

  // *NOTE*: called just BEFORE 'this' is passed back to the allocator
}

ACE_Message_Block*
Olimex_Mod_MPU6050_Message::duplicate (void) const
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_Message::duplicate"));

  Olimex_Mod_MPU6050_Message* new_message = NULL;

  // create a new Olimex_Mod_MPU6050_Message that contains unique copies of
  // the message block fields, but a (reference counted) "shallow" duplicate of
  // the same datablock

  // if there is no allocator, use the standard new and delete calls.
  if (!inherited::message_block_allocator_)
    ACE_NEW_RETURN (new_message,
                    Olimex_Mod_MPU6050_Message (*this),
                    NULL);
  else // otherwise, use the existing message_block_allocator
  {
    // *NOTE*: the argument to malloc SHOULDN'T really matter, as this will be
    // a "shallow" copy referencing the same datablock...
    // *TODO*: (depending on the allocator) a datablock is allocated anyway,
    // only to be immediately released again...
    ACE_NEW_MALLOC_RETURN (new_message,
                           static_cast<Olimex_Mod_MPU6050_Message*> (inherited::message_block_allocator_->malloc (inherited::capacity ())),
                           Olimex_Mod_MPU6050_Message (*this),
                           NULL);
  } // end ELSE

  // increment the reference counts of any continuation messages
  if (inherited::cont_)
  {
    new_message->cont_ = cont_->duplicate ();

    // when things go wrong, release all resources and return
    if (!new_message->cont_)
    {
      new_message->release ();
      new_message = NULL;
    } // end IF
  } // end IF

  // *NOTE*: if "this" is initialized, so is the "clone" (and vice-versa)...

  return new_message;
}
