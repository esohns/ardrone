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

#include "olimex_mod_mpu6050_callbacks.h"

#include <sstream>

#include "ace/Dirent_Selector.h"
#include "ace/Log_Msg.h"
#include "ace/OS.h"
#include "ace/Reactor.h"

#include "GL/gl.h"

#include "gmodule.h"

#include "gtk/gtkgl.h"

#include "common_macros.h"
#include "common_file_tools.h"

#include "olimex_mod_mpu6050_defines.h"

//unsigned int
//load_files(const RPG_Client_Repository& repository_in,
//           GtkListStore* listStore_in)
//{
//  RPG_TRACE(ACE_TEXT("::load_files"));

//  unsigned int return_value = 0;

//  // sanity check(s)
//  ACE_ASSERT(listStore_in);
//  std::string repository;
//  ACE_SCANDIR_SELECTOR selector = NULL;
//  std::string extension;
//  switch (repository_in)
//  {
//    case REPOSITORY_MAPS:
//    {
//      repository = RPG_Map_Common_Tools::getMapsDirectory();
//      selector = ::dirent_selector_maps;
//      extension = ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_LEVEL_FILE_EXT);
//      break;
//    }
//    case REPOSITORY_PROFILES:
//    {
//      repository = RPG_Player_Common_Tools::getPlayerProfilesDirectory();
//      selector = ::dirent_selector_profiles;
//      extension = ACE_TEXT_ALWAYS_CHAR(RPG_PLAYER_PROFILE_EXT);
//      break;
//    }
//    case REPOSITORY_ENGINESTATE:
//    {
//      repository = RPG_Engine_Common_Tools::getEngineStateDirectory();
//      selector = ::dirent_selector_savedstates;
//      extension = ACE_TEXT_ALWAYS_CHAR(RPG_ENGINE_STATE_EXT);
//      break;
//    }
//    default:
//    {
//      ACE_DEBUG((LM_ERROR,
//                 ACE_TEXT("invalid repository (was: %d), aborting\n"),
//                 repository_in));

//      return 0;
//    }
//  } // end IF
//  if (!RPG_Common_File_Tools::isDirectory(repository))
//  {
//    ACE_DEBUG((LM_ERROR,
//               ACE_TEXT("failed to load_files(\"%s\"), not a directory, aborting\n"),
//               ACE_TEXT(repository.c_str())));

//    return 0;
//  } // end IF

//  // retrieve all relevant files and sort them alphabetically...
//  ACE_Dirent_Selector entries;
//  if (entries.open(repository.c_str(),
//                   selector,
//                   &::dirent_comparator) == -1)
//  {
//    ACE_DEBUG((LM_ERROR,
//               ACE_TEXT("failed to ACE_Dirent_Selector::open(\"%s\"): \"%m\", aborting\n"),
//               ACE_TEXT(repository.c_str())));

//    return 0;
//  } // end IF

//  // clear existing entries
//  // *WARNING* triggers the "changed" signal of the combobox...
//  gtk_list_store_clear(listStore_in);

//  // iterate over entries
//  std::string entry;
//  GtkTreeIter iter;
//  size_t position = -1;
//  for (unsigned int i = 0;
//       i < static_cast<unsigned int>(entries.length());
//       i++)
//  {
//    // sanitize name (chop off extension)
//    entry = entries[i]->d_name;
//    position = entry.rfind(extension,
//                           std::string::npos);
//    ACE_ASSERT(position != std::string::npos);
//    entry.erase(position,
//                std::string::npos);

//    // append new (text) entry
//    gtk_list_store_append(listStore_in, &iter);
//    gtk_list_store_set(listStore_in, &iter,
//                       0, ACE_TEXT(entry.c_str()), // column 0
//                       -1);

//    return_value++;
//  } // end FOR

//  // clean up
//  if (entries.close() == -1)
//    ACE_DEBUG((LM_ERROR,
//               ACE_TEXT("failed to ACE_Dirent_Selector::close: \"%m\", continuing\n")));

//  // debug info
//  GValue value;
//  ACE_OS::memset(&value, 0, sizeof(value));
//  const gchar* text = NULL;
//  if (!gtk_tree_model_get_iter_first(GTK_TREE_MODEL(listStore_in),
//                                     &iter))
//    return 0;
//  gtk_tree_model_get_value(GTK_TREE_MODEL(listStore_in), &iter,
//                           0, &value);
//  text = g_value_get_string(&value);
//  extension.erase(0, 1);
//  ACE_DEBUG((LM_DEBUG,
//             ACE_TEXT("%s[0]: %s\n"),
//             ACE_TEXT(extension.c_str()),
//             ACE_TEXT(text)));
//  g_value_unset(&value);
//  for (unsigned int i = 1;
//       gtk_tree_model_iter_next(GTK_TREE_MODEL(listStore_in),
//                                &iter);
//       i++)
//  {
//    ACE_OS::memset(&value, 0, sizeof(value));
//    text = NULL;

