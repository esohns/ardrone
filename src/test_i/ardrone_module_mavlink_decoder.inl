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

#include "stream_dec_defines.h"

#include "ardrone_macros.h"

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataContainerType>
ARDrone_Module_MAVLinkDecoder_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionDataContainerType>::ARDrone_Module_MAVLinkDecoder_T ()
 : inherited ()
 , buffer_ (NULL)
 , bufferState_ (NULL)
 , isFirst_ (true)
 , scannerState_ (NULL)
 , useYYScanBuffer_ (STREAM_DECODER_FLEX_DEFAULT_USE_YY_SCAN_BUFFER)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_MAVLinkDecoder_T::ARDrone_Module_MAVLinkDecoder_T"));

  // step1: initialize flex state
  int result = ARDrone_MAVLink_Scanner_lex_init_extra (this,
                                                       &scannerState_);
  if (result)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to yylex_init_extra: \"%m\", returning\n")));
    return;
  } // end IF
  ACE_ASSERT (scannerState_);
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataContainerType>
ARDrone_Module_MAVLinkDecoder_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionDataContainerType>::~ARDrone_Module_MAVLinkDecoder_T ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_MAVLinkDecoder_T::~ARDrone_Module_MAVLinkDecoder_T"));

  // clean up any unprocessed (chained) buffer(s)
  if (buffer_)
    buffer_->release ();

  if (ARDrone_MAVLink_Scanner_lex_destroy (scannerState_))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to yylex_destroy(): \"%m\", continuing\n")));
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataContainerType>
bool
ARDrone_Module_MAVLinkDecoder_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionDataContainerType>::initialize (const ConfigurationType& configuration_in,
                                                                       Stream_IAllocator* allocator_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_MAVLinkDecoder_T::initialize"));

  // sanity check(s)
  // *TODO*: remove type inferences
  ACE_ASSERT (configuration_in.parserConfiguration);
  ACE_ASSERT (configuration_in.streamConfiguration);

  if (inherited::isInitialized_)
  {
    ACE_DEBUG ((LM_WARNING,
                ACE_TEXT ("re-initializing...\n")));

    //allocator_ = NULL;
    if (buffer_)
      buffer_->release ();
    buffer_ = NULL;

    if (bufferState_)
    { ACE_ASSERT (scannerState_);
      ARDrone_MAVLink_Scanner__delete_buffer (bufferState_,
                                              scannerState_);
      bufferState_ = NULL;
    } // end IF

    isFirst_ = true;

    useYYScanBuffer_ = STREAM_DECODER_FLEX_DEFAULT_USE_YY_SCAN_BUFFER;
  } // end IF

  // trace ?
  ARDrone_MAVLink_Scanner_set_debug (COMMON_PARSER_DEFAULT_LEX_TRACE,
                                     scannerState_);

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
ARDrone_Module_MAVLinkDecoder_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionDataContainerType>::handleDataMessage (DataMessageType*& message_inout,
                                                                              bool& passMessageDownstream_out)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_MAVLinkDecoder_T::handleDataMessage"));

  int result = -1;
  ACE_Message_Block* message_block_p = NULL;
  bool do_scan_end = false;

  // initialize return value(s)
  // *NOTE*: the default behavior is to pass all messages along
  //         --> in this case, the individual frames are extracted and passed
  //             as such
  passMessageDownstream_out = false;

  // append the "\0\0"-sequence, as required by flex
  ACE_ASSERT (message_inout->capacity () - message_inout->length () >=
              STREAM_DECODER_FLEX_BUFFER_BOUNDARY_SIZE);
  *(message_inout->wr_ptr ()) = YY_END_OF_BUFFER_CHAR;
  *(message_inout->wr_ptr () + 1) = YY_END_OF_BUFFER_CHAR;
  // *NOTE*: DO NOT adjust the write pointer --> length() must stay as it was

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

scan:
  if (!scan_begin (buffer_->rd_ptr (),
                   buffer_->length ()))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ARDrone_Module_MAVLinkDecoder_T::scan_begin(), aborting\n")));
    goto error;
  } // end IF
  do_scan_end = true;

  // initialize scanner ?
  if (isFirst_)
  { /* column is only valid if an input buffer exists. */
    ARDrone_MAVLink_Scanner_set_column (1, scannerState_);
    ARDrone_MAVLink_Scanner_set_lineno (1, scannerState_);
  } // end IF

  // parse data fragment
  try {
    result = ARDrone_MAVLink_Scanner_lex (scannerState_,
                                          this);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in yylex(), continuing\n")));
    result = 1;
  }
  switch (result)
  {
    case -1:
    {
      // *NOTE*: most probable reason: connection
      //         has been closed --> session end
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("%s: failed to parse MAVLink messages (result was: %d), aborting\n"),
                  inherited::mod_->name (),
                  result));
      goto error;
    }
    default:
    {
      // clean up
      scan_end ();
      do_scan_end = false;

      // more data ?
      if (buffer_)
        goto scan;

      break;
    }
  } // end SWITCH

  return;

