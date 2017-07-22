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
#include "ardrone_statemachine_navdata.h"

#include "ace/Guard_T.h"
#include "ace/Log_Msg.h"

#include "ardrone_macros.h"

ARDrone_StateMachine_NavData::ARDrone_StateMachine_NavData ()
 : inherited (&lock_,                // lock handle
              NAVDATA_STATE_INVALID) // (initial) state
 , lock_ ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_StateMachine_NavData::ARDrone_StateMachine_NavData"));

}

ARDrone_StateMachine_NavData::~ARDrone_StateMachine_NavData ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_StateMachine_NavData::~ARDrone_StateMachine_NavData"));

}

void
ARDrone_StateMachine_NavData::initialize ()
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_StateMachine_NavData::initialize"));

  if (!change (NAVDATA_STATE_INVALID))
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to ARDrone_StateMachine_NavData::change(NAVDATA_STATE_INVALID), continuing\n")));
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
        // good case
        case NAVDATA_STATE_BOOTSTRAP:
        {
          inherited::change (newState_in);
          return true;
        }
        default:
          break;
      } // end SWITCH

      break;
    }
    case NAVDATA_STATE_BOOTSTRAP:
    {
      switch (newState_in)
      {
        // good case
        case NAVDATA_STATE_COMMAND_ACK:
        {
          inherited::change (newState_in);
          return true;
        }
        default:
          break;
      } // end SWITCH

      break;
    }
    case NAVDATA_STATE_COMMAND_ACK:
    {
      switch (newState_in)
      {
        // good case
        case NAVDATA_STATE_READY:
        {
          inherited::change (newState_in);

          ACE_ASSERT (inherited::condition_);
          int result = inherited::condition_->broadcast ();
          if (result == -1)
            ACE_DEBUG ((LM_ERROR,
                        ACE_TEXT ("failed to ACE_SYNCH_CONDITION_T::broadcast(): \"%m\", continuing\n")));

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
        // good case(s)
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
    case NAVDATA_STATE_SET_PARAMETER:
    {
      switch (newState_in)
      {
        // good case
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
              ACE_TEXT (state2String (inherited::state_).c_str ()),
              ACE_TEXT (state2String (newState_in).c_str ())));

  return false;
}

std::string
ARDrone_StateMachine_NavData::state2String (enum ARDRone_NavDataState state_in) const
{
  ARDRONE_TRACE (ACE_TEXT ("ARDrone_StateMachine_NavData::state2String"));

  // initialize return value(s)
  std::string result = ACE_TEXT_ALWAYS_CHAR ("INVALID");

  switch (state_in)
  {
    case NAVDATA_STATE_INVALID:
      break;
    case NAVDATA_STATE_INITIAL:
    {
      result = ACE_TEXT_ALWAYS_CHAR ("INITIAL");
      break;
    }
    case NAVDATA_STATE_BOOTSTRAP:
    {
      result = ACE_TEXT_ALWAYS_CHAR ("BOOTSTRAP");
      break;
    }
    case NAVDATA_STATE_COMMAND_ACK:
    {
      result = ACE_TEXT_ALWAYS_CHAR ("COMMAND_ACK");
      break;
    }
    case NAVDATA_STATE_READY:
    {
      result = ACE_TEXT_ALWAYS_CHAR ("READY");
      break;
    }
    case NAVDATA_STATE_SET_PARAMETER:
    {
      result = ACE_TEXT_ALWAYS_CHAR ("SET_PARAMETER");
      break;
    }
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
