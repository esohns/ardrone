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
#else
#include "stream_lib_ffmpeg_common.h"
#endif // ACE_WIN32 || ACE_WIN64
#include "stream_misc_dump.h"

#include "stream_net_source.h"

#include "stream_stat_statistic_report.h"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#if defined (GUI_SUPPORT)
#include "stream_vis_target_direct3d.h"
#include "stream_vis_target_directshow.h"
#include "stream_vis_target_mediafoundation.h"
#endif // GUI_SUPPORT
#else
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
#include "stream_vis_gtk_pixbuf.h"
#elif defined (WXWIDGETS_USE)
#include "stream_vis_x11_window.h"
#endif
#endif // GUI_SUPPORT
#endif // ACE_WIN32 || ACE_WIN64

#include "net_connection_manager.h"

//#include "ardrone_message.h"
#include "ardrone_module_control_decoder.h"
#include "ardrone_module_controller.h"
#include "ardrone_module_mavlink_decoder.h"
#include "ardrone_module_navdata_decoder.h"
#include "ardrone_module_pave_decoder.h"
#include "ardrone_network.h"
//#include "ardrone_sessionmessage.h"
#include "ardrone_types.h"

// forward declarations
struct ARDrone_ConnectionConfiguration;
struct ARDrone_ConnectionState;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
class ARDrone_DirectShow_SessionData;
typedef Stream_SessionData_T<ARDrone_DirectShow_SessionData> ARDrone_DirectShow_SessionData_t;
#else
struct ARDrone_SessionData;
typedef Stream_SessionData_T<struct ARDrone_SessionData> ARDrone_SessionData_t;
#endif // ACE_WIN32 || ACE_WIN64
struct ARDrone_StreamState;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Stream_Module_Net_SourceH_T<ACE_MT_SYNCH,
                                    ARDrone_ControlMessage_t,
                                    ARDrone_Message,
                                    ARDrone_DirectShow_SessionMessage,
                                    struct ARDrone_DirectShow_ModuleHandlerConfiguration,
                                    enum Stream_ControlType,
                                    enum Stream_SessionMessageType,
                                    struct ARDrone_StreamState,
                                    ARDrone_DirectShow_SessionData,
                                    ARDrone_DirectShow_SessionData_t,
                                    struct ARDrone_Statistic,
                                    Common_Timer_Manager_t,
                                    ARDrone_DirectShow_Stream_ConnectionConfigurationIterator_t,
                                    ARDrone_DirectShow_ConnectionManager_t,
                                    ARDrone_DirectShow_TCPConnector_t,
                                    struct ARDrone_UserData> ARDrone_Module_DirectShow_TCPSource;
typedef Stream_Module_Net_SourceH_T<ACE_MT_SYNCH,
                                    ARDrone_ControlMessage_t,
                                    ARDrone_Message,
                                    ARDrone_DirectShow_SessionMessage,
                                    struct ARDrone_DirectShow_ModuleHandlerConfiguration,
                                    enum Stream_ControlType,
                                    enum Stream_SessionMessageType,
                                    struct ARDrone_StreamState,
                                    ARDrone_DirectShow_SessionData,
                                    ARDrone_DirectShow_SessionData_t,
                                    struct ARDrone_Statistic,
                                    Common_Timer_Manager_t,
                                    ARDrone_DirectShow_Stream_ConnectionConfigurationIterator_t,
                                    ARDrone_DirectShow_ConnectionManager_t,
                                    ARDrone_DirectShow_AsynchTCPConnector_t,
                                    struct ARDrone_UserData> ARDrone_Module_DirectShow_AsynchTCPSource;
typedef Stream_Module_Net_SourceH_T<ACE_MT_SYNCH,
                                    ARDrone_ControlMessage_t,
                                    ARDrone_Message,
                                    ARDrone_DirectShow_SessionMessage,
                                    struct ARDrone_DirectShow_ModuleHandlerConfiguration,
                                    enum Stream_ControlType,
                                    enum Stream_SessionMessageType,
                                    struct ARDrone_StreamState,
                                    ARDrone_DirectShow_SessionData,
                                    ARDrone_DirectShow_SessionData_t,
                                    struct ARDrone_Statistic,
                                    Common_Timer_Manager_t,
                                    ARDrone_DirectShow_Stream_ConnectionConfigurationIterator_t,
                                    ARDrone_DirectShow_ConnectionManager_t,
                                    ARDrone_DirectShow_UDPConnector_t,
                                    struct ARDrone_UserData> ARDrone_Module_DirectShow_UDPSource;
typedef Stream_Module_Net_SourceH_T<ACE_MT_SYNCH,
                                    ARDrone_ControlMessage_t,
                                    ARDrone_Message,
                                    ARDrone_DirectShow_SessionMessage,
                                    struct ARDrone_DirectShow_ModuleHandlerConfiguration,
                                    enum Stream_ControlType,
                                    enum Stream_SessionMessageType,
                                    struct ARDrone_StreamState,
                                    ARDrone_DirectShow_SessionData,
                                    ARDrone_DirectShow_SessionData_t,
                                    struct ARDrone_Statistic,
                                    Common_Timer_Manager_t,
                                    ARDrone_DirectShow_Stream_ConnectionConfigurationIterator_t,
                                    ARDrone_DirectShow_ConnectionManager_t,
                                    ARDrone_DirectShow_AsynchUDPConnector_t,
                                    struct ARDrone_UserData> ARDrone_Module_DirectShow_AsynchUDPSource;

