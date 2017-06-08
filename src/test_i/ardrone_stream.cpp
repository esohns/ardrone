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

#include "ardrone_modules_common.h"

ARDrone_ControlStream::ARDrone_ControlStream ()
 : inherited (ACE_TEXT_ALWAYS_CHAR ("ControlStream"))
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_ControlStream::ARDrone_ControlStream"));

}

ARDrone_ControlStream::~ARDrone_ControlStream ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_ControlStream::~ARDrone_ControlStream"));

  inherited::shutdown ();
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
//                  ARDrone_Module_FileWriter_Module (ACE_TEXT_ALWAYS_CHAR ("FileWriter"),
//                                                    NULL,
//                                                    false),
//                  false);
//  modules_out.push_back (module_p);
//  module_p = NULL;
#if defined (_DEBUG)
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_Dump_Module (this,
                                              ACE_TEXT_ALWAYS_CHAR ("Dump"),
                                              NULL,
                                              false),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
#endif
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_StatisticReport_Module (this,
                                                         ACE_TEXT_ALWAYS_CHAR ("StatisticReport"),
                                                         NULL,
                                                         false),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
//  ACE_NEW_RETURN (module_p,
//                  ARDrone_Module_ControlDecoder_Module (ACE_TEXT_ALWAYS_CHAR ("ControlDecoder"),
//                                                        NULL,
//                                                        false),
//                  false);
//  modules_out.push_back (module_p);
//  module_p = NULL;
  if (inherited::configuration_->useReactor)
    ACE_NEW_RETURN (module_p,
                    ARDrone_Module_TCPSource_Module (this,
                                                     ACE_TEXT_ALWAYS_CHAR ("ControlSource"),
                                                     NULL,
                                                     false),
                    false);
  else
    ACE_NEW_RETURN (module_p,
                    ARDrone_Module_AsynchTCPSource_Module (this,
                                                           ACE_TEXT_ALWAYS_CHAR ("ControlSource"),
                                                           NULL,
                                                           false),
                    false);
  modules_out.push_back (module_p);

  delete_out = true;

  return true;
}

bool
ARDrone_ControlStream::initialize (const struct ARDrone_StreamConfiguration& configuration_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_ControlStream::initialize"));

  if (inherited::isInitialized_)
  {
  } // end IF

  bool result = false;
  bool setup_pipeline = configuration_in.setupPipeline;
  bool reset_setup_pipeline = false;

  // allocate a new session state, reset stream
  const_cast<struct ARDrone_StreamConfiguration&> (configuration_in).setupPipeline =
    false;
  reset_setup_pipeline = true;
  if (!inherited::initialize (configuration_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_Base_T::initialize(), aborting\n"),
                ACE_TEXT (inherited::name_.c_str ())));
    return false;
  } // end IF
  const_cast<struct ARDrone_StreamConfiguration&> (configuration_in).setupPipeline =
    setup_pipeline;
  reset_setup_pipeline = false;
  ACE_ASSERT (inherited::sessionData_);

  // things to be done here:
  // - create modules (done for the ones "owned" by the stream itself)
  // - initialize modules
  // - push them onto the stream (tail-first)
  struct ARDrone_SessionData& session_data_r =
    const_cast<struct ARDrone_SessionData&> (inherited::sessionData_->get ());
  session_data_r.sessionID = configuration_in.sessionID;
  //  ACE_ASSERT (configuration_in.moduleConfiguration);
  //  configuration_in.moduleConfiguration->streamState = &inherited::state_;
