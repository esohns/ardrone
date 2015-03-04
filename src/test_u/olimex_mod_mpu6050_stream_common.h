/*  I2C kernel module driver for the Olimex MOD-MPU6050 UEXT module
    (see https://www.olimex.com/Products/Modules/Sensors/MOD-MPU6050/open-source-hardware,
         http://www.invensense.com/mems/gyro/mpu6050.html)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef OLIMEX_MOD_MPU6050_STREAM_COMMON_H
#define OLIMEX_MOD_MPU6050_STREAM_COMMON_H

#include "ace/Synch_Traits.h"

#include "common.h"

#include "stream_common.h"
#include "stream_messageallocatorheap_base.h"
//#include "stream_session_data_base.h"
#include "stream_streammodule_base.h"

#include "net_module_runtimestatistic.h"
#include "net_module_sockethandler.h"
#include "net_stream_common.h"

#include "olimex_mod_mpu6050_message.h"
#include "olimex_mod_mpu6050_sessionmessage.h"

typedef Stream_MessageAllocatorHeapBase_T<Olimex_Mod_MPU6050_Message,
                                          Olimex_Mod_MPU6050_SessionMessage> Olimex_Mod_MPU6050_MessageAllocator_t;

//struct Olimex_Mod_MPU6050_SessionData_t
//{
//  Stream_State_t streamState;
//};
//
//typedef Stream_SessionDataBase_T<Olimex_Mod_MPU6050_SessionData_t> Olimex_Mod_MPU6050_StreamSessionData_t;

typedef Net_Module_UDPSocketHandler_T<Stream_State_t,
                                      Net_SessionData_t,
                                      Net_StreamSessionData_t,
                                      Olimex_Mod_MPU6050_SessionMessage,
                                      Olimex_Mod_MPU6050_Message> Olimex_Mod_MPU6050_Module_SocketHandler;
DATASTREAM_MODULE_INPUT_ONLY (ACE_MT_SYNCH,                             // task synch type
                              Common_TimePolicy_t,                      // time policy type
                              Olimex_Mod_MPU6050_Module_SocketHandler); // writer type

typedef Net_Module_Statistic_ReaderTask_T<ACE_MT_SYNCH,
                                          Common_TimePolicy_t,
                                          Olimex_Mod_MPU6050_SessionMessage,
                                          Olimex_Mod_MPU6050_Message,
                                          Olimex_Mod_MPU6050_MessageType_t,
                                          Stream_Statistic_t> Olimex_Mod_MPU6050_Module_Statistic_ReaderTask_t;
typedef Net_Module_Statistic_WriterTask_T<ACE_MT_SYNCH,
                                          Common_TimePolicy_t,
                                          Olimex_Mod_MPU6050_SessionMessage,
                                          Olimex_Mod_MPU6050_Message,
                                          Olimex_Mod_MPU6050_MessageType_t,
                                          Stream_Statistic_t> Olimex_Mod_MPU6050_Module_Statistic_WriterTask_t;
DATASTREAM_MODULE_DUPLEX (ACE_MT_SYNCH,                                     // task synch type
                          Common_TimePolicy_t,                              // time policy type
                          Olimex_Mod_MPU6050_Module_Statistic_ReaderTask_t, // reader type
                          Olimex_Mod_MPU6050_Module_Statistic_WriterTask_t, // writer type
                          Olimex_Mod_MPU6050_Module_RuntimeStatistic);      // name

#endif // #ifndef OLIMEX_MOD_MPU6050_STREAM_COMMON_H