//    gtk_tree_model_get_value(GTK_TREE_MODEL(listStore_in), &iter,
//                             0, &value);
//    text = g_value_get_string(&value);
//    ACE_DEBUG((LM_DEBUG,
//               ACE_TEXT("%s[%u]: %s\n"),
//               ACE_TEXT(extension.c_str()),
//               i,
//               ACE_TEXT(text)));

//    // clean up
//    g_value_unset(&value);
//  } // end FOR

//  return return_value;
//}

//gint
//combobox_sort_function(GtkTreeModel* model_in,
//											 GtkTreeIter*  iterator1_in,
//											 GtkTreeIter*  iterator2_in,
//											 gpointer      userData_in)
//{
//	RPG_TRACE(ACE_TEXT("::combobox_sort_function"));

//	gint sort_column = GPOINTER_TO_INT(userData_in);
//	gint result = 0; // -1: row1 < row2, 0: equal, 1: row1 > row2

//	switch (sort_column)
//	{
//		case 0:
//		{
//			gchar* row1, *row2;
//			gtk_tree_model_get(model_in, iterator1_in, sort_column, &row1, -1);
//			gtk_tree_model_get(model_in, iterator2_in, sort_column, &row2, -1);
//			if ((row1 == NULL) ||
//					(row2 == NULL))
//			{
//				if ((row1 == NULL) &&
//						(row2 == NULL))
//					break; // --> equal

//				result = ((row1 == NULL) ? -1 : 1);

//				break;
//			} // end IF

//			// compare (case-insensitive)
//			gchar* row1_lower, *row2_lower;
//			row1_lower = g_utf8_strdown(row1, -1);
//			row2_lower = g_utf8_strdown(row2, -1);
//			g_free(row1);
//			g_free(row2);

//			result = g_utf8_collate(row1_lower, row2_lower);
//			g_free(row1_lower);
//			g_free(row2_lower);

//			break;
//		}
//		default:
//			g_return_val_if_reached(0);
//	} // end SWITCH

//	return result;
//}

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
G_MODULE_EXPORT gboolean
idle_initialize_UI_cb (gpointer act_in)
{
  COMMON_TRACE (ACE_TEXT ("::idle_initialize_UI_cb"));

  Olimex_Mod_MPU6050_GtkCBData_t* data =
      static_cast<Olimex_Mod_MPU6050_GtkCBData_t*> (act_in);
  ACE_ASSERT (data);

  // sanity check(s)
  ACE_ASSERT (data->xml);

//	// activate first repository entry (if any)
//	GtkComboBox* combobox =
//			GTK_COMBO_BOX(glade_xml_get_widget(data->XML,
//																				 ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_COMBOBOX_CHARACTER_NAME)));
//  ACE_ASSERT(combobox);
//  if (gtk_widget_is_sensitive(GTK_WIDGET(combobox)))
//    gtk_combo_box_set_active(combobox, 0);
//  combobox =
//      GTK_COMBO_BOX(glade_xml_get_widget(data->XML,
//                                         ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_COMBOBOX_MAP_NAME)));
//  ACE_ASSERT(combobox);
//  if (gtk_widget_is_sensitive(GTK_WIDGET(combobox)))
//    gtk_combo_box_set_active(combobox, 0);

  // one-shot action
  return FALSE; // G_SOURCE_REMOVE
}

G_MODULE_EXPORT gboolean
idle_finalize_UI_cb (gpointer act_in)
{
  COMMON_TRACE (ACE_TEXT ("::idle_finalize_UI_cb"));

  Olimex_Mod_MPU6050_GtkCBData_t* data =
      static_cast<Olimex_Mod_MPU6050_GtkCBData_t*> (act_in);
  ACE_ASSERT (data);

  // sanity check(s)
  ACE_ASSERT (data->xml);

//  GtkWidget* widget =
//      glade_xml_get_widget(data->XML,
//                           ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_SERVER_PART_NAME));
//  ACE_ASSERT(widget);
//  // raise dialog window
//  GdkWindow* toplevel = gtk_widget_get_parent_window(widget);
//  ACE_ASSERT(toplevel);
//  gdk_window_deiconify(toplevel);
//  // emit a signal...
//  gtk_button_clicked(GTK_BUTTON(widget));

  if (data->opengl_refresh_timer_id)
    g_source_remove (data->opengl_refresh_timer_id);

  gtk_main_quit ();

  // one-shot action
  return FALSE; // G_SOURCE_REMOVE
}

G_MODULE_EXPORT gint
about_clicked_GTK_cb (GtkWidget* widget_in,
                      gpointer userData_in)
{
  COMMON_TRACE (ACE_TEXT ("::about_clicked_GTK_cb"));

  ACE_UNUSED_ARG (widget_in);
  Olimex_Mod_MPU6050_GtkCBData_t* data =
      static_cast<Olimex_Mod_MPU6050_GtkCBData_t*> (userData_in);
  ACE_ASSERT (data);

  // sanity check(s)
  ACE_ASSERT (data->xml);

  // retrieve about dialog handle
  GtkWidget* about_dialog =
      GTK_WIDGET (glade_xml_get_widget (data->xml,
                                        ACE_TEXT_ALWAYS_CHAR (UI_WIDGET_NAME_DIALOG_ABOUT)));
  ACE_ASSERT (about_dialog);
  if (!about_dialog)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to glade_xml_get_widget(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT_ALWAYS_CHAR (UI_WIDGET_NAME_DIALOG_ABOUT)));

    return TRUE; // propagate
  } // end IF

  // draw it
  if (!GTK_WIDGET_VISIBLE (about_dialog))
    gtk_widget_show_all (about_dialog);

  return FALSE;
}

