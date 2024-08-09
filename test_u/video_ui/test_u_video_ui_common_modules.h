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

#ifndef TEST_U_VIDEO_UI_COMMON_MODULES_H
#define TEST_U_VIDEO_UI_COMMON_MODULES_H

#include "ace/Synch_Traits.h"

#include "common_time_common.h"
#include "common_timer_manager_common.h"

#include "stream_common.h"
#include "stream_streammodule_base.h"

#include "stream_dec_libav_converter.h"
#include "stream_dec_libav_decoder.h"

#include "stream_lib_ffmpeg_common.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "stream_lib_directshow_asynch_source_filter.h"
#include "stream_lib_directshow_source_filter.h"
#endif // ACE_WIN32 || ACE_WIN64

#include "stream_misc_defines.h"
#include "stream_misc_distributor.h"
#include "stream_misc_messagehandler.h"

#include "stream_stat_statistic_report.h"

#if defined (GTK_SUPPORT)
#include "stream_vis_gtk_window.h"
#endif // GTK_SUPPORT
#include "stream_vis_libav_resize.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "stream_vis_target_direct3d.h"
#include "stream_vis_target_directshow.h"
#else
#include "stream_vis_wayland_window.h"
#include "stream_vis_x11_window.h"
#endif // ACE_WIN32 || ACE_WIN64
#include "stream_vis_opencv_classifier.h"

#include "ardrone_module_pave_decoder.h"

#include "test_u_video_ui_message.h"
//#include "test_u_video_ui_session_message.h"
#include "test_u_video_ui_stream_common.h"

// declare module(s)
typedef ARDrone_Module_PaVEDecoder_T<ACE_MT_SYNCH,
                                     Common_TimePolicy_t,
                                     struct Test_U_ModuleHandlerConfiguration,
                                     Stream_ControlMessage_t,
                                     Test_U_Message_t,
                                     Test_U_SessionMessage_t,
                                     struct Stream_MediaFramework_FFMPEG_VideoMediaType,
                                     struct Stream_UserData> Test_U_PaVEDecoder;

typedef Stream_Decoder_LibAVDecoder_T<ACE_MT_SYNCH,
                                     Common_TimePolicy_t,
                                     struct Test_U_ModuleHandlerConfiguration,
                                     Stream_ControlMessage_t,
                                     Test_U_Message_t,
                                     Test_U_SessionMessage_t,
                                     Test_U_SessionData_t,
                                     struct Stream_MediaFramework_FFMPEG_VideoMediaType> Test_U_H264Decoder;

typedef Stream_TaskBaseSynch_T<ACE_MT_SYNCH,
                               Common_TimePolicy_t,
                               struct Test_U_ModuleHandlerConfiguration,
                               Stream_ControlMessage_t,
                               Test_U_Message_t,
                               Test_U_SessionMessage_t,
                               enum Stream_ControlType,
                               enum Stream_SessionMessageType,
                               struct Stream_UserData> Test_U_TaskBaseSynch_t;
typedef Stream_TaskBaseAsynch_T<ACE_MT_SYNCH,
                                Common_TimePolicy_t,
                                struct Test_U_ModuleHandlerConfiguration,
                                Stream_ControlMessage_t,
                                Test_U_Message_t,
                                Test_U_SessionMessage_t,
                                enum Stream_ControlType,
                                enum Stream_SessionMessageType,
                                struct Stream_UserData> Test_U_TaskBaseAsynch_t;

typedef Stream_Decoder_LibAVConverter_T<Test_U_TaskBaseSynch_t,
                                        struct Stream_MediaFramework_FFMPEG_VideoMediaType> Test_U_LibAVConvert;

typedef Stream_Visualization_OpenCVClassifier_T<ACE_MT_SYNCH,
                                                Common_TimePolicy_t,
                                                struct Test_U_ModuleHandlerConfiguration,
                                                Stream_ControlMessage_t,
                                                Test_U_Message_t,
                                                Test_U_SessionMessage_t,
                                                Test_U_SessionData_t,
                                                struct Stream_MediaFramework_FFMPEG_VideoMediaType> Test_U_OpenCVClassifier;

typedef Stream_Visualization_LibAVResize_T<Test_U_TaskBaseSynch_t,
                                           struct Stream_MediaFramework_FFMPEG_VideoMediaType> Test_U_LibAVResize;

typedef Stream_Statistic_StatisticReport_ReaderTask_T<ACE_MT_SYNCH,
                                                      Common_TimePolicy_t,
                                                      struct Test_U_ModuleHandlerConfiguration,
                                                      Stream_ControlMessage_t,
                                                      Test_U_Message_t,
                                                      Test_U_SessionMessage_t,
                                                      Stream_CommandType_t,
                                                      struct Stream_Statistic,
                                                      Common_Timer_Manager_t,
                                                      Test_U_SessionData,
                                                      Test_U_SessionData_t> Test_U_Statistic_ReaderTask_t;
