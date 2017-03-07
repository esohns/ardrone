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

ARDrone_MAVLinkMessage::ARDrone_MAVLinkMessage (unsigned int messageSize_in)
 : inherited (messageSize_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_MAVLinkMessage::ARDrone_MAVLinkMessage"));

}

// *NOTE*: this is implicitly invoked by clone()
ARDrone_MAVLinkMessage::ARDrone_MAVLinkMessage (ACE_Data_Block* dataBlock_in,
                                                ACE_Allocator* messageAllocator_in,
                                                bool incrementMessageCounter_in)
 : inherited (dataBlock_in,               // 'own' this data block reference
              messageAllocator_in,        // allocator
              incrementMessageCounter_in) // increment message counter ?
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_MAVLinkMessage::ARDrone_MAVLinkMessage"));

}

// *NOTE*: this is implicitly invoked by duplicate()
ARDrone_MAVLinkMessage::ARDrone_MAVLinkMessage (const ARDrone_MAVLinkMessage& message_in)
 : inherited (message_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_MAVLinkMessage::ARDrone_MAVLinkMessage"));

}

ARDrone_MAVLinkMessage::~ARDrone_MAVLinkMessage ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_MAVLinkMessage::~ARDrone_MAVLinkMessage"));

  // *NOTE*: called just BEFORE 'this' is passed back to the allocator

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  // release media sample ?
  if (inherited::data_.sample)
  {
    inherited::data_.sample->Release ();
    inherited::data_.sample = NULL;
  } // end IF
#endif
}

std::string
ARDrone_MAVLinkMessage::CommandType2String (int command_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_MAVLinkMessage::CommandType2String"));

  std::string result = ACE_TEXT ("INVALID");

  //switch (command_in)
  //{
  //  case ARDrone_MAVLinkMessage_SENSOR_DATA:
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
ARDrone_MAVLinkMessage::duplicate (void) const
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_MAVLinkMessage::duplicate"));

  ARDrone_MAVLinkMessage* message_p = NULL;

  // allocate a new ARDrone_MAVLinkMessage that contains unique copies of the message
  // block fields, but "shallow" (i.e. reference-counted) duplicate(s) of the
  // data block(s)

  // *NOTE*: if there is no allocator, use the standard new/delete calls

  if (inherited::message_block_allocator_)
  {
    // *NOTE*: the argument to calloc() doesn't matter (as long as it is not 0),
    //         the returned memory is always sizeof(ARDrone_MAVLinkMessage)
    ACE_NEW_MALLOC_NORETURN (message_p,
                             static_cast<ARDrone_MAVLinkMessage*> (inherited::message_block_allocator_->calloc (sizeof (ARDrone_MAVLinkMessage),
                                                                                                         '\0')),
                             ARDrone_MAVLinkMessage (*this));
  } // end IF
  else
    ACE_NEW_NORETURN (message_p,
                      ARDrone_MAVLinkMessage (*this));
  if (!message_p)
  {
    Stream_IAllocator* allocator_p =
        dynamic_cast<Stream_IAllocator*> (inherited::message_block_allocator_);
    ACE_ASSERT (allocator_p);
    if (allocator_p->block ())
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate ARDrone_MAVLinkMessage: \"%m\", aborting\n")));
    return NULL;
  } // end IF

  // increment the reference counts of any continuations
  if (inherited::cont_)
  {
    try {
      message_p->cont_ = inherited::cont_->duplicate ();
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in ACE_Message_Block::duplicate(): \"%m\", continuing\n")));
    }
    if (!message_p->cont_)
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to ACE_Message_Block::duplicate(): \"%m\", aborting\n")));

      // clean up
      message_p->release ();

      return NULL;
    } // end IF
  } // end IF

  // *NOTE*: if 'this' is initialized, so is the "clone"

  return message_p;
}

void
ARDrone_MAVLinkMessage::dump_state (void) const
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_MAVLinkMessage::dump_state"));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("[%u]: %u byte(s): seq: %u, id (msg/comp/sys): %u/%u/%u\n"),
              inherited::id_,
              inherited::data_.MAVLinkMessage.len,
              inherited::data_.MAVLinkMessage.seq,
              inherited::data_.MAVLinkMessage.msgid,
              inherited::data_.MAVLinkMessage.compid,
              inherited::data_.MAVLinkMessage.sysid));
}

