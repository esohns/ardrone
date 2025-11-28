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

//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//#include "stream_lib_directshow_tools.h"
//#include "stream_lib_mediafoundation_tools.h"
//#endif // ACE_WIN32 || ACE_WIN64

const char stream_name_string_[] =
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_NAVDATA_STREAM_NAME_STRING);
const char control_stream_name_string_[] =
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_CONTROL_STREAM_NAME_STRING);

//////////////////////////////////////////

Test_U_Stream::Test_U_Stream ()
 : inherited ()
 , source_ (this,
            ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING))
 , asynchSource_ (this,
                  ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING))
 , decode_ (this,
            ACE_TEXT_ALWAYS_CHAR (ARDRONE_STREAM_MODULE_NAVDATA_DECODER_NAME_STRING))
// , report_ (this,
//            ACE_TEXT_ALWAYS_CHAR (MODULE_STAT_REPORT_DEFAULT_NAME_STRING))
 , controller_ (this,
                ACE_TEXT_ALWAYS_CHAR (ARDRONE_STREAM_MODULE_CONTROLLER_NAME_STRING))
 , CBData_ (NULL)
{
  ARDRONE_TRACE (ACE_TEXT ("Test_U_Stream::Test_U_Stream"));

}

bool
Test_U_Stream::load (Stream_ILayout* layout_inout,
                     bool& delete_out)
{
  ARDRONE_TRACE (ACE_TEXT ("Test_U_Stream::load"));

  // initialize return value(s)
  delete_out = false;

  if (inherited::configuration_->configuration_->dispatchConfiguration->dispatch == COMMON_EVENT_DISPATCH_PROACTOR)
    layout_inout->append (&asynchSource_, NULL, 0);
  else
    layout_inout->append (&source_, NULL, 0);
  layout_inout->append (&decode_, NULL, 0);
//  layout_inout->append (&report_, NULL, 0);
  layout_inout->append (&controller_, NULL, 0);

  return true;
}

