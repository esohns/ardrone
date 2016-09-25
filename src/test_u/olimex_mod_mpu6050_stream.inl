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
#include <string>

#include "ace/Log_Msg.h"

#include "olimex_mod_mpu6050_defines.h"
#include "olimex_mod_mpu6050_macros.h"
#include "olimex_mod_mpu6050_stream_common.h"

template <typename SourceModuleType>
Olimex_Mod_MPU6050_Stream_T<SourceModuleType>::Olimex_Mod_MPU6050_Stream_T ()
 : inherited (ACE_TEXT_ALWAYS_CHAR ("OlimexModMPU6050"),
              false)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_Stream_T::Olimex_Mod_MPU6050_Stream_T"));

}

template <typename SourceModuleType>
Olimex_Mod_MPU6050_Stream_T<SourceModuleType>::~Olimex_Mod_MPU6050_Stream_T ()
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_Stream_T::~Olimex_Mod_MPU6050_Stream_T"));

  // *NOTE*: this implements an ordered shutdown on destruction...
  inherited::shutdown ();
}

template <typename SourceModuleType>
bool
Olimex_Mod_MPU6050_Stream_T<SourceModuleType>::load (Stream_ModuleList_t& modules_out,
                                                     bool& delete_out)
{
  STREAM_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_Stream_T::load"));

  Stream_Module_t* module_p = NULL;
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  Olimex_Mod_MPU6050_Module_RuntimeStatistic_Module (ACE_TEXT_ALWAYS_CHAR ("RuntimeStatistic"),
                                                                     NULL,
                                                                     false),
                  false);
  modules_out.push_back (module_p);
  module_p = NULL;
  ACE_NEW_RETURN (module_p,
                  SourceModuleType (ACE_TEXT_ALWAYS_CHAR ("CamSource"),
                                    NULL,
                                    false),
                  false);
  modules_out.push_back (module_p);

  delete_out = true;

  return true;
}

template <typename SourceModuleType>
bool
Olimex_Mod_MPU6050_Stream_T<SourceModuleType>::initialize (const Olimex_Mod_MPU6050_StreamConfiguration& configuration_in,
                                                           bool setupPipeline_in,
                                                           bool resetSessionData_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_Stream_T::initialize"));

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
  Olimex_Mod_MPU6050_SessionData& session_data_r =
    const_cast<Olimex_Mod_MPU6050_SessionData&> (inherited::sessionData_->get ());
  session_data_r.sessionID = configuration_in.sessionID;
  //  ACE_ASSERT (configuration_in.moduleConfiguration);
  //  configuration_in.moduleConfiguration->streamState = &inherited::state_;

  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------

//  // ******************* Protocol Handler ************************
//  Net_Module_ProtocolHandler* protocolHandler_impl = NULL;
//  protocolHandler_impl =
//      dynamic_cast<Net_Module_ProtocolHandler*> (protocolHandler_.writer ());
//  if (!protocolHandler_impl)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("dynamic_cast<Net_Module_ProtocolHandler> failed, aborting\n")));
//    return false;
//  } // end IF
//  if (!protocolHandler_impl->initialize (configuration_in.messageAllocator,
//                                         protocolConfiguration_in.peerPingInterval,
//                                         protocolConfiguration_in.pingAutoAnswer,
//                                         protocolConfiguration_in.printPongMessages)) // print ('.') for received "pong"s...
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to initialize module: \"%s\", aborting\n"),
//                ACE_TEXT (protocolHandler_.name ())));
//    return false;
//  } // end IF

//  // enqueue the module...
//  if (inherited::push (&protocolHandler_) == -1)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to ACE_Stream::push(\"%s\"): \"%m\", aborting\n"),
//                ACE_TEXT (protocolHandler_.name ())));
//    return false;
//  } // end IF

  // ***************************** Statistics **********************************
