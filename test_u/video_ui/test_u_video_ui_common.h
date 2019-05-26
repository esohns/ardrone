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

#ifndef TEST_U_VIDEO_UI_COMMON_H
#define TEST_U_VIDEO_UI_COMMON_H

#include "common_test_u_common.h"

#include "test_u_video_ui_network_common.h"
#include "test_u_video_ui_stream_common.h"

enum Test_U_ProgramMode
{
  TEST_U_PROGRAMMODE_PRINT_VERSION = 0,
  //TEST_U_PROGRAMMODE_TEST_METHODS,
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
   , dispatchConfiguration ()
   , streamConfiguration ()
#if defined (ACE_WIN32) || defined (ACE_WIN64)
   , direct3DConfiguration ()
#endif // ACE_WIN32 || ACE_WIN64
  {}

  Stream_AllocatorConfiguration     allocatorConfiguration;
  Net_ConnectionConfigurations_t    connectionConfigurations;
  Common_EventDispatchConfiguration dispatchConfiguration;
  // **************************** stream data **********************************
  Test_U_StreamConfiguration_t      streamConfiguration;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  struct Stream_MediaFramework_Direct3D_Configuration direct3DConfiguration;
#endif // ACE_WIN32 || ACE_WIN64
};

#endif
