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
              messageAllocator_in) // use this when destruction is imminent...
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_Message::Olimex_Mod_MPU6050_Message"));

}

Olimex_Mod_MPU6050_Message::~Olimex_Mod_MPU6050_Message ()
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_Message::~Olimex_Mod_MPU6050_Message"));

  // *NOTE*: called just BEFORE 'this' is passed back to the allocator
}

Olimex_Mod_MPU6050_MessageType_t
Olimex_Mod_MPU6050_Message::getCommand () const
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_Message::getCommand"));

  return OLIMEX_MOD_MPU6050_MESSAGE_SENSOR_DATA;
}

std::string
Olimex_Mod_MPU6050_Message::CommandType2String (Olimex_Mod_MPU6050_MessageType_t messageType_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_Message::CommandType2String"));

  std::string result = ACE_TEXT ("INVALID");

  switch (messageType_in)
  {
    case OLIMEX_MOD_MPU6050_MESSAGE_SENSOR_DATA:
      result = ACE_TEXT ("SENSOR_DATA"); break;
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid message type (was %d), aborting\n"),
                  messageType_in));

      break;
    }
  } // end SWITCH

  return result;
}

ACE_Message_Block*
Olimex_Mod_MPU6050_Message::duplicate (void) const
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_Message::duplicate"));

  Olimex_Mod_MPU6050_Message* message_p = NULL;

  // create a new Olimex_Mod_MPU6050_Message that contains unique copies of
  // the message block fields, but a (reference counted) "shallow" duplicate of
  // the same datablock

  // if there is no allocator, use the standard new and delete calls.
  if (!inherited::message_block_allocator_)
    ACE_NEW_RETURN (message_p,
                    Olimex_Mod_MPU6050_Message (*this),
                    NULL);
  else // otherwise, use the existing message_block_allocator
  {
    // *NOTE*: the argument to malloc SHOULDN'T really matter, as this will be
    // a "shallow" copy referencing the same datablock...
    ACE_NEW_MALLOC_RETURN (message_p,
                           static_cast<Olimex_Mod_MPU6050_Message*> (inherited::message_block_allocator_->calloc (inherited::capacity ())),
                           Olimex_Mod_MPU6050_Message (*this),
                           NULL);
  } // end ELSE
  if (!message_p)
  {
    ACE_DEBUG ((LM_CRITICAL,
                ACE_TEXT ("failed to allocate Olimex_Mod_MPU6050_Message: \"%m\", aborting\n")));

    return NULL;
  } // end IF

  // increment the reference counts of any continuation messages
  if (inherited::cont_)
  {
    message_p->cont_ = cont_->duplicate ();
    if (!message_p->cont_)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Olimex_Mod_MPU6050_Message::duplicate(): \"%m\", aborting\n")));

      // clean up
      message_p->release ();

      return NULL;
    } // end IF
  } // end IF

  // *NOTE*: if "this" is initialized, so is the "clone" (and vice-versa)...

  return message_p;
}
