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

#include "ace/Synch.h"
#include "ardrone_stream.h"

#include "stream_misc_defines.h"

#include "ardrone_defines.h"
#include "ardrone_modules_common.h"

const char video_stream_name_string_[] =
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING);
const char control_stream_name_string_[] =
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_CONTROL_STREAM_NAME_STRING);
const char navdata_stream_name_string_[] =
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_NAVDATA_STREAM_NAME_STRING);
const char mavlink_stream_name_string_[] =
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_MAVLINK_STREAM_NAME_STRING);

std::string 
ARDroneStreamTypeToString (const enum ARDrone_StreamType type_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::ARDroneStreamTypeToString"));

  switch (type_in)
  {
    case ARDRONE_STREAM_CONTROL:
      return ACE_TEXT_ALWAYS_CHAR (ARDRONE_CONTROL_STREAM_NAME_STRING);
    case ARDRONE_STREAM_MAVLINK:
      return ACE_TEXT_ALWAYS_CHAR (ARDRONE_MAVLINK_STREAM_NAME_STRING);
    case ARDRONE_STREAM_NAVDATA:
      return ACE_TEXT_ALWAYS_CHAR (ARDRONE_NAVDATA_STREAM_NAME_STRING);
    case ARDRONE_STREAM_VIDEO:
      return ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING);
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unkown stream type (was: %d), aborting\n"),
                  type_in));
      break;
    }
  } // end SWITCH

  return ACE_TEXT_ALWAYS_CHAR ("");
}

std::string 
ARDroneVideoModeToString (const enum ARDrone_VideoMode mode_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::ARDroneVideoModeToString"));
  
  switch (mode_in)
  {
    case ARDRONE_VIDEOMODE_360P:
      return ACE_TEXT_ALWAYS_CHAR ("H264 360p");
    case ARDRONE_VIDEOMODE_720P:
      return ACE_TEXT_ALWAYS_CHAR ("H264 720p");
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unkown video mode (was: %d), aborting\n"),
                  mode_in));
      break;
    }
  } // end SWITCH

  return ACE_TEXT_ALWAYS_CHAR ("");
}
void
ARDroneVideoModeToResolution (const enum ARDrone_VideoMode mode_in,
                              unsigned int& width_out,
                              unsigned int& height_out)
{
  ARDRONE_TRACE (ACE_TEXT ("::ARDroneVideoModeToResolution"));

  // initialize return value(s)
  width_out = 0;
  height_out = 0;

  switch (mode_in)
  {
    case ARDRONE_VIDEOMODE_360P:
      width_out = ARDRONE_H264_360P_VIDEO_WIDTH;
      height_out = ARDRONE_H264_360P_VIDEO_HEIGHT;
      break;
    case ARDRONE_VIDEOMODE_720P:
      width_out = ARDRONE_H264_720P_VIDEO_WIDTH;
      height_out = ARDRONE_H264_720P_VIDEO_HEIGHT;
      break;
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unkown video mode (was: %d), aborting\n"),
                  mode_in));
      break;
    }
  } // end SWITCH
}

//////////////////////////////////////////

ARDrone_ControlStream::ARDrone_ControlStream ()
 : inherited ()
 , inherited2 (&(inherited::sessionDataLock_))
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_ControlStream::ARDrone_ControlStream"));

  inherited::state_.type = ARDRONE_STREAM_CONTROL;
}

bool
ARDrone_ControlStream::load (Stream_ModuleList_t& modules_out,
                             bool& delete_out)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_ControlStream::load"));

  // sanity check(s)
  ACE_ASSERT (inherited::configuration_);

  Stream_Module_t* module_p = NULL;
//  ACE_NEW_RETURN (module_p,
//                  ARDrone_Module_FileWriter_Module (this,
//                                                    ACE_TEXT_ALWAYS_CHAR ("FileWriter")),
//                  false);
//  modules_out.push_back (module_p);
//  module_p = NULL;
//#if defined (_DEBUG)
//  ACE_NEW_RETURN (module_p,
//                  ARDrone_Module_Dump_Module (this,
//                                              ACE_TEXT_ALWAYS_CHAR ("Dump")),
//                  false);
//  modules_out.push_back (module_p);
//  module_p = NULL;
//#endif
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_ControlDecoder_Module (this,
                                                        ACE_TEXT_ALWAYS_CHAR (MODULE_MISC_PARSER_DEFAULT_NAME_STRING)),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_StatisticReport_Module (this,
                                                         ACE_TEXT_ALWAYS_CHAR (MODULE_STAT_REPORT_DEFAULT_NAME_STRING)),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
  if (inherited::configuration_->configuration_.useReactor)
    ACE_NEW_RETURN (module_p,
                    ARDrone_Module_TCPSource_Module (this,
                                                     ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING)),
                    false);
  else
    ACE_NEW_RETURN (module_p,
                    ARDrone_Module_AsynchTCPSource_Module (this,
                                                           ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING)),
                    false);
  modules_out.push_back (module_p);

  delete_out = true;

  return true;
}