typedef Stream_Module_Net_SourceH_T<ACE_MT_SYNCH,
                                    ARDrone_ControlMessage_t,
                                    ARDrone_Message,
                                    ARDrone_DirectShow_SessionMessage,
                                    struct ARDrone_MediaFoundation_ModuleHandlerConfiguration,
                                    enum Stream_ControlType,
                                    enum Stream_SessionMessageType,
                                    struct ARDrone_StreamState,
                                    ARDrone_DirectShow_SessionData,
                                    ARDrone_DirectShow_SessionData_t,
                                    struct ARDrone_Statistic,
                                    Common_Timer_Manager_t,
                                    ARDrone_MediaFoundation_Stream_ConnectionConfigurationIterator_t,
                                    ARDrone_MediaFoundation_ConnectionManager_t,
                                    ARDrone_MediaFoundation_TCPConnector_t,
                                    struct ARDrone_UserData> ARDrone_Module_MediaFoundation_TCPSource;
typedef Stream_Module_Net_SourceH_T<ACE_MT_SYNCH,
                                    ARDrone_ControlMessage_t,
                                    ARDrone_Message,
                                    ARDrone_DirectShow_SessionMessage,
                                    struct ARDrone_MediaFoundation_ModuleHandlerConfiguration,
                                    enum Stream_ControlType,
                                    enum Stream_SessionMessageType,
                                    struct ARDrone_StreamState,
                                    ARDrone_DirectShow_SessionData,
                                    ARDrone_DirectShow_SessionData_t,
                                    struct ARDrone_Statistic,
                                    Common_Timer_Manager_t,
                                    ARDrone_MediaFoundation_Stream_ConnectionConfigurationIterator_t,
                                    ARDrone_MediaFoundation_ConnectionManager_t,
                                    ARDrone_MediaFoundation_AsynchTCPConnector_t,
                                    struct ARDrone_UserData> ARDrone_Module_MediaFoundation_AsynchTCPSource;
typedef Stream_Module_Net_SourceH_T<ACE_MT_SYNCH,
                                    ARDrone_ControlMessage_t,
                                    ARDrone_Message,
                                    ARDrone_DirectShow_SessionMessage,
                                    struct ARDrone_MediaFoundation_ModuleHandlerConfiguration,
                                    enum Stream_ControlType,
                                    enum Stream_SessionMessageType,
                                    struct ARDrone_StreamState,
                                    ARDrone_DirectShow_SessionData,
                                    ARDrone_DirectShow_SessionData_t,
                                    struct ARDrone_Statistic,
                                    Common_Timer_Manager_t,
                                    ARDrone_MediaFoundation_Stream_ConnectionConfigurationIterator_t,
                                    ARDrone_MediaFoundation_ConnectionManager_t,
                                    ARDrone_MediaFoundation_UDPConnector_t,
                                    struct ARDrone_UserData> ARDrone_Module_MediaFoundation_UDPSource;
typedef Stream_Module_Net_SourceH_T<ACE_MT_SYNCH,
                                    ARDrone_ControlMessage_t,
                                    ARDrone_Message,
                                    ARDrone_DirectShow_SessionMessage,
                                    struct ARDrone_MediaFoundation_ModuleHandlerConfiguration,
                                    enum Stream_ControlType,
                                    enum Stream_SessionMessageType,
                                    struct ARDrone_StreamState,
                                    ARDrone_DirectShow_SessionData,
                                    ARDrone_DirectShow_SessionData_t,
                                    struct ARDrone_Statistic,
                                    Common_Timer_Manager_t,
                                    ARDrone_MediaFoundation_Stream_ConnectionConfigurationIterator_t,
                                    ARDrone_MediaFoundation_ConnectionManager_t,
                                    ARDrone_MediaFoundation_AsynchUDPConnector_t,
                                    struct ARDrone_UserData> ARDrone_Module_MediaFoundation_AsynchUDPSource;
#else
typedef Stream_Module_Net_SourceH_T<ACE_MT_SYNCH,
                                    ARDrone_ControlMessage_t,
                                    ARDrone_Message,
                                    ARDrone_SessionMessage,
                                    struct ARDrone_ModuleHandlerConfiguration,
                                    enum Stream_ControlType,
                                    enum Stream_SessionMessageType,
                                    struct ARDrone_StreamState,
                                    struct ARDrone_SessionData,
                                    ARDrone_SessionData_t,
                                    struct ARDrone_Statistic,
                                    Common_Timer_Manager_t,
                                    ARDrone_Stream_ConnectionConfigurationIterator_t,
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
                                    ARDrone_SessionData_t,
                                    struct ARDrone_Statistic,
                                    Common_Timer_Manager_t,
                                    ARDrone_Stream_ConnectionConfigurationIterator_t,
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
                                    ARDrone_SessionData_t,
                                    struct ARDrone_Statistic,
                                    Common_Timer_Manager_t,
                                    ARDrone_Stream_ConnectionConfigurationIterator_t,
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
                                    ARDrone_SessionData_t,
                                    struct ARDrone_Statistic,
                                    Common_Timer_Manager_t,
                                    ARDrone_Stream_ConnectionConfigurationIterator_t,
                                    ARDrone_ConnectionManager_t,
                                    ARDrone_AsynchUDPConnector_t,
                                    struct ARDrone_UserData> ARDrone_Module_AsynchUDPSource;
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Stream_Statistic_StatisticReport_ReaderTask_T<ACE_MT_SYNCH,
                                                      Common_TimePolicy_t,
                                                      struct ARDrone_DirectShow_ModuleHandlerConfiguration,
                                                      ARDrone_ControlMessage_t,
                                                      ARDrone_Message,
                                                      ARDrone_DirectShow_SessionMessage,
                                                      int,
                                                      struct ARDrone_Statistic,
                                                      Common_Timer_Manager_t,
                                                      ARDrone_DirectShow_SessionData,
                                                      ARDrone_DirectShow_SessionData_t> ARDrone_Module_DirectShow_Statistic_ReaderTask_t;
