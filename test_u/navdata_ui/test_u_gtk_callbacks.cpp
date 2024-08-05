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
#include "stdafx.h"

#include "ace/Log_Msg.h"
//#include "ace/Synch.h"

#include "common_ui_defines.h"

#include "common_ui_gtk_common.h"
#include "common_ui_gtk_manager_common.h"

#include "stream_macros.h"

#include "test_u_common.h"
#include "test_u_defines.h"

#include "test_u_gtk_callbacks.h"

// global variables
bool un_toggling_start = false;

gboolean
idle_initialize_UI_cb (gpointer userData_in)
{
  STREAM_TRACE (ACE_TEXT ("::idle_initialize_UI_cb"));

  // sanity check(s)
  struct Test_U_UI_CBData* ui_cb_data_p =
    static_cast<struct Test_U_UI_CBData*> (userData_in);
  ACE_ASSERT (ui_cb_data_p);
  Common_UI_GTK_BuildersIterator_t iterator =
    ui_cb_data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN));
  ACE_ASSERT (iterator != ui_cb_data_p->UIState->builders.end ());
  Common_UI_GTK_Manager_t* gtk_manager_p =
    COMMON_UI_GTK_MANAGER_SINGLETON::instance ();
  ACE_ASSERT (gtk_manager_p);
  Common_UI_GTK_State_t& state_r =
    const_cast<Common_UI_GTK_State_t&> (gtk_manager_p->getR ());

  // step1: initialize dialog window(s)
  GtkWidget* dialog_p =
    GTK_WIDGET (gtk_builder_get_object ((*iterator).second.second,
      ACE_TEXT_ALWAYS_CHAR (TEST_U_UI_GTK_DIALOG_MAIN_NAME)));
  ACE_ASSERT (dialog_p);
  //  GtkWidget* image_icon_p = gtk_image_new_from_file (path.c_str ());
  //  ACE_ASSERT (image_icon_p);
  //  gtk_window_set_icon (GTK_WINDOW (dialog_p),
  //                       gtk_image_get_pixbuf (GTK_IMAGE (image_icon_p)));
  //GdkWindow* dialog_window_p = gtk_widget_get_window (dialog_p);
  //gtk_window_set_title (dialog_window_p,
  //                      caption.c_str ());

  GtkWidget* about_dialog_p =
    GTK_WIDGET (gtk_builder_get_object ((*iterator).second.second,
      ACE_TEXT_ALWAYS_CHAR (TEST_U_UI_GTK_DIALOG_ABOUT_NAME)));
  ACE_ASSERT (about_dialog_p);

  GtkSpinButton* spin_button_p =
    GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
      ACE_TEXT_ALWAYS_CHAR (TEST_U_UI_GTK_SPINBUTTON_DATAMESSAGES_NAME)));
  ACE_ASSERT (spin_button_p);
  gtk_spin_button_set_range (spin_button_p,
                             0.0,
                             static_cast<double> (std::numeric_limits<ACE_UINT64>::max ()));
  spin_button_p =
    GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
      ACE_TEXT_ALWAYS_CHAR (TEST_U_UI_GTK_SPINBUTTON_SESSIONMESSAGES_NAME)));
  ACE_ASSERT (spin_button_p);
  gtk_spin_button_set_range (spin_button_p,
                             0.0,
                             static_cast<double> (std::numeric_limits<ACE_UINT64>::max ()));
  spin_button_p =
    GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
      ACE_TEXT_ALWAYS_CHAR (TEST_U_UI_GTK_SPINBUTTON_DATA_NAME)));
  ACE_ASSERT (spin_button_p);
  gtk_spin_button_set_range (spin_button_p,
                             0.0,
                             static_cast<double> (std::numeric_limits<ACE_UINT64>::max ()));

  GtkDrawingArea* drawing_area_p =
    GTK_DRAWING_AREA (gtk_builder_get_object ((*iterator).second.second,
      ACE_TEXT_ALWAYS_CHAR (TEST_U_UI_GTK_DRAWINGAREA_NAME)));
  ACE_ASSERT (drawing_area_p);

  GtkProgressBar* progress_bar_p =
    GTK_PROGRESS_BAR (gtk_builder_get_object ((*iterator).second.second,
      ACE_TEXT_ALWAYS_CHAR (TEST_U_UI_GTK_PROGRESSBAR_NAME)));
  ACE_ASSERT (progress_bar_p);
  gint width, height;
  gtk_widget_get_size_request (GTK_WIDGET (progress_bar_p), &width, &height);
  gtk_progress_bar_set_pulse_step (progress_bar_p,
    1.0 / static_cast<double> (width));
  gtk_progress_bar_set_text (progress_bar_p,
                             ACE_TEXT_ALWAYS_CHAR (""));

  // step5: initialize updates
  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, state_r.lock, G_SOURCE_REMOVE);
    // schedule asynchronous updates of the log view
    guint event_source_id =
      g_timeout_add (COMMON_UI_REFRESH_DEFAULT_WIDGET_MS,
                     idle_update_info_display_cb,
                     userData_in);
    if (event_source_id > 0)
      state_r.eventSourceIds.insert (event_source_id);
    else
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("failed to g_timeout_add(): \"%m\", aborting\n")));
      return G_SOURCE_REMOVE;
    } // end ELSE
  } // end lock scope

  // step2: (auto-)connect signals/slots
  gtk_builder_connect_signals ((*iterator).second.second,
                               ui_cb_data_p);

  // step6a: connect default signals
  gulong result_2 =
    g_signal_connect (dialog_p,
                      ACE_TEXT_ALWAYS_CHAR ("destroy"),
                      G_CALLBACK (gtk_widget_destroyed),
                      NULL);
  ACE_ASSERT (result_2);

  result_2 = g_signal_connect_swapped (G_OBJECT (about_dialog_p),
                                       ACE_TEXT_ALWAYS_CHAR ("response"),
                                       G_CALLBACK (gtk_widget_hide),
                                       about_dialog_p);
  ACE_ASSERT (result_2);

  //   // step8: use correct screen
  //   if (parentWidget_in)
  //     gtk_window_set_screen (GTK_WINDOW (dialog_p),
  //                            gtk_widget_get_screen (const_cast<GtkWidget*> (//parentWidget_in)));

  // step9: draw main dialog
  gtk_widget_show_all (dialog_p);

  return G_SOURCE_REMOVE;
}

