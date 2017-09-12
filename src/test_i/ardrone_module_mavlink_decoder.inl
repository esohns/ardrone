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

#include "ace/ACE.h"
#include "ace/Log_Msg.h"

//#include "checksum.h"

#include "stream_misc_defines.h"

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
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                                SessionDataContainerType>::ARDrone_Module_MAVLinkDecoder_T (ISTREAM_T* stream_in)
#else
                                SessionDataContainerType>::ARDrone_Module_MAVLinkDecoder_T (typename inherited::ISTREAM_T* stream_in)
#endif
 : inherited (stream_in,
              STREAM_MISC_PARSER_DEFAULT_LEX_TRACE,
              STREAM_MISC_PARSER_DEFAULT_YACC_TRACE)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_MAVLinkDecoder_T::ARDrone_Module_MAVLinkDecoder_T"));

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
                                SessionDataContainerType>::record (struct __mavlink_message*& record_inout)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_MAVLinkDecoder_T::record"));

  // sanity check(s)
  ACE_ASSERT (inherited::sessionData_);
  ACE_ASSERT (buffer_);
  ACE_ASSERT (record_inout);

  // frame the MAVLink message data
  int result = -1;
  unsigned int remaining_bytes = record_inout->len + MAVLINK_NUM_CHECKSUM_BYTES;
  unsigned int remaining_bytes_2 = MAVLINK_NUM_HEADER_BYTES;
  unsigned int length = 0;
  unsigned int trailing_bytes_total, trailing_bytes = 0;
  ACE_Message_Block* message_block_p = buffer_;
  ACE_Message_Block* message_block_2 = NULL;
  typename DataMessageType::DATA_T* message_data_container_p = NULL;
  typename DataMessageType::DATA_T::DATA_T* message_data_p = NULL;
  const typename SessionDataContainerType::DATA_T& session_data_r =
    inherited::sessionData_->get ();
//  unsigned char byte_c = 0x00;
//  ACE_UINT16 checksum_i = 0;
//  ACE_UINT16 checksum_swapped = 0;

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
      length = (length >= remaining_bytes ? remaining_bytes : length);
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
      goto error;
    } // end IF
    message_block_2->length (message_block_2->length () - trailing_bytes);
    message_block_2->cont (NULL);
    ACE_ASSERT (buffer_->total_length () ==
                static_cast<unsigned int> (record_inout->len +
                                           MAVLINK_NUM_CHECKSUM_BYTES));
    message_block_p->rd_ptr (message_block_p->length () - trailing_bytes);
    ACE_ASSERT (message_block_p->total_length () == trailing_bytes_total);
  } // end IF

//  // validate checksum
//  byte_c = *(buffer_->rd_ptr () + record_inout->len + 1);
//  static const uint8_t mavlink_message_crcs[256] = MAVLINK_MESSAGE_CRCS;
//  *(buffer_->rd_ptr () + record_inout->len + 1) =
//      mavlink_message_crcs[record_inout->msgid];
//  checksum_i = ACE::crc_ccitt (buffer_->rd_ptr () - MAVLINK_CORE_HEADER_LEN,
//                               MAVLINK_CORE_HEADER_LEN + record_inout->len + 1,
//                               0);
//  checksum_i =
//      crc_calculate (reinterpret_cast<uint8_t*> (buffer_->rd_ptr ()) - MAVLINK_CORE_HEADER_LEN,
//                     MAVLINK_CORE_HEADER_LEN + record_inout->len + 1);
//  *(buffer_->rd_ptr () + record_inout->len + 1) = byte_c;
//  //  static const uint8_t mavlink_message_crcs[256] = MAVLINK_MESSAGE_CRCS;
////#define MAVLINK_MESSAGE_CRC(msgid) mavlink_message_crcs[msgid]
////  crc_accumulate (MAVLINK_MESSAGE_CRC (record_inout->msgid), &checksum_i);
//  checksum_swapped = ACE_SWAP_WORD (record_inout->checksum);
//  if (checksum_i != record_inout->checksum)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("%s: invalid checksum (message id: %u), continuing\n"),
//                inherited::mod_->name (),
//                buffer_->id ()));

//    // clean up
//    buffer_->release ();
//    buffer_ = NULL;

