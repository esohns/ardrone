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

#ifndef ARDrone_SESSIONMESSAGE_H
#define ARDrone_SESSIONMESSAGE_H

#include "ace/Global_Macros.h"

#include "stream_common.h"
#include "stream_messageallocatorheap_base.h"
#include "stream_session_message_base.h"

#include "ardrone_message.h"
#include "ardrone_stream_common.h"
#include "ardrone_types.h"

// forward declarations
class ACE_Allocator;
class ACE_Data_Block;
class ACE_Message_Block;

class ARDrone_SessionMessage
 : public Stream_SessionMessageBase_T<struct ARDRone_AllocatorConfiguration,
                                      enum Stream_SessionMessageType,
                                      ARDrone_StreamSessionData_t,
                                      struct ARDrone_UserData>
{
  // enable access to private ctor(s)
  friend class Stream_MessageAllocatorHeapBase_T<ACE_MT_SYNCH,
                                                 struct ARDRone_AllocatorConfiguration,
                                                 ARDrone_ControlMessage_t,
                                                 ARDrone_Message,
                                                 ARDrone_SessionMessage>;

 public:
  // *NOTE*: assume lifetime responsibility for the third argument
  ARDrone_SessionMessage (Stream_SessionId_t,             // session id
                          enum Stream_SessionMessageType, // session message type
                          ARDrone_StreamSessionData_t*&,  // session data handle
                          struct ARDrone_UserData*);      // user data handle
  // *NOTE*: to be used by message allocators
  ARDrone_SessionMessage (Stream_SessionId_t, // session id
                          ACE_Allocator*);    // message allocator
  ARDrone_SessionMessage (Stream_SessionId_t, // session id
                          ACE_Data_Block*,    // data block to use
                          ACE_Allocator*);    // message allocator
  virtual ~ARDrone_SessionMessage ();

  // override from ACE_Message_Block
  // *WARNING*: any children need to override this as well
  virtual ACE_Message_Block* duplicate (void) const;

 private:
  typedef Stream_SessionMessageBase_T<struct ARDRone_AllocatorConfiguration,
                                      enum Stream_SessionMessageType,
                                      ARDrone_StreamSessionData_t,
                                      struct ARDrone_UserData> inherited;

  ACE_UNIMPLEMENTED_FUNC (ARDrone_SessionMessage ())
  // copy ctor (to be used by duplicate())
  ARDrone_SessionMessage (const ARDrone_SessionMessage&);
  ACE_UNIMPLEMENTED_FUNC (ARDrone_SessionMessage& operator= (const ARDrone_SessionMessage&))
};

#endif
