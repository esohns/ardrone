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

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "stream_lib_directshow_asynch_source_filter.h"
#include "stream_lib_directshow_source_filter.h"
#endif // ACE_WIN32 || ACE_WIN64

#include "stream_misc_defines.h"
#include "stream_misc_distributor.h"
#include "stream_misc_messagehandler.h"

#include "stream_stat_statistic_report.h"

#include "stream_vis_gtk_window.h"
#include "stream_vis_libav_resize.h"
#include "stream_vis_x11_window.h"

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
                                     struct Stream_UserData> Test_U_PaVEDecoder;

typedef Stream_Decoder_LibAVDecoder_T<ACE_MT_SYNCH,
                                     Common_TimePolicy_t,
                                     struct Test_U_ModuleHandlerConfiguration,
                                     Stream_ControlMessage_t,
                                     Test_U_Message_t,
                                     Test_U_SessionMessage_t,
                                     Test_U_SessionData_t,
                                     struct Stream_MediaFramework_FFMPEG_MediaType> Test_U_H264Decoder;

typedef Stream_Decoder_LibAVConverter_T<ACE_MT_SYNCH,
                                        Common_TimePolicy_t,
                                        struct Test_U_ModuleHandlerConfiguration,
                                        Stream_ControlMessage_t,
                                        Test_U_Message_t,
                                        Test_U_SessionMessage_t,
                                        Test_U_SessionData_t,
                                        struct Stream_MediaFramework_FFMPEG_MediaType> Test_U_LibAVConvert;

typedef Stream_Visualization_LibAVResize_T<ACE_MT_SYNCH,
                                           Common_TimePolicy_t,
                                           struct Test_U_ModuleHandlerConfiguration,
                                           Stream_ControlMessage_t,
                                           Test_U_Message_t,
                                           Test_U_SessionMessage_t,
                                           Test_U_SessionData_t,
                                           struct Stream_MediaFramework_FFMPEG_MediaType> Test_U_LibAVResize;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Stream_Statistic_StatisticReport_ReaderTask_T<ACE_MT_SYNCH,
                                                      Common_TimePolicy_t,
                                                      struct Test_U_DirectShow_ModuleHandlerConfiguration,
                                                      Stream_ControlMessage_t,
                                                      Test_U_DirectShow_Message_t,
                                                      Test_U_DirectShow_SessionMessage_t,
                                                      Stream_CommandType_t,
                                                      struct Test_U_StatisticData,
                                                      Common_Timer_Manager_t,
                                                      Test_U_DirectShow_SessionData,
                                                      Test_U_DirectShow_SessionData_t> Test_U_DirectShow_Statistic_ReaderTask_t;
typedef Stream_Statistic_StatisticReport_WriterTask_T<ACE_MT_SYNCH,
                                                      Common_TimePolicy_t,
                                                      struct Test_U_DirectShow_ModuleHandlerConfiguration,
                                                      Stream_ControlMessage_t,
                                                      Test_U_DirectShow_Message_t,
                                                      Test_U_DirectShow_SessionMessage_t,
                                                      Stream_CommandType_t,
                                                      struct Test_U_StatisticData,
                                                      Common_Timer_Manager_t,
                                                      Test_U_DirectShow_SessionData,
                                                      Test_U_DirectShow_SessionData_t> Test_U_DirectShow_Statistic_WriterTask_t;

typedef Stream_Statistic_StatisticReport_ReaderTask_T<ACE_MT_SYNCH,
                                                      Common_TimePolicy_t,
                                                      struct Test_U_MediaFoundation_ModuleHandlerConfiguration,
                                                      Stream_ControlMessage_t,
                                                      Test_U_MediaFoundation_Message_t,
                                                      Test_U_MediaFoundation_SessionMessage_t,
                                                      Stream_CommandType_t,
                                                      struct Test_U_StatisticData,
                                                      Common_Timer_Manager_t,
                                                      Test_U_MediaFoundation_SessionData,
                                                      Test_U_MediaFoundation_SessionData_t> Test_U_MediaFoundation_Statistic_ReaderTask_t;
typedef Stream_Statistic_StatisticReport_WriterTask_T<ACE_MT_SYNCH,
                                                      Common_TimePolicy_t,
                                                      struct Test_U_MediaFoundation_ModuleHandlerConfiguration,
                                                      Stream_ControlMessage_t,
                                                      Test_U_MediaFoundation_Message_t,
                                                      Test_U_MediaFoundation_SessionMessage_t,
                                                      Stream_CommandType_t,
                                                      struct Test_U_StatisticData,
                                                      Common_Timer_Manager_t,
                                                      Test_U_MediaFoundation_SessionData,
                                                      Test_U_MediaFoundation_SessionData_t> Test_U_MediaFoundation_Statistic_WriterTask_t;
