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
#include "stdafx.h"

#include "test_u_session_message.h"
#include "test_u_stream.h"

#include "ace/Log_Msg.h"

#include "stream_macros.h"

#include "stream_dec_defines.h"

#include "stream_misc_defines.h"

#include "stream_stat_defines.h"

#include "stream_vis_defines.h"
#include "stream_vis_tools.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "stream_lib_directshow_tools.h"
#include "stream_lib_mediafoundation_tools.h"
#endif // ACE_WIN32 || ACE_WIN64

Test_U_Stream::Test_U_Stream ()
 : inherited ()
 , source_ (this,
            ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING))
 , decode_ (this,
            ACE_TEXT_ALWAYS_CHAR (ARDRONE_STREAM_MDOULE_MAVLINK_DECODER_NAME_STRING))
// , report_ (this,
//            ACE_TEXT_ALWAYS_CHAR (MODULE_STAT_REPORT_DEFAULT_NAME_STRING))
 , handler_ (this,
             ACE_TEXT_ALWAYS_CHAR (STREAM_MISC_MESSAGEHANDLER_DEFAULT_NAME_STRING))
{
  STREAM_TRACE (ACE_TEXT ("Test_U_Stream::Test_U_Stream"));

}

Test_U_Stream::~Test_U_Stream ()
{
  STREAM_TRACE (ACE_TEXT ("Test_U_Stream::~Test_U_Stream"));

  // *NOTE*: this implements an ordered shutdown on destruction...
  inherited::shutdown ();
}

bool
Test_U_Stream::load (Stream_ILayout* layout_inout,
                     bool& delete_out)
{
  STREAM_TRACE (ACE_TEXT ("Test_U_Stream::load"));

  // initialize return value(s)
  delete_out = false;

  layout_inout->append (&source_, NULL, 0);
  layout_inout->append (&decode_, NULL, 0);
//  layout_inout->append (&report_, NULL, 0);
  layout_inout->append (&handler_, NULL, 0);

  return true;
}

bool
Test_U_Stream::initialize (const typename inherited::CONFIGURATION_T& configuration_in)
{
  STREAM_TRACE (ACE_TEXT ("Test_U_Stream::initialize"));

  // sanity check(s)
  ACE_ASSERT (!isRunning ());

  bool setup_pipeline = configuration_in.configuration_->setupPipeline;
  bool reset_setup_pipeline = false;
  Test_U_SessionData* session_data_p = NULL;
  typename inherited::CONFIGURATION_T::ITERATOR_T iterator;
  struct Test_U_ModuleHandlerConfiguration* configuration_p = NULL;
  Test_U_AsynchUDPSource* source_impl_p = NULL;

  // allocate a new session state, reset stream
  const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_->setupPipeline =
    false;
  reset_setup_pipeline = true;
  if (!inherited::initialize (configuration_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_Base_T::initialize(), aborting\n"),
                ACE_TEXT (stream_name_string_)));
    goto error;
  } // end IF
  const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_->setupPipeline =
    setup_pipeline;
  reset_setup_pipeline = false;

  // sanity check(s)
  ACE_ASSERT (inherited::sessionData_);
  session_data_p =
    &const_cast<Test_U_SessionData&> (inherited::sessionData_->getR ());
  iterator =
      const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (iterator != configuration_in.end ());
  configuration_p =
      dynamic_cast<struct Test_U_ModuleHandlerConfiguration*> ((*iterator).second.second);
  ACE_ASSERT (configuration_p);

  // ---------------------------------------------------------------------------

  // ******************* Source ************************
  source_impl_p = dynamic_cast<Test_U_AsynchUDPSource*> (source_.writer ());
  ACE_ASSERT (source_impl_p);
  //source_impl_p->setP (&(inherited::state_));

  //// *NOTE*: push()ing the module will open() it
  ////         --> set the argument that is passed along (head module expects a
  ////             handle to the session data)
  //source_.arg (inherited::sessionData_);

  if (configuration_in.configuration_->setupPipeline)
    if (!inherited::setup (NULL))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to set up pipeline, aborting\n"),
                  ACE_TEXT (stream_name_string_)));
      goto error;
    } // end IF

  // -------------------------------------------------------------

  ACE_ASSERT (configuration_in.configuration_->initializeMAVLink);
  if (!configuration_in.configuration_->initializeMAVLink->initialize (this))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to initialize event handler, aborting\n"),
                ACE_TEXT (stream_name_string_)));
    goto error;
  } // end IF

  inherited::isInitialized_ = true;

  return true;

