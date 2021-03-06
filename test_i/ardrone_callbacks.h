﻿/***************************************************************************
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

#ifndef ARDRONE_CALLBACKS_H
#define ARDRONE_CALLBACKS_H

#include "gtk/gtk.h"

// thread functions
ACE_THR_FUNC_RETURN stream_processing_function (void*);

//------------------------------------------------------------------------------

gboolean idle_associated_SSID_cb (gpointer);
gboolean idle_disassociated_SSID_cb (gpointer);
gboolean idle_finalize_ui_cb (gpointer);
gboolean idle_initialize_ui_cb (gpointer);
gboolean idle_session_end_cb (gpointer);
gboolean idle_session_start_cb (gpointer);
gboolean idle_update_info_display_cb (gpointer);
//gboolean idle_update_log_display_cb (gpointer);
gboolean idle_update_state_cb (gpointer);
gboolean idle_update_progress_cb (gpointer);
gboolean idle_update_orientation_display_cb (gpointer);
gboolean idle_update_video_display_cb (gpointer);

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
// -----------------------------------------------------------------------------
G_MODULE_EXPORT void toggleaction_connect_toggled_cb (GtkToggleAction*, gpointer);
G_MODULE_EXPORT void action_trim_activate_cb (GtkAction*, gpointer);
G_MODULE_EXPORT void action_calibrate_activate_cb (GtkAction*, gpointer);
G_MODULE_EXPORT void action_dump_activate_cb (GtkAction*, gpointer);
// -----------------------------------------------------------------------------
G_MODULE_EXPORT void combobox_wlan_interface_changed_cb (GtkComboBox*, gpointer);
G_MODULE_EXPORT void entry_address_delete_text_cb (GtkEditable*,
                                                   gint,
                                                   gint,
                                                   gpointer);
G_MODULE_EXPORT void entry_address_insert_text_cb (GtkEditable*,
                                                   gchar*,
                                                   gint,
                                                   gpointer,
                                                   gpointer);
G_MODULE_EXPORT void combobox_display_device_changed_cb (GtkComboBox*, gpointer);
G_MODULE_EXPORT void combobox_display_format_changed_cb (GtkComboBox*, gpointer);
//G_MODULE_EXPORT void combobox_save_format_changed_cb (GtkComboBox*, gpointer);
#if GTK_CHECK_VERSION(3,10,0)
G_MODULE_EXPORT void places_save_mount_cb (GtkPlacesSidebar*,
                                           GMountOperation*,
                                           gpointer);
#endif // GTK_CHECK_VERSION(3,10,0)
//------------------------------------------------------------------------------
#if defined (GTKGL_SUPPORT)
G_MODULE_EXPORT void glarea_realize_cb (GtkWidget*, gpointer);
#if GTK_CHECK_VERSION(3,0,0)
#if GTK_CHECK_VERSION(3,16,0)
G_MODULE_EXPORT GdkGLContext* glarea_create_context_cb (GtkGLArea*, gpointer);
G_MODULE_EXPORT gboolean glarea_render_cb (GtkGLArea*, GdkGLContext*, gpointer);
G_MODULE_EXPORT void glarea_resize_cb (GtkGLArea*, gint, gint, gpointer);
#else
#if defined (GTKGLAREA_SUPPORT)
G_MODULE_EXPORT void glarea_configure_event_cb (GtkWidget*, GdkEvent*, gpointer);
G_MODULE_EXPORT gboolean glarea_expose_event_cb (GtkWidget*, cairo_t*, gpointer);
#else
G_MODULE_EXPORT void glarea_size_allocate_event_cb (GtkWidget*, GdkRectangle*, gpointer);
G_MODULE_EXPORT gboolean glarea_draw_cb (GtkWidget*, cairo_t*, gpointer);
#endif /* GTKGLAREA_SUPPORT */
#endif /* GTK_CHECK_VERSION (3,16,0) */
#else
#if defined (GTKGLAREA_SUPPORT)
G_MODULE_EXPORT void glarea_configure_event_cb (GtkWidget*, GdkEvent*, gpointer);
G_MODULE_EXPORT gboolean glarea_expose_event_cb (GtkWidget*, cairo_t*, gpointer);
#else
G_MODULE_EXPORT void glarea_configure_event_cb (GtkWidget*, GdkEvent*, gpointer);
G_MODULE_EXPORT gboolean glarea_expose_event_cb (GtkWidget*, cairo_t*, gpointer);
#endif /* GTKGLAREA_SUPPORT */
#endif /* GTK_CHECK_VERSION (3,0,0) */
#endif /* GTKGL_SUPPORT */
//------------------------------------------------------------------------------
G_MODULE_EXPORT void toggleaction_video_toggled_cb (GtkToggleAction*, gpointer);
G_MODULE_EXPORT void toggleaction_fullscreen_toggled_cb (GtkToggleAction*, gpointer);
G_MODULE_EXPORT void toggleaction_save_toggled_cb (GtkToggleAction*, gpointer);
G_MODULE_EXPORT void action_cut_activate_cb (GtkAction*, gpointer);
G_MODULE_EXPORT void toggleaction_associate_toggled_cb (GtkToggleAction*, gpointer);
//------------------------------------------------------------------------------
#if GTK_CHECK_VERSION(3,0,0)
G_MODULE_EXPORT void drawingarea_video_size_allocate_cb (GtkWidget*, GdkRectangle*, gpointer);
#else
G_MODULE_EXPORT gboolean drawingarea_video_configure_cb (GtkWidget*, GdkEvent*, gpointer);
#endif // GTK_CHECK_VERSION(3,0,0)
G_MODULE_EXPORT gboolean drawingarea_video_draw_cb (GtkWidget*, cairo_t*, gpointer);
G_MODULE_EXPORT void drawingarea_video_realize_cb (GtkWidget*, gpointer);
G_MODULE_EXPORT gboolean drawingarea_video_key_press_event_cb (GtkWidget*, GdkEventKey*, gpointer);
G_MODULE_EXPORT gboolean key_cb (GtkWidget*, GdkEventKey*, gpointer);
//------------------------------------------------------------------------------
G_MODULE_EXPORT gboolean dialog_main_key_press_event_cb (GtkWidget*, GdkEventKey*, gpointer);
//G_MODULE_EXPORT gint button_clear_clicked_cb (GtkWidget*, gpointer);
G_MODULE_EXPORT gint button_about_clicked_cb (GtkWidget*, gpointer);
G_MODULE_EXPORT gint button_quit_clicked_cb (GtkWidget*, gpointer);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
