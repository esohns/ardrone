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

#ifndef TEST_U_EVENTHANDLER_H
#define TEST_U_EVENTHANDLER_H

#include "ace/Global_Macros.h"

#include "common_iinitialize.h"

#include "stream_common.h"

#include "ardrone_common.h"

// forward declarations
#if defined (GUI_SUPPORT)
struct Test_U_UI_CBData;
#endif // GUI_SUPPORT

template <typename NotificationType,
          typename DataMessageType,
          typename SessionMessageType>
class Test_U_EventHandler_T
 : public NotificationType
 , public Common_IInitializeP_T<ARDrone_INavDataNotify>
 , public Common_IInitializeP_T<ARDrone_IControlNotify>
{
 public:
#if defined (GUI_SUPPORT)
  Test_U_EventHandler_T (struct Test_U_UI_CBData*); // callback data handle
#else
  Test_U_EventHandler_T ();
#endif // GUI_SUPPORT
  inline virtual ~Test_U_EventHandler_T () {}

  // implement Stream_ISessionDataNotify_T
  virtual void start (Stream_SessionId_t,
                      const typename SessionMessageType::DATA_T::DATA_T&);
  virtual void notify (Stream_SessionId_t,
                       const enum Stream_SessionMessageType&,
                       bool = false);
  virtual void end (Stream_SessionId_t);
  virtual void notify (Stream_SessionId_t,
                       const DataMessageType&);
  virtual void notify (Stream_SessionId_t,
                       const SessionMessageType&);

  // implement Common_IInitializeP_T
  inline bool initialize (const ARDrone_INavDataNotify* notify_in) { notify_ = const_cast<ARDrone_INavDataNotify*> (notify_in); return true; }
  inline bool initialize (const ARDrone_IControlNotify* notify_in) { notify_2_ = const_cast<ARDrone_IControlNotify*> (notify_in); return true; }

 private:
#if defined (GUI_SUPPORT)
  ACE_UNIMPLEMENTED_FUNC (Test_U_EventHandler_T ())
#endif // GUI_SUPPORT
  ACE_UNIMPLEMENTED_FUNC (Test_U_EventHandler_T (const Test_U_EventHandler_T&))
  ACE_UNIMPLEMENTED_FUNC (Test_U_EventHandler_T& operator= (const Test_U_EventHandler_T&))

#if defined (GUI_SUPPORT)
  struct Test_U_UI_CBData*                     CBData_;
#endif // GUI_SUPPORT
  ARDrone_INavDataNotify*                      notify_;
  ARDrone_IControlNotify*                      notify_2_;
  typename SessionMessageType::DATA_T::DATA_T* sessionData_;
};

// include template definition
#include "test_u_eventhandler.inl"

#endif