//  ARDrone_ModuleHandlerConfigurationsIterator_t iterator =
//      const_cast<struct ARDrone_StreamConfiguration&> (configuration_in).moduleHandlerConfigurations.find (ACE_TEXT_ALWAYS_CHAR (""));
//  ACE_ASSERT (iterator != configuration_in.moduleHandlerConfigurations.end ());
//  struct ARDrone_ModuleHandlerConfiguration* configuration_p =
//      dynamic_cast<struct ARDrone_ModuleHandlerConfiguration*> ((*iterator).second);
//  ACE_ASSERT (configuration_p);
//  ACE_ASSERT (configuration_p->subscribers);
//  configuration_p->subscribers->push_back (this);

  // ---------------------------------------------------------------------------

  Stream_Module_t* module_p = NULL;

  // ******************************** Source ***********************************
  module_p =
    const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR ("ControlSource")));
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to retrieve \"%s\" module handle, aborting\n"),
                ACE_TEXT ("ControlSource")));
    return false;
  } // end IF

  if (configuration_in.useReactor)
  {
    ARDrone_Module_TCPSource* sourceWriter_impl_p =
        dynamic_cast<ARDrone_Module_TCPSource*> (module_p->writer ());
    if (!sourceWriter_impl_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("dynamic_cast<ARDrone_Module_TCPSource> failed, aborting\n")));
      return false;
    } // end IF
    sourceWriter_impl_p->set (&(inherited::state_));
  } // end IF
  else
  {
    ARDrone_Module_AsynchTCPSource* asynchSourceWriter_impl_p =
        dynamic_cast<ARDrone_Module_AsynchTCPSource*> (module_p->writer ());
    if (!asynchSourceWriter_impl_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("dynamic_cast<ARDrone_Module_AsynchTCPSource> failed, aborting\n")));
      return false;
    } // end IF
    asynchSourceWriter_impl_p->set (&(inherited::state_));
  } // end ELSE

  // enqueue the module
  // *NOTE*: push()ing the module will open() it
  // --> set the argument that is passed along
  module_p->arg (inherited::sessionData_);

  // ---------------------------------------------------------------------------

  if (configuration_in.setupPipeline)
    if (!inherited::setup (NULL))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to setup pipeline, aborting\n")));
      goto error;
    } // end IF

  inherited::isInitialized_ = true;
  //   inherited::dump_state();

  return true;

error:
  if (reset_setup_pipeline)
    const_cast<struct ARDrone_StreamConfiguration&> (configuration_in).setupPipeline =
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
      inherited::sessionData_->get ();
  if (!session_data_r.sessionID != sessionId_in)
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
      inherited::sessionData_->get ();
  if (!session_data_r.sessionID != sessionId_in)
    return;
}

void
ARDrone_ControlStream::ping ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_ControlStream::ping"));

//  Net_Module_ProtocolHandler* protocolHandler_impl = NULL;
//  protocolHandler_impl = dynamic_cast<Net_Module_ProtocolHandler*> (protocolHandler_.writer ());
//  if (!protocolHandler_impl)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("dynamic_cast<Net_Module_ProtocolHandler> failed, returning\n")));

//    return;
//  } // end IF

//  // delegate to this module
//  protocolHandler_impl->handleTimeout (NULL);

  ACE_ASSERT (false);
  ACE_NOTSUP;
  ACE_NOTREACHED (return;)
}

bool
ARDrone_ControlStream::collect (ARDrone_RuntimeStatistic_t& data_out)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_ControlStream::collect"));

  int result = -1;
  struct ARDrone_SessionData& session_data_r =
      const_cast<struct ARDrone_SessionData&> (inherited::sessionData_->get ());
  Stream_Module_t* module_p =
    const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR ("StatisticReport")));
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to retrieve \"%s\" module handle, aborting\n"),
                ACE_TEXT ("StatisticReport")));
    return false;
  } // end IF
  ARDrone_Module_Statistic_WriterTask_t* statistic_impl_p =
    dynamic_cast<ARDrone_Module_Statistic_WriterTask_t*> (module_p->writer ());
  if (!statistic_impl_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<ARDrone_Module_Statistic_WriterTask_t> failed, aborting\n")));
    return false;
  } // end IF

  // synch access
  if (session_data_r.lock)
  {
    result = session_data_r.lock->acquire ();
    if (result == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_SYNCH_MUTEX::acquire(): \"%m\", aborting\n")));
      return false;
    } // end IF
  } // end IF

  session_data_r.currentStatistic.timeStamp = COMMON_TIME_NOW;

  // delegate to the statistic module
  bool result_2 = false;
  try {
    result_2 = statistic_impl_p->collect (data_out);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Common_IStatistic_T::collect(), continuing\n")));
  }
  if (!result_2)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_IStatistic_T::collect(), aborting\n")));
  else
    session_data_r.currentStatistic = data_out;

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
ARDrone_ControlStream::report () const
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_ControlStream::report"));

  ACE_ASSERT (inherited::state_.currentSessionData);

  ACE_DEBUG ((LM_INFO,
              ACE_TEXT ("*** [session: %u] RUNTIME STATISTIC ***\n--> Stream Statistic @ %#D<--\n (data) messages: %u\n dropped messages: %u\n bytes total: %.0f\n*** RUNTIME STATISTICS ***\\END\n"),
              inherited::state_.currentSessionData->sessionID,
              &(inherited::state_.currentSessionData->lastCollectionTimeStamp),
              inherited::state_.currentSessionData->currentStatistic.dataMessages,
              inherited::state_.currentSessionData->currentStatistic.droppedFrames,
              inherited::state_.currentSessionData->currentStatistic.bytes));
}

