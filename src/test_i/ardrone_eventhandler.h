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

#ifndef ardrone_EVENTHANDLER_H
#define ardrone_EVENTHANDLER_H

#include <ace/Global_Macros.h>

#include "common_iinitialize.h"

#include "stream_common.h"

#include "ardrone_message.h"
#include "ardrone_sessionmessage.h"
//#include "ardrone_types.h"

// forward declarations
struct ARDrone_GtkCBData;
struct ARDrone_SessionData;

class ARDrone_EventHandler
 : public ARDrone_Notification_t
{
 public:
  ARDrone_EventHandler (struct ARDrone_GtkCBData*, // Gtk state
                        bool = false);             // console mode ?
  virtual ~ARDrone_EventHandler ();

  // implement Stream_ISessionDataNotify_T
  virtual void start (Stream_SessionId_t,          // session id
                      const ARDrone_SessionData&); // session data
  virtual void notify (Stream_SessionId_t,                // session id
                       const Stream_SessionMessageType&); // event (state/status change, ...)
  virtual void notify (Stream_SessionId_t,      // session id
                       const ARDrone_Message&); // data
  virtual void notify (Stream_SessionId_t,             // session id
                       const ARDrone_SessionMessage&); // session message
  virtual void end (Stream_SessionId_t); // session id

 private:
  typedef ARDrone_Notification_t inherited;

  ACE_UNIMPLEMENTED_FUNC (ARDrone_EventHandler ())
  ACE_UNIMPLEMENTED_FUNC (ARDrone_EventHandler (const ARDrone_EventHandler&))
  ACE_UNIMPLEMENTED_FUNC (ARDrone_EventHandler& operator= (const ARDrone_EventHandler&))

  bool                      consoleMode_;
  struct ARDrone_GtkCBData* GtkCBData_;
};

#endif