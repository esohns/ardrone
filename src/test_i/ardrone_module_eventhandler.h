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

#ifndef ARDRONE_MODULE_EVENTHANDLER_H
#define ARDRONE_MODULE_EVENTHANDLER_H

#include <map>

#include "ace/Global_Macros.h"
#include "ace/Synch_Traits.h"

#include "common_time_common.h"

#include "stream_common.h"
#include "stream_streammodule_base.h"

#include "stream_misc_messagehandler.h"

#include "ardrone_message.h"
#include "ardrone_sessionmessage.h"
#include "ardrone_stream_common.h"
#include "ardrone_types.h"

class ARDrone_Module_EventHandler
 : public Stream_Module_MessageHandlerA_T<ACE_MT_SYNCH,
                                          Common_TimePolicy_t,
                                          struct ARDrone_ModuleHandlerConfiguration,
                                          ARDrone_ControlMessage_t,
                                          ARDrone_Message,
                                          ARDrone_SessionMessage,
                                          Stream_SessionId_t,
                                          struct ARDrone_SessionData,
                                          struct ARDrone_UserData>
{
  typedef Stream_Module_MessageHandlerA_T<ACE_MT_SYNCH,
                                          Common_TimePolicy_t,
                                          struct ARDrone_ModuleHandlerConfiguration,
                                          ARDrone_ControlMessage_t,
                                          ARDrone_Message,
                                          ARDrone_SessionMessage,
                                          Stream_SessionId_t,
                                          struct ARDrone_SessionData,
                                          struct ARDrone_UserData> inherited;

 public:
  ARDrone_Module_EventHandler (typename inherited::ISTREAM_T*); // stream handle
  inline virtual ~ARDrone_Module_EventHandler () {};

  // override (part of) Stream_ITaskBase_T
  virtual void handleDataMessage (ARDrone_Message*&, // message handle
                                  bool&);            // return value: pass message downstream ?
  virtual void handleSessionMessage (ARDrone_SessionMessage*&, // session message handle
                                     bool&);                   // return value: pass message downstream ?

  // implement Common_IClone_T
  virtual ACE_Task<ACE_MT_SYNCH,
                   Common_TimePolicy_t>* clone ();

 private:
  ACE_UNIMPLEMENTED_FUNC (ARDrone_Module_EventHandler ())
  ACE_UNIMPLEMENTED_FUNC (ARDrone_Module_EventHandler (const ARDrone_Module_EventHandler&))
  ACE_UNIMPLEMENTED_FUNC (ARDrone_Module_EventHandler& operator= (const ARDrone_Module_EventHandler&))

  //typedef std::map<Stream_SessionId_t, enum ARDrone_StreamType> SESSIONID_TO_STREAM_MAP_T;
  //typedef SESSIONID_TO_STREAM_MAP_T::iterator SESSIONID_TO_STREAM_MAP_ITERATOR_T;
  //typedef std::pair<Stream_SessionId_t, enum ARDrone_StreamType> SESSIONID_TO_STREAM_PAIR_T;
  //struct SESSIONID_TO_STREAM_MAP_FIND_S
  // : public std::binary_function<SESSIONID_TO_STREAM_PAIR_T,
  //                               enum ARDrone_StreamType,
  //                               bool>
  //{
  //  inline bool operator() (const SESSIONID_TO_STREAM_PAIR_T& entry_in, enum ARDrone_StreamType value_in) const { return entry_in.second == value_in; };
  //};

  //SESSIONID_TO_STREAM_MAP_T streams_;
};
typedef ARDrone_Module_EventHandler::READER_TASK_T ARDrone_EventHandler_ReaderTask_t;

// declare module
DATASTREAM_MODULE_DUPLEX_A (struct ARDrone_SessionData,                // session data type
                            enum Stream_SessionMessageType,            // session event type
                            struct ARDrone_ModuleHandlerConfiguration, // module handler configuration type
                            Stream_INotify_t,                          // stream notification interface type
                            ARDrone_EventHandler_ReaderTask_t,         // reader type
                            ARDrone_Module_EventHandler,               // writer type
                            ARDrone_Module_EventHandler);              // name

#endif
