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

//#include "ATcodec/ATcodec_api.h"
#include "VLIB/video_codec.h"

#include "ardrone_callbacks.h"
#include "ardrone_macros.h"

// initialize statics
template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataContainerType,
          typename WLANMonitorType,
          typename ConnectionConfigurationIteratorType,
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
                            WLANMonitorType,
                            ConnectionConfigurationIteratorType,
                            ConnectionManagerType,
                            ConnectorType>::currentID = 1;

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataContainerType,
          typename WLANMonitorType,
          typename ConnectionConfigurationIteratorType,
          typename ConnectionManagerType,
          typename ConnectorType>
ARDrone_Module_Controller_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ConfigurationType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            SessionDataContainerType,
                            WLANMonitorType,
                            ConnectionConfigurationIteratorType,
                            ConnectionManagerType,
#if defined (ACE_WIN32) || defined (ACE_WIN64)
                            ConnectorType>::ARDrone_Module_Controller_T (ISTREAM_T* stream_in)
#else
                            ConnectorType>::ARDrone_Module_Controller_T (typename inherited::ISTREAM_T* stream_in)
#endif
 : inherited (stream_in)
 , inherited2 ()
 , isFirst_ (true)
 , videoModeSet_ (false)
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
          typename WLANMonitorType,
          typename ConnectionConfigurationIteratorType,
          typename ConnectionManagerType,
          typename ConnectorType>
ARDrone_Module_Controller_T<ACE_SYNCH_USE,
                            TimePolicyType,
                            ConfigurationType,
                            ControlMessageType,
                            DataMessageType,
                            SessionMessageType,
                            SessionDataContainerType,
                            WLANMonitorType,
                            ConnectionConfigurationIteratorType,
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
          typename WLANMonitorType,
          typename ConnectionConfigurationIteratorType,
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
                            WLANMonitorType,
                            ConnectionConfigurationIteratorType,
                            ConnectionManagerType,
                            ConnectorType>::initialize (const ConfigurationType& configuration_in,
                                                        Stream_IAllocator* allocator_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_Controller_T::initialize"));

  if (inherited::isInitialized_)
  {
    isFirst_ = true;
    videoModeSet_ = false;
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
          typename WLANMonitorType,
          typename ConnectionConfigurationIteratorType,
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
                            WLANMonitorType,
                            ConnectionConfigurationIteratorType,
                            ConnectionManagerType,
                            ConnectorType>::handleDataMessage (DataMessageType*& message_inout,
                                                               bool& passMessageDownstream_out)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_Controller_T::handleDataMessage"));

  ACE_UNUSED_ARG (message_inout);
  ACE_UNUSED_ARG (passMessageDownstream_out);

  // sanity check(s)
  // *NOTE*: (depending on previous sessions) navdata may start arriving as soon
  //         as the stream connects to the navdata port. Discard all 'early'
  //         data until the module reaches a defined state (see below)
  if (inherited2::state_ < NAVDATA_STATE_INITIAL)
    return;

  const typename DataMessageType::DATA_T& data_container_r =
    message_inout->get ();
  const typename DataMessageType::DATA_T::DATA_T& data_r =
    data_container_r.get ();
  ACE_ASSERT (data_r.messageType == ARDRONE_MESSAGE_NAVDATAMESSAGE);

  if (isFirst_)
  {
    isFirst_ = false;

    // *NOTE*: depending on previous communications, the device may/may not be
    //         in bootstrap mode
    if (data_r.NavData.NavData.ardrone_state & ARDRONE_NAVDATA_BOOTSTRAP)
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("%s: drone is currently in bootstrap mode; requesting NavData demo mode\n"),
                  inherited::mod_->name ()));
    else
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("%s: requesting NavData demo mode\n"),
                  inherited::mod_->name ()));
    try {
      start ();
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: caught exception in ARDrone_IController::start(), returning\n"),
                  inherited::mod_->name ()));
      return;
    }
  } // end IF

  switch (inherited2::state_)
  {
    case NAVDATA_STATE_BOOTSTRAP:
    {
      if (data_r.NavData.NavData.ardrone_state & ARDRONE_COMMAND_MASK)
        inherited2::change (NAVDATA_STATE_COMMAND_ACK);
      break;
    }
    case NAVDATA_STATE_SET_PARAMETER:
    {
      if (data_r.NavData.NavData.ardrone_state & ARDRONE_COMMAND_MASK)
        inherited2::change (NAVDATA_STATE_READY);
      break;
    }
    default:
      break;
  } // end SWITCH

  // reset watchdog ?
  if (data_r.NavData.NavData.ardrone_state & ARDRONE_COM_WATCHDOG_MASK)
  {
    try {
      resetWatchdog ();
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: caught exception in ARDrone_IController::resetWatchdog(), returning\n"),
                  inherited::mod_->name ()));
      return;
    }
  } // end IF
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataContainerType,
          typename WLANMonitorType,
          typename ConnectionConfigurationIteratorType,
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
                            WLANMonitorType,
                            ConnectionConfigurationIteratorType,
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
    case STREAM_SESSION_MESSAGE_BEGIN:
    {
      try {
        init ();
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: caught exception in ARDrone_IController::init(), returning\n"),
                    inherited::mod_->name ()));
        return;
      }

      break;
    }
    case STREAM_SESSION_MESSAGE_END:
      break;
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
          typename WLANMonitorType,
          typename ConnectionConfigurationIteratorType,
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
                            WLANMonitorType,
                            ConnectionConfigurationIteratorType,
                            ConnectionManagerType,
                            ConnectorType>::onChange (enum ARDRone_NavDataState newState_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_Controller_T::onChange"));

  int result = -1;
  std::ostringstream converter;
  std::string command_string;

  switch (newState_in)
  {
    case NAVDATA_STATE_INITIAL:
    {
      DataMessageType* message_p =
        inherited::allocateMessage (ARDRONE_PROTOCOL_AT_COMMAND_MAXIMUM_LENGTH);
      if (!message_p)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to Stream_TaskBase_T::allocateMessage(%u): \"%m\", returning\n"),
                    inherited::mod_->name (),
                    ARDRONE_PROTOCOL_AT_COMMAND_MAXIMUM_LENGTH));
        return;
      } // end IF
      message_p->set (ARDRONE_MESSAGE_ATCOMMANDMESSAGE);
      unsigned char buffer_a[] = { 0x01, 0x00, 0x00, 0x00 };
      result = message_p->copy (reinterpret_cast<char*> (buffer_a),
                                sizeof (buffer_a));
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

      return;

