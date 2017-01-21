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

#include "ardrone_message.h"

#include "ace/Malloc_Base.h"

#include "stream_iallocator.h"

#include "ardrone_macros.h"
#include "ardrone_types.h"

// *NOTE*: this is implicitly invoked by duplicate()...
ARDrone_Message::ARDrone_Message (const ARDrone_Message& message_in)
 : inherited (message_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Message::ARDrone_Message"));

}

ARDrone_Message::ARDrone_Message (ACE_Data_Block* dataBlock_in,
                                  ACE_Allocator* messageAllocator_in,
                                  bool incrementMessageCounter_in)
 : inherited (dataBlock_in,               // use (don't own !) this data block
              messageAllocator_in,        // allocator
              incrementMessageCounter_in) // increment message counter ?
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Message::ARDrone_Message"));

}

ARDrone_Message::~ARDrone_Message ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Message::~ARDrone_Message"));

  // *NOTE*: called just BEFORE 'this' is passed back to the allocator
}

int
ARDrone_Message::command () const
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Message::command"));

  return 0;
}

std::string
ARDrone_Message::CommandType2String (int command_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Message::CommandType2String"));

  std::string result = ACE_TEXT ("INVALID");

  //switch (command_in)
  //{
  //  case ARDRONE_MESSAGE_SENSOR_DATA:
  //    result = ACE_TEXT ("SENSOR_DATA"); break;
  //  default:
  //  {
  //    ACE_DEBUG ((LM_ERROR,
  //                ACE_TEXT ("invalid message type (was %d), aborting\n"),
  //                messageType_in));
  //    break;
  //  }
  //} // end SWITCH

  return result;
}

ACE_Message_Block*
ARDrone_Message::duplicate (void) const
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Message::duplicate"));

  ARDrone_Message* message_p = NULL;

  // create a new ARDrone_Message that contains unique copies of
  // the message block fields, but a (reference counted) "shallow" duplicate of
  // the same datablock

  // if there is no allocator, use the standard new and delete calls.
  if (!inherited::message_block_allocator_)
    ACE_NEW_NORETURN (message_p,
                      ARDrone_Message (*this));
  else // otherwise, use the existing message_block_allocator
  {
    // *NOTE*: the argument to malloc SHOULDN'T really matter, as this will be
    // a "shallow" copy referencing the same datablock...
    ACE_NEW_MALLOC_NORETURN (message_p,
                             static_cast<ARDrone_Message*> (inherited::message_block_allocator_->calloc (inherited::capacity (),
                                                                                                         '\0')),
                             ARDrone_Message (*this));
  } // end ELSE
  if (!message_p)
  {
    Stream_IAllocator* allocator_p =
     dynamic_cast<Stream_IAllocator*> (inherited::message_block_allocator_);
    ACE_ASSERT (allocator_p);
    if (allocator_p->block ())
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate ARDrone_Message: \"%m\", aborting\n")));
    return NULL;
  } // end IF

  // increment the reference counts of any continuation messages
  if (inherited::cont_)
  {
    message_p->cont_ = inherited::cont_->duplicate ();
    if (!message_p->cont_)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ARDrone_Message::duplicate(): \"%m\", aborting\n")));

      // clean up
      message_p->release ();

      return NULL;
    } // end IF
  } // end IF

  // *NOTE*: if "this" is initialized, so is the "clone" (and vice-versa)...

  return message_p;
}
