﻿/*  I2C kernel module driver for the Olimex MOD-MPU6050 UEXT module
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

#ifndef OLIMEX_MOD_MPU6050_STREAM_H
#define OLIMEX_MOD_MPU6050_STREAM_H

#include "stream_messageallocatorheap_base.h"

#include "olimex_mod_mpu6050_message.h"
#include "olimex_mod_mpu6050_sessionmessage.h"

typedef Stream_MessageAllocatorHeapBase_T<Olimex_Mod_MPU6050_Message,
                                          Olimex_Mod_MPU6050_SessionMessage> Olimex_Mod_MPU6050_MessageAllocator_t;

#endif // #ifndef OLIMEX_MOD_MPU6050_STREAM_H