//////////////////////////////////////////

ARDrone_NavDataStream::ARDrone_NavDataStream ()
 : inherited (ACE_TEXT_ALWAYS_CHAR ("NavDataStream"))
 , inherited2 (&(inherited::sessionDataLock_))
 , controller_ (NULL)
 , videoModeSet_ (false)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_NavDataStream::ARDrone_NavDataStream"));

}

ARDrone_NavDataStream::~ARDrone_NavDataStream ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_NavDataStream::~ARDrone_NavDataStream"));

  inherited::shutdown ();
}

bool
ARDrone_NavDataStream::load (Stream_ModuleList_t& modules_out,
                             bool& delete_out)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_NavDataStream::load"));

  // sanity check(s)
  ACE_ASSERT (inherited::configuration_);

  Stream_Module_t* module_p = NULL;
  if (inherited::configuration_->useReactor)
    ACE_NEW_RETURN (module_p,
                    ARDrone_Module_Controller_Module (this,
                                                      ACE_TEXT_ALWAYS_CHAR ("Controller"),
                                                      NULL,
                                                      false),
                    false);
  else
    ACE_NEW_RETURN (module_p,
                    ARDrone_Module_AsynchController_Module (this,
                                                            ACE_TEXT_ALWAYS_CHAR ("Controller"),
                                                            NULL,
                                                            false),
                    false);
  modules_out.push_back (module_p);
  module_p = NULL;
//  ACE_NEW_RETURN (module_p,
//                  ARDrone_Module_FileWriter_Module (ACE_TEXT_ALWAYS_CHAR ("FileWriter"),
//                                                    NULL,
//                                                    false),
//                  false);
//  modules_out.push_back (module_p);
//  module_p = NULL;
#if defined (_DEBUG)
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_Dump_Module (this,
                                              ACE_TEXT_ALWAYS_CHAR ("Dump"),
                                              NULL,
                                              false),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
#endif
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_StatisticReport_Module (this,
                                                         ACE_TEXT_ALWAYS_CHAR ("StatisticReport"),
                                                         NULL,
                                                         false),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_NavDataDecoder_Module (this,
                                                        ACE_TEXT_ALWAYS_CHAR ("NavDataDecoder"),
                                                        NULL,
                                                        false),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
  if (inherited::configuration_->useReactor)
    ACE_NEW_RETURN (module_p,
                    ARDrone_Module_UDPSource_Module (this,
                                                     ACE_TEXT_ALWAYS_CHAR ("NavDataSource"),
                                                     NULL,
                                                     false),
                    false);
  else
    ACE_NEW_RETURN (module_p,
                    ARDrone_Module_AsynchUDPSource_Module (this,
                                                           ACE_TEXT_ALWAYS_CHAR ("NavDataSource"),
                                                           NULL,
                                                           false),
                    false);
  modules_out.push_back (module_p);

  delete_out = true;

  return true;
}

