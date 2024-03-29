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

#include "ace/Malloc_Base.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
//#include <DShow.h>
#else
#include "libv4l2.h"
#include "linux/videodev2.h"
#endif

#include "stream_control_message.h"

#include "ardrone_macros.h"

template <typename DataType,
          typename SessionDataType>
Test_U_Message_T<DataType,
                 SessionDataType>::Test_U_Message_T (unsigned int size_in)
 : inherited (size_in)
{
  ARDRONE_TRACE (ACE_TEXT ("Test_U_Message_T::Test_U_Message_T"));

}

template <typename DataType,
          typename SessionDataType>
Test_U_Message_T<DataType,
                 SessionDataType>::Test_U_Message_T (const OWN_TYPE_T& message_in)
 : inherited (message_in)
{
  ARDRONE_TRACE (ACE_TEXT ("Test_U_Message_T::Test_U_Message_T"));

}

template <typename DataType,
          typename SessionDataType>
Test_U_Message_T<DataType,
                 SessionDataType>::Test_U_Message_T (Stream_SessionId_t sessionId_in,
                                                     ACE_Data_Block* dataBlock_in,
                                                     ACE_Allocator* messageAllocator_in,
                                                     bool incrementMessageCounter_in)
 : inherited (sessionId_in,
              dataBlock_in,               // use (don't own (!) memory of-) this data block
              messageAllocator_in,        // message block allocator
              incrementMessageCounter_in)
{
  ARDRONE_TRACE (ACE_TEXT ("Test_U_Message_T::Test_U_Message_T"));

}

template <typename DataType,
          typename SessionDataType>
Test_U_Message_T<DataType,
                 SessionDataType>::Test_U_Message_T (Stream_SessionId_t sessionId_in,
                                                     ACE_Allocator* messageAllocator_in)
 : inherited (sessionId_in,
              messageAllocator_in) // message block allocator
{
  ARDRONE_TRACE (ACE_TEXT ("Test_U_Message_T::Test_U_Message_T"));

}

template <typename DataType,
          typename SessionDataType>
ACE_Message_Block*
Test_U_Message_T<DataType,
                 SessionDataType>::clone (ACE_Message_Block::Message_Flags flags_in) const
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Message::clone"));

  ACE_UNUSED_ARG (flags_in);

  int result = -1;
  size_t current_size = 0;

  // sanity check(s)
  ACE_ASSERT (inherited::data_block_);

  // step1: "deep"-copy the fragment chain
  OWN_TYPE_T* result_p = NULL;

  current_size = inherited::data_block_->size ();
  // *NOTE*: ACE_Data_Block::clone() does not retain the value of 'cur_size_'
  //         --> reset it
  // *TODO*: resolve ACE bugzilla issue #4219
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
    data_block_p->release (); data_block_p = NULL;
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
                             static_cast<OWN_TYPE_T*> (inherited::message_block_allocator_->calloc (sizeof (OWN_TYPE_T),
                                                                                                    '\0')),
                             OWN_TYPE_T (inherited::sessionId_,
                                         data_block_p,
                                         inherited::message_block_allocator_,
                                         true));
  } // end IF
  else
    ACE_NEW_NORETURN (result_p,
                      OWN_TYPE_T (inherited::sessionId_,
                                  data_block_p,
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
    data_block_p->release (NULL); data_block_p = NULL;
    return NULL;
  } // end IF
  // set read-/write pointers
  result_p->rd_ptr (inherited::rd_ptr_);
  result_p->wr_ptr (inherited::wr_ptr_);

  // set message type
  result_p->set (inherited::type_);

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
      result_p->release (); result_p = NULL;
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

template <typename DataType,
          typename SessionDataType>
ACE_Message_Block*
Test_U_Message_T<DataType,
                              SessionDataType>::duplicate (void) const
{
  ARDRONE_TRACE (ACE_TEXT ("Test_U_Message_T::duplicate"));

  OWN_TYPE_T* message_p = NULL;

  // create a new Test_U_Message_T that contains unique copies of
  // the message block fields, but a (reference counted) shallow duplicate of
  // the ACE_Data_Block

  // if there is no allocator, use the standard new and delete calls.
  if (unlikely (!inherited::message_block_allocator_))
    ACE_NEW_NORETURN (message_p,
                      OWN_TYPE_T (*this));
  else // otherwise, use the existing message_block_allocator
  {
    // *NOTE*: the argument to alloc() does not really matter, as this creates
    //         a shallow copy of the existing data block
    ACE_NEW_MALLOC_NORETURN (message_p,
                             static_cast<OWN_TYPE_T*> (inherited::message_block_allocator_->calloc (inherited::capacity (),
                                                                                                    '\0')),
                             OWN_TYPE_T (*this));
  } // end ELSE
  if (unlikely (!message_p))
  {
    Stream_IAllocator* allocator_p =
      dynamic_cast<Stream_IAllocator*> (inherited::message_block_allocator_);
    ACE_ASSERT (allocator_p);
    if (allocator_p->block ())
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate Test_U_Message_T: \"%m\", aborting\n")));
    return NULL;
  } // end IF

  // increment the reference counts of any continuation messages
  if (inherited::cont_)
  {
    message_p->cont_ = inherited::cont_->duplicate ();
    if (unlikely (!message_p->cont_))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to Test_U_Message_T::duplicate(): \"%m\", aborting\n")));
      message_p->release (); message_p = NULL;
      return NULL;
    } // end IF
  } // end IF

  // *NOTE*: if "this" is initialized, so is the "clone" (and vice-versa)...

//  // *NOTE*: duplicates may reuse the device buffer memory, but only the
//  //         original message will requeue it (see release() below)
//  DataType& data_r = const_cast<DataType&> (message_p->getR ());
//  data_r.device = -1;

  return message_p;
}
