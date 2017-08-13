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
#include "Soft/Common/ardrone_api.h"
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
 , deviceState_ (0)
 , isFirst_ (true)
 , previousState_ (NAVDATA_STATE_INVALID)
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
    deviceState_ = 0;
    isFirst_ = true;
    previousState_ = NAVDATA_STATE_INVALID;
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
  deviceState_ = data_r.NavData.NavData.ardrone_state;

  if (isFirst_)
  {
    // *NOTE*: the device may still be acknowleging a control command from a
    //         previous session --> disable it first
    if (deviceState_ & ARDRONE_COMMAND_MASK)
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("%s: resetting control command ACK flag...\n"),
                  inherited::mod_->name ()));
      resetACKFlag ();
    } // end IF

    isFirst_ = false;

    try {
      start ();
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: caught exception in ARDrone_IController::start(), returning\n"),
                  inherited::mod_->name ()));
      return;
    }
  } // end IF

  // received acknowledgement ?
  if (deviceState_ & ARDRONE_COMMAND_MASK)
  { // *NOTE*: the command ACK flag needs to be reset manually
    if (inherited2::state_ != NAVDATA_STATE_COMMAND_ACK)
      change (NAVDATA_STATE_COMMAND_ACK);
  } // end IF
  else if (inherited2::state_ == NAVDATA_STATE_COMMAND_ACK)
  {
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("%s: received command acknowlegement\n"),
                inherited::mod_->name ()));

    change (NAVDATA_STATE_COMMAND_NACK);
  } // end ELSE IF

  // reset watchdog ?
  if (state_ & ARDRONE_COM_WATCHDOG_MASK)
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
      // reset message sequence number generator
      OWN_TYPE_T::currentID = 1;

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
    {
      ConnectionConfigurationIteratorType iterator;
      struct Net_UDPSocketConfiguration* socket_configuration_p = NULL;
      Common_IReset* ireset_p = NULL;

       if (!inherited::configuration_)
          goto continue_;

      // reset the connection configuration
      iterator =
        inherited::configuration_->connectionConfigurations->find (ACE_TEXT_ALWAYS_CHAR (inherited::mod_->name ()));
      if (iterator == inherited::configuration_->connectionConfigurations->end ())
        iterator =
          inherited::configuration_->connectionConfigurations->find (ACE_TEXT_ALWAYS_CHAR (""));
      ACE_ASSERT (iterator != inherited::configuration_->connectionConfigurations->end ());
      ACE_ASSERT ((*iterator).second.socketHandlerConfiguration.socketConfiguration);
      socket_configuration_p =
        dynamic_cast<struct Net_UDPSocketConfiguration*> ((*iterator).second.socketHandlerConfiguration.socketConfiguration);
      ACE_ASSERT (socket_configuration_p);
      socket_configuration_p->address.set_port_number (ARDRONE_PORT_UDP_NAVDATA,
                                                       1);
      socket_configuration_p->sourcePort = ARDRONE_PORT_UDP_NAVDATA;

      if (!inherited::connection_)
        goto continue_;

      ireset_p = dynamic_cast<Common_IReset*> (inherited::connection_);
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

continue_:
      change (NAVDATA_STATE_INVALID);

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
    case NAVDATA_STATE_CONFIG:
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
      socket_configuration_p->sourcePort = ARDRONE_PORT_UDP_CONTROL_CONFIGURATION;
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

      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("%s: requesting device configuration data\n"),
                  inherited::mod_->name ()));

      command_string =
        ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_PREFIX_STRING);
      command_string +=
        ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_COMMAND_CONTROL_STRING);
      command_string += ACE_TEXT_ALWAYS_CHAR ("=");
      converter << (OWN_TYPE_T::currentID).value ();
      (OWN_TYPE_T::currentID)++;
      command_string += converter.str ();
      command_string += ACE_TEXT_ALWAYS_CHAR (",");
      converter.str (ACE_TEXT_ALWAYS_CHAR (""));
      converter.clear ();
      converter << CFG_GET_CONTROL_MODE;
      command_string += converter.str ();
      command_string += ACE_TEXT_ALWAYS_CHAR (",");
      converter.str (ACE_TEXT_ALWAYS_CHAR (""));
      converter.clear ();
      converter << 0;
      command_string += converter.str ();
      command_string += ACE_TEXT_ALWAYS_CHAR ("\r");

      if (!sendATCommand (command_string))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to ARDrone_Module_Controller_T::sendATCommand(\"%s\"), continuing\n"),
                    inherited::mod_->name (),
                    ACE_TEXT (command_string.c_str ())));

      break;
    }
    case NAVDATA_STATE_MODE:
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("%s: switching navdata mode: %s\n"),
                  inherited::mod_->name (),
                  (ARDRONE_PROTOCOL_DEBUG_NAVDATA_OPTIONS ? ACE_TEXT_ALWAYS_CHAR ("full")
                                                          : ACE_TEXT_ALWAYS_CHAR ("demo"))));

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
          ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_COMMAND_CATEGORY_GENERAL_STRING);
      command_string += ACE_TEXT_ALWAYS_CHAR (":");
      command_string +=
        ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_COMMAND_SETTING_MODE_STRING);
      command_string += ACE_TEXT_ALWAYS_CHAR ("\",\"");
      command_string +=
        (ARDRONE_PROTOCOL_DEBUG_NAVDATA_OPTIONS ? ACE_TEXT_ALWAYS_CHAR ("FALSE")
                                                : ACE_TEXT_ALWAYS_CHAR ("TRUE"));
      command_string += ACE_TEXT_ALWAYS_CHAR ("\"\r");

      if (!sendATCommand (command_string))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to ARDrone_Module_Controller_T::sendATCommand(\"%s\"), continuing\n"),
                    inherited::mod_->name (),
                    ACE_TEXT (command_string.c_str ())));

      break;
    }
    case NAVDATA_STATE_OPTIONS:
    {
      uint32_t navdata_options_u =
        (NAVDATA_OPTION_MASK (NAVDATA_DEMO_TAG)            |
         NAVDATA_OPTION_MASK (NAVDATA_TIME_TAG)            |
         NAVDATA_OPTION_MASK (NAVDATA_RAW_MEASURES_TAG)    |
         NAVDATA_OPTION_MASK (NAVDATA_PHYS_MEASURES_TAG)   |
         NAVDATA_OPTION_MASK (NAVDATA_GYROS_OFFSETS_TAG)   |
         NAVDATA_OPTION_MASK (NAVDATA_EULER_ANGLES_TAG)    |
         NAVDATA_OPTION_MASK (NAVDATA_REFERENCES_TAG)      |
         NAVDATA_OPTION_MASK (NAVDATA_TRIMS_TAG)           |
         NAVDATA_OPTION_MASK (NAVDATA_RC_REFERENCES_TAG)   |
         NAVDATA_OPTION_MASK (NAVDATA_PWM_TAG)             |
         NAVDATA_OPTION_MASK (NAVDATA_ALTITUDE_TAG)        |
         NAVDATA_OPTION_MASK (NAVDATA_VISION_RAW_TAG)      |
         NAVDATA_OPTION_MASK (NAVDATA_VISION_OF_TAG)       |
         NAVDATA_OPTION_MASK (NAVDATA_VISION_TAG)          |
         NAVDATA_OPTION_MASK (NAVDATA_VISION_PERF_TAG)     |
         NAVDATA_OPTION_MASK (NAVDATA_TRACKERS_SEND_TAG)   |
         NAVDATA_OPTION_MASK (NAVDATA_VISION_DETECT_TAG)   |
         NAVDATA_OPTION_MASK (NAVDATA_WATCHDOG_TAG)        |
         NAVDATA_OPTION_MASK (NAVDATA_ADC_DATA_FRAME_TAG)  |
         NAVDATA_OPTION_MASK (NAVDATA_VIDEO_STREAM_TAG)    |
         NAVDATA_OPTION_MASK (NAVDATA_GAMES_TAG)           |
         NAVDATA_OPTION_MASK (NAVDATA_PRESSURE_RAW_TAG)    |
         NAVDATA_OPTION_MASK (NAVDATA_MAGNETO_TAG)         |
         NAVDATA_OPTION_MASK (NAVDATA_WIND_TAG)            |
         NAVDATA_OPTION_MASK (NAVDATA_KALMAN_PRESSURE_TAG) |
         NAVDATA_OPTION_MASK (NAVDATA_HDVIDEO_STREAM_TAG)  |
         NAVDATA_OPTION_MASK (NAVDATA_WIFI_TAG));

      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("%s: requesting navdata options: 0x%x\n"),
                  inherited::mod_->name (),
                  navdata_options_u));

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
          ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_COMMAND_CATEGORY_GENERAL_STRING);
      command_string += ACE_TEXT_ALWAYS_CHAR (":");
      command_string +=
        ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_COMMAND_SETTING_OPTIONS_STRING);
      command_string += ACE_TEXT_ALWAYS_CHAR ("\",\"");
      converter.str (ACE_TEXT_ALWAYS_CHAR (""));
      converter.clear ();
      converter << navdata_options_u;
      command_string += converter.str ();
      command_string += ACE_TEXT_ALWAYS_CHAR ("\"\r");

      if (!sendATCommand (command_string))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to ARDrone_Module_Controller_T::sendATCommand(\"%s\"), continuing\n"),
                    inherited::mod_->name (),
                    ACE_TEXT (command_string.c_str ())));

      break;
    }
    case NAVDATA_STATE_READY:
    {
      if (!videoModeSet_)
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("%s: navdata initialized, setting video mode...\n"),
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
    case NAVDATA_STATE_COMMAND_ACK:
    {
      // step0: remember previous state
      previousState_ = inherited2::state_;

      // step1: confirm command ACK receipt
      resetACKFlag ();

      break;
    }
    case NAVDATA_STATE_COMMAND_NACK:
    {
      // switch to next state
      enum ARDRone_NavDataState next_state_e = previousState_;
      switch (previousState_)
      {
        case NAVDATA_STATE_CONFIG:
        {
          // *NOTE*: the device may (still) be in 'bootstrap' mode
          if (deviceState_ & ARDRONE_NAVDATA_BOOTSTRAP)
          {
            ACE_DEBUG ((LM_DEBUG,
                        ACE_TEXT ("%s: switching to navdata mode\n"),
                        inherited::mod_->name ()));
            next_state_e = NAVDATA_STATE_MODE;
          } // end IF
          else
            next_state_e = NAVDATA_STATE_OPTIONS;
          break;
        }
        case NAVDATA_STATE_MODE:
          next_state_e = NAVDATA_STATE_OPTIONS; break;
        case NAVDATA_STATE_OPTIONS:
        case NAVDATA_STATE_READY:
          next_state_e = NAVDATA_STATE_READY; break;
        default:
        {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("%s: unknown/invalid previous state (was: \"%s\"), continuing\n"),
                      inherited::mod_->name (),
                      ACE_TEXT (inherited2::stateToString (previousState_).c_str ())));
          ACE_ASSERT (false);
          break;
        }
      } // end SWITCH

      { ACE_GUARD (ACE_NULL_SYNCH::MUTEX, aGuard, *(inherited2::stateLock_));
        inherited2::state_ = NAVDATA_STATE_COMMAND_NACK;
      } // end lock scope
      change (next_state_e);

      ACE_ASSERT (inherited2::condition_);
      int result = inherited2::condition_->broadcast ();
      if (result == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_SYNCH_CONDITION_T::broadcast(): \"%m\", continuing\n")));

      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: invalid/unknown state (was: \"%s\"), returning\n"),
                  inherited::mod_->name (),
                  ACE_TEXT (inherited2::stateToString (newState_in).c_str ())));
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
                            ConnectorType>::resetACKFlag ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_Controller_T::resetACKFlag"));

  std::ostringstream converter;

  std::string command_string =
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_PREFIX_STRING);
  command_string +=
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_COMMAND_CONTROL_STRING);
  command_string += ACE_TEXT_ALWAYS_CHAR ("=");
  converter << (OWN_TYPE_T::currentID).value ();
  (OWN_TYPE_T::currentID)++;
  command_string += converter.str ();
  command_string += ACE_TEXT_ALWAYS_CHAR (",");
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << ACK_CONTROL_MODE;
  command_string += converter.str ();
  command_string += ACE_TEXT_ALWAYS_CHAR (",");
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << 0;
  command_string += converter.str ();
  command_string += ACE_TEXT_ALWAYS_CHAR ("\r");

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
      ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_COMMAND_CATEGORY_VIDEO_STRING);
  command_string += ACE_TEXT_ALWAYS_CHAR (":");
  command_string +=
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_COMMAND_SETTING_CODEC_STRING);
  command_string += ACE_TEXT_ALWAYS_CHAR ("\",\"");
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
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
}