typedef Stream_Statistic_StatisticReport_WriterTask_T<ACE_MT_SYNCH,
                                                      Common_TimePolicy_t,
                                                      struct ARDrone_DirectShow_ModuleHandlerConfiguration,
                                                      ARDrone_ControlMessage_t,
                                                      ARDrone_Message,
                                                      ARDrone_DirectShow_SessionMessage,
                                                      int,
                                                      struct ARDrone_Statistic,
                                                      Common_Timer_Manager_t,
                                                      ARDrone_DirectShow_SessionData,
                                                      ARDrone_DirectShow_SessionData_t> ARDrone_Module_DirectShow_Statistic_WriterTask_t;

typedef Stream_Statistic_StatisticReport_ReaderTask_T<ACE_MT_SYNCH,
                                                      Common_TimePolicy_t,
                                                      struct ARDrone_MediaFoundation_ModuleHandlerConfiguration,
                                                      ARDrone_ControlMessage_t,
                                                      ARDrone_Message,
                                                      ARDrone_DirectShow_SessionMessage,
                                                      int,
                                                      struct ARDrone_Statistic,
                                                      Common_Timer_Manager_t,
                                                      ARDrone_DirectShow_SessionData,
                                                      ARDrone_DirectShow_SessionData_t> ARDrone_Module_MediaFoundation_Statistic_ReaderTask_t;
typedef Stream_Statistic_StatisticReport_WriterTask_T<ACE_MT_SYNCH,
                                                      Common_TimePolicy_t,
                                                      struct ARDrone_MediaFoundation_ModuleHandlerConfiguration,
                                                      ARDrone_ControlMessage_t,
                                                      ARDrone_Message,
                                                      ARDrone_DirectShow_SessionMessage,
                                                      int,
                                                      struct ARDrone_Statistic,
                                                      Common_Timer_Manager_t,
                                                      ARDrone_DirectShow_SessionData,
                                                      ARDrone_DirectShow_SessionData_t> ARDrone_Module_MediaFoundation_Statistic_WriterTask_t;
#else
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
                                                      ARDrone_SessionData_t> ARDrone_Module_Statistic_ReaderTask_t;
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
                                                      ARDrone_SessionData_t> ARDrone_Module_Statistic_WriterTask_t;
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef ARDrone_Module_ControlDecoder_T<ACE_MT_SYNCH,
                                        Common_TimePolicy_t,
                                        struct ARDrone_DirectShow_ModuleHandlerConfiguration,
                                        ARDrone_ControlMessage_t,
                                        ARDrone_Message,
                                        ARDrone_DirectShow_SessionMessage,
                                        ARDrone_DirectShow_SessionData_t> ARDrone_Module_DirectShow_ControlDecoder;
typedef ARDrone_Module_MAVLinkDecoder_T<ACE_MT_SYNCH,
                                        Common_TimePolicy_t,
                                        struct ARDrone_DirectShow_ModuleHandlerConfiguration,
                                        ARDrone_ControlMessage_t,
                                        ARDrone_Message,
                                        ARDrone_DirectShow_SessionMessage,
                                        ARDrone_DirectShow_SessionData_t> ARDrone_Module_DirectShow_MAVLinkDecoder;
typedef ARDrone_Module_NavDataDecoder_T<ACE_MT_SYNCH,
                                        Common_TimePolicy_t,
                                        struct ARDrone_DirectShow_ModuleHandlerConfiguration,
                                        ARDrone_ControlMessage_t,
                                        ARDrone_Message,
                                        ARDrone_DirectShow_SessionMessage,
                                        ARDrone_DirectShow_SessionData_t> ARDrone_Module_DirectShow_NavDataDecoder;
typedef ARDrone_Module_PaVEDecoder_T<ACE_MT_SYNCH,
                                     Common_TimePolicy_t,
                                     struct ARDrone_DirectShow_ModuleHandlerConfiguration,
                                     ARDrone_ControlMessage_t,
                                     ARDrone_Message,
                                     ARDrone_DirectShow_SessionMessage,
                                     ARDrone_DirectShow_SessionData_t> ARDrone_Module_DirectShow_PaVEDecoder;
typedef Stream_Decoder_LibAVDecoder_T<ACE_MT_SYNCH,
                                      Common_TimePolicy_t,
                                      struct ARDrone_DirectShow_ModuleHandlerConfiguration,
                                      ARDrone_ControlMessage_t,
                                      ARDrone_Message,
                                      ARDrone_DirectShow_SessionMessage,
                                      ARDrone_DirectShow_SessionData_t,
                                      struct _AMMediaType> ARDrone_Module_DirectShow_H264Decoder;

typedef ARDrone_Module_ControlDecoder_T<ACE_MT_SYNCH,
                                        Common_TimePolicy_t,
                                        struct ARDrone_MediaFoundation_ModuleHandlerConfiguration,
                                        ARDrone_ControlMessage_t,
                                        ARDrone_Message,
                                        ARDrone_DirectShow_SessionMessage,
                                        ARDrone_DirectShow_SessionData_t> ARDrone_Module_MediaFoundation_ControlDecoder;
typedef ARDrone_Module_MAVLinkDecoder_T<ACE_MT_SYNCH,
                                        Common_TimePolicy_t,
                                        struct ARDrone_MediaFoundation_ModuleHandlerConfiguration,
                                        ARDrone_ControlMessage_t,
                                        ARDrone_Message,
                                        ARDrone_DirectShow_SessionMessage,
                                        ARDrone_DirectShow_SessionData_t> ARDrone_Module_MediaFoundation_MAVLinkDecoder;
typedef ARDrone_Module_NavDataDecoder_T<ACE_MT_SYNCH,
                                        Common_TimePolicy_t,
                                        struct ARDrone_MediaFoundation_ModuleHandlerConfiguration,
                                        ARDrone_ControlMessage_t,
                                        ARDrone_Message,
                                        ARDrone_DirectShow_SessionMessage,
                                        ARDrone_DirectShow_SessionData_t> ARDrone_Module_MediaFoundation_NavDataDecoder;