G_MODULE_EXPORT gint
quit_clicked_GTK_cb (GtkWidget* widget_in,
                     gpointer userData_in)
{
  COMMON_TRACE (ACE_TEXT ("::quit_clicked_GTK_cb"));

  ACE_UNUSED_ARG (widget_in);
//   ACE_UNUSED_ARG(userData_in);
  Olimex_Mod_MPU6050_GtkCBData_t* data =
      static_cast<Olimex_Mod_MPU6050_GtkCBData_t*> (userData_in);
  ACE_ASSERT (data);

//  // interrupt SDL event loop
//  SDL_Event sdl_event;
//  sdl_event.type = SDL_QUIT;
//  if (SDL_PushEvent (&sdl_event))
//    ACE_DEBUG ((LM_ERROR,
//                ACE_TEXT ("failed to SDL_PushEvent(): \"%s\", continuing\n"),
//                ACE_TEXT (SDL_GetError ())));

  pid_t pid = ACE_OS::getpid ();
  int result = -1;
  result = ACE_OS::kill (pid, SIGINT);
  if (result == -1)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to kill(%d, SIGINT): \"%m\", continuing\n"),
                pid));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("leaving GTK...\n")));

  // this is the "delete-event" handler
  // --> destroy the main dialog widget
  return TRUE; // --> propagate
}

//G_MODULE_EXPORT gint
//character_repository_combobox_changed_GTK_cb(GtkWidget* widget_in,
//                                             gpointer userData_in)
//{
//  RPG_TRACE(ACE_TEXT("::character_repository_combobox_changed_GTK_cb"));

//  RPG_Client_GTK_CBData_t* data =
//      static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
//  ACE_ASSERT(data);

//  // sanity check(s)
//  ACE_ASSERT(widget_in);
//  ACE_ASSERT(data->XML);

//  // retrieve active item
//  std::string active_item;
//  GtkTreeIter selected;
//  GtkTreeModel* model = NULL;
//  GValue value;
//  const gchar* text = NULL;
//	GtkVBox* vbox =
//		GTK_VBOX(glade_xml_get_widget(data->XML,
//		                              ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_VBOX_TOOLS_NAME)));
//	ACE_ASSERT(vbox);
//	GtkFrame* frame =
//		GTK_FRAME(glade_xml_get_widget(data->XML,
//		                               ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_FRAME_CHARACTER_NAME)));
//	ACE_ASSERT(frame);
//  if (!gtk_combo_box_get_active_iter(GTK_COMBO_BOX(widget_in), &selected))
//  {
//    // *WARNING*: refreshing the combobox triggers removal of items
//    // which also generates this signal...

//		// clean up
//		::reset_character_profile(data->XML);
//		GtkImage* image =
//			GTK_IMAGE(glade_xml_get_widget(data->XML,
//			                               ACE_TEXT_ALWAYS_CHAR("image_sprite")));
//		ACE_ASSERT(image);
//		gtk_image_clear(image);
//		// desensitize tools vbox
//		gtk_widget_set_sensitive(GTK_WIDGET(vbox), FALSE);
//		// remove character frame widget
//		gtk_widget_set_sensitive(GTK_WIDGET(frame), FALSE);
//		gtk_widget_hide(GTK_WIDGET(frame));

//    return FALSE;
//  } // end IF
//  model = gtk_combo_box_get_model(GTK_COMBO_BOX(widget_in));
//  ACE_ASSERT(model);
//  ACE_OS::memset(&value, 0, sizeof(value));
//  gtk_tree_model_get_value(model, &selected,
//                           0, &value);
//  text = g_value_get_string(&value);
//  // sanity check
//  ACE_ASSERT(text);
//  active_item = text;
//  g_value_unset(&value);

//  // clean up
//  if (data->entity.character)
//  {
//    delete data->entity.character;
//    data->entity.character = NULL;
//    data->entity.position =
//        std::make_pair(std::numeric_limits<unsigned int>::max(),
//                       std::numeric_limits<unsigned int>::max());
//    data->entity.modes.clear();
//    data->entity.actions.clear();
//    data->entity.is_spawned = false;
//  } // end IF

//  // construct filename
//  std::string profiles_directory =
//      RPG_Player_Common_Tools::getPlayerProfilesDirectory();
//  std::string filename = profiles_directory;
//  filename += ACE_DIRECTORY_SEPARATOR_CHAR_A;
//  filename += active_item;
//  filename += ACE_TEXT_ALWAYS_CHAR(RPG_PLAYER_PROFILE_EXT);