error:
      if (message_p)
        message_p->release ();

      break;
    }
    case NAVDATA_STATE_BOOTSTRAP:
    {
      // sanity check(s)
      ACE_ASSERT (inherited::configuration_);
      ACE_ASSERT (inherited::connection_);

      // step1: the UDP control port is 5556 (not 5554)
      //        --> update the connection configuration

      ConnectionConfigurationIteratorType iterator =
        inherited::configuration_->connectionConfigurations->find (ACE_TEXT_ALWAYS_CHAR (inherited::mod_->name ()));
      if (iterator == inherited::configuration_->connectionConfigurations->end ())
        iterator =
          inherited::configuration_->connectionConfigurations->find (ACE_TEXT_ALWAYS_CHAR (""));
      ACE_ASSERT (iterator != inherited::configuration_->connectionConfigurations->end ());
      ACE_ASSERT ((*iterator).second.socketHandlerConfiguration.socketConfiguration);
      struct Net_UDPSocketConfiguration* socket_configuration_p =
        dynamic_cast<struct Net_UDPSocketConfiguration*> ((*iterator).second.socketHandlerConfiguration.socketConfiguration);
      ACE_ASSERT (socket_configuration_p);
      socket_configuration_p->address.set_port_number (ARDRONE_PORT_UDP_CONTROL_CONFIGURATION,
                                                       1);
      Common_IReset* ireset_p =
        dynamic_cast<Common_IReset*> (inherited::connection_);
      ACE_ASSERT (ireset_p);
      try {
        ireset_p->reset ();
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: caught exception in Common_IReset::reset(), continuing\n"),
                    inherited::mod_->name ()));
      }
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("%s: reset target address to %s\n"),
                  inherited::mod_->name (),
                  ACE_TEXT (Net_Common_Tools::IPAddressToString (socket_configuration_p->address).c_str ())));

      command_string =
        ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_PREFIX_STRING);
      command_string +=
        ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_COMMAND_CONFIG_STRING);
      command_string += ACE_TEXT_ALWAYS_CHAR ("=");
      converter << (OWN_TYPE_T::currentID).value ();
      (OWN_TYPE_T::currentID)++;
      command_string += converter.str ();
      command_string += ACE_TEXT_ALWAYS_CHAR (",\"");
      command_string +=
          ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_COMMAND_GENERAL_STRING);
      command_string += ACE_TEXT_ALWAYS_CHAR (":");
      command_string +=
        ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_COMMAND_PARAMETER_DEMO_STRING);
      command_string += ACE_TEXT_ALWAYS_CHAR ("\",\"");
      command_string += ACE_TEXT_ALWAYS_CHAR ("TRUE");
      command_string += ACE_TEXT_ALWAYS_CHAR ("\"\r");

      if (!sendATCommand (command_string))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to ARDrone_Module_Controller_T::sendATCommand(\"%s\"), continuing\n"),
                    inherited::mod_->name (),
                    ACE_TEXT (command_string.c_str ())));

      break;
    }
    case NAVDATA_STATE_COMMAND_ACK:
    {
      command_string =
        ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_PREFIX_STRING);
      command_string +=
        ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_COMMAND_CONTROL_STRING);
      command_string += ACE_TEXT_ALWAYS_CHAR ("=");
      converter.str (ACE_TEXT_ALWAYS_CHAR (""));
      converter.clear ();
      converter << 0;
      command_string += converter.str ();
      command_string += ACE_TEXT_ALWAYS_CHAR ("\"\r");

      if (!sendATCommand (command_string))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to ARDrone_Module_Controller_T::sendATCommand(\"%s\"), continuing\n"),
                    inherited::mod_->name (),
                    ACE_TEXT (command_string.c_str ())));

      change (NAVDATA_STATE_READY);

      break;
    }
    case NAVDATA_STATE_READY:
    {
      if (!videoModeSet_)
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("%s: NavData initialized, setting video mode...\n"),
                    inherited::mod_->name ()));

        try {
          set (ARDRONE_VIDEOMODE_720P);
        } catch (...) {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("%s: caught exception in ARDrone_IController::set(%d), returning\n"),
                      inherited::mod_->name (),
                      ARDRONE_VIDEOMODE_720P));
          return;
        }
        videoModeSet_ = true;
      } // end IF

      break;
    }
    case NAVDATA_STATE_SET_PARAMETER:
      break;
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: invalid/unknown state (was: \"%s\"), returning\n"),
                  inherited::mod_->name (),
                  ACE_TEXT (inherited2::state2String (newState_in).c_str ())));
      break;
    }
  } // end SWITCH
}

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          typename ConfigurationType,
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          typename SessionDataContainerType,
          typename WLANMonitorType,
          typename ConnectionConfigurationIteratorType,
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
                            WLANMonitorType,
                            ConnectionConfigurationIteratorType,
                            ConnectionManagerType,
                            ConnectorType>::wait (const ACE_Time_Value* timeout_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_Controller_T::wait"));

  int result = -1;
  int error = 0;

  ACE_GUARD_RETURN (ACE_SYNCH_NULL_MUTEX, aGuard, *inherited2::stateLock_, false);

  result = inherited2::condition_->wait (timeout_in);
  if (result == -1)
  {
    error = ACE_OS::last_error ();
    if (error != ETIME)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to ACE_SYNCH_CONDITION_T::wait(): \"%m\", continuing\n"),
                  inherited::mod_->name ()));
  } // end IF
  if ((error == ETIME) ||
      (inherited2::state_ != NAVDATA_STATE_READY))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to initialize NavData, aborting\n"),
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
          typename SessionDataContainerType,
          typename WLANMonitorType,
          typename ConnectionConfigurationIteratorType,
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
                            WLANMonitorType,
                            ConnectionConfigurationIteratorType,
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
  message_p->set (ARDRONE_MESSAGE_ATCOMMANDMESSAGE);
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

