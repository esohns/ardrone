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

#include "stream_misc_mediafoundation_target.h"

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
class Stream_Vis_Target_MediaFoundation_T
 : public Stream_TaskBaseSynch_T<ACE_SYNCH_USE,
                                 TimePolicyType,
                                 ConfigurationType,
                                 ControlMessageType,
                                 DataMessageType,
                                 SessionMessageType,
                                 Stream_SessionId_t,
                                 Stream_SessionMessageType>
 //, public Stream_IModuleHandler_T<ConfigurationType>
{
 public:
  Stream_Vis_Target_MediaFoundation_T ();
  virtual ~Stream_Vis_Target_MediaFoundation_T ();

  virtual bool initialize (const ConfigurationType&,
                           Stream_IAllocator* = NULL);

  // implement (part of) Stream_ITaskBase_T
  virtual void handleDataMessage (DataMessageType*&, // data message handle
                                  bool&);            // return value: pass message downstream ?
  virtual void handleSessionMessage (SessionMessageType*&, // session message handle
                                     bool&);               // return value: pass message downstream ?

  //// implement Stream_IModuleHandler_T
  //virtual const ConfigurationType& get () const;

 private:
  typedef Stream_TaskBaseSynch_T<ACE_SYNCH_USE,
                                 TimePolicyType,
                                 ConfigurationType,
                                 ControlMessageType,
                                 DataMessageType,
                                 SessionMessageType,
                                 Stream_SessionId_t,
                                 Stream_SessionMessageType> inherited;

  ACE_UNIMPLEMENTED_FUNC (Stream_Vis_Target_MediaFoundation_T (const Stream_Vis_Target_MediaFoundation_T&))
  ACE_UNIMPLEMENTED_FUNC (Stream_Vis_Target_MediaFoundation_T& operator= (const Stream_Vis_Target_MediaFoundation_T&))

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
class Stream_Vis_Target_MediaFoundation_2
 : public Stream_Misc_MediaFoundation_Target_T<ACE_SYNCH_USE,
                                               TimePolicyType,
                                               ConfigurationType,
                                               ControlMessageType,
                                               DataMessageType,
                                               SessionMessageType,
                                               SessionDataType,
                                               SessionDataContainerType>
{
 public:
  Stream_Vis_Target_MediaFoundation_2 ();
  virtual ~Stream_Vis_Target_MediaFoundation_2 ();

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
  typedef Stream_Misc_MediaFoundation_Target_T<ACE_SYNCH_USE,
                                               TimePolicyType,
                                               ConfigurationType,
                                               ControlMessageType,
                                               DataMessageType,
                                               SessionMessageType,
                                               SessionDataType,
                                               SessionDataContainerType> inherited;

  ACE_UNIMPLEMENTED_FUNC (Stream_Vis_Target_MediaFoundation_2 (const Stream_Vis_Target_MediaFoundation_2&))
  ACE_UNIMPLEMENTED_FUNC (Stream_Vis_Target_MediaFoundation_2& operator= (const Stream_Vis_Target_MediaFoundation_2&))
};

// include template definition
#include "stream_vis_target_mediafoundation.inl"

#endif
