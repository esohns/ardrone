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

#ifndef ardrone_MESSAGE_H
#define ardrone_MESSAGE_H

#include <string>

#include <ace/Global_Macros.h>

#include "stream_common.h"
#include "stream_control_message.h"
#include "stream_message_base.h"
#include "stream_messageallocatorheap_base.h"

//#include "ardrone_sessionmessage.h"
#include "ardrone_types.h"

// forward declaration(s)
class ACE_Allocator;
class ACE_Data_Block;
class ACE_Message_Block;
class ARDrone_Message;
class ARDrone_SessionMessage;
typedef Stream_ControlMessage_T<enum Stream_ControlType,
                                enum Stream_ControlMessageType,
                                struct ARDrone_AllocatorConfiguration> ARDrone_ControlMessage_t;

class ARDrone_Message
 : public Stream_MessageBase_T<struct ARDrone_AllocatorConfiguration,
                               enum ARDrone_MessageType,
                               int>
 //: public Stream_DataMessageBase_T<ardrone_MessageType>
{
  // enable access to specific private ctors
  friend class Stream_MessageAllocatorHeapBase_T<ACE_MT_SYNCH,
                                                 struct ARDrone_AllocatorConfiguration,
                                                 ARDrone_ControlMessage_t,
                                                 ARDrone_Message,
                                                 ARDrone_SessionMessage>;

 public:
  ARDrone_Message (unsigned int);
  virtual ~ARDrone_Message ();

  virtual int command () const; // return value: message type
  static std::string CommandType2String (int);

  // overrides from ACE_Message_Block
  // --> create a "shallow" copy that references the same datablock
  // *NOTE*: this uses the allocator (if any) to create a new message
  virtual ACE_Message_Block* duplicate (void) const;

 protected:
  // copy ctor to be used by duplicate() and child classes
  // --> uses an (incremented refcount of) the same datablock ("shallow copy")
  ARDrone_Message (const ARDrone_Message&);

 private:
  typedef Stream_MessageBase_T<struct ARDrone_AllocatorConfiguration,
                               enum ARDrone_MessageType,
                               int> inherited;
  //typedef Stream_MessageBase_T<ardrone_MessageType> inherited;

  ACE_UNIMPLEMENTED_FUNC (ARDrone_Message ())
  // *NOTE*: to be used by allocators...
  ARDrone_Message (ACE_Data_Block*, // data block to use
                   ACE_Allocator*,  // message allocator
                   bool = true);    // increment running message counter ?
  ACE_UNIMPLEMENTED_FUNC (ARDrone_Message& operator= (const ARDrone_Message&))
};

#endif