bool
ARDrone_ControlStream::initialize (const typename inherited::CONFIGURATION_T& configuration_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_ControlStream::initialize"));

  if (inherited::isInitialized_)
  {
    configuration_ = NULL;
  } // end IF

//  bool result = false;
  bool setup_pipeline = configuration_in.configuration_.setupPipeline;
  bool reset_setup_pipeline = false;
  //  struct ARDrone_SessionData* session_data_p = NULL;
  typename inherited::CONFIGURATION_T::ITERATOR_T iterator;
  struct ARDrone_ModuleHandlerConfiguration* configuration_p = NULL;
  Stream_Module_t* module_p = NULL;
  Common_ISetP_T<struct ARDrone_StreamState>* iset_p = NULL;

  // allocate a new session state, reset stream
  const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_.setupPipeline =
    false;
  reset_setup_pipeline = true;
  if (!inherited::initialize (configuration_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_Base_T::initialize(), aborting\n"),
                ACE_TEXT (control_stream_name_string_)));
    goto error;
  } // end IF
  const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_.setupPipeline =
    setup_pipeline;
  reset_setup_pipeline = false;

  // sanity check(s)
  ACE_ASSERT (configuration_in.configuration_.CBData);

  configuration_ = configuration_in.configuration_.CBData->controller;

  // sanity check(s)
  ACE_ASSERT (configuration_);

  iterator =
    const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).find (ACE_TEXT_ALWAYS_CHAR (""));

  // sanity check(s)
  ACE_ASSERT (iterator != configuration_in.end ());

  configuration_p =
    dynamic_cast<struct ARDrone_ModuleHandlerConfiguration*> (&((*iterator).second.second));

  // sanity check(s)
  ACE_ASSERT (configuration_p);
  ACE_ASSERT (configuration_p->subscribers);

  configuration_p->subscribers->push_back (this);
  configuration_p->subscribers->sort ();
  configuration_p->subscribers->unique (SUBSCRIBERS_IS_EQUAL_P ());

  // ---------------------------------------------------------------------------

  // ******************************** Source ***********************************
  module_p =
    const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING),
                                                   true,
                                                   false));
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to retrieve \"%s\" module handle, aborting\n"),
                ACE_TEXT (control_stream_name_string_),
                ACE_TEXT (MODULE_NET_SOURCE_DEFAULT_NAME_STRING)));
    goto error;
  } // end IF

  iset_p =
    dynamic_cast<Common_ISetP_T<struct ARDrone_StreamState>*> (module_p->writer ());
  ACE_ASSERT (iset_p);
  iset_p->setP (&(inherited::state_));

  // enqueue the module
  // *NOTE*: push()ing the module will open() it
  // --> set the argument that is passed along
  module_p->arg (inherited::sessionData_);

  // ---------------------------------------------------------------------------

  ACE_ASSERT (configuration_in.configuration_.initializeControl);
  if (!configuration_in.configuration_.initializeControl->initialize (this))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to initialize event handler, aborting\n"),
                ACE_TEXT (control_stream_name_string_)));
    goto error;
  } // end IF

  // ---------------------------------------------------------------------------

  if (configuration_in.configuration_.setupPipeline)
    if (!inherited::setup (NULL))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to set up pipeline, aborting\n"),
                  ACE_TEXT (control_stream_name_string_)));
      goto error;
    } // end IF

  inherited::isInitialized_ = true;
  //   inherited::dump_state();

  return true;

error:
  if (reset_setup_pipeline)
    const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_.setupPipeline =
      setup_pipeline;

  return false;
}

void
ARDrone_ControlStream::start (Stream_SessionId_t sessionId_in,
                              const struct ARDrone_SessionData& sessionData_in)
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_ControlStream::start"));

  ACE_UNUSED_ARG (sessionId_in);
  ACE_UNUSED_ARG (sessionData_in);
}
void
ARDrone_ControlStream::notify (Stream_SessionId_t sessionId_in,
                               const enum Stream_SessionMessageType& event_in)
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_ControlStream::notify"));

  // sanity check(s)
  ACE_ASSERT (inherited::sessionData_);
  const struct ARDrone_SessionData& session_data_r =
      inherited::sessionData_->getR ();
  if (session_data_r.sessionId != sessionId_in)
    return;

  ACE_UNUSED_ARG (event_in);
}
void
ARDrone_ControlStream::end (Stream_SessionId_t sessionId_in)
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_ControlStream::end"));

  // sanity check(s)
  ACE_ASSERT (inherited::sessionData_);
  const struct ARDrone_SessionData& session_data_r =
      inherited::sessionData_->getR ();
  if (session_data_r.sessionId != sessionId_in)
    return;
}

bool
ARDrone_ControlStream::collect (struct ARDrone_Statistic& data_out)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_ControlStream::collect"));

  int result = -1;
  struct ARDrone_SessionData& session_data_r =
      const_cast<struct ARDrone_SessionData&> (inherited::sessionData_->getR ());
  Stream_Module_t* module_p =
    const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR (MODULE_STAT_REPORT_DEFAULT_NAME_STRING),
                                                   true,
                                                   false));
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to retrieve \"%s\" module handle, aborting\n"),
                ACE_TEXT (control_stream_name_string_),
                ACE_TEXT (MODULE_STAT_REPORT_DEFAULT_NAME_STRING)));
    return false;
  } // end IF
  ARDrone_Module_Statistic_WriterTask_t* statistic_impl_p =
    dynamic_cast<ARDrone_Module_Statistic_WriterTask_t*> (module_p->writer ());
  if (!statistic_impl_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: dynamic_cast<ARDrone_Module_Statistic_WriterTask_t> failed, aborting\n"),
                ACE_TEXT (control_stream_name_string_)));
    return false;
  } // end IF

  // synch access
  if (session_data_r.lock)
  {
    result = session_data_r.lock->acquire ();
    if (result == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to ACE_SYNCH_MUTEX::acquire(): \"%m\", aborting\n"),
                  ACE_TEXT (control_stream_name_string_)));
      return false;
    } // end IF
  } // end IF

  session_data_r.statistic.timeStamp = COMMON_TIME_NOW;

  // delegate to the statistic module
  bool result_2 = false;
  try {
    result_2 = statistic_impl_p->collect (data_out);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: caught exception in Common_IStatistic_T::collect(), continuing\n"),
                ACE_TEXT (control_stream_name_string_)));
  }
  if (!result_2)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Common_IStatistic_T::collect(), aborting\n"),
                ACE_TEXT (control_stream_name_string_)));
  else
    session_data_r.statistic = data_out;

  if (session_data_r.lock)
  {
    result = session_data_r.lock->release ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to ACE_SYNCH_MUTEX::release(): \"%m\", continuing\n"),
                  ACE_TEXT (control_stream_name_string_)));
  } // end IF

  return result_2;
}

void
ARDrone_ControlStream::report () const
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_ControlStream::report"));

  ACE_DEBUG ((LM_INFO,
              ACE_TEXT ("*** [session: %d] RUNTIME STATISTIC ***\n--> stream statistic @ %#D<--\n (data) messages: %u\n dropped messages: %u\n bytes total: %.0f\n*** RUNTIME STATISTIC ***\\END\n"),
              (inherited::state_.sessionData ? static_cast<int> (inherited::state_.sessionData->sessionId) : -1),
              &(inherited::state_.sessionData->lastCollectionTimeStamp),
              inherited::state_.sessionData->statistic.dataMessages,
              inherited::state_.sessionData->statistic.droppedFrames,
              inherited::state_.sessionData->statistic.bytes));
}