bool
Test_U_Stream::initialize (const typename inherited::CONFIGURATION_T& configuration_in)
{
  ARDRONE_TRACE (ACE_TEXT ("Test_U_Stream::initialize"));

  // sanity check(s)
  ACE_ASSERT (!isRunning ());

  bool setup_pipeline = configuration_in.configuration_->setupPipeline;
  bool reset_setup_pipeline = false;
  Test_U_SessionData* session_data_p = NULL;
  typename inherited::CONFIGURATION_T::ITERATOR_T iterator;
  struct Test_U_ModuleHandlerConfiguration* configuration_p = NULL;
  Test_U_AsynchUDPSource* source_impl_p = NULL;
  Test_U_SessionManager_t* session_manager_p = NULL;

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
  session_manager_p = Test_U_SessionManager_t::SINGLETON_T::instance ();
  ACE_ASSERT (session_manager_p);
  session_data_p =
    &const_cast<Test_U_SessionData&> (session_manager_p->getR (inherited::id_));
  session_data_p->stream = this;
  iterator =
      const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (iterator != configuration_in.end ());
  configuration_p =
      dynamic_cast<struct Test_U_ModuleHandlerConfiguration*> ((*iterator).second.second);
  ACE_ASSERT (configuration_p);

  CBData_ = configuration_in.configuration_->CBData;

  // ---------------------------------------------------------------------------

  // ******************* Source ************************
  //source_impl_p = dynamic_cast<Test_U_AsynchUDPSource*> (source_.writer ());
  //ACE_ASSERT (source_impl_p);
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

  ACE_ASSERT (configuration_in.configuration_->initializeNavData);
  if (!configuration_in.configuration_->initializeNavData->initialize (this))
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
Test_U_Stream::messageCB (const struct _navdata_t& record_in,
                          const ARDrone_NavDataOptionOffsets_t& offsets_in,
                          void* payload_in)
{
  ARDRONE_TRACE (ACE_TEXT ("Test_U_Stream::messageCB"));

  //  // dump state
  //  ACE_DEBUG ((LM_DEBUG,
  //              ACE_TEXT ("state:\n\tflying: %s\n\tvideo: %s\n\tvision: %s\n\tcontrol algorithm: %s\n\taltitude control active: %s\n\tstart button state: %s\n\tcontrol command: %s\n\tcamera ready: %s\n\ttravelling: %s\n\tUSB key ready: %s\n\tNavData demo only: %s\n\tbootstrap mode: %s\n\tmotor status: %s\n\tCOM lost: %s\n\tsoftware fault: %s\n\tbattery low: %s\n\temergency landing (user): %s\n\ttimer elapsed: %s\n\tmagnetometer needs calibration: %s\n\tangles out of range: %s\n\twind mask: %s\n\tultrasound mask: %s\n\tcutout system: %s\n\tPIC version number: %s\n\tATcodec thread: %s\n\tNavData thread: %s\n\tvideo thread: %s\n\tacquisition thread: %s\n\tcontrol watchdog: %s\n\tADC watchdog: %s\n\tCOM watchdog: %s\n\temergency landing: %s\n"),
  //              ((record_in.ardrone_state & ARDRONE_FLY_MASK) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
  //              ((record_in.ardrone_state & ARDRONE_VIDEO_MASK) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
  //              ((record_in.ardrone_state & ARDRONE_VISION_MASK) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
  //              ((record_in.ardrone_state & ARDRONE_CONTROL_MASK) ? ACE_TEXT ("euler angles") : ACE_TEXT ("angular speed")),
  //              ((record_in.ardrone_state & ARDRONE_ALTITUDE_MASK) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
  //              ((record_in.ardrone_state & ARDRONE_USER_FEEDBACK_START) ? ACE_TEXT ("on") : ACE_TEXT ("off")),
  //              ((record_in.ardrone_state & ARDRONE_COMMAND_MASK) ? ACE_TEXT ("ACK") : ACE_TEXT ("not set")),
  //              ((record_in.ardrone_state & ARDRONE_CAMERA_MASK) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
  //              ((record_in.ardrone_state & ARDRONE_TRAVELLING_MASK) ? ACE_TEXT ("enabled") : ACE_TEXT ("disabled")),
  //              ((record_in.ardrone_state & ARDRONE_USB_MASK) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
  //              ((record_in.ardrone_state & ARDRONE_NAVDATA_DEMO_MASK) ? ACE_TEXT ("demo only") : ACE_TEXT ("all")),
  //              ((record_in.ardrone_state & ARDRONE_NAVDATA_BOOTSTRAP) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
  //              ((record_in.ardrone_state & ARDRONE_MOTORS_MASK) ? ACE_TEXT ("error") : ACE_TEXT ("OK")),
  //              ((record_in.ardrone_state & ARDRONE_COM_LOST_MASK) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
  //              ((record_in.ardrone_state & ARDRONE_SOFTWARE_FAULT) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
  //              ((record_in.ardrone_state & ARDRONE_VBAT_LOW) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
  //              ((record_in.ardrone_state & ARDRONE_USER_EL) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
  //              ((record_in.ardrone_state & ARDRONE_TIMER_ELAPSED) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
  //              ((record_in.ardrone_state & ARDRONE_MAGNETO_NEEDS_CALIB) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
  //              ((record_in.ardrone_state & ARDRONE_ANGLES_OUT_OF_RANGE) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
  //              ((record_in.ardrone_state & ARDRONE_WIND_MASK) ? ACE_TEXT ("error") : ACE_TEXT ("OK")),
  //              ((record_in.ardrone_state & ARDRONE_ULTRASOUND_MASK) ? ACE_TEXT ("error") : ACE_TEXT ("OK")),
  //              ((record_in.ardrone_state & ARDRONE_CUTOUT_MASK) ? ACE_TEXT ("detected") : ACE_TEXT ("not detected")),
  //              ((record_in.ardrone_state & ARDRONE_PIC_VERSION_MASK) ? ACE_TEXT ("OK") : ACE_TEXT ("error")),
  //              ((record_in.ardrone_state & ARDRONE_ATCODEC_THREAD_ON) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
  //              ((record_in.ardrone_state & ARDRONE_NAVDATA_THREAD_ON) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
  //              ((record_in.ardrone_state & ARDRONE_VIDEO_THREAD_ON) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
  //              ((record_in.ardrone_state & ARDRONE_ACQ_THREAD_ON) ? ACE_TEXT ("yes") : ACE_TEXT ("no")),
  //              ((record_in.ardrone_state & ARDRONE_CTRL_WATCHDOG_MASK) ? ACE_TEXT ("delayed >5ms") : ACE_TEXT ("OK")),
  //              ((record_in.ardrone_state & ARDRONE_ADC_WATCHDOG_MASK) ? ACE_TEXT ("delayed >5ms") : ACE_TEXT ("OK")),
  //              ((record_in.ardrone_state & ARDRONE_COM_WATCHDOG_MASK) ? ACE_TEXT ("error") : ACE_TEXT ("OK")),
  //              ((record_in.ardrone_state & ARDRONE_EMERGENCY_MASK) ? ACE_TEXT ("yes") : ACE_TEXT ("no"))));

  //  // *TODO*: dump options

  struct _navdata_option_t* option_p = NULL;
  for (ARDrone_NavDataOptionOffsetsIterator_t iterator = offsets_in.begin ();
       iterator != offsets_in.end ();
       ++iterator)
  {
    option_p =
        reinterpret_cast<struct _navdata_option_t*> (static_cast<char*> (payload_in) + *iterator);
    switch (option_p->tag)
    {
      case NAVDATA_DEMO_TAG: // 0
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_demo_t));
        struct _navdata_demo_t* option_2 =
            reinterpret_cast<struct _navdata_demo_t*> (option_p);
        ACE_ASSERT (option_2);

#if defined (GUI_SUPPORT)
        // *NOTE*: values are in milli-degrees
        CBData_->openGLScene.orientation.x =
            option_2->phi / 1000.0f; // roll (--> rotation along x)
        CBData_->openGLScene.orientation.y =
            option_2->psi / 1000.0f; // yaw (--> rotation along y)
        CBData_->openGLScene.orientation.z =
            option_2->theta / 1000.0f; // pitch (--> rotation along z)
#endif // GUI_SUPPORT
        //ACE_DEBUG ((LM_DEBUG,
        //            ACE_TEXT ("orientation (roll/pitch/yaw): %.2f/%.2f/%.2f\n"),
        //            CBData_->openGLScene.orientation.x, CBData_->openGLScene.orientation.y, CBData_->openGLScene.orientation.z));
        break;
      }
      case NAVDATA_TIME_TAG: // 1
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_time_t));
        struct _navdata_time_t* option_2 =
            reinterpret_cast<struct _navdata_time_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_RAW_MEASURES_TAG: // 2
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_raw_measures_t));
        struct _navdata_raw_measures_t* option_2 =
            reinterpret_cast<struct _navdata_raw_measures_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_PHYS_MEASURES_TAG: // 3
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_phys_measures_t));
        struct _navdata_phys_measures_t* option_2 =
            reinterpret_cast<struct _navdata_phys_measures_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_GYROS_OFFSETS_TAG: // 4
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_gyros_offsets_t));
        struct _navdata_gyros_offsets_t* option_2 =
            reinterpret_cast<struct _navdata_gyros_offsets_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_EULER_ANGLES_TAG: // 5
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_euler_angles_t));
        struct _navdata_euler_angles_t* option_2 =
            reinterpret_cast<struct _navdata_euler_angles_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_REFERENCES_TAG: // 6
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_references_t));
        struct _navdata_references_t* option_2 =
            reinterpret_cast<struct _navdata_references_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_TRIMS_TAG: // 7
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_trims_t));
        struct _navdata_trims_t* option_2 =
            reinterpret_cast<struct _navdata_trims_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_RC_REFERENCES_TAG: // 8
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_rc_references_t));
        struct _navdata_rc_references_t* option_2 =
            reinterpret_cast<struct _navdata_rc_references_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_PWM_TAG: // 9
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_pwm_t));
        struct _navdata_pwm_t* option_2 =
            reinterpret_cast<struct _navdata_pwm_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_ALTITUDE_TAG: // 10
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_altitude_t));
        struct _navdata_altitude_t* option_2 =
            reinterpret_cast<struct _navdata_altitude_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_VISION_RAW_TAG: // 11
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_vision_raw_t));
        struct _navdata_vision_raw_t* option_2 =
            reinterpret_cast<struct _navdata_vision_raw_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_VISION_OF_TAG: // 12
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_vision_of_t));
        struct _navdata_vision_of_t* option_2 =
            reinterpret_cast<struct _navdata_vision_of_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_VISION_TAG: // 13
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_vision_t));
        struct _navdata_vision_t* option_2 =
            reinterpret_cast<struct _navdata_vision_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_VISION_PERF_TAG: // 14
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_vision_perf_t));
        struct _navdata_vision_perf_t* option_2 =
            reinterpret_cast<struct _navdata_vision_perf_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_TRACKERS_SEND_TAG: // 15
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_trackers_send_t));
        struct _navdata_trackers_send_t* option_2 =
            reinterpret_cast<struct _navdata_trackers_send_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_VISION_DETECT_TAG: // 16
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_vision_detect_t));
        struct _navdata_vision_detect_t* option_2 =
            reinterpret_cast<struct _navdata_vision_detect_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_WATCHDOG_TAG: // 17
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_watchdog_t));
        struct _navdata_watchdog_t* option_2 =
            reinterpret_cast<struct _navdata_watchdog_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_ADC_DATA_FRAME_TAG: // 18
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_adc_data_frame_t));
        struct _navdata_adc_data_frame_t* option_2 =
            reinterpret_cast<struct _navdata_adc_data_frame_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_VIDEO_STREAM_TAG: // 19
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_video_stream_t));
        struct _navdata_video_stream_t* option_2 =
            reinterpret_cast<struct _navdata_video_stream_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_GAMES_TAG: // 20
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_games_t));
        struct _navdata_games_t* option_2 =
            reinterpret_cast<struct _navdata_games_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_PRESSURE_RAW_TAG: // 21
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_pressure_raw_t));
        struct _navdata_pressure_raw_t* option_2 =
            reinterpret_cast<struct _navdata_pressure_raw_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_MAGNETO_TAG: // 22
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_magneto_t));
        struct _navdata_magneto_t* option_2 =
            reinterpret_cast<struct _navdata_magneto_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
        }
      case NAVDATA_WIND_TAG: // 23
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_wind_speed_t));
        struct _navdata_wind_speed_t* option_2 =
            reinterpret_cast<struct _navdata_wind_speed_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_KALMAN_PRESSURE_TAG: // 24
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_kalman_pressure_t));
        struct _navdata_kalman_pressure_t* option_2 =
            reinterpret_cast<struct _navdata_kalman_pressure_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_HDVIDEO_STREAM_TAG: // 25
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_hdvideo_stream_t));
        struct _navdata_hdvideo_stream_t* option_2 =
            reinterpret_cast<struct _navdata_hdvideo_stream_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_WIFI_TAG: // 26
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_wifi_t));
        struct _navdata_wifi_t* option_2 =
            reinterpret_cast<struct _navdata_wifi_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_ZIMMU_3000_TAG: // 27
      { //ACE_ASSERT (option_p->size == sizeof (struct _navdata_zimmu_3000_t));
        struct _navdata_zimmu_3000_t* option_2 =
            reinterpret_cast<struct _navdata_zimmu_3000_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      case NAVDATA_CKS_TAG: // 65535
      { ACE_ASSERT (option_p->size == sizeof (struct _navdata_cks_t));
        struct _navdata_cks_t* option_2 =
            reinterpret_cast<struct _navdata_cks_t*> (option_p);
        ACE_UNUSED_ARG (option_2);
        break;
      }
      default:
      {
        //ACE_DEBUG ((LM_ERROR,
        //            ACE_TEXT ("%s: invalid/unknown NavData option (was: %d), continuing\n"),
        //            ACE_TEXT (stream_name_string_),
        //            option_p->tag));
        break;
      }
    } // end SWITCH
  } // end FOR
}