typedef ARDrone_Module_PaVEDecoder_T<ACE_MT_SYNCH,
                                     Common_TimePolicy_t,
                                     struct ARDrone_MediaFoundation_ModuleHandlerConfiguration,
                                     ARDrone_ControlMessage_t,
                                     ARDrone_Message,
                                     ARDrone_DirectShow_SessionMessage,
                                     ARDrone_DirectShow_SessionData_t> ARDrone_Module_MediaFoundation_PaVEDecoder;
typedef Stream_Decoder_LibAVDecoder_T<ACE_MT_SYNCH,
                                      Common_TimePolicy_t,
                                      struct ARDrone_MediaFoundation_ModuleHandlerConfiguration,
                                      ARDrone_ControlMessage_t,
                                      ARDrone_Message,
                                      ARDrone_DirectShow_SessionMessage,
                                      ARDrone_DirectShow_SessionData_t,
                                      IMFMediaType*> ARDrone_Module_MediaFoundation_H264Decoder;
#else
typedef ARDrone_Module_ControlDecoder_T<ACE_MT_SYNCH,
                                        Common_TimePolicy_t,
                                        struct ARDrone_ModuleHandlerConfiguration,
                                        ARDrone_ControlMessage_t,
                                        ARDrone_Message,
                                        ARDrone_SessionMessage,
                                        ARDrone_SessionData_t> ARDrone_Module_ControlDecoder;
typedef ARDrone_Module_MAVLinkDecoder_T<ACE_MT_SYNCH,
                                        Common_TimePolicy_t,
                                        struct ARDrone_ModuleHandlerConfiguration,
                                        ARDrone_ControlMessage_t,
                                        ARDrone_Message,
                                        ARDrone_SessionMessage,
                                        ARDrone_SessionData_t> ARDrone_Module_MAVLinkDecoder;
typedef ARDrone_Module_NavDataDecoder_T<ACE_MT_SYNCH,
                                        Common_TimePolicy_t,
                                        struct ARDrone_ModuleHandlerConfiguration,
                                        ARDrone_ControlMessage_t,
                                        ARDrone_Message,
                                        ARDrone_SessionMessage,
                                        ARDrone_SessionData_t> ARDrone_Module_NavDataDecoder;
typedef ARDrone_Module_PaVEDecoder_T<ACE_MT_SYNCH,
                                     Common_TimePolicy_t,
                                     struct ARDrone_ModuleHandlerConfiguration,
                                     ARDrone_ControlMessage_t,
                                     ARDrone_Message,
                                     ARDrone_SessionMessage,
                                     ARDrone_SessionData_t> ARDrone_Module_PaVEDecoder;
typedef Stream_Decoder_LibAVDecoder_T<ACE_MT_SYNCH,
                                      Common_TimePolicy_t,
                                      struct ARDrone_ModuleHandlerConfiguration,
                                      ARDrone_ControlMessage_t,
                                      ARDrone_Message,
                                      ARDrone_SessionMessage,
                                      ARDrone_SessionData_t,
                                      struct Stream_MediaFramework_FFMPEG_MediaType> ARDrone_Module_H264Decoder;
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef ARDrone_Module_Controller_T<ACE_MT_SYNCH,
                                    Common_TimePolicy_t,
                                    struct ARDrone_DirectShow_ModuleHandlerConfiguration,
                                    ARDrone_ControlMessage_t,
                                    ARDrone_Message,
                                    ARDrone_DirectShow_SessionMessage,
                                    ARDrone_DirectShow_SessionData_t,
                                    ARDrone_WLANMonitor_t,
                                    ARDrone_DirectShow_Stream_ConnectionConfigurationIterator_t,
                                    ARDrone_DirectShow_ConnectionManager_t,
                                    ARDrone_DirectShow_UDPConnector_t,
                                    struct ARDrone_UI_CBData_Base> ARDrone_Module_DirectShow_Controller;
typedef ARDrone_Module_Controller_T<ACE_MT_SYNCH,
                                    Common_TimePolicy_t,
                                    struct ARDrone_DirectShow_ModuleHandlerConfiguration,
                                    ARDrone_ControlMessage_t,
                                    ARDrone_Message,
                                    ARDrone_DirectShow_SessionMessage,
                                    ARDrone_DirectShow_SessionData_t,
                                    ARDrone_WLANMonitor_t,
                                    ARDrone_DirectShow_Stream_ConnectionConfigurationIterator_t,
                                    ARDrone_DirectShow_ConnectionManager_t,
                                    ARDrone_DirectShow_AsynchUDPConnector_t,
                                    struct ARDrone_UI_CBData_Base> ARDrone_Module_DirectShow_AsynchController;

typedef ARDrone_Module_Controller_T<ACE_MT_SYNCH,
                                    Common_TimePolicy_t,
                                    struct ARDrone_MediaFoundation_ModuleHandlerConfiguration,
                                    ARDrone_ControlMessage_t,
                                    ARDrone_Message,
                                    ARDrone_DirectShow_SessionMessage,
                                    ARDrone_DirectShow_SessionData_t,
                                    ARDrone_WLANMonitor_t,
                                    ARDrone_MediaFoundation_Stream_ConnectionConfigurationIterator_t,
                                    ARDrone_MediaFoundation_ConnectionManager_t,
                                    ARDrone_MediaFoundation_UDPConnector_t,
                                    struct ARDrone_UI_CBData_Base> ARDrone_Module_MediaFoundation_Controller;
