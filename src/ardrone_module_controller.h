﻿/***************************************************************************
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

#include <string>

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "guiddef.h"
#endif // ACE_WIN32 || ACE_WIN64

#include "ace/Atomic_Op.h"
#include "ace/Global_Macros.h"
#include "ace/Synch_Traits.h"

#include "common_iscanner.h"
#include "common_time_common.h"

#include "common_timer_manager_common.h"
#include "common_timer_second_publisher.h"

#include "stream_task_base_synch.h"

#include "ardrone_common.h"
#include "ardrone_statemachine_navdata.h"

// forward declaration(s)
class Stream_IAllocator;

extern const char ardrone_default_controller_module_name_string[];

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
          typename WLANMonitorType,
          typename ConnectionManagerType,
          typename ConnectorType,
          ////////////////////////////////
          typename CBDataType>
class ARDrone_Module_Controller_T
 : public Stream_TaskBaseSynch_T<ACE_SYNCH_USE,
                                 TimePolicyType,
                                 ConfigurationType,
                                 ControlMessageType,
                                 DataMessageType,
                                 SessionMessageType,
                                 enum Stream_ControlType,
                                 enum Stream_SessionMessageType,
                                 struct Stream_UserData>
 , public ARDrone_StateMachine_NavData
 , public ARDrone_IController
 , public Common_ICounter
{
  typedef Stream_TaskBaseSynch_T<ACE_SYNCH_USE,
                                 TimePolicyType,
                                 ConfigurationType,
                                 ControlMessageType,
                                 DataMessageType,
                                 SessionMessageType,
                                 enum Stream_ControlType,
                                 enum Stream_SessionMessageType,
                                 struct Stream_UserData> inherited;
  typedef ARDrone_StateMachine_NavData inherited2;

 public:
//  // *TODO*: on MSVC 2015u3 the accurate declaration does not compile
//#if defined (ACE_WIN32) || defined (ACE_WIN64)
//  ARDrone_Module_Controller_T (ISTREAM_T*);                     // stream handle
//#else
  ARDrone_Module_Controller_T (typename inherited::ISTREAM_T*); // stream handle
//#endif
  inline virtual ~ARDrone_Module_Controller_T () {}

  virtual bool initialize (const ConfigurationType&,
                           Stream_IAllocator* = NULL);

  // implement (part of) Stream_ITaskBase
  virtual void handleDataMessage (DataMessageType*&, // data message handle
                                  bool&);            // return value: pass message downstream ?
  virtual void handleSessionMessage (SessionMessageType*&, // session message handle
                                     bool&);               // return value: pass message downstream ?

  // implement (part of) ARDrone_IController
  inline virtual const ARDrone_DeviceConfiguration_t& getR_3 () const { return deviceConfiguration_; }
  inline virtual void setP (ARDrone_DeviceConfiguration_t* configuration_in) { ACE_ASSERT (configuration_in); deviceConfiguration_ = *configuration_in; }
  inline virtual const uint32_t get () const { return deviceState_; }
  inline virtual const struct _navdata_demo_t get_2 () const { return deviceState_2; }
  virtual void calibrate ();
  virtual void dump ();
  virtual void takeoff ();
  virtual void land ();
  void set (enum ARDrone_VideoMode); // video mode
  virtual void leds ();
  virtual void trim ();

  inline bool wait (const ACE_Time_Value* timeout_in = NULL) { return inherited2::wait (NAVDATA_STATE_READY, timeout_in); }

 protected:
  // implement (part of) ARDrone_IController
  virtual void ids (const std::string&,  // session id
                    const std::string&,  // user id
                    const std::string&); // application id
  inline virtual void init () { inherited2::change (NAVDATA_STATE_INITIAL); }
  inline virtual void start () { inherited2::change (NAVDATA_STATE_GET_CONFIGURATION); }
  virtual void resetWatchdog ();

  void resetEmergencyMode ();

 private:
  // convenient types
  typedef ARDrone_Module_Controller_T<ACE_SYNCH_USE,
                                      TimePolicyType,
                                      ConfigurationType,
                                      ControlMessageType,
                                      DataMessageType,
                                      SessionMessageType,
                                      SessionDataContainerType,
                                      WLANMonitorType,
                                      ConnectionManagerType,
                                      ConnectorType,
                                      CBDataType> OWN_TYPE_T;

  ACE_UNIMPLEMENTED_FUNC (ARDrone_Module_Controller_T ())
  ACE_UNIMPLEMENTED_FUNC (ARDrone_Module_Controller_T (const ARDrone_Module_Controller_T&))
  ACE_UNIMPLEMENTED_FUNC (ARDrone_Module_Controller_T& operator= (const ARDrone_Module_Controller_T&))

  // implement (part of) Common_IStateMachine_T
  virtual bool onChange (enum ARDRone_NavDataState); // new state

  // helper methods
  void resetACKFlag ();
  bool sendATCommand (const std::string&); // AT command string

  // implement Common_ICounter
  inline virtual void reset () { resetWatchdog (); }

  // atomic id generator
  typedef ACE_Atomic_Op<ACE_SYNCH_MUTEX,
                        unsigned long> SEQUENCENUMBER_GENERATOR_T;
  static SEQUENCENUMBER_GENERATOR_T currentNavDataMessageId;

  // one-second timer
  typedef Common_Timer_SecondPublisher_t TIMER_SECONDPUBLISHER_T;

  CBDataType*                                    CBData_;
  typename ConnectionManagerType::ICONNECTION_T* connection_;
  ARDrone_DeviceConfiguration_t                  deviceConfiguration_;
  bool                                           deviceInitialized_;
  uint32_t                                       deviceState_;
  struct _navdata_demo_t                         deviceState_2;
  bool                                           isFirst_;
  struct Net_UserData                            userData_;
};

// include template definition
#include "ardrone_module_controller.inl"

#endif
