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

//#include "ATcodec/ATcodec_api.h"
#include "VLIB/video_codec.h"

#include "ardrone_macros.h"

// initialize statics
template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataContainerType,
          typename HandlerConfigurationType,
          typename ConnectionManagerType,
          typename ConnectorType>
ACE_Atomic_Op<ACE_SYNCH_MUTEX, unsigned long>
ARDrone_Module_Controller_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ConfigurationType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            SessionDataContainerType,
                            HandlerConfigurationType,
                            ConnectionManagerType,
                            ConnectorType>::currentID = 1;

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataContainerType,
          typename HandlerConfigurationType,
          typename ConnectionManagerType,
          typename ConnectorType>
ARDrone_Module_Controller_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ConfigurationType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            SessionDataContainerType,
                            HandlerConfigurationType,
                            ConnectionManagerType,
                            ConnectorType>::ARDrone_Module_Controller_T ()
 : inherited ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_Controller_T::ARDrone_Module_Controller_T"));

}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataContainerType,
          typename HandlerConfigurationType,
          typename ConnectionManagerType,
          typename ConnectorType>
ARDrone_Module_Controller_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ConfigurationType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            SessionDataContainerType,
                            HandlerConfigurationType,
                            ConnectionManagerType,
                            ConnectorType>::~ARDrone_Module_Controller_T ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_Controller_T::~ARDrone_Module_Controller_T"));

}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataContainerType,
          typename HandlerConfigurationType,
          typename ConnectionManagerType,
          typename ConnectorType>
bool
ARDrone_Module_Controller_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ConfigurationType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            SessionDataContainerType,
                            HandlerConfigurationType,
                            ConnectionManagerType,
                            ConnectorType>::initialize (const ConfigurationType& configuration_in,
                                                        Stream_IAllocator* allocator_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_Controller_T::initialize"));

  if (inherited::isInitialized_)
  {
  } // end IF

  return inherited::initialize (configuration_in,
                                allocator_in);
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataContainerType,
          typename HandlerConfigurationType,
          typename ConnectionManagerType,
          typename ConnectorType>
void
ARDrone_Module_Controller_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ConfigurationType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            SessionDataContainerType,
                            HandlerConfigurationType,
                            ConnectionManagerType,
                            ConnectorType>::handleDataMessage (DataMessageType*& message_inout,
                                                               bool& passMessageDownstream_out)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_Controller_T::handleDataMessage"));

  inherited::handleDataMessage (message_inout,
                                passMessageDownstream_out);
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataContainerType,
          typename HandlerConfigurationType,
          typename ConnectionManagerType,
          typename ConnectorType>
void
ARDrone_Module_Controller_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ConfigurationType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            SessionDataContainerType,
                            HandlerConfigurationType,
                            ConnectionManagerType,
                            ConnectorType>::handleSessionMessage (SessionMessageType*& message_inout,
                                                                  bool& passMessageDownstream_out)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_Controller_T::handleSessionMessage"));

  // don't care (implies yes per default, if part of a stream)
  ACE_UNUSED_ARG (passMessageDownstream_out);

  // sanity check(s)
  ACE_ASSERT (inherited::isInitialized_);

  // set up connection
  inherited::handleSessionMessage (message_inout,
                                   passMessageDownstream_out);

  // sanity check(s)
  if (!message_inout || !passMessageDownstream_out)
    return;

  switch (message_inout->type ())
  {
    case STREAM_SESSION_MESSAGE_CONNECT:
    {
      // *TODO*: this needs more work
      // *TODO*: remove type inference
      enum ARDrone_VideoMode video_mode = ARDRONE_VIDEOMODE_INVALID;
      switch (inherited::configuration_->sourceFormat.height)
      {
        case 368:
          video_mode = ARDRONE_VIDEOMODE_360P;
          break;
        case 720:
          video_mode = ARDRONE_VIDEOMODE_720P;
          break;
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("%s: invalid/unknown device video mode (resolution/height was: %d), aborting\n"),
                      inherited::mod_->name (),
                      inherited::configuration_->sourceFormat.height));
          goto error;
        }
      } // end SWITCH
      try {
        set (video_mode);
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to ARDrone_IController::set(%d), aborting\n"),
                    inherited::mod_->name (),
                    video_mode));
        goto error;
      }

      goto continue_;

error:
      this->notify (STREAM_SESSION_MESSAGE_ABORT);

      break;

continue_:
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
          typename SessionDataContainerType,
          typename HandlerConfigurationType,
          typename ConnectionManagerType,
          typename ConnectorType>
bool
ARDrone_Module_Controller_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ConfigurationType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            SessionDataContainerType,
                            HandlerConfigurationType,
                            ConnectionManagerType,
                            ConnectorType>::sendATCommand (const std::string& commandString_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_Controller_T::sendATCommand"));

  DataMessageType* message_p =
    inherited::allocateMessage (ARDRONE_PROTOCOL_AT_COMMAND_MAXIMUM_LENGTH);
  if (!message_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_Task_Base::allocateMessage(%u): \"%m\", aborting\n"),
                inherited::mod_->name (),
                ARDRONE_PROTOCOL_AT_COMMAND_MAXIMUM_LENGTH));
    return false;
  } // end IF
  int result = message_p->copy (commandString_in.c_str (),
                                commandString_in.size ());
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to ACE_Message_Block::copy(): \"%m\", aborting\n"),
                inherited::mod_->name ()));
    goto error;
  } // end IF

  result = inherited::put_next (message_p, NULL);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to ACE_Task::put_next(): \"%m\", aborting\n"),
                inherited::mod_->name ()));
    goto error;
  } // end IF

