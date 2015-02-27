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

#ifndef OLIMEX_MOD_MPU6050_EVENTHANDLER_H
#define OLIMEX_MOD_MPU6050_EVENTHANDLER_H

#include "olimex_mod_mpu6050_message.h"
#include "olimex_mod_mpu6050_types.h"

class Olimex_Mod_MPU6050_EventHandler
 : public Olimex_Mod_MPU6050_Notification_t
{
 public:
  Olimex_Mod_MPU6050_EventHandler (Olimex_Mod_MPU6050_GtkCBData_t*); // Gtk state
  virtual ~Olimex_Mod_MPU6050_EventHandler ();

  // implement Common_INotify_T
  virtual void start ();
  virtual void notify (const Olimex_Mod_MPU6050_Message&); // data
  virtual void end ();

 private:
  typedef Olimex_Mod_MPU6050_Notification_t inherited;

  ACE_UNIMPLEMENTED_FUNC (Olimex_Mod_MPU6050_EventHandler ());
  ACE_UNIMPLEMENTED_FUNC (Olimex_Mod_MPU6050_EventHandler (const Olimex_Mod_MPU6050_EventHandler&));
  ACE_UNIMPLEMENTED_FUNC (Olimex_Mod_MPU6050_EventHandler& operator= (const Olimex_Mod_MPU6050_EventHandler&));

  Olimex_Mod_MPU6050_GtkCBData_t* GtkCBData_;
};

#endif
