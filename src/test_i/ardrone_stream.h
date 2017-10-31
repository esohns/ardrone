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

#include "ace/Global_Macros.h"
#include "ace/Synch_Traits.h"

#include "common_time_common.h"

#include "stream_base.h"
#include "stream_common.h"
#include "stream_session_base.h"
#include "stream_statemachine_control.h"

#include "ardrone_message.h"
#include "ardrone_modules_common.h"
#include "ardrone_sessionmessage.h"
#include "ardrone_stream_common.h"
#include "ardrone_types.h"

extern const char video_stream_name_string_[];
extern const char control_stream_name_string_[];
extern const char navdata_stream_name_string_[];
extern const char mavlink_stream_name_string_[];

template <typename SourceModuleType>
class ARDrone_VideoStream_T
 : public Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        video_stream_name_string_,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct ARDrone_StreamState,
                        struct ARDrone_StreamConfiguration,
                        struct ARDrone_Statistic,
                        struct ARDrone_AllocatorConfiguration,
                        struct Stream_ModuleConfiguration,
                        struct ARDrone_ModuleHandlerConfiguration,
                        struct ARDrone_SessionData,
                        ARDrone_StreamSessionData_t,
                        ARDrone_ControlMessage_t,
                        ARDrone_Message,
                        ARDrone_SessionMessage>
{
  typedef Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        video_stream_name_string_,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct ARDrone_StreamState,
                        struct ARDrone_StreamConfiguration,
                        struct ARDrone_Statistic,
                        struct ARDrone_AllocatorConfiguration,
                        struct Stream_ModuleConfiguration,
                        struct ARDrone_ModuleHandlerConfiguration,
                        struct ARDrone_SessionData,
                        ARDrone_StreamSessionData_t,
                        ARDrone_ControlMessage_t,
                        ARDrone_Message,
                        ARDrone_SessionMessage> inherited;

 public:
  ARDrone_VideoStream_T ();
  virtual ~ARDrone_VideoStream_T ();

  // implement (part of) Stream_IStreamControlBase
  virtual bool load (Stream_ModuleList_t&, // return value: module list
                     bool&);               // return value: delete modules ?

  // implement Common_IInitialize_T
  virtual bool initialize (const typename inherited::CONFIGURATION_T&); // configuration

  // implement Common_IStatistic_T
  // *NOTE*: these delegate to runtimeStatistic_
  virtual bool collect (struct ARDrone_Statistic&); // return value: statistic data
  virtual void report () const;

 private:
  ACE_UNIMPLEMENTED_FUNC (ARDrone_VideoStream_T (const ARDrone_VideoStream_T&))
  ACE_UNIMPLEMENTED_FUNC (ARDrone_VideoStream_T& operator= (const ARDrone_VideoStream_T&))

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  IGraphBuilder*   graphBuilder_;
  IMFMediaSession* mediaSession_;
#endif
};

typedef ARDrone_VideoStream_T<ARDrone_Module_AsynchTCPSource_Module> ARDrone_AsynchVideoStream_t;
typedef ARDrone_VideoStream_T<ARDrone_Module_TCPSource_Module> ARDrone_VideoStream_t;

//////////////////////////////////////////