error:
  if (do_scan_end)
    scan_end ();
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataContainerType>
void
ARDrone_Module_MAVLinkDecoder_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionDataContainerType>::handleSessionMessage (SessionMessageType*& message_inout,
                                                                                 bool& passMessageDownstream_out)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_MAVLinkDecoder_T::handleSessionMessage"));

  // don't care (implies yes per default, if part of a stream)
  ACE_UNUSED_ARG (passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT (inherited::isInitialized_);

  switch (message_inout->type ())
  {
    case STREAM_SESSION_MESSAGE_BEGIN:
    {
      goto continue_;

//error:
      this->notify (STREAM_SESSION_MESSAGE_ABORT);

      break;

continue_:
      break;
    }
    case STREAM_SESSION_MESSAGE_END:
    {
      if (buffer_)
      {
        buffer_->release ();
        buffer_ = NULL;
      } // end IF

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
void
ARDrone_Module_MAVLinkDecoder_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionDataContainerType>::record (struct __mavlink_message*& record_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_MAVLinkDecoder_T::record"));

  // sanity check(s)
  ACE_ASSERT (buffer_);
  ACE_ASSERT (record_in);

  // frame the MAVLink message data
  int result = -1;
  unsigned int remaining_bytes = record_in->len + MAVLINK_NUM_CHECKSUM_BYTES;
  unsigned int remaining_bytes_2 = MAVLINK_NUM_HEADER_BYTES;
  unsigned int length = 0;
  unsigned int trailing_bytes_total, trailing_bytes = 0;
  ACE_Message_Block* message_block_p = buffer_;
  ACE_Message_Block* message_block_2 = NULL;

  while (remaining_bytes_2)
  { ACE_ASSERT (message_block_p);
    length = message_block_p->length ();
    length = (remaining_bytes_2 <= length ? remaining_bytes_2 : length);
    message_block_p->rd_ptr (length);
    message_block_p = message_block_p->cont ();
    remaining_bytes_2 -= length;
  } // end WHILE

  length = buffer_->total_length ();
  if (length > remaining_bytes)
  {
    trailing_bytes_total = length - remaining_bytes;

    message_block_2 = buffer_;
    for (;
         message_block_2;
         message_block_2 = message_block_2->cont ())
    {
      length = message_block_2->length ();
      length = (length >= remaining_bytes ? remaining_bytes
                                          : length);
      remaining_bytes -= length;
      if (!remaining_bytes)
      {
        trailing_bytes = message_block_2->length () - length;
        break;
      } // end IF
    } // end FOR
    ACE_ASSERT (trailing_bytes);

    message_block_p = message_block_2->duplicate ();
    if (!message_block_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to DataMessageType::duplicate(): \"%m\", returning\n"),
                  inherited::mod_->name ()));
      return;
    } // end IF
    message_block_2->length (message_block_2->length () - trailing_bytes);
    message_block_2->cont (NULL);
    ACE_ASSERT (buffer_->total_length () ==
                static_cast<unsigned int> (record_in->len +
                                           MAVLINK_NUM_CHECKSUM_BYTES));
    message_block_p->rd_ptr (message_block_p->length () - trailing_bytes);
    ACE_ASSERT (message_block_p->total_length () == trailing_bytes_total);
  } // end IF

  result = inherited::put_next (buffer_, NULL);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to ACE_Task_T::put_next(): \"%m\", returning\n"),
                inherited::mod_->name ()));
    // clean up
    buffer_->release ();
    buffer_ = NULL;

    return;
  } // end IF
  buffer_ = NULL;

  if (message_block_p)
  {
    buffer_ = dynamic_cast<DataMessageType*> (message_block_p);
    ACE_ASSERT (buffer_);
  } // end IF
}
//template <ACE_SYNCH_DECL,
//          typename TimePolicyType,
//          typename ConfigurationType,
//          typename ControlMessageType,
//          typename DataMessageType,
//          typename SessionMessageType,
//          typename SessionDataContainerType>
//bool
//ARDrone_Module_MAVLinkDecoder_T<ACE_SYNCH_USE,
//                                TimePolicyType,
//                                ConfigurationType,
//                                ControlMessageType,
//                                DataMessageType,
//                                SessionMessageType,
//                                SessionDataContainerType>::initialize (const struct Common_ParserConfiguration& configuration_in)
//{
//  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_MAVLinkDecoder_T::initialize"));