void
ARDrone_ControlStream::messageCB (const ARDrone_DeviceConfiguration_t& deviceConfiguration_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_ControlStream::messageCB"));

  // sanity check(s)
  ACE_ASSERT (configuration_);

  try {
    configuration_->setP (&const_cast<ARDrone_DeviceConfiguration_t&> (deviceConfiguration_in));
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: caught exception in ARDrone_IDeviceConfiguration::setP(), continuing\n"),
                ACE_TEXT (control_stream_name_string_)));
  }

//#if defined (_DEBUG)
//  // debug info
//  unsigned int number_of_settings = 0;
//  for (ARDrone_DeviceConfigurationConstIterator_t iterator = deviceConfiguration_in.begin ();
//       iterator != deviceConfiguration_in.end ();
//       ++iterator)
//    number_of_settings += (*iterator).second.size ();
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("received device configuration (%d setting(s) in %d categories):\n"),
//              number_of_settings, deviceConfiguration_in.size ()));
//  for (ARDrone_DeviceConfigurationConstIterator_t iterator = deviceConfiguration_in.begin ();
//       iterator != deviceConfiguration_in.end ();
//       ++iterator)
//  {
//    ACE_DEBUG ((LM_DEBUG,
//                ACE_TEXT ("--- \"%s\" (%d setting(s) ---):\n"),
//                ACE_TEXT ((*iterator).first.c_str ()), (*iterator).second.size ()));
//    for (ARDrone_DeviceConfigurationCategoryIterator_t iterator_2 = (*iterator).second.begin ();
//         iterator_2 != (*iterator).second.end ();
//         ++iterator_2)
//      ACE_DEBUG ((LM_DEBUG,
//                  ACE_TEXT ("\t%s:\t%s\n"),
//                  ACE_TEXT ((*iterator_2).first.c_str ()),
//                  ACE_TEXT ((*iterator_2).second.c_str ())));
//  } // end FOR
//#endif
}

//////////////////////////////////////////

ARDrone_NavDataStream::ARDrone_NavDataStream ()
 : inherited ()
 , inherited2 (&(inherited::sessionDataLock_))
 , isFirst_ (true)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_NavDataStream::ARDrone_NavDataStream"));

  inherited::state_.type = ARDRONE_STREAM_NAVDATA;

  ARDrone_WLANMonitor_t* WLAN_monitor_p =
    ARDRONE_WLANMONITOR_SINGLETON::instance ();
  ACE_ASSERT (WLAN_monitor_p);
  WLAN_monitor_p->subscribe (this);
}

ARDrone_NavDataStream::~ARDrone_NavDataStream ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_NavDataStream::~ARDrone_NavDataStream"));

  ARDrone_WLANMonitor_t* WLAN_monitor_p =
    ARDRONE_WLANMONITOR_SINGLETON::instance ();
  ACE_ASSERT (WLAN_monitor_p);
  WLAN_monitor_p->unsubscribe (this);

  inherited::shutdown ();
}

bool
ARDrone_NavDataStream::load (Stream_ModuleList_t& modules_out,
                             bool& delete_out)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_NavDataStream::load"));

  Stream_Module_t* module_p = NULL;
  if (inherited::configuration_->configuration_.useReactor)
    ACE_NEW_RETURN (module_p,
                    ARDrone_Module_Controller_Module (this,
                                                      ACE_TEXT_ALWAYS_CHAR (MODULE_NET_TARGET_DEFAULT_NAME_STRING)),
                    false);
  else
    ACE_NEW_RETURN (module_p,
                    ARDrone_Module_AsynchController_Module (this,
                                                            ACE_TEXT_ALWAYS_CHAR (MODULE_NET_TARGET_DEFAULT_NAME_STRING)),
                    false);
  modules_out.push_back (module_p);
  module_p = NULL;
//  ACE_NEW_RETURN (module_p,
//                  ARDrone_Module_FileWriter_Module (ACE_TEXT_ALWAYS_CHAR ("FileWriter")),
//                  false);
//  modules_out.push_back (module_p);
//  module_p = NULL;
//#if defined (_DEBUG)
//  ACE_NEW_RETURN (module_p,
//                  ARDrone_Module_Dump_Module (this,
//                                              ACE_TEXT_ALWAYS_CHAR ("Dump")),
//                  false);
//  modules_out.push_back (module_p);
//  module_p = NULL;
//#endif
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_StatisticReport_Module (this,
                                                         ACE_TEXT_ALWAYS_CHAR (MODULE_STAT_REPORT_DEFAULT_NAME_STRING)),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_NavDataDecoder_Module (this,
                                                        ACE_TEXT_ALWAYS_CHAR (MODULE_MISC_PARSER_DEFAULT_NAME_STRING)),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
  if (inherited::configuration_->configuration_.useReactor)
    ACE_NEW_RETURN (module_p,
                    ARDrone_Module_UDPSource_Module (this,
                                                     ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING)),
                    false);
  else
    ACE_NEW_RETURN (module_p,
                    ARDrone_Module_AsynchUDPSource_Module (this,
                                                           ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING)),
                    false);
  modules_out.push_back (module_p);

  delete_out = true;

  return true;
}

bool
ARDrone_NavDataStream::initialize (const typename inherited::CONFIGURATION_T& configuration_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_NavDataStream::initialize"));

  if (inherited::isInitialized_)
  {
    isFirst_ = true;
  } // end IF

  bool setup_pipeline = configuration_in.configuration_.setupPipeline;
  bool reset_setup_pipeline = false;
//  struct ARDrone_SessionData* session_data_p = NULL;
  typename inherited::CONFIGURATION_T::ITERATOR_T iterator;
  struct ARDrone_ModuleHandlerConfiguration* configuration_p = NULL;
  Stream_Module_t* module_p = NULL;
  Common_ISetP_T<struct ARDrone_StreamState>* iset_p = NULL;

  // allocate a new session state, reset stream
  const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_.setupPipeline =
    false;
  reset_setup_pipeline = true;
  if (!inherited::initialize (configuration_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_Base_T::initialize(), aborting\n"),
                ACE_TEXT (navdata_stream_name_string_)));
    goto error;
  } // end IF
  const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_.setupPipeline =
    setup_pipeline;
  reset_setup_pipeline = false;

  //// sanity check(s)
  //ACE_ASSERT (inherited::sessionData_);
  { ACE_GUARD_RETURN (ACE_SYNCH_RECURSIVE_MUTEX, aGuard, inherited::lock_, false);
    inherited::state_.CBData = configuration_in.configuration_.CBData;
  } // end lock scope