//#if defined (_DEBUG)
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("%s: sent AT command: \"%s\"\n"),
//              inherited::mod_->name (),
//              ACE_TEXT (commandString_in.c_str ())));
//#endif

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
          typename WLANMonitorType,
          typename ConnectionConfigurationIteratorType,
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
                            WLANMonitorType,
                            ConnectionConfigurationIteratorType,
                            ConnectionManagerType,
                            ConnectorType>::ids (uint8_t sessionID_in,
                                                 uint8_t userID_in,
                                                 uint8_t applicationID_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_Controller_T::ids"));

  std::ostringstream converter;

  std::string command_string =
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_PREFIX_STRING);
  command_string +=
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_COMMAND_CONFIG_IDS_STRING);
  command_string +=
    ACE_TEXT_ALWAYS_CHAR ("=");
  converter << OWN_TYPE_T::currentID.value ();
  OWN_TYPE_T::currentID++;
  command_string += converter.str ();
  command_string +=
    ACE_TEXT_ALWAYS_CHAR (",");

  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << static_cast<int> (sessionID_in);
  command_string += converter.str ();
  command_string +=
    ACE_TEXT_ALWAYS_CHAR (",");

  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << static_cast<int> (userID_in);
  command_string += converter.str ();
  command_string +=
    ACE_TEXT_ALWAYS_CHAR (",");

  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << static_cast<int> (applicationID_in);
  command_string += converter.str ();

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
          typename WLANMonitorType,
          typename ConnectionConfigurationIteratorType,
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
                            WLANMonitorType,
                            ConnectionConfigurationIteratorType,
                            ConnectionManagerType,
                            ConnectorType>::resetWatchdog ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_Controller_T::resetWatchdog"));

  std::string command_string =
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_PREFIX_STRING);
  command_string +=
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_COMMAND_RESET_WATCHDOG_STRING);
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
          typename WLANMonitorType,
          typename ConnectionConfigurationIteratorType,
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
                            WLANMonitorType,
                            ConnectionConfigurationIteratorType,
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
          typename WLANMonitorType,
          typename ConnectionConfigurationIteratorType,
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
                            WLANMonitorType,
                            ConnectionConfigurationIteratorType,
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
          typename WLANMonitorType,
          typename ConnectionConfigurationIteratorType,
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
                            WLANMonitorType,
                            ConnectionConfigurationIteratorType,
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
          typename WLANMonitorType,
          typename ConnectionConfigurationIteratorType,
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
                            WLANMonitorType,
                            ConnectionConfigurationIteratorType,
                            ConnectionManagerType,
                            ConnectorType>::set (enum ARDrone_VideoMode videoMode_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_Controller_T::set"));

//  ids (ARDRONE_PROTOCOL_DEFAULT_SESSION_ID,
//       ARDRONE_PROTOCOL_DEFAULT_USER_ID,
//       ARDRONE_PROTOCOL_DEFAULT_APPLICATION_ID);

  std::ostringstream converter;

  std::string command_string =
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_PREFIX_STRING);
  command_string +=
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_COMMAND_CONFIG_STRING);
  command_string += ACE_TEXT_ALWAYS_CHAR ("=");
  converter << (OWN_TYPE_T::currentID).value ();
  (OWN_TYPE_T::currentID)++;
  command_string += converter.str ();
  command_string += ACE_TEXT_ALWAYS_CHAR (",\"");
  command_string +=
      ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_COMMAND_VIDEO_STRING);
  command_string += ACE_TEXT_ALWAYS_CHAR (":");
  command_string +=
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_COMMAND_PARAMETER_CODEC_STRING);
  command_string += ACE_TEXT_ALWAYS_CHAR ("\",\"");
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
  command_string += ACE_TEXT_ALWAYS_CHAR ("\"\r");

  if (!sendATCommand (command_string))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to ARDrone_Module_Controller_T::sendATCommand(\"%s\"), continuing\n"),
                inherited::mod_->name (),
                ACE_TEXT (command_string.c_str ())));

  change (NAVDATA_STATE_SET_PARAMETER);
}