#else
typedef Stream_Statistic_StatisticReport_ReaderTask_T<ACE_MT_SYNCH,
                                                      Common_TimePolicy_t,
                                                      struct Test_U_ModuleHandlerConfiguration,
                                                      Stream_ControlMessage_t,
                                                      Test_U_Message_t,
                                                      Test_U_SessionMessage_t,
                                                      Stream_CommandType_t,
                                                      struct Test_U_StatisticData,
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
                                                      struct Test_U_StatisticData,
                                                      Common_Timer_Manager_t,
                                                      Test_U_SessionData,
                                                      Test_U_SessionData_t> Test_U_Statistic_WriterTask_t;
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Stream_Vis_Target_Direct3D_T<ACE_MT_SYNCH,
                                     Common_TimePolicy_t,
                                     struct Test_U_DirectShow_ModuleHandlerConfiguration,
                                     Stream_ControlMessage_t,
                                     Test_U_DirectShow_Message_t,
                                     Test_U_DirectShow_SessionMessage_t,
                                     Test_U_DirectShow_SessionData,
                                     Test_U_DirectShow_SessionData_t,
                                     struct _AMMediaType> Test_U_DirectShow_Direct3DDisplay;
typedef Stream_Vis_Target_Direct3D_T<ACE_MT_SYNCH,
                                     Common_TimePolicy_t,
                                     struct Test_U_MediaFoundation_ModuleHandlerConfiguration,
                                     Stream_ControlMessage_t,
                                     Test_U_MediaFoundation_Message_t,
                                     Test_U_MediaFoundation_SessionMessage_t,
                                     Test_U_MediaFoundation_SessionData,
                                     Test_U_MediaFoundation_SessionData_t,
                                     IMFMediaType*> Test_U_MediaFoundation_Direct3DDisplay;

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
typedef Stream_MediaFramework_DirectShow_Source_Filter_T<Common_TimePolicy_t,
                                                         Test_U_DirectShow_SessionMessage_t,
                                                         Test_U_DirectShow_Message_t,
                                                         struct Test_U_DirectShow_FilterConfiguration,
                                                         struct Stream_MediaFramework_DirectShow_FilterPinConfiguration,
                                                         struct _AMMediaType> Test_U_DirectShowFilter_t;
typedef Stream_MediaFramework_DirectShow_Asynch_Source_Filter_T<Common_TimePolicy_t,
                                                                Test_U_DirectShow_SessionMessage_t,
                                                                Test_U_DirectShow_Message_t,
                                                                struct Test_U_DirectShow_FilterConfiguration,
                                                                struct Stream_MediaFramework_DirectShow_FilterPinConfiguration,
                                                                struct _AMMediaType> Test_U_AsynchDirectShowFilter_t;
typedef Stream_Vis_Target_DirectShow_T<ACE_MT_SYNCH,
                                       Common_TimePolicy_t,
                                       struct Test_U_DirectShow_ModuleHandlerConfiguration,
                                       Stream_ControlMessage_t,
                                       Test_U_DirectShow_Message_t,
                                       Test_U_DirectShow_SessionMessage_t,
                                       Test_U_DirectShow_SessionData_t,
                                       Test_U_DirectShow_SessionData,
                                       struct Test_U_DirectShow_FilterConfiguration,
                                       struct Test_U_DirectShow_PinConfiguration,
                                       Test_U_DirectShowFilter_t> Test_U_DirectShow_DirectShowDisplay;

typedef Stream_Vis_Target_MediaFoundation_T<ACE_MT_SYNCH,
                                            Common_TimePolicy_t,
                                            struct Test_U_MediaFoundation_ModuleHandlerConfiguration,
                                            Stream_ControlMessage_t,
                                            Test_U_MediaFoundation_Message_t,
                                            Test_U_MediaFoundation_SessionMessage_t,
                                            Test_U_MediaFoundation_SessionData,
                                            Test_U_MediaFoundation_SessionData_t,
                                            struct Stream_UserData> Test_U_MediaFoundation_MediaFoundationDisplay;
typedef Stream_Vis_Target_MediaFoundation_2<ACE_MT_SYNCH,
                                            Common_TimePolicy_t,
                                            struct Test_U_MediaFoundation_ModuleHandlerConfiguration,
                                            Stream_ControlMessage_t,
                                            Test_U_MediaFoundation_Message_t,
                                            Test_U_MediaFoundation_SessionMessage_t,
                                            Test_U_MediaFoundation_SessionData,
                                            Test_U_MediaFoundation_SessionData_t> Test_U_MediaFoundation_MediaFoundationDisplayNull;