//  session_data_p =
//    &const_cast<struct ARDrone_SessionData&> (inherited::sessionData_->getR ());
  iterator =
      const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).find (ACE_TEXT_ALWAYS_CHAR (""));

  // sanity check(s)
  ACE_ASSERT (iterator != configuration_in.end ());

  configuration_p =
      dynamic_cast<struct ARDrone_ModuleHandlerConfiguration*> (&((*iterator).second.second));

  // sanity check(s)
  ACE_ASSERT (configuration_p);
  ACE_ASSERT (configuration_p->subscribers);

  configuration_p->subscribers->push_back (this);
  configuration_p->subscribers->sort ();
  configuration_p->subscribers->unique (SUBSCRIBERS_IS_EQUAL_P ());

  // ---------------------------------------------------------------------------

  // ******************************** Source ***********************************
  module_p =
    const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING),
                                                   true,
                                                   false));
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to retrieve \"%s\" module handle, aborting\n"),
                ACE_TEXT (navdata_stream_name_string_),
                ACE_TEXT (MODULE_NET_SOURCE_DEFAULT_NAME_STRING)));
    goto error;
  } // end IF

  iset_p =
    dynamic_cast<Common_ISetP_T<struct ARDrone_StreamState>*> (module_p->writer ());
  ACE_ASSERT (iset_p);
  iset_p->setP (&(inherited::state_));

  // enqueue the module
  // *NOTE*: push()ing the module will open() it
  // --> set the argument that is passed along
  module_p->arg (inherited::sessionData_);

  // ---------------------------------------------------------------------------

  ACE_ASSERT (configuration_in.configuration_.initializeNavData);
  if (!configuration_in.configuration_.initializeNavData->initialize (this))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to initialize event handler, aborting\n"),
                ACE_TEXT (navdata_stream_name_string_)));
    goto error;
  } // end IF

  // ---------------------------------------------------------------------------

  if (configuration_in.configuration_.setupPipeline)
    if (!inherited::setup (NULL))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to set up pipeline, aborting\n"),
                  ACE_TEXT (navdata_stream_name_string_)));
      goto error;
    } // end IF

  inherited::isInitialized_ = true;
  //   inherited::dump_state();

  return true;

error:
  if (reset_setup_pipeline)
    const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_.setupPipeline =
      setup_pipeline;

  return false;
}

void
ARDrone_NavDataStream::start (Stream_SessionId_t sessionId_in,
                              const struct ARDrone_SessionData& sessionData_in)
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_NavDataStream::start"));

  ACE_UNUSED_ARG (sessionId_in);

  // sanity check(s)
  ACE_ASSERT (sessionData_in.state);
  if (sessionData_in.state->type != ARDRONE_STREAM_NAVDATA)
    return;

  ACE_ASSERT (inherited2::lock_);
  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, *inherited2::lock_);
    inherited2::inSession_ = true;
  } // end lock scope

  try {
    inherited2::startCB ();
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: caught exception in Stream_ISessionCB::startCB(), continuing\n"),
                ACE_TEXT (navdata_stream_name_string_)));
  }
}
void
ARDrone_NavDataStream::notify (Stream_SessionId_t sessionId_in,
                               const enum Stream_SessionMessageType& event_in)
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_NavDataStream::notify"));

  // sanity check(s)
  ACE_ASSERT (inherited::sessionData_);
  const struct ARDrone_SessionData& session_data_r =
      inherited::sessionData_->getR ();
  if (session_data_r.sessionId != sessionId_in)
    return;

  switch (event_in)
  {
    case STREAM_SESSION_MESSAGE_LINK:
    {
      // *NOTE*: there will be two 'link' messages; wait for the second one
      if (isFirst_)
      {
        isFirst_ = false;
        break;
      } // end IF

      // reset event dispatch notification for outbound data
      typename ARDrone_StreamConfiguration_t::ITERATOR_T iterator;
      ARDrone_StreamConnectionConfigurationIterator_t iterator_2;
      struct Net_UDPSocketConfiguration* socket_configuration_p = NULL;
      ARDrone_IConnectionManager_t* iconnection_manager_p =
          ARDRONE_CONNECTIONMANAGER_SINGLETON::instance ();
      ACE_ASSERT (iconnection_manager_p);
      typename ARDrone_IConnectionManager_t::CONNECTION_T* connection_p = NULL;
      ARDrone_IStreamConnection_t* istream_connection_p = NULL;
      Stream_IOutboundDataNotify* ioutbound_data_notify = NULL;
      std::string module_name_string;

      // sanity check(s)
      ACE_ASSERT (inherited::configuration_);

      iterator =
          inherited::configuration_->find (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_TARGET_DEFAULT_NAME_STRING));
      ACE_ASSERT (iterator != inherited::configuration_->end ());
      ACE_ASSERT ((*iterator).second.second.connectionConfigurations);
      iterator_2 =
          (*iterator).second.second.connectionConfigurations->find (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_TARGET_DEFAULT_NAME_STRING));
      ACE_ASSERT (iterator_2 != (*iterator).second.second.connectionConfigurations->end ());
      ACE_ASSERT ((*iterator_2).second.socketHandlerConfiguration.socketConfiguration);
      socket_configuration_p =
          dynamic_cast<struct Net_UDPSocketConfiguration*> ((*iterator_2).second.socketHandlerConfiguration.socketConfiguration);
      ACE_ASSERT (socket_configuration_p);
      connection_p =
          iconnection_manager_p->get (socket_configuration_p->peerAddress,
                                      true); // is peer address ?
      if (!connection_p)
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: failed to Net_IConnectionManager::get(0x%@), returning"),
                    ACE_TEXT (navdata_stream_name_string_),
                    ACE_TEXT (Net_Common_Tools::IPAddressToString (socket_configuration_p->peerAddress).c_str ())));
        return;
      } // end IF
      istream_connection_p =
          dynamic_cast<ARDrone_IStreamConnection_t*> (connection_p);
      ACE_ASSERT (istream_connection_p);
      ioutbound_data_notify =
          &const_cast<typename ARDrone_IStreamConnection_t::STREAM_T&> (istream_connection_p->stream ());
      try {
        if (!ioutbound_data_notify->initialize (connection_p->notification (),
                                                module_name_string))
          ACE_DEBUG ((LM_ERROR,
                      ACE_TEXT ("%s: failed to Stream_IOutboundDataNotify::initialize(0x%@,\"%s\"), returning\n"),
                      ACE_TEXT (navdata_stream_name_string_),
                      connection_p->notification (),
                      ACE_TEXT (module_name_string.c_str ())));
        return;
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: caught exception in Stream_IOutboundDataNotify::initialize(0x%@,\"%s\"), returning\n"),
                    ACE_TEXT (navdata_stream_name_string_),
                    connection_p->notification (),
                    ACE_TEXT (module_name_string.c_str ())));
        return;
      }

      break;
    }
    default:
      break;
  } // end SWITCH
}
void
ARDrone_NavDataStream::end (Stream_SessionId_t sessionId_in)
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_NavDataStream::end"));

  // sanity check(s)
  ACE_ASSERT (inherited::sessionData_);
  const struct ARDrone_SessionData& session_data_r =
      inherited::sessionData_->getR ();
  if (session_data_r.sessionId != sessionId_in)
    return;

  try {
    inherited2::endCB ();
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: caught exception in Stream_ISessionCB::endCB(), continuing\n"),
                ACE_TEXT (navdata_stream_name_string_)));
  }

  ACE_ASSERT (inherited2::lock_);
  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, *inherited2::lock_);
    inherited2::inSession_ = false;
  } // end lock scope
}

