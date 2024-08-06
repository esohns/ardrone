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

#ifndef ARDRONE_STREAM_H
#define ARDRONE_STREAM_H

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "mfidl.h"
#include "strmif.h"
#endif // ACE_WIN32 || ACE_WIN64

#include "ace/Global_Macros.h"
#include "ace/Synch_Traits.h"

#include "common_time_common.h"

#include "stream_base.h"
#include "stream_common.h"
#include "stream_session_base.h"
#include "stream_statemachine_control.h"

#include "ardrone_modules_common.h"
#include "ardrone_stream_common.h"
#include "ardrone_types.h"

// forward declarations
class ARDrone_Message;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
class ARDrone_DirectShow_SessionMessage;
#else
class ARDrone_SessionMessage;
#endif // ACE_WIN32 || ACE_WIN64

extern const char video_stream_name_string_[];
extern const char control_stream_name_string_[];
extern const char navdata_stream_name_string_[];
extern const char mavlink_stream_name_string_[];

template <typename ModuleConfigurationType,
          typename SessionDataType, // implements Stream_SessionData_T
          typename SessionMessageType,
          typename SourceModuleType>
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
                        ModuleConfigurationType,
                        typename SessionDataType::DATA_T,
                        SessionDataType,
                        Stream_ControlMessage_t,
                        ARDrone_Message,
                        SessionMessageType>
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
                        ModuleConfigurationType,
                        typename SessionDataType::DATA_T,
                        SessionDataType,
                        Stream_ControlMessage_t,
                        ARDrone_Message,
                        SessionMessageType> inherited;

 public:
  ARDrone_VideoStream_T ();
  virtual ~ARDrone_VideoStream_T ();

  // implement (part of) Stream_IStreamControlBase
  virtual bool load (Stream_ILayout*, // layout handle
                     bool&);          // return value: delete modules ?

  // implement Common_IInitialize_T
  virtual bool initialize (const typename inherited::CONFIGURATION_T&); // configuration

 private:
  ACE_UNIMPLEMENTED_FUNC (ARDrone_VideoStream_T (const ARDrone_VideoStream_T&))
  ACE_UNIMPLEMENTED_FUNC (ARDrone_VideoStream_T& operator= (const ARDrone_VideoStream_T&))

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  IGraphBuilder*   graphBuilder_;
  IMFMediaSession* mediaSession_;
#endif // ACE_WIN32 || ACE_WIN64
};

//////////////////////////////////////////

template <typename ModuleConfigurationType,
          typename SessionDataType, // implements Stream_SessionData_T
          typename SessionMessageType>
class ARDrone_ControlStream_T
 : public Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        control_stream_name_string_,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct ARDrone_StreamState,
                        struct ARDrone_StreamConfiguration,
                        struct ARDrone_Statistic,
                        ModuleConfigurationType,
                        typename SessionDataType::DATA_T,
                        SessionDataType,
                        Stream_ControlMessage_t,
                        ARDrone_Message,
                        SessionMessageType>
 , public Stream_SessionBase_T<typename SessionDataType::DATA_T,
                               enum Stream_SessionMessageType,
                               ARDrone_Message,
                               SessionMessageType>
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
                        ModuleConfigurationType,
                        typename SessionDataType::DATA_T,
                        SessionDataType,
                        Stream_ControlMessage_t,
                        ARDrone_Message,
                        SessionMessageType> inherited;
  typedef Stream_SessionBase_T<typename SessionDataType::DATA_T,
                               enum Stream_SessionMessageType,
                               ARDrone_Message,
                               SessionMessageType> inherited2;

 public:
  ARDrone_ControlStream_T ();
  inline virtual ~ARDrone_ControlStream_T () { inherited::shutdown (); }

  using inherited::start;
  using inherited::wait;
  // implement (part of) Stream_IStreamControlBase
  virtual bool load (Stream_ILayout*, // layout handle
                     bool&);          // return value: delete modules ?

  // implement Common_IInitialize_T
  virtual bool initialize (const typename inherited::CONFIGURATION_T&); // configuration

  // implement ARDrone_IControlNotify
  virtual void messageCB (const ARDrone_DeviceConfiguration_t&); // device configuration

 private:
  ACE_UNIMPLEMENTED_FUNC (ARDrone_ControlStream_T (const ARDrone_ControlStream_T&))
  ACE_UNIMPLEMENTED_FUNC (ARDrone_ControlStream_T& operator= (const ARDrone_ControlStream_T&))

  // convenient types
  struct SUBSCRIBERS_IS_EQUAL_P
  {
    inline bool operator() (typename inherited2::INOTIFY_T* first, typename inherited2::INOTIFY_T* second) { return (first == second); }
  };

  // override (part of) Stream_ISessionNotify_T
  virtual void start (Stream_SessionId_t,
                      const typename SessionDataType::DATA_T&);
  virtual void notify (Stream_SessionId_t,
                       const enum Stream_SessionMessageType&,
                       bool = false);
  virtual void end (Stream_SessionId_t);

  inline virtual void onSessionBegin (Stream_SessionId_t) {}
  inline virtual void onSessionEnd (Stream_SessionId_t) {}

  ARDrone_IDeviceConfiguration* configuration_;
};