#else
typedef Stream_Module_Vis_GTK_Window_T<ACE_MT_SYNCH,
                                       Common_TimePolicy_t,
                                       struct Test_U_ModuleHandlerConfiguration,
                                       Stream_ControlMessage_t,
                                       Test_U_Message_t,
                                       Test_U_SessionMessage_t,
                                       struct Stream_MediaFramework_FFMPEG_MediaType> Test_U_Display_2;
typedef Stream_Module_Vis_X11_Window_T<ACE_MT_SYNCH,
                                       Common_TimePolicy_t,
                                       struct Test_U_ModuleHandlerConfiguration,
                                       Stream_ControlMessage_t,
                                       Test_U_Message_t,
                                       Test_U_SessionMessage_t,
                                       Test_U_SessionData_t,
                                       struct Stream_MediaFramework_FFMPEG_MediaType> Test_U_Display;
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Stream_Module_MessageHandler_T<ACE_MT_SYNCH,
                                       Common_TimePolicy_t,
                                       struct Test_U_DirectShow_ModuleHandlerConfiguration,
                                       Stream_ControlMessage_t,
                                       Test_U_DirectShow_Message_t,
                                       Test_U_DirectShow_SessionMessage_t,
                                       Stream_SessionId_t,
                                       Test_U_DirectShow_SessionData,
                                       struct Stream_UserData> Test_U_DirectShow_MessageHandler;

typedef Stream_Module_MessageHandler_T<ACE_MT_SYNCH,
                                       Common_TimePolicy_t,
                                       struct Test_U_MediaFoundation_ModuleHandlerConfiguration,
                                       Stream_ControlMessage_t,
                                       Test_U_MediaFoundation_Message_t,
                                       Test_U_MediaFoundation_SessionMessage_t,
                                       Stream_SessionId_t,
                                       Test_U_MediaFoundation_SessionData,
                                       struct Stream_UserData> Test_U_MediaFoundation_MessageHandler;
#else
typedef Stream_Module_MessageHandler_T<ACE_MT_SYNCH,
                                       Common_TimePolicy_t,
                                       struct Test_U_ModuleHandlerConfiguration,
                                       Stream_ControlMessage_t,
                                       Test_U_Message_t,
                                       Test_U_SessionMessage_t,
                                       Stream_SessionId_t,
                                       Test_U_SessionData,
                                       struct Stream_UserData> Test_U_MessageHandler;
#endif // ACE_WIN32 || ACE_WIN64

//////////////////////////////////////////

#if defined (ACE_WIN32) || defined (ACE_WIN64)
DATASTREAM_MODULE_INPUT_ONLY (Test_U_DirectShow_SessionData,                // session data type
                              enum Stream_SessionMessageType,                   // session event type
                              struct Test_U_DirectShow_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_dev_cam_source_mediafoundation_module_name_string,
                              Stream_INotify_t,                                 // stream notification interface type
                              Test_U_DirectShow_Source);                // writer type

DATASTREAM_MODULE_INPUT_ONLY (Test_U_MediaFoundation_SessionData,                // session data type
                              enum Stream_SessionMessageType,                   // session event type
                              struct Test_U_MediaFoundation_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_dev_cam_source_mediafoundation_module_name_string,
                              Stream_INotify_t,                                 // stream notification interface type
                              Test_U_MediaFoundation_Source);           // writer type
#else
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
                              libacestream_default_vis_libav_resize_module_name_string,
                              Stream_INotify_t,                                 // stream notification interface type
                              Test_U_LibAVResize);                      // writer type
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
DATASTREAM_MODULE_DUPLEX (Test_U_DirectShow_SessionData,                // session data type
                          enum Stream_SessionMessageType,                   // session event type
                          struct Test_U_DirectShow_ModuleHandlerConfiguration, // module handler configuration type
                          libacestream_default_stat_report_module_name_string,
                          Stream_INotify_t,                                 // stream notification interface type
                          Test_U_DirectShow_Statistic_ReaderTask_t, // reader type
                          Test_U_DirectShow_Statistic_WriterTask_t, // writer type
                          Test_U_DirectShow_StatisticReport);       // name

DATASTREAM_MODULE_DUPLEX (Test_U_MediaFoundation_SessionData,                // session data type
                          enum Stream_SessionMessageType,                   // session event type
                          struct Test_U_MediaFoundation_ModuleHandlerConfiguration, // module handler configuration type
                          libacestream_default_stat_report_module_name_string,
                          Stream_INotify_t,                                 // stream notification interface type
                          Test_U_MediaFoundation_Statistic_ReaderTask_t, // reader type
                          Test_U_MediaFoundation_Statistic_WriterTask_t, // writer type
                          Test_U_MediaFoundation_StatisticReport);  // name
