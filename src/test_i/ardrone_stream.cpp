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

#include "ardrone_configuration.h"
#include "ardrone_modules_common.h"

ARDrone_NavDataStream::ARDrone_NavDataStream ()
 : inherited (ACE_TEXT_ALWAYS_CHAR ("NavDataStream"))
 , inherited2 ()
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
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_Controller_Module (ACE_TEXT_ALWAYS_CHAR ("Controller"),
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
                  ARDrone_Module_Dump_Module (ACE_TEXT_ALWAYS_CHAR ("Dump"),
                                              NULL,
                                              false),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
#endif
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_StatisticReport_Module (ACE_TEXT_ALWAYS_CHAR ("StatisticReport"),
                                                         NULL,
                                                         false),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_NavDataDecoder_Module (ACE_TEXT_ALWAYS_CHAR ("NavDataDecoder"),
                                                        NULL,
                                                        false),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_AsynchUDPSource_Module (ACE_TEXT_ALWAYS_CHAR ("NavDataSource"),
                                                         NULL,
                                                         false),
                  false);
  modules_out.push_back (module_p);

  delete_out = true;

  return true;
}

bool
ARDrone_NavDataStream::initialize (const ARDrone_StreamConfiguration& configuration_in,
                                   bool setupPipeline_in,
                                   bool resetSessionData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_NavDataStream::initialize"));

  if (inherited::isInitialized_)
  {
  } // end IF

  // allocate a new session state, reset stream
  if (!inherited::initialize (configuration_in,
                              false,
                              resetSessionData_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_Base_T::initialize(), aborting\n"),
                ACE_TEXT (inherited::name ().c_str ())));
    return false;
  } // end IF
  ACE_ASSERT (inherited::sessionData_);

  // things to be done here:
  // - create modules (done for the ones "owned" by the stream itself)
  // - initialize modules
  // - push them onto the stream (tail-first)
  struct ARDrone_SessionData& session_data_r =
    const_cast<struct ARDrone_SessionData&> (inherited::sessionData_->get ());
  session_data_r.isNavData = true;
  session_data_r.sessionID = configuration_in.sessionID;
  //  ACE_ASSERT (configuration_in.moduleConfiguration);
  //  configuration_in.moduleConfiguration->streamState = &inherited::state_;
  Stream_ModuleHandlerConfigurationsIterator_t iterator =
      const_cast<struct ARDrone_StreamConfiguration&> (configuration_in).moduleHandlerConfigurations.find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (iterator != configuration_in.moduleHandlerConfigurations.end ());
  struct ARDrone_ModuleHandlerConfiguration* configuration_p =
      dynamic_cast<struct ARDrone_ModuleHandlerConfiguration*> ((*iterator).second);
  ACE_ASSERT (configuration_p);
  ACE_ASSERT (configuration_p->subscribers);
  configuration_p->subscribers->push_back (this);

  // ---------------------------------------------------------------------------

  Stream_Module_t* module_p = NULL;

  // ******************************** Source ***********************************
  module_p =
    const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR ("NavDataSource")));
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to retrieve \"%s\" module handle, aborting\n"),
                ACE_TEXT ("NavDataSource")));
    return false;
  } // end IF

  ARDrone_Module_AsynchUDPSource* sourceWriter_impl_p =
    dynamic_cast<ARDrone_Module_AsynchUDPSource*> (module_p->writer ());
  if (!sourceWriter_impl_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<ARDrone_Module_AsynchUDPSource> failed, aborting\n")));
    return false;
  } // end IF
  sourceWriter_impl_p->set (&(inherited::state_));

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
                ACE_TEXT ("failed to retrieve \"%s\" module handle, aborting\n"),
                ACE_TEXT ("Controller")));
    return false;
  } // end IF

  ARDrone_IController* icontroller_p =
    dynamic_cast<ARDrone_IController*> (module_p->writer ());
  if (!icontroller_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<ARDrone_IController> failed, aborting\n")));
    return false;
  } // end IF

  ACE_ASSERT (configuration_in.initialize);
  if (!configuration_in.initialize->initialize (icontroller_p))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize event handler, aborting\n")));
    return false;
  } // end IF

  // -------------------------------------------------------------

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_ASSERT (configuration_p->format);
  if (session_data_r.format)
    Stream_Module_Device_DirectShow_Tools::deleteMediaType (session_data_r.format);
  ACE_ASSERT (!session_data_r.format);
  if (!Stream_Module_Device_DirectShow_Tools::copyMediaType (*configuration_p->format,
                                                             session_data_r.format))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Stream_Module_Device_DirectShow_Tools::copyMediaType(), aborting\n")));
    goto error;
  } // end IF
  ACE_ASSERT (session_data_r.format);
#endif

  // ---------------------------------------------------------------------------

  if (setupPipeline_in)
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
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  //if (media_type_p)
  //  media_type_p->Release ();
  //if (topology_p)
  //  topology_p->Release ();
  if (session_data_r.direct3DDevice)
  {
    session_data_r.direct3DDevice->Release ();
    session_data_r.direct3DDevice = NULL;
  } // end IF
  if (session_data_r.format)
    Stream_Module_Device_DirectShow_Tools::deleteMediaType (session_data_r.format);
  session_data_r.resetToken = 0;
  if (session_data_r.session)
  {
    session_data_r.session->Release ();
    session_data_r.session = NULL;
  } // end IF
  //if (mediaSession_)
  //{
  //  mediaSession_->Release ();
  //  mediaSession_ = NULL;
  //} // end IF

  //if (COM_initialized)
  //  CoUninitialize ();
