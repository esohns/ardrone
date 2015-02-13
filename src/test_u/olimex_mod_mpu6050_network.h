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

#ifndef OLIMEX_MOD_MPU6050_NETWORK_H
#define OLIMEX_MOD_MPU6050_NETWORK_H

#include "ace/Singleton.h"
#include "ace/Synch.h"

#include "net_connectionmanager.h"

#include "olimex_mod_mpu6050_types.h"

typedef Net_Connection_Manager_T<Olimex_Mod_MPU6050_StreamProtocolConfigurationState_t,
                                 Olimex_Mod_MPU6050_StreamStatistic_t> Connection_Manager_t;
typedef ACE_Singleton<Connection_Manager_t,
                      ACE_Recursive_Thread_Mutex> CONNECTIONMANAGER_SINGLETON;

#endif // #ifndef OLIMEX_MOD_MPU6050_NETWORK_H
