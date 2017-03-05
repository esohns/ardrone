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

#include <ace/INET_Addr.h>
#include <ace/Synch_Traits.h>

#include "common_time_common.h"

#include "stream_common.h"
#include "stream_streammodule_base.h"

#include "stream_dec_h264_nal_decoder.h"
#include "stream_dec_libav_decoder.h"

#include "stream_file_sink.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "stream_misc_directshow_asynch_source_filter.h"
#include "stream_misc_directshow_source_filter.h"
#endif
#include "stream_misc_dump.h"
#include "stream_misc_statistic_report.h"

#include "stream_module_source.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include "stream_vis_target_directshow.h"
#include "stream_vis_target_mediafoundation.h"
#else
#include "stream_vis_gtk_pixbuf.h"
#endif

#include "net_connection_manager.h"

#include "ardrone_message.h"
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
struct ARDrone_StreamState;

typedef Stream_Statistic ARDrone_RuntimeStatistic_t;
//typedef Net_Connection_Manager_T<ACE_INET_Addr,
//                                 struct ARDrone_ConnectionConfiguration,
//                                 struct ARDrone_ConnectionState,
//                                 ARDrone_RuntimeStatistic_t,
//                                 struct ARDrone_UserData> ARDrone_ConnectionManager_t;
typedef Stream_INotify_T<enum Stream_SessionMessageType> ARDrone_IStreamNotify_t;

//typedef Stream_Module_Net_SourceH_T<ACE_MT_SYNCH,
//                                    ARDrone_ControlMessage_t,
//                                    ARDrone_Message,
//                                    ARDrone_SessionMessage,
//                                    struct ARDrone_ModuleHandlerConfiguration,
//                                    enum Stream_ControlType,
//                                    enum Stream_SessionMessageType,
//                                    struct ARDrone_StreamState,
//                                    struct ARDrone_SessionData,
//                                    ARDrone_StreamSessionData_t,
//                                    ARDrone_RuntimeStatistic_t,
//                                    ARDrone_ConnectionManager_t,
//                                    ARDrone_TCPConnector_t,
//                                    struct ARDrone_UserData> ARDrone_Module_NetSource;
//DATASTREAM_MODULE_INPUT_ONLY (struct ARDrone_SessionData,                // session data type
//                              enum Stream_SessionMessageType,            // session event type
//                              struct ARDrone_ModuleHandlerConfiguration, // module handler configuration type
//                              ARDrone_IStreamNotify_t,                   // stream notification interface type
//                              ARDrone_Module_NetSource);                 // writer type
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
                                    ARDrone_RuntimeStatistic_t,
                                    struct ARDrone_SocketHandlerConfiguration,
                                    ARDrone_ConnectionManager_t,
                                    ARDrone_AsynchTCPConnector_t,
                                    struct ARDrone_UserData> ARDrone_Module_AsynchNetSource;
DATASTREAM_MODULE_INPUT_ONLY (struct ARDrone_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_ModuleHandlerConfiguration, // module handler configuration type
                              ARDrone_IStreamNotify_t,                   // stream notification interface type
                              ARDrone_Module_AsynchNetSource);           // writer type
//typedef Stream_Module_Net_SourceH_T<ACE_MT_SYNCH,
//                                    ARDrone_ControlMessage_t,
//                                    ARDrone_Message,
//                                    ARDrone_SessionMessage,
//                                    struct ARDrone_ModuleHandlerConfiguration,
//                                    enum Stream_ControlType,
//                                    enum Stream_SessionMessageType,
//                                    struct ARDrone_StreamState,
//                                    struct ARDrone_SessionData,
//                                    ARDrone_StreamSessionData_t,
//                                    ARDrone_RuntimeStatistic_t,
//                                    ARDrone_ConnectionManager_t,
//                                    ARDrone_UDPConnector_t,
//                                    struct ARDrone_UserData> ARDrone_Module_UDPNetSource;
//DATASTREAM_MODULE_INPUT_ONLY (struct ARDrone_SessionData,                // session data type
//                              enum Stream_SessionMessageType,            // session event type
//                              struct ARDrone_ModuleHandlerConfiguration, // module handler configuration type
//                              ARDrone_IStreamNotify_t,                   // stream notification interface type
//                              ARDrone_Module_UDPNetSource);              // writer type
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
                                    ARDrone_RuntimeStatistic_t,
                                    struct ARDrone_SocketHandlerConfiguration,
                                    ARDrone_ConnectionManager_t,
                                    ARDrone_AsynchUDPConnector_t,
                                    struct ARDrone_UserData> ARDrone_Module_AsynchUDPNetSource;
DATASTREAM_MODULE_INPUT_ONLY (struct ARDrone_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_ModuleHandlerConfiguration, // module handler configuration type
                              ARDrone_IStreamNotify_t,                   // stream notification interface type
                              ARDrone_Module_AsynchUDPNetSource);        // writer type

