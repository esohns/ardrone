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

//#include "net_defines.h"

#include "olimex_mod_mpu6050_defines.h"
#include "olimex_mod_mpu6050_macros.h"

Olimex_Mod_MPU6050_Stream::Olimex_Mod_MPU6050_Stream ()
 : inherited ()
 , netReader_ (ACE_TEXT_ALWAYS_CHAR ("NetReader"),
               NULL)
// , protocolHandler_ (std::string("ProtocolHandler"),
//                     NULL)
 , runtimeStatistic_ (ACE_TEXT_ALWAYS_CHAR ("RuntimeStatistic"),
                      NULL)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_Stream::Olimex_Mod_MPU6050_Stream"));

  // remember the "owned" ones...
  // *TODO*: clean this up
  // *NOTE*: one problem is that we need to explicitly close() all
  // modules which we have NOT enqueued onto the stream (e.g. because init()
  // failed...)
  inherited::availableModules_.push_front (&netReader_);
  //  inherited::availableModules_.push_front (&protocolHandler_);
  inherited::availableModules_.push_front (&runtimeStatistic_);

  // *TODO*: fix ACE bug: modules should initialize their "next" member to NULL
  //inherited::MODULE_T* module_p = NULL;
  //for (ACE_DLList_Iterator<inherited::MODULE_T> iterator (inherited::availableModules_);
  //     iterator.next (module_p);
  //     iterator.advance ())
  //  module_p->next (NULL);
  for (inherited::MODULE_CONTAINER_ITERATOR_T iterator = inherited::availableModules_.begin ();
       iterator != inherited::availableModules_.end ();
       iterator++)
       (*iterator)->next (NULL);
}

Olimex_Mod_MPU6050_Stream::~Olimex_Mod_MPU6050_Stream ()
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_Stream::~Olimex_Mod_MPU6050_Stream"));

  // *NOTE*: this implements an ordered shutdown on destruction...
  inherited::shutdown ();
}

bool
Olimex_Mod_MPU6050_Stream::initialize (const Olimex_Mod_MPU6050_StreamConfiguration& configuration_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_Stream::initialize"));

  // sanity check(s)
  ACE_ASSERT (!inherited::isInitialized_);
  ACE_ASSERT (!isRunning ());

  // allocate a new session state, reset stream
  inherited::initialize ();

  // things to be done here:
  // - create modules (done for the ones "owned" by the stream itself)
  // - initialize modules
  // - push them onto the stream (tail-first) !

  inherited::sessionData_->sessionID = configuration_in.sessionID;

  int result = -1;
  inherited::MODULE_T* module_p = NULL;
  if (configuration_in.notificationStrategy)
  {
    module_p = inherited::head ();
    if (!module_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("no head module found, aborting\n")));
      return false;
    } // end IF
    inherited::TASK_T* task_p = module_p->reader ();
    if (!task_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("no head module reader task found, aborting\n")));
      return false;
    } // end IF
    inherited::QUEUE_T* queue_p = task_p->msg_queue ();
    if (!queue_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("no head module reader task queue found, aborting\n")));
      return false;
    } // end IF
    queue_p->notification_strategy (configuration_in.notificationStrategy);
  } // end IF

  //  ACE_ASSERT (configuration_in.moduleConfiguration);
  //  configuration_in.moduleConfiguration->streamState = &inherited::state_;

  // ---------------------------------------------------------------------------
  if (configuration_in.module)
  {
    // *TODO*: (at least part of) this procedure belongs in libACEStream
    //         --> remove type inferences
    inherited::IMODULE_T* module_2 =
      dynamic_cast<inherited::IMODULE_T*> (configuration_in.module);
    if (!module_2)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: dynamic_cast<Stream_IModule_T> failed, aborting\n"),
                  configuration_in.module->name ()));
      return false;
    } // end IF
    if (!module_2->initialize (*configuration_in.moduleConfiguration))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to initialize module, aborting\n"),
                  configuration_in.module->name ()));
      return false;
    } // end IF
    Stream_Task_t* task_p = configuration_in.module->writer ();
    ACE_ASSERT (task_p);
    inherited::IMODULEHANDLER_T* module_handler_p =
      dynamic_cast<inherited::IMODULEHANDLER_T*> (task_p);
    if (!module_handler_p)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: dynamic_cast<Common_IInitialize_T<HandlerConfigurationType>> failed, aborting\n"),
                  configuration_in.module->name ()));
      return false;
    } // end IF
    if (!module_handler_p->initialize (*configuration_in.moduleHandlerConfiguration))
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("%s: failed to initialize module handler, aborting\n"),
                  configuration_in.module->name ()));
      return false;
    } // end IF
    result = inherited::push (configuration_in.module);
    if (result == -1)
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Stream::push(\"%s\"): \"%m\", aborting\n"),
                  configuration_in.module->name ()));
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
  Olimex_Mod_MPU6050_Module_Statistic_WriterTask_t* runtimeStatistic_impl_p =
    dynamic_cast<Olimex_Mod_MPU6050_Module_Statistic_WriterTask_t*> (runtimeStatistic_.writer ());
  if (!runtimeStatistic_impl_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<Olimex_Mod_MPU6050_Module_RuntimeStatistic> failed, aborting\n")));
    return false;
  } // end IF
  if (!runtimeStatistic_impl_p->initialize (configuration_in.statisticReportingInterval,
                                            configuration_in.messageAllocator))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to initialize module writer, aborting\n"),
                runtimeStatistic_.name ()));
    return false;
  } // end IF

  // enqueue the module...
  result = inherited::push (&runtimeStatistic_);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Stream::push(\"%s\"): \"%m\", aborting\n"),
                runtimeStatistic_.name ()));
    return false;
  } // end IF

  // ******************* Net Reader ************************
  Olimex_Mod_MPU6050_Module_SocketHandler* socketHandler_impl_p =
    dynamic_cast<Olimex_Mod_MPU6050_Module_SocketHandler*> (netReader_.writer ());
  if (!socketHandler_impl_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<Olimex_Mod_MPU6050_Module_SocketHandler> failed, aborting\n")));
    return false;
  } // end IF
  if (!socketHandler_impl_p->initialize (*configuration_in.moduleHandlerConfiguration))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("%s: failed to initialize module writer, aborting\n"),
                netReader_.name ()));
    return false;
  } // end IF

  // enqueue the module...
  // *NOTE*: push()ing the module will open() it
  // --> set the argument that is passed along
  netReader_.arg (const_cast<Olimex_Mod_MPU6050_ModuleHandlerConfiguration*> (configuration_in.moduleHandlerConfiguration));
  result = inherited::push (&netReader_);
  if (result == -1)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ACE_Stream::push(\"%s\"): \"%m\", aborting\n"),
                netReader_.name ()));
    return false;
  } // end IF

  // -------------------------------------------------------------

  // set (session) message allocator
  // *TODO*: clean this up ! --> sanity check
  ACE_ASSERT (configuration_in.messageAllocator);
  inherited::allocator_ = configuration_in.messageAllocator;

  inherited::isInitialized_ = true;
  //   inherited::dump_state();

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
  ACE_NOTREACHED (return;)
}