typedef Stream_Statistic_StatisticReport_WriterTask_T<ACE_MT_SYNCH,
                                                      Common_TimePolicy_t,
                                                      struct Test_U_ModuleHandlerConfiguration,
                                                      Stream_ControlMessage_t,
                                                      Test_U_Message_t,
                                                      Test_U_SessionMessage_t,
                                                      Stream_CommandType_t,
                                                      struct Stream_Statistic,
                                                      Common_Timer_Manager_t,
                                                      Test_U_SessionData,
                                                      Test_U_SessionData_t> Test_U_Statistic_WriterTask_t;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Stream_Vis_Target_Direct3D_T<ACE_MT_SYNCH,
                                     Common_TimePolicy_t,
                                     struct Test_U_ModuleHandlerConfiguration,
                                     Stream_ControlMessage_t,
                                     Test_U_Message_t,
                                     Test_U_SessionMessage_t,
                                     Test_U_SessionData,
                                     Test_U_SessionData_t,
                                     struct _AMMediaType> Test_U_DirectShow_Direct3DDisplay;

struct Test_U_DirectShow_FilterConfiguration
 : Stream_MediaFramework_DirectShow_FilterConfiguration
{
  Test_U_DirectShow_FilterConfiguration ()
   : Stream_MediaFramework_DirectShow_FilterConfiguration ()
   , module (NULL)
   , pinConfiguration (NULL)
  {}

  Stream_Module_t*                                                module; // handle
  struct Stream_MediaFramework_DirectShow_FilterPinConfiguration* pinConfiguration; // handle
};
typedef Stream_MediaFramework_DirectShow_Source_Filter_T<Test_U_Message_t,
                                                         struct Test_U_DirectShow_FilterConfiguration,
                                                         struct Stream_MediaFramework_DirectShow_FilterPinConfiguration> Test_U_DirectShowFilter_t;
typedef Stream_MediaFramework_DirectShow_Asynch_Source_Filter_T<Test_U_Message_t,
                                                                struct Test_U_DirectShow_FilterConfiguration,
                                                                struct Stream_MediaFramework_DirectShow_FilterPinConfiguration> Test_U_AsynchDirectShowFilter_t;
typedef Stream_Vis_Target_DirectShow_T<ACE_MT_SYNCH,
                                       Common_TimePolicy_t,
                                       struct Test_U_ModuleHandlerConfiguration,
                                       Stream_ControlMessage_t,
                                       Test_U_Message_t,
                                       Test_U_SessionMessage_t,
                                       Test_U_SessionData_t,
                                       Test_U_SessionData,
                                       struct Test_U_DirectShow_FilterConfiguration,
                                       struct Test_U_DirectShow_PinConfiguration,
                                       Test_U_DirectShowFilter_t,
                                       struct _AMMediaType> Test_U_DirectShow_DirectShowDisplay;
#else
#if defined (GTK_SUPPORT)
typedef Stream_Module_Vis_GTK_Window_T<ACE_MT_SYNCH,
                                       Common_TimePolicy_t,
                                       struct Test_U_ModuleHandlerConfiguration,
                                       Stream_ControlMessage_t,
                                       Test_U_Message_t,
                                       Test_U_SessionMessage_t,
                                       struct Stream_MediaFramework_FFMPEG_VideoMediaType> Test_U_GTKDisplay;
#endif // GTK_SUPPORT
typedef Stream_Module_Vis_Wayland_Window_T<ACE_MT_SYNCH,
                                           Common_TimePolicy_t,
                                           struct Test_U_ModuleHandlerConfiguration,
                                           Stream_ControlMessage_t,
                                           Test_U_Message_t,
                                           Test_U_SessionMessage_t,
                                           Test_U_SessionData_t,
                                           struct Stream_MediaFramework_FFMPEG_VideoMediaType> Test_U_WaylandDisplay;
typedef Stream_Module_Vis_X11_Window_T<ACE_MT_SYNCH,
                                       Common_TimePolicy_t,
                                       struct Test_U_ModuleHandlerConfiguration,
                                       Stream_ControlMessage_t,
                                       Test_U_Message_t,
                                       Test_U_SessionMessage_t,
                                       Test_U_SessionData_t,
                                       struct Stream_MediaFramework_FFMPEG_VideoMediaType> Test_U_X11Display;
#endif // ACE_WIN32 || ACE_WIN64

typedef Stream_Module_MessageHandler_T<ACE_MT_SYNCH,
                                       Common_TimePolicy_t,
                                       struct Test_U_ModuleHandlerConfiguration,
                                       Stream_ControlMessage_t,
                                       Test_U_Message_t,
                                       Test_U_SessionMessage_t,
                                       Test_U_SessionData,
                                       struct Stream_UserData> Test_U_MessageHandler;

