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

#ifndef ARDRONE_MODULE_PAVE_DECODER_H
#define ARDRONE_MODULE_PAVE_DECODER_H

#include "ace/Global_Macros.h"

#include "common_time_common.h"

#include "stream_task_base_synch.h"

#include "ardrone_types.h"

// forward declaration(s)
class ACE_Message_Block;
class Stream_IAllocator;

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          ////////////////////////////////
          typename ConfigurationType,
          ////////////////////////////////
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          ////////////////////////////////
          typename SessionDataContainerType>
class ARDrone_Module_PaVEDecoder_T
 : public Stream_TaskBaseSynch_T<ACE_SYNCH_USE,
                                 TimePolicyType,
                                 ConfigurationType,
                                 ControlMessageType,
                                 DataMessageType,
                                 SessionMessageType,
                                 Stream_SessionId_t,
                                 enum Stream_ControlType,
                                 enum Stream_SessionMessageType,
                                 struct Stream_UserData>
{
  typedef Stream_TaskBaseSynch_T<ACE_SYNCH_USE,
                                 TimePolicyType,
                                 ConfigurationType,
                                 ControlMessageType,
                                 DataMessageType,
                                 SessionMessageType,
                                 Stream_SessionId_t,
                                 enum Stream_ControlType,
                                 enum Stream_SessionMessageType,
                                 struct Stream_UserData> inherited;

 public:
  ARDrone_Module_PaVEDecoder_T (typename inherited::ISTREAM_T*); // stream handle
  virtual ~ARDrone_Module_PaVEDecoder_T ();

  //// override (part of) Stream_IModuleHandler_T
  virtual bool initialize (const ConfigurationType&,
                           Stream_IAllocator* = NULL);
  //virtual const ConfigurationType& get () const;

  // implement (part of) Stream_ITaskBase
  virtual void handleDataMessage (DataMessageType*&, // data message handle
                                  bool&);            // return value: pass message downstream ?
  virtual void handleSessionMessage (SessionMessageType*&, // session message handle
                                     bool&);               // return value: pass message downstream ?

 private:
  ACE_UNIMPLEMENTED_FUNC (ARDrone_Module_PaVEDecoder_T ())
  ACE_UNIMPLEMENTED_FUNC (ARDrone_Module_PaVEDecoder_T (const ARDrone_Module_PaVEDecoder_T&))
  ACE_UNIMPLEMENTED_FUNC (ARDrone_Module_PaVEDecoder_T& operator= (const ARDrone_Module_PaVEDecoder_T&))

  // helper methods
  DataMessageType* allocateMessage (unsigned int); // requested size

  Stream_IAllocator*                             allocator_;
  DataMessageType*                               buffer_; // <-- continuation chain
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct ARDrone_ParrotVideoEncapsulation_Header header_;
#else
  parrot_video_encapsulation_t                   header_;
#endif
  bool                                           headerDecoded_;
};

// include template definition
#include "ardrone_module_pave_decoder.inl"

#endif