bool
ARDrone_NavDataStream::collect (struct ARDrone_Statistic& data_out)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_NavDataStream::collect"));

  int result = -1;
  struct ARDrone_SessionData& session_data_r =
      const_cast<struct ARDrone_SessionData&> (inherited::sessionData_->getR ());
  Stream_Module_t* module_p =
    const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR (MODULE_STAT_REPORT_DEFAULT_NAME_STRING),
                                                   true,
                                                   false));
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to retrieve \"%s\" module handle, aborting\n"),
                ACE_TEXT (navdata_stream_name_string_),
                ACE_TEXT (MODULE_STAT_REPORT_DEFAULT_NAME_STRING)));
    return false;
  } // end IF
  ARDrone_Module_Statistic_WriterTask_t* statistic_impl_p =
    dynamic_cast<ARDrone_Module_Statistic_WriterTask_t*> (module_p->writer ());
  if (!statistic_impl_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: dynamic_cast<ARDrone_Module_Statistic_WriterTask_t> failed, aborting\n"),
                ACE_TEXT (navdata_stream_name_string_)));
    return false;
  } // end IF

  // synch access
  if (session_data_r.lock)
  {
    result = session_data_r.lock->acquire ();
    if (result == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to ACE_SYNCH_MUTEX::acquire(): \"%m\", aborting\n"),
                  ACE_TEXT (navdata_stream_name_string_)));
      return false;
    } // end IF
  } // end IF

  session_data_r.statistic.timeStamp = COMMON_TIME_NOW;

  // delegate to the statistic module
  bool result_2 = false;
  try {
    result_2 = statistic_impl_p->collect (data_out);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: caught exception in Common_IStatistic_T::collect(), continuing\n"),
                ACE_TEXT (navdata_stream_name_string_)));
  }
  if (!result_2)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Common_IStatistic_T::collect(), aborting\n"),
                ACE_TEXT (navdata_stream_name_string_)));
  else
    session_data_r.statistic = data_out;

  if (session_data_r.lock)
  {
    result = session_data_r.lock->release ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_SYNCH_MUTEX::release(): \"%m\", continuing\n")));
  } // end IF

  return result_2;
}

void
ARDrone_NavDataStream::report () const
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_NavDataStream::report"));

  ACE_DEBUG ((LM_INFO,
              ACE_TEXT ("*** [session: %d] RUNTIME STATISTIC ***\n--> stream statistic @ %#D<--\n (data) messages: %u\n dropped messages: %u\n bytes total: %.0f\n*** RUNTIME STATISTIC ***\\END\n"),
              (inherited::state_.sessionData ? static_cast<int> (inherited::state_.sessionData->sessionId) : -1),
              &(inherited::state_.sessionData->lastCollectionTimeStamp),
              inherited::state_.sessionData->statistic.dataMessages,
              inherited::state_.sessionData->statistic.droppedFrames,
              inherited::state_.sessionData->statistic.bytes));
}

void
ARDrone_NavDataStream::messageCB (const struct _navdata_t& record_in,
                                  const ARDrone_NavDataOptionOffsets_t& offsets_in,
                                  void* payload_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_NavDataStream::messageCB"));

//#if defined (_DEBUG)
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
//#endif

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

        ACE_ASSERT (inherited::state_.CBData);
        { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, inherited::state_.CBData->lock);
          inherited::state_.CBData->openGLScene.orientation.x =
              option_2->phi; // roll (--> rotation along x)
          inherited::state_.CBData->openGLScene.orientation.y =
              option_2->psi; // yaw (--> rotation along y)
          inherited::state_.CBData->openGLScene.orientation.z =
              option_2->theta; // pitch (--> rotation along z)
        } // end lock scope

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
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("%s: invalid/unknown NavData option (was: %d), continuing\n"),
                    ACE_TEXT (navdata_stream_name_string_),
                    option_p->tag));
        break;
      }
    } // end SWITCH
  } // end FOR
}

