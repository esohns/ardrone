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
#include "ardrone_configuration.h"
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
                            ConnectorType>::currentNavDataMessageId = 1;

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
 , deviceConfiguration_ ()
 , deviceInitialized_ (false)
 , deviceState_ (0)
 , deviceState_2 ()
 , GtkCBData_ (NULL)
 , isFirst_ (true)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_Controller_T::ARDrone_Module_Controller_T"));

  ACE_OS::memset (&deviceState_2, 0, sizeof (struct _navdata_demo_t));
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
    deviceConfiguration_.clear ();
    deviceInitialized_ = false;
    deviceState_ = 0;
    ACE_OS::memset (&deviceState_2, 0, sizeof (struct _navdata_demo_t));
    GtkCBData_ = NULL;
    isFirst_ = true;
  } // end IF

  GtkCBData_ = configuration_in.CBData;

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
  // *NOTE*: (depending on previous 'sessions') navdata may start arriving as
  //         soon as upstream connects to the (previously subscribed) port
  //         --> discard all 'early' data until the module reaches a defined
  //             state (see below)
  if (inherited2::state_ < NAVDATA_STATE_INITIAL)
    return;

  // update state
  const typename DataMessageType::DATA_T& data_container_r =
    message_inout->getR ();
  const typename DataMessageType::DATA_T::DATA_T& data_r =
    data_container_r.getR ();
  ACE_ASSERT (data_r.messageType == ARDRONE_MESSAGE_NAVDATA);
  deviceState_ = data_r.NavData.NavData.ardrone_state;
  struct _navdata_option_t* option_p = NULL;
  for (ARDrone_NavDataOptionOffsetsIterator_t iterator = data_r.NavData.NavDataOptionOffsets.begin ();
       iterator != data_r.NavData.NavDataOptionOffsets.end ();
       ++iterator)
  {
    option_p =
      reinterpret_cast<struct _navdata_option_t*> (message_inout->rd_ptr () + *iterator);
    if (option_p->tag == NAVDATA_DEMO_TAG)
    {
      deviceState_2 =
        *reinterpret_cast<struct _navdata_demo_t*> (option_p);
      break;
    } // end IF
  } // end FOR

  if (isFirst_)
  {
    // *NOTE*: the device may still be acknowleging a control command from a
    //         previous session --> disable it first
    if (deviceState_ & ARDRONE_COMMAND_MASK)
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("%s: resetting control command ACK flag\n"),
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

  // received acknowlegement ?
  if (deviceState_ & ARDRONE_COMMAND_MASK)
  { 
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("%s: received command acknowlegement\n"),
                inherited::mod_->name ()));

    change (NAVDATA_STATE_COMMAND_ACK);

    // *NOTE*: the command ACK flag needs to be reset manually
    resetACKFlag ();
  } // end IF

  // reset watchdog ?
  if (deviceState_ & ARDRONE_COM_WATCHDOG_MASK)
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

  enum Stream_SessionMessageType message_type_e =
    message_inout->type ();

  // set up connection
  inherited::handleSessionMessage (message_inout,
                                   passMessageDownstream_out);

  switch (message_type_e)
  {
    case STREAM_SESSION_MESSAGE_BEGIN:
    {
      // reset message sequence number generator
      OWN_TYPE_T::currentNavDataMessageId = 1;

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
      //// sanity check(s)
      //ACE_ASSERT (inherited::sessionData_);

      //const typename SessionMessageType::DATA_T::DATA_T& session_data_r =
      //  inherited::sessionData_->getR ();
      ConnectionConfigurationIteratorType iterator, iterator_2;
      struct Net_UDPSocketConfiguration* socket_configuration_p, *socket_configuration_2 =
          NULL;
      ACE_INET_Addr local_SAP, remote_SAP, gateway_address;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      struct _GUID interface_identifier;
#else
      std::string interface_identifier;
#endif
      ACE_Message_Block* message_block_p = NULL;
      unsigned char buffer_a[] = {0x01, 0x00, 0x00, 0x00};

      // retrieve listen address (connection has been set up upstream) and peer
      // address to set up the local SAP
      // sanity check(s)
      ACE_ASSERT (inherited::configuration_);
      ACE_ASSERT (inherited::configuration_->connectionConfigurations);
      iterator =
        inherited::configuration_->connectionConfigurations->find (ACE_TEXT_ALWAYS_CHAR ("NavDataSource"));
      // sanity check(s)
      ACE_ASSERT (iterator != inherited::configuration_->connectionConfigurations->end ());
      ACE_ASSERT ((*iterator).second.socketHandlerConfiguration.socketConfiguration);
      socket_configuration_p =
        dynamic_cast<struct Net_UDPSocketConfiguration*> ((*iterator).second.socketHandlerConfiguration.socketConfiguration);
      ACE_ASSERT (socket_configuration_p);
      iterator_2 =
        inherited::configuration_->connectionConfigurations->find (ACE_TEXT_ALWAYS_CHAR (inherited::mod_->name ()));
      // sanity check(s)
      ACE_ASSERT (iterator_2 != inherited::configuration_->connectionConfigurations->end ());
      ACE_ASSERT ((*iterator_2).second.socketHandlerConfiguration.socketConfiguration);
      socket_configuration_2 =
        dynamic_cast<struct Net_UDPSocketConfiguration*> ((*iterator_2).second.socketHandlerConfiguration.socketConfiguration);
      ACE_ASSERT (socket_configuration_2);
      if (unlikely (!Net_Common_Tools::IPAddressToInterface (socket_configuration_2->peerAddress,
                                                             interface_identifier)))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Net_Common_Tools::IPAddressToInterface(%s), aborting\n"),
                    ACE_TEXT (Net_Common_Tools::IPAddressToString (socket_configuration_2->peerAddress).c_str ())));
        goto error;
      } // end IF