#endif
  return false;
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

//////////////////////////////////////////

ARDrone_MAVLinkStream::ARDrone_MAVLinkStream ()
 : inherited (ACE_TEXT_ALWAYS_CHAR ("MAVLinkStream"))
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
                  ARDrone_Module_StatisticReport_Module (ACE_TEXT_ALWAYS_CHAR ("StatisticReport"),
                                                         NULL,
                                                         false),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_MAVLinkDecoder_Module (ACE_TEXT_ALWAYS_CHAR ("MAVLinkDecoder"),
                                                        NULL,
                                                        false),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  ARDrone_Module_AsynchUDPSource_Module (ACE_TEXT_ALWAYS_CHAR ("MAVLinkSource"),
                                                         NULL,
                                                         false),
                  false);
  modules_out.push_back (module_p);

  delete_out = true;

  return true;
}

bool
ARDrone_MAVLinkStream::initialize (const ARDrone_StreamConfiguration& configuration_in,
                                   bool setupPipeline_in,
                                   bool resetSessionData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_MAVLinkStream::initialize"));

  if (inherited::isInitialized_)
  {
  } // end IF

  // allocate a new session state, reset stream
  if (!inherited::initialize (configuration_in,
                              false,
                              resetSessionData_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to Stream_Base_T::initialize(), aborting\n"),
                ACE_TEXT (inherited::name ().c_str ())));
    return false;
  } // end IF
  ACE_ASSERT (inherited::sessionData_);

  // things to be done here:
  // - create modules (done for the ones "owned" by the stream itself)
  // - initialize modules
  // - push them onto the stream (tail-first)
  struct ARDrone_SessionData& session_data_r =
    const_cast<struct ARDrone_SessionData&> (inherited::sessionData_->get ());
  //session_data_r.sessionID = configuration_in.sessionID;
  Stream_ModuleHandlerConfigurationsIterator_t iterator =
      const_cast<struct ARDrone_StreamConfiguration&> (configuration_in).moduleHandlerConfigurations.find (ACE_TEXT_ALWAYS_CHAR (""));
  ACE_ASSERT (iterator != configuration_in.moduleHandlerConfigurations.end ());
  struct ARDrone_ModuleHandlerConfiguration* configuration_p =
      dynamic_cast<struct ARDrone_ModuleHandlerConfiguration*> ((*iterator).second);
  ACE_ASSERT (configuration_p);
  ACE_ASSERT (configuration_p->subscribers);
  configuration_p->subscribers->push_back (this);

  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------

  Stream_Module_t* module_p = NULL;

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
                ACE_TEXT ("failed to retrieve \"%s\" module handle, aborting\n"),
                ACE_TEXT ("MAVLinkSource")));
    return false;
  } // end IF

  ARDrone_Module_AsynchUDPSource* sourceWriter_impl_p =
    dynamic_cast<ARDrone_Module_AsynchUDPSource*> (module_p->writer ());
  if (!sourceWriter_impl_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<ARDrone_Module_AsynchUDPSource> failed, aborting\n")));
    return false;
  } // end IF
  sourceWriter_impl_p->set (&(inherited::state_));

  // enqueue the module
  // *NOTE*: push()ing the module will open() it
  // --> set the argument that is passed along
  module_p->arg (inherited::sessionData_);

  // -------------------------------------------------------------

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ACE_ASSERT (configuration_p->format);
  if (session_data_r.format)
    Stream_Module_Device_DirectShow_Tools::deleteMediaType (session_data_r.format);
  ACE_ASSERT (!session_data_r.format);
  if (!Stream_Module_Device_DirectShow_Tools::copyMediaType (*configuration_p->format,
                                                             session_data_r.format))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Stream_Module_Device_DirectShow_Tools::copyMediaType(), aborting\n")));
    goto error;
  } // end IF
  ACE_ASSERT (session_data_r.format);
#endif

  // ---------------------------------------------------------------------------

  if (setupPipeline_in)
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
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  //if (media_type_p)
  //  media_type_p->Release ();
  //if (topology_p)
  //  topology_p->Release ();
  if (session_data_r.direct3DDevice)
  {
    session_data_r.direct3DDevice->Release ();
    session_data_r.direct3DDevice = NULL;
  } // end IF
  if (session_data_r.format)
    Stream_Module_Device_DirectShow_Tools::deleteMediaType (session_data_r.format);
  session_data_r.resetToken = 0;
  if (session_data_r.session)
  {
    session_data_r.session->Release ();
    session_data_r.session = NULL;
  } // end IF
  //if (mediaSession_)
  //{
  //  mediaSession_->Release ();
  //  mediaSession_ = NULL;
  //} // end IF

  //if (COM_initialized)
  //  CoUninitialize ();
#endif
  return false;
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