//  // load player profile
//  RPG_Character_Conditions_t condition;
//  condition.insert(CONDITION_NORMAL);
//  short int hitpoints = std::numeric_limits<short int>::max();
//  RPG_Magic_Spells_t spells;
//  data->entity.character = RPG_Player::load(filename,
//                                            data->schema_repository,
//                                            condition,
//                                            hitpoints,
//                                            spells);
//  if (!data->entity.character)
//  {
//    ACE_DEBUG((LM_ERROR,
//               ACE_TEXT("failed to RPG_Player::load(\"%s\"), aborting\n"),
//               ACE_TEXT(filename.c_str())));

//    return FALSE;
//  } // end IF

//  // update entity profile widgets
//  ::update_entity_profile(data->entity,
//                          data->XML);

//	// sensitize tools vbox
//	gtk_widget_set_sensitive(GTK_WIDGET(vbox), TRUE);
//  // make character frame visible/sensitive (if it's not already)
//	gtk_widget_show(GTK_WIDGET(frame));
//	gtk_widget_set_sensitive(GTK_WIDGET(frame), TRUE);

//  // make join button sensitive IFF player is not disabled
//  GtkButton* button =
//      GTK_BUTTON(glade_xml_get_widget(data->XML,
//                                      ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_SERVER_JOIN_NAME)));
//  ACE_ASSERT(button);
//  if (!RPG_Engine_Common_Tools::isCharacterDisabled(data->entity.character))
//    gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

//  // make equip button sensitive (if it's not already)
//  button = GTK_BUTTON(glade_xml_get_widget(data->XML,
//                                           ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_EQUIP_NAME)));
//  ACE_ASSERT(button);
//  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

//  return FALSE;
//}

//G_MODULE_EXPORT gint
//character_repository_button_clicked_GTK_cb(GtkWidget* widget_in,
//                                           gpointer userData_in)
//{
//  RPG_TRACE(ACE_TEXT("::character_repository_button_clicked_GTK_cb"));

//  ACE_UNUSED_ARG(widget_in);
//  RPG_Client_GTK_CBData_t* data =
//      static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
//  ACE_ASSERT(data);

//  // sanity check(s)
//  ACE_ASSERT(data->XML);

//  // retrieve tree model
//  GtkComboBox* repository_combobox =
//      GTK_COMBO_BOX(glade_xml_get_widget(data->XML,
//                                         ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_COMBOBOX_CHARACTER_NAME)));
//  ACE_ASSERT(repository_combobox);
//  GtkTreeModel* model = gtk_combo_box_get_model(repository_combobox);
//  ACE_ASSERT(model);

//  // re-load profile data
//  unsigned int num_entries =
//      ::load_files(REPOSITORY_PROFILES,
//                   GTK_LIST_STORE(model));

//  // set sensitive as appropriate
//  GtkFrame* character_frame =
//      GTK_FRAME(glade_xml_get_widget(data->XML,
//                                     ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_FRAME_CHARACTER_NAME)));
//  ACE_ASSERT(character_frame);

//  // ... sensitize/activate widgets as appropriate
//  if (num_entries)
//  {
//    gtk_widget_set_sensitive(GTK_WIDGET(repository_combobox), TRUE);
//    gtk_combo_box_set_active(repository_combobox, 0);
//  } // end IF
//  else
//  {
//    gtk_widget_set_sensitive(GTK_WIDGET(repository_combobox), FALSE);
//    gtk_widget_set_sensitive(GTK_WIDGET(character_frame), FALSE);

//    // make create button sensitive (if it's not already)
//    GtkButton* button =
//        GTK_BUTTON(glade_xml_get_widget(data->XML,
//                                        ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_CREATE_NAME)));
//    ACE_ASSERT(button);
//    gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);

//    // make load button sensitive (if it's not already)
//    button = GTK_BUTTON(glade_xml_get_widget(data->XML,
//                                             ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_LOAD_NAME)));
//    ACE_ASSERT(button);
//    gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);
//  } // end ELSE

//  return FALSE;
//}

