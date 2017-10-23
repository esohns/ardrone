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

#include "ardrone_control_scanner.h"
#include "ardrone_macros.h"

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataContainerType>
ARDrone_Module_ControlDecoder_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                                SessionDataContainerType>::ARDrone_Module_ControlDecoder_T (ISTREAM_T* stream_in)
#else
                                SessionDataContainerType>::ARDrone_Module_ControlDecoder_T (typename inherited::ISTREAM_T* stream_in)
#endif
 : inherited (stream_in,
              STREAM_MISC_PARSER_DEFAULT_LEX_TRACE,
              STREAM_MISC_PARSER_DEFAULT_YACC_TRACE)
 , configuration_ ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_ControlDecoder_T::ARDrone_Module_ControlDecoder_T"));

}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataContainerType>
void
ARDrone_Module_ControlDecoder_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionDataContainerType>::record (ARDrone_DeviceConfiguration_t*& record_inout)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_ControlDecoder_T::record"));

  int result = -1;

  // sanity check(s)
  ACE_ASSERT (inherited::fragment_);
  ACE_ASSERT (record_inout);

  const typename DataMessageType::DATA_T& message_data_container_r =
    inherited::fragment_->getR ();
  typename DataMessageType::DATA_T::DATA_T& message_data_r =
    const_cast<typename DataMessageType::DATA_T::DATA_T&> (message_data_container_r.getR ());

  // sanity check(s)
  ACE_ASSERT (message_data_r.messageType == ARDRONE_MESSAGE_CONTROL);

  message_data_r.controlData = configuration_;

  result = inherited::put_next (inherited::fragment_, NULL);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to ACE_Task_T::put_next(): \"%m\", returning\n"),
                inherited::mod_->name ()));

    // clean up
    inherited::fragment_->release ();
  } // end IF
  inherited::fragment_ = NULL;
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataContainerType>
void
ARDrone_Module_ControlDecoder_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionDataContainerType>::error (const std::string& message_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_ControlDecoder_T::error"));

  // *NOTE*: the output format has been "adjusted" to fit in with bison error-reporting
  ACE_DEBUG ((LM_ERROR,
              ACE_TEXT ("%s: failed to parse message: \"%s\"\n"),
              inherited::mod_->name (),
              ACE_TEXT (message_in.c_str ())));
//   ACE_DEBUG((LM_ERROR,
//              ACE_TEXT("failed to parse \"%s\": \"%s\"\n"),
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
ARDrone_Module_ControlDecoder_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionDataContainerType>::waitBuffer ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_ControlDecoder_T::waitBuffer"));

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
      {
        if (!inherited::msg_queue_->message_count ())
          done = true; // session has finished --> abort
        break;
      }
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
  if (inherited::fragment_)
  {
    ACE_Message_Block* message_block_2 = inherited::fragment_;
    for (;
         message_block_2->cont ();
         message_block_2 = message_block_2->cont ());
    message_block_2->cont (message_block_p);
  } // end IF
  else
  {
    // sanity check(s)
    ACE_ASSERT (inherited::sessionData_);

    session_data_p =
      &const_cast<typename SessionDataContainerType::DATA_T&> (inherited::sessionData_->getR ());

    inherited::fragment_ = dynamic_cast<DataMessageType*> (message_block_p);
    ACE_ASSERT (inherited::fragment_);

    ACE_NEW_NORETURN (message_data_p,
                      typename DataMessageType::DATA_T::DATA_T ());
    if (!message_data_p)
    {
      ACE_DEBUG ((LM_CRITICAL,
                  ACE_TEXT ("%s: failed to allocate memory: \"%m\", returning\n"),
                  inherited::mod_->name ()));
      return;
    } // end IF
    message_data_p->messageType = ARDRONE_MESSAGE_CONTROL;
    message_data_p->controlData.clear ();
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
    inherited::fragment_->initialize (message_data_container_p,
                                      session_data_p->sessionId,
                                      NULL);
    message_data_container_p = NULL;
    inherited::fragment_->set (ARDRONE_MESSAGE_CONTROL);
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
ARDrone_Module_ControlDecoder_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionDataContainerType>::begin (const char* data_in,
                                                                  unsigned int length_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_ControlDecoder_T::begin"));

  // sanity check(s)
  ACE_ASSERT (!inherited::buffer_);
  ACE_ASSERT (inherited::state_);
  ACE_ASSERT (data_in);

  // create/initialize a new buffer state
  inherited::buffer_ =
    (inherited::useYYScanBuffer_ ? ARDrone_Control_Scanner__scan_buffer (const_cast<char*> (data_in),
                                                                         length_in + NET_PROTOCOL_PARSER_FLEX_BUFFER_BOUNDARY_SIZE,
                                                                         inherited::state_)
                                 : ARDrone_Control_Scanner__scan_bytes (data_in,
                                                                        length_in,
                                                                        inherited::state_));
  if (!inherited::buffer_)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to yy_scan_[buffer/bytes](0x%@, %u): \"%m\", aborting\n"),
                inherited::mod_->name (),
                data_in,
                length_in));
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
ARDrone_Module_ControlDecoder_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionDataContainerType>::end ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_ControlDecoder_T::end"));

  // sanity check(s)
  ACE_ASSERT (inherited::buffer_);

  // clean state
  ARDrone_Control_Scanner__delete_buffer (inherited::buffer_,
                                          inherited::state_);
  inherited::buffer_ = NULL;
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataContainerType>
int
ARDrone_Module_ControlDecoder_T<ACE_SYNCH_USE,
                                TimePolicyType,
                                ConfigurationType,
                                ControlMessageType,
                                DataMessageType,
                                SessionMessageType,
                                SessionDataContainerType>::svc (void)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_ControlDecoder_T::svc"));

  // sanity check(s)
  ACE_ASSERT (inherited::state_);

  ACE_Message_Block* message_block_p = NULL;
  int result = -1;
  typename DataMessageType::DATA_T* message_data_container_p = NULL;
  typename DataMessageType::DATA_T::DATA_T* message_data_p = NULL;
  bool do_scan_end = false;
  bool done = false;

  do
  {
    message_block_p = NULL;
    result = inherited::getq (message_block_p,
                              NULL);
    if (result == -1)
    {
      int error = ACE_OS::last_error ();
      if (error != EWOULDBLOCK) // Win32: 10035
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: worker thread (id: %t) failed to ACE_Task::getq(): \"%m\", aborting\n"),
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

        // clean up
        message_block_p->release ();

        result = 0;
        done = true;

        break;
      }
      default:
      { ACE_ASSERT (!inherited::fragment_);
        inherited::fragment_ = dynamic_cast<DataMessageType*> (message_block_p);
        ACE_ASSERT (inherited::fragment_);
        message_block_p = NULL;

        if (inherited::fragment_->isInitialized ())
        {
          //// sanity check(s)
          //message_data_container_p =
          //  &const_cast<typename DataMessageType::DATA_T&> (buffer_->get ());
          //message_data_p =
          //  &const_cast<typename DataMessageType::DATA_T::DATA_T&> (message_data_container_p->get ());
          //ACE_ASSERT (message_data_p->messageType == ARDRONE_MESSAGE_CONTROL);

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
        message_data_p->messageType = ARDRONE_MESSAGE_CONTROL;
        message_data_p->controlData.clear ();
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
        inherited::fragment_->initialize (message_data_container_p,
                                          inherited::fragment_->sessionId (),
                                          NULL);
        message_data_container_p = NULL;
        inherited::fragment_->set (ARDRONE_MESSAGE_CONTROL);
        //buffer_->set_2 (inherited::stream_);

continue_:
        if (!begin (inherited::fragment_->rd_ptr (),
                    inherited::fragment_->length ()))
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("%s: failed to begin(), aborting\n"),
                      inherited::mod_->name ()));
          goto error;
        } // end IF
        do_scan_end = true;

        // initialize scanner ?
        if (inherited::isFirst_)
        {
          inherited::isFirst_ = false;

          /* column is only valid if an input buffer exists. */
          ARDrone_Control_Scanner_set_column (1, inherited::state_);
          ARDrone_Control_Scanner_set_lineno (1, inherited::state_);
        } // end IF

        // parse data fragment
        try {
          result = ARDrone_Control_Scanner_lex (inherited::state_);
        } catch (...) {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("%s: caught exception in yylex(), continuing\n"),
                      inherited::mod_->name ()));
          result = -1;
        }
        switch (result)
        {
          case -1:
          case 0:
          {
            // *NOTE*: most probable reason: connection
            //         has been closed --> session end
            //ACE_DEBUG ((LM_DEBUG,
            //            ACE_TEXT ("%s: failed to parse control message(s) (result was: %d), aborting\n"),
            //            inherited::mod_->name (),
            //            result));

            // clean up
            end ();

            done = true;
            result = 0;

            break;
          }
          default:
          {
            // clean up
            end ();
            do_scan_end = false;

            // more data ?
            if (inherited::fragment_)
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

        done = true;
      }
    } // end SWITCH
  } while (!done);

  return result;
}