#else
DATASTREAM_MODULE_DUPLEX (Test_U_SessionData,                // session data type
                          enum Stream_SessionMessageType,                   // session event type
                          struct Test_U_ModuleHandlerConfiguration, // module handler configuration type
                          libacestream_default_stat_report_module_name_string,
                          Stream_INotify_t,                                 // stream notification interface type
                          Test_U_Statistic_ReaderTask_t,            // reader type
                          Test_U_Statistic_WriterTask_t,            // writer type
                          Test_U_StatisticReport);                  // name
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
DATASTREAM_MODULE_INPUT_ONLY (Test_U_DirectShow_SessionData,                // session data type
                              enum Stream_SessionMessageType,                   // session event type
                              struct Test_U_DirectShow_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_vis_directshow_module_name_string,
                              Stream_INotify_t,                                 // stream notification interface type
                              Test_U_DirectShow_Direct3DDisplay);       // writer type

DATASTREAM_MODULE_INPUT_ONLY (Test_U_MediaFoundation_SessionData,                // session data type
                              enum Stream_SessionMessageType,                   // session event type
                              struct Test_U_MediaFoundation_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_vis_mediafoundation_module_name_string,
                              Stream_INotify_t,                                 // stream notification interface type
                              Test_U_MediaFoundation_Direct3DDisplay);  // writer type

DATASTREAM_MODULE_INPUT_ONLY (Test_U_DirectShow_SessionData,                // session data type
                              enum Stream_SessionMessageType,                   // session event type
                              struct Test_U_DirectShow_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_vis_directshow_module_name_string,
                              Stream_INotify_t,                                 // stream notification interface type
                              Test_U_DirectShow_DirectShowDisplay);     // writer type

DATASTREAM_MODULE_INPUT_ONLY (Test_U_MediaFoundation_SessionData,                      // session data type
                              enum Stream_SessionMessageType,                         // session event type
                              struct Test_U_MediaFoundation_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_vis_mediafoundation_module_name_string,
                              Stream_INotify_t,                                       // stream notification interface type
                              Test_U_MediaFoundation_MediaFoundationDisplay); // writer type
DATASTREAM_MODULE_INPUT_ONLY (Test_U_MediaFoundation_SessionData,                          // session data type
                              enum Stream_SessionMessageType,                             // session event type
                              struct Test_U_MediaFoundation_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_vis_mediafoundation_module_name_string,
                              Stream_INotify_t,                                           // stream notification interface type
                              Test_U_MediaFoundation_MediaFoundationDisplayNull); // writer type
#else
DATASTREAM_MODULE_INPUT_ONLY (Test_U_SessionData,                   // session data type
                              enum Stream_SessionMessageType,                   // session event type
                              struct Test_U_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_vis_gtk_window_module_name_string,
                              Stream_INotify_t,                                 // stream notification interface type
                              Test_U_Display_2);                        // writer type
DATASTREAM_MODULE_INPUT_ONLY (Test_U_SessionData,                   // session data type
                              enum Stream_SessionMessageType,                   // session event type
                              struct Test_U_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_vis_x11_window_module_name_string,
                              Stream_INotify_t,                                 // stream notification interface type
                              Test_U_Display);                          // writer type
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
DATASTREAM_MODULE_INPUT_ONLY (Test_U_DirectShow_SessionData,                // session data type
                              enum Stream_SessionMessageType,                   // session event type
                              struct Test_U_DirectShow_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_misc_messagehandler_module_name_string,
                              Stream_INotify_t,                                 // stream notification interface type
                              Test_U_DirectShow_MessageHandler);        // writer type

DATASTREAM_MODULE_INPUT_ONLY (Test_U_MediaFoundation_SessionData,                // session data type
                              enum Stream_SessionMessageType,                   // session event type
                              struct Test_U_MediaFoundation_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_misc_messagehandler_module_name_string,
                              Stream_INotify_t,                                 // stream notification interface type
                              Test_U_MediaFoundation_MessageHandler);   // writer type
#else
DATASTREAM_MODULE_INPUT_ONLY (Test_U_SessionData,                           // session data type
                              enum Stream_SessionMessageType,                       // session event type
                              struct Test_U_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_misc_messagehandler_module_name_string,
                              Stream_INotify_t,                                     // stream notification interface type
                              Test_U_MessageHandler);                       // writer type
#endif // ACE_WIN32 || ACE_WIN64

#endif
