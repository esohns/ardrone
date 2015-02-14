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

#ifndef OLIMEX_MOD_MPU6050_MESSAGE_H
#define OLIMEX_MOD_MPU6050_MESSAGE_H

#include "ace/Global_Macros.h"

#include "stream_message_base.h"

// forward declaration(s)
class ACE_Allocator;
class ACE_Data_Block;
class ACE_Message_Block;
//class Olimex_Mod_MPU6050_MessageAllocator;
template <typename MessageType,
          typename SessionMessageType> class Stream_MessageAllocatorHeapBase_T;
class Olimex_Mod_MPU6050_SessionMessage;

class Olimex_Mod_MPU6050_Message
 : public Stream_MessageBase
{
  // enable access to specific private ctors...
//  friend class Olimex_Mod_MPU6050_MessageAllocator;
  friend class Stream_MessageAllocatorHeapBase_T<Olimex_Mod_MPU6050_Message,
                                                 Olimex_Mod_MPU6050_SessionMessage>;

 public:
  virtual ~Olimex_Mod_MPU6050_Message ();

  // overrides from ACE_Message_Block
  // --> create a "shallow" copy that references the same datablock
  // *NOTE*: this uses the allocator (if any) to create a new message
  virtual ACE_Message_Block* duplicate (void) const;

 protected:
  // copy ctor to be used by duplicate() and child classes
  // --> uses an (incremented refcount of) the same datablock ("shallow copy")
  Olimex_Mod_MPU6050_Message (const Olimex_Mod_MPU6050_Message&);

 private:
  typedef Stream_MessageBase inherited;

  ACE_UNIMPLEMENTED_FUNC (Olimex_Mod_MPU6050_Message ());
  // *NOTE*: to be used by allocators...
  Olimex_Mod_MPU6050_Message (ACE_Data_Block*, // data block to use
                              ACE_Allocator*); // message allocator
  ACE_UNIMPLEMENTED_FUNC (Olimex_Mod_MPU6050_Message& operator= (const Olimex_Mod_MPU6050_Message&));
};

#endif