//////////////////////////////////////////

Test_U_ControlStream::Test_U_ControlStream ()
 : inherited ()
 , source_ (this,
            ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING))
 , asynchSource_ (this,
                  ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING))
 , decode_ (this,
            ACE_TEXT_ALWAYS_CHAR (ARDRONE_STREAM_MODULE_CONTROL_DECODER_NAME_STRING))
 //, report_ (this,
 //           ACE_TEXT_ALWAYS_CHAR (MODULE_STAT_REPORT_DEFAULT_NAME_STRING))
 , configuration_ (NULL)
{
  ARDRONE_TRACE (ACE_TEXT ("Test_U_ControlStream::Test_U_ControlStream"));

}

bool
Test_U_ControlStream::load (Stream_ILayout* layout_inout,
                            bool& delete_out)
{
  ARDRONE_TRACE (ACE_TEXT ("Test_U_ControlStream::load"));

  // initialize return value(s)
  delete_out = false;

  if (inherited::configuration_->configuration_->dispatchConfiguration->dispatch == COMMON_EVENT_DISPATCH_PROACTOR)
    layout_inout->append (&asynchSource_, NULL, 0);
  else
    layout_inout->append (&source_, NULL, 0);
  layout_inout->append (&decode_, NULL, 0);
  //layout_inout->append (&report_, NULL, 0);

  return true;
}

