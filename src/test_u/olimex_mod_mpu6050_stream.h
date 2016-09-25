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

#ifndef OLIMEX_MOD_MPU6050_STREAM_H
#define OLIMEX_MOD_MPU6050_STREAM_H

#include "ace/Global_Macros.h"
#include "ace/Synch_Traits.h"

#include "common_time_common.h"

#include "stream_base.h"
#include "stream_common.h"
#include "stream_statemachine_control.h"

#include "olimex_mod_mpu6050_message.h"
#include "olimex_mod_mpu6050_modules_common.h"
#include "olimex_mod_mpu6050_sessionmessage.h"
#include "olimex_mod_mpu6050_types.h"

template <typename SourceModuleType>
class Olimex_Mod_MPU6050_Stream_T
 : public Stream_Base_T<ACE_MT_SYNCH,
                        ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        Stream_ControlType,
                        Stream_SessionMessageType,
                        Stream_StateMachine_ControlState,
                        Olimex_Mod_MPU6050_StreamState,
                        Olimex_Mod_MPU6050_StreamConfiguration,
                        Olimex_Mod_MPU6050_RuntimeStatistic_t,
                        Stream_ModuleConfiguration,
                        Olimex_Mod_MPU6050_ModuleHandlerConfiguration,
                        Olimex_Mod_MPU6050_SessionData,
                        Olimex_Mod_MPU6050_StreamSessionData_t,
                        Olimex_Mod_MPU6050_ControlMessage_t,
                        Olimex_Mod_MPU6050_Message,
                        Olimex_Mod_MPU6050_SessionMessage>
{
 public:
  Olimex_Mod_MPU6050_Stream_T ();
  virtual ~Olimex_Mod_MPU6050_Stream_T ();

  // implement (part of) Stream_IStreamControlBase
  virtual bool load (Stream_ModuleList_t&, // return value: module list
                     bool&);               // return value: delete modules ?

  // implement Common_IInitialize_T
  virtual bool initialize (const Olimex_Mod_MPU6050_StreamConfiguration&, // configuration
                           bool = true,                                   // setup pipeline ?
                           bool = true);                                  // reset session data ?

  // implement Common_IStatistic_T
  // *NOTE*: these delegate to runtimeStatistic_
  virtual bool collect (Olimex_Mod_MPU6050_RuntimeStatistic_t&); // return value: statistic data
  virtual void report () const;

 private:
  typedef Stream_Base_T<ACE_MT_SYNCH,
                        ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        Stream_ControlType,
                        Stream_SessionMessageType,
                        Stream_StateMachine_ControlState,
                        Olimex_Mod_MPU6050_StreamState,
                        Olimex_Mod_MPU6050_StreamConfiguration,
                        Olimex_Mod_MPU6050_RuntimeStatistic_t,
                        Stream_ModuleConfiguration,
                        Olimex_Mod_MPU6050_ModuleHandlerConfiguration,
                        Olimex_Mod_MPU6050_SessionData,
                        Olimex_Mod_MPU6050_StreamSessionData_t,
                        Olimex_Mod_MPU6050_ControlMessage_t,
                        Olimex_Mod_MPU6050_Message,
                        Olimex_Mod_MPU6050_SessionMessage> inherited;

  ACE_UNIMPLEMENTED_FUNC (Olimex_Mod_MPU6050_Stream_T (const Olimex_Mod_MPU6050_Stream_T&))
  ACE_UNIMPLEMENTED_FUNC (Olimex_Mod_MPU6050_Stream_T& operator= (const Olimex_Mod_MPU6050_Stream_T&))

  // *TODO*: re-consider this API
  void ping ();
};

typedef Olimex_Mod_MPU6050_Stream_T<Olimex_Mod_MPU6050_Module_SocketHandler_Module> Olimex_Mod_MPU6050_Stream_t;
typedef Olimex_Mod_MPU6050_Stream_T<Olimex_Mod_MPU6050_Module_AsynchSocketHandler_Module> Olimex_Mod_MPU6050_AsynchStream_t;

#include "olimex_mod_mpu6050_stream.inl"

#endif