//  ACE_UNUSED_ARG (configuration_in);

//  return true;
//}
//template <ACE_SYNCH_DECL,
//          typename TimePolicyType,
//          typename ConfigurationType,
//          typename ControlMessageType,
//          typename DataMessageType,
//          typename SessionMessageType,
//          typename SessionDataContainerType>
//void
//ARDrone_Module_MAVLinkDecoder_T<ACE_SYNCH_USE,
//                                TimePolicyType,
//                                ConfigurationType,
//                                ControlMessageType,
//                                DataMessageType,
//                                SessionMessageType,
//                                SessionDataContainerType>::dump_state () const
//{
//  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_MAVLinkDecoder_T::dump_state"));

//}
template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataContainerType>
void
ARDrone_Module_MAVLinkDecoder_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionDataContainerType>::error (const std::string& message_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_MAVLinkDecoder_T::error"));

  // *NOTE*: the output format has been "adjusted" to fit in with bison error-reporting
  ACE_DEBUG ((LM_ERROR,
              ACE_TEXT ("\": \"%s\"...\n"),
              ACE_TEXT (message_in.c_str ())));
//   ACE_DEBUG((LM_ERROR,
//              ACE_TEXT("failed to parse \"%s\": \"%s\"...\n"),
//              std::string(fragment_->rd_ptr(), fragment_->length()).c_str(),
//              message_in.c_str()));

//   std::clog << message_in << std::endl;
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataContainerType>
bool
ARDrone_Module_MAVLinkDecoder_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionDataContainerType>::switchBuffer (bool unlink_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_MAVLinkDecoder_T::switchBuffer"));

  ACE_UNUSED_ARG (unlink_in);

  // sanity check(s)
  ACE_ASSERT (buffer_);
  ACE_ASSERT (scannerState_);

  // retrieve trailing chunk
  ACE_Message_Block* message_block_p = buffer_;
  ACE_Message_Block* message_block_2 = NULL;
  do
  {
    message_block_2 = message_block_p->cont ();
    if (message_block_2)
      message_block_p = message_block_2;
    else
      break;
  } while (true);

  ACE_ASSERT (!message_block_p->cont ());
  waitBuffer (); // <-- wait for data
  message_block_2 = message_block_p->cont ();
  if (!message_block_2)
  {
    // *NOTE*: most probable reason: received session end
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("no data after ARDrone_Module_MAVLinkDecoder_T::waitBuffer(), aborting\n")));
    return false;
  } // end IF

  // switch to the next fragment

  // clean state
  scan_end ();

  // initialize next buffer

  // append the "\0\0"-sequence, as required by flex
  ACE_ASSERT (message_block_2->capacity () - message_block_2->length () >=
              STREAM_DECODER_FLEX_BUFFER_BOUNDARY_SIZE);
  *(message_block_2->wr_ptr ()) = YY_END_OF_BUFFER_CHAR;
  *(message_block_2->wr_ptr () + 1) = YY_END_OF_BUFFER_CHAR;
  // *NOTE*: DO NOT adjust the write pointer --> length() must stay as it was

  if (!scan_begin (message_block_2->rd_ptr (),
                   message_block_2->length ()))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ARDrone_Module_MAVLinkDecoder_T::scan_begin(), aborting\n")));
    return false;
  } // end IF

  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("switched input buffers...\n")));

  return true;
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataContainerType>
void
ARDrone_Module_MAVLinkDecoder_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionDataContainerType>::waitBuffer ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_MAVLinkDecoder_T::waitBuffer"));

  int result = -1;
  ACE_Message_Block* message_block_p = NULL;
  bool done = false;
  SessionMessageType* session_message_p = NULL;
