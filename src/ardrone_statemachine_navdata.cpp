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

#include "ardrone_statemachine_navdata.h"

#include "ace/Guard_T.h"
#include "ace/Log_Msg.h"

#include "ardrone_defines.h"
#include "ardrone_macros.h"

const char ardrone_navdata_statemachine_name_string_[] =
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_STATEMACHINE_NAVDATA_NAME);

ARDrone_StateMachine_NavData::ARDrone_StateMachine_NavData ()
 : inherited (&lock_,                // lock handle
              NAVDATA_STATE_INVALID) // (initial) state
 , lock_ ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_StateMachine_NavData::ARDrone_StateMachine_NavData"));

}

bool
ARDrone_StateMachine_NavData::change (enum ARDRone_NavDataState newState_in)
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_StateMachine_NavData::change"));

  // sanity check(s)
  ACE_ASSERT (inherited::stateLock_);

  // synchronize access to state machine
  ACE_GUARD_RETURN (ACE_SYNCH_NULL_MUTEX, aGuard, *inherited::stateLock_, false);

  switch (inherited::state_)
  {
    case NAVDATA_STATE_INVALID:
    {
      switch (newState_in)
      {
        // good case
        case NAVDATA_STATE_INITIAL:
        {
          inherited::change (newState_in);
          return true;
        }
        default:
          break;
      } // end SWITCH

      break;
    }
    case NAVDATA_STATE_INITIAL:
    {
      switch (newState_in)
      {
        case NAVDATA_STATE_INVALID:
        // good case
        case NAVDATA_STATE_GET_CONFIGURATION:
        {
          inherited::change (newState_in);
          return true;
        }
        default:
          break;
      } // end SWITCH

      break;
    }
    case NAVDATA_STATE_COMMAND_ACK: // *TODO*: this is wrong; remove ASAP
    {
      switch (newState_in)
      {
        case NAVDATA_STATE_INVALID:
        // good case
        case NAVDATA_STATE_COMMAND_ACK: // *TODO*: this is wrong; remove ASAP
        case NAVDATA_STATE_READY: // *TODO*: this is wrong; remove ASAP
        {
          inherited::change (newState_in);
          return true;
        }
        default:
          break;
      } // end SWITCH

      break;
    }
    case NAVDATA_STATE_GET_CONFIGURATION:
    {
      switch (newState_in)
      {
        case NAVDATA_STATE_INVALID:
        // good case
        case NAVDATA_STATE_COMMAND_ACK:
        case NAVDATA_STATE_SWITCH_MODE:
        case NAVDATA_STATE_NAVDATA_OPTIONS:
        {
          inherited::change (newState_in);
          return true;
        }
        default:
          break;
      } // end SWITCH

      break;
    }
    case NAVDATA_STATE_SWITCH_MODE:
    {
      switch (newState_in)
      {
        case NAVDATA_STATE_INVALID:
        // good case
        case NAVDATA_STATE_COMMAND_ACK:
        case NAVDATA_STATE_NAVDATA_OPTIONS:
        {
          inherited::change (newState_in);
          return true;
        }
        default:
          break;
      } // end SWITCH

      break;
    }
    case NAVDATA_STATE_NAVDATA_OPTIONS:
    {
      switch (newState_in)
      {
        case NAVDATA_STATE_INVALID:
        // good case
        case NAVDATA_STATE_COMMAND_ACK:
        case NAVDATA_STATE_SET_VIDEO:
        {
          inherited::change (newState_in);
          return true;
        }
        default:
          break;
      } // end SWITCH

      break;
    }
    case NAVDATA_STATE_SET_VIDEO:
    {
      switch (newState_in)
      {
        case NAVDATA_STATE_INVALID:
        // good case
        case NAVDATA_STATE_COMMAND_ACK:
        case NAVDATA_STATE_CALIBRATE_SENSOR:
        {
          inherited::change (newState_in);
          return true;
        }
        default:
          break;
      } // end SWITCH

      break;
    }
    case NAVDATA_STATE_READY:
    {
      switch (newState_in)
      {
        case NAVDATA_STATE_INVALID:
        // good case
        case NAVDATA_STATE_COMMAND_ACK: // *TODO*: this is wrong; remove ASAP
        case NAVDATA_STATE_READY:       // *TODO*: this is wrong; remove ASAP
        case NAVDATA_STATE_CALIBRATE_SENSOR:
        case NAVDATA_STATE_SET_PARAMETER:
        {
          inherited::change (newState_in);
          return true;
        }
        default:
          break;
      } // end SWITCH

      break;
    }
    case NAVDATA_STATE_CALIBRATE_SENSOR:
    case NAVDATA_STATE_SET_PARAMETER:
    {
      switch (newState_in)
      {
        case NAVDATA_STATE_INVALID:
        // good case
        case NAVDATA_STATE_COMMAND_ACK:
        case NAVDATA_STATE_READY:
        {
          inherited::change (newState_in);
          return true;
        }
        default:
          break;
      } // end SWITCH

      break;
    }
    default:
      break;
  } // end SWITCH
  ACE_DEBUG ((LM_ERROR,
              ACE_TEXT ("unknown/invalid state switch: \"%s\" --> \"%s\" --> check implementation !, aborting\n"),
              ACE_TEXT (stateToString (inherited::state_).c_str ()),
              ACE_TEXT (stateToString (newState_in).c_str ())));

  return false;
}

std::string
ARDrone_StateMachine_NavData::stateToString (enum ARDRone_NavDataState state_in) const
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_StateMachine_NavData::stateToString"));

  // initialize return value(s)
  std::string result = ACE_TEXT_ALWAYS_CHAR ("INVALID");

  switch (state_in)
  {
    case NAVDATA_STATE_INVALID:
      break;
    case NAVDATA_STATE_INITIAL:
      result = ACE_TEXT_ALWAYS_CHAR ("INITIAL"); break;
    case NAVDATA_STATE_COMMAND_ACK:
      result = ACE_TEXT_ALWAYS_CHAR ("COMMAND_ACK"); break;
    case NAVDATA_STATE_GET_CONFIGURATION:
      result = ACE_TEXT_ALWAYS_CHAR ("CONFIG"); break;
    case NAVDATA_STATE_SWITCH_MODE:
      result = ACE_TEXT_ALWAYS_CHAR ("MODE"); break;
    case NAVDATA_STATE_NAVDATA_OPTIONS:
      result = ACE_TEXT_ALWAYS_CHAR ("OPTIONS"); break;
    case NAVDATA_STATE_READY:
      result = ACE_TEXT_ALWAYS_CHAR ("READY"); break;
    case NAVDATA_STATE_SET_PARAMETER:
      result = ACE_TEXT_ALWAYS_CHAR ("SET_PARAMETER"); break;
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("unknown/invalid state (was: %d), aborting\n"),
                  state_in));
      break;
    }
  } // end SWITCH

  return result;
}