//    goto error;
//  } // end IF

  result = inherited::put_next (buffer_, NULL);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to ACE_Task_T::put_next(): \"%m\", returning\n"),
                inherited::mod_->name ()));

    // clean up
    buffer_->release ();
    buffer_ = NULL;

    goto error;
  } // end IF
  buffer_ = NULL;

  if (message_block_p)
  {
    buffer_ = dynamic_cast<DataMessageType*> (message_block_p);
    ACE_ASSERT (buffer_);

    ACE_NEW_NORETURN (message_data_p,
                      typename DataMessageType::DATA_T::DATA_T ());
    if (!message_data_p)
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("%s: failed to allocate memory: \"%m\", returning\n"),
                  inherited::mod_->name ()));
      goto error;
    } // end IF
    message_data_p->messageType = ARDRONE_MESSAGE_MAVLINK;
    ACE_OS::memset (&message_data_p->MAVLinkData,
                    0,
                    sizeof (struct __mavlink_message));
    ACE_NEW_NORETURN (message_data_container_p,
                      typename DataMessageType::DATA_T (message_data_p));
    if (!message_data_container_p)
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("%s: failed to allocate memory: \"%m\", returning\n"),
                  inherited::mod_->name ()));
      goto error;
    } // end IF
    message_data_p = NULL;
    buffer_->initialize (message_data_container_p,
                         session_data_r.sessionId,
                         NULL);
    message_data_container_p = NULL;
    buffer_->set (ARDRONE_MESSAGE_MAVLINK);
    //buffer_->set_2 (inherited::stream_);
  } // end IF

  return;

