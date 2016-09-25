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

#include "stream_session_data.h"

#include "stream_common.h"
#include "stream_messageallocatorheap_base.h"

#include "olimex_mod_mpu6050_types.h"

// forward declarations
class Olimex_Mod_MPU6050_Message;
class Olimex_Mod_MPU6050_SessionMessage;

//typedef Stream_MessageAllocatorHeapBase_T<Stream_AllocatorConfiguration,
//                                          Olimex_Mod_MPU6050_ControlMessage_t,
//                                          Olimex_Mod_MPU6050_Message,
//                                          Olimex_Mod_MPU6050_SessionMessage> Olimex_Mod_MPU6050_MessageAllocator_t;

struct Olimex_Mod_MPU6050_UserData;
struct Olimex_Mod_MPU6050_SessionData;
struct Olimex_Mod_MPU6050_StreamState
 : Stream_State
{
  inline Olimex_Mod_MPU6050_StreamState ()
   : currentSessionData (NULL)
   , userData (NULL)
  {};

  Olimex_Mod_MPU6050_SessionData* currentSessionData;
  Olimex_Mod_MPU6050_UserData*    userData;
};

struct Olimex_Mod_MPU6050_ConnectionState;
typedef Stream_Statistic Olimex_Mod_MPU6050_RuntimeStatistic_t;
struct Olimex_Mod_MPU6050_SessionData
 : Stream_SessionData
{
  inline Olimex_Mod_MPU6050_SessionData ()
   : Stream_SessionData ()
   , connectionState (NULL)
   , currentStatistic ()
   , state (NULL)
   , userData (NULL)
  {};

  Olimex_Mod_MPU6050_ConnectionState*   connectionState;
  Olimex_Mod_MPU6050_RuntimeStatistic_t currentStatistic;

  Olimex_Mod_MPU6050_StreamState*       state;

  Olimex_Mod_MPU6050_UserData*          userData;
};
typedef Stream_SessionData_T<Olimex_Mod_MPU6050_SessionData> Olimex_Mod_MPU6050_StreamSessionData_t;

#endif // #ifndef OLIMEX_MOD_MPU6050_STREAM_COMMON_H