typedef ARDrone_Module_Controller_T<ACE_MT_SYNCH,
                                    Common_TimePolicy_t,
                                    struct ARDrone_MediaFoundation_ModuleHandlerConfiguration,
                                    ARDrone_ControlMessage_t,
                                    ARDrone_Message,
                                    ARDrone_DirectShow_SessionMessage,
                                    ARDrone_DirectShow_SessionData_t,
                                    ARDrone_WLANMonitor_t,
                                    ARDrone_MediaFoundation_Stream_ConnectionConfigurationIterator_t,
                                    ARDrone_MediaFoundation_ConnectionManager_t,
                                    ARDrone_MediaFoundation_AsynchUDPConnector_t,
                                    struct ARDrone_UI_CBData_Base> ARDrone_Module_MediaFoundation_AsynchController;
#else
typedef ARDrone_Module_Controller_T<ACE_MT_SYNCH,
                                    Common_TimePolicy_t,
                                    struct ARDrone_ModuleHandlerConfiguration,
                                    ARDrone_ControlMessage_t,
                                    ARDrone_Message,
                                    ARDrone_SessionMessage,
                                    ARDrone_SessionData_t,
                                    ARDrone_WLANMonitor_t,
                                    ARDrone_Stream_ConnectionConfigurationIterator_t,
                                    ARDrone_ConnectionManager_t,
                                    ARDrone_UDPConnector_t,
                                    struct ARDrone_UI_CBData_Base> ARDrone_Module_Controller;
typedef ARDrone_Module_Controller_T<ACE_MT_SYNCH,
                                    Common_TimePolicy_t,
                                    struct ARDrone_ModuleHandlerConfiguration,
                                    ARDrone_ControlMessage_t,
                                    ARDrone_Message,
                                    ARDrone_SessionMessage,
                                    ARDrone_SessionData_t,
                                    ARDrone_WLANMonitor_t,
                                    ARDrone_Stream_ConnectionConfigurationIterator_t,
                                    ARDrone_ConnectionManager_t,
                                    ARDrone_AsynchUDPConnector_t,
                                    struct ARDrone_UI_CBData_Base> ARDrone_Module_AsynchController;
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Stream_Vis_Target_Direct3D_T<ACE_MT_SYNCH,
                                     Common_TimePolicy_t,
                                     struct ARDrone_DirectShow_ModuleHandlerConfiguration,
                                     ARDrone_ControlMessage_t,
                                     ARDrone_Message,
                                     ARDrone_DirectShow_SessionMessage,
                                     ARDrone_DirectShow_SessionData,
                                     ARDrone_DirectShow_SessionData_t,
                                     struct _AMMediaType> ARDrone_Module_DirectShow_Display;
typedef Stream_Vis_Target_Direct3D_T<ACE_MT_SYNCH,
                                     Common_TimePolicy_t,
                                     struct ARDrone_MediaFoundation_ModuleHandlerConfiguration,
                                     ARDrone_ControlMessage_t,
                                     ARDrone_Message,
                                     ARDrone_DirectShow_SessionMessage,
                                     ARDrone_DirectShow_SessionData,
                                     ARDrone_DirectShow_SessionData_t,
                                     IMFMediaType*> ARDrone_Module_MediaFoundation_Display;
//typedef Stream_MediaFramework_DirectShow_Source_Filter_T<Common_TimePolicy_t,
//                                                         ARDrone_SessionMessage,
//                                                         ARDrone_Message,
//                                                         struct ARDrone_DirectShow_FilterConfiguration,
//                                                         struct Stream_MediaFramework_DirectShow_FilterPinConfiguration,
//                                                         struct _AMMediaType> ARDrone_DirectShowFilter_t;
//typedef Stream_MediaFramework_DirectShow_Asynch_Source_Filter_T<Common_TimePolicy_t,
//                                                                ARDrone_SessionMessage,
//                                                                ARDrone_Message,
//                                                                struct ARDrone_DirectShow_FilterConfiguration,
//                                                                struct Stream_MediaFramework_DirectShow_FilterPinConfiguration,
//                                                                struct _AMMediaType> ARDrone_AsynchDirectShowFilter_t;
//typedef Stream_Vis_Target_DirectShow_T<ACE_MT_SYNCH,
//                                       Common_TimePolicy_t,
//                                       struct ARDrone_DirectShow_ModuleHandlerConfiguration,
//                                       ARDrone_ControlMessage_t,
//                                       ARDrone_Message,
//                                       ARDrone_SessionMessage,
//                                       ARDrone_SessionData_t,
//                                       struct ARDrone_SessionData,
//                                       struct ARDrone_DirectShow_FilterConfiguration,
//                                       struct ARDrone_DirectShow_PinConfiguration,
//                                       ARDrone_DirectShowFilter_t> ARDrone_Module_DirectShow_Display;
//typedef Stream_Vis_Target_MediaFoundation_T<ACE_MT_SYNCH,
//                                            Common_TimePolicy_t,
//                                            struct ARDrone_MediaFoundation_ModuleHandlerConfiguration,
//                                            ARDrone_ControlMessage_t,
//                                            ARDrone_Message,
//                                            ARDrone_SessionMessage,
//                                            struct ARDrone_SessionData,
//                                            ARDrone_SessionData_t,
//                                            struct ARDrone_UserData> ARDrone_Module_MediaFoundation_Display;
#else
typedef Stream_Module_Vis_GTK_Pixbuf_T<ACE_MT_SYNCH,
                                       Common_TimePolicy_t,
                                       struct ARDrone_ModuleHandlerConfiguration,
                                       ARDrone_ControlMessage_t,
                                       ARDrone_Message,
                                       ARDrone_SessionMessage,
                                       ARDrone_SessionData_t,
                                       struct Stream_MediaFramework_FFMPEG_MediaType> ARDrone_Module_Display;
#endif // ACE_WIN32 || ACE_WIN64

