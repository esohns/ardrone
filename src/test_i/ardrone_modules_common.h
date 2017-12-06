/***************************************************************************
 *   Copyright (C) 2010 by Erik Sohns   *
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

#ifndef ARDRONE_MODULES_COMMON_H
#define ARDRONE_MODULES_COMMON_H

#include "ace/INET_Addr.h"
#include "ace/Synch_Traits.h"

#include "common_time_common.h"
#include "common_timer_manager_common.h"

#include "stream_common.h"
#include "stream_session_data.h"
#include "stream_streammodule_base.h"

#include "stream_dec_avi_encoder.h"
#include "stream_dec_h264_nal_decoder.h"
#include "stream_dec_libav_decoder.h"

#include "stream_file_sink.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "stream_lib_directshow_asynch_source_filter.h"
#include "stream_lib_directshow_source_filter.h"
#endif
#include "stream_misc_dump.h"

#include "stream_net_source.h"

#include "stream_stat_statistic_report.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "stream_vis_target_directshow.h"
#include "stream_vis_target_mediafoundation.h"
#else
#include "stream_vis_gtk_pixbuf.h"
#endif

#include "net_connection_manager.h"

#include "ardrone_message.h"
#include "ardrone_module_control_decoder.h"
#include "ardrone_module_controller.h"
#include "ardrone_module_mavlink_decoder.h"
#include "ardrone_module_navdata_decoder.h"
#include "ardrone_module_pave_decoder.h"
#include "ardrone_network.h"
#include "ardrone_sessionmessage.h"
#include "ardrone_types.h"

// forward declarations
struct ARDrone_ConnectionConfiguration;
struct ARDrone_ConnectionState;
struct ARDrone_ModuleHandlerConfiguration;
struct ARDrone_SessionData;
typedef Stream_SessionData_T<struct ARDrone_SessionData> ARDrone_SessionData_t;
struct ARDrone_StreamState;

//typedef struct Stream_Statistic struct ARDrone_Statistic;
//typedef Net_Connection_Manager_T<ACE_INET_Addr,
//                                 struct ARDrone_ConnectionConfiguration,
//                                 struct ARDrone_ConnectionState,
//                                 struct ARDrone_Statistic,
//                                 struct ARDrone_UserData> ARDrone_ConnectionManager_t;
//typedef Stream_INotify_T<enum Stream_SessionMessageType> ARDrone_IStreamNotify_t;

typedef Stream_Module_Net_SourceH_T<ACE_MT_SYNCH,
                                    ARDrone_ControlMessage_t,
                                    ARDrone_Message,
                                    ARDrone_SessionMessage,
                                    struct ARDrone_ModuleHandlerConfiguration,
                                    enum Stream_ControlType,
                                    enum Stream_SessionMessageType,
                                    struct ARDrone_StreamState,
                                    struct ARDrone_SessionData,
                                    ARDrone_StreamSessionData_t,
                                    struct ARDrone_Statistic,
                                    Common_Timer_Manager_t,
                                    ARDrone_StreamConnectionConfigurationIterator_t,
                                    ARDrone_ConnectionManager_t,
                                    ARDrone_TCPConnector_t,
                                    struct ARDrone_UserData> ARDrone_Module_TCPSource;
typedef Stream_Module_Net_SourceH_T<ACE_MT_SYNCH,
                                    ARDrone_ControlMessage_t,
                                    ARDrone_Message,
                                    ARDrone_SessionMessage,
                                    struct ARDrone_ModuleHandlerConfiguration,
                                    enum Stream_ControlType,
                                    enum Stream_SessionMessageType,
                                    struct ARDrone_StreamState,
                                    struct ARDrone_SessionData,
                                    ARDrone_StreamSessionData_t,
                                    struct ARDrone_Statistic,
                                    Common_Timer_Manager_t,
                                    ARDrone_StreamConnectionConfigurationIterator_t,
                                    ARDrone_ConnectionManager_t,
                                    ARDrone_AsynchTCPConnector_t,
                                    struct ARDrone_UserData> ARDrone_Module_AsynchTCPSource;
typedef Stream_Module_Net_SourceH_T<ACE_MT_SYNCH,
                                    ARDrone_ControlMessage_t,
                                    ARDrone_Message,
                                    ARDrone_SessionMessage,
                                    struct ARDrone_ModuleHandlerConfiguration,
                                    enum Stream_ControlType,
                                    enum Stream_SessionMessageType,
                                    struct ARDrone_StreamState,
                                    struct ARDrone_SessionData,
                                    ARDrone_StreamSessionData_t,
                                    struct ARDrone_Statistic,
                                    Common_Timer_Manager_t,
                                    ARDrone_StreamConnectionConfigurationIterator_t,
                                    ARDrone_ConnectionManager_t,
                                    ARDrone_UDPConnector_t,
                                    struct ARDrone_UserData> ARDrone_Module_UDPSource;
typedef Stream_Module_Net_SourceH_T<ACE_MT_SYNCH,
                                    ARDrone_ControlMessage_t,
                                    ARDrone_Message,
                                    ARDrone_SessionMessage,
                                    struct ARDrone_ModuleHandlerConfiguration,
                                    enum Stream_ControlType,
                                    enum Stream_SessionMessageType,
                                    struct ARDrone_StreamState,
                                    struct ARDrone_SessionData,
                                    ARDrone_StreamSessionData_t,
                                    struct ARDrone_Statistic,
                                    Common_Timer_Manager_t,
                                    ARDrone_StreamConnectionConfigurationIterator_t,
                                    ARDrone_ConnectionManager_t,
                                    ARDrone_AsynchUDPConnector_t,
                                    struct ARDrone_UserData> ARDrone_Module_AsynchUDPSource;

typedef Stream_Statistic_StatisticReport_ReaderTask_T<ACE_MT_SYNCH,
                                                      Common_TimePolicy_t,
                                                      struct ARDrone_ModuleHandlerConfiguration,
                                                      ARDrone_ControlMessage_t,
                                                      ARDrone_Message,
                                                      ARDrone_SessionMessage,
                                                      int,
                                                      struct ARDrone_Statistic,
                                                      Common_Timer_Manager_t,
                                                      struct ARDrone_SessionData,
                                                      ARDrone_StreamSessionData_t> ARDrone_Module_Statistic_ReaderTask_t;
typedef Stream_Statistic_StatisticReport_WriterTask_T<ACE_MT_SYNCH,
                                                      Common_TimePolicy_t,
                                                      struct ARDrone_ModuleHandlerConfiguration,
                                                      ARDrone_ControlMessage_t,
                                                      ARDrone_Message,
                                                      ARDrone_SessionMessage,
                                                      int,
                                                      struct ARDrone_Statistic,
                                                      Common_Timer_Manager_t,
                                                      struct ARDrone_SessionData,
                                                      ARDrone_StreamSessionData_t> ARDrone_Module_Statistic_WriterTask_t;

typedef ARDrone_Module_ControlDecoder_T<ACE_MT_SYNCH,
                                        Common_TimePolicy_t,
                                        struct ARDrone_ModuleHandlerConfiguration,
                                        ARDrone_ControlMessage_t,
                                        ARDrone_Message,
                                        ARDrone_SessionMessage,
                                        ARDrone_StreamSessionData_t> ARDrone_Module_ControlDecoder;
typedef ARDrone_Module_MAVLinkDecoder_T<ACE_MT_SYNCH,
                                        Common_TimePolicy_t,
                                        struct ARDrone_ModuleHandlerConfiguration,
                                        ARDrone_ControlMessage_t,
                                        ARDrone_Message,
                                        ARDrone_SessionMessage,
                                        ARDrone_StreamSessionData_t> ARDrone_Module_MAVLinkDecoder;
typedef ARDrone_Module_NavDataDecoder_T<ACE_MT_SYNCH,
                                        Common_TimePolicy_t,
                                        struct ARDrone_ModuleHandlerConfiguration,
                                        ARDrone_ControlMessage_t,
                                        ARDrone_Message,
                                        ARDrone_SessionMessage,
                                        ARDrone_StreamSessionData_t> ARDrone_Module_NavDataDecoder;
typedef ARDrone_Module_PaVEDecoder_T<ACE_MT_SYNCH,
                                     Common_TimePolicy_t,
                                     struct ARDrone_ModuleHandlerConfiguration,
                                     ARDrone_ControlMessage_t,
                                     ARDrone_Message,
                                     ARDrone_SessionMessage,
                                     ARDrone_StreamSessionData_t> ARDrone_Module_PaVEDecoder;

typedef Stream_Decoder_LibAVDecoder_T<ACE_MT_SYNCH,
                                      Common_TimePolicy_t,
                                      struct ARDrone_ModuleHandlerConfiguration,
                                      ARDrone_ControlMessage_t,
                                      ARDrone_Message,
                                      ARDrone_SessionMessage,
                                      ARDrone_StreamSessionData_t> ARDrone_Module_H264Decoder;

typedef ARDrone_Module_Controller_T<ACE_MT_SYNCH,
                                    Common_TimePolicy_t,
                                    struct ARDrone_ModuleHandlerConfiguration,
                                    ARDrone_ControlMessage_t,
                                    ARDrone_Message,
                                    ARDrone_SessionMessage,
                                    ARDrone_StreamSessionData_t,
                                    ARDrone_WLANMonitor_t,
                                    ARDrone_StreamConnectionConfigurationIterator_t,
                                    ARDrone_ConnectionManager_t,
                                    ARDrone_UDPConnector_t> ARDrone_Module_Controller;
typedef ARDrone_Module_Controller_T<ACE_MT_SYNCH,
                                    Common_TimePolicy_t,
                                    struct ARDrone_ModuleHandlerConfiguration,
                                    ARDrone_ControlMessage_t,
                                    ARDrone_Message,
                                    ARDrone_SessionMessage,
                                    ARDrone_StreamSessionData_t,
                                    ARDrone_WLANMonitor_t,
                                    ARDrone_StreamConnectionConfigurationIterator_t,
                                    ARDrone_ConnectionManager_t,
                                    ARDrone_AsynchUDPConnector_t> ARDrone_Module_AsynchController;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Stream_MediaFramework_DirectShow_Source_Filter_T<Common_TimePolicy_t,
                                                         ARDrone_SessionMessage,
                                                         ARDrone_Message,
                                                         struct ARDrone_DirectShow_FilterConfiguration,
                                                         struct Stream_MediaFramework_DirectShow_FilterPinConfiguration,
                                                         struct _AMMediaType> ARDrone_DirectShowFilter_t;
//typedef Stream_MediaFramework_DirectShow_Asynch_Source_Filter_T<Common_TimePolicy_t,
//                                                                ARDrone_SessionMessage,
//                                                                ARDrone_Message,
//                                                                struct ARDrone_DirectShow_FilterConfiguration,
//                                                                struct Stream_MediaFramework_DirectShow_FilterPinConfiguration,
//                                                                struct _AMMediaType> ARDrone_AsynchDirectShowFilter_t;
typedef Stream_Vis_Target_DirectShow_T<ACE_MT_SYNCH,
                                       Common_TimePolicy_t,
                                       struct ARDrone_ModuleHandlerConfiguration,
                                       ARDrone_ControlMessage_t,
                                       ARDrone_Message,
                                       ARDrone_SessionMessage,
                                       ARDrone_StreamSessionData_t,
                                       struct ARDrone_SessionData,
                                       struct ARDrone_DirectShow_FilterConfiguration,
                                       struct ARDrone_DirectShow_PinConfiguration,
                                       ARDrone_DirectShowFilter_t> ARDrone_Module_DirectShowDisplay;
typedef Stream_Vis_Target_MediaFoundation_T<ACE_MT_SYNCH,
                                            Common_TimePolicy_t,
                                            struct ARDrone_ModuleHandlerConfiguration,
                                            ARDrone_ControlMessage_t,
                                            ARDrone_Message,
                                            ARDrone_SessionMessage,
                                            struct ARDrone_SessionData,
                                            ARDrone_StreamSessionData_t,
                                            struct ARDrone_UserData> ARDrone_Module_MediaFoundationDisplay;
#else
typedef Stream_Module_Vis_GTK_Pixbuf_T<ACE_MT_SYNCH,
                                       Common_TimePolicy_t,
                                       struct ARDrone_ModuleHandlerConfiguration,
                                       ARDrone_ControlMessage_t,
                                       ARDrone_Message,
                                       ARDrone_SessionMessage,
                                       ARDrone_StreamSessionData_t> ARDrone_Module_Display;
#endif

typedef Stream_Module_Dump_T<ACE_MT_SYNCH,
                             Common_TimePolicy_t,
                             struct ARDrone_ModuleHandlerConfiguration,
                             ARDrone_ControlMessage_t,
                             ARDrone_Message,
                             ARDrone_SessionMessage,
                             ARDrone_StreamSessionData_t,
                             struct ARDrone_UserData> ARDrone_Module_Dump;

typedef Stream_Module_FileWriter_T<ACE_MT_SYNCH,
                                   Common_TimePolicy_t,
                                   struct ARDrone_ModuleHandlerConfiguration,
                                   ARDrone_ControlMessage_t,
                                   ARDrone_Message,
                                   ARDrone_SessionMessage,
                                   struct ARDrone_SessionData> ARDrone_Module_FileWriter;

typedef Stream_Decoder_AVIEncoder_ReaderTask_T<ACE_MT_SYNCH,
                                               Common_TimePolicy_t,
                                               ARDrone_SessionData_t,
                                               struct ARDrone_SessionData> ARDrone_Module_AVIEncoder_ReaderTask_t;
typedef Stream_Decoder_AVIEncoder_WriterTask_T<ACE_MT_SYNCH,
                                               Common_TimePolicy_t,
                                               struct ARDrone_ModuleHandlerConfiguration,
                                               ARDrone_ControlMessage_t,
                                               ARDrone_Message,
                                               ARDrone_SessionMessage,
                                               ARDrone_SessionData_t,
                                               struct ARDrone_SessionData,
                                               enum AVCodecID,
                                               struct ARDrone_UserData> ARDrone_Module_AVIEncoder_WriterTask_t;

//////////////////////////////////////////

DATASTREAM_MODULE_INPUT_ONLY (struct ARDrone_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_net_source_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_TCPSource);                 // writer type
DATASTREAM_MODULE_INPUT_ONLY (struct ARDrone_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_net_source_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_AsynchTCPSource);           // writer type
DATASTREAM_MODULE_INPUT_ONLY (struct ARDrone_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_net_source_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_UDPSource);                 // writer type
DATASTREAM_MODULE_INPUT_ONLY (struct ARDrone_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_net_source_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_AsynchUDPSource);           // writer type

DATASTREAM_MODULE_DUPLEX (struct ARDrone_SessionData,                // session data type
                          enum Stream_SessionMessageType,            // session event type
                          struct ARDrone_ModuleHandlerConfiguration, // module handler configuration type
                          libacestream_default_stat_report_module_name_string,
                          Stream_INotify_t,                          // stream notification interface type
                          ARDrone_Module_Statistic_ReaderTask_t,     // reader type
                          ARDrone_Module_Statistic_WriterTask_t,     // writer type
                          ARDrone_Module_StatisticReport);           // name

DATASTREAM_MODULE_INPUT_ONLY (struct ARDrone_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_misc_parser_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_ControlDecoder);            // writer type
DATASTREAM_MODULE_INPUT_ONLY (struct ARDrone_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_misc_parser_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_MAVLinkDecoder);            // writer type
DATASTREAM_MODULE_INPUT_ONLY (struct ARDrone_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_misc_parser_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_NavDataDecoder);            // writer type
DATASTREAM_MODULE_INPUT_ONLY (struct ARDrone_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_ModuleHandlerConfiguration, // module handler configuration type
                              ardrone_default_pave_decoder_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_PaVEDecoder);               // writer type

DATASTREAM_MODULE_INPUT_ONLY (struct ARDrone_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_dec_libav_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_H264Decoder);               // writer type

DATASTREAM_MODULE_INPUT_ONLY (struct ARDrone_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_net_target_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_Controller);                // writer type
DATASTREAM_MODULE_INPUT_ONLY (struct ARDrone_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_net_target_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_AsynchController);          // writer type

#if defined (ACE_WIN32) || defined (ACE_WIN64)
DATASTREAM_MODULE_INPUT_ONLY (struct ARDrone_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_vis_directshow_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_DirectShowDisplay);         // writer type
DATASTREAM_MODULE_INPUT_ONLY (struct ARDrone_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_vis_mediafoundation_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_MediaFoundationDisplay);    // writer type
#else
DATASTREAM_MODULE_INPUT_ONLY (struct ARDrone_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_vis_gtk_pixbuf_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_Display);                   // writer type
#endif

DATASTREAM_MODULE_INPUT_ONLY (struct ARDrone_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_misc_dump_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_Dump);                      // writer type

DATASTREAM_MODULE_INPUT_ONLY (struct ARDrone_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_file_sink_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_FileWriter);                // writer type

DATASTREAM_MODULE_DUPLEX (struct ARDrone_SessionData,                // session data type
                          enum Stream_SessionMessageType,            // session event type
                          struct ARDrone_ModuleHandlerConfiguration, // module handler configuration type
                          libacestream_default_dec_avi_encoder_module_name_string,
                          Stream_INotify_t,                          // stream notification interface type
                          ARDrone_Module_AVIEncoder_ReaderTask_t,    // reader type
                          ARDrone_Module_AVIEncoder_WriterTask_t,    // writer type
                          ARDrone_Module_AVIEncoder);                // name

#endif