#if defined (ACE_WIN32) || defined (ACE_WIN64)
      if (unlikely (!Net_Common_Tools::interfaceToIPAddress (interface_identifier,
                                                             local_SAP,
                                                             gateway_address)))
#else
      if (unlikely (!Net_Common_Tools::interfaceToIPAddress (interface_identifier,
                                                             NULL,
                                                             local_SAP,
                                                             gateway_address)))
#endif
      {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Net_Common_Tools::interfaceToIPAddress(%s): \"%m\", aborting\n"),
                    ACE_TEXT (Net_Common_Tools::interfaceToString (interface_identifier).c_str ())));
#else
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to Net_Common_Tools::interfaceToIPAddress(%s): \"%m\", aborting\n"),
                    ACE_TEXT (interface_identifier.c_str ())));
#endif
        goto error;
      } // end IF
      local_SAP.set_port_number (socket_configuration_p->listenAddress.get_port_number (),
                                 1);
      remote_SAP = socket_configuration_2->peerAddress;
      remote_SAP.set_port_number (ARDRONE_PORT_UDP_NAVDATA, 1);

      // 'subscribe' to the NavData stream
      message_block_p = inherited::allocateMessage (sizeof (buffer_a));
      if (unlikely (!message_block_p))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to Stream_TaskBase_T::allocateMessage(%u): \"%m\", returning\n"),
                    inherited::mod_->name (),
                    sizeof (buffer_a)));
        goto error;
      } // end IF
      //message_p->initialize (session_data_r.sessionId,
      //                       NULL);
      //message_p->set (ARDRONE_MESSAGE_ATCOMMAND);
      //message_p->set_2 (inherited::stream_);
      result = message_block_p->copy (reinterpret_cast<char*> (buffer_a),
                                      sizeof (buffer_a));
      if (unlikely (result == -1))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to ACE_Message_Block::copy(): \"%m\", aborting\n"),
                    inherited::mod_->name ()));
        goto error;
      } // end IF
      if (unlikely (!Net_Common_Tools::sendDatagram (local_SAP,
                                                     remote_SAP,
                                                     message_block_p)))
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to Net_Common_Tools::sendDatagram(%s,%s,%u), aborting\n"),
                    inherited::mod_->name (),
                    ACE_TEXT (Net_Common_Tools::IPAddressToString (local_SAP).c_str ()),
                    ACE_TEXT (Net_Common_Tools::IPAddressToString (remote_SAP).c_str ()),
                    sizeof (buffer_a)));
        goto error;
      } // end IF

