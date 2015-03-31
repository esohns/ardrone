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

#include "olimex_mod_mpu6050_stream.h"

#include <string>

#include "ace/Log_Msg.h"

#include "net_defines.h"

#include "olimex_mod_mpu6050_macros.h"

Olimex_Mod_MPU6050_Stream::Olimex_Mod_MPU6050_Stream ()
 : inherited ()
 , socketHandler_ (std::string("SocketHandler"),
                   NULL)
// , protocolHandler_ (std::string("ProtocolHandler"),
//                     NULL)
 , runtimeStatistic_ (std::string("RuntimeStatistic"),
                      NULL)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_Stream::Olimex_Mod_MPU6050_Stream"));

  // remember the "owned" ones...
  // *TODO*: clean this up
  // *NOTE*: one problem is that we need to explicitly close() all
  // modules which we have NOT enqueued onto the stream (e.g. because init()
  // failed...)
  inherited::availableModules_.insert_tail (&socketHandler_);
  //  inherited::availableModules_.insert_tail (&protocolHandler_);
  inherited::availableModules_.insert_tail (&runtimeStatistic_);

  // *CHECK* fix ACE bug: modules should initialize their "next" member to NULL !
  inherited::MODULE_T* module = NULL;
  for (ACE_DLList_Iterator<inherited::MODULE_T> iterator (inherited::availableModules_);
       iterator.next (module);
       iterator.advance ())
    module->next (NULL);
}

Olimex_Mod_MPU6050_Stream::~Olimex_Mod_MPU6050_Stream ()
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_Stream::~Olimex_Mod_MPU6050_Stream"));

  // *NOTE*: this implements an ordered shutdown on destruction...
  inherited::shutdown ();
}

bool
Olimex_Mod_MPU6050_Stream::initialize (unsigned int sessionID_in,
                                       Stream_Configuration_t& configuration_in,
                                       const Net_ProtocolConfiguration_t& protocolConfiguration_in,
                                       const Net_UserData_t& userData_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_Stream::initialize"));

  ACE_UNUSED_ARG (protocolConfiguration_in);

  bool result = false;

  // sanity check(s)
  ACE_ASSERT (!isInitialized_);

  // things to be done here:
  // [- init base class]
  // ------------------------------------
  // - init notification strategy (if any)
  // ------------------------------------
  // - push the final module onto the stream (if any)
  // ------------------------------------
  // - init modules (done for the ones "owned" by the stream)
  // - push them onto the stream (tail-first) !
  // ------------------------------------

//  ACE_OS::memset (&inherited::state_, 0, sizeof (inherited::state_));
  inherited::state_.sessionID = sessionID_in;

  if (configuration_in.notificationStrategy)
  {
    inherited::MODULE_T* module = inherited::head ();
    if (!module)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("no head module found, aborting\n")));
      return false;
    } // end IF
    inherited::TASK_T* task = module->reader ();
    if (!task)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("no head module reader task found, aborting\n")));
      return false;
    } // end IF
    task->msg_queue ()->notification_strategy (configuration_in.notificationStrategy);
  } // end IF

  // ---------------------------------------------------------------------------

  if (configuration_in.module)
  {
    inherited::IMODULE_T* module_p = NULL;
    module_p =
        dynamic_cast<inherited::IMODULE_T*> (configuration_in.module);
    if (!module_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("dynamic_cast<Stream_IModule> failed, aborting\n")));
      return false;
    } // end IF

    configuration_in.moduleConfiguration.streamState =
        const_cast<Stream_State_t*> (getState ());
    try
    {
      result = module_p->initialize (configuration_in.moduleConfiguration);
    }
    catch (...)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in Stream_IModule::initialize(\"%s\"): \"%s\", aborting\n"),
                  ACE_TEXT (configuration_in.module->name ())));
      return false;
    }
    if (!result)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to initialize module: \"%s\", aborting\n"),
                  ACE_TEXT (configuration_in.module->name ())));
      return false;
    } // end IF

    // enqueue the module...
    if (inherited::push (configuration_in.module) == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Stream::push(\"%s\"): \"%m\", aborting\n"),
                  ACE_TEXT (configuration_in.module->name ())));
      return false;
    } // end IF
  } // end IF

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

  // ******************* Runtime Statistics ************************
  Olimex_Mod_MPU6050_Module_Statistic_WriterTask_t* runtimeStatistic_impl = NULL;
  runtimeStatistic_impl =
      dynamic_cast<Olimex_Mod_MPU6050_Module_Statistic_WriterTask_t*> (runtimeStatistic_.writer ());
  if (!runtimeStatistic_impl)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<Olimex_Mod_MPU6050_Module_RuntimeStatistic> failed, aborting\n")));
    return false;
  } // end IF
  if (!runtimeStatistic_impl->initialize (configuration_in.statisticReportingInterval, // reporting interval (seconds)
                                          configuration_in.printFinalReport,           // print final report ?
                                          configuration_in.messageAllocator))          // message allocator handle
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize module: \"%s\", aborting\n"),
                ACE_TEXT (runtimeStatistic_.name ())));
    return false;
  } // end IF

  // enqueue the module...
  if (inherited::push (&runtimeStatistic_) == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Stream::push(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (runtimeStatistic_.name ())));
    return false;
  } // end IF

  // ******************* Socket Handler ************************
  Olimex_Mod_MPU6050_Module_SocketHandler* socketHandler_impl = NULL;
  socketHandler_impl =
      dynamic_cast<Olimex_Mod_MPU6050_Module_SocketHandler*> (socketHandler_.writer ());
  if (!socketHandler_impl)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<Olimex_Mod_MPU6050_Module_SocketHandler> failed, aborting\n")));
    return false;
  } // end IF
  if (!socketHandler_impl->initialize (&state_,
                                       configuration_in.messageAllocator,
                                       configuration_in.useThreadPerConnection,
                                       NET_STATISTICS_COLLECTION_INTERVAL))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to initialize module: \"%s\", aborting\n"),
                ACE_TEXT (socketHandler_.name ())));
    return false;
  } // end IF

  // enqueue the module...
  // *NOTE*: push()ing the module will open() it
  // --> set the argument that is passed along
  socketHandler_.arg (&const_cast<Net_UserData_t&> (userData_in));
  if (inherited::push (&socketHandler_) == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Stream::push(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (socketHandler_.name ())));
    return false;
  } // end IF

  // -------------------------------------------------------------

  // set (session) message allocator
  inherited::allocator_ = configuration_in.messageAllocator;

  // OK: all went well
  inherited::isInitialized_ = true;