bool
ARDrone_NavDataStream::initialize (const struct ARDrone_StreamConfiguration& configuration_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_NavDataStream::initialize"));

  if (inherited::isInitialized_)
  {
    controller_ = NULL;
    videoModeSet_ = NULL;
  } // end IF

  bool result = false;
  bool setup_pipeline = configuration_in.setupPipeline;
  bool reset_setup_pipeline = false;
  struct ARDrone_SessionData* session_data_p = NULL;
  ARDrone_ModuleHandlerConfigurationsIterator_t iterator;
  struct ARDrone_ModuleHandlerConfiguration* configuration_p = NULL;
  Stream_Module_t* module_p = NULL;

  // allocate a new session state, reset stream
  const_cast<struct ARDrone_StreamConfiguration&> (configuration_in).setupPipeline =
    false;
  reset_setup_pipeline = true;
  if (!inherited::initialize (configuration_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_Base_T::initialize(), aborting\n"),
                ACE_TEXT (inherited::name_.c_str ())));
    goto error;
  } // end IF
  const_cast<struct ARDrone_StreamConfiguration&> (configuration_in).setupPipeline =
    setup_pipeline;
  reset_setup_pipeline = false;
  ACE_ASSERT (inherited::sessionData_);

  // things to be done here:
  // - create modules (done for the ones "owned" by the stream itself)
  // - initialize modules
  // - push them onto the stream (tail-first)
  session_data_p =
    &const_cast<struct ARDrone_SessionData&> (inherited::sessionData_->get ());
  session_data_p->isNavData = true;
  session_data_p->sessionID = configuration_in.sessionID;
  //  ACE_ASSERT (configuration_in.moduleConfiguration);
  //  configuration_in.moduleConfiguration->streamState = &inherited::state_;
  iterator =
      const_cast<struct ARDrone_StreamConfiguration&> (configuration_in).moduleHandlerConfigurations.find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (iterator != configuration_in.moduleHandlerConfigurations.end ());
  configuration_p =
      dynamic_cast<struct ARDrone_ModuleHandlerConfiguration*> (&((*iterator).second));
  ACE_ASSERT (configuration_p);
  ACE_ASSERT (configuration_p->subscribers);
  configuration_p->subscribers->push_back (this);

  // ---------------------------------------------------------------------------

  // ******************************** Source ***********************************
  module_p =
    const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR ("NavDataSource")));
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to retrieve \"%s\" module handle, aborting\n"),
                ACE_TEXT (inherited::name_.c_str ()),
                ACE_TEXT ("NavDataSource")));
    goto error;
  } // end IF

  if (configuration_in.useReactor)
  {
    ARDrone_Module_UDPSource* sourceWriter_impl_p =
        dynamic_cast<ARDrone_Module_UDPSource*> (module_p->writer ());
    if (!sourceWriter_impl_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: dynamic_cast<ARDrone_Module_UDPSource> failed, aborting\n"),
                  ACE_TEXT (inherited::name_.c_str ())));
      goto error;
    } // end IF
    sourceWriter_impl_p->set (&(inherited::state_));
  } // end IF
  else
  {
    ARDrone_Module_AsynchUDPSource* asynchSourceWriter_impl_p =
        dynamic_cast<ARDrone_Module_AsynchUDPSource*> (module_p->writer ());
    if (!asynchSourceWriter_impl_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: dynamic_cast<ARDrone_Module_AsynchUDPSource> failed, aborting\n"),
                  ACE_TEXT (inherited::name_.c_str ())));
      goto error;
    } // end IF
    asynchSourceWriter_impl_p->set (&(inherited::state_));
  } // end ELSE

  // enqueue the module
  // *NOTE*: push()ing the module will open() it
  // --> set the argument that is passed along
  module_p->arg (inherited::sessionData_);

  // **************************** Controller ***********************************
  module_p =
    const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR ("Controller")));
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to retrieve \"%s\" module handle, aborting\n"),
                ACE_TEXT (inherited::name_.c_str ()),
                ACE_TEXT ("Controller")));
    goto error;
  } // end IF

  controller_ =
    dynamic_cast<ARDrone_IController*> (module_p->writer ());
  if (!controller_)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: dynamic_cast<ARDrone_IController> failed, aborting\n"),
                ACE_TEXT (inherited::name_.c_str ())));
    goto error;
  } // end IF

  ACE_ASSERT (configuration_in.initializeNavData);
  if (!configuration_in.initializeNavData->initialize (this))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to initialize event handler, aborting\n"),
                ACE_TEXT (inherited::name_.c_str ())));
    goto error;
  } // end IF

  // ---------------------------------------------------------------------------

  if (configuration_in.setupPipeline)
    if (!inherited::setup (NULL))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to set up pipeline, aborting\n"),
                  ACE_TEXT (inherited::name_.c_str ())));
      goto error;
    } // end IF

  inherited::isInitialized_ = true;
  //   inherited::dump_state();

  return true;