#if defined (_DEBUG)
#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Stream_Module_Dump_T<ACE_MT_SYNCH,
                             Common_TimePolicy_t,
                             struct ARDrone_DirectShow_ModuleHandlerConfiguration,
                             ARDrone_ControlMessage_t,
                             ARDrone_Message,
                             ARDrone_DirectShow_SessionMessage,
                             ARDrone_DirectShow_SessionData_t,
                             struct ARDrone_UserData> ARDrone_Module_DirectShow_Dump;

typedef Stream_Module_Dump_T<ACE_MT_SYNCH,
                             Common_TimePolicy_t,
                             struct ARDrone_MediaFoundation_ModuleHandlerConfiguration,
                             ARDrone_ControlMessage_t,
                             ARDrone_Message,
                             ARDrone_DirectShow_SessionMessage,
                             ARDrone_DirectShow_SessionData_t,
                             struct ARDrone_UserData> ARDrone_Module_MediaFoundation_Dump;
#else
typedef Stream_Module_Dump_T<ACE_MT_SYNCH,
                             Common_TimePolicy_t,
                             struct ARDrone_ModuleHandlerConfiguration,
                             ARDrone_ControlMessage_t,
                             ARDrone_Message,
                             ARDrone_SessionMessage,
<<<<<<< HEAD
=======
                             ARDrone_SessionData_t,
>>>>>>> 08a0ebaebe1264dab92c7d8bd230639371d2d643
                             struct ARDrone_UserData> ARDrone_Module_Dump;
#endif // ACE_WIN32 || ACE_WIN64
#endif // _DEBUG

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Stream_Module_FileWriter_T<ACE_MT_SYNCH,
                                   Common_TimePolicy_t,
                                   struct ARDrone_DirectShow_ModuleHandlerConfiguration,
                                   ARDrone_ControlMessage_t,
                                   ARDrone_Message,
                                   ARDrone_DirectShow_SessionMessage,
                                   ARDrone_DirectShow_SessionData> ARDrone_Module_DirectShow_FileWriter;

typedef Stream_Module_FileWriter_T<ACE_MT_SYNCH,
                                   Common_TimePolicy_t,
                                   struct ARDrone_MediaFoundation_ModuleHandlerConfiguration,
                                   ARDrone_ControlMessage_t,
                                   ARDrone_Message,
                                   ARDrone_DirectShow_SessionMessage,
                                   ARDrone_DirectShow_SessionData> ARDrone_Module_MediaFoundation_FileWriter;
#else
typedef Stream_Module_FileWriter_T<ACE_MT_SYNCH,
                                   Common_TimePolicy_t,
                                   struct ARDrone_ModuleHandlerConfiguration,
                                   ARDrone_ControlMessage_t,
                                   ARDrone_Message,
<<<<<<< HEAD
                                   ARDrone_SessionMessage> ARDrone_Module_FileWriter;
=======
                                   ARDrone_SessionMessage,
                                   struct ARDrone_SessionData> ARDrone_Module_FileWriter;
>>>>>>> 08a0ebaebe1264dab92c7d8bd230639371d2d643
#endif // ACE_WIN32 || ACE_WIN64

#if defined (ACE_WIN32) || defined (ACE_WIN64)
typedef Stream_Decoder_AVIEncoder_ReaderTask_T<ACE_MT_SYNCH,
                                               Common_TimePolicy_t,
                                               struct ARDrone_DirectShow_ModuleHandlerConfiguration,
                                               ARDrone_ControlMessage_t,
                                               ARDrone_Message,
                                               ARDrone_DirectShow_SessionMessage,
                                               ARDrone_DirectShow_SessionData_t,
                                               ARDrone_DirectShow_SessionData,
                                               struct _AMMediaType,
                                               struct ARDrone_UserData> ARDrone_Module_DirectShow_AVIEncoder_ReaderTask_t;
typedef Stream_Decoder_AVIEncoder_WriterTask_T<ACE_MT_SYNCH,
                                               Common_TimePolicy_t,
                                               struct ARDrone_DirectShow_ModuleHandlerConfiguration,
                                               ARDrone_ControlMessage_t,
                                               ARDrone_Message,
                                               ARDrone_DirectShow_SessionMessage,
                                               ARDrone_DirectShow_SessionData_t,
                                               ARDrone_DirectShow_SessionData,
                                               struct _AMMediaType,
                                               struct ARDrone_UserData> ARDrone_Module_DirectShow_AVIEncoder_WriterTask_t;

typedef Stream_Decoder_AVIEncoder_ReaderTask_T<ACE_MT_SYNCH,
                                               Common_TimePolicy_t,
                                               struct ARDrone_MediaFoundation_ModuleHandlerConfiguration,
                                               ARDrone_ControlMessage_t,
                                               ARDrone_Message,
                                               ARDrone_DirectShow_SessionMessage,
                                               ARDrone_DirectShow_SessionData_t,
                                               ARDrone_DirectShow_SessionData,
                                               IMFMediaType*,
                                               struct ARDrone_UserData> ARDrone_Module_MediaFoundation_AVIEncoder_ReaderTask_t;
typedef Stream_Decoder_AVIEncoder_WriterTask_T<ACE_MT_SYNCH,
                                               Common_TimePolicy_t,
                                               struct ARDrone_MediaFoundation_ModuleHandlerConfiguration,
                                               ARDrone_ControlMessage_t,
                                               ARDrone_Message,
                                               ARDrone_DirectShow_SessionMessage,
                                               ARDrone_DirectShow_SessionData_t,
                                               ARDrone_DirectShow_SessionData,
                                               IMFMediaType*,
                                               struct ARDrone_UserData> ARDrone_Module_MediaFoundation_AVIEncoder_WriterTask_t;
#else
typedef Stream_Decoder_AVIEncoder_ReaderTask_T<ACE_MT_SYNCH,
                                               Common_TimePolicy_t,
                                               struct ARDrone_ModuleHandlerConfiguration,
                                               ARDrone_ControlMessage_t,
                                               ARDrone_Message,
                                               ARDrone_SessionMessage,
                                               ARDrone_SessionData_t,
                                               struct ARDrone_SessionData,
                                               struct Stream_MediaFramework_FFMPEG_MediaType,
                                               struct ARDrone_UserData> ARDrone_Module_AVIEncoder_ReaderTask_t;