//////////////////////////////////////////

ARDrone_NavDataMessage::ARDrone_NavDataMessage (unsigned int messageSize_in)
 : inherited (messageSize_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_NavDataMessage::ARDrone_NavDataMessage"));

}

// *NOTE*: this is implicitly invoked by clone()
ARDrone_NavDataMessage::ARDrone_NavDataMessage (ACE_Data_Block* dataBlock_in,
                                                ACE_Allocator* messageAllocator_in,
                                                bool incrementMessageCounter_in)
 : inherited (dataBlock_in,               // 'own' this data block reference
              messageAllocator_in,        // allocator
              incrementMessageCounter_in) // increment message counter ?
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_NavDataMessage::ARDrone_NavDataMessage"));

}

// *NOTE*: this is implicitly invoked by duplicate()
ARDrone_NavDataMessage::ARDrone_NavDataMessage (const ARDrone_NavDataMessage& message_in)
 : inherited (message_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_NavDataMessage::ARDrone_NavDataMessage"));

}

ARDrone_NavDataMessage::~ARDrone_NavDataMessage ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_NavDataMessage::~ARDrone_NavDataMessage"));

  // *NOTE*: called just BEFORE 'this' is passed back to the allocator

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  // release media sample ?
  if (inherited::data_.sample)
  {
    inherited::data_.sample->Release ();
    inherited::data_.sample = NULL;
  } // end IF
#endif
}

std::string
ARDrone_NavDataMessage::CommandType2String (int command_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_NavDataMessage::CommandType2String"));

  std::string result = ACE_TEXT ("INVALID");

  //switch (command_in)
  //{
  //  case ARDrone_NavDataMessage_SENSOR_DATA:
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
ARDrone_NavDataMessage::duplicate (void) const
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_NavDataMessage::duplicate"));

  ARDrone_NavDataMessage* message_p = NULL;

  // allocate a new ARDrone_NavDataMessage that contains unique copies of the message
  // block fields, but "shallow" (i.e. reference-counted) duplicate(s) of the
  // data block(s)

  // *NOTE*: if there is no allocator, use the standard new/delete calls

  if (inherited::message_block_allocator_)
  {
    // *NOTE*: the argument to calloc() doesn't matter (as long as it is not 0),
    //         the returned memory is always sizeof(ARDrone_NavDataMessage)
    ACE_NEW_MALLOC_NORETURN (message_p,
                             static_cast<ARDrone_NavDataMessage*> (inherited::message_block_allocator_->calloc (sizeof (ARDrone_NavDataMessage),
                                                                                                         '\0')),
                             ARDrone_NavDataMessage (*this));
  } // end IF
  else
    ACE_NEW_NORETURN (message_p,
                      ARDrone_NavDataMessage (*this));
  if (!message_p)
  {
    Stream_IAllocator* allocator_p =
        dynamic_cast<Stream_IAllocator*> (inherited::message_block_allocator_);
    ACE_ASSERT (allocator_p);
    if (allocator_p->block ())
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate ARDrone_NavDataMessage: \"%m\", aborting\n")));
    return NULL;
  } // end IF

  // increment the reference counts of any continuations
  if (inherited::cont_)
  {
    try {
      message_p->cont_ = inherited::cont_->duplicate ();
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in ACE_Message_Block::duplicate(): \"%m\", continuing\n")));
    }
    if (!message_p->cont_)
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to ACE_Message_Block::duplicate(): \"%m\", aborting\n")));

      // clean up
      message_p->release ();

      return NULL;
    } // end IF
  } // end IF

  // *NOTE*: if 'this' is initialized, so is the "clone"

  return message_p;
}