//  Olimex_Mod_MPU6050_Module_Statistic_WriterTask_t* statistic_impl_p =
//    dynamic_cast<Olimex_Mod_MPU6050_Module_Statistic_WriterTask_t*> (statistic_.writer ());
//  if (!statistic_impl_p)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("dynamic_cast<Olimex_Mod_MPU6050_Module_RuntimeStatistic> failed, aborting\n")));
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
  Stream_Module_t* module_p =
    const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR ("Source")));
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to retrieve \"%s\" module handle, aborting\n"),
                ACE_TEXT ("Source")));
    return false;
  } // end IF

  typename SourceModuleType::WRITER_T* sourceWriter_impl_p = NULL;
    dynamic_cast<typename SourceModuleType::WRITER_T*> (module_p->writer ());
  if (!sourceWriter_impl_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<SourceModuleType::WRITER_T> failed, aborting\n")));
    return false;
  } // end IF

  if (!sourceWriter_impl_p->initialize (inherited::state_))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to initialize module writer, aborting\n"),
                module_p->name ()));
    return false;
  } // end IF

  // enqueue the module...
  // *NOTE*: push()ing the module will open() it
  // --> set the argument that is passed along
  module_p->arg (inherited::sessionData_);

  // -------------------------------------------------------------

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
  return false;
}

template <typename SourceModuleType>
void
Olimex_Mod_MPU6050_Stream_T<SourceModuleType>::ping ()
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_Stream_T::ping"));

//  Net_Module_ProtocolHandler* protocolHandler_impl = NULL;
//  protocolHandler_impl = dynamic_cast<Net_Module_ProtocolHandler*> (protocolHandler_.writer ());
//  if (!protocolHandler_impl)
//  {
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("dynamic_cast<Net_Module_ProtocolHandler> failed, returning\n")));

//    return;
//  } // end IF

//  // delegate to this module...
//  protocolHandler_impl->handleTimeout (NULL);

  ACE_ASSERT (false);
  ACE_NOTSUP;
  ACE_NOTREACHED (return;)
}

template <typename SourceModuleType>
bool
Olimex_Mod_MPU6050_Stream_T<SourceModuleType>::collect (Olimex_Mod_MPU6050_RuntimeStatistic_t& data_out)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_Stream_T::collect"));

  int result = -1;
  Olimex_Mod_MPU6050_SessionData& session_data_r =
      const_cast<Olimex_Mod_MPU6050_SessionData&> (inherited::sessionData_->get ());
  Stream_Module_t* module_p =
    const_cast<Stream_Module_t*> (inherited::find (ACE_TEXT_ALWAYS_CHAR ("RuntimeStatistic")));
  if (!module_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to retrieve \"%s\" module handle, aborting\n"),
                ACE_TEXT ("RuntimeStatistic")));
    return false;
  } // end IF
  Olimex_Mod_MPU6050_Module_Statistic_WriterTask_t* statistic_impl_p =
    dynamic_cast<Olimex_Mod_MPU6050_Module_Statistic_WriterTask_t*> (module_p->writer ());
  if (!statistic_impl_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<Olimex_Mod_MPU6050_Module_Statistic_WriterTask_t> failed, aborting\n")));
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

template <typename SourceModuleType>
void
Olimex_Mod_MPU6050_Stream_T<SourceModuleType>::report () const
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_Stream_T::report"));

  ACE_ASSERT (inherited::state_.currentSessionData);

  ACE_DEBUG ((LM_INFO,
              ACE_TEXT ("*** [session: %u] RUNTIME STATISTICS ***\n--> Stream Statistics @ %#D<--\n (data) messages: %u\n dropped messages: %u\n bytes total: %.0f\n*** RUNTIME STATISTICS ***\\END\n"),
              inherited::state_.currentSessionData->sessionID,
              &(inherited::state_.currentSessionData->lastCollectionTimeStamp),
              inherited::state_.currentSessionData->currentStatistic.dataMessages,
              inherited::state_.currentSessionData->currentStatistic.droppedMessages,
              inherited::state_.currentSessionData->currentStatistic.bytes));
}