//   inherited::dump_state ();

  return true;
}

void
Olimex_Mod_MPU6050_Stream::ping ()
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_Stream::ping"));

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

#if defined (_MSC_VER)
  ACE_NOTREACHED (true);
#endif
}

bool
Olimex_Mod_MPU6050_Stream::collect (Stream_Statistic_t& data_out)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_Stream::collect"));

  Olimex_Mod_MPU6050_Module_Statistic_WriterTask_t* runtimeStatistic_impl =
   NULL;
  runtimeStatistic_impl =
      dynamic_cast<Olimex_Mod_MPU6050_Module_Statistic_WriterTask_t*> (const_cast<Olimex_Mod_MPU6050_Module_RuntimeStatistic_Module&> (runtimeStatistic_).writer ());
  if (!runtimeStatistic_impl)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<Olimex_Mod_MPU6050_Module_Statistic_WriterTask_t> failed, aborting\n")));
    return false;
  } // end IF

  // *NOTE*: the statistics module knows nothing about dropped messages
  //         --> retain this data, as it could be overwritten
  data_out = inherited::state_.currentStatistics;
  // delegate to statistics module...
  if (!runtimeStatistic_impl->collect (inherited::state_.currentStatistics))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Olimex_Mod_MPU6050_Module_Statistic_WriterTask_t::collect(), aborting\n")));
    return false;
  } // end IF
  inherited::state_.lastCollectionTimestamp = ACE_OS::gettimeofday ();
  inherited::state_.currentStatistics.numDroppedMessages =
   data_out.numDroppedMessages;

  data_out = inherited::state_.currentStatistics;

  return true;
}

void
Olimex_Mod_MPU6050_Stream::report () const
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_Stream::report"));

  ACE_DEBUG ((LM_INFO,
              ACE_TEXT ("*** [session: %u] RUNTIME STATISTICS ***\n--> Stream Statistics @ %#D<--\n (data) messages: %u\n dropped messages: %u\n bytes total: %.0f\n*** RUNTIME STATISTICS ***\\END\n"),
              inherited::state_.sessionID,
              &inherited::state_.lastCollectionTimestamp,
              inherited::state_.currentStatistics.numDataMessages,
              inherited::state_.currentStatistics.numDroppedMessages,
              inherited::state_.currentStatistics.numBytes));
}