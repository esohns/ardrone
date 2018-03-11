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

//#include <map>

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

extern const char ardrone_default_handler_module_name_string[];

template <typename ConfigurationType>
class ARDrone_Module_EventHandler_T
 : public Stream_Module_MessageHandlerA_T<ACE_MT_SYNCH,
                                          Common_TimePolicy_t,
                                          ConfigurationType,
                                          ARDrone_ControlMessage_t,
                                          ARDrone_Message,
                                          ARDrone_SessionMessage,
                                          Stream_SessionId_t,
                                          struct ARDrone_SessionData,
                                          struct ARDrone_UserData>
{
  typedef Stream_Module_MessageHandlerA_T<ACE_MT_SYNCH,
                                          Common_TimePolicy_t,
                                          ConfigurationType,
                                          ARDrone_ControlMessage_t,
                                          ARDrone_Message,
                                          ARDrone_SessionMessage,
                                          Stream_SessionId_t,
                                          struct ARDrone_SessionData,
                                          struct ARDrone_UserData> inherited;

 public:
  ARDrone_Module_EventHandler_T (typename inherited::ISTREAM_T*); // stream handle
  inline virtual ~ARDrone_Module_EventHandler_T () {}

  // override (part of) Stream_ITaskBase_T
  virtual void handleDataMessage (ARDrone_Message*&, // message handle
                                  bool&);            // return value: pass message downstream ?
  virtual void handleSessionMessage (ARDrone_SessionMessage*&, // session message handle
                                     bool&);                   // return value: pass message downstream ?

  // implement Common_IClone_T
  virtual ACE_Task<ACE_MT_SYNCH,
                   Common_TimePolicy_t>* clone ();

 private:
  ACE_UNIMPLEMENTED_FUNC (ARDrone_Module_EventHandler_T ())
  ACE_UNIMPLEMENTED_FUNC (ARDrone_Module_EventHandler_T (const ARDrone_Module_EventHandler_T&))
  ACE_UNIMPLEMENTED_FUNC (ARDrone_Module_EventHandler_T& operator= (const ARDrone_Module_EventHandler_T&))

  // convenient types
  typedef ARDrone_Module_EventHandler_T<ConfigurationType> OWN_TYPE_T;
  typedef Stream_StreamModuleA_T<ACE_MT_SYNCH,
                                 Common_TimePolicy_t,
                                 Stream_SessionId_t,
                                 struct ARDrone_SessionData,
                                 enum Stream_SessionMessageType,
                                 struct Stream_ModuleConfiguration,
                                 ConfigurationType,
                                 ardrone_default_handler_module_name_string,
                                 Stream_INotify_t,
                                 typename OWN_TYPE_T::READER_TASK_T,
                                 OWN_TYPE_T> MODULE_T;
};

// include template definition
#include "ardrone_module_eventhandler.inl"

//////////////////////////////////////////

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef ARDrone_Module_EventHandler_T<struct ARDrone_DirectShow_ModuleHandlerConfiguration> ARDrone_Module_DirectShow_EventHandler_t;
typedef ARDrone_Module_EventHandler_T<struct ARDrone_MediaFoundation_ModuleHandlerConfiguration> ARDrone_Module_MediaFoundation_EventHandler_t;

// declare module
DATASTREAM_MODULE_DUPLEX_A (struct ARDrone_SessionData,                              // session data type
                            enum Stream_SessionMessageType,                          // session event type
                            struct ARDrone_DirectShow_ModuleHandlerConfiguration,    // module handler configuration type
                            ardrone_default_handler_module_name_string,              // module name
                            Stream_INotify_t,                                        // stream notification interface type
                            ARDrone_Module_DirectShow_EventHandler_t::READER_TASK_T, // reader type
                            ARDrone_Module_DirectShow_EventHandler_t,                // writer type
                            ARDrone_Module_DirectShow_EventHandler);                 // class name
DATASTREAM_MODULE_DUPLEX_A (struct ARDrone_SessionData,                                   // session data type
                            enum Stream_SessionMessageType,                               // session event type
                            struct ARDrone_MediaFoundation_ModuleHandlerConfiguration,    // module handler configuration type
                            ardrone_default_handler_module_name_string,                   // module name
                            Stream_INotify_t,                                             // stream notification interface type
                            ARDrone_Module_MediaFoundation_EventHandler_t::READER_TASK_T, // reader type
                            ARDrone_Module_MediaFoundation_EventHandler_t,                // writer type
                            ARDrone_Module_MediaFoundation_EventHandler);                 // class name
#else
typedef ARDrone_Module_EventHandler_T<struct ARDrone_ModuleHandlerConfiguration> ARDrone_Module_EventHandler_t;

// declare module
DATASTREAM_MODULE_DUPLEX_A (struct ARDrone_SessionData,                   // session data type
                            enum Stream_SessionMessageType,               // session event type
                            struct ARDrone_ModuleHandlerConfiguration,    // module handler configuration type
                            ardrone_default_handler_module_name_string,   // module name
                            Stream_INotify_t,                             // stream notification interface type
                            ARDrone_Module_EventHandler_t::READER_TASK_T, // reader type
                            ARDrone_Module_EventHandler_t,                // writer type
                            ARDrone_Module_EventHandler);                 // class name
#endif

#endif
