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

#include "ace/Log_Msg.h"

#include "net_defines.h"

#include "ardrone_macros.h"
#include "ardrone_navdata_scanner.h"

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataContainerType>
ARDrone_Module_NavDataDecoder_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                                SessionDataContainerType>::ARDrone_Module_NavDataDecoder_T (ISTREAM_T* stream_in)
#else
                                SessionDataContainerType>::ARDrone_Module_NavDataDecoder_T (typename inherited::ISTREAM_T* stream_in)
#endif
 : inherited (stream_in)
 , buffer_ (NULL)
 , bufferState_ (NULL)
 , isFirst_ (true)
 , numberOfOptions_ (0)
 , scannerState_ (NULL)
 , useYYScanBuffer_ (NET_PROTOCOL_PARSER_FLEX_USE_YY_SCAN_BUFFER)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_NavDataDecoder_T::ARDrone_Module_NavDataDecoder_T"));

  // step1: initialize flex state
  int result = ARDrone_NavData_Scanner_lex_init_extra (this,
                                                       &scannerState_);
  if (result)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to yylex_init_extra: \"%m\", returning\n"),
                inherited::mod_->name ()));
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
ARDrone_Module_NavDataDecoder_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionDataContainerType>::~ARDrone_Module_NavDataDecoder_T ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_NavDataDecoder_T::~ARDrone_Module_NavDataDecoder_T"));

  // clean up any unprocessed (chained) buffer(s)
  if (buffer_)
    buffer_->release ();

  if (ARDrone_NavData_Scanner_lex_destroy (scannerState_))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to yylex_destroy(): \"%m\", continuing\n"),
                inherited::mod_->name ()));
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataContainerType>
bool
ARDrone_Module_NavDataDecoder_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionDataContainerType>::initialize (const ConfigurationType& configuration_in,
                                                                       Stream_IAllocator* allocator_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_NavDataDecoder_T::initialize"));

  int result = -1;

  // sanity check(s)
  // *TODO*: remove type inferences
  ACE_ASSERT (configuration_in.parserConfiguration);
  ACE_ASSERT (configuration_in.streamConfiguration);

  if (inherited::isInitialized_)
  {
    if (buffer_)
      buffer_->release ();
    buffer_ = NULL;

    if (bufferState_)
    { ACE_ASSERT (scannerState_);
      ARDrone_NavData_Scanner__delete_buffer (bufferState_,
                                              scannerState_);
      bufferState_ = NULL;
    } // end IF

    isFirst_ = true;

    useYYScanBuffer_ = NET_PROTOCOL_PARSER_FLEX_USE_YY_SCAN_BUFFER;
  } // end IF

  ACE_ASSERT (inherited::msg_queue_);
  result = inherited::msg_queue_->activate ();
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to ACE_Message_Queue::activate(): \"%m\", aborting\n"),
                inherited::mod_->name ()));
    return false;
  } // end IF

  // trace ?
  ARDrone_NavData_Scanner_set_debug (configuration_in.debugScanner,
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
ARDrone_Module_NavDataDecoder_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionDataContainerType>::handleDataMessage (DataMessageType*& message_inout,
                                                                              bool& passMessageDownstream_out)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_NavDataDecoder_T::handleDataMessage"));

  int result = -1;

  // initialize return value(s)
  // *NOTE*: the default behavior is to pass all messages along
  //         --> in this case, the individual frames are extracted and passed
  //             as such
  passMessageDownstream_out = false;

  // append the "\0\0"-sequence, as required by flex
  ACE_ASSERT (message_inout->capacity () - message_inout->length () >= NET_PROTOCOL_PARSER_FLEX_BUFFER_BOUNDARY_SIZE);
  *(message_inout->wr_ptr ()) = YY_END_OF_BUFFER_CHAR;
  *(message_inout->wr_ptr () + 1) = YY_END_OF_BUFFER_CHAR;
  // *NOTE*: DO NOT adjust the write pointer --> length() must stay as it was

  result = inherited::msg_queue_->enqueue_tail (message_inout,
                                                NULL);
  if (result == -1)
  {
    int error = ACE_OS::last_error ();
    if (error != ESHUTDOWN)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to ACE_Message_Queue::enqueue_tail(): \"%m\", returning\n"),
                  inherited::mod_->name ()));

    // clean up
    message_inout->release ();
    message_inout = NULL;

    return;
  } // end IF
  message_inout = NULL;
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataContainerType>
void
ARDrone_Module_NavDataDecoder_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionDataContainerType>::handleSessionMessage (SessionMessageType*& message_inout,
                                                                                 bool& passMessageDownstream_out)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_NavDataDecoder_T::handleSessionMessage"));

  int result = -1;

  // don't care (implies yes per default, if part of a stream)
  ACE_UNUSED_ARG (passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT (inherited::isInitialized_);

  switch (message_inout->type ())
  {
    case STREAM_SESSION_MESSAGE_BEGIN:
    {
      result = inherited::activate ();
      if (result == -1)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to ACE_Task_T::activate(): \"%m\", aborting\n"),
                    inherited::mod_->name ()));
        goto error;
      } // end IF

      goto continue_;

