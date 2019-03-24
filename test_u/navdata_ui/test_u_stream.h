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

#ifndef TEST_U_STREAM_H
#define TEST_U_STREAM_H

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#if COMMON_OS_WIN32_TARGET_PLATFORM(0x0602) // _WIN32_WINNT_WIN8
#include <minwindef.h>
#else
#include <windef.h>
#endif // COMMON_OS_WIN32_TARGET_PLATFORM(0x0602)
#include <winnt.h>
#include <guiddef.h>
#include <mfidl.h>
#include <mfobjects.h>
#endif // ACE_WIN32 || ACE_WIN64

#include "ace/Global_Macros.h"
#include "ace/Synch_Traits.h"

#include "common_time_common.h"

#include "stream_base.h"
#include "stream_common.h"

#include "ardrone_common.h"

#include "test_u_common.h"
#include "test_u_common_modules.h"
#include "test_u_net_modules.h"
#include "test_u_message.h"
#include "test_u_session_message.h"

// forward declarations
class Stream_IAllocator;

extern const char stream_name_string_[];

class Test_U_Stream
 : public Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        stream_name_string_,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct Test_U_StreamState,
                        struct Test_U_StreamConfiguration,
                        struct Test_U_StatisticData,
                        struct Stream_AllocatorConfiguration,
                        struct Stream_ModuleConfiguration,
                        struct Test_U_ModuleHandlerConfiguration,
                        Test_U_SessionData,
                        Test_U_SessionData_t,
                        Stream_ControlMessage_t,
                        Test_U_Message_t,
                        Test_U_SessionMessage_t>
 , public ARDrone_INavDataNotify
{
  typedef Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        stream_name_string_,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct Test_U_StreamState,
                        struct Test_U_StreamConfiguration,
                        struct Test_U_StatisticData,
                        struct Stream_AllocatorConfiguration,
                        struct Stream_ModuleConfiguration,
                        struct Test_U_ModuleHandlerConfiguration,
                        Test_U_SessionData,
                        Test_U_SessionData_t,
                        Stream_ControlMessage_t,
                        Test_U_Message_t,
                        Test_U_SessionMessage_t> inherited;

 public:
  Test_U_Stream ();
  virtual ~Test_U_Stream ();

  // implement (part of) Stream_IStreamControlBase
  virtual bool load (Stream_ILayout*, // layout
                     bool&);          // return value: delete modules ?

  // implement Common_IInitialize_T
  virtual bool initialize (const typename inherited::CONFIGURATION_T&); // configuration

  // implement ARDrone_IMAVLinkNotify
  virtual void messageCB (const struct _navdata_t&,              // message record
                          const ARDrone_NavDataOptionOffsets_t&, // option offsets
                          void*);                          // payload handle

 private:
  ACE_UNIMPLEMENTED_FUNC (Test_U_Stream (const Test_U_Stream&))
  ACE_UNIMPLEMENTED_FUNC (Test_U_Stream& operator= (const Test_U_Stream&))

  // modules
  Test_U_AsynchUDPSource_Module source_;
  Test_U_NavDataDecoder_Module  decode_;
  Test_U_StatisticReport_Module report_;
  Test_U_AsynchController_Module controller_;
  Test_U_MessageHandler_Module handler_;
};

#endif