bool
Test_U_ControlStream::initialize (const typename inherited::CONFIGURATION_T& configuration_in)
{
  ARDRONE_TRACE (ACE_TEXT ("Test_U_ControlStream::initialize"));

  // sanity check(s)
  ACE_ASSERT (!isRunning ());

  bool setup_pipeline = configuration_in.configuration_->setupPipeline;
  bool reset_setup_pipeline = false;
  Test_U_SessionData* session_data_p = NULL;
  typename inherited::CONFIGURATION_T::ITERATOR_T iterator;
  struct Test_U_ModuleHandlerConfiguration* configuration_p = NULL;
  Test_U_AsynchTCPSource* source_impl_p = NULL;
  Test_U_SessionManager_t* session_manager_p = NULL;

  // allocate a new session state, reset stream
  const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_->setupPipeline =
    false;
  reset_setup_pipeline = true;
  if (!inherited::initialize (configuration_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_Base_T::initialize(), aborting\n"),
                ACE_TEXT (control_stream_name_string_)));
    goto error;
  } // end IF
  const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_->setupPipeline =
    setup_pipeline;
  reset_setup_pipeline = false;

  // sanity check(s)
  session_manager_p = Test_U_SessionManager_t::SINGLETON_T::instance ();
  ACE_ASSERT (session_manager_p);
  session_data_p =
    &const_cast<Test_U_SessionData&> (session_manager_p->getR (inherited::id_));
  session_data_p->stream = this;
  iterator =
    const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (iterator != configuration_in.end ());
  configuration_p =
    dynamic_cast<struct Test_U_ModuleHandlerConfiguration*> ((*iterator).second.second);
  ACE_ASSERT (configuration_p);

  // sanity check(s)
  ACE_ASSERT (configuration_in.configuration_->deviceConfiguration);

  configuration_ = configuration_in.configuration_->deviceConfiguration;

  ACE_ASSERT (configuration_in.configuration_->initializeControl);
  if (!configuration_in.configuration_->initializeControl->initialize (this))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to initialize event handler, aborting\n"),
                ACE_TEXT (control_stream_name_string_)));
    goto error;
  } // end IF

  // ---------------------------------------------------------------------------

  // ******************* Source ************************
  //source_impl_p = dynamic_cast<Test_U_AsynchTCPSource*> (source_.writer ());
  //ACE_ASSERT (source_impl_p);
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
                  ACE_TEXT (control_stream_name_string_)));
      goto error;
    } // end IF

  // -------------------------------------------------------------

  inherited::isInitialized_ = true;

  return true;