error:
  if (reset_setup_pipeline)
    const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_->setupPipeline =
      setup_pipeline;

  return false;
}

void
Test_U_Stream::messageCB (const struct __mavlink_message& record_in,
                          void* payload_in)
{
  STREAM_TRACE (ACE_TEXT ("Test_U_Stream::messageCB"));

  switch (record_in.msgid)
  {
    case MAVLINK_MSG_ID_HEARTBEAT: // 0
    { ACE_ASSERT (record_in.len == sizeof (struct __mavlink_heartbeat_t));
      struct __mavlink_heartbeat_t* message_p =
          reinterpret_cast<struct __mavlink_heartbeat_t*> (payload_in);
      ACE_UNUSED_ARG (message_p);
      break;
    }
    case MAVLINK_MSG_ID_SYS_STATUS: // 1
    { ACE_ASSERT (record_in.len == sizeof (struct __mavlink_sys_status_t));
      struct __mavlink_sys_status_t* message_p =
          reinterpret_cast<struct __mavlink_sys_status_t*> (payload_in);
      ACE_UNUSED_ARG (message_p);
      break;
    }
    case MAVLINK_MSG_ID_PARAM_VALUE: // 22
    { ACE_ASSERT (record_in.len == sizeof (struct __mavlink_param_value_t));
      struct __mavlink_param_value_t* message_p =
          reinterpret_cast<struct __mavlink_param_value_t*> (payload_in);
      ACE_UNUSED_ARG (message_p);
      break;
    }
    case MAVLINK_MSG_ID_GPS_RAW_INT: // 24
    { //ACE_ASSERT (record_in.len == sizeof (struct __mavlink_gps_raw_int_t));
      ACE_ASSERT ((record_in.len == MAVLINK_MSG_ID_GPS_RAW_INT_MIN_LEN) || (record_in.len == MAVLINK_MSG_ID_GPS_RAW_INT_LEN));
      struct __mavlink_gps_raw_int_t* message_p =
          reinterpret_cast<struct __mavlink_gps_raw_int_t*> (payload_in);
      ACE_UNUSED_ARG (message_p);
      break;
    }
    case MAVLINK_MSG_ID_ATTITUDE: // 30
    { ACE_ASSERT (record_in.len == sizeof (struct __mavlink_attitude_t));
      struct __mavlink_attitude_t* message_p =
          reinterpret_cast<struct __mavlink_attitude_t*> (payload_in);
#if defined (_DEBUG)
        ACE_DEBUG ((LM_DEBUG,
                    ACE_TEXT ("orientation (roll/pitch/yaw): %f/%f/%f\n\n"),
                    message_p->roll, message_p->pitch, message_p->yaw));
#endif // _DEBUG
      break;
    }
    case MAVLINK_MSG_ID_GLOBAL_POSITION_INT: // 33
    { ACE_ASSERT (record_in.len == sizeof (struct __mavlink_global_position_int_t));
      struct __mavlink_global_position_int_t* message_p =
          reinterpret_cast<struct __mavlink_global_position_int_t*> (payload_in);
      ACE_UNUSED_ARG (message_p);
      break;
    }
    case MAVLINK_MSG_ID_MISSION_CURRENT: // 42
    { ACE_ASSERT (record_in.len == sizeof (struct __mavlink_mission_current_t));
      struct __mavlink_mission_current_t* message_p =
          reinterpret_cast<struct __mavlink_mission_current_t*> (payload_in);
      ACE_UNUSED_ARG (message_p);
      break;
    }
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: invalid/unknown MAVLink message id (was: %u), continuing\n"),
                  ACE_TEXT (stream_name_string_),
                  record_in.msgid));
      break;
    }
  } // end SWITCH}
}
