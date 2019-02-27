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

#include "ace/Synch.h"
#include "ardrone_sessionmessage.h"

#include "ace/Malloc_Base.h"

#include "ardrone_macros.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
ARDrone_DirectShow_SessionMessage::ARDrone_DirectShow_SessionMessage (Stream_SessionId_t sessionId_in,
                                                                      enum Stream_SessionMessageType messageType_in,
                                                                      ARDrone_DirectShow_SessionData_t*& sessionData_inout,
                                                                      struct ARDrone_UserData* userData_in)
 : inherited (sessionId_in, 
              messageType_in,
              sessionData_inout,
              userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_DirectShow_SessionMessage::ARDrone_DirectShow_SessionMessage"));

}

ARDrone_DirectShow_SessionMessage::ARDrone_DirectShow_SessionMessage (const ARDrone_DirectShow_SessionMessage& message_in)
 : inherited (message_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_DirectShow_SessionMessage::ARDrone_DirectShow_SessionMessage"));

}

ARDrone_DirectShow_SessionMessage::ARDrone_DirectShow_SessionMessage (Stream_SessionId_t sessionId_in,
                                                                      ACE_Allocator* messageAllocator_in)
 : inherited (sessionId_in,
              messageAllocator_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_DirectShow_SessionMessage::ARDrone_DirectShow_SessionMessage"));

}

ARDrone_DirectShow_SessionMessage::ARDrone_DirectShow_SessionMessage (Stream_SessionId_t sessionId_in,
                                                                      ACE_Data_Block* dataBlock_in,
                                                                      ACE_Allocator* messageAllocator_in)
 : inherited (sessionId_in,
              dataBlock_in,
              messageAllocator_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_DirectShow_SessionMessage::ARDrone_DirectShow_SessionMessage"));

}

ACE_Message_Block*
ARDrone_DirectShow_SessionMessage::duplicate (void) const
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_DirectShow_SessionMessage::duplicate"));

  ARDrone_DirectShow_SessionMessage* message_p = NULL;

  // *NOTE*: create a new ARDrone_SessionMessage that contains
  //         unique copies of the message block fields, but a reference
  //         counted duplicate of the ACE_Data_Block

  // if there is no allocator, use the standard new and delete calls.
  if (inherited::message_block_allocator_ == NULL)
  {
    // uses the copy ctor
    ACE_NEW_NORETURN (message_p,
                      ARDrone_DirectShow_SessionMessage (*this));
  } // end IF
  else
  {
    // *NOTE*: instruct the allocator to return a session message by passing 0 as
    //         argument to malloc()...
    ACE_NEW_MALLOC_NORETURN (message_p,
                             static_cast<ARDrone_DirectShow_SessionMessage*> (inherited::message_block_allocator_->malloc (0)),
                             ARDrone_DirectShow_SessionMessage (*this));
  } // end IF
  if (!message_p)
  {
    Stream_IAllocator* allocator_p =
      dynamic_cast<Stream_IAllocator*> (inherited::message_block_allocator_);
    ACE_ASSERT (allocator_p);
    if (allocator_p->block ())
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate ARDrone_DirectShow_SessionMessage: \"%m\", aborting\n")));
    return NULL;
  } // end IF

  // *NOTE*: if "this" is initialized, so is the "clone" (and vice-versa)...

  return message_p;
}
#else
ARDrone_SessionMessage::ARDrone_SessionMessage (Stream_SessionId_t sessionId_in,
                                                enum Stream_SessionMessageType messageType_in,
                                                ARDrone_SessionData_t*& sessionData_inout,
                                                struct ARDrone_UserData* userData_in)
 : inherited (sessionId_in, 
              messageType_in,
              sessionData_inout,
              userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_SessionMessage::ARDrone_SessionMessage"));

}

ARDrone_SessionMessage::ARDrone_SessionMessage (const ARDrone_SessionMessage& message_in)
 : inherited (message_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_SessionMessage::ARDrone_SessionMessage"));

}

ARDrone_SessionMessage::ARDrone_SessionMessage (Stream_SessionId_t sessionId_in,
                                                ACE_Allocator* messageAllocator_in)
 : inherited (sessionId_in,
              messageAllocator_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_SessionMessage::ARDrone_SessionMessage"));

}

ARDrone_SessionMessage::ARDrone_SessionMessage (Stream_SessionId_t sessionId_in,
                                                ACE_Data_Block* dataBlock_in,
                                                ACE_Allocator* messageAllocator_in)
 : inherited (sessionId_in,
              dataBlock_in,
              messageAllocator_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_SessionMessage::ARDrone_SessionMessage"));

}

ACE_Message_Block*
ARDrone_SessionMessage::duplicate (void) const
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_SessionMessage::duplicate"));

  ARDrone_SessionMessage* message_p = NULL;

  // *NOTE*: create a new ARDrone_SessionMessage that contains
  //         unique copies of the message block fields, but a reference
  //         counted duplicate of the ACE_Data_Block

  // if there is no allocator, use the standard new and delete calls.
  if (inherited::message_block_allocator_ == NULL)
  {
    // uses the copy ctor
    ACE_NEW_NORETURN (message_p,
                      ARDrone_SessionMessage (*this));
  } // end IF
  else
  {
    // *NOTE*: instruct the allocator to return a session message by passing 0 as
    //         argument to malloc()...
    ACE_NEW_MALLOC_NORETURN (message_p,
                             static_cast<ARDrone_SessionMessage*> (inherited::message_block_allocator_->malloc (0)),
                             ARDrone_SessionMessage (*this));
  } // end IF
  if (!message_p)
  {
    Stream_IAllocator* allocator_p =
      dynamic_cast<Stream_IAllocator*> (inherited::message_block_allocator_);
    ACE_ASSERT (allocator_p);
    if (allocator_p->block ())
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate ARDrone_SessionMessage: \"%m\", aborting\n")));
    return NULL;
  } // end IF

  // *NOTE*: if "this" is initialized, so is the "clone" (and vice-versa)...

  return message_p;
}
#endif // ACE_WIN32 || ACE_WIN64
