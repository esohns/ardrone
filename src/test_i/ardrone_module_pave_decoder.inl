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

#include <ace/Log_Msg.h>

#include "stream_macros.h"

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataContainerType>
ARDrone_Module_PaVEDecoder_T<ACE_SYNCH_USE,
                             TimePolicyType,
                             ConfigurationType,
                             ControlMessageType,
                             DataMessageType,
                             SessionMessageType,
                             SessionDataContainerType>::ARDrone_Module_PaVEDecoder_T ()
 : inherited ()
 , allocator_ (NULL)
 , buffer_ (NULL)
 , header_ ()
 , headerDecoded_ (false)
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_Module_PaVEDecoder_T::ARDrone_Module_PaVEDecoder_T"));

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_OS::memset (&header_,
                  0,
                  sizeof (struct ARDrone_ParrotVideoEncapsulation_Header));
#else
  ACE_OS::memset (&header_, 0, sizeof (parrot_video_encapsulation_t));
#endif
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataContainerType>
ARDrone_Module_PaVEDecoder_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ConfigurationType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            SessionDataContainerType>::~ARDrone_Module_PaVEDecoder_T ()
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_Module_PaVEDecoder_T::~ARDrone_Module_PaVEDecoder_T"));

  // clean up any unprocessed (chained) buffer(s)
  if (buffer_)
    buffer_->release ();
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataContainerType>
bool
ARDrone_Module_PaVEDecoder_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ConfigurationType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            SessionDataContainerType>::initialize (const ConfigurationType& configuration_in,
                                                                   Stream_IAllocator* allocator_in)
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_Module_PaVEDecoder_T::initialize"));

  // sanity check(s)
  // *TODO*: remove type inferences
  ACE_ASSERT (configuration_in.parserConfiguration);
  ACE_ASSERT (configuration_in.streamConfiguration);

  if (inherited::isInitialized_)
  {
    allocator_ = NULL;
    if (buffer_)
      buffer_->release ();
    buffer_ = NULL;
  } // end IF

  // *TODO*: remove type inferences
  allocator_ = allocator_in;

  return inherited::initialize (configuration_in,
                                allocator_in);
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataContainerType>
void
ARDrone_Module_PaVEDecoder_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ConfigurationType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            SessionDataContainerType>::handleDataMessage (DataMessageType*& message_inout,
                                                                          bool& passMessageDownstream_out)
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_Module_PaVEDecoder_T::handleDataMessage"));

  int result = -1;
  ACE_Message_Block* message_block_p, *message_block_2 = NULL;
  unsigned int skipped_bytes, trailing_bytes = 0;
  unsigned int length = 0;

  // initialize return value(s)
  // *NOTE*: the default behavior is to pass all messages along
  //         --> in this case, the individual frames are extracted and passed
  //             as such
  passMessageDownstream_out = false;

  // sanity check(s)
  //ACE_ASSERT (inherited::isInitialized_);

  // form a chain of buffers
  if (buffer_)
  {
    message_block_p = buffer_->cont ();
    if (message_block_p)
    {
      while (message_block_p->cont ()) // skip to end
        message_block_p = message_block_p->cont ();
    } // end IF
    else
      message_block_p = buffer_;
    message_block_p->cont (message_inout); // chain the buffer
  } // end IF
  else
    buffer_ = message_inout;

  unsigned int buffered_bytes, missing_bytes, bytes_to_copy;
next:
  buffered_bytes = buffer_->total_length ();
  message_block_p = buffer_;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  missing_bytes =
    sizeof (struct ARDrone_ParrotVideoEncapsulation_Header);
#else
  missing_bytes = sizeof (parrot_video_encapsulation_t);