error:
  if (reset_setup_pipeline)
    const_cast<struct ARDrone_StreamConfiguration&> (configuration_in).setupPipeline =
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
  if (!sessionData_in.isNavData)
    return;

  ACE_ASSERT (inherited2::lock_);
  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, *inherited2::lock_);

    inSession_ = true;
  } // end lock scope

  try {
    inherited2::startCB ();
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Stream_ISessionCB::startCB(), continuing\n")));
  }

//  if (!videoModeSet_)
//  { ACE_ASSERT (controller_);
//    try {
//      controller_->set (ARDRONE_VIDEOMODE_720P);
//    } catch (...) {
//      ACE_DEBUG ((LM_ERROR,
//                  ACE_TEXT ("caught exception in ARDrone_IController::set(%d), returning\n"),
//                  ARDRONE_VIDEOMODE_720P));
//      return;
//    }

//    videoModeSet_ = true;
//  } // end IF

  try {
    controller_->init ();
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in ARDrone_IController::init(), returning\n")));
    return;
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
      inherited::sessionData_->get ();
  if (!session_data_r.sessionID != sessionId_in)
    return;

  ACE_UNUSED_ARG (event_in);
}
void
ARDrone_NavDataStream::end (Stream_SessionId_t sessionId_in)
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_NavDataStream::end"));

  // sanity check(s)
  ACE_ASSERT (inherited::sessionData_);
  const struct ARDrone_SessionData& session_data_r =
      inherited::sessionData_->get ();
  if (!session_data_r.sessionID != sessionId_in)
    return;

  try {
    inherited2::endCB ();
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Stream_ISessionCB::endCB(), continuing\n")));
  }

  ACE_ASSERT (inherited2::lock_);
  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, *inherited2::lock_);

    inSession_ = false;
  } // end lock scope
}

void
ARDrone_NavDataStream::ping ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_NavDataStream::ping"));

//  Net_Module_ProtocolHandler* protocolHandler_impl = NULL;
//  protocolHandler_impl = dynamic_cast<Net_Module_ProtocolHandler*> (protocolHandler_.writer ());
//  if (!protocolHandler_impl)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("dynamic_cast<Net_Module_ProtocolHandler> failed, returning\n")));

//    return;
//  } // end IF

//  // delegate to this module
//  protocolHandler_impl->handleTimeout (NULL);

  ACE_ASSERT (false);
  ACE_NOTSUP;
  ACE_NOTREACHED (return;)
}

bool
ARDrone_NavDataStream::collect (ARDrone_RuntimeStatistic_t& data_out)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_NavDataStream::collect"));

  int result = -1;
  struct ARDrone_SessionData& session_data_r =
      const_cast<struct ARDrone_SessionData&> (inherited::sessionData_->get ());
  Stream_Module_t* module_p =
    const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR ("StatisticReport")));
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to retrieve \"%s\" module handle, aborting\n"),
                ACE_TEXT ("StatisticReport")));
    return false;
  } // end IF
  ARDrone_Module_Statistic_WriterTask_t* statistic_impl_p =
    dynamic_cast<ARDrone_Module_Statistic_WriterTask_t*> (module_p->writer ());
  if (!statistic_impl_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<ARDrone_Module_Statistic_WriterTask_t> failed, aborting\n")));
    return false;
  } // end IF

  // synch access
  if (session_data_r.lock)
  {
    result = session_data_r.lock->acquire ();
    if (result == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_SYNCH_MUTEX::acquire(): \"%m\", aborting\n")));
      return false;
    } // end IF
  } // end IF

  session_data_r.currentStatistic.timeStamp = COMMON_TIME_NOW;

  // delegate to the statistic module
  bool result_2 = false;
  try {
    result_2 = statistic_impl_p->collect (data_out);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Common_IStatistic_T::collect(), continuing\n")));
  }
  if (!result_2)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_IStatistic_T::collect(), aborting\n")));
  else
    session_data_r.currentStatistic = data_out;

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

  ACE_ASSERT (inherited::state_.currentSessionData);

  ACE_DEBUG ((LM_INFO,
              ACE_TEXT ("*** [session: %u] RUNTIME STATISTIC ***\n--> Stream Statistic @ %#D<--\n (data) messages: %u\n dropped messages: %u\n bytes total: %.0f\n*** RUNTIME STATISTICS ***\\END\n"),
              inherited::state_.currentSessionData->sessionID,
              &(inherited::state_.currentSessionData->lastCollectionTimeStamp),
              inherited::state_.currentSessionData->currentStatistic.dataMessages,
              inherited::state_.currentSessionData->currentStatistic.droppedFrames,
              inherited::state_.currentSessionData->currentStatistic.bytes));
}

