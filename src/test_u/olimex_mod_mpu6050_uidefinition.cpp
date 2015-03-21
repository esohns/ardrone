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
#include "stdafx.h"

#include "olimex_mod_mpu6050_uidefinition.h"

#include "gtk/gtk.h"
#include "glade/glade.h"

#include "ace/Log_Msg.h"
#include "ace/Synch.h"

#include "common_file_tools.h"

#include "olimex_mod_mpu6050_callbacks.h"
#include "olimex_mod_mpu6050_macros.h"
#include "olimex_mod_mpu6050_types.h"

Olimex_Mod_MPU6050_GTKUIDefinition::Olimex_Mod_MPU6050_GTKUIDefinition (int argc_in,
                                                                        ACE_TCHAR** argv_in,
                                                                        Olimex_Mod_MPU6050_GtkCBData_t* GtkCBData_in)
 : argc_ (argc_in)
 , argv_ (argv_in)
 , GtkCBData_ (GtkCBData_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_GTKUIDefinition::Olimex_Mod_MPU6050_GTKUIDefinition"));

  ACE_ASSERT (GtkCBData_);
}

Olimex_Mod_MPU6050_GTKUIDefinition::~Olimex_Mod_MPU6050_GTKUIDefinition ()
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_GTKUIDefinition::~Olimex_Mod_MPU6050_GTKUIDefinition"));

}

bool
Olimex_Mod_MPU6050_GTKUIDefinition::initialize (const std::string& filename_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_GTKUIDefinition::initialize"));

  // sanity check(s)
  if (!Common_File_Tools::isReadable (filename_in.c_str ()))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("UI definition file \"%s\" doesn't exist, aborting\n"),
                ACE_TEXT (filename_in.c_str ())));
    return false;
  } // end IF

  ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard (GtkCBData_->lock);

  // step1: load widget tree
  ACE_ASSERT (!GtkCBData_->XML);
  GtkCBData_->XML = glade_xml_new (filename_in.c_str (), // definition file
                                   NULL,                 // root widget --> construct all
                                   NULL);                // domain
  if (!GtkCBData_->XML)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to glade_xml_new(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (filename_in.c_str ())));
    return false;
  } // end IF

  // step2: schedule UI initialization
  guint event_source_id = g_idle_add (idle_initialize_ui_cb,
                                      GtkCBData_);
  if (event_source_id == 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to g_idle_add(): \"%m\", aborting\n")));
    return false;
  } // end IF
  else
    GtkCBData_->eventSourceIds.push_back (event_source_id);

  return true;
}

void
Olimex_Mod_MPU6050_GTKUIDefinition::finalize ()
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_GTKUIDefinition::finalize"));

  // schedule UI finalization
  gpointer userData_p =
      const_cast<Olimex_Mod_MPU6050_GtkCBData_t*> (GtkCBData_);
  guint event_source_id = g_idle_add (idle_finalize_ui_cb,
                                      userData_p);
  if (event_source_id == 0)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to g_idle_add(): \"%m\", continuing\n")));
  else
  {
    ACE_Guard<ACE_Recursive_Thread_Mutex> aGuard (GtkCBData_->lock);

    GtkCBData_->eventSourceIds.push_back (event_source_id);
  } // end ELSE
}