#endif
  if (!headerDecoded_)
  {
    // PaVE header has not been received yet

    // --> wait for more data ?
    if (buffered_bytes < missing_bytes)
      return; // done

    // received a PaVE header --> decode
    char* buffer_p = reinterpret_cast<char*> (&header_);
    skipped_bytes = 0;
    for (;
         message_block_p;
         message_block_p = message_block_p->cont ())
    {
      bytes_to_copy =
        ((missing_bytes - skipped_bytes) < message_block_p->length () ? (missing_bytes - skipped_bytes)
                                                                      : message_block_p->length ());
      ACE_OS::memcpy (buffer_p, message_block_p->rd_ptr (), bytes_to_copy);
      message_block_p->rd_ptr (bytes_to_copy);
      skipped_bytes += bytes_to_copy;
      if (skipped_bytes == missing_bytes)
        break;
      buffer_p += bytes_to_copy;
    } // end FOR
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
    ACE_ASSERT (PAVE_CHECK (header_.signature));
#endif

    buffered_bytes -= missing_bytes;

    headerDecoded_ = true;
  } // end IF

  // --> wait for more data ?
  missing_bytes = header_.header_size - missing_bytes;
  ACE_ASSERT (missing_bytes >= 0);
  if (buffered_bytes < (missing_bytes + header_.payload_size))
    return; // done

  // skip over (undocumented) header bytes
  skipped_bytes = 0;
  for (;
       message_block_p;
       message_block_p = message_block_p->cont ())
  {
    bytes_to_copy =
      (missing_bytes < message_block_p->length () ? missing_bytes
                                                  : message_block_p->length ());
    message_block_p->rd_ptr (bytes_to_copy);
    skipped_bytes += bytes_to_copy;
    if (skipped_bytes == missing_bytes)
      break;
  } // end FOR
  buffered_bytes -= missing_bytes;

  // forward a H264 frame

  if (buffered_bytes > header_.payload_size)
  {
    missing_bytes = header_.payload_size;
    trailing_bytes = 0;
    for (ACE_Message_Block* message_block_3 = buffer_;
         missing_bytes;
         message_block_3 = message_block_3->cont ())
    {
      length = message_block_3->length ();
      length = (missing_bytes <= length ? missing_bytes : length);
      if (missing_bytes <= length)
      {
        message_block_p = message_block_3;
        trailing_bytes = message_block_3->length () - missing_bytes;
        break;
      } // end IF
      missing_bytes -= length;
    } // end FOR

    message_block_2 = message_block_p->duplicate ();
    if (!message_block_2)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to MessageType::duplicate(): \"%m\", returning\n"),
                  inherited::mod_->name ()));
      return;
    } // end IF
    message_block_p->cont (NULL);

    if (trailing_bytes)
    {
      message_block_p->length (length);
      message_block_2->rd_ptr (length);
    } // end IF
  } // end IF
  else
    message_block_2 = NULL;
  ACE_ASSERT (buffer_->total_length () == header_.payload_size);
  buffer_->set (ARDRONE_MESSAGE_LIVEVIDEOFRAME);
  result = inherited::put_next (buffer_, NULL);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to ACE_Task::put_next(): \"%m\", returning\n"),
                inherited::mod_->name ()));

    // clean up
    if (message_block_2)
      message_block_2->release ();
    buffer_->release ();
    buffer_ = NULL;

    return;
  } // end IF
  if (message_block_2)
  {
    buffer_ = dynamic_cast<DataMessageType*> (message_block_2);
    ACE_ASSERT (buffer_);
  } // end IF
  else
    buffer_ = NULL;

  // reset header
  headerDecoded_ = false;

  // parse next frame ?
  if (buffer_)
    goto next;
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataContainerType>
void
ARDrone_Module_PaVEDecoder_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ConfigurationType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            SessionDataContainerType>::handleSessionMessage (SessionMessageType*& message_inout,
                                                                             bool& passMessageDownstream_out)
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_Module_PaVEDecoder_T::handleSessionMessage"));

  // don't care (implies yes per default, if part of a stream)
  ACE_UNUSED_ARG (passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT (inherited::isInitialized_);

  switch (message_inout->type ())
  {
    case STREAM_SESSION_MESSAGE_BEGIN:
    {
      break;
    }
    default:
      break;
  } // end SWITCH
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataContainerType>
DataMessageType*
ARDrone_Module_PaVEDecoder_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ConfigurationType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            SessionDataContainerType>::allocateMessage (unsigned int requestedSize_in)
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_Module_PaVEDecoder_T::allocateMessage"));

  // initialize return value(s)
  DataMessageType* message_p = NULL;

  if (allocator_)
  {
allocate:
    try {
      message_p =
        static_cast<DataMessageType*> (allocator_->malloc (requestedSize_in));
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in Stream_IAllocator::malloc(%u), aborting\n"),
                  requestedSize_in));
      return NULL;
    }

    // keep retrying ?
    if (!message_p && !allocator_->block ())
      goto allocate;
  } // end IF
  else
    ACE_NEW_NORETURN (message_p,
                      DataMessageType (requestedSize_in));
  if (!message_p)
  {
    if (allocator_)
    {
      if (allocator_->block ())
        ACE_DEBUG ((LM_CRITICAL,
                    ACE_TEXT ("failed to allocate data message: \"%m\", aborting\n")));
    } // end IF
    else
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("failed to allocate data message: \"%m\", aborting\n")));
  } // end IF

  return message_p;
}