void
ARDrone_NavDataStream::messageCB (const struct _navdata_t& record_in,
                                  const ARDrone_NavDataMessageOptionOffsets_t& offsets_in,
                                  void* payload_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_NavDataStream::messageCB"));

  ACE_ASSERT (false);
  ACE_NOTSUP;
  ACE_NOTREACHED (return;)
}

//////////////////////////////////////////

ARDrone_MAVLinkStream::ARDrone_MAVLinkStream ()
 : inherited (ACE_TEXT_ALWAYS_CHAR ("MAVLinkStream"))
 , inherited2 (&(inherited::sessionDataLock_))
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_MAVLinkStream::ARDrone_MAVLinkStream"));

}

ARDrone_MAVLinkStream::~ARDrone_MAVLinkStream ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_MAVLinkStream::~ARDrone_MAVLinkStream"));

  inherited::shutdown ();
}

bool
ARDrone_MAVLinkStream::load (Stream_ModuleList_t& modules_out,
                             bool& delete_out)
{
  STREAM_TRACE (ACE_TEXT ("ARDrone_MAVLinkStream::load"));

  // sanity check(s)
  ACE_ASSERT (inherited::configuration_);

  Stream_Module_t* module_p = NULL;
//  ACE_NEW_RETURN (module_p,
//                  ARDrone_Module_FileWriter_Module (ACE_TEXT_ALWAYS_CHAR ("FileWriter"),
//                                                    NULL,
//                                                    false),
//                  false);
//  modules_out.push_back (module_p);
//  module_p = NULL;
//#if defined (_DEBUG)
//  ACE_NEW_RETURN (module_p,
//                  ARDrone_Module_Dump_Module (ACE_TEXT_ALWAYS_CHAR ("Dump"),
//                                              NULL,
//                                              false),
//                  false);
//  modules_out.push_back (module_p);
//  module_p = NULL;
//#endif
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_StatisticReport_Module (this,
                                                         ACE_TEXT_ALWAYS_CHAR ("StatisticReport"),
                                                         NULL,
                                                         false),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_MAVLinkDecoder_Module (this,
                                                        ACE_TEXT_ALWAYS_CHAR ("MAVLinkDecoder"),
                                                        NULL,
                                                        false),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
  if (inherited::configuration_->useReactor)
    ACE_NEW_RETURN (module_p,
                    ARDrone_Module_UDPSource_Module (this,
                                                     ACE_TEXT_ALWAYS_CHAR ("MAVLinkSource"),
                                                     NULL,
                                                     false),
                    false);
  else
    ACE_NEW_RETURN (module_p,
                    ARDrone_Module_AsynchUDPSource_Module (this,
                                                           ACE_TEXT_ALWAYS_CHAR ("MAVLinkSource"),
                                                           NULL,
                                                           false),
                    false);
  modules_out.push_back (module_p);

  delete_out = true;

  return true;
}