error:
      this->notify (STREAM_SESSION_MESSAGE_ABORT);

      break;

continue_:
      break;
    }
    case STREAM_SESSION_MESSAGE_END:
    {
      if (inherited::thr_count_)
        inherited::stop (false, // wait for completion ?
                         true); // locked access ?
      else
      {
        ACE_ASSERT (inherited::msg_queue_);
        result = inherited::msg_queue_->deactivate ();
        if (result == -1)
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("%s: failed to ACE_Message_Queue::deactivate(): \"%m\", continuing\n"),
                      inherited::mod_->name ()));
      } // end ELSE

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
ARDrone_Module_NavDataDecoder_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionDataContainerType>::record (struct _navdata_t*& record_inout)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_NavDataDecoder_T::record"));

  // sanity check(s)
  ACE_ASSERT (inherited::sessionData_);
  ACE_ASSERT (buffer_);
  ACE_ASSERT (record_inout);

  // frame the NavData message data
  int result = -1;
  typename DataMessageType::DATA_T::DATA_T* message_data_p = NULL;
  typename DataMessageType::DATA_T* message_data_container_p = NULL;
  unsigned int missing_bytes =
    sizeof (struct _navdata_t) - sizeof (struct _navdata_option_t);

  const typename DataMessageType::DATA_T& message_data_container_r =
    buffer_->getR ();
  const typename DataMessageType::DATA_T::DATA_T& message_data_r =
    message_data_container_r.getR ();
  ACE_ASSERT (message_data_r.messageType == ARDRONE_MESSAGE_NAVDATA);
  struct _navdata_option_t* options_p =
    reinterpret_cast<struct _navdata_option_t*> (buffer_->rd_ptr () + missing_bytes);
  for (ARDrone_NavDataOptionOffsetsIterator_t iterator = message_data_r.NavData.NavDataOptionOffsets.begin ();
       iterator != message_data_r.NavData.NavDataOptionOffsets.end ();
       ++iterator)
  {
    missing_bytes +=
      ((ACE_BYTE_ORDER != ACE_LITTLE_ENDIAN) ? ACE_SWAP_WORD (options_p->size)
                                             : options_p->size);
    if (options_p->tag == NAVDATA_CKS_TAG)
      break;
    options_p =
      reinterpret_cast<struct _navdata_option_t*> (buffer_->rd_ptr () + missing_bytes);
  } // end FOR
  ACE_ASSERT (buffer_->total_length () >= missing_bytes);
  const typename SessionDataContainerType::DATA_T& session_data_r =
    inherited::sessionData_->getR ();

  unsigned int trailing_bytes = 0;
  unsigned int length = 0;
  unsigned int bytes_to_skip = 0;
  ACE_Message_Block* message_block_p = NULL;
  ACE_Message_Block* message_block_2 = NULL;
  for (message_block_2 = buffer_;
       message_block_2;
       message_block_2 = message_block_2->next ())
  {
    length = message_block_2->length ();
    bytes_to_skip = (length >= missing_bytes ? missing_bytes : length);
    missing_bytes -= bytes_to_skip;
    if (!missing_bytes)
    {
      trailing_bytes = length - bytes_to_skip;
      break;
    } // end IF
  } // end FOR
  if (trailing_bytes)
  {
    ACE_ASSERT (message_block_2);
    ACE_ASSERT (trailing_bytes < length);
    message_block_p = message_block_2->duplicate ();
    if (!message_block_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to DataMessageType::duplicate(): \"%m\", returning\n"),
                  inherited::mod_->name ()));
      goto error;
    } // end IF
    message_block_2->length (length - trailing_bytes);
    message_block_2->cont (message_block_p);
    message_block_p->rd_ptr (length - trailing_bytes);
  } // end IF

  // validate header
  ACE_ASSERT (record_inout->header == NAVDATA_HEADER);
  // *TODO*: verify checksum
  
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
    message_data_p->messageType = ARDRONE_MESSAGE_NAVDATA;
    ACE_OS::memset (&message_data_p->NavData,
                    0,
                    sizeof (struct _navdata_t));
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
    buffer_->set (ARDRONE_MESSAGE_NAVDATA);
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
ARDrone_Module_NavDataDecoder_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionDataContainerType>::error (const std::string& message_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_NavDataDecoder_T::error"));

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
bool
ARDrone_Module_NavDataDecoder_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionDataContainerType>::switchBuffer (bool unlink_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_NavDataDecoder_T::switchBuffer"));

  ACE_UNUSED_ARG (unlink_in);

  // sanity check(s)
  ACE_ASSERT (scannerState_);

  ACE_Message_Block* message_block_p = buffer_;
  ACE_Message_Block* message_block_2 = NULL;

  // retrieve trailing chunk
  if (!buffer_)
    goto continue_;

  do
  {
    message_block_2 = message_block_p->cont ();
    if (message_block_2)
      message_block_p = message_block_2;
    else
      break;
  } while (true);
  ACE_ASSERT (!message_block_p->cont ());

continue_:
  waitBuffer (); // <-- wait for data

  message_block_2 = message_block_p ? message_block_p->cont ()
                                    : buffer_;
  if (!message_block_2)
  {
    // *NOTE*: most probable reason: received session end
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("%s: no data after waitBuffer(), aborting\n"),
                inherited::mod_->name ()));
    return false;
  } // end IF

  // switch to the next fragment

  // clean state
  end ();

  // initialize next buffer

  // append the "\0\0"-sequence, as required by flex
  ACE_ASSERT (message_block_2->capacity () - message_block_2->length () >= NET_PROTOCOL_PARSER_FLEX_BUFFER_BOUNDARY_SIZE);
  *(message_block_2->wr_ptr ()) = YY_END_OF_BUFFER_CHAR;
  *(message_block_2->wr_ptr () + 1) = YY_END_OF_BUFFER_CHAR;
  // *NOTE*: DO NOT adjust the write pointer --> length() must stay as it was

  if (!begin (message_block_2->rd_ptr (),
              message_block_2->length ()))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to begin(), aborting\n"),
                inherited::mod_->name ()));
    return false;
  } // end IF

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
ARDrone_Module_NavDataDecoder_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionDataContainerType>::waitBuffer ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_NavDataDecoder_T::waitBuffer"));

  int result = -1;
  ACE_Message_Block* message_block_p = NULL;
  bool done = false;
  SessionMessageType* session_message_p = NULL;
  bool is_data = false;
  bool stop_processing = false;
  typename DataMessageType::DATA_T* message_data_container_p = NULL;
  typename DataMessageType::DATA_T::DATA_T* message_data_p = NULL;

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
    message_data_p->messageType = ARDRONE_MESSAGE_NAVDATA;
    ACE_OS::memset (&message_data_p->NavData,
                    0,
                    sizeof (struct _navdata_t));
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
                         buffer_->sessionId (),
                         NULL);
    message_data_container_p = NULL;
    buffer_->set (ARDRONE_MESSAGE_NAVDATA);
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
ARDrone_Module_NavDataDecoder_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionDataContainerType>::begin (const char* data_in,
                                                                  unsigned int length_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_NavDataDecoder_T::begin"));

  // sanity check(s)
  ACE_ASSERT (!bufferState_);
  ACE_ASSERT (scannerState_);
  ACE_ASSERT (data_in);

  // create/initialize a new buffer state
  bufferState_ =
    (useYYScanBuffer_ ? ARDrone_NavData_Scanner__scan_buffer (const_cast<char*> (data_in),
                                                              length_in + NET_PROTOCOL_PARSER_FLEX_BUFFER_BOUNDARY_SIZE,
                                                              scannerState_)
                      : ARDrone_NavData_Scanner__scan_bytes (data_in,
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
ARDrone_Module_NavDataDecoder_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionDataContainerType>::end ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_NavDataDecoder_T::end"));

  // sanity check(s)
  ACE_ASSERT (bufferState_);

  // clean state
  ARDrone_NavData_Scanner__delete_buffer (bufferState_,
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
ARDrone_Module_NavDataDecoder_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionDataContainerType>::svc (void)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_NavDataDecoder_T::svc"));

  // sanity check(s)
  ACE_ASSERT (scannerState_);

  ACE_Message_Block* message_block_p = NULL;
  int result = -1;
  int error = 0;
  typename DataMessageType::DATA_T* message_data_container_p = NULL;
  typename DataMessageType::DATA_T::DATA_T* message_data_p = NULL;
  bool do_scan_end = false;

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
        {
          // sanity check(s)
          message_data_container_p =
            &const_cast<typename DataMessageType::DATA_T&> (buffer_->getR ());
          message_data_p =
            &const_cast<typename DataMessageType::DATA_T::DATA_T&> (message_data_container_p->getR ());
          ACE_ASSERT (message_data_p->messageType == ARDRONE_MESSAGE_NAVDATA);

          goto continue_;
        } // end IF

        ACE_NEW_NORETURN (message_data_p,
                          typename DataMessageType::DATA_T::DATA_T ());
        if (!message_data_p)
        {
          ACE_DEBUG ((LM_CRITICAL,
                      ACE_TEXT ("%s: failed to allocate memory: \"%m\", returning\n"),
                      inherited::mod_->name ()));
          goto error;
        } // end IF
        message_data_p->messageType = ARDRONE_MESSAGE_NAVDATA;
        ACE_OS::memset (&message_data_p->NavData,
                        0,
                        sizeof (struct _navdata_t));
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
                             buffer_->sessionId (),
                             NULL);
        message_data_container_p = NULL;
        buffer_->set (ARDRONE_MESSAGE_NAVDATA);
        //buffer_->set_2 (inherited::stream_);

continue_:
        if (!begin (buffer_->rd_ptr (),
                    buffer_->length ()))
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("%s: failed to Common_IScannerBase::begin(), aborting\n"),
                      inherited::mod_->name ()));
          goto error;
        } // end IF
        do_scan_end = true;

        // initialize scanner ?
        if (isFirst_)
        {
          isFirst_ = false;

          /* column is only valid if an input buffer exists. */
          ARDrone_NavData_Scanner_set_column (1, scannerState_);
          ARDrone_NavData_Scanner_set_lineno (1, scannerState_);
        } // end IF

        // parse data fragment
        try {
          result = ARDrone_NavData_Scanner_lex (scannerState_);
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
                        ACE_TEXT ("%s: failed to parse NavData message(s) (result was: %d), aborting\n"),
                        inherited::mod_->name (),
                        result));
            goto error;
          }
          default:
          {
            // clean up
            end ();
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
          end ();

        goto done;
      }
    } // end SWITCH
  } while (true);
  result = -1;

done:
  return result;
}
