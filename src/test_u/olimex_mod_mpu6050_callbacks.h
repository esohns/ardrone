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

#ifndef OLIMEX_MOD_MPU6050_CALLBACKS_H
#define OLIMEX_MOD_MPU6050_CALLBACKS_H

#include <string>

#include "gtk/gtk.h"
#include "gtk/gtkgl.h"

#include "common_ui_igtk.h"

#include "olimex_mod_mpu6050_types.h"

//unsigned int load_files (const RPG_Client_Repository&, // repository
//                         GtkListStore*);               // target liststore
//gint combobox_sort_function (GtkTreeModel*, // model
//                             GtkTreeIter*,  // row 1
//                             GtkTreeIter*,  // row 2
//                             gpointer);     // user data

// GTK callback functions
#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
G_MODULE_EXPORT gboolean idle_initialize_UI_cb (gpointer);
G_MODULE_EXPORT gboolean idle_finalize_UI_cb (gpointer);
// -----------------------------------------------------------------------------
//G_MODULE_EXPORT gint create_character_clicked_GTK_cb (GtkWidget*, gpointer);
//G_MODULE_EXPORT gint character_repository_combobox_changed_GTK_cb (GtkWidget*, gpointer);
//G_MODULE_EXPORT gint character_repository_button_clicked_GTK_cb (GtkWidget*, gpointer);
// -----------------------------------------------------------------------------
G_MODULE_EXPORT gboolean delete_event_cb (GtkWidget*, GdkEvent*, gpointer);
G_MODULE_EXPORT gboolean expose_cb (GtkWidget*, GdkEventExpose*, gpointer);
G_MODULE_EXPORT gboolean idle_cb (gpointer);
G_MODULE_EXPORT gboolean configure_cb (GtkWidget*, GdkEventConfigure*, gpointer);
//------------------------------------------------------------------------------
G_MODULE_EXPORT gint about_clicked_GTK_cb (GtkWidget*, gpointer);
G_MODULE_EXPORT gint quit_clicked_GTK_cb (GtkWidget*, gpointer);
#ifdef __cplusplus
}
#endif /* __cplusplus */

// GTK initialization functions
bool
initialize_UI_client (int,                              // argc
                      ACE_TCHAR**,                      // argv
                      const std::string&,               // UI definiton file
                      Olimex_Mod_MPU6050_GtkCBData_t&); // GTK cb data (out)
void
finalize_UI_client (const Olimex_Mod_MPU6050_GtkCBData_t&); // GTK cb data

class Olimex_Mod_MPU6050_GTKUIDefinition
 : public Common_UI_IGTK
{
 public:
  Olimex_Mod_MPU6050_GTKUIDefinition (int,                              // argc
                                      ACE_TCHAR**,                      // argv
                                      Olimex_Mod_MPU6050_GtkCBData_t*); // GTK cb data handle
  virtual ~Olimex_Mod_MPU6050_GTKUIDefinition ();

  // implement Common_UI_IGTK
  virtual bool initialize (const std::string&); // definiton filename
  virtual void finalize ();

 private:
  ACE_UNIMPLEMENTED_FUNC (Olimex_Mod_MPU6050_GTKUIDefinition ());
  ACE_UNIMPLEMENTED_FUNC (Olimex_Mod_MPU6050_GTKUIDefinition (const Olimex_Mod_MPU6050_GTKUIDefinition&));
  ACE_UNIMPLEMENTED_FUNC (Olimex_Mod_MPU6050_GTKUIDefinition& operator= (const Olimex_Mod_MPU6050_GTKUIDefinition&));

  int                             argc_;
  ACE_TCHAR**                     argv_;
  Olimex_Mod_MPU6050_GtkCBData_t* GTKCBData_;
};

#endif
