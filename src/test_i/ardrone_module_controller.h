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

#ifndef ARDRONE_MODULE_CONTROLLER_H
#define ARDRONE_MODULE_CONTROLLER_H

#include "ace/Atomic_Op.h"
#include "ace/Global_Macros.h"
#include "ace/Synch_Traits.h"

#include "common_iscanner.h"
#include "common_time_common.h"

#include "stream_task_base_synch.h"

#include "stream_module_target.h"

#include "ardrone_common.h"

// forward declaration(s)
class Stream_IAllocator;

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          ////////////////////////////////
          typename ConfigurationType,
          ////////////////////////////////
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          ////////////////////////////////
          typename SessionDataContainerType,
          ////////////////////////////////
          typename ConnectionConfigurationIteratorType,
          typename ConnectionManagerType,
          typename ConnectorType>
class ARDrone_Module_Controller_T
 : public Stream_Module_Net_Target_T<ACE_SYNCH_USE,
                                     TimePolicyType,
                                     ConfigurationType,
                                     ControlMessageType,
                                     DataMessageType,
                                     SessionMessageType,
                                     SessionDataContainerType,
                                     ConnectionConfigurationIteratorType,
                                     ConnectionManagerType,
                                     ConnectorType>
 , public ARDrone_IController
{
  typedef Stream_Module_Net_Target_T<ACE_SYNCH_USE,
                                     TimePolicyType,
                                     ConfigurationType,
                                     ControlMessageType,
                                     DataMessageType,
                                     SessionMessageType,
                                     SessionDataContainerType,
                                     ConnectionConfigurationIteratorType,
                                     ConnectionManagerType,
                                     ConnectorType> inherited;

 public:
  // *TODO*: on MSVC 2015u3 the accurate declaration does not compile
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  ARDrone_Module_Controller_T (ISTREAM_T*);                     // stream handle
#else
  ARDrone_Module_Controller_T (typename inherited::ISTREAM_T*); // stream handle
#endif
  virtual ~ARDrone_Module_Controller_T ();

  // implement (part of) Stream_ITaskBase
  virtual void handleSessionMessage (SessionMessageType*&, // session message handle
                                     bool&);               // return value: pass message downstream ?

  // implement ARDrone_IController
  virtual void ids (uint8_t,  // session id
                    uint8_t,  // user id
                    uint8_t); // application id

  virtual void init ();
  virtual void start ();
  virtual void resetWatchdog ();

  virtual void trim ();
  virtual void takeoff ();
  virtual void land ();

  virtual void set (enum ARDrone_VideoMode); // video mode

 private:
  ACE_UNIMPLEMENTED_FUNC (ARDrone_Module_Controller_T ())
  ACE_UNIMPLEMENTED_FUNC (ARDrone_Module_Controller_T (const ARDrone_Module_Controller_T&))
  ACE_UNIMPLEMENTED_FUNC (ARDrone_Module_Controller_T& operator= (const ARDrone_Module_Controller_T&))

  // convenient types
  typedef ARDrone_Module_Controller_T<ACE_SYNCH_USE,
                                      TimePolicyType,
                                      ConfigurationType,
                                      ControlMessageType,
                                      DataMessageType,
                                      SessionMessageType,
                                      SessionDataContainerType,
                                      ConnectionConfigurationIteratorType,
                                      ConnectionManagerType,
                                      ConnectorType> OWN_TYPE_T;

  // helper methods
  bool sendATCommand (const std::string&); // AT command string

  // atomic ID generator
  typedef ACE_Atomic_Op<ACE_SYNCH_MUTEX,
                        unsigned long> SEQUENCENUMBER_GENERATOR_T;
  static SEQUENCENUMBER_GENERATOR_T currentID;
};

// include template definition
#include "ardrone_module_controller.inl"

#endif