//////////////////////////////////////////

DATASTREAM_MODULE_INPUT_ONLY (Test_U_SessionData,                   // session data type
                              enum Stream_SessionMessageType,                   // session event type
                              struct Test_U_ModuleHandlerConfiguration, // module handler configuration type
                              ardrone_default_pave_decoder_module_name_string,
                              Stream_INotify_t,                                 // stream notification interface type
                              Test_U_PaVEDecoder);                       // writer type

DATASTREAM_MODULE_INPUT_ONLY (Test_U_SessionData,                   // session data type
                              enum Stream_SessionMessageType,                   // session event type
                              struct Test_U_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_dec_libav_decoder_module_name_string,
                              Stream_INotify_t,                                 // stream notification interface type
                              Test_U_H264Decoder);                      // writer type

DATASTREAM_MODULE_INPUT_ONLY (Test_U_SessionData,                   // session data type
                              enum Stream_SessionMessageType,                   // session event type
                              struct Test_U_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_dec_libav_converter_module_name_string,
                              Stream_INotify_t,                                 // stream notification interface type
                              Test_U_LibAVConvert);                      // writer type

DATASTREAM_MODULE_INPUT_ONLY (Test_U_SessionData,                   // session data type
                              enum Stream_SessionMessageType,                   // session event type
                              struct Test_U_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_vis_opencv_classifier_module_name_string,
                              Stream_INotify_t,                                 // stream notification interface type
                              Test_U_OpenCVClassifier);                      // writer type

DATASTREAM_MODULE_INPUT_ONLY (Test_U_SessionData,                   // session data type
                              enum Stream_SessionMessageType,                   // session event type
                              struct Test_U_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_vis_libav_resize_module_name_string,
                              Stream_INotify_t,                                 // stream notification interface type
                              Test_U_LibAVResize);                      // writer type

DATASTREAM_MODULE_DUPLEX (Test_U_SessionData,                // session data type
                          enum Stream_SessionMessageType,                   // session event type
                          struct Test_U_ModuleHandlerConfiguration, // module handler configuration type
                          libacestream_default_stat_report_module_name_string,
                          Stream_INotify_t,                                 // stream notification interface type
                          Test_U_Statistic_ReaderTask_t,            // reader type
                          Test_U_Statistic_WriterTask_t,            // writer type
                          Test_U_StatisticReport);                  // name

#if defined (ACE_WIN32) || defined (ACE_WIN64)
DATASTREAM_MODULE_INPUT_ONLY (Test_U_SessionData,                // session data type
                              enum Stream_SessionMessageType,                   // session event type
                              struct Test_U_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_vis_directshow_module_name_string,
                              Stream_INotify_t,                                 // stream notification interface type
                              Test_U_DirectShow_Direct3DDisplay);       // writer type

DATASTREAM_MODULE_INPUT_ONLY (Test_U_SessionData,                // session data type
                              enum Stream_SessionMessageType,                   // session event type
                              struct Test_U_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_vis_directshow_module_name_string,
                              Stream_INotify_t,                                 // stream notification interface type
                              Test_U_DirectShow_DirectShowDisplay);     // writer type
#else
#if defined (GTK_SUPPORT)
DATASTREAM_MODULE_INPUT_ONLY (Test_U_SessionData,                                     // session data type
                              enum Stream_SessionMessageType,                         // session event type
                              struct Test_U_ModuleHandlerConfiguration,               // module handler configuration type
                              libacestream_default_vis_gtk_window_module_name_string,
                              Stream_INotify_t,                                       // stream notification interface type
                              Test_U_GTKDisplay);                                     // writer type
#endif // GTK_SUPPORT
DATASTREAM_MODULE_INPUT_ONLY (Test_U_SessionData,                                    // session data type
                             enum Stream_SessionMessageType,                         // session event type
                             struct Test_U_ModuleHandlerConfiguration,               // module handler configuration type
                             libacestream_default_vis_wayland_window_module_name_string,
                             Stream_INotify_t,                                       // stream notification interface type
                             Test_U_WaylandDisplay);                                 // writer type
DATASTREAM_MODULE_INPUT_ONLY (Test_U_SessionData,                   // session data type
                              enum Stream_SessionMessageType,                   // session event type
                              struct Test_U_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_vis_x11_window_module_name_string,
                              Stream_INotify_t,                                 // stream notification interface type
                              Test_U_X11Display);                          // writer type
#endif // ACE_WIN32 || ACE_WIN64

DATASTREAM_MODULE_INPUT_ONLY (Test_U_SessionData,                           // session data type
                              enum Stream_SessionMessageType,                       // session event type
                              struct Test_U_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_misc_messagehandler_module_name_string,
                              Stream_INotify_t,                                     // stream notification interface type
                              Test_U_MessageHandler);                       // writer type

#endif