typedef Stream_Module_StatisticReport_ReaderTask_T<ACE_MT_SYNCH,
                                                   Common_TimePolicy_t,
                                                   struct ARDrone_ModuleHandlerConfiguration,
                                                   ARDrone_ControlMessage_t,
                                                   ARDrone_Message,
                                                   ARDrone_SessionMessage,
                                                   int,
                                                   ARDrone_RuntimeStatistic_t,
                                                   struct ARDrone_SessionData,
                                                   ARDrone_StreamSessionData_t> ARDrone_Module_Statistic_ReaderTask_t;
typedef Stream_Module_StatisticReport_WriterTask_T<ACE_MT_SYNCH,
                                                   Common_TimePolicy_t,
                                                   struct ARDrone_ModuleHandlerConfiguration,
                                                   ARDrone_ControlMessage_t,
                                                   ARDrone_Message,
                                                   ARDrone_SessionMessage,
                                                   int,
                                                   ARDrone_RuntimeStatistic_t,
                                                   struct ARDrone_SessionData,
                                                   ARDrone_StreamSessionData_t> ARDrone_Module_Statistic_WriterTask_t;
DATASTREAM_MODULE_DUPLEX (struct ARDrone_SessionData,                // session data type
                          enum Stream_SessionMessageType,            // session event type
                          struct ARDrone_ModuleHandlerConfiguration, // module handler configuration type
                          ARDrone_IStreamNotify_t,                   // stream notification interface type
                          ARDrone_Module_Statistic_ReaderTask_t,     // reader type
                          ARDrone_Module_Statistic_WriterTask_t,     // writer type
                          ARDrone_Module_StatisticReport);           // name

typedef ARDrone_Module_PaVEDecoder_T<ACE_MT_SYNCH,
                                     Common_TimePolicy_t,
                                     struct ARDrone_ModuleHandlerConfiguration,
                                     ARDrone_ControlMessage_t,
                                     ARDrone_Message,
                                     ARDrone_SessionMessage,
                                     ARDrone_StreamSessionData_t> ARDrone_Module_PaVEDecoder;
DATASTREAM_MODULE_INPUT_ONLY (struct ARDrone_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_ModuleHandlerConfiguration, // module handler configuration type
                              ARDrone_IStreamNotify_t,                   // stream notification interface type
                              ARDrone_Module_PaVEDecoder);               // writer type
typedef ARDrone_Module_MAVLinkDecoder_T<ACE_MT_SYNCH,
                                        Common_TimePolicy_t,
                                        struct ARDrone_ModuleHandlerConfiguration,
                                        ARDrone_ControlMessage_t,
                                        ARDrone_Message,
                                        ARDrone_SessionMessage,
                                        ARDrone_StreamSessionData_t> ARDrone_Module_MAVLinkDecoder;
DATASTREAM_MODULE_INPUT_ONLY (struct ARDrone_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_ModuleHandlerConfiguration, // module handler configuration type
                              ARDrone_IStreamNotify_t,                   // stream notification interface type
                              ARDrone_Module_MAVLinkDecoder);            // writer type
typedef ARDrone_Module_NavDataDecoder_T<ACE_MT_SYNCH,
                                        Common_TimePolicy_t,
                                        struct ARDrone_ModuleHandlerConfiguration,
                                        ARDrone_ControlMessage_t,
                                        ARDrone_Message,
                                        ARDrone_SessionMessage,
                                        ARDrone_StreamSessionData_t> ARDrone_Module_NavDataDecoder;
DATASTREAM_MODULE_INPUT_ONLY (struct ARDrone_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_ModuleHandlerConfiguration, // module handler configuration type
                              ARDrone_IStreamNotify_t,                   // stream notification interface type
                              ARDrone_Module_NavDataDecoder);            // writer type

typedef Stream_Decoder_LibAVDecoder_T<ACE_MT_SYNCH,
                                      Common_TimePolicy_t,
                                      struct ARDrone_ModuleHandlerConfiguration,
                                      ARDrone_ControlMessage_t,
                                      ARDrone_Message,
                                      ARDrone_SessionMessage,
                                      ARDrone_StreamSessionData_t> ARDrone_Module_H264Decoder;
DATASTREAM_MODULE_INPUT_ONLY (struct ARDrone_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_ModuleHandlerConfiguration, // module handler configuration type
                              ARDrone_IStreamNotify_t,                   // stream notification interface type
                              ARDrone_Module_H264Decoder);               // writer type
//typedef Stream_Decoder_H264_NAL_Decoder_T<ACE_MT_SYNCH,
//                                          Common_TimePolicy_t,
//                                          struct ARDrone_ModuleHandlerConfiguration,
//                                          ARDrone_ControlMessage_t,
//                                          ARDrone_Message,
//                                          ARDrone_SessionMessage,
//                                          ARDrone_StreamSessionData_t> ARDrone_Module_H264NALDecoder;
//DATASTREAM_MODULE_INPUT_ONLY (struct ARDrone_SessionData,                // session data type
//                              enum Stream_SessionMessageType,            // session event type
//                              struct ARDrone_ModuleHandlerConfiguration, // module handler configuration type
//                              ARDrone_IStreamNotify_t,                   // stream notification interface type
//                              ARDrone_Module_H264NALDecoder);            // writer type