//////////////////////////////////////////

template <typename ModuleConfigurationType,
          typename SessionDataType, // implements Stream_SessionData_T
          typename SessionMessageType>
class ARDrone_NavDataStream_T
 : public Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        navdata_stream_name_string_,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct ARDrone_StreamState,
                        struct ARDrone_StreamConfiguration,
                        struct ARDrone_Statistic,
                        ModuleConfigurationType,
                        typename SessionDataType::DATA_T,
                        SessionDataType,
                        Stream_ControlMessage_t,
                        ARDrone_Message,
                        SessionMessageType>
 , public Stream_SessionBase_T<typename SessionDataType::DATA_T,
                               enum Stream_SessionMessageType,
                               ARDrone_Message,
                               SessionMessageType>
 , public ARDrone_INavDataNotify
 , public Net_WLAN_IMonitorCB
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
                        ModuleConfigurationType,
                        typename SessionDataType::DATA_T,
                        SessionDataType,
                        Stream_ControlMessage_t,
                        ARDrone_Message,
                        SessionMessageType> inherited;
  typedef Stream_SessionBase_T<typename SessionDataType::DATA_T,
                               enum Stream_SessionMessageType,
                               ARDrone_Message,
                               SessionMessageType> inherited2;

 public:
  ARDrone_NavDataStream_T ();
  virtual ~ARDrone_NavDataStream_T ();

  using inherited::start;
  using inherited::wait;
  // implement (part of) Stream_IStreamControlBase
  virtual bool load (Stream_ILayout*, // layout handle
                     bool&);          // return value: delete modules ?

  // implement Common_IInitialize_T
  virtual bool initialize (const typename inherited::CONFIGURATION_T&); // configuration

  // implement ARDrone_INavDataNotify
  virtual void messageCB (const struct _navdata_t&,              // message record
                          const ARDrone_NavDataOptionOffsets_t&, // option offsets
                          void*);                                // payload handle

  // implement Common_IGetP_T
  virtual const ARDrone_IController* const getP () const;

 private:
  ACE_UNIMPLEMENTED_FUNC (ARDrone_NavDataStream_T (const ARDrone_NavDataStream_T&))
  ACE_UNIMPLEMENTED_FUNC (ARDrone_NavDataStream_T& operator= (const ARDrone_NavDataStream_T&))

  // convenient types
  typedef Common_ISubscribe_T<Net_WLAN_IMonitorCB> IWLAN_SUBSCRIBE_T;
  struct SUBSCRIBERS_IS_EQUAL_P
  {
    inline bool operator() (typename inherited2::INOTIFY_T* first, typename inherited2::INOTIFY_T* second) { return (first == second); }
  };

  // override (part of) Stream_ISessionNotify_T
  virtual void start (Stream_SessionId_t,
                      const typename SessionDataType::DATA_T&);
  virtual void notify (Stream_SessionId_t,
                       const enum Stream_SessionMessageType&,
                       bool = false);
  virtual void end (Stream_SessionId_t);

  inline virtual void onSessionBegin (Stream_SessionId_t) {}
  inline virtual void onSessionEnd (Stream_SessionId_t) {}

  // implement Net_WLAN_IMonitorCB
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  virtual void onSignalQualityChange (REFGUID,              // interface identifier
                                      WLAN_SIGNAL_QUALITY); // signal quality (of current association)
#else
  virtual void onSignalQualityChange (const std::string&, // interface identifier
                                      unsigned int);      // signal quality (of current association)
