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

#ifndef TEST_U_COMMON_MODULES_H
#define TEST_U_COMMON_MODULES_H

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

//#include "ardrone_module_controller.h"
#include "ardrone_module_mavlink_decoder.h"

#include "test_u_message.h"
//#include "test_u_session_message.h"
#include "test_u_stream_common.h"

// declare module(s)
typedef ARDrone_Module_MAVLinkDecoder_T <ACE_MT_SYNCH,
                                         Common_TimePolicy_t,
                                         struct Test_U_ModuleHandlerConfiguration,
                                         Stream_ControlMessage_t,
                                         Test_U_Message_t,
                                         Test_U_SessionMessage_t> Test_U_MAVLinkDecoder;

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

typedef Stream_Module_MessageHandler_T<ACE_MT_SYNCH,
                                       Common_TimePolicy_t,
                                       struct Test_U_ModuleHandlerConfiguration,
                                       Stream_ControlMessage_t,
                                       Test_U_Message_t,
                                       Test_U_SessionMessage_t,
                                       Stream_SessionId_t,
                                       Test_U_SessionData,
                                       struct Stream_UserData> Test_U_MessageHandler;

//////////////////////////////////////////

DATASTREAM_MODULE_INPUT_ONLY (Test_U_SessionData,                   // session data type
                              enum Stream_SessionMessageType,                   // session event type
                              struct Test_U_ModuleHandlerConfiguration, // module handler configuration type
                              ardrone_default_mavlink_decoder_module_name_string,
                              Stream_INotify_t,                                 // stream notification interface type
                              Test_U_MAVLinkDecoder);                       // writer type

DATASTREAM_MODULE_DUPLEX (Test_U_SessionData,                // session data type
                          enum Stream_SessionMessageType,                   // session event type
                          struct Test_U_ModuleHandlerConfiguration, // module handler configuration type
                          libacestream_default_stat_report_module_name_string,
                          Stream_INotify_t,                                 // stream notification interface type
                          Test_U_Statistic_ReaderTask_t,            // reader type
                          Test_U_Statistic_WriterTask_t,            // writer type
                          Test_U_StatisticReport);                  // name

DATASTREAM_MODULE_INPUT_ONLY (Test_U_SessionData,                           // session data type
                              enum Stream_SessionMessageType,                       // session event type
                              struct Test_U_ModuleHandlerConfiguration, // module handler configuration type
                              libacestream_default_misc_messagehandler_module_name_string,
                              Stream_INotify_t,                                     // stream notification interface type
                              Test_U_MessageHandler);                       // writer type

#endif
