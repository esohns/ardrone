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

#ifndef TEST_U_COMMON_H
#define TEST_U_COMMON_H

#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
#include "common_ui_gtk_common.h"
#elif defined (QT_USE)
#include "common_ui_qt_common.h"
#endif // GTK_USE
#endif // GUI_SUPPORT

#include "stream_istreamcontrol.h"

#include "test_u_network_common.h"
#include "test_u_stream_common.h"

enum Test_U_ProgramMode
{
  TEST_U_PROGRAMMODE_PRINT_VERSION = 0,
  TEST_U_PROGRAMMODE_NORMAL,
  ////////////////////////////////////////
  TEST_U_PROGRAMMODE_MAX,
  TEST_U_PROGRAMMODE_INVALID
};

struct Test_U_Configuration
{
  Test_U_Configuration ()
   : allocatorConfiguration ()
   , connectionConfigurations ()
   , connectionConfigurations_2 ()
   , dispatchConfiguration ()
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
   , GTKConfiguration ()
#endif // GTK_USE
#endif // GUI_SUPPORT
   , parserConfiguration ()
   , streamConfiguration ()
   , streamConfiguration_2 ()
  {}

  struct Stream_AllocatorConfiguration     allocatorConfiguration;
  Net_ConnectionConfigurations_t           connectionConfigurations;
  Net_ConnectionConfigurations_t           connectionConfigurations_2;
  struct Common_EventDispatchConfiguration dispatchConfiguration;
#if defined (GUI_SUPPORT)
#if defined (GTK_USE)
  struct Common_UI_GTK_Configuration       GTKConfiguration;
#endif // GTK_USE
#endif // GUI_SUPPORT
  struct Common_ParserConfiguration        parserConfiguration;
  // **************************** stream data **********************************
  Test_U_StreamConfiguration_t             streamConfiguration;
  Test_U_StreamConfiguration_t             streamConfiguration_2;
};

//////////////////////////////////////////

#if defined (GUI_SUPPORT)
struct Test_U_UI_CBData
#if defined (GTK_USE)
 : Common_UI_GTK_CBData
#elif defined (QT_USE)
 : Common_UI_Qt_CBData
#endif
{
  Test_U_UI_CBData ()
#if defined (GTK_USE)
   : Common_UI_GTK_CBData ()
#elif defined (QT_USE)
   : Common_UI_Qt_CBData ()
#endif
   , configuration (NULL)
   , stream (NULL)
   , videoMode (ARDRONE_VIDEOMODE_360P)
  {}

  struct Test_U_Configuration* configuration;
  Stream_IStreamControlBase*   stream;
  enum ARDrone_VideoMode       videoMode;
};
#endif // GUI_SUPPORT

#endif