// G_MODULE_EXPORT gint
// do_SDLEventLoop_GTK_cb(gpointer userData_in)
// {
//   RPG_TRACE(ACE_TEXT("::do_SDLEventLoop_GTK_cb"));
//
//   RPG_Client_GTK_CBData_t* data = static_cast<RPG_Client_GTK_CBData_t*> (userData_in);
//   ACE_ASSERT(data);
//
//   SDL_Event event;
//   RPG_Graphics_Position_t mouse_position = std::make_pair(0, 0);
//   RPG_Graphics_IWindow* window = NULL;
//   bool need_redraw = false;
//   bool done = false;
// //   while (SDL_WaitEvent(&event) > 0)
//   if (SDL_PollEvent(&event))
//   {
//     // if necessary, reset hover_time
//     if (event.type != RPG_GRAPHICS_SDL_HOVEREVENT)
//     {
//       // synch access
//       ACE_Guard<ACE_Thread_Mutex> aGuard(data->hover_quit_lock);
//
//       data->hover_time = 0;
//     } // end IF
//
//     switch (event.type)
//     {
//       case SDL_KEYDOWN:
//       {
//         switch (event.key.keysym.sym)
//         {
//           case SDLK_m:
//           {
//             std::string dump_path = RPG_MAP_DUMP_DIR;
//             dump_path += ACE_DIRECTORY_SEPARATOR_CHAR_A;
//             dump_path += ACE_TEXT("map.txt");
//             if (!RPG_Map_Common_Tools::save(dump_path,         // file
//                                             data->seed_points, // seed points
//                                             data->plan))       // plan
//             {
//               ACE_DEBUG((LM_ERROR,
//                          ACE_TEXT("failed to RPG_Map_Common_Tools::save(\"%s\"), aborting\n"),
//                          dump_path.c_str()));
//
//               done = true;
//             } // end IF
//
//             break;
//           }
//           case SDLK_q:
//           {
//             // finished event processing
//             done = true;
//
//             break;
//           }
//           default:
//           {
//             break;
//           }
//         } // end SWITCH
//
//         if (done)
//           break; // leave
//         // *WARNING*: falls through !
//       }
//       case SDL_ACTIVEEVENT:
//       case SDL_MOUSEMOTION:
//       case SDL_MOUSEBUTTONDOWN:
//       case RPG_GRAPHICS_SDL_HOVEREVENT: // hovering...
//       {
//         // find window
//         switch (event.type)
//         {
//           case SDL_MOUSEMOTION:
//             mouse_position = std::make_pair(event.motion.x,
//                                             event.motion.y); break;
//           case SDL_MOUSEBUTTONDOWN:
//             mouse_position = std::make_pair(event.button.x,
//                                             event.button.y); break;
//           default:
//           {
//             int x,y;
//             SDL_GetMouseState(&x, &y);
//             mouse_position = std::make_pair(x, y);
//
//             break;
//           }
//         } // end SWITCH
//
//         window = data->main_window->getWindow(mouse_position);
//         ACE_ASSERT(window);
//
//         // notify previously "active" window upon losing "focus"
//         if (event.type == SDL_MOUSEMOTION)
//         {
//           if (data->previous_window &&
//               // (data->previous_window != mainWindow)
//               (data->previous_window != window))
//           {
//             event.type = RPG_GRAPHICS_SDL_MOUSEMOVEOUT;
//             try
//             {
//               data->previous_window->handleEvent(event,
//                                                  data->previous_window,
//                                                  need_redraw);
//             }
//             catch (...)
//             {
//               ACE_DEBUG((LM_ERROR,
//                          ACE_TEXT("caught exception in RPG_Graphics_IWindow::handleEvent(), continuing\n")));
//             }
//             event.type = SDL_MOUSEMOTION;
//           } // end IF
//         } // end IF
//         // remember last "active" window
//         data->previous_window = window;
//
//         // notify "active" window
//         try
//         {
//           window->handleEvent(event,
//                               window,
//                               need_redraw);
//         }
//         catch (...)
//         {
//           ACE_DEBUG((LM_ERROR,
//                      ACE_TEXT("caught exception in RPG_Graphics_IWindow::handleEvent(), continuing\n")));
//         }
//
//         break;
//       }
//       case SDL_QUIT:
//       {
//         // finished event processing
//         done = true;
//
//         break;
//       }
//       case SDL_KEYUP:
//       case SDL_MOUSEBUTTONUP:
//       case SDL_JOYAXISMOTION:
//       case SDL_JOYBALLMOTION:
//       case SDL_JOYHATMOTION:
//       case SDL_JOYBUTTONDOWN:
//       case SDL_JOYBUTTONUP:
//       case SDL_SYSWMEVENT:
//       case SDL_VIDEORESIZE:
//       case SDL_VIDEOEXPOSE:
//       case RPG_CLIENT_SDL_TIMEREVENT:
//       default:
//       {
//
//         break;
//       }
//     } // end SWITCH
//
//         // redraw map ?
//     if (need_redraw)
//     {
//       try
//       {
//         data->map_window->draw();
//         data->map_window->refresh();
//       }
//       catch (...)
//       {
//         ACE_DEBUG((LM_ERROR,
//                    ACE_TEXT("caught exception in RPG_Graphics_IWindow::draw()/refresh(), continuing\n")));
//       }
//     } // end IF
//
//     // redraw cursor ?
//     switch (event.type)
//     {
//       case SDL_KEYDOWN:
//       case SDL_MOUSEBUTTONDOWN:
//       {
//         // map hasn't changed --> no need to redraw
//         if (!need_redraw)
//           break;
//
//         // *WARNING*: falls through !
//       }
//       case SDL_MOUSEMOTION:
//       case RPG_GRAPHICS_SDL_HOVEREVENT:
//       {
//         // map has changed, cursor MAY have been drawn over...
//         // --> redraw cursor
//         SDL_Rect dirtyRegion;
//         RPG_GRAPHICS_CURSOR_SINGLETON::instance()->put(mouse_position.first,
//                                                        mouse_position.second,
//                                                        data->screen,
//                                                        dirtyRegion);
//         RPG_Graphics_Surface::update(dirtyRegion,
//                                      data->screen);
//
//         break;
//       }
//       default:
//       {
//         break;
//       }
//     } // end SWITCH
//
//     if (done)
//     {
//       if (!SDL_RemoveTimer(data->event_timer))
//         ACE_DEBUG((LM_ERROR,
//                    ACE_TEXT("failed to SDL_RemoveTimer(): \"%s\", continuing\n"),
//                    SDL_GetError()));
//
//       // leave GTK
//       gtk_main_quit();
//
//       ACE_DEBUG((LM_DEBUG,
//                  ACE_TEXT("leaving...\n")));
//
//       // quit idle task
//       return 0;
//     } // end IF
//   } // end IF
//
//   // continue idle task
//   return 1;
// }

