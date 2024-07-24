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

#ifndef ARDRONE_MODULE_MEDIAFOUNDATION_H
#define ARDRONE_MODULE_MEDIAFOUNDATION_H

#include "ace/Global_Macros.h"

#include <d3d9.h>
#include <evr.h>

#include "common_time_common.h"

#include "stream_imodule.h"
#include "stream_task_base_synch.h"

#include "stream_lib_mediafoundation_target.h"

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          ////////////////////////////////
          typename ConfigurationType,
          ////////////////////////////////
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          ////////////////////////////////
          typename SessionDataType,
          typename SessionDataContainerType>
class ARDrone_Module_MediaFoundation_T
 : public Stream_TaskBaseSynch_T<ACE_SYNCH_USE,
                                 TimePolicyType,
                                 ConfigurationType,
                                 ControlMessageType,
                                 DataMessageType,
                                 SessionMessageType,
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
                                 enum Stream_ControlType,
                                 enum Stream_SessionMessageType,
                                 struct Stream_UserData> inherited;

 public:
  ARDrone_Module_MediaFoundation_T (typename inherited::ISTREAM_T*);
  virtual ~ARDrone_Module_MediaFoundation_T ();

  virtual bool initialize (const ConfigurationType&,
                           Stream_IAllocator* = NULL);

  // implement (part of) Stream_ITaskBase_T
  virtual void handleDataMessage (DataMessageType*&, // data message handle
                                  bool&);            // return value: pass message downstream ?
  virtual void handleSessionMessage (SessionMessageType*&, // session message handle
                                     bool&);               // return value: pass message downstream ?

 private:
  ACE_UNIMPLEMENTED_FUNC (ARDrone_Module_MediaFoundation_T ())
  ACE_UNIMPLEMENTED_FUNC (ARDrone_Module_MediaFoundation_T (const ARDrone_Module_MediaFoundation_T&))
  ACE_UNIMPLEMENTED_FUNC (ARDrone_Module_MediaFoundation_T& operator= (const ARDrone_Module_MediaFoundation_T&))

  // helper methods
  bool initialize_MediaFoundation (const HWND,                // (target) window handle
                                   const struct tagRECT&,     // (target) window area
                                   //const IMFMediaType*,       // media type handle
                                   TOPOID,                    // renderer node id
                                   IMFMediaSink*&,            // return value: media sink handle
                                   IMFVideoDisplayControl*&,  // return value: video display control handle
                                   //IMFVideoSampleAllocator*&, // return value: video sample allocator handle
                                   IMFMediaSession*);         // media session handle

  IDirect3DDevice9Ex*     device_;
  IMFMediaSession*        mediaSession_;
  IMFStreamSink*          streamSink_;
  IMFVideoDisplayControl* videoDisplayControl_;
  //IMFVideoSampleAllocator* videoSampleAllocator_;
};

//////////////////////////////////////////

template <ACE_SYNCH_DECL,
          typename TimePolicyType,
          ////////////////////////////////
          typename ConfigurationType,
          ////////////////////////////////
          typename ControlMessageType,
          typename DataMessageType,
          typename SessionMessageType,
          ////////////////////////////////
          typename SessionDataType,
          typename SessionDataContainerType>
class ARDrone_Module_MediaFoundation_2
 : public Stream_MediaFramework_MediaFoundation_Target_T<ACE_SYNCH_USE,
                                                         TimePolicyType,
                                                         ConfigurationType,
                                                         ControlMessageType,
                                                         DataMessageType,
                                                         SessionMessageType,
                                                         SessionDataType,
                                                         SessionDataContainerType,
                                                         IMFMediaType*>
{
  typedef Stream_MediaFramework_MediaFoundation_Target_T<ACE_SYNCH_USE,
                                                         TimePolicyType,
                                                         ConfigurationType,
                                                         ControlMessageType,
                                                         DataMessageType,
                                                         SessionMessageType,
                                                         SessionDataType,
                                                         SessionDataContainerType,
                                                         IMFMediaType*> inherited;

 public:
  ARDrone_Module_MediaFoundation_2 (typename inherited::ISTREAM_T*);
  virtual ~ARDrone_Module_MediaFoundation_2 ();

  //virtual bool initialize (const ConfigurationType&);

  // implement (part of) Stream_ITaskBase_T
  virtual void handleDataMessage (DataMessageType*&, // data message handle
                                  bool&);            // return value: pass message downstream ?
  virtual void handleSessionMessage (SessionMessageType*&, // session message handle
                                     bool&);               // return value: pass message downstream ?

  //// implement Stream_IModuleHandler_T
  //virtual const ConfigurationType& get () const;

  //// override (part of) IMFSampleGrabberSinkCallback2
  //STDMETHODIMP OnProcessSampleEx (const struct _GUID&, // major media type
  //                                DWORD,               // flags
  //                                LONGLONG,            // timestamp
  //                                LONGLONG,            // duration
  //                                const BYTE*,         // buffer
  //                                DWORD,               // buffer size
  //                                IMFAttributes*);     // media sample attributes

 private:
  ACE_UNIMPLEMENTED_FUNC (ARDrone_Module_MediaFoundation_2 ())
  ACE_UNIMPLEMENTED_FUNC (ARDrone_Module_MediaFoundation_2 (const ARDrone_Module_MediaFoundation_2&))
  ACE_UNIMPLEMENTED_FUNC (ARDrone_Module_MediaFoundation_2& operator= (const ARDrone_Module_MediaFoundation_2&))
};

// include template definition
#include "ardrone_module_mediafoundation.inl"

#endif