#endif
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  virtual void onAssociate (REFGUID,            // device identifier
#else
  virtual void onAssociate (const std::string&, // device identifier
#endif
                            const std::string&, // SSID
                            bool);              // success ?
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  virtual void onDisassociate (REFGUID,            // device identifier
#else
  virtual void onDisassociate (const std::string&, // device identifier
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
  virtual void onDisconnect (REFGUID,            // device identifier
#else
  virtual void onDisconnect (const std::string&, // device identifier
#endif
                             const std::string&, // SSID
                             bool);              // success ?
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  virtual void onHotPlug (REFGUID,            // device identifier
#else
  virtual void onHotPlug (const std::string&, // device identifier
#endif
                          bool);              // success ?
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  virtual void onRemove (REFGUID,            // device identifier
#else
  virtual void onRemove (const std::string&, // device identifier
#endif
                         bool);              // success ?
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  virtual void onScanComplete (REFGUID);            // device identifier
#else
  virtual void onScanComplete (const std::string&); // device identifier
#endif

  bool isFirst_;
};

//////////////////////////////////////////

template <typename ModuleConfigurationType,
          typename SessionDataType, // implements Stream_SessionData_T
          typename SessionMessageType>
class ARDrone_MAVLinkStream_T
 : public Stream_Base_T<ACE_MT_SYNCH,
                        Common_TimePolicy_t,
                        mavlink_stream_name_string_,
                        enum Stream_ControlType,
                        enum Stream_SessionMessageType,
                        enum Stream_StateMachine_ControlState,
                        struct ARDrone_StreamState,
                        struct ARDrone_StreamConfiguration,
                        struct ARDrone_Statistic,
                        ModuleConfigurationType,
                        typename SessionDataType::DATA_T,
                        SessionDataType,
                        Stream_ControlMessage_t,
                        ARDrone_Message,
                        SessionMessageType>
 , public Stream_SessionBase_T<typename SessionDataType::DATA_T,
                               enum Stream_SessionMessageType,
                               ARDrone_Message,
                               SessionMessageType>
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
                        ModuleConfigurationType,
                        typename SessionDataType::DATA_T,
                        SessionDataType,
                        Stream_ControlMessage_t,
                        ARDrone_Message,
                        SessionMessageType> inherited;
  typedef Stream_SessionBase_T<typename SessionDataType::DATA_T,
                               enum Stream_SessionMessageType,
                               ARDrone_Message,
                               SessionMessageType> inherited2;

 public:
  ARDrone_MAVLinkStream_T ();
  inline virtual ~ARDrone_MAVLinkStream_T () { inherited::shutdown (); }

  using inherited::start;
  using inherited::wait;
  // implement (part of) Stream_IStreamControlBase
  virtual bool load (Stream_ILayout*, // layout handle
                     bool&);          // return value: delete modules ?

  // implement Common_IInitialize_T
  virtual bool initialize (const typename inherited::CONFIGURATION_T&); // configuration

  // implement ARDrone_IMAVLinkNotify
  virtual void messageCB (const struct __mavlink_message&, // message record
                          void*);                          // payload handle

 private:
  ACE_UNIMPLEMENTED_FUNC (ARDrone_MAVLinkStream_T (const ARDrone_MAVLinkStream_T&))
  ACE_UNIMPLEMENTED_FUNC (ARDrone_MAVLinkStream_T& operator= (const ARDrone_MAVLinkStream_T&))

  // convenient types
  struct SUBSCRIBERS_IS_EQUAL_P
  {
    inline bool operator() (typename inherited2::INOTIFY_T* first, typename inherited2::INOTIFY_T* second) { return (first == second); }
  };

  // override (part of) Stream_ISessionNotify_T
  virtual void start (Stream_SessionId_t,
                      const typename SessionDataType::DATA_T&);
  virtual void notify (Stream_SessionId_t,
                       const enum Stream_SessionMessageType&,
                       bool = false);
  virtual void end (Stream_SessionId_t);

  inline virtual void onSessionBegin (Stream_SessionId_t) {}
  inline virtual void onSessionEnd (Stream_SessionId_t) {}
};

//////////////////////////////////////////

// include template definition
#include "ardrone_stream.inl"

//////////////////////////////////////////

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef ARDrone_VideoStream_T<struct ARDrone_DirectShow_ModuleHandlerConfiguration,
                              ARDrone_DirectShow_SessionData_t,
                              ARDrone_DirectShow_SessionMessage,
                              ARDrone_Module_DirectShow_AsynchTCPSource_Module> ARDrone_DirectShow_AsynchVideoStream_t;
typedef ARDrone_VideoStream_T<struct ARDrone_DirectShow_ModuleHandlerConfiguration,
                              ARDrone_DirectShow_SessionData_t,
                              ARDrone_DirectShow_SessionMessage,
                              ARDrone_Module_DirectShow_TCPSource_Module> ARDrone_DirectShow_VideoStream_t;
typedef ARDrone_VideoStream_T<struct ARDrone_MediaFoundation_ModuleHandlerConfiguration,
                              ARDrone_DirectShow_SessionData_t,
                              ARDrone_DirectShow_SessionMessage,
                              ARDrone_Module_MediaFoundation_AsynchTCPSource_Module> ARDrone_MediaFoundation_AsynchVideoStream_t;
typedef ARDrone_VideoStream_T<struct ARDrone_MediaFoundation_ModuleHandlerConfiguration,
                              ARDrone_DirectShow_SessionData_t,
                              ARDrone_DirectShow_SessionMessage,
                              ARDrone_Module_MediaFoundation_TCPSource_Module> ARDrone_MediaFoundation_VideoStream_t;

typedef ARDrone_ControlStream_T<struct ARDrone_DirectShow_ModuleHandlerConfiguration,
                                ARDrone_DirectShow_SessionData_t,
                                ARDrone_DirectShow_SessionMessage> ARDrone_DirectShow_ControlStream_t;
typedef ARDrone_ControlStream_T<struct ARDrone_MediaFoundation_ModuleHandlerConfiguration,
                                ARDrone_DirectShow_SessionData_t,
                                ARDrone_DirectShow_SessionMessage> ARDrone_MediaFoundation_ControlStream_t;

typedef ARDrone_NavDataStream_T<struct ARDrone_DirectShow_ModuleHandlerConfiguration,
                                ARDrone_DirectShow_SessionData_t,
                                ARDrone_DirectShow_SessionMessage> ARDrone_DirectShow_NavDataStream_t;
typedef ARDrone_NavDataStream_T<struct ARDrone_MediaFoundation_ModuleHandlerConfiguration,
                                ARDrone_DirectShow_SessionData_t,
                                ARDrone_DirectShow_SessionMessage> ARDrone_MediaFoundation_NavDataStream_t;

typedef ARDrone_MAVLinkStream_T<struct ARDrone_DirectShow_ModuleHandlerConfiguration,
                                ARDrone_DirectShow_SessionData_t,
                                ARDrone_DirectShow_SessionMessage> ARDrone_DirectShow_MAVLinkStream_t;
typedef ARDrone_MAVLinkStream_T<struct ARDrone_MediaFoundation_ModuleHandlerConfiguration,
                                ARDrone_DirectShow_SessionData_t,
                                ARDrone_DirectShow_SessionMessage> ARDrone_MediaFoundation_MAVLinkStream_t;
#else
typedef ARDrone_VideoStream_T<struct ARDrone_ModuleHandlerConfiguration,
                              ARDrone_SessionData_t,
                              ARDrone_SessionMessage,
                              ARDrone_Module_AsynchTCPSource_Module> ARDrone_AsynchVideoStream_t;
typedef ARDrone_VideoStream_T<struct ARDrone_ModuleHandlerConfiguration,
                              ARDrone_SessionData_t,
                              ARDrone_SessionMessage,
                              ARDrone_Module_TCPSource_Module> ARDrone_VideoStream_t;

typedef ARDrone_ControlStream_T<struct ARDrone_ModuleHandlerConfiguration,
                                ARDrone_SessionData_t,
                                ARDrone_SessionMessage> ARDrone_ControlStream_t;

typedef ARDrone_NavDataStream_T<struct ARDrone_ModuleHandlerConfiguration,
                                ARDrone_SessionData_t,
                                ARDrone_SessionMessage> ARDrone_NavDataStream_t;

typedef ARDrone_MAVLinkStream_T<struct ARDrone_ModuleHandlerConfiguration,
                                ARDrone_SessionData_t,
                                ARDrone_SessionMessage> ARDrone_MAVLinkStream_t;
#endif // ACE_WIN32 || ACE_WIN64

#endif