error:
  if (reset_setup_pipeline)
    const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_->setupPipeline =
      setup_pipeline;

  return false;
}

void
Test_U_ControlStream::messageCB (const ARDrone_DeviceConfiguration_t& deviceConfiguration_in)
{
  ARDRONE_TRACE (ACE_TEXT ("Test_U_ControlStream::messageCB"));

  // sanity check(s)
  ACE_ASSERT (configuration_);

  try {
    configuration_->setP (&const_cast<ARDrone_DeviceConfiguration_t&> (deviceConfiguration_in));
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: caught exception in ARDrone_IDeviceConfiguration::setP(), continuing\n"),
                ACE_TEXT (control_stream_name_string_)));
  }

  //// debug info
  //unsigned int number_of_settings = 0;
  //for (ARDrone_DeviceConfigurationConstIterator_t iterator = deviceConfiguration_in.begin ();
  //     iterator != deviceConfiguration_in.end ();
  //     ++iterator)
  //  number_of_settings += (*iterator).second.size ();
  //ACE_DEBUG ((LM_DEBUG,
  //            ACE_TEXT ("received device configuration (%d setting(s) in %d categories):\n"),
  //            number_of_settings, deviceConfiguration_in.size ()));
  //for (ARDrone_DeviceConfigurationConstIterator_t iterator = deviceConfiguration_in.begin ();
  //     iterator != deviceConfiguration_in.end ();
  //     ++iterator)
  //{
  //  ACE_DEBUG ((LM_DEBUG,
  //              ACE_TEXT ("--- \"%s\" (%d setting(s) ---):\n"),
  //              ACE_TEXT ((*iterator).first.c_str ()), (*iterator).second.size ()));
  //  for (ARDrone_DeviceConfigurationCategoryIterator_t iterator_2 = (*iterator).second.begin ();
  //       iterator_2 != (*iterator).second.end ();
  //       ++iterator_2)
  //    ACE_DEBUG ((LM_DEBUG,
  //                ACE_TEXT ("\t%s:\t%s\n"),
  //                ACE_TEXT ((*iterator_2).first.c_str ()),
  //                ACE_TEXT ((*iterator_2).second.c_str ())));
  //} // end FOR
}
