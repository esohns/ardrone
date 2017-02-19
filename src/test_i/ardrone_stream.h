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

#ifndef ARDRONE_STREAM_T_H
#define ARDRONE_STREAM_T_H

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <minwindef.h>
#include <mfidl.h>
#include <strmif.h>
#endif

#include <ace/Global_Macros.h>
#include <ace/Synch_Traits.h>

#include "common_time_common.h"

#include "stream_base.h"
#include "stream_common.h"
#include "stream_statemachine_control.h"

#include "ardrone_message.h"
#include "ardrone_sessionmessage.h"
#include "ardrone_stream_common.h"

template <typename SourceModuleType>
class ARDrone_VideoStream_T
 : public Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct ARDrone_StreamState,
                        struct ARDrone_StreamConfiguration,
                        ARDrone_RuntimeStatistic_t,
                        struct Stream_ModuleConfiguration,
                        struct ARDrone_ModuleHandlerConfiguration,
                        struct ARDrone_SessionData,
                        ARDrone_StreamSessionData_t,
                        ARDrone_ControlMessage_t,
                        ARDrone_Message,
                        ARDrone_SessionMessage>
{
 public:
  ARDrone_VideoStream_T ();
  virtual ~ARDrone_VideoStream_T ();

  // implement (part of) Stream_IStreamControlBase
  virtual bool load (Stream_ModuleList_t&, // return value: module list
                     bool&);               // return value: delete modules ?

  // implement Common_IInitialize_T
  virtual bool initialize (const struct ARDrone_StreamConfiguration&, // configuration
                           bool = true,                               // setup pipeline ?
                           bool = true);                              // reset session data ?

  // implement Common_IStatistic_T
  // *NOTE*: these delegate to runtimeStatistic_
  virtual bool collect (ARDrone_RuntimeStatistic_t&); // return value: statistic data
  virtual void report () const;

 private:
  typedef Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct ARDrone_StreamState,
                        struct ARDrone_StreamConfiguration,
                        ARDrone_RuntimeStatistic_t,
                        struct Stream_ModuleConfiguration,
                        struct ARDrone_ModuleHandlerConfiguration,
                        struct ARDrone_SessionData,
                        ARDrone_StreamSessionData_t,
                        ARDrone_ControlMessage_t,
                        ARDrone_Message,
                        ARDrone_SessionMessage> inherited;

  ACE_UNIMPLEMENTED_FUNC (ARDrone_VideoStream_T (const ARDrone_VideoStream_T&))
  ACE_UNIMPLEMENTED_FUNC (ARDrone_VideoStream_T& operator= (const ARDrone_VideoStream_T&))

  // *TODO*: re-consider this API
  void ping ();

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  IGraphBuilder*   graphBuilder_;
  IMFMediaSession* mediaSession_;
#endif
};

//typedef ARDrone_VideoStream_T<ARDrone_Module_NetSource_Module> ARDrone_Stream_t;
typedef ARDrone_VideoStream_T<ARDrone_Module_AsynchNetSource_Module> ARDrone_AsynchStream_t;

//////////////////////////////////////////

class ARDrone_NavDataStream
 : public Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct ARDrone_StreamState,
                        struct ARDrone_StreamConfiguration,
                        ARDrone_RuntimeStatistic_t,
                        struct Stream_ModuleConfiguration,
                        struct ARDrone_ModuleHandlerConfiguration,
                        struct ARDrone_SessionData,
                        ARDrone_StreamSessionData_t,
                        ARDrone_ControlMessage_t,
                        ARDrone_Message,
                        ARDrone_SessionMessage>
{
 public:
  ARDrone_NavDataStream ();
  virtual ~ARDrone_NavDataStream ();

  // implement (part of) Stream_IStreamControlBase
  virtual bool load (Stream_ModuleList_t&, // return value: module list
                     bool&);               // return value: delete modules ?

  // implement Common_IInitialize_T
  virtual bool initialize (const struct ARDrone_StreamConfiguration&, // configuration
                           bool = true,                               // setup pipeline ?
                           bool = true);                              // reset session data ?

  // implement Common_IStatistic_T
  // *NOTE*: these delegate to runtimeStatistic_
  virtual bool collect (ARDrone_RuntimeStatistic_t&); // return value: statistic data
  virtual void report () const;

 private:
  typedef Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct ARDrone_StreamState,
                        struct ARDrone_StreamConfiguration,
                        ARDrone_RuntimeStatistic_t,
                        struct Stream_ModuleConfiguration,
                        struct ARDrone_ModuleHandlerConfiguration,
                        struct ARDrone_SessionData,
                        ARDrone_StreamSessionData_t,
                        ARDrone_ControlMessage_t,
                        ARDrone_Message,
                        ARDrone_SessionMessage> inherited;

  ACE_UNIMPLEMENTED_FUNC (ARDrone_NavDataStream (const ARDrone_NavDataStream&))
  ACE_UNIMPLEMENTED_FUNC (ARDrone_NavDataStream& operator= (const ARDrone_NavDataStream&))

  // *TODO*: re-consider this API
  void ping ();
};

//////////////////////////////////////////

class ARDrone_MAVLinkStream
 : public Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct ARDrone_StreamState,
                        struct ARDrone_StreamConfiguration,
                        ARDrone_RuntimeStatistic_t,
                        struct Stream_ModuleConfiguration,
                        struct ARDrone_ModuleHandlerConfiguration,
                        struct ARDrone_SessionData,
                        ARDrone_StreamSessionData_t,
                        ARDrone_ControlMessage_t,
                        ARDrone_Message,
                        ARDrone_SessionMessage>
{
 public:
  ARDrone_MAVLinkStream ();
  virtual ~ARDrone_MAVLinkStream ();

  // implement (part of) Stream_IStreamControlBase
  virtual bool load (Stream_ModuleList_t&, // return value: module list
                     bool&);               // return value: delete modules ?

  // implement Common_IInitialize_T
  virtual bool initialize (const struct ARDrone_StreamConfiguration&, // configuration
                           bool = true,                               // setup pipeline ?
                           bool = true);                              // reset session data ?

  // implement Common_IStatistic_T
  // *NOTE*: these delegate to runtimeStatistic_
  virtual bool collect (ARDrone_RuntimeStatistic_t&); // return value: statistic data
  virtual void report () const;

 private:
  typedef Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct ARDrone_StreamState,
                        struct ARDrone_StreamConfiguration,
                        ARDrone_RuntimeStatistic_t,
                        struct Stream_ModuleConfiguration,
                        struct ARDrone_ModuleHandlerConfiguration,
                        struct ARDrone_SessionData,
                        ARDrone_StreamSessionData_t,
                        ARDrone_ControlMessage_t,
                        ARDrone_Message,
                        ARDrone_SessionMessage> inherited;

  ACE_UNIMPLEMENTED_FUNC (ARDrone_MAVLinkStream (const ARDrone_MAVLinkStream&))
  ACE_UNIMPLEMENTED_FUNC (ARDrone_MAVLinkStream& operator= (const ARDrone_MAVLinkStream&))

  // *TODO*: re-consider this API
  void ping ();
};

// include template definition
#include "ardrone_stream.inl"

#endif