bool
ARDrone_MAVLinkStream::initialize (const struct ARDrone_StreamConfiguration& configuration_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_MAVLinkStream::initialize"));

  if (inherited::isInitialized_)
  {
  } // end IF

  bool result = false;
  bool setup_pipeline = configuration_in.setupPipeline;
  bool reset_setup_pipeline = false;
  struct ARDrone_SessionData* session_data_p = NULL;
  ARDrone_ModuleHandlerConfigurationsIterator_t iterator;
  struct ARDrone_ModuleHandlerConfiguration* configuration_p = NULL;
  Stream_Module_t* module_p = NULL;

  // allocate a new session state, reset stream
  const_cast<struct ARDrone_StreamConfiguration&> (configuration_in).setupPipeline =
    false;
  reset_setup_pipeline = true;
  if (!inherited::initialize (configuration_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_Base_T::initialize(), aborting\n"),
                ACE_TEXT (inherited::name_.c_str ())));
    goto error;
  } // end IF
  const_cast<struct ARDrone_StreamConfiguration&> (configuration_in).setupPipeline =
    setup_pipeline;
  reset_setup_pipeline = false;
  ACE_ASSERT (inherited::sessionData_);

  // things to be done here:
  // - create modules (done for the ones "owned" by the stream itself)
  // - initialize modules
  // - push them onto the stream (tail-first)
  session_data_p =
    &const_cast<struct ARDrone_SessionData&> (inherited::sessionData_->get ());
  //session_data_r.sessionID = configuration_in.sessionID;
  iterator =
      const_cast<struct ARDrone_StreamConfiguration&> (configuration_in).moduleHandlerConfigurations.find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (iterator != configuration_in.moduleHandlerConfigurations.end ());
  configuration_p =
      dynamic_cast<struct ARDrone_ModuleHandlerConfiguration*> (&((*iterator).second));
  ACE_ASSERT (configuration_p);
  ACE_ASSERT (configuration_p->subscribers);
  configuration_p->subscribers->push_back (this);

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
    const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR ("MAVLinkSource")));
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to retrieve \"%s\" module handle, aborting\n"),
                ACE_TEXT (inherited::name_.c_str ()),
                ACE_TEXT ("MAVLinkSource")));
    goto error;
  } // end IF

  if (configuration_in.useReactor)
  {
    ARDrone_Module_UDPSource* sourceWriter_impl_p =
        dynamic_cast<ARDrone_Module_UDPSource*> (module_p->writer ());
    if (!sourceWriter_impl_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: dynamic_cast<ARDrone_Module_UDPSource> failed, aborting\n"),
                  ACE_TEXT (inherited::name_.c_str ())));
      goto error;
    } // end IF
    sourceWriter_impl_p->set (&(inherited::state_));
  } // end IF
  else
  {
    ARDrone_Module_AsynchUDPSource* asynchSourceWriter_impl_p =
        dynamic_cast<ARDrone_Module_AsynchUDPSource*> (module_p->writer ());
    if (!asynchSourceWriter_impl_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: dynamic_cast<ARDrone_Module_AsynchUDPSource> failed, aborting\n"),
                  ACE_TEXT (inherited::name_.c_str ())));
      goto error;
    } // end IF
    asynchSourceWriter_impl_p->set (&(inherited::state_));
  } // end ELSE

  // enqueue the module
  // *NOTE*: push()ing the module will open() it
  // --> set the argument that is passed along
  module_p->arg (inherited::sessionData_);

  // -------------------------------------------------------------

  ACE_ASSERT (configuration_in.initializeMAVLink);
  if (!configuration_in.initializeMAVLink->initialize (this))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to initialize event handler, aborting\n"),
                ACE_TEXT (inherited::name_.c_str ())));
    goto error;
  } // end IF

  // ---------------------------------------------------------------------------

  if (configuration_in.setupPipeline)
    if (!inherited::setup (NULL))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to set up pipeline, aborting\n"),
                  ACE_TEXT (inherited::name_.c_str ())));
      goto error;
    } // end IF

  inherited::isInitialized_ = true;
  //   inherited::dump_state();

  return true;

