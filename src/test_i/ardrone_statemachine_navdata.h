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

#ifndef ARDRONE_STATEMACHINE_NAVDATA_H
#define ARDRONE_STATEMACHINE_NAVDATA_H

#include <string>

#include "ace/Global_Macros.h"
#include "ace/Synch_Traits.h"

#include "common_istatemachine.h"
#include "common_statemachine_base.h"

enum ARDRone_NavDataState
{
  NAVDATA_STATE_INVALID = -1,
  // *NOTE*: --> port 5554
  NAVDATA_STATE_INITIAL = 0,       // request navdata
  // -------------------------------------
  // *NOTE*: --> port 5556
  NAVDATA_STATE_GET_CONFIGURATION, // first packet arrives --> request configuration
  NAVDATA_STATE_SWITCH_MODE,       // [switch to demo/full mode iff in bootstrap]
  NAVDATA_STATE_NAVDATA_OPTIONS,   // select navdata options
  NAVDATA_STATE_SET_VIDEO,         // set up video
  NAVDATA_STATE_READY,             // initialization complete
  ////////////////////////////////////////
  NAVDATA_STATE_CALIBRATE_SENSOR,  // trimming accelerometer/calibrating gyroscope/...
  NAVDATA_STATE_SET_PARAMETER,     // setting parameter (e.g. video birate/codec/...)
  ////////////////////////////////////////
  NAVDATA_STATE_COMMAND_ACK,       // command ACK arrived --> reset ACK_CONTROL_MODE and switch states
  ////////////////////////////////////////
  NAVDATA_STATE_MAX
};

class ARDrone_StateMachine_NavData
 : public Common_StateMachine_Base_T<ACE_NULL_SYNCH,
                                     enum ARDRone_NavDataState>
{
 public:
  ARDrone_StateMachine_NavData ();
  inline virtual ~ARDrone_StateMachine_NavData () {}

  // implement (part of) Common_IStateMachine_T
  virtual void initialize ();
  inline virtual void reset () { initialize (); };
  virtual std::string stateToString (enum ARDRone_NavDataState) const;

 protected:
   ACE_SYNCH_NULL_MUTEX lock_;

  // implement (part of) Common_IStateMachine_T
  // *NOTE*: only derived classes can change state
  virtual bool change (enum ARDRone_NavDataState); // new state

 private:
  typedef Common_StateMachine_Base_T<ACE_NULL_SYNCH,
                                     enum ARDRone_NavDataState> inherited;

  ACE_UNIMPLEMENTED_FUNC (ARDrone_StateMachine_NavData (const ARDrone_StateMachine_NavData&))
  ACE_UNIMPLEMENTED_FUNC (ARDrone_StateMachine_NavData& operator= (const ARDrone_StateMachine_NavData&))
};

// convenient types
typedef Common_IStateMachine_T<enum ARDRone_NavDataState> ARDrone_IStateMachine_NavData_t;

#endif