#if defined (_DEBUG)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("%s: sent AT command: \"%s\"...\n"),
              inherited::mod_->name (),
              ACE_TEXT (commandString_in.c_str ())));
#endif

  return true;

error:
  if (message_p)
    message_p->release ();

  return false;
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataContainerType,
          typename HandlerConfigurationType,
          typename ConnectionManagerType,
          typename ConnectorType>
void
ARDrone_Module_Controller_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ConfigurationType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            SessionDataContainerType,
                            HandlerConfigurationType,
                            ConnectionManagerType,
                            ConnectorType>::trim ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_Controller_T::trim"));

  std::ostringstream converter;

  std::string command_string =
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_PREFIX_STRING);
  command_string +=
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_COMMAND_FTRIM_STRING);
  command_string +=
    ACE_TEXT_ALWAYS_CHAR ("=");
  converter << OWN_TYPE_T::currentID.value ();
  OWN_TYPE_T::currentID++;
  command_string += converter.str ();
  command_string +=
    ACE_TEXT_ALWAYS_CHAR (",");
  command_string += '\r';

  if (!sendATCommand (command_string))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to ARDrone_Module_Controller_T::sendATCommand(\"%s\"), continuing\n"),
                inherited::mod_->name (),
                ACE_TEXT (command_string.c_str ())));
}
template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataContainerType,
          typename HandlerConfigurationType,
          typename ConnectionManagerType,
          typename ConnectorType>
void
ARDrone_Module_Controller_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ConfigurationType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            SessionDataContainerType,
                            HandlerConfigurationType,
                            ConnectionManagerType,
                            ConnectorType>::takeoff ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_Controller_T::takeoff"));

  ACE_ASSERT (false);
  ACE_NOTSUP;
  ACE_NOTREACHED (return;)
}
template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataContainerType,
          typename HandlerConfigurationType,
          typename ConnectionManagerType,
          typename ConnectorType>
void
ARDrone_Module_Controller_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ConfigurationType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            SessionDataContainerType,
                            HandlerConfigurationType,
                            ConnectionManagerType,
                            ConnectorType>::land ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_Controller_T::land"));

  ACE_ASSERT (false);
  ACE_NOTSUP;
  ACE_NOTREACHED (return;)
}
template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataContainerType,
          typename HandlerConfigurationType,
          typename ConnectionManagerType,
          typename ConnectorType>
void
ARDrone_Module_Controller_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ConfigurationType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            SessionDataContainerType,
                            HandlerConfigurationType,
                            ConnectionManagerType,
                            ConnectorType>::set (ARDrone_VideoMode videoMode_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_Controller_T::set"));

  std::ostringstream converter;

  std::string command_string =
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_PREFIX_STRING);
  command_string +=
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_COMMAND_CONFIG_STRING);
  command_string +=
    ACE_TEXT_ALWAYS_CHAR ("=");
  converter << (OWN_TYPE_T::currentID).value ();
  (OWN_TYPE_T::currentID)++;
  command_string += converter.str ();
  command_string +=
    ACE_TEXT_ALWAYS_CHAR (",\"");
  command_string +=
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_COMMAND_VIDEO_STRING);
  command_string +=
    ACE_TEXT_ALWAYS_CHAR (":");
  command_string +=
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_COMMAND_PARAMETER_CODEC_STRING);
  command_string +=
    ACE_TEXT_ALWAYS_CHAR ("\",\"");
  switch (videoMode_in)
  {
    case ARDRONE_VIDEOMODE_360P:
    {
      converter << H264_360P_CODEC;
      command_string += converter.str ();
      break;
    }
    case ARDRONE_VIDEOMODE_720P:
    {
      converter << H264_720P_CODEC;
      command_string += converter.str ();
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: invalid/unknown video mode (was: %d), returning\n"),
                  inherited::mod_->name (),
                  videoMode_in));
      return;
    }
  } // end SWITCH
  command_string +=
    ACE_TEXT_ALWAYS_CHAR ("\"\r");

  if (!sendATCommand (command_string))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to ARDrone_Module_Controller_T::sendATCommand(\"%s\"), continuing\n"),
                inherited::mod_->name (),
                ACE_TEXT (command_string.c_str ())));
}
template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataContainerType,
          typename HandlerConfigurationType,
          typename ConnectionManagerType,
          typename ConnectorType>
void
ARDrone_Module_Controller_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ConfigurationType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            SessionDataContainerType,
                            HandlerConfigurationType,
                            ConnectionManagerType,
                            ConnectorType>::messageCB ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_Controller_T::messageCB"));

  ACE_ASSERT (false);
  ACE_NOTSUP;
  ACE_NOTREACHED (return;)
}