typedef Stream_Decoder_AVIEncoder_WriterTask_T<ACE_MT_SYNCH,
                                               Common_TimePolicy_t,
                                               struct ARDrone_ModuleHandlerConfiguration,
                                               ARDrone_ControlMessage_t,
                                               ARDrone_Message,
                                               ARDrone_SessionMessage,
                                               ARDrone_SessionData_t,
                                               struct ARDrone_SessionData,
                                               struct Stream_MediaFramework_FFMPEG_MediaType,
                                               struct ARDrone_UserData> ARDrone_Module_AVIEncoder_WriterTask_t;
#endif // ACE_WIN32 || ACE_WIN64

//////////////////////////////////////////

#if defined (ACE_WIN32) || defined (ACE_WIN64)
// directshow
DATASTREAM_MODULE_INPUT_ONLY (ARDrone_DirectShow_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_DirectShow_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_net_source_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_DirectShow_TCPSource);      // writer type
DATASTREAM_MODULE_INPUT_ONLY (ARDrone_DirectShow_SessionData,                 // session data type
                              enum Stream_SessionMessageType,             // session event type
                              struct ARDrone_DirectShow_ModuleHandlerConfiguration,  // module handler configuration type
                              libacestream_default_net_source_module_name_string,
                              Stream_INotify_t,                           // stream notification interface type
                              ARDrone_Module_DirectShow_AsynchTCPSource); // writer type
DATASTREAM_MODULE_INPUT_ONLY (ARDrone_DirectShow_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_DirectShow_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_net_source_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_DirectShow_UDPSource);      // writer type
DATASTREAM_MODULE_INPUT_ONLY (ARDrone_DirectShow_SessionData,                 // session data type
                              enum Stream_SessionMessageType,             // session event type
                              struct ARDrone_DirectShow_ModuleHandlerConfiguration,  // module handler configuration type
                              libacestream_default_net_source_module_name_string,
                              Stream_INotify_t,                           // stream notification interface type
                              ARDrone_Module_DirectShow_AsynchUDPSource); // writer type

DATASTREAM_MODULE_DUPLEX (ARDrone_DirectShow_SessionData,                       // session data type
                          enum Stream_SessionMessageType,                   // session event type
                          struct ARDrone_DirectShow_ModuleHandlerConfiguration,        // module handler configuration type
                          libacestream_default_stat_report_module_name_string,
                          Stream_INotify_t,                                 // stream notification interface type
                          ARDrone_Module_DirectShow_Statistic_ReaderTask_t, // reader type
                          ARDrone_Module_DirectShow_Statistic_WriterTask_t, // writer type
                          ARDrone_Module_DirectShow_StatisticReport);       // name

DATASTREAM_MODULE_INPUT_ONLY (ARDrone_DirectShow_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_DirectShow_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_misc_parser_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_DirectShow_ControlDecoder); // writer type
DATASTREAM_MODULE_INPUT_ONLY (ARDrone_DirectShow_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_DirectShow_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_misc_parser_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_DirectShow_MAVLinkDecoder); // writer type
DATASTREAM_MODULE_INPUT_ONLY (ARDrone_DirectShow_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_DirectShow_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_misc_parser_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_DirectShow_NavDataDecoder); // writer type
DATASTREAM_MODULE_INPUT_ONLY (ARDrone_DirectShow_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_DirectShow_ModuleHandlerConfiguration, // module handler configuration type
                              ardrone_default_pave_decoder_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_DirectShow_PaVEDecoder);    // writer type
DATASTREAM_MODULE_INPUT_ONLY (ARDrone_DirectShow_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_DirectShow_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_dec_libav_decoder_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_DirectShow_H264Decoder);    // writer type

DATASTREAM_MODULE_INPUT_ONLY (ARDrone_DirectShow_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_DirectShow_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_net_target_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_DirectShow_Controller);     // writer type
DATASTREAM_MODULE_INPUT_ONLY (ARDrone_DirectShow_SessionData,                  // session data type
                              enum Stream_SessionMessageType,              // session event type
                              struct ARDrone_DirectShow_ModuleHandlerConfiguration,   // module handler configuration type
                              libacestream_default_net_target_module_name_string,
                              Stream_INotify_t,                            // stream notification interface type
                              ARDrone_Module_DirectShow_AsynchController); // writer type

DATASTREAM_MODULE_INPUT_ONLY (ARDrone_DirectShow_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_DirectShow_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_vis_directshow_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_DirectShow_Display);        // writer type

#if defined (_DEBUG)
DATASTREAM_MODULE_INPUT_ONLY (ARDrone_DirectShow_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_DirectShow_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_misc_dump_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_DirectShow_Dump);           // writer type
#endif

DATASTREAM_MODULE_INPUT_ONLY (ARDrone_DirectShow_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_DirectShow_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_file_sink_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_DirectShow_FileWriter);     // writer type

DATASTREAM_MODULE_DUPLEX (ARDrone_DirectShow_SessionData,                        // session data type
                          enum Stream_SessionMessageType,                    // session event type
                          struct ARDrone_DirectShow_ModuleHandlerConfiguration, // module handler configuration type
                          libacestream_default_dec_avi_encoder_module_name_string,
                          Stream_INotify_t,                                  // stream notification interface type
                          ARDrone_Module_DirectShow_AVIEncoder_ReaderTask_t, // reader type
                          ARDrone_Module_DirectShow_AVIEncoder_WriterTask_t, // writer type
                          ARDrone_Module_DirectShow_AVIEncoder);             // name

