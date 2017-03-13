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

#include <ace/Global_Macros.h>
#include <ace/Synch_Traits.h>

#include "common_time_common.h"

#include "stream_common.h"
#include "stream_streammodule_base.h"

#include "stream_misc_messagehandler.h"

#include "ardrone_message.h"
#include "ardrone_sessionmessage.h"
#include "ardrone_stream_common.h"
#include "ardrone_types.h"

class ARDrone_Module_EventHandler
 : public Stream_Module_MessageHandler_T<ACE_MT_SYNCH,
                                         Common_TimePolicy_t,
                                         struct ARDrone_ModuleHandlerConfiguration,
                                         ARDrone_ControlMessage_t,
                                         ARDrone_Message,
                                         ARDrone_SessionMessage,
                                         Stream_SessionId_t,
                                         struct ARDrone_SessionData>
{
 public:
  ARDrone_Module_EventHandler ();
  virtual ~ARDrone_Module_EventHandler ();

  // implement Common_IClone_T
  virtual ACE_Task<ACE_MT_SYNCH,
                   Common_TimePolicy_t>* clone ();

 private:
  typedef Stream_Module_MessageHandler_T<ACE_MT_SYNCH,
                                         Common_TimePolicy_t,
                                         struct ARDrone_ModuleHandlerConfiguration,
                                         ARDrone_ControlMessage_t,
                                         ARDrone_Message,
                                         ARDrone_SessionMessage,
                                         Stream_SessionId_t,
                                         struct ARDrone_SessionData> inherited;

  ACE_UNIMPLEMENTED_FUNC (ARDrone_Module_EventHandler (const ARDrone_Module_EventHandler&))
  ACE_UNIMPLEMENTED_FUNC (ARDrone_Module_EventHandler& operator= (const ARDrone_Module_EventHandler&))
};

// declare module
DATASTREAM_MODULE_INPUT_ONLY (struct ARDrone_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_ModuleHandlerConfiguration, // module handler configuration type
                              ARDrone_IStreamNotify_t,                   // stream notification interface type
                              ARDrone_Module_EventHandler);              // writer type

#endif
