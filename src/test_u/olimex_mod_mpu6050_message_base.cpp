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

#include "olimex_mod_mpu6050_message_base.h"

#include "ace/Log_Msg.h"

#include "olimex_mod_mpu6050_macros.h"

// init static(s)
static Olimex_Mod_MPU6050_MessageBase::currentID_ = ACE_Atomic_Op<ACE_Thread_Mutex, unsigned int>(0);

// *NOTE*: this is implicitly invoked by duplicate() as well...
Olimex_Mod_MPU6050_MessageBase::Olimex_Mod_MPU6050_MessageBase (const Olimex_Mod_MPU6050_MessageBase& message_in)
 : inherited (message_in)
 , initialized_ (message_in.initialized_)
 , messageID_ (message_in.messageID_)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_MessageBase::Olimex_Mod_MPU6050_MessageBase"));

}

Olimex_Mod_MPU6050_MessageBase::Olimex_Mod_MPU6050_MessageBase (ACE_Data_Block* dataBlock_in,
                                                                ACE_Allocator* messageAllocator_in,
                                                                bool incrementCounter_in)
 : inherited (dataBlock_in,        // use (don't own !) this data block
              messageAllocator_in) // use this when destruction is imminent...
 , initialized_ (true)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_MessageBase::Olimex_Mod_MPU6050_MessageBase"));

  messageID_ = (incrementCounter_in ? ++Olimex_Mod_MPU6050_MessageBase::currentID_
                                    : Olimex_Mod_MPU6050_MessageBase::currentID_);
}

Olimex_Mod_MPU6050_MessageBase::Olimex_Mod_MPU6050_MessageBase (ACE_Allocator* messageAllocator_in)
 : inherited (messageAllocator_in) // use this when destruction is imminent...
 , initialized_ (false)
 , messageID_ (0)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_MessageBase::Olimex_Mod_MPU6050_MessageBase"));

}

Olimex_Mod_MPU6050_MessageBase::~Olimex_Mod_MPU6050_MessageBase ()
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_MessageBase::~Olimex_Mod_MPU6050_MessageBase"));

  // *NOTE*: will be called just BEFORE this is passed back to the allocator

  initialized_ = false;
  messageID_ = 0;
}

void
Olimex_Mod_MPU6050_MessageBase::init (ACE_Data_Block* dataBlock_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_MessageBase::init"));

  // sanity check(s)
  ACE_ASSERT (!initialized_);

  inherited::init (dataBlock_in);
  messageID_ = ++Olimex_Mod_MPU6050_MessageBase::currentID_;
  initialized_ = true;
}

unsigned int
Olimex_Mod_MPU6050_MessageBase::getID ()
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_MessageBase::getID"));

  return messageID_;
}

static
void
Olimex_Mod_MPU6050_MessageBase::resetMessageIDGenerator ()
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_MessageBase::resetMessageIDGenerator"));

  currentID_ = 0;
}

static
void
Olimex_Mod_MPU6050_MessageBase::messageType2String (const ACE_Message_Type& type_in,
                                                    std::string& string_out)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_MessageBase::messageType2String"));

  // init return values
  string_out.clear ();

  switch (type_in)
  {
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unknown message type (was: %d), continuing\n"),
                  type_in));

      break;
    }
  }
}

ACE_Message_Block*
Olimex_Mod_MPU6050_MessageBase::duplicate (void) const
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_MessageBase::duplicate"));

  Olimex_Mod_MPU6050_MessageBase* new_message = NULL;

  // create a new Olimex_Mod_MPU6050_MessageBase that contains unique copies of
  // the message block fields, but a (reference counted) "shallow" duplicate of
  // the same datablock

  // if there is no allocator, use the standard new and delete calls.
  if (!inherited::message_block_allocator_)
    ACE_NEW_RETURN (new_message,
                    Olimex_Mod_MPU6050_MessageBase (*this),
                    NULL);
  else // otherwise, use the existing message_block_allocator
  {
    // *NOTE*: the argument to malloc SHOULDN'T really matter, as this will be
    // a "shallow" copy referencing the same datablock...
    // *TODO*: (depending on the allocator) a datablock is allocated anyway,
    // only to be immediately released again...
    ACE_NEW_MALLOC_RETURN (new_message,
                           static_cast<Olimex_Mod_MPU6050_MessageBase*> (inherited::message_block_allocator_->malloc (inherited::capacity ())),
                           Olimex_Mod_MPU6050_MessageBase (*this),
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
