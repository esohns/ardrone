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

#ifndef OLIMEX_MOD_MPU6050_MODULES_COMMON_H
#define OLIMEX_MOD_MPU6050_MODULES_COMMON_H

#include "ace/Synch_Traits.h"

#include "common_time_common.h"

#include "stream_common.h"
#include "stream_streammodule_base.h"

#include "stream_module_runtimestatistic.h"
#include "stream_module_source.h"

#include "olimex_mod_mpu6050_message.h"
#include "olimex_mod_mpu6050_network.h"
#include "olimex_mod_mpu6050_sessionmessage.h"
#include "olimex_mod_mpu6050_stream_common.h"
#include "olimex_mod_mpu6050_types.h"

typedef Stream_Module_Net_Source_T<Olimex_Mod_MPU6050_SessionMessage,
                                   Olimex_Mod_MPU6050_Message,
                                   //////
                                   Olimex_Mod_MPU6050_ModuleHandlerConfiguration,
                                   //////
                                   Olimex_Mod_MPU6050_StreamState,
                                   //////
                                   Olimex_Mod_MPU6050_SessionData,
                                   Olimex_Mod_MPU6050_StreamSessionData_t,
                                   //////
                                   Olimex_Mod_MPU6050_RuntimeStatistic_t,
                                   //////
                                   Olimex_Mod_MPU6050_ConnectionManager_t,
                                   Olimex_Mod_MPU6050_Connector_t> Olimex_Mod_MPU6050_Module_SocketHandler;
DATASTREAM_MODULE_INPUT_ONLY (ACE_MT_SYNCH,                                  // task synch type
                              Common_TimePolicy_t,                           // time policy type
                              Stream_ModuleConfiguration,                    // module configuration type
                              Olimex_Mod_MPU6050_ModuleHandlerConfiguration, // module handler configuration type
                              Olimex_Mod_MPU6050_Module_SocketHandler);      // writer type

typedef Stream_Module_Statistic_ReaderTask_T<ACE_MT_SYNCH,
                                             Common_TimePolicy_t,
                                             Olimex_Mod_MPU6050_SessionMessage,
                                             Olimex_Mod_MPU6050_Message,
                                             Olimex_Mod_MPU6050_MessageType,
                                             Olimex_Mod_MPU6050_RuntimeStatistic_t> Olimex_Mod_MPU6050_Module_Statistic_ReaderTask_t;
typedef Stream_Module_Statistic_WriterTask_T<ACE_MT_SYNCH,
                                             Common_TimePolicy_t,
                                             Olimex_Mod_MPU6050_SessionMessage,
                                             Olimex_Mod_MPU6050_Message,
                                             Olimex_Mod_MPU6050_MessageType,
                                             Olimex_Mod_MPU6050_RuntimeStatistic_t> Olimex_Mod_MPU6050_Module_Statistic_WriterTask_t;
DATASTREAM_MODULE_DUPLEX (ACE_MT_SYNCH,                                     // task synch type
                          Common_TimePolicy_t,                              // time policy type
                          Stream_ModuleConfiguration,                       // module configuration type
                          Olimex_Mod_MPU6050_ModuleHandlerConfiguration,    // module handler configuration type
                          Olimex_Mod_MPU6050_Module_Statistic_ReaderTask_t, // reader type
                          Olimex_Mod_MPU6050_Module_Statistic_WriterTask_t, // writer type
                          Olimex_Mod_MPU6050_Module_RuntimeStatistic);      // name

#endif
