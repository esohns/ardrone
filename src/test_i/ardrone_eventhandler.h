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

#include <map>
#include <utility>

#include "ace/Global_Macros.h"

#include "common_iinitialize.h"

#include "stream_common.h"

#include "ardrone_message.h"
#include "ardrone_sessionmessage.h"
#include "ardrone_types.h"

// forward declarations
struct ARDrone_GtkCBData;
struct ARDrone_SessionData;

class ARDrone_EventHandler
 : public ARDrone_Notification_t
 , public Common_IInitializeP_T<ARDrone_IMAVLinkNotify>
 , public Common_IInitializeP_T<ARDrone_INavDataNotify>
{
 public:
  ARDrone_EventHandler (struct ARDrone_GtkCBData*, // Gtk state
                        bool = false);             // console mode ?
  inline virtual ~ARDrone_EventHandler () {};

  // implement Stream_ISessionDataNotify_T
  virtual void start (Stream_SessionId_t,          // session id
                      const ARDrone_SessionData&); // session data
  inline virtual void notify (Stream_SessionId_t,                        // session id
                              const enum Stream_SessionMessageType&) {}; // event (state/status change, ...)
  virtual void notify (Stream_SessionId_t,      // session id
                       const ARDrone_Message&); // data
  virtual void notify (Stream_SessionId_t,             // session id
                       const ARDrone_SessionMessage&); // session message
  virtual void end (Stream_SessionId_t); // session id

  // implement Common_IInitializeP_T
  inline bool initialize (const ARDrone_IMAVLinkNotify* notify_in) { MAVLinkNotify_ = const_cast<ARDrone_IMAVLinkNotify*> (notify_in); return true; };
  inline bool initialize (const ARDrone_INavDataNotify* notify_in) { NavDataNotify_ = const_cast<ARDrone_INavDataNotify*> (notify_in); return true; };

 private:
  ACE_UNIMPLEMENTED_FUNC (ARDrone_EventHandler ())
  ACE_UNIMPLEMENTED_FUNC (ARDrone_EventHandler (const ARDrone_EventHandler&))
  ACE_UNIMPLEMENTED_FUNC (ARDrone_EventHandler& operator= (const ARDrone_EventHandler&))

  typedef std::map<Stream_SessionId_t, enum ARDrone_StreamType> SESSIONID_TO_STREAM_MAP_T;
  typedef SESSIONID_TO_STREAM_MAP_T::iterator SESSIONID_TO_STREAM_MAP_ITERATOR_T;
  typedef std::pair<Stream_SessionId_t, enum ARDrone_StreamType> SESSIONID_TO_STREAM_PAIR_T;
  struct SESSIONID_TO_STREAM_MAP_FIND_S
   : public std::binary_function<SESSIONID_TO_STREAM_PAIR_T,
                                 enum ARDrone_StreamType,
                                 bool>
  {
    inline bool operator() (const SESSIONID_TO_STREAM_PAIR_T& entry_in, enum ARDrone_StreamType value_in) const { return entry_in.second == value_in; };
  };

  bool                      consoleMode_;
  struct ARDrone_GtkCBData* GtkCBData_;
  ARDrone_IMAVLinkNotify*   MAVLinkNotify_;
  ARDrone_INavDataNotify*   NavDataNotify_;
  SESSIONID_TO_STREAM_MAP_T streams_;
};

#endif