// G_MODULE_EXPORT gboolean
// gtk_quit_handler_cb(gpointer userData_in)
// {
//   RPG_TRACE(ACE_TEXT("::gtk_quit_handler_cb"));
//
//   RPG_Client_GTK_CBData_t* data = static_cast<RPG_Client_GTK_CBData_t*>(userData_in);
//   ACE_ASSERT(data);
//
//   // synch access
//   {
//     ACE_Guard<ACE_Thread_Mutex> aGuard(data->hover_quit_lock);
//
//     ACE_ASSERT(!data->GTK_done);
//     data->GTK_done = true;
//   } // end lock scope
//
//   // de-register this hook
//   return FALSE;
// }

#ifdef __cplusplus
}
#endif /* __cplusplus */

gboolean
delete_event_cb (GtkWidget* widget_in,
                 GdkEvent* event_in,
                 gpointer userData_in)
{
  COMMON_TRACE (ACE_TEXT ("::delete_event_cb"));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("received delete event...\n")));

  return FALSE;
}

gboolean
expose_cb (GtkWidget* widget_in,
           GdkEventExpose* event_in,
           gpointer userData_in)
{
  COMMON_TRACE (ACE_TEXT ("::expose_cb"));

  // sanity check(s)
  ACE_ASSERT (widget_in);

  GdkGLContext* opengl_context = gtk_widget_get_gl_context (widget_in);
  GdkGLDrawable* opengl_drawable = gtk_widget_get_gl_drawable (widget_in);

  if (!gdk_gl_drawable_gl_begin (opengl_drawable,
                                 opengl_context))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gdk_gl_drawable_gl_begin(), aborting\n")));

    return FALSE;
  } // end IF

  /* draw in here */
  glClearColor (0.0, 0.0, 0.0, 0.0);
  glClear (GL_COLOR_BUFFER_BIT |
           GL_DEPTH_BUFFER_BIT);
  glColor3f (1.0, 1.0, 1.0); /* set colour to white */

  const gboolean SOLID = FALSE; /* toggle if you don't want wireframe */
  const gdouble SCALE = 0.5;
  gdk_gl_draw_teapot (SOLID, SCALE);

  /* swap buffer if we're using double-buffering */
  if (gdk_gl_drawable_is_double_buffered (opengl_drawable))
    gdk_gl_drawable_swap_buffers (opengl_drawable);
  else
  {
    /* all programs should call glFlush whenever they count on having all of
     * their previously issued commands completed. */
    glFlush ();
  } // end ELSE

  gdk_gl_drawable_gl_end (opengl_drawable);

  return TRUE;
}

gboolean
idle_cb (gpointer userData_in)
{
  COMMON_TRACE (ACE_TEXT ("::idle_cb"));

  // sanity check(s)
  ACE_ASSERT (userData_in);

  /* update control data/params  in this function if needed */

  /* invalidate drawing area, marking it "dirty" and to be redrawn when main
   *  loop signals expose-events, which it does as needed when it returns */
  GtkWidget* drawing_area = GTK_WIDGET (userData_in);
  GtkAllocation allocation;
  gtk_widget_get_allocation (drawing_area, &allocation);
  gdk_window_invalidate_rect (gtk_widget_get_root_window (drawing_area),
                              &allocation,
                              FALSE);

  return TRUE;
}

gboolean
configure_cb (GtkWidget* widget_in,
              GdkEventConfigure* event_in,
              gpointer userData_in)
{
  COMMON_TRACE (ACE_TEXT ("::configure_cb"));

  // sanity check(s)
  ACE_ASSERT (widget_in);

  GdkGLContext* opengl_context = gtk_widget_get_gl_context (widget_in);
  ACE_ASSERT (opengl_context);
  GdkGLDrawable* opengl_drawable = gtk_widget_get_gl_drawable (widget_in);
  ACE_ASSERT (opengl_drawable);

  if (!gdk_gl_drawable_gl_begin (opengl_drawable,
                                 opengl_context))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gdk_gl_drawable_gl_begin(), aborting\n")));

    return FALSE;
  } // end IF

  /* specify the lower left corner, as well as width/height of the viewport */
  GtkAllocation allocation;
  gtk_widget_get_allocation (widget_in, &allocation);
  glViewport (0, 0, allocation.width, allocation.height);

  const static GLfloat light0_position[] = {1.0, 1.0, 1.0, 0.0};
  glLightfv (GL_LIGHT0, GL_POSITION, light0_position);
  glEnable (GL_DEPTH_TEST);
  glEnable (GL_LIGHTING);
  glEnable (GL_LIGHT0);

  gdk_gl_drawable_gl_end (opengl_drawable);

  return TRUE;
}

