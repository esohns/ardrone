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

#ifndef OLIMEX_MOD_MPU6050_MESSAGE_BASE_H
#define OLIMEX_MOD_MPU6050_MESSAGE_BASE_H

#include "ace/Global_Macros.h"
#include "ace/Message_Block.h"
#include "ace/Atomic_Op.h"
#include "ace/Synch.h"

#include <string>

// forward declaration(s)
class ACE_Allocator;

class Olimex_Mod_MPU6050_MessageBase
 : public ACE_Message_Block
{
 public:
  virtual ~Olimex_Mod_MPU6050_MessageBase ();

  // used for pre-allocated messages...
  virtual void init (// Stream_MessageBase members
                     ACE_Data_Block*); // data block to use

  // message types
  enum MessageType
  {
    // *NOTE*: see <ace/Message_Block.h> for details...
    MB_BEGIN_STREAM_SESSION_MAP = ACE_Message_Block::MB_USER,
    // *** STREAM CONTROL ***
    MB_STREAM_SESSION,
    // *** STREAM CONTROL - END ***
    MB_BEGIN_STREAM_DATA_MAP = 0x300,
    // *** STREAM DATA ***
    MB_STREAM_DATA, // protocol data
    MB_STREAM_OBJ,  // OO type --> can be downcast dynamically
    // *** STREAM DATA - END ***
    MB_BEGIN_PROTOCOL_MAP = 0x400
  };

  // info
  unsigned int getID () const;

  // reset atomic id generator
  static void resetMessageIDGenerator ();

  // helper methods
  static void messageType2String (const ACE_Message_Type&, // as returned by msg_type()
                                  std::string&);           // return value: type string

 protected:
  // copy ctor, to be used by children
  Olimex_Mod_MPU6050_MessageBase (const Olimex_Mod_MPU6050_MessageBase&);

  // *NOTE*: to be used by message allocators...
  Olimex_Mod_MPU6050_MessageBase (ACE_Data_Block*, // data block
                                  ACE_Allocator*,  // message allocator
                                  bool = true);    // increment running message counter ?
  Olimex_Mod_MPU6050_MessageBase (ACE_Allocator*); // message allocator

  // overrides from ACE_Message_Block
  // *WARNING*: most probably, any children need to override this as well !
  virtual ACE_Message_Block* duplicate (void) const;

 private:
  typedef ACE_Message_Block inherited;

  ACE_UNIMPLEMENTED_FUNC (Olimex_Mod_MPU6050_MessageBase ());
  ACE_UNIMPLEMENTED_FUNC (Olimex_Mod_MPU6050_MessageBase& operator= (const Olimex_Mod_MPU6050_MessageBase&));

  // atomic ID generator
  static ACE_Atomic_Op<ACE_Thread_Mutex, unsigned int> currentID_;

  bool         initialized_;
  unsigned int messageID_;
};

#endif