error:
  if (message_block_p)
    message_block_p->release ();
  if (message_data_p)
    delete message_data_p;
  if (message_data_container_p)
    message_data_container_p->decrease ();
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
                                SessionDataContainerType>::error (const std::string& message_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_MAVLinkDecoder_T::error"));

  // *NOTE*: the output format has been "adjusted" to fit in with bison error-reporting
  ACE_DEBUG ((LM_ERROR,
              ACE_TEXT ("%s: failed to parse message: \"%s\"\n"),
              inherited::mod_->name (),
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
  bool is_data = false;
  bool stop_processing = false;
  typename DataMessageType::DATA_T* message_data_container_p = NULL;
  typename DataMessageType::DATA_T::DATA_T* message_data_p = NULL;
  typename SessionDataContainerType::DATA_T* session_data_p = NULL;

  // *IMPORTANT NOTE*: 'this' is the parser thread currently blocked in yylex()

  // sanity check(s)
  ACE_ASSERT (inherited::msg_queue_);
  ACE_ASSERT (inherited::sessionData_);
  //ACE_ASSERT (blockInParse_);

  session_data_p =
    &const_cast<typename SessionDataContainerType::DATA_T&> (inherited::sessionData_->get ());

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
                    ACE_TEXT ("%s: failed to ACE_Message_Queue::dequeue_head(): \"%m\", returning\n"),
                    inherited::mod_->name ()));
      return;
    } // end IF
    ACE_ASSERT (message_block_p);

    switch (message_block_p->msg_type ())
    {
      case ACE_Message_Block::MB_DATA:
      case ACE_Message_Block::MB_PROTO:
        is_data = true;
        break;
      case ACE_Message_Block::MB_STOP:
        done = true; // session has finished --> abort
        break;
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
    if (is_data)
      break;

    // requeue message ?
    if (message_block_p)
    {
      result = inherited::msg_queue_->enqueue_tail (message_block_p, NULL);
      if (result == -1)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to ACE_Message_Queue::enqueue_tail(): \"%m\", returning\n"),
                    inherited::mod_->name ()));
        return;
      } // end IF
      message_block_p = NULL;
    } // end IF
  } while (!done);
  if (!message_block_p)
    return;

  // 2. append data ?
  if (buffer_)
  {
    ACE_Message_Block* message_block_2 = buffer_;
    for (;
         message_block_2->cont ();
         message_block_2 = message_block_2->cont ());
    message_block_2->cont (message_block_p);
  } // end IF
  else
  {
    buffer_ = dynamic_cast<DataMessageType*> (message_block_p);
    ACE_ASSERT (buffer_);

    ACE_NEW_NORETURN (message_data_p,
                      typename DataMessageType::DATA_T::DATA_T ());
    if (!message_data_p)
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("%s: failed to allocate memory: \"%m\", returning\n"),
                  inherited::mod_->name ()));
      return;
    } // end IF
    message_data_p->messageType = ARDRONE_MESSAGE_MAVLINK;
    ACE_OS::memset (&message_data_p->MAVLinkData,
                    0,
                    sizeof (struct __mavlink_message));
    ACE_NEW_NORETURN (message_data_container_p,
                      typename DataMessageType::DATA_T (message_data_p));
    if (!message_data_container_p)
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("%s: failed to allocate memory: \"%m\", returning\n"),
                  inherited::mod_->name ()));

      // clean up
      delete message_data_p;

      return;
    } // end IF
    message_data_p = NULL;
    buffer_->initialize (message_data_container_p,
                         session_data_p->sessionId,
                         NULL);
    message_data_container_p = NULL;
    buffer_->set (ARDRONE_MESSAGE_MAVLINK);
    //buffer_->set_2 (inherited::stream_);
  } // end ELSE
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
                                                              length_in + NET_PROTOCOL_PARSER_FLEX_BUFFER_BOUNDARY_SIZE,
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

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataContainerType>
int
ARDrone_Module_MAVLinkDecoder_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionDataContainerType>::svc (void)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_MAVLinkDecoder_T::svc"));

  // sanity check(s)
  ACE_ASSERT (scannerState_);

  ACE_Message_Block* message_block_p = NULL;
  int result = -1;
  int error = 0;
  typename DataMessageType::DATA_T* message_data_container_p = NULL;
  typename DataMessageType::DATA_T::DATA_T* message_data_p = NULL;
  bool do_scan_end = false;
  const typename SessionMessageType::DATA_T::DATA_T& session_data_p = NULL;

  do
  {
    message_block_p = NULL;
    result = inherited::getq (message_block_p,
                              NULL);
    if (result == -1)
    {
      error = ACE_OS::last_error ();
      if (error != EWOULDBLOCK) // Win32: 10035
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: worker thread (ID: %t) failed to ACE_Task::getq(): \"%m\", aborting\n"),
                    inherited::mod_->name ()));
      break;
    } // end IF
    ACE_ASSERT (message_block_p);

    switch (message_block_p->msg_type ())
    {
      case ACE_Message_Block::MB_STOP:
      {
        // *NOTE*: when close()d manually (i.e. user abort), 'finished' will
        //         not have been set at this stage

        message_block_p->release ();

        result = 0;

        goto done;
      }
      default:
      { ACE_ASSERT (!buffer_);
        buffer_ = dynamic_cast<DataMessageType*> (message_block_p);
        ACE_ASSERT (buffer_);

        if (buffer_->isInitialized ())
          goto continue_;

        // sanity check(s)
        ACE_ASSERT (inherited::sessionData_);

        session_data_p = &inherited::sessionData_->get ();

        ACE_NEW_NORETURN (message_data_p,
                          typename DataMessageType::DATA_T::DATA_T ());
        if (!message_data_p)
        {
          ACE_DEBUG ((LM_CRITICAL,
                      ACE_TEXT ("%s: failed to allocate memory: \"%m\", returning\n"),
                      inherited::mod_->name ()));
          goto error;
        } // end IF
        message_data_p->messageType = ARDRONE_MESSAGE_MAVLINK;
        ACE_OS::memset (&message_data_p->MAVLinkData,
                        0,
                        sizeof (struct __mavlink_message));
        ACE_NEW_NORETURN (message_data_container_p,
                          typename DataMessageType::DATA_T (message_data_p));
        if (!message_data_container_p)
        {
          ACE_DEBUG ((LM_CRITICAL,
                      ACE_TEXT ("%s: failed to allocate memory: \"%m\", returning\n"),
                      inherited::mod_->name ()));
          goto error;
        } // end IF
        message_data_p = NULL;
        buffer_->initialize (message_data_container_p,
                             session_data_p->sessionId,
                             NULL);
        message_data_container_p = NULL;
        buffer_->set (ARDRONE_MESSAGE_MAVLINK);
        //buffer_->set_2 (inherited::stream_);

continue_:
        if (!scan_begin (buffer_->rd_ptr (),
                         buffer_->length ()))
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("%s: failed to ARDrone_Module_MAVLinkDecoder_T::scan_begin(), aborting\n"),
                      inherited::mod_->name ()));
          goto error;
        } // end IF
        do_scan_end = true;

        // initialize scanner ?
        if (isFirst_)
        {
          isFirst_ = false;

          /* column is only valid if an input buffer exists. */
          ARDrone_MAVLink_Scanner_set_column (1, scannerState_);
          ARDrone_MAVLink_Scanner_set_lineno (1, scannerState_);
        } // end IF

        // parse data fragment
        try {
          result = ARDrone_MAVLink_Scanner_lex (scannerState_,
                                                this);
        } catch (...) {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("%s: caught exception in yylex(), continuing\n"),
                      inherited::mod_->name ()));
          result = 1;
        }
        switch (result)
        {
          case -1:
          {
            // *NOTE*: most probable reason: connection
            //         has been closed --> session end
            ACE_DEBUG ((LM_DEBUG,
                        ACE_TEXT ("%s: failed to parse MAVLink message(s) (result was: %d), aborting\n"),
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
              goto continue_;

            break;
          }
        } // end SWITCH

        break;

error:
        if (message_data_p)
          delete message_data_p;
        if (message_data_container_p)
          message_data_container_p->decrease ();
        if (message_block_p)
          message_block_p->release ();
        if (do_scan_end)
          scan_end ();

        goto done;
      }
    } // end SWITCH
  } while (true);
  result = -1;

done:
  return result;
}