bool
initialize_UI_client (int argc_in,
                      ACE_TCHAR** argv_in,
                      const std::string& UIFile_in,
                      Olimex_Mod_MPU6050_GtkCBData_t& GtkCBData_in)
{
  COMMON_TRACE (ACE_TEXT ("::initialize_UI_client"));

  // sanity check(s)
  if (!Common_File_Tools::isReadable (UIFile_in.c_str ()))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("UI definition file \"%s\" doesn't exist, aborting\n"),
                ACE_TEXT (UIFile_in.c_str ())));

    return false;
  } // end IF

  // step1: load widget tree
  ACE_ASSERT (GtkCBData_in.xml == NULL);
  GtkCBData_in.xml = glade_xml_new (UIFile_in.c_str (), // definition file
                                    NULL,               // root widget --> construct all
                                    NULL);              // domain
  if (!GtkCBData_in.xml)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to glade_xml_new(\"%s\"): \"%m\", aborting\n"),
                ACE_TEXT (UIFile_in.c_str ())));

    return false;
  } // end IF

  GtkWindow* main_window = GTK_WINDOW (glade_xml_get_widget (GtkCBData_in.xml,
                                                             ACE_TEXT_ALWAYS_CHAR (UI_WIDGET_NAME_WINDOW_MAIN)));
  ACE_ASSERT (main_window);

  GtkDialog* about_dialog = GTK_DIALOG (glade_xml_get_widget (GtkCBData_in.xml,
                                                              ACE_TEXT_ALWAYS_CHAR (UI_WIDGET_NAME_DIALOG_ABOUT)));
  ACE_ASSERT (about_dialog);

  GtkWidget* opengl_container = GTK_WIDGET (glade_xml_get_widget (GtkCBData_in.xml,
                                                                  ACE_TEXT_ALWAYS_CHAR (UI_WIDGET_NAME_OPENGL_CONTAINER)));
  ACE_ASSERT (opengl_container);

  gtk_window_set_default_size (main_window,
                               DEFAULT_UI_WIDGET_WINDOW_MAIN_SIZE_WIDTH,
                               DEFAULT_UI_WIDGET_WINDOW_MAIN_SIZE_HEIGHT);

  GtkWidget* drawing_area = gtk_drawing_area_new ();
  ACE_ASSERT (drawing_area);
  gtk_widget_set_events (drawing_area,
                         GDK_EXPOSURE_MASK);
  gtk_container_add (GTK_CONTAINER (opengl_container), drawing_area);

  // step2: (auto-)connect signals/slots
  // *NOTE*: glade_xml_signal_autoconnect doesn't work reliably
  //glade_xml_signal_autoconnect(xml);
  // step2a: connect default signals
  // *NOTE*: glade_xml_signal_connect_data doesn't work reliably
//  g_signal_connect_swapped (G_OBJECT (main_window),
//                            ACE_TEXT_ALWAYS_CHAR ("destroy"),
//                            G_CALLBACK (gtk_main_quit),
//                            NULL);
  g_signal_connect (G_OBJECT (main_window),
                    ACE_TEXT_ALWAYS_CHAR ("destroy"),
                    G_CALLBACK (quit_clicked_GTK_cb),
                    &GtkCBData_in);
