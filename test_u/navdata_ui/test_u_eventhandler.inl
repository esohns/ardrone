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

#include "ace/Guard_T.h"
#include "ace/Synch_Traits.h"

#include "stream_macros.h"

#include "test_u_common.h"
#include "test_u_defines.h"

template <typename NotificationType,
          typename DataMessageType,
          typename SessionMessageType>
#if defined (GUI_SUPPORT)
Test_U_EventHandler_T<NotificationType,
                      DataMessageType,
                      SessionMessageType>::Test_U_EventHandler_T (struct Test_U_UI_CBData* CBData_in)
#else
Test_U_EventHandler_T<NotificationType,
                      DataMessageType,
                      SessionMessageType>::Test_U_EventHandler_T ()
#endif // GUI_SUPPORT
#if defined (GUI_SUPPORT)
 : CBData_ (CBData_in)
 , notify_(NULL)
#else
 : notify_ (NULL)
#endif // GUI_SUPPORT
 , notify_2_ (NULL)
 , sessionData_ (NULL)
{
  STREAM_TRACE (ACE_TEXT ("Test_U_EventHandler_T::Test_U_EventHandler_T"));

}

template <typename NotificationType,
          typename DataMessageType,
          typename SessionMessageType>
void
Test_U_EventHandler_T<NotificationType,
                      DataMessageType,
                      SessionMessageType>::start (Stream_SessionId_t sessionId_in,
                                                  const typename SessionMessageType::DATA_T::DATA_T& sessionData_in)
{
  STREAM_TRACE (ACE_TEXT ("Test_U_EventHandler_T::start"));

  // sanity check(s)
  //ACE_ASSERT (!sessionData_);

  if (!sessionData_)
    sessionData_ =
      &const_cast<typename SessionMessageType::DATA_T::DATA_T&> (sessionData_in);
}

template <typename NotificationType,
          typename DataMessageType,
          typename SessionMessageType>
void
Test_U_EventHandler_T<NotificationType,
                      DataMessageType,
                      SessionMessageType>::notify (Stream_SessionId_t sessionId_in,
                                                   const enum Stream_SessionMessageType& sessionEvent_in,
                                                   bool expedite_in)
{
  STREAM_TRACE (ACE_TEXT ("Test_U_EventHandler_T::notify"));

  ACE_UNUSED_ARG (sessionId_in);
  ACE_UNUSED_ARG (sessionEvent_in);
  ACE_UNUSED_ARG (expedite_in);
}

template <typename NotificationType,
          typename DataMessageType,
          typename SessionMessageType>
void
Test_U_EventHandler_T<NotificationType,
                      DataMessageType,
                      SessionMessageType>::end (Stream_SessionId_t sessionId_in)
{
  STREAM_TRACE (ACE_TEXT ("Test_U_EventHandler_T::end"));

  ACE_UNUSED_ARG (sessionId_in);

  //if (sessionData_)
  //  sessionData_ = NULL;
}

template <typename NotificationType,
          typename DataMessageType,
          typename SessionMessageType>
void
Test_U_EventHandler_T<NotificationType,
                      DataMessageType,
                      SessionMessageType>::notify (Stream_SessionId_t sessionId_in,
                                                   const DataMessageType& message_in)
{
  STREAM_TRACE (ACE_TEXT ("Test_U_EventHandler_T::notify"));

  ACE_UNUSED_ARG (sessionId_in);
  // sanity check(s)
  ACE_ASSERT (sessionData_);

  const typename DataMessageType::DATA_T& data_container_r = message_in.getR ();
  const typename DataMessageType::DATA_T::DATA_T& data_r =
    data_container_r.getR ();

  switch (message_in.type ())
  {
    case ARDRONE_MESSAGE_NAVDATA:
    {
      ACE_ASSERT (notify_);
      try {
        // *TODO*: remove type inference
        notify_->messageCB (data_r.NavData.NavData,
                            data_r.NavData.NavDataOptionOffsets,
                            message_in.rd_ptr ());
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in ARDrone_INavDataNotify::messageCB(), returning\n")));
      }

      break;
    }
    case ARDRONE_MESSAGE_CONTROL:
    {
      ACE_ASSERT (notify_2_);
      try {
        // *TODO*: remove type inference
        notify_2_->messageCB (data_r.controlData);
      } catch (...) {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("caught exception in ARDrone_IControlNotify::messageCB(), returning\n")));
      }

      break;
    }
    default:
      break;
  } // end SWITCH

#if defined (GUI_SUPPORT)
  { ACE_GUARD (ACE_Thread_Mutex, aGuard, CBData_->UIState->lock);
    CBData_->UIState->eventStack.push (COMMON_UI_EVENT_DATA);
  } // end lock scope

  CBData_->statistic = sessionData_->statistic;
#endif // GUI_SUPPORT
}

template <typename NotificationType,
          typename DataMessageType,
          typename SessionMessageType>
void
Test_U_EventHandler_T<NotificationType,
                      DataMessageType,
                      SessionMessageType>::notify (Stream_SessionId_t sessionId_in,
                                                   const SessionMessageType& sessionMessage_in)
{
  STREAM_TRACE (ACE_TEXT ("Test_U_EventHandler_T::notify"));

  ACE_UNUSED_ARG (sessionId_in);
  // sanity check(s)
  ACE_ASSERT (sessionData_);

  enum Common_UI_EventType event_e = COMMON_UI_EVENT_INVALID;
  switch (sessionMessage_in.type ())
  {
    case STREAM_SESSION_MESSAGE_ABORT:
      event_e = COMMON_UI_EVENT_ABORT; break;
    case STREAM_SESSION_MESSAGE_BEGIN:
      event_e = COMMON_UI_EVENT_STARTED; break;
    case STREAM_SESSION_MESSAGE_LINK:
    case STREAM_SESSION_MESSAGE_UNLINK:
      event_e = COMMON_UI_EVENT_STEP; break;
    case STREAM_SESSION_MESSAGE_CONNECT:
      event_e = COMMON_UI_EVENT_CONNECT; break;
    case STREAM_SESSION_MESSAGE_STEP_DATA:
      event_e = COMMON_UI_EVENT_STEP; break;
    case STREAM_SESSION_MESSAGE_STATISTIC:
      event_e = COMMON_UI_EVENT_STATISTIC; break;
    case STREAM_SESSION_MESSAGE_DISCONNECT:
      event_e = COMMON_UI_EVENT_DISCONNECT; break;
    case STREAM_SESSION_MESSAGE_END:
      event_e = COMMON_UI_EVENT_FINISHED; break;
    default:
    {
      ACE_ASSERT (false);
      break;
    }
  } // end SWITCH

#if defined (GUI_SUPPORT)
  { ACE_GUARD (ACE_Thread_Mutex, aGuard, CBData_->UIState->lock);
    CBData_->UIState->eventStack.push (event_e);
  } // end lock scope

  CBData_->statistic = sessionData_->statistic;
#endif // GUI_SUPPORT
}