gboolean
idle_finalize_UI_cb (gpointer userData_in)
{
  STREAM_TRACE (ACE_TEXT ("::idle_finalize_UI_cb"));

  ACE_UNUSED_ARG (userData_in);

  // leave GTK
  gtk_main_quit ();

  return G_SOURCE_REMOVE;
}

gboolean
idle_update_info_display_cb (gpointer userData_in)
{
  STREAM_TRACE (ACE_TEXT ("::idle_update_info_display_cb"));

  // sanity check(s)
  struct Test_U_UI_CBData* ui_cb_data_p =
    static_cast<struct Test_U_UI_CBData*> (userData_in);
  ACE_ASSERT (ui_cb_data_p);
  Common_UI_GTK_BuildersIterator_t iterator =
    ui_cb_data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN));
  ACE_ASSERT (iterator != ui_cb_data_p->UIState->builders.end ());

  GtkSpinButton* spin_button_p = NULL;
  bool is_session_message = false;
  enum Common_UI_EventType* event_p = NULL;
  int result = -1;
  Common_UI_GTK_Manager_t* gtk_manager_p =
    COMMON_UI_GTK_MANAGER_SINGLETON::instance ();
  ACE_ASSERT (gtk_manager_p);
  Common_UI_GTK_State_t& state_r =
    const_cast<Common_UI_GTK_State_t&> (gtk_manager_p->getR ());

  { ACE_GUARD_RETURN (ACE_SYNCH_MUTEX, aGuard, state_r.lock, G_SOURCE_REMOVE);
    for (Common_UI_Events_t::ITERATOR iterator_2 (state_r.eventStack);
         iterator_2.next (event_p);
         iterator_2.advance ())
    { ACE_ASSERT (event_p);
      switch (*event_p)
      {
        case COMMON_UI_EVENT_STARTED:
        {
          spin_button_p =
            GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                     ACE_TEXT_ALWAYS_CHAR (TEST_U_UI_GTK_SPINBUTTON_DATAMESSAGES_NAME)));
          ACE_ASSERT (spin_button_p);
          gtk_spin_button_set_value (spin_button_p, 0.0);

          spin_button_p =
            //GTK_SPIN_BUTTON (glade_xml_get_widget ((*iterator).second.second,
            //                                       ACE_TEXT_ALWAYS_CHAR (TEST_U_STREAM_UI_GTK_SPINBUTTON_SESSIONMESSAGES_NAME)));
            GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                     ACE_TEXT_ALWAYS_CHAR (TEST_U_UI_GTK_SPINBUTTON_SESSIONMESSAGES_NAME)));
          ACE_ASSERT (spin_button_p);
          gtk_spin_button_set_value (spin_button_p, 0.0);
          is_session_message = true;
          break;
        }
        case COMMON_UI_EVENT_DATA:
        {
          spin_button_p =
            GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                     ACE_TEXT_ALWAYS_CHAR (TEST_U_UI_GTK_SPINBUTTON_DATAMESSAGES_NAME)));
          ACE_ASSERT (spin_button_p);
          break;
        }
        case COMMON_UI_EVENT_FINISHED:
        {
          spin_button_p =
            //GTK_SPIN_BUTTON (glade_xml_get_widget ((*iterator).second.second,
            //                                       ACE_TEXT_ALWAYS_CHAR (TEST_U_STREAM_UI_GTK_SPINBUTTON_SESSIONMESSAGES_NAME)));
            GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                     ACE_TEXT_ALWAYS_CHAR (TEST_U_UI_GTK_SPINBUTTON_SESSIONMESSAGES_NAME)));
          ACE_ASSERT (spin_button_p);
          is_session_message = true;
          break;
        }
        case COMMON_UI_EVENT_ABORT:
        case COMMON_UI_EVENT_CONNECT:
        case COMMON_UI_EVENT_DISCONNECT:
        case COMMON_UI_EVENT_STATISTIC:
        case COMMON_UI_EVENT_STEP:
        {
          spin_button_p =
            GTK_SPIN_BUTTON (gtk_builder_get_object ((*iterator).second.second,
                                                     ACE_TEXT_ALWAYS_CHAR (TEST_U_UI_GTK_SPINBUTTON_SESSIONMESSAGES_NAME)));
          ACE_ASSERT (spin_button_p);
          is_session_message = true;
          break;
        }
        default:
        {
          ACE_DEBUG ((LM_WARNING,
                      ACE_TEXT ("invalid/unknown event type (was: %d), continuing\n"),
                      *event_p));
          spin_button_p = NULL;
          break;
        }
      } // end SWITCH
      ACE_UNUSED_ARG (is_session_message);
      if (likely (spin_button_p))
        gtk_spin_button_spin (spin_button_p,
                              GTK_SPIN_STEP_FORWARD,
                              1.0);
      event_p = NULL;
    } // end FOR

    // clean up
    Common_UI_EventType event_e;
    while (!state_r.eventStack.is_empty ())
    {
      result = state_r.eventStack.pop (event_e);
      if (result == -1)
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("failed to ACE_Unbounded_Stack::pop(): \"%m\", continuing\n")));
    } // end WHILE
  } // end lock scope

  std::ostringstream converter;
  converter.precision (5);
  converter << ui_cb_data_p->openGLScene.orientation.x;
  GtkLabel* label_p =
    GTK_LABEL (gtk_builder_get_object ((*iterator).second.second,
                                       ACE_TEXT_ALWAYS_CHAR (TEST_U_UI_GTK_LABEL_ROLL_NAME)));
  ACE_ASSERT (label_p);
  gtk_label_set_text (label_p,
                      converter.str ().c_str ());

  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << ui_cb_data_p->openGLScene.orientation.y;
  label_p =
      GTK_LABEL (gtk_builder_get_object ((*iterator).second.second,
                                         ACE_TEXT_ALWAYS_CHAR (TEST_U_UI_GTK_LABEL_PITCH_NAME)));
  ACE_ASSERT (label_p);
  gtk_label_set_text (label_p,
                      converter.str ().c_str ());

  converter.str (ACE_TEXT_ALWAYS_CHAR (""));
  converter.clear ();
  converter << ui_cb_data_p->openGLScene.orientation.z;
  label_p =
    GTK_LABEL (gtk_builder_get_object ((*iterator).second.second,
                                       ACE_TEXT_ALWAYS_CHAR (TEST_U_UI_GTK_LABEL_YAW_NAME)));
  ACE_ASSERT (label_p);
  gtk_label_set_text (label_p,
                      converter.str ().c_str ());

  return G_SOURCE_CONTINUE;
}