bool
Olimex_Mod_MPU6050_Stream::collect (Olimex_Mod_MPU6050_RuntimeStatistic_t& data_out)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_Stream::collect"));

  Olimex_Mod_MPU6050_Module_Statistic_WriterTask_t* runtimeStatistic_impl_p =
    dynamic_cast<Olimex_Mod_MPU6050_Module_Statistic_WriterTask_t*> (const_cast<Olimex_Mod_MPU6050_Module_RuntimeStatistic_Module&> (runtimeStatistic_).writer ());
  if (!runtimeStatistic_impl_p)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("dynamic_cast<Olimex_Mod_MPU6050_Module_Statistic_WriterTask_t> failed, aborting\n")));
    return false;
  } // end IF

  // *NOTE*: the statistic module knows nothing about dropped messages
  //         --> retain this data, as it could be overwritten
  ACE_ASSERT (inherited::state_.currentSessionData);

  int result = -1;
  if (inherited::state_.currentSessionData->lock)
  {
    result = inherited::state_.currentSessionData->lock->acquire ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Thread_Mutex::acquire(): \"%m\", continuing\n")));
  } // end IF
  data_out = inherited::state_.currentSessionData->currentStatistic;

  // delegate to statistics module...
  if (!runtimeStatistic_impl_p->collect (inherited::state_.currentSessionData->currentStatistic))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to Olimex_Mod_MPU6050_Module_Statistic_WriterTask_t::collect(), aborting\n")));

    // clean up
    if (inherited::state_.currentSessionData->lock)
    {
      result = inherited::state_.currentSessionData->lock->release ();
      if (result == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Thread_Mutex::release(): \"%m\", continuing\n")));
    } // end IF

    return false;
  } // end IF
  inherited::state_.currentSessionData->lastCollectionTimestamp =
    ACE_OS::gettimeofday ();
  inherited::state_.currentSessionData->currentStatistic.droppedMessages =
    data_out.droppedMessages;

  data_out = inherited::state_.currentSessionData->currentStatistic;

  if (inherited::state_.currentSessionData->lock)
  {
    result = inherited::state_.currentSessionData->lock->release ();
    if (result == -1)
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to ACE_Thread_Mutex::release(): \"%m\", continuing\n")));
  } // end IF

  return true;
}

void
Olimex_Mod_MPU6050_Stream::report () const
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_Stream::report"));

  ACE_ASSERT (inherited::state_.currentSessionData);

  ACE_DEBUG ((LM_INFO,
              ACE_TEXT ("*** [session: %u] RUNTIME STATISTICS ***\n--> Stream Statistics @ %#D<--\n (data) messages: %u\n dropped messages: %u\n bytes total: %.0f\n*** RUNTIME STATISTICS ***\\END\n"),
              inherited::state_.currentSessionData->sessionID,
              &inherited::state_.currentSessionData->lastCollectionTimestamp,
              inherited::state_.currentSessionData->currentStatistic.dataMessages,
              inherited::state_.currentSessionData->currentStatistic.droppedMessages,
              inherited::state_.currentSessionData->currentStatistic.bytes));
}