const ARDrone_IController* const
ARDrone_NavDataStream::getP () const
{
  Stream_Module_t* module_p =
    const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_TARGET_DEFAULT_NAME_STRING),
                                                   true,
                                                   false));
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to retrieve \"%s\" module handle, aborting\n"),
                ACE_TEXT (navdata_stream_name_string_),
                ACE_TEXT (MODULE_NET_TARGET_DEFAULT_NAME_STRING)));
    return NULL;
  } // end IF

  ARDrone_IController* controller_p =
    dynamic_cast<ARDrone_IController*> (module_p->writer ());
  if (!controller_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s/%s: dynamic_cast<ARDrone_IController> failed, aborting\n"),
                ACE_TEXT (navdata_stream_name_string_),
                module_p->name ()));
    return NULL;
  } // end IF

  return controller_p;
}

//////////////////////////////////////////

void
#if defined (ACE_WIN32) || defined (ACE_WIN64)
ARDrone_NavDataStream::onAssociate (REFGUID interfaceIdentifier_in,
#else
ARDrone_NavDataStream::onAssociate (const std::string& interfaceIdentifier_in,
#endif
                                    const std::string& SSID_in,
                                    bool success_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_NavDataStream::onAssociate"));

  ACE_UNUSED_ARG (interfaceIdentifier_in);
  ACE_UNUSED_ARG (SSID_in);

  // sanity check(s)
  if (!inherited::configuration_)
    return;
  if (!success_in)
    return;

  // update GUI ?
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  // *TODO*: move this to onConnect() (see below)
  if (inherited::configuration_->configuration_.CBData)
  {
    guint event_source_id =
      g_idle_add (idle_associated_SSID_cb,
                  inherited::configuration_->configuration_.CBData);
    if (event_source_id == 0)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to g_idle_add(idle_associated_SSID_cb): \"%m\", returning\n"),
                  ACE_TEXT (navdata_stream_name_string_)));
      return;
    } // end IF
  } // end IF
#endif
}
void
#if defined (ACE_WIN32) || defined (ACE_WIN64)
ARDrone_NavDataStream::onConnect (REFGUID interfaceIdentifier_in,
#else
ARDrone_NavDataStream::onConnect (const std::string& interfaceIdentifier_in,
#endif
                                  const std::string& SSID_in,
                                  bool success_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_NavDataStream::onConnect"));

  ACE_UNUSED_ARG (interfaceIdentifier_in);
  ACE_UNUSED_ARG (SSID_in);

  // sanity check(s)
  if (!inherited::configuration_)
    return;
  if (!success_in)
    return;

  // debug info
  ACE_INET_Addr local_SAP, peer_SAP;
  ARDrone_WLANMonitor_t* wlan_monitor_p =
    ARDRONE_WLANMONITOR_SINGLETON::instance ();
  ACE_ASSERT (wlan_monitor_p);
  if (!Net_Common_Tools::interfaceToIPAddress (interfaceIdentifier_in,
                                               local_SAP,
                                               peer_SAP))
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_Common_Tools::interfaceToIPAddress(\"%s\"), returning\n"),
                ACE_TEXT (Net_Common_Tools::interfaceToString (interfaceIdentifier_in).c_str ())));
#else
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Net_Common_Tools::interfaceToIPAddress(\"%s\"), returning\n"),
                ACE_TEXT (interfaceIdentifier_in.c_str ())));
#endif
    return;
  } // end IF
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("\"%s\": connected to SSID %s: %s <---> %s\n"),
              ACE_TEXT (Net_Common_Tools::interfaceToString (interfaceIdentifier_in).c_str ()),
              ACE_TEXT (SSID_in.c_str ()),
              ACE_TEXT (Net_Common_Tools::IPAddressToString (local_SAP).c_str ()),
              ACE_TEXT (Net_Common_Tools::IPAddressToString (peer_SAP).c_str ())));
#else
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("\"%s\": connected to SSID %s: %s <---> %s\n"),
              ACE_TEXT (interfaceIdentifier_in.c_str ()),
              ACE_TEXT (SSID_in.c_str ()),
              ACE_TEXT (Net_Common_Tools::IPAddressToString (local_SAP).c_str ()),
              ACE_TEXT (Net_Common_Tools::IPAddressToString (peer_SAP).c_str ())));
#endif

  // update GUI ?
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
  if (inherited::configuration_->configuration_.CBData)
  {
    guint event_source_id =
      g_idle_add (idle_associated_SSID_cb,
                  inherited::configuration_->configuration_.CBData);
    if (event_source_id == 0)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to g_idle_add(idle_associated_SSID_cb): \"%m\", returning\n")));
      return;
    } // end IF
  } // end IF
#endif
}
void
#if defined (ACE_WIN32) || defined (ACE_WIN64)
ARDrone_NavDataStream::onHotPlug (REFGUID interfaceIdentifier_in,
#else
ARDrone_NavDataStream::onHotPlug (const std::string& interfaceIdentifier_in,
#endif
                                  bool enabled_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_NavDataStream::onHotPlug"));

//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("\"%s\": interface %s\n"),
//              ACE_TEXT (Net_Common_Tools::interfaceToString (interfaceIdentifier_in).c_str ()),
//              (enabled_in ? ACE_TEXT ("enabled") : ACE_TEXT ("disabled/removed"))));
//#else
//  ACE_DEBUG ((LM_DEBUG,
//              ACE_TEXT ("\"%s\": interface %s\n"),
//              ACE_TEXT (interfaceIdentifier_in.c_str ()),
//              (enabled_in ? ACE_TEXT ("enabled") : ACE_TEXT ("disabled/removed"))));
//#endif
}
void
#if defined (ACE_WIN32) || defined (ACE_WIN64)
ARDrone_NavDataStream::onScanComplete (REFGUID interfaceIdentifier_in)
#else
ARDrone_NavDataStream::onScanComplete (const std::string& interfaceIdentifier_in)
#endif
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_NavDataStream::onScanComplete"));

//  // sanity check(s)
//  if (!isInitialized_ || !isActive_)
//    return;
//  ACE_ASSERT (configuration_);
//  if (!configuration_->autoAssociate)
//    return;
//
//  if (!associate (interfaceIdentifier_in,
//                  configuration_->SSID))
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to Net_IWLANMonitor_T::associate(\"%s\",%s), returning\n"),
//                ACE_TEXT (Net_Common_Tools::interfaceToString (interfaceIdentifier_in).c_str ()),
//                ACE_TEXT (configuration_->SSID.c_str ())));
//#else
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to Net_IWLANMonitor_T::associate(\"%s\",%s), returning\n"),
//                ACE_TEXT (interfaceIdentifier_in.c_str ()),
//                ACE_TEXT (configuration_->SSID.c_str ())));
//#endif
}