error:
  if (reset_setup_pipeline)
    const_cast<struct ARDrone_StreamConfiguration&> (configuration_in).setupPipeline =
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
  if (sessionData_in.isNavData)
    return;

  ACE_ASSERT (inherited2::lock_);
  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, *inherited2::lock_);

    inSession_ = true;
  } // end lock scope

  try {
    inherited2::startCB ();
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Stream_ISessionCB::startCB(), continuing\n")));
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
      inherited::sessionData_->get ();
  if (!session_data_r.sessionID != sessionId_in)
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
      inherited::sessionData_->get ();
  if (!session_data_r.sessionID != sessionId_in)
    return;

  try {
    inherited2::endCB ();
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Stream_ISessionCB::endCB(), continuing\n")));
  }

  ACE_ASSERT (inherited2::lock_);
  { ACE_GUARD (ACE_SYNCH_MUTEX, aGuard, *inherited2::lock_);

    inSession_ = false;
  } // end lock scope
}

void
ARDrone_MAVLinkStream::ping ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_MAVLinkStream::ping"));

//  Net_Module_ProtocolHandler* protocolHandler_impl = NULL;
//  protocolHandler_impl = dynamic_cast<Net_Module_ProtocolHandler*> (protocolHandler_.writer ());
//  if (!protocolHandler_impl)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("dynamic_cast<Net_Module_ProtocolHandler> failed, returning\n")));

//    return;
//  } // end IF

//  // delegate to this module
//  protocolHandler_impl->handleTimeout (NULL);

  ACE_ASSERT (false);
  ACE_NOTSUP;
  ACE_NOTREACHED (return;)
}

bool
ARDrone_MAVLinkStream::collect (ARDrone_RuntimeStatistic_t& data_out)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_MAVLinkStream::collect"));

  int result = -1;
  struct ARDrone_SessionData& session_data_r =
      const_cast<struct ARDrone_SessionData&> (inherited::sessionData_->get ());
  Stream_Module_t* module_p =
    const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR ("StatisticReport")));
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to retrieve \"%s\" module handle, aborting\n"),
                ACE_TEXT ("StatisticReport")));
    return false;
  } // end IF
  ARDrone_Module_Statistic_WriterTask_t* statistic_impl_p =
    dynamic_cast<ARDrone_Module_Statistic_WriterTask_t*> (module_p->writer ());
  if (!statistic_impl_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<ARDrone_Module_Statistic_WriterTask_t> failed, aborting\n")));
    return false;
  } // end IF

  // synch access
  if (session_data_r.lock)
  {
    result = session_data_r.lock->acquire ();
    if (result == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_SYNCH_MUTEX::acquire(): \"%m\", aborting\n")));
      return false;
    } // end IF
  } // end IF

  session_data_r.currentStatistic.timeStamp = COMMON_TIME_NOW;

  // delegate to the statistic module
  bool result_2 = false;
  try {
    result_2 = statistic_impl_p->collect (data_out);
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in Common_IStatistic_T::collect(), continuing\n")));
  }
  if (!result)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Common_IStatistic_T::collect(), aborting\n")));
  else
    session_data_r.currentStatistic = data_out;

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
ARDrone_MAVLinkStream::report () const
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_MAVLinkStream::report"));

  ACE_ASSERT (inherited::state_.currentSessionData);

  ACE_DEBUG ((LM_INFO,
              ACE_TEXT ("*** [session: %u] RUNTIME STATISTIC ***\n--> Stream Statistic @ %#D<--\n (data) messages: %u\n dropped messages: %u\n bytes total: %.0f\n*** RUNTIME STATISTICS ***\\END\n"),
              inherited::state_.currentSessionData->sessionID,
              &(inherited::state_.currentSessionData->lastCollectionTimeStamp),
              inherited::state_.currentSessionData->currentStatistic.dataMessages,
              inherited::state_.currentSessionData->currentStatistic.droppedFrames,
              inherited::state_.currentSessionData->currentStatistic.bytes));
}

void
ARDrone_MAVLinkStream::messageCB (const struct __mavlink_message& record_in,
                                  void* payload_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_MAVLinkStream::messageCB"));

  ACE_UNUSED_ARG (record_in);
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
                  ACE_TEXT ("invalid/unknown MAVLink message id (was: %u), continuing\n"),
                  record_in.msgid));
      break;
    }
  } // end SWITCH
}
