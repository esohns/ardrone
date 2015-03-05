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

#include "gtk/gtk.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
G_MODULE_EXPORT gboolean idle_initialize_UI_cb (gpointer);
G_MODULE_EXPORT gboolean idle_finalize_UI_cb (gpointer);
// -----------------------------------------------------------------------------
G_MODULE_EXPORT gboolean button_cb (GtkWidget*, GdkEventButton*, gpointer);
G_MODULE_EXPORT gboolean configure_cb (GtkWidget*, GdkEventConfigure*, gpointer);
//G_MODULE_EXPORT gboolean delete_event_cb (GtkWidget*, GdkEvent*, gpointer);
G_MODULE_EXPORT gboolean expose_cb (GtkWidget*, GdkEventExpose*, gpointer);
G_MODULE_EXPORT gboolean process_cb (gpointer);
G_MODULE_EXPORT gboolean key_cb (GtkWidget*, GdkEventKey*, gpointer);
//------------------------------------------------------------------------------
G_MODULE_EXPORT gint about_clicked_GTK_cb (GtkWidget*, gpointer);
G_MODULE_EXPORT gint quit_clicked_GTK_cb (GtkWidget*, gpointer);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