error:
      if (message_block_p)
        message_block_p->release ();

      break;
    }
    case NAVDATA_STATE_GET_CONFIGURATION:
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("%s: requesting device configuration data...\n"),
                  inherited::mod_->name ()));

      dump ();

      break;
    }
    case NAVDATA_STATE_SWITCH_MODE:
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("%s: switching navdata mode: %s\n"),
                  inherited::mod_->name (),
                  (ARDRONE_PROTOCOL_FULL_NAVDATA_OPTIONS ? ACE_TEXT_ALWAYS_CHAR ("full")
                                                         : ACE_TEXT_ALWAYS_CHAR ("reduced"))));

      command_string =
        ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_PREFIX_STRING);
      command_string +=
        ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_COMMAND_CONFIG_STRING);
      command_string += ACE_TEXT_ALWAYS_CHAR ("=");
      converter << OWN_TYPE_T::currentNavDataMessageId.value ();
      OWN_TYPE_T::currentNavDataMessageId++;
      command_string += converter.str ();
      command_string += ACE_TEXT_ALWAYS_CHAR (",\"");
      command_string +=
          ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_COMMAND_CATEGORY_GENERAL_STRING);
      command_string += ACE_TEXT_ALWAYS_CHAR (":");
      command_string +=
        ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_COMMAND_SETTING_MODE_STRING);
      command_string += ACE_TEXT_ALWAYS_CHAR ("\",\"");
      command_string +=
        (ARDRONE_PROTOCOL_FULL_NAVDATA_OPTIONS ? ACE_TEXT_ALWAYS_CHAR ("FALSE")
                                               : ACE_TEXT_ALWAYS_CHAR ("TRUE"));
      command_string += ACE_TEXT_ALWAYS_CHAR ("\"\r");

      if (!sendATCommand (command_string))
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to ARDrone_Module_Controller_T::sendATCommand(\"%s\"), continuing\n"),
                    inherited::mod_->name (),
                    ACE_TEXT (command_string.c_str ())));

      break;
    }
    case NAVDATA_STATE_NAVDATA_OPTIONS:
    {
      ACE_UINT32 navdata_options_u =
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
         NAVDATA_OPTION_MASK (NAVDATA_WIFI_TAG)            |
         NAVDATA_OPTION_MASK (NAVDATA_ZIMMU_3000_TAG));

      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("%s: requesting navdata option(s) (mask: 0x%x)...\n"),
                  inherited::mod_->name (),
                  navdata_options_u));

      command_string =
        ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_PREFIX_STRING);
      command_string +=
        ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_COMMAND_CONFIG_STRING);
      command_string += ACE_TEXT_ALWAYS_CHAR ("=");
      converter << OWN_TYPE_T::currentNavDataMessageId.value ();
      OWN_TYPE_T::currentNavDataMessageId++;
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
    case NAVDATA_STATE_SET_VIDEO:
    { ACE_ASSERT (!deviceConfiguration_.empty ());
      ARDrone_DeviceConfigurationConstIterator_t iterator =
        deviceConfiguration_.find (ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_COMMAND_CATEGORY_VIDEO_STRING));
      ACE_ASSERT (iterator != deviceConfiguration_.end ());
      ARDrone_DeviceConfigurationCategoryIterator_t iterator_2 =
        (*iterator).second.begin ();
      for (;
           iterator_2 != (*iterator).second.end ();
           ++iterator_2)
        if (!ACE_OS::strcmp ((*iterator_2).first.c_str (),
                             ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_COMMAND_SETTING_CODEC_STRING)))
          break;
      ACE_ASSERT (iterator_2 != (*iterator).second.end ());
      std::istringstream converter_2;
      int value_i = 0;
      converter_2.str ((*iterator_2).second);
      converter_2 >> value_i;
      enum _codec_type_t codec_e = static_cast<enum _codec_type_t> (value_i);

      // sanity check(s)
      ACE_ASSERT (GtkCBData_);

      if (codec_e != GtkCBData_->videoMode)
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("%s: navdata initialized, setting video mode...\n"),
                    inherited::mod_->name ()));

        try {
          set (GtkCBData_->videoMode);
        } catch (...) {
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("%s: caught exception in ARDrone_IController::set(%d), returning\n"),
                      inherited::mod_->name (),
                      GtkCBData_->videoMode));
          return;
        }
      } // end IF
      break;
    }
    case NAVDATA_STATE_READY:
    {
      if (!deviceInitialized_)
      {
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("%s: initialization complete\n"),
                    inherited::mod_->name ()));

        deviceInitialized_ = true;
      } // end IF

      break;
    }
    case NAVDATA_STATE_CALIBRATE_SENSOR:
    {
      if (!deviceInitialized_)
        trim ();

      break;
    }
    case NAVDATA_STATE_SET_PARAMETER:
      break;
    case NAVDATA_STATE_COMMAND_ACK:
    {
      // switch to next state
      enum ARDRone_NavDataState next_state_e = NAVDATA_STATE_INVALID;
      { ACE_GUARD (ACE_SYNCH_NULL_MUTEX, aGuard, *inherited2::stateLock_);
        switch (inherited2::state_)
        {
          case NAVDATA_STATE_GET_CONFIGURATION:
          {
            // *NOTE*: the device may (still) be in 'bootstrap' mode
            if (deviceState_ & ARDRONE_NAVDATA_BOOTSTRAP)
            {
              ACE_DEBUG ((LM_DEBUG,
                          ACE_TEXT ("%s: switching to navdata mode...\n"),
                          inherited::mod_->name ()));
              next_state_e = NAVDATA_STATE_SWITCH_MODE;
            } // end IF
            else
              next_state_e = NAVDATA_STATE_NAVDATA_OPTIONS;
            break;
          }
          case NAVDATA_STATE_SWITCH_MODE:
            next_state_e = NAVDATA_STATE_NAVDATA_OPTIONS; break;
          case NAVDATA_STATE_NAVDATA_OPTIONS:
            next_state_e = NAVDATA_STATE_SET_VIDEO; break;
          case NAVDATA_STATE_SET_VIDEO:
            next_state_e = NAVDATA_STATE_CALIBRATE_SENSOR; break;
          case NAVDATA_STATE_COMMAND_ACK: // *TODO*: this is wrong; remove ASAP
          case NAVDATA_STATE_CALIBRATE_SENSOR:
          case NAVDATA_STATE_SET_PARAMETER:
          case NAVDATA_STATE_READY:
            next_state_e = NAVDATA_STATE_READY; break;
          default:
          {
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("%s: unknown/invalid previous state (was: \"%s\"), continuing\n"),
                        inherited::mod_->name (),
                        ACE_TEXT (inherited2::stateToString (inherited2::state_).c_str ())));
            ACE_ASSERT (false);
            break;
          }
        } // end SWITCH
      } // end lock scope
      change (next_state_e);

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

  { ACE_GUARD_RETURN (ACE_SYNCH_NULL_MUTEX, aGuard, *inherited2::stateLock_, false);
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
  } // end lock scope

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
  converter << OWN_TYPE_T::currentNavDataMessageId.value ();
  OWN_TYPE_T::currentNavDataMessageId++;
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

  // sanity check(s)
  ACE_ASSERT (inherited::sessionData_);

  const typename SessionMessageType::DATA_T::DATA_T& session_data_r =
    inherited::sessionData_->getR ();

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
  message_p->initialize (session_data_r.sessionId,
                         NULL);
  message_p->set (ARDRONE_MESSAGE_ATCOMMAND);
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
                            ConnectorType>::ids (const std::string& sessionId_in,
                                                 const std::string& userId_in,
                                                 const std::string& applicationId_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_Controller_T::ids"));

  std::ostringstream converter;

  std::string command_string =
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_PREFIX_STRING);
  command_string +=
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_COMMAND_CONFIG_IDS_STRING);
  command_string += ACE_TEXT_ALWAYS_CHAR ("=");
  converter << OWN_TYPE_T::currentNavDataMessageId.value ();
  OWN_TYPE_T::currentNavDataMessageId++;
  command_string += converter.str ();
  command_string += ACE_TEXT_ALWAYS_CHAR (",\"");
  command_string += sessionId_in;
  command_string += ACE_TEXT_ALWAYS_CHAR ("\",\"");
  command_string += userId_in;
  command_string += ACE_TEXT_ALWAYS_CHAR ("\",\"");
  command_string += applicationId_in;
  command_string += ACE_TEXT_ALWAYS_CHAR ("\"\r");

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

  std::ostringstream converter;

  std::string command_string =
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_PREFIX_STRING);
  command_string +=
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_COMMAND_RESET_WATCHDOG_STRING);
  command_string += ACE_TEXT_ALWAYS_CHAR ("=");
  converter << OWN_TYPE_T::currentNavDataMessageId.value ();
  OWN_TYPE_T::currentNavDataMessageId++;
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
                            ConnectorType>::trim ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_Controller_T::trim"));

  std::ostringstream converter;

  std::string command_string =
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_PREFIX_STRING);
  command_string +=
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_COMMAND_FTRIM_STRING);
  command_string += ACE_TEXT_ALWAYS_CHAR ("=");
  converter << OWN_TYPE_T::currentNavDataMessageId.value ();
  OWN_TYPE_T::currentNavDataMessageId++;
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
                            ConnectorType>::calibrate ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_Controller_T::calibrate"));

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
                            ConnectorType>::dump ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_Module_Controller_T::dump"));

  std::ostringstream converter;

  std::string command_string =
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_PREFIX_STRING);
  command_string +=
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_PROTOCOL_AT_COMMAND_CONTROL_STRING);
  command_string += ACE_TEXT_ALWAYS_CHAR ("=");
  converter << OWN_TYPE_T::currentNavDataMessageId.value ();
  OWN_TYPE_T::currentNavDataMessageId++;
  command_string += converter.str ();
  command_string += ACE_TEXT_ALWAYS_CHAR (",");
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << CFG_GET_CONTROL_MODE;
  command_string += converter.str ();
  command_string += ACE_TEXT_ALWAYS_CHAR (",");
  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << 1;
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
  converter << OWN_TYPE_T::currentNavDataMessageId.value ();
  OWN_TYPE_T::currentNavDataMessageId++;
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