// mediafoundation
DATASTREAM_MODULE_INPUT_ONLY (ARDrone_DirectShow_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_MediaFoundation_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_net_source_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_MediaFoundation_TCPSource);      // writer type
DATASTREAM_MODULE_INPUT_ONLY (ARDrone_DirectShow_SessionData,                 // session data type
                              enum Stream_SessionMessageType,             // session event type
                              struct ARDrone_MediaFoundation_ModuleHandlerConfiguration,  // module handler configuration type
                              libacestream_default_net_source_module_name_string,
                              Stream_INotify_t,                           // stream notification interface type
                              ARDrone_Module_MediaFoundation_AsynchTCPSource); // writer type
DATASTREAM_MODULE_INPUT_ONLY (ARDrone_DirectShow_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_MediaFoundation_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_net_source_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_MediaFoundation_UDPSource);      // writer type
DATASTREAM_MODULE_INPUT_ONLY (ARDrone_DirectShow_SessionData,                 // session data type
                              enum Stream_SessionMessageType,             // session event type
                              struct ARDrone_MediaFoundation_ModuleHandlerConfiguration,  // module handler configuration type
                              libacestream_default_net_source_module_name_string,
                              Stream_INotify_t,                           // stream notification interface type
                              ARDrone_Module_MediaFoundation_AsynchUDPSource); // writer type

DATASTREAM_MODULE_DUPLEX (ARDrone_DirectShow_SessionData,                       // session data type
                          enum Stream_SessionMessageType,                   // session event type
                          struct ARDrone_MediaFoundation_ModuleHandlerConfiguration,        // module handler configuration type
                          libacestream_default_stat_report_module_name_string,
                          Stream_INotify_t,                                 // stream notification interface type
                          ARDrone_Module_MediaFoundation_Statistic_ReaderTask_t, // reader type
                          ARDrone_Module_MediaFoundation_Statistic_WriterTask_t, // writer type
                          ARDrone_Module_MediaFoundation_StatisticReport);       // name

DATASTREAM_MODULE_INPUT_ONLY (ARDrone_DirectShow_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_MediaFoundation_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_misc_parser_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_MediaFoundation_ControlDecoder); // writer type
DATASTREAM_MODULE_INPUT_ONLY (ARDrone_DirectShow_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_MediaFoundation_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_misc_parser_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_MediaFoundation_MAVLinkDecoder); // writer type
DATASTREAM_MODULE_INPUT_ONLY (ARDrone_DirectShow_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_MediaFoundation_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_misc_parser_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_MediaFoundation_NavDataDecoder); // writer type
DATASTREAM_MODULE_INPUT_ONLY (ARDrone_DirectShow_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_MediaFoundation_ModuleHandlerConfiguration, // module handler configuration type
                              ardrone_default_pave_decoder_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_MediaFoundation_PaVEDecoder);    // writer type
DATASTREAM_MODULE_INPUT_ONLY (ARDrone_DirectShow_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_MediaFoundation_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_dec_libav_decoder_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_MediaFoundation_H264Decoder);    // writer type

DATASTREAM_MODULE_INPUT_ONLY (ARDrone_DirectShow_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_MediaFoundation_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_net_target_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_MediaFoundation_Controller);     // writer type
DATASTREAM_MODULE_INPUT_ONLY (ARDrone_DirectShow_SessionData,                  // session data type
                              enum Stream_SessionMessageType,              // session event type
                              struct ARDrone_MediaFoundation_ModuleHandlerConfiguration,   // module handler configuration type
                              libacestream_default_net_target_module_name_string,
                              Stream_INotify_t,                            // stream notification interface type
                              ARDrone_Module_MediaFoundation_AsynchController); // writer type

DATASTREAM_MODULE_INPUT_ONLY (ARDrone_DirectShow_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_MediaFoundation_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_vis_mediafoundation_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_MediaFoundation_Display);   // writer type

#if defined (_DEBUG)
DATASTREAM_MODULE_INPUT_ONLY (ARDrone_DirectShow_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_MediaFoundation_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_misc_dump_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_MediaFoundation_Dump);      // writer type
#endif

DATASTREAM_MODULE_INPUT_ONLY (ARDrone_DirectShow_SessionData,                 // session data type
                              enum Stream_SessionMessageType,             // session event type
                              struct ARDrone_DirectShow_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_file_sink_module_name_string,
                              Stream_INotify_t,                           // stream notification interface type
                              ARDrone_Module_MediaFoundation_FileWriter); // writer type

DATASTREAM_MODULE_DUPLEX (ARDrone_DirectShow_SessionData,                             // session data type
                          enum Stream_SessionMessageType,                         // session event type
                          struct ARDrone_DirectShow_ModuleHandlerConfiguration,   // module handler configuration type
                          libacestream_default_dec_avi_encoder_module_name_string,
                          Stream_INotify_t,                                       // stream notification interface type
                          ARDrone_Module_MediaFoundation_AVIEncoder_ReaderTask_t, // reader type
                          ARDrone_Module_MediaFoundation_AVIEncoder_WriterTask_t, // writer type
                          ARDrone_Module_MediaFoundation_AVIEncoder);             // name
#else
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
                              libacestream_default_dec_libav_decoder_module_name_string,
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

DATASTREAM_MODULE_INPUT_ONLY (struct ARDrone_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_vis_gtk_pixbuf_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_Display);                   // writer type

#if defined (_DEBUG)
DATASTREAM_MODULE_INPUT_ONLY (struct ARDrone_SessionData,                // session data type
                              enum Stream_SessionMessageType,            // session event type
                              struct ARDrone_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_misc_dump_module_name_string,
                              Stream_INotify_t,                          // stream notification interface type
                              ARDrone_Module_Dump);                      // writer type
#endif // _DEBUG

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
#endif // ACE_WIN32 || ACE_WIN64

#endif