//  enum Stream_SessionMessageType session_message_type =
//      STREAM_SESSION_MESSAGE_INVALID;
  bool is_data = false;
  bool stop_processing = false;

  // *IMPORTANT NOTE*: 'this' is the parser thread currently blocked in yylex()

  //// sanity check(s)
  //ACE_ASSERT (blockInParse_);

  // 1. wait for data
  do
  {
    result = inherited::msg_queue_->dequeue_head (message_block_p,
                                                  NULL);
    if (result == -1)
    {
      int error = ACE_OS::last_error ();
      if (error != ESHUTDOWN)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Message_Queue::dequeue_head(): \"%m\", returning\n")));
      return;
    } // end IF
    ACE_ASSERT (message_block_p);

    switch (message_block_p->msg_type ())
    {
      case ACE_Message_Block::MB_DATA:
      case ACE_Message_Block::MB_PROTO:
        is_data = true; break;
      case ACE_Message_Block::MB_USER:
      {
        session_message_p = dynamic_cast<SessionMessageType*> (message_block_p);
        if (session_message_p)
        {
          switch (session_message_p->type ())
          {
            case STREAM_SESSION_MESSAGE_END:
            {
              done = true; // session has finished --> abort

              break;
            }
            default:
            {
              inherited::handleMessage (session_message_p,
                                        stop_processing);
              if (stop_processing)
                done = true; // session has finished (error) --> abort
              message_block_p = NULL;
              break;
            }
          } // end SWITCH
        } // end IF
        break;
      }
      default:
        break;
    } // end SWITCH
    if (is_data) break;

    // requeue message ?
    if (message_block_p)
    {
      result = inherited::msg_queue_->enqueue_tail (message_block_p, NULL);
      if (result == -1)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Message_Queue::enqueue_tail(): \"%m\", returning\n")));
        return;
      } // end IF
      message_block_p = NULL;
    } // end IF
  } while (!done);

  // 2. append data ?
  if (message_block_p)
  {
    // sanity check(s)
    ACE_ASSERT (buffer_);

    ACE_Message_Block* message_block_2 = buffer_;
    for (;
         message_block_2->cont ();
         message_block_2 = message_block_2->cont ());
    message_block_2->cont (message_block_p);
  } // end IF
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataContainerType>
bool
ARDrone_Module_MAVLinkDecoder_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionDataContainerType>::scan_begin (const char* data_in,
                                                                       unsigned int length_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_MAVLinkDecoder_T::scan_begin"));

  // sanity check(s)
  ACE_ASSERT (!bufferState_);
  ACE_ASSERT (scannerState_);
  ACE_ASSERT (data_in);

  // create/initialize a new buffer state
  bufferState_ =
    (useYYScanBuffer_ ? ARDrone_MAVLink_Scanner__scan_buffer (const_cast<char*> (data_in),
                                                              length_in + STREAM_DECODER_FLEX_BUFFER_BOUNDARY_SIZE,
                                                              scannerState_)
                      : ARDrone_MAVLink_Scanner__scan_bytes (data_in,
                                                             length_in,
                                                             scannerState_));
  if (!bufferState_)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to yy_scan_[buffer/bytes](0x%@, %u): \"%m\", aborting\n"),
                inherited::mod_->name (),
                data_in,
                length_in));
    return false;
  } // end IF
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("parsing fragment #%d --> %d byte(s)...\n"),
//              counter++,
//              fragment_->length ()));

//  // *WARNING*: contrary (!) to the documentation, still need to switch_buffers()...
//  HTTP_Scanner__switch_to_buffer (bufferState_,
//                                  scannerState_);

  return true;
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataContainerType>
void
ARDrone_Module_MAVLinkDecoder_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionDataContainerType>::scan_end ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_MAVLinkDecoder_T::scan_end"));

  // sanity check(s)
  ACE_ASSERT (bufferState_);

  // clean state
  ARDrone_MAVLink_Scanner__delete_buffer (bufferState_,
                                          scannerState_);
  bufferState_ = NULL;
}