//////////////////////////////////////////

void
togglebutton_start_land_toggled_cb (GtkToggleButton* toggleButton_in,
                                    gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::togglebutton_start_land_toggled_cb"));

  // handle untoggle --> PLAY
  if (un_toggling_start)
  {
    un_toggling_start = false;
    return; // done
  } // end IF

  // sanity check(s)
  struct Test_U_UI_CBData* ui_cb_data_p =
    static_cast<struct Test_U_UI_CBData*> (userData_in);
  ACE_ASSERT (ui_cb_data_p);
  ACE_ASSERT (ui_cb_data_p->controller);

  Common_UI_GTK_BuildersIterator_t iterator =
    ui_cb_data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != ui_cb_data_p->UIState->builders.end ());

  bool is_active_b = gtk_toggle_button_get_active (toggleButton_in);

  if (!is_active_b)
  {
    // --> user pressed pause/stop

    try {
      ui_cb_data_p->controller->land ();
    } catch (...) {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("caught exception in ARDrone_IController::land(), continuing\n")));
    }

    return;
  } // end IF

  // --> user pressed play

  try {
    ui_cb_data_p->controller->takeoff ();
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in ARDrone_IController::takeoff(), continuing\n")));
  }
}

gint
button_trim_clicked_cb (GtkWidget* widget_in,
                        gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::button_trim_clicked_cb"));

  ACE_UNUSED_ARG (widget_in);
  // sanity check(s)
  struct Test_U_UI_CBData* ui_cb_data_p =
    static_cast<struct Test_U_UI_CBData*> (userData_in);
  ACE_ASSERT (ui_cb_data_p);
  ACE_ASSERT (ui_cb_data_p->controller);

  try {
    ui_cb_data_p->controller->trim ();
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in ARDrone_IController::trim(), continuing\n")));
  }

  return FALSE;
}