void
ARDrone_NavDataMessage::dump_state (void) const
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_NavDataMessage::dump_state"));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("[%u]: header: 0x%x, state: 0x%x, seq: %u, vision: %u, options [%u]:\n"),
              inherited::id_,
              inherited::data_.NavDataMessage.header,
              inherited::data_.NavDataMessage.ardrone_state,
              inherited::data_.NavDataMessage.sequence,
              inherited::data_.NavDataMessage.vision_defined,
              1));
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("\tid: %u, size: %u\n"),
              inherited::data_.NavDataMessage.options[0].tag,
              inherited::data_.NavDataMessage.options[0].size));
}

//////////////////////////////////////////

ARDrone_LiveVideoMessage::ARDrone_LiveVideoMessage (unsigned int messageSize_in)
 : inherited (messageSize_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_LiveVideoMessage::ARDrone_LiveVideoMessage"));

}

// *NOTE*: this is implicitly invoked by clone()
ARDrone_LiveVideoMessage::ARDrone_LiveVideoMessage (ACE_Data_Block* dataBlock_in,
                                                    ACE_Allocator* messageAllocator_in,
                                                    bool incrementMessageCounter_in)
 : inherited (dataBlock_in,               // 'own' this data block reference
              messageAllocator_in,        // allocator
              incrementMessageCounter_in) // increment message counter ?
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_LiveVideoMessage::ARDrone_LiveVideoMessage"));

}

// *NOTE*: this is implicitly invoked by duplicate()
ARDrone_LiveVideoMessage::ARDrone_LiveVideoMessage (const ARDrone_LiveVideoMessage& message_in)
 : inherited (message_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_LiveVideoMessage::ARDrone_LiveVideoMessage"));

}

ARDrone_LiveVideoMessage::~ARDrone_LiveVideoMessage ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_LiveVideoMessage::~ARDrone_LiveVideoMessage"));

  // *NOTE*: called just BEFORE 'this' is passed back to the allocator

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  // release media sample ?
  if (inherited::data_.sample)
  {
    inherited::data_.sample->Release ();
    inherited::data_.sample = NULL;
  } // end IF
#endif
}

std::string
ARDrone_LiveVideoMessage::CommandType2String (int command_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_LiveVideoMessage::CommandType2String"));

  std::string result = ACE_TEXT ("INVALID");

  //switch (command_in)
  //{
  //  case ARDrone_LiveVideoMessage_SENSOR_DATA:
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
ARDrone_LiveVideoMessage::clone (ACE_Message_Block::Message_Flags flags_in) const
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_LiveVideoMessage::clone"));

  int result = -1;
  size_t current_size = 0;

  // sanity check(s)
  ACE_ASSERT (inherited::data_block_);

  // step1: "deep"-copy the fragment chain
  ARDrone_LiveVideoMessage* result_p = NULL;

  current_size = inherited::data_block_->size ();
  // *NOTE*: ACE_Data_Block::clone() does not retain the value of 'cur_size_'
  //         --> reset it
  // *TODO*: submit a patch for bug #4219 to ACE
  ACE_Data_Block* data_block_p = inherited::data_block_->clone (0);
  if (!data_block_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Data_Block::clone(0): \"%m\", aborting\n")));
    return NULL;
  } // end IF
  result = data_block_p->size (current_size);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Data_Block::size(%u): \"%m\", aborting\n"),
                current_size));

    // clean up
    data_block_p->release ();

    return NULL;
  } // end IF

  // allocate a new ARDrone_LiveVideoMessage that contains unique copies of the message
  // block fields, and "deep" copie(s) of the data block(s)

  // *NOTE*: if there is no allocator, use the standard new/delete calls

  if (inherited::message_block_allocator_)
  {
    // *NOTE*: the argument to calloc() doesn't matter (as long as it is not 0),
    //         the returned memory is always sizeof(ARDrone_LiveVideoMessage)
    ACE_NEW_MALLOC_NORETURN (result_p,
                             static_cast<ARDrone_LiveVideoMessage*> (inherited::message_block_allocator_->calloc (sizeof (ARDrone_LiveVideoMessage),
                                                                                                         '\0')),
                             ARDrone_LiveVideoMessage (data_block_p,
                                              inherited::message_block_allocator_,
                                              true));
  } // end IF
  else
    ACE_NEW_NORETURN (result_p,
                      ARDrone_LiveVideoMessage (data_block_p,
                                       NULL,
                                       true));
  if (!result_p)
  {
    Stream_IAllocator* allocator_p =
        dynamic_cast<Stream_IAllocator*> (inherited::message_block_allocator_);
    ACE_ASSERT (allocator_p);
    if (allocator_p->block ())
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate ARDrone_LiveVideoMessage: \"%m\", aborting\n")));

    // clean up
    data_block_p->release (NULL);

    return NULL;
  } // end IF
  // set read-/write pointers
  result_p->rd_ptr (inherited::rd_ptr_);
  result_p->wr_ptr (inherited::wr_ptr_);

  // clone any continuations
  if (inherited::cont_)
  {
    try {
      result_p->cont_ = inherited::cont_->clone ();
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in ACE_Message_Block::clone(): \"%m\", continuing\n")));
    }
    if (!result_p->cont_)
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to ACE_Message_Block::clone(): \"%m\", aborting\n")));

      // clean up
      result_p->release ();

      return NULL;
    } // end IF
  } // end IF

  // *NOTE*: if 'this' is initialized, so is the "clone"