class ARDrone_ControlStream
 : public Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        control_stream_name_string_,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct ARDrone_StreamState,
                        struct ARDrone_StreamConfiguration,
                        struct ARDrone_Statistic,
                        struct ARDrone_AllocatorConfiguration,
                        struct Stream_ModuleConfiguration,
                        struct ARDrone_ModuleHandlerConfiguration,
                        struct ARDrone_SessionData,
                        ARDrone_StreamSessionData_t,
                        ARDrone_ControlMessage_t,
                        ARDrone_Message,
                        ARDrone_SessionMessage>
 , public Stream_SessionBase_T<Stream_SessionId_t,
                               struct ARDrone_SessionData,
                               enum Stream_SessionMessageType,
                               ARDrone_Message,
                               ARDrone_SessionMessage>
 , public ARDrone_IControlNotify
{
  typedef Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        control_stream_name_string_,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct ARDrone_StreamState,
                        struct ARDrone_StreamConfiguration,
                        struct ARDrone_Statistic,
                        struct ARDrone_AllocatorConfiguration,
                        struct Stream_ModuleConfiguration,
                        struct ARDrone_ModuleHandlerConfiguration,
                        struct ARDrone_SessionData,
                        ARDrone_StreamSessionData_t,
                        ARDrone_ControlMessage_t,
                        ARDrone_Message,
                        ARDrone_SessionMessage> inherited;
  typedef Stream_SessionBase_T<Stream_SessionId_t,
                               struct ARDrone_SessionData,
                               enum Stream_SessionMessageType,
                               ARDrone_Message,
                               ARDrone_SessionMessage> inherited2;

 public:
  ARDrone_ControlStream ();
  inline virtual ~ARDrone_ControlStream () { inherited::shutdown (); }

  // implement (part of) Stream_IStreamControlBase
  virtual bool load (Stream_ModuleList_t&, // return value: module list
                     bool&);               // return value: delete modules ?

  // implement Common_IInitialize_T
  virtual bool initialize (const typename inherited::CONFIGURATION_T&); // configuration

  // implement Common_IStatistic_T
  virtual bool collect (struct ARDrone_Statistic&); // return value: statistic data
  virtual void report () const;

  // implement ARDrone_IControlNotify
  virtual void messageCB (const ARDrone_DeviceConfiguration_t&); // device configuration

 private:
  ACE_UNIMPLEMENTED_FUNC (ARDrone_ControlStream (const ARDrone_ControlStream&))
  ACE_UNIMPLEMENTED_FUNC (ARDrone_ControlStream& operator= (const ARDrone_ControlStream&))

  // convenient types
  struct SUBSCRIBERS_IS_EQUAL_P
  {
    inline bool operator() (typename inherited2::INOTIFY_T* first, typename inherited2::INOTIFY_T* second) { return (first == second); }
  };

  // override (part of) Stream_ISessionNotify_T
  virtual void start (Stream_SessionId_t,
                      const struct ARDrone_SessionData&);
  virtual void notify (Stream_SessionId_t,
                       const enum Stream_SessionMessageType&);
  virtual void end (Stream_SessionId_t);

  ARDrone_IDeviceConfiguration* configuration_;
};

//////////////////////////////////////////