//                   G_CALLBACK(gtk_widget_destroyed),
//                   &main_dialog,
  g_signal_connect (G_OBJECT (main_window),
                    ACE_TEXT_ALWAYS_CHAR ("delete-event"),
                    G_CALLBACK (delete_event_cb),
                    NULL);
  GtkWidget* widget = GTK_WIDGET (glade_xml_get_widget (GtkCBData_in.xml,
                                                        ACE_TEXT_ALWAYS_CHAR (UI_WIDGET_NAME_MENU_FILE_QUIT)));
  ACE_ASSERT (widget);
  g_signal_connect (G_OBJECT (widget),
                    ACE_TEXT_ALWAYS_CHAR ("activate"),
                    G_CALLBACK (quit_clicked_GTK_cb),
                    &GtkCBData_in);

  widget = GTK_WIDGET (glade_xml_get_widget (GtkCBData_in.xml,
                                             ACE_TEXT_ALWAYS_CHAR (UI_WIDGET_NAME_MENU_HELP_ABOUT)));
  ACE_ASSERT (widget);
  g_signal_connect (G_OBJECT (widget),
                    ACE_TEXT_ALWAYS_CHAR ("activate"),
                    G_CALLBACK (about_clicked_GTK_cb),
                    &GtkCBData_in);
  g_signal_connect (G_OBJECT (about_dialog),
                    ACE_TEXT_ALWAYS_CHAR ("response"),
                    G_CALLBACK (gtk_widget_hide),
                    &about_dialog);

  // step3: initialize OpenGL
  if (!gtk_gl_init_check (&argc_in, &argv_in))
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_gl_init_check(): \"%m\", aborting\n")));

    return false;
  } // end IF

  GdkGLConfigMode mode = static_cast<GdkGLConfigMode> (GDK_GL_MODE_RGBA  |
                                                       GDK_GL_MODE_DEPTH |
                                                       GDK_GL_MODE_DOUBLE);
  GdkGLConfig* configuration = gdk_gl_config_new_by_mode (mode);
  if (!configuration)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gdk_gl_config_new_by_mode(): \"%m\", aborting\n")));

    return false;
  } // end IF

  if (!gtk_widget_set_gl_capability (drawing_area,      // (container) widget
                                     configuration,     // GdkGLConfig: configuration
                                     NULL,              // GdkGLContext: share list
                                     TRUE,              // direct rendering ?
                                     GDK_GL_RGBA_TYPE)) // render_type
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_widget_set_gl_capability(): \"%m\", aborting\n")));

    // clean up
    g_free (configuration);

    return false;
  } // end IF

  guint opengl_refresh_rate = static_cast<guint> (UI_WIDGET_GL_REFRESH_INTERVAL);
  GtkCBData_in.opengl_refresh_timer_id = g_timeout_add (opengl_refresh_rate,
                                                        idle_cb,
                                                        drawing_area);
  if (!GtkCBData_in.opengl_refresh_timer_id)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to gtk_widget_set_gl_capability(): \"%m\", aborting\n")));

    // clean up
    g_free (configuration);

    return false;
  } // end IF

  //   // step4: connect custom signals
  //  GtkButton* button = NULL;
  //  button = GTK_BUTTON(glade_xml_get_widget(userData_in.XML,
  //                                           ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_BUTTON_CREATE_NAME)));
  //  ACE_ASSERT(button);
  //  g_signal_connect(button,
  //                   ACE_TEXT_ALWAYS_CHAR("clicked"),
  //                   G_CALLBACK(create_character_clicked_GTK_cb),
  //                   userData_p);

  //  combobox =
  //      GTK_COMBO_BOX(glade_xml_get_widget(userData_in.XML,
  //                                         ACE_TEXT_ALWAYS_CHAR(RPG_CLIENT_GTK_COMBOBOX_CHARACTER_NAME)));
  //  ACE_ASSERT(combobox);
  //  g_signal_connect(combobox,
  //                   ACE_TEXT_ALWAYS_CHAR("changed"),
  //                   G_CALLBACK(character_repository_combobox_changed_GTK_cb),
  //                   userData_p);
  g_signal_connect (drawing_area,
                    ACE_TEXT_ALWAYS_CHAR ("configure-event"),
                    G_CALLBACK (configure_cb),
                    NULL);
  g_signal_connect (drawing_area,
                    ACE_TEXT_ALWAYS_CHAR ("expose-event"),
                    G_CALLBACK (expose_cb),
                    NULL);

  // step5: use correct screen
//   if (parentWidget_in)
//     gtk_window_set_screen(GTK_WINDOW(dialog),
//                           gtk_widget_get_screen(const_cast<GtkWidget*>(//parentWidget_in)));

  // step6: draw main window
  gtk_widget_show_all (GTK_WIDGET (main_window));

  // step7: schedule UI initialization
  guint event_source_id = g_idle_add (idle_initialize_UI_cb,
                                      &GtkCBData_in);
  if (event_source_id == 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to g_idle_add(): \"%m\", aborting\n")));

    return false;
  } // end IF
  GtkCBData_in.event_source_ids.push_back (event_source_id);

  return true;
}

void
finalize_UI_client (const Olimex_Mod_MPU6050_GtkCBData_t& GtkCBData_in)
{
  COMMON_TRACE (ACE_TEXT ("::finalize_UI_client"));

  // schedule UI finalization
  gpointer userData_p =
      const_cast<Olimex_Mod_MPU6050_GtkCBData_t*> (&GtkCBData_in);
  guint event_source_id = g_idle_add (idle_finalize_UI_cb,
                                      userData_p);
  if (event_source_id == 0)
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to g_idle_add(): \"%m\", continuing\n")));
}

Olimex_Mod_MPU6050_GTKUIDefinition::Olimex_Mod_MPU6050_GTKUIDefinition (int argc_in,
                                                                        ACE_TCHAR** argv_in,
                                                                        Olimex_Mod_MPU6050_GtkCBData_t* GTKCBData_in)
 : argc_ (argc_in)
 , argv_ (argv_in)
 , GTKCBData_ (GTKCBData_in)
{
  COMMON_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_GTKUIDefinition::Olimex_Mod_MPU6050_GTKUIDefinition"));

  ACE_ASSERT (GTKCBData_);
}

Olimex_Mod_MPU6050_GTKUIDefinition::~Olimex_Mod_MPU6050_GTKUIDefinition ()
{
  COMMON_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_GTKUIDefinition::~Olimex_Mod_MPU6050_GTKUIDefinition"));

}

bool
Olimex_Mod_MPU6050_GTKUIDefinition::initialize (const std::string& filename_in)
{
  COMMON_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_GTKUIDefinition::initialize"));

  return initialize_UI_client (argc_,
                               argv_,
                               filename_in,
                               *GTKCBData_);
}

void
Olimex_Mod_MPU6050_GTKUIDefinition::finalize ()
{
  COMMON_TRACE (ACE_TEXT ("Olimex_Mod_MPU6050_GTKUIDefinition::finalize"));

  finalize_UI_client (*GTKCBData_);
}
