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

#ifndef OLIMEX_MOD_MPU6050_SESSIONCONFIGURATION_H
#define OLIMEX_MOD_MPU6050_SESSIONCONFIGURATION_H

#include "ace/Global_Macros.h"
#include "ace/Time_Value.h"

#include "stream_session_configuration_base.h"

#include "olimex_mod_mpu6050_types.h"

class Olimex_Mod_MPU6050_SessionConfiguration
 : public Stream_SessionConfigurationBase_T<Olimex_Mod_MPU6050_StreamProtocolConfigurationState_t>
{
 public:
  Olimex_Mod_MPU6050_SessionConfiguration (const Olimex_Mod_MPU6050_StreamProtocolConfigurationState_t&, // user data
                                           const ACE_Time_Value& = ACE_Time_Value::zero,                 // "official" start of session
                                           bool = false);                                                // session ended because of user abort ?
  virtual ~Olimex_Mod_MPU6050_SessionConfiguration ();

  // override Common_IDumpState
  virtual void dump_state () const;

 private:
  typedef Stream_SessionConfigurationBase_T<Olimex_Mod_MPU6050_StreamProtocolConfigurationState_t> inherited;

  ACE_UNIMPLEMENTED_FUNC (Olimex_Mod_MPU6050_SessionConfiguration ());
  ACE_UNIMPLEMENTED_FUNC (Olimex_Mod_MPU6050_SessionConfiguration (const Olimex_Mod_MPU6050_SessionConfiguration&));
  ACE_UNIMPLEMENTED_FUNC (Olimex_Mod_MPU6050_SessionConfiguration& operator= (const Olimex_Mod_MPU6050_SessionConfiguration&));
};

#endif