//  // *NOTE*: the new fragment chain is already 'crunch'ed, i.e. aligned to base_
//  // *TODO*: consider defragment()ing the chain before padding
//
//  // step2: 'pad' the fragment(s)
//  unsigned int padding_bytes =
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//    AV_INPUT_BUFFER_PADDING_SIZE;
//#else
//    FF_INPUT_BUFFER_PADDING_SIZE;
//#endif
//  for (ACE_Message_Block* message_block_p = result_p;
//       message_block_p;
//       message_block_p = message_block_p->cont ())
//  { ACE_ASSERT ((message_block_p->capacity () - message_block_p->size ()) >= padding_bytes);
//    ACE_OS::memset (message_block_p->wr_ptr (), 0, padding_bytes);
//  } // end FOR

  return result_p;
}

ACE_Message_Block*
ARDrone_LiveVideoMessage::duplicate (void) const
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_LiveVideoMessage::duplicate"));

  ARDrone_LiveVideoMessage* message_p = NULL;

  // allocate a new ARDrone_LiveVideoMessage that contains unique copies of the message
  // block fields, but "shallow" (i.e. reference-counted) duplicate(s) of the
  // data block(s)

  // *NOTE*: if there is no allocator, use the standard new/delete calls

  if (inherited::message_block_allocator_)
  {
    // *NOTE*: the argument to calloc() doesn't matter (as long as it is not 0),
    //         the returned memory is always sizeof(ARDrone_LiveVideoMessage)
    ACE_NEW_MALLOC_NORETURN (message_p,
                             static_cast<ARDrone_LiveVideoMessage*> (inherited::message_block_allocator_->calloc (sizeof (ARDrone_LiveVideoMessage),
                                                                                                         '\0')),
                             ARDrone_LiveVideoMessage (*this));
  } // end IF
  else
    ACE_NEW_NORETURN (message_p,
                      ARDrone_LiveVideoMessage (*this));
  if (!message_p)
  {
    Stream_IAllocator* allocator_p =
        dynamic_cast<Stream_IAllocator*> (inherited::message_block_allocator_);
    ACE_ASSERT (allocator_p);
    if (allocator_p->block ())
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate ARDrone_LiveVideoMessage: \"%m\", aborting\n")));
    return NULL;
  } // end IF

  // increment the reference counts of any continuations
  if (inherited::cont_)
  {
    try {
      message_p->cont_ = inherited::cont_->duplicate ();
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in ACE_Message_Block::duplicate(): \"%m\", continuing\n")));
    }
    if (!message_p->cont_)
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to ACE_Message_Block::duplicate(): \"%m\", aborting\n")));

      // clean up
      message_p->release ();

      return NULL;
    } // end IF
  } // end IF

  // *NOTE*: if 'this' is initialized, so is the "clone"

  return message_p;
}

void
ARDrone_LiveVideoMessage::dump_state (void) const
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_LiveVideoMessage::dump_state"));

}
