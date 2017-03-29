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

#ifndef ARDRONE_EVENTHANDLER_H
#define ARDRONE_EVENTHANDLER_H

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
 , public ARDrone_INotify
 , public Common_IInitializeP_T<ARDrone_IController>
{
 public:
  ARDrone_EventHandler (struct ARDrone_GtkCBData*, // Gtk state
                        bool = false);             // console mode ?
  virtual ~ARDrone_EventHandler ();

  // implement Stream_ISessionDataNotify_T
  virtual void start (Stream_SessionId_t,          // session id
                      const ARDrone_SessionData&); // session data
  virtual void notify (Stream_SessionId_t,                     // session id
                       const enum Stream_SessionMessageType&); // event (state/status change, ...)
  virtual void notify (Stream_SessionId_t,      // session id
                       const ARDrone_Message&); // data
  virtual void notify (Stream_SessionId_t,             // session id
                       const ARDrone_SessionMessage&); // session message
  virtual void end (Stream_SessionId_t); // session id

  // implement ARDrone_INotify
  virtual void messageCB (const struct __mavlink_message&, // message record
                          void*);                          // payload handle
  virtual void messageCB (const struct _navdata_t&,                     // message record
                          const ARDrone_NavDataMessageOptionOffsets_t&, // option offsets
                          void*);                                       // payload handle

  // implement Common_IInitializeP_T
  inline bool initialize (const ARDrone_IController* controller_in) { controller_ = const_cast<ARDrone_IController*> (controller_in); return true; };

 private:
  ACE_UNIMPLEMENTED_FUNC (ARDrone_EventHandler ())
  ACE_UNIMPLEMENTED_FUNC (ARDrone_EventHandler (const ARDrone_EventHandler&))
  ACE_UNIMPLEMENTED_FUNC (ARDrone_EventHandler& operator= (const ARDrone_EventHandler&))

  bool                      consoleMode_;
  ARDrone_IController*      controller_;
  struct ARDrone_GtkCBData* GtkCBData_;
  bool                      navDataSessionStarted_;
  bool                      videoModeSet_;
};

#endif
