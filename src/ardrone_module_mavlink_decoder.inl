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
          typename SessionMessageType>
ARDrone_Module_MAVLinkDecoder_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//                                SessionMessageType>::ARDrone_Module_MAVLinkDecoder_T (ISTREAM_T* stream_in)
//#else
                                SessionMessageType>::ARDrone_Module_MAVLinkDecoder_T (typename inherited::ISTREAM_T* stream_in)
//#endif
 : inherited (stream_in)
 , buffer_ (NULL)
 , fragment_ (NULL)
 , isFirst_ (true)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_MAVLinkDecoder_T::ARDrone_Module_MAVLinkDecoder_T"));

}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType>
void
ARDrone_Module_MAVLinkDecoder_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType>::record (struct __mavlink_message*& record_inout)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_MAVLinkDecoder_T::record"));

  // sanity check(s)
  ACE_ASSERT (inherited::sessionData_);
  ACE_ASSERT (fragment_);
  ACE_ASSERT (record_inout);

  // frame the MAVLink message data
  int result = -1;
  size_t remaining_bytes = record_inout->len + MAVLINK_NUM_CHECKSUM_BYTES;
  size_t remaining_bytes_2 = MAVLINK_NUM_HEADER_BYTES;
  size_t length = 0;
  size_t trailing_bytes_total, trailing_bytes = 0;
  ACE_Message_Block* message_block_p = fragment_;
  ACE_Message_Block* message_block_2 = NULL;
  typename DataMessageType::DATA_T* message_data_container_p = NULL;
  typename DataMessageType::DATA_T::DATA_T* message_data_p = NULL;
  const typename SessionMessageType::DATA_T::DATA_T& session_data_r =
    inherited::sessionData_->getR ();
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

  length = fragment_->total_length ();
  if (length > remaining_bytes)
  {
    trailing_bytes_total = length - remaining_bytes;

    message_block_2 = fragment_;
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
    ACE_ASSERT (fragment_->total_length () == static_cast<unsigned int> (record_inout->len + MAVLINK_NUM_CHECKSUM_BYTES));
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
//    buffer_->release (); buffer_ = NULL;
//    goto error;
//  } // end IF

  result = inherited::put_next (fragment_, NULL);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to ACE_Task_T::put_next(): \"%m\", returning\n"),
                inherited::mod_->name ()));

    // clean up
    fragment_->release (); fragment_ = NULL;

    goto error;
  } // end IF
  fragment_ = NULL;

  if (message_block_p)
  {
    fragment_ = static_cast<DataMessageType*> (message_block_p);
    ACE_ASSERT (fragment_);

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
    fragment_->initialize (message_data_container_p,
                           session_data_r.sessionId,
                           NULL);
    message_data_container_p = NULL;
    fragment_->set (ARDRONE_MESSAGE_MAVLINK);
    //inherited::fragment_->set_2 (inherited::stream_);
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
          typename SessionMessageType>
void
ARDrone_Module_MAVLinkDecoder_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType>::error (const std::string& message_in)
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
          typename SessionMessageType>
void
ARDrone_Module_MAVLinkDecoder_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType>::waitBuffer ()
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
  typename SessionMessageType::DATA_T::DATA_T* session_data_p = NULL;

  // *IMPORTANT NOTE*: 'this' is the parser thread currently blocked in yylex()

  // sanity check(s)
  ACE_ASSERT (inherited::msg_queue_);
  ACE_ASSERT (inherited::sessionData_);
  //ACE_ASSERT (blockInParse_);

  session_data_p =
    &const_cast<typename SessionMessageType::DATA_T::DATA_T&> (inherited::sessionData_->getR ());

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
  if (fragment_)
  {
    ACE_Message_Block* message_block_2 = fragment_;
    for (;
         message_block_2->cont ();
         message_block_2 = message_block_2->cont ());
    message_block_2->cont (message_block_p);
  } // end IF
  else
  {
    fragment_ = static_cast<DataMessageType*> (message_block_p);
    ACE_ASSERT (fragment_);

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
    fragment_->initialize (message_data_container_p,
                           session_data_p->sessionId,
                           NULL);
    message_data_container_p = NULL;
    fragment_->set (ARDRONE_MESSAGE_MAVLINK);
    //fragment_->set_2 (inherited::stream_);
  } // end ELSE
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType>
bool
ARDrone_Module_MAVLinkDecoder_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType>::begin (const char* data_in,
                                                            unsigned int length_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_MAVLinkDecoder_T::begin"));

  // sanity check(s)
  ACE_ASSERT (!buffer_);
  ACE_ASSERT (state_);
  ACE_ASSERT (data_in);

  // create/initialize a new buffer state
  buffer_ =
    (inherited::configuration_->parserConfiguration->useYYScanBuffer ? ARDrone_MAVLink_Scanner__scan_buffer (const_cast<char*> (data_in),
                                                                                                             length_in + COMMON_PARSER_FLEX_BUFFER_BOUNDARY_SIZE,
                                                                                                             state_)
                                                                     : ARDrone_MAVLink_Scanner__scan_bytes (data_in,
                                                                                                            length_in,
                                                                                                            state_));
  if (!buffer_)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to yy_scan_[buffer/bytes](0x%@, %u): \"%m\", aborting\n"),
                inherited::mod_->name (),
                data_in,
                length_in));
    return false;
  } // end IF
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("parsing fragment #%d --> %d byte(s)\n"),
//              counter++,
//              inherited::fragment_->length ()));

  return true;
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType>
void
ARDrone_Module_MAVLinkDecoder_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType>::end ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_MAVLinkDecoder_T::end"));

  // sanity check(s)
  ACE_ASSERT (buffer_);

  // clean state
  ARDrone_MAVLink_Scanner__delete_buffer (buffer_,
                                          state_);
  buffer_ = NULL;
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType>
int
ARDrone_Module_MAVLinkDecoder_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType>::svc (void)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_MAVLinkDecoder_T::svc"));

  // sanity check(s)
  ACE_ASSERT (state_);

  ACE_Message_Block* message_block_p = NULL;
  int result = -1;
  int error = 0;
  typename DataMessageType::DATA_T* message_data_container_p = NULL;
  typename DataMessageType::DATA_T::DATA_T* message_data_p = NULL;
  bool do_scan_end = false;
  const typename SessionMessageType::DATA_T::DATA_T* session_data_p = NULL;
  bool result_2 = false;

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
      { ACE_ASSERT (!fragment_);
        fragment_ = dynamic_cast<DataMessageType*> (message_block_p);
        ACE_ASSERT (fragment_);

        if (fragment_->isInitialized ())
          goto continue_;

        // sanity check(s)
        ACE_ASSERT (inherited::sessionData_);

        session_data_p = &inherited::sessionData_->getR ();

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
        fragment_->initialize (message_data_container_p,
                               session_data_p->sessionId,
                               NULL);
        message_data_container_p = NULL;
        fragment_->set (ARDRONE_MESSAGE_MAVLINK);
        //fragment_->set_2 (inherited::stream_);

continue_:
        if (!begin (fragment_->rd_ptr (),
                    fragment_->length ()))
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
          ARDrone_MAVLink_Scanner_set_column (1, state_);
          ARDrone_MAVLink_Scanner_set_lineno (1, state_);
        } // end IF

        // parse data fragment
        try {
          result_2 = this->lex (state_);
        } catch (...) {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("%s: caught exception in yylex(), continuing\n"),
                      inherited::mod_->name ()));
          result_2 = false;
        }
        if (!result_2)
        {
          // *NOTE*: most probable reason: connection
          //         has been closed --> session end
          ACE_DEBUG ((LM_DEBUG,
                      ACE_TEXT ("%s: failed to parse MAVLink message(s): \"%m\", aborting\n"),
                      inherited::mod_->name ()));
          goto error;
        } // end IF

        // clean up
        end ();
        do_scan_end = false;

        // more data ?
        if (fragment_)
          goto continue_;

        break;

error:
        if (message_data_p)
          delete message_data_p;
        if (message_data_container_p)
          message_data_container_p->decrease ();
        if (message_block_p)
          message_block_p->release ();
        if (do_scan_end)
          end ();

        goto done;
      }
    } // end SWITCH
  } while (true);
  result = -1;

done:
  return result;
}
