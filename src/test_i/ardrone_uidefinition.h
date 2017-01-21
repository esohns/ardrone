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

#ifndef OLIMEX_MOD_MPU6050_UIDEFINITION_H
#define OLIMEX_MOD_MPU6050_UIDEFINITION_H

#include <string>

#include "ace/Global_Macros.h"

#include "common_ui_common.h"
#include "common_ui_igtk.h"

// forward declarations
struct Olimex_Mod_MPU6050_GtkCBData;

// *NOTE*: the implementation is in olimex_mod_mpu6050_callbacks.cpp
class Olimex_Mod_MPU6050_GTKUIDefinition
 : public Common_UI_IGTK_T<Common_UI_GTKState>
{
 public:
  Olimex_Mod_MPU6050_GTKUIDefinition (int,                            // argc
                                      ACE_TCHAR**,                    // argv
                                      Olimex_Mod_MPU6050_GtkCBData*); // GTK cb data handle
  virtual ~Olimex_Mod_MPU6050_GTKUIDefinition ();

  // implement Common_UI_IGTK_T
  virtual bool initialize (const std::string&); // definiton filename
  virtual void finalize ();

 private:
  ACE_UNIMPLEMENTED_FUNC (Olimex_Mod_MPU6050_GTKUIDefinition ())
  ACE_UNIMPLEMENTED_FUNC (Olimex_Mod_MPU6050_GTKUIDefinition (const Olimex_Mod_MPU6050_GTKUIDefinition&))
  ACE_UNIMPLEMENTED_FUNC (Olimex_Mod_MPU6050_GTKUIDefinition& operator= (const Olimex_Mod_MPU6050_GTKUIDefinition&))

  int                           argc_;
  ACE_TCHAR**                   argv_;
  Olimex_Mod_MPU6050_GtkCBData* GtkCBData_;
};

#endif