//////////////////////////////////////////

ARDrone_MAVLinkStream::ARDrone_MAVLinkStream ()
 : inherited ()
 , inherited2 (&(inherited::sessionDataLock_))
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_MAVLinkStream::ARDrone_MAVLinkStream"));

  inherited::state_.type = ARDRONE_STREAM_MAVLINK;
}

bool
ARDrone_MAVLinkStream::load (Stream_ModuleList_t& modules_out,
                             bool& delete_out)
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_MAVLinkStream::load"));

  Stream_Module_t* module_p = NULL;
//  ACE_NEW_RETURN (module_p,
//                  ARDrone_Module_FileWriter_Module (this,
//                                                    ACE_TEXT_ALWAYS_CHAR ("FileWriter")),
//                  false);
//  modules_out.push_back (module_p);
//  module_p = NULL;
//#if defined (_DEBUG)
//  ACE_NEW_RETURN (module_p,
//                  ARDrone_Module_Dump_Module (this,
//                                              ACE_TEXT_ALWAYS_CHAR ("Dump")),
//                  false);
//  modules_out.push_back (module_p);
//  module_p = NULL;
//#endif
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_StatisticReport_Module (this,
                                                         ACE_TEXT_ALWAYS_CHAR (MODULE_STAT_REPORT_DEFAULT_NAME_STRING)),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_MAVLinkDecoder_Module (this,
                                                        ACE_TEXT_ALWAYS_CHAR (MODULE_MISC_PARSER_DEFAULT_NAME_STRING)),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
  if (inherited::configuration_->configuration_.useReactor)
    ACE_NEW_RETURN (module_p,
                    ARDrone_Module_UDPSource_Module (this,
                                                     ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING)),
                    false);
  else
    ACE_NEW_RETURN (module_p,
                    ARDrone_Module_AsynchUDPSource_Module (this,
                                                           ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING)),
                    false);
  modules_out.push_back (module_p);

  delete_out = true;

  return true;
}

bool
ARDrone_MAVLinkStream::initialize (const typename inherited::CONFIGURATION_T& configuration_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_MAVLinkStream::initialize"));

  if (inherited::isInitialized_)
  {
  } // end IF

//  bool result = false;
  bool setup_pipeline = configuration_in.configuration_.setupPipeline;
  bool reset_setup_pipeline = false;
//  struct ARDrone_SessionData* session_data_p = NULL;
  typename inherited::CONFIGURATION_T::ITERATOR_T iterator;
  struct ARDrone_ModuleHandlerConfiguration* configuration_p = NULL;
  Stream_Module_t* module_p = NULL;
  Common_ISetP_T<struct ARDrone_StreamState>* iset_p = NULL;

  // allocate a new session state, reset stream
  const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_.setupPipeline =
    false;
  reset_setup_pipeline = true;
  if (!inherited::initialize (configuration_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_Base_T::initialize(), aborting\n"),
                ACE_TEXT (mavlink_stream_name_string_)));
    goto error;
  } // end IF
  const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_.setupPipeline =
    setup_pipeline;
  reset_setup_pipeline = false;

  // sanity check(s)
  ACE_ASSERT (inherited::sessionData_);

  //session_data_p =
  //  &const_cast<struct ARDrone_SessionData&> (inherited::sessionData_->getR ());
  iterator =
      const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).find (ACE_TEXT_ALWAYS_CHAR (""));

  // sanity check(s)
  ACE_ASSERT (iterator != configuration_in.end ());

  configuration_p =
      dynamic_cast<struct ARDrone_ModuleHandlerConfiguration*> (&((*iterator).second.second));

  // sanity check(s)
  ACE_ASSERT (configuration_p);
  ACE_ASSERT (configuration_p->subscribers);

  configuration_p->subscribers->push_back (this);
  configuration_p->subscribers->sort ();
  configuration_p->subscribers->unique (SUBSCRIBERS_IS_EQUAL_P ());

  // ---------------------------------------------------------------------------

  // ***************************** Statistic **********************************
//  ARDrone_Module_Statistic_WriterTask_t* statistic_impl_p =
//    dynamic_cast<ARDrone_Module_Statistic_WriterTask_t*> (statistic_.writer ());
//  if (!statistic_impl_p)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("dynamic_cast<ARDrone_Module_RuntimeStatistic> failed, aborting\n")));
//    return false;
//  } // end IF
//  if (!statistic_impl_p->initialize (configuration_in.statisticReportingInterval,
//                                     configuration_in.messageAllocator))
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("%s: failed to initialize module writer, aborting\n"),
//                statistic_.name ()));
//    return false;
//  } // end IF

  // ******************************** Source ***********************************
  module_p =
    const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR (MODULE_NET_SOURCE_DEFAULT_NAME_STRING),
                                                   true,
                                                   false));
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to retrieve \"%s\" module handle, aborting\n"),
                ACE_TEXT (mavlink_stream_name_string_),
                ACE_TEXT (MODULE_NET_SOURCE_DEFAULT_NAME_STRING)));
    goto error;
  } // end IF

  iset_p =
    dynamic_cast<Common_ISetP_T<struct ARDrone_StreamState>*> (module_p->writer ());
  ACE_ASSERT (iset_p);
  iset_p->setP (&(inherited::state_));

  // enqueue the module
  // *NOTE*: push()ing the module will open() it
  // --> set the argument that is passed along
  module_p->arg (inherited::sessionData_);

  // ---------------------------------------------------------------------------

  ACE_ASSERT (configuration_in.configuration_.initializeMAVLink);
  if (!configuration_in.configuration_.initializeMAVLink->initialize (this))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to initialize event handler, aborting\n"),
                ACE_TEXT (mavlink_stream_name_string_)));
    goto error;
  } // end IF

  // ---------------------------------------------------------------------------

  if (configuration_in.configuration_.setupPipeline)
    if (!inherited::setup (NULL))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to set up pipeline, aborting\n"),
                  ACE_TEXT (mavlink_stream_name_string_)));
      goto error;
    } // end IF

  inherited::isInitialized_ = true;
  //   inherited::dump_state();

  return true;

