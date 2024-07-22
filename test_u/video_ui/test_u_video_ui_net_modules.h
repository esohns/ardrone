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

#ifndef TEST_U_VIDEO_UI_NET_MODULES_H
#define TEST_U_VIDEO_UI_NET_MODULES_H

#include "ace/Synch_Traits.h"

#include "common_time_common.h"
#include "common_timer_manager_common.h"

#include "stream_common.h"
#include "stream_streammodule_base.h"

#include "stream_net_source.h"

#include "test_u_video_ui_message.h"
#include "test_u_video_ui_session_message.h"
#include "test_u_video_ui_network.h"
#include "test_u_video_ui_network_common.h"
#include "test_u_video_ui_stream_common.h"

// declare module(s)
typedef Stream_Module_Net_SourceH_T<ACE_MT_SYNCH,
                                    Stream_ControlMessage_t,
                                    Test_U_Message_t,
                                    Test_U_SessionMessage_t,
                                    struct Test_U_ModuleHandlerConfiguration,
                                    enum Stream_ControlType,
                                    enum Stream_SessionMessageType,
                                    struct Test_U_StreamState,
                                    struct Stream_Statistic,
                                    Common_Timer_Manager_t,
                                    Test_U_TCPConnector_t,
                                    struct Stream_UserData> Test_U_TCPSource;
typedef Stream_Module_Net_SourceH_T<ACE_MT_SYNCH,
                                    Stream_ControlMessage_t,
                                    Test_U_Message_t,
                                    Test_U_SessionMessage_t,
                                    struct Test_U_ModuleHandlerConfiguration,
                                    enum Stream_ControlType,
                                    enum Stream_SessionMessageType,
                                    struct Test_U_StreamState,
                                    struct Stream_Statistic,
                                    Common_Timer_Manager_t,
                                    Test_U_AsynchTCPConnector_t,
                                    struct Stream_UserData> Test_U_AsynchTCPSource;
//typedef Stream_Module_Net_SourceH_T<ACE_MT_SYNCH,
//                                    Stream_ControlMessage_t,
//                                    Test_U_Message_t,
//                                    Test_U_SessionMessage_t,
//                                    struct Test_U_ModuleHandlerConfiguration,
//                                    enum Stream_ControlType,
//                                    enum Stream_SessionMessageType,
//                                    struct Test_U_StreamState,
//                                    Test_U_SessionData,
//                                    Test_U_SessionData_t,
//                                    struct Stream_Statistic,
//                                    Common_Timer_Manager_t,
//                                    Test_U_Stream_ConnectionConfigurationIterator_t,
//                                    Test_U_ConnectionManager_t,
//                                    Test_U_UDPConnector_t,
//                                    struct Net_UserData> Test_U_UDPSource;
//typedef Stream_Module_Net_SourceH_T<ACE_MT_SYNCH,
//                                    Stream_ControlMessage_t,
//                                    Test_U_Message_t,
//                                    Test_U_SessionMessage_t,
//                                    struct Test_U_ModuleHandlerConfiguration,
//                                    enum Stream_ControlType,
//                                    enum Stream_SessionMessageType,
//                                    struct Test_U_StreamState,
//                                    Test_U_SessionData,
//                                    Test_U_SessionData_t,
//                                    struct Stream_Statistic,
//                                    Common_Timer_Manager_t,
//                                    Test_U_UDPConnectionManager_t,
//                                    Test_U_AsynchUDPConnector_t,
//                                    struct Net_UserData> Test_U_AsynchUDPSource;

//////////////////////////////////////////

DATASTREAM_MODULE_INPUT_ONLY (Test_U_SessionData,                               // session data type
                              enum Stream_SessionMessageType,                   // session event type
                              struct Test_U_ModuleHandlerConfiguration,         // module handler configuration type
                              libacestream_default_net_source_module_name_string,
                              Stream_INotify_t,                                 // stream notification interface type
                              Test_U_AsynchTCPSource);                          // writer type
DATASTREAM_MODULE_INPUT_ONLY (Test_U_SessionData,                               // session data type
                              enum Stream_SessionMessageType,                   // session event type
                              struct Test_U_ModuleHandlerConfiguration,         // module handler configuration type
                              libacestream_default_net_source_module_name_string,
                              Stream_INotify_t,                                 // stream notification interface type
                              Test_U_TCPSource);                                // writer type

//DATASTREAM_MODULE_INPUT_ONLY (Test_U_SessionData,                               // session data type
//                              enum Stream_SessionMessageType,                   // session event type
//                              struct Test_U_ModuleHandlerConfiguration,         // module handler configuration type
//                              libacestream_default_net_source_module_name_string,
//                              Stream_INotify_t,                                 // stream notification interface type
//                              Test_U_AsynchUDPSource);                          // writer type

#endif