gint
button_calibrate_clicked_cb (GtkWidget* widget_in,
                             gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::button_calibrate_clicked_cb"));

  ACE_UNUSED_ARG (widget_in);
  // sanity check(s)
  struct Test_U_UI_CBData* ui_cb_data_p =
    static_cast<struct Test_U_UI_CBData*> (userData_in);
  ACE_ASSERT (ui_cb_data_p);
  ACE_ASSERT (ui_cb_data_p->controller);

  try {
    ui_cb_data_p->controller->calibrate ();
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in ARDrone_IController::calibrate(), continuing\n")));
  }

  return FALSE;
}

gint
button_leds_clicked_cb (GtkWidget* widget_in,
                        gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::button_leds_clicked_cb"));

  ACE_UNUSED_ARG (widget_in);
  // sanity check(s)
  struct Test_U_UI_CBData* ui_cb_data_p =
    static_cast<struct Test_U_UI_CBData*> (userData_in);
  ACE_ASSERT (ui_cb_data_p);
  ACE_ASSERT (ui_cb_data_p->controller);

  try {
    ui_cb_data_p->controller->leds ();
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in ARDrone_IController::leds(), continuing\n")));
  }

  return FALSE;
}

gint
button_reset_clicked_cb (GtkWidget* widget_in,
                         gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::button_reset_clicked_cb"));

  ACE_UNUSED_ARG (widget_in);
  // sanity check(s)
  struct Test_U_UI_CBData* ui_cb_data_p =
    static_cast<struct Test_U_UI_CBData*> (userData_in);
  ACE_ASSERT (ui_cb_data_p);
  ACE_ASSERT (ui_cb_data_p->controller);

  try {
    ui_cb_data_p->controller->reset ();
  } catch (...) {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("caught exception in ARDrone_IController::reset(), continuing\n")));
  }

  return FALSE;
}