error:
  if (reset_setup_pipeline)
    const_cast<typename inherited::CONFIGURATION_T&> (configuration_in).configuration_.setupPipeline =
      setup_pipeline;

  return false;
}

void
ARDrone_MAVLinkStream::start (Stream_SessionId_t sessionId_in,
                              const struct ARDrone_SessionData& sessionData_in)
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_MAVLinkStream::start"));

  ACE_UNUSED_ARG (sessionId_in);

  // sanity check(s)
  ACE_ASSERT (sessionData_in.state);
  if (sessionData_in.state->type != ARDRONE_STREAM_MAVLINK)
    return;

  ACE_ASSERT (inherited2::lock_);
  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, *inherited2::lock_);
    inherited2::inSession_ = true;
  } // end lock scope

  try {
    inherited2::startCB ();
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: caught exception in Stream_ISessionCB::startCB(), continuing\n"),
                ACE_TEXT (mavlink_stream_name_string_)));
  }
}
void
ARDrone_MAVLinkStream::notify (Stream_SessionId_t sessionId_in,
                               const enum Stream_SessionMessageType& event_in)
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_MAVLinkStream::notify"));

  // sanity check(s)
  ACE_ASSERT (inherited::sessionData_);
  const struct ARDrone_SessionData& session_data_r =
      inherited::sessionData_->getR ();
  if (session_data_r.sessionId != sessionId_in)
    return;

  ACE_UNUSED_ARG (event_in);
}
void
ARDrone_MAVLinkStream::end (Stream_SessionId_t sessionId_in)
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_MAVLinkStream::end"));

  // sanity check(s)
  ACE_ASSERT (inherited::sessionData_);
  const struct ARDrone_SessionData& session_data_r =
      inherited::sessionData_->getR ();
  if (session_data_r.sessionId != sessionId_in)
    return;

  try {
    inherited2::endCB ();
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: caught exception in Stream_ISessionCB::endCB(), continuing\n"),
                ACE_TEXT (mavlink_stream_name_string_)));
  }

  ACE_ASSERT (inherited2::lock_);
  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, *inherited2::lock_);
    inherited2::inSession_ = false;
  } // end lock scope
}

bool
ARDrone_MAVLinkStream::collect (struct ARDrone_Statistic& data_out)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_MAVLinkStream::collect"));

  int result = -1;
  struct ARDrone_SessionData& session_data_r =
      const_cast<struct ARDrone_SessionData&> (inherited::sessionData_->getR ());
  Stream_Module_t* module_p =
    const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR (MODULE_STAT_REPORT_DEFAULT_NAME_STRING),
                                                   true,
                                                   false));
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to retrieve \"%s\" module handle, aborting\n"),
                ACE_TEXT (mavlink_stream_name_string_),
                ACE_TEXT (MODULE_STAT_REPORT_DEFAULT_NAME_STRING)));
    return false;
  } // end IF
  ARDrone_Module_Statistic_WriterTask_t* statistic_impl_p =
    dynamic_cast<ARDrone_Module_Statistic_WriterTask_t*> (module_p->writer ());
  if (!statistic_impl_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: dynamic_cast<ARDrone_Module_Statistic_WriterTask_t> failed, aborting\n"),
                ACE_TEXT (mavlink_stream_name_string_)));
    return false;
  } // end IF

  // synch access
  if (session_data_r.lock)
  {
    result = session_data_r.lock->acquire ();
    if (result == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to ACE_SYNCH_MUTEX::acquire(): \"%m\", aborting\n"),
                  ACE_TEXT (mavlink_stream_name_string_)));
      return false;
    } // end IF
  } // end IF

  session_data_r.statistic.timeStamp = COMMON_TIME_NOW;

  // delegate to the statistic module
  bool result_2 = false;
  try {
    result_2 = statistic_impl_p->collect (data_out);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: caught exception in Common_IStatistic_T::collect(), continuing\n"),
                ACE_TEXT (mavlink_stream_name_string_)));
  }
  if (!result)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Common_IStatistic_T::collect(), aborting\n"),
                ACE_TEXT (mavlink_stream_name_string_)));
  else
    session_data_r.statistic = data_out;

  if (session_data_r.lock)
  {
    result = session_data_r.lock->release ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to ACE_SYNCH_MUTEX::release(): \"%m\", continuing\n"),
                  ACE_TEXT (mavlink_stream_name_string_)));
  } // end IF

  return result_2;
}

void
ARDrone_MAVLinkStream::report () const
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_MAVLinkStream::report"));

  ACE_DEBUG ((LM_INFO,
              ACE_TEXT ("*** [session: %u] RUNTIME STATISTIC ***\n--> stream statistic @ %#D<--\n (data) messages: %u\n dropped messages: %u\n bytes total: %.0f\n*** RUNTIME STATISTIC ***\\END\n"),
              (inherited::state_.sessionData ? static_cast<int> (inherited::state_.sessionData->sessionId) : -1),
              &(inherited::state_.sessionData->lastCollectionTimeStamp),
              inherited::state_.sessionData->statistic.dataMessages,
              inherited::state_.sessionData->statistic.droppedFrames,
              inherited::state_.sessionData->statistic.bytes));
}

void
ARDrone_MAVLinkStream::messageCB (const struct __mavlink_message& record_in,
                                  void* payload_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_MAVLinkStream::messageCB"));

  ACE_UNUSED_ARG (payload_in);

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
    { ACE_ASSERT (record_in.len == sizeof (struct __mavlink_gps_raw_int_t));
      struct __mavlink_gps_raw_int_t* message_p =
          reinterpret_cast<struct __mavlink_gps_raw_int_t*> (payload_in);
      ACE_UNUSED_ARG (message_p);
      break;
    }
    case MAVLINK_MSG_ID_ATTITUDE: // 30
    { ACE_ASSERT (record_in.len == sizeof (struct __mavlink_attitude_t));
      struct __mavlink_attitude_t* message_p =
          reinterpret_cast<struct __mavlink_attitude_t*> (payload_in);
      ACE_UNUSED_ARG (message_p);
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
                  ACE_TEXT (mavlink_stream_name_string_),
                  record_in.msgid));
      break;
    }
  } // end SWITCH
}