typedef ARDrone_Module_Controller_T<ACE_MT_SYNCH,
                                    Common_TimePolicy_t,
                                    struct ARDrone_ModuleHandlerConfiguration,
                                    ARDrone_ControlMessage_t,
                                    ARDrone_Message,
                                    ARDrone_SessionMessage,
                                    ARDrone_StreamSessionData_t,
                                    struct ARDrone_SocketHandlerConfiguration,
                                    ARDrone_ConnectionManager_t,
                                    ARDrone_AsynchUDPConnector_t> ARDrone_Module_AsynchUDPNetTarget;
DATASTREAM_MODULE_INPUT_ONLY (struct ARDrone_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_ModuleHandlerConfiguration, // module handler configuration type
                              ARDrone_IStreamNotify_t,                   // stream notification interface type
                              ARDrone_Module_AsynchUDPNetTarget);        // writer type

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Stream_Misc_DirectShow_Source_Filter_T<Common_TimePolicy_t,
                                               ARDrone_SessionMessage,
                                               ARDrone_Message,
                                               struct ARDrone_DirectShow_FilterConfiguration,
                                               struct Stream_Miscellaneous_DirectShow_FilterPinConfiguration,
                                               struct _AMMediaType> ARDrone_DirectShowFilter_t;
typedef Stream_Misc_DirectShow_Asynch_Source_Filter_T<Common_TimePolicy_t,
                                                      ARDrone_SessionMessage,
                                                      ARDrone_Message,
                                                      struct ARDrone_DirectShow_FilterConfiguration,
                                                      struct Stream_Miscellaneous_DirectShow_FilterPinConfiguration,
                                                      struct _AMMediaType> ARDrone_AsynchDirectShowFilter_t;
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
DATASTREAM_MODULE_INPUT_ONLY (struct ARDrone_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_ModuleHandlerConfiguration, // module handler configuration type
                              ARDrone_IStreamNotify_t,                   // stream notification interface type
                              ARDrone_Module_DirectShowDisplay);         // writer type
typedef Stream_Vis_Target_MediaFoundation_T<ACE_MT_SYNCH,
                                            Common_TimePolicy_t,
                                            struct ARDrone_ModuleHandlerConfiguration,
                                            ARDrone_ControlMessage_t,
                                            ARDrone_Message,
                                            ARDrone_SessionMessage,
                                            struct ARDrone_SessionData,
                                            ARDrone_StreamSessionData_t,
                                            struct ARDrone_UserData> ARDrone_Module_MediaFoundationDisplay;
DATASTREAM_MODULE_INPUT_ONLY (struct ARDrone_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_ModuleHandlerConfiguration, // module handler configuration type
                              ARDrone_IStreamNotify_t,                   // stream notification interface type
                              ARDrone_Module_MediaFoundationDisplay);    // writer type
#else
typedef Stream_Module_Vis_GTK_Pixbuf_T<ACE_MT_SYNCH,
                                       Common_TimePolicy_t,
                                       struct ARDrone_ModuleHandlerConfiguration,
                                       ARDrone_ControlMessage_t,
                                       ARDrone_Message,
                                       ARDrone_SessionMessage,
                                       ARDrone_StreamSessionData_t> ARDrone_Module_Display;
DATASTREAM_MODULE_INPUT_ONLY (struct ARDrone_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_ModuleHandlerConfiguration, // module handler configuration type
                              ARDrone_IStreamNotify_t,                   // stream notification interface type
                              ARDrone_Module_Display);                   // writer type
#endif

typedef Stream_Module_Dump_T<ACE_MT_SYNCH,
                             Common_TimePolicy_t,
                             struct ARDrone_ModuleHandlerConfiguration,
                             ARDrone_ControlMessage_t,
                             ARDrone_Message,
                             ARDrone_SessionMessage,
                             ARDrone_StreamSessionData_t,
                             struct ARDrone_UserData> ARDrone_Module_Dump;
DATASTREAM_MODULE_INPUT_ONLY (struct ARDrone_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_ModuleHandlerConfiguration, // module handler configuration type
                              ARDrone_IStreamNotify_t,                   // stream notification interface type
                              ARDrone_Module_Dump);                      // writer type

typedef Stream_Module_FileWriter_T<ACE_MT_SYNCH,
                                   Common_TimePolicy_t,
                                   struct ARDrone_ModuleHandlerConfiguration,
                                   ARDrone_ControlMessage_t,
                                   ARDrone_Message,
                                   ARDrone_SessionMessage,
                                   struct ARDrone_SessionData> ARDrone_Module_FileWriter;
DATASTREAM_MODULE_INPUT_ONLY (struct ARDrone_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_ModuleHandlerConfiguration, // module handler configuration type
                              ARDrone_IStreamNotify_t,                   // stream notification interface type
                              ARDrone_Module_FileWriter);                // writer type

#endif