gint
button_about_clicked_cb (GtkWidget* widget_in,
                         gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::button_about_clicked_cb"));

  ACE_UNUSED_ARG (widget_in);
  // sanity check(s)
  struct Test_U_UI_CBData* ui_cb_data_p =
    static_cast<struct Test_U_UI_CBData*> (userData_in);
  ACE_ASSERT (ui_cb_data_p);

  Common_UI_GTK_BuildersIterator_t iterator =
    ui_cb_data_p->UIState->builders.find (ACE_TEXT_ALWAYS_CHAR (COMMON_UI_DEFINITION_DESCRIPTOR_MAIN));
  // sanity check(s)
  ACE_ASSERT (iterator != ui_cb_data_p->UIState->builders.end ());

  // retrieve about dialog handle
  GtkDialog* dialog_p =
    GTK_DIALOG (gtk_builder_get_object ((*iterator).second.second,
                                        ACE_TEXT_ALWAYS_CHAR (TEST_U_UI_GTK_DIALOG_ABOUT_NAME)));
  ACE_ASSERT (dialog_p);

  // run dialog
  gint result = gtk_dialog_run (dialog_p);
  switch (result)
  {
    case GTK_RESPONSE_ACCEPT:
      break;
    default:
      break;
  } // end SWITCH
  gtk_widget_hide (GTK_WIDGET (dialog_p));

  return FALSE;
} // button_about_clicked_cb

gint
button_quit_clicked_cb (GtkWidget* widget_in,
                        gpointer userData_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::button_quit_clicked_cb"));

  ACE_UNUSED_ARG (widget_in);
  // sanity check(s)
  struct Test_U_UI_CBData* ui_cb_data_p =
    static_cast<struct Test_U_UI_CBData*> (userData_in);
  ACE_ASSERT (ui_cb_data_p);
  ACE_ASSERT (ui_cb_data_p->stream);
  ACE_ASSERT (ui_cb_data_p->UIState);

  // clear any event source(s)
  for (Common_UI_GTK_EventSourceIdsIterator_t iterator = ui_cb_data_p->UIState->eventSourceIds.begin ();
       iterator != ui_cb_data_p->UIState->eventSourceIds.end ();
       ++iterator)
    g_source_remove (*iterator);
  ui_cb_data_p->UIState->eventSourceIds.clear ();

  // stop stream
  ui_cb_data_p->stream->stop (false,
                              true,
                              true);

  // step2: initiate shutdown sequence
  COMMON_UI_GTK_MANAGER_SINGLETON::instance ()->stop (false); // do not wait for ourselves !

  return FALSE;
} // button_quit_clicked_cb