class ARDrone_NavDataStream
 : public Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        navdata_stream_name_string_,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct ARDrone_StreamState,
                        struct ARDrone_StreamConfiguration,
                        struct ARDrone_Statistic,
                        struct ARDrone_AllocatorConfiguration,
                        struct Stream_ModuleConfiguration,
                        struct ARDrone_ModuleHandlerConfiguration,
                        struct ARDrone_SessionData,
                        ARDrone_StreamSessionData_t,
                        ARDrone_ControlMessage_t,
                        ARDrone_Message,
                        ARDrone_SessionMessage>
 , public Stream_SessionBase_T<Stream_SessionId_t,
                               struct ARDrone_SessionData,
                               enum Stream_SessionMessageType,
                               ARDrone_Message,
                               ARDrone_SessionMessage>
 , public ARDrone_INavDataNotify
 , public Net_IWLANCB
 , public Common_IGetP_T<ARDrone_IController>
{
  typedef Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        navdata_stream_name_string_,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct ARDrone_StreamState,
                        struct ARDrone_StreamConfiguration,
                        struct ARDrone_Statistic,
                        struct ARDrone_AllocatorConfiguration,
                        struct Stream_ModuleConfiguration,
                        struct ARDrone_ModuleHandlerConfiguration,
                        struct ARDrone_SessionData,
                        ARDrone_StreamSessionData_t,
                        ARDrone_ControlMessage_t,
                        ARDrone_Message,
                        ARDrone_SessionMessage> inherited;
  typedef Stream_SessionBase_T<Stream_SessionId_t,
                               struct ARDrone_SessionData,
                               enum Stream_SessionMessageType,
                               ARDrone_Message,
                               ARDrone_SessionMessage> inherited2;

 public:
  ARDrone_NavDataStream ();
  virtual ~ARDrone_NavDataStream ();

  // implement (part of) Stream_IStreamControlBase
  virtual bool load (Stream_ModuleList_t&, // return value: module list
                     bool&);               // return value: delete modules ?

  // implement Common_IInitialize_T
  virtual bool initialize (const typename inherited::CONFIGURATION_T&); // configuration

  // implement Common_IStatistic_T
  // *NOTE*: these delegate to runtimeStatistic_
  virtual bool collect (struct ARDrone_Statistic&); // return value: statistic data
  virtual void report () const;

  // implement ARDrone_INavDataNotify
  virtual void messageCB (const struct _navdata_t&,              // message record
                          const ARDrone_NavDataOptionOffsets_t&, // option offsets
                          void*);                                // payload handle

  // implement Common_IGetP_T
  virtual const ARDrone_IController* const getP () const;

 private:
  ACE_UNIMPLEMENTED_FUNC (ARDrone_NavDataStream (const ARDrone_NavDataStream&))
  ACE_UNIMPLEMENTED_FUNC (ARDrone_NavDataStream& operator= (const ARDrone_NavDataStream&))

  // convenient types
  struct SUBSCRIBERS_IS_EQUAL_P
  {
    inline bool operator() (typename inherited2::INOTIFY_T* first, typename inherited2::INOTIFY_T* second) { return (first == second); }
  };

  // override (part of) Stream_ISessionNotify_T
  virtual void start (Stream_SessionId_t,
                      const struct ARDrone_SessionData&);
  virtual void notify (Stream_SessionId_t,
                       const enum Stream_SessionMessageType&);
  virtual void end (Stream_SessionId_t);

  // implement Net_IWLANCB
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  virtual void onAssociate (REFGUID,            // device identifier
#else
  virtual void onAssociate (const std::string&, // device identifier
#endif
                            const std::string&, // SSID
                            bool);              // success ?
  // *IMPORTANT NOTE*: Net_IWLANMonitor_T::addresses() may not return
  //                   significant data before this, as the link layer
  //                   configuration (e.g. DHCP handshake, ...) most likely has
  //                   not been established
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  virtual void onConnect (REFGUID,            // device identifier
#else
  virtual void onConnect (const std::string&, // device identifier
#endif
                          const std::string&, // SSID
                          bool);              // success ?
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  virtual void onHotPlug (REFGUID,            // device identifier
#else
  virtual void onHotPlug (const std::string&, // device identifier
#endif
                          bool);              // enabled ? : disabled/removed
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  virtual void onScanComplete (REFGUID);            // device identifier
#else
  virtual void onScanComplete (const std::string&); // device identifier
#endif

  bool isFirst_;
};

//////////////////////////////////////////

class ARDrone_MAVLinkStream
 : public Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        mavlink_stream_name_string_,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct ARDrone_StreamState,
                        struct ARDrone_StreamConfiguration,
                        struct ARDrone_Statistic,
                        struct ARDrone_AllocatorConfiguration,
                        struct Stream_ModuleConfiguration,
                        struct ARDrone_ModuleHandlerConfiguration,
                        struct ARDrone_SessionData,
                        ARDrone_StreamSessionData_t,
                        ARDrone_ControlMessage_t,
                        ARDrone_Message,
                        ARDrone_SessionMessage>
 , public Stream_SessionBase_T<Stream_SessionId_t,
                               struct ARDrone_SessionData,
                               enum Stream_SessionMessageType,
                               ARDrone_Message,
                               ARDrone_SessionMessage>
 , public ARDrone_IMAVLinkNotify
{
  typedef Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        mavlink_stream_name_string_,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct ARDrone_StreamState,
                        struct ARDrone_StreamConfiguration,
                        struct ARDrone_Statistic,
                        struct ARDrone_AllocatorConfiguration,
                        struct Stream_ModuleConfiguration,
                        struct ARDrone_ModuleHandlerConfiguration,
                        struct ARDrone_SessionData,
                        ARDrone_StreamSessionData_t,
                        ARDrone_ControlMessage_t,
                        ARDrone_Message,
                        ARDrone_SessionMessage> inherited;
  typedef Stream_SessionBase_T<Stream_SessionId_t,
                               struct ARDrone_SessionData,
                               enum Stream_SessionMessageType,
                               ARDrone_Message,
                               ARDrone_SessionMessage> inherited2;

 public:
  ARDrone_MAVLinkStream ();
  inline virtual ~ARDrone_MAVLinkStream () { inherited::shutdown (); }

  // implement (part of) Stream_IStreamControlBase
  virtual bool load (Stream_ModuleList_t&, // return value: module list
                     bool&);               // return value: delete modules ?

  // implement Common_IInitialize_T
  virtual bool initialize (const typename inherited::CONFIGURATION_T&); // configuration

  // implement Common_IStatistic_T
  // *NOTE*: these delegate to runtimeStatistic_
  virtual bool collect (struct ARDrone_Statistic&); // return value: statistic data
  virtual void report () const;

  // implement ARDrone_IMAVLinkNotify
  virtual void messageCB (const struct __mavlink_message&, // message record
                          void*);                          // payload handle

 private:
  ACE_UNIMPLEMENTED_FUNC (ARDrone_MAVLinkStream (const ARDrone_MAVLinkStream&))
  ACE_UNIMPLEMENTED_FUNC (ARDrone_MAVLinkStream& operator= (const ARDrone_MAVLinkStream&))

  // convenient types
  struct SUBSCRIBERS_IS_EQUAL_P
  {
    inline bool operator() (typename inherited2::INOTIFY_T* first, typename inherited2::INOTIFY_T* second) { return (first == second); }
  };

  // override (part of) Stream_ISessionNotify_T
  virtual void start (Stream_SessionId_t,
                      const struct ARDrone_SessionData&);
  virtual void notify (Stream_SessionId_t,
                       const enum Stream_SessionMessageType&);
  virtual void end (Stream_SessionId_t);
};

// include template definition
#include "ardrone_stream.inl"

#endif
