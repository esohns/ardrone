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

#include "ace/Synch.h"
#include "ardrone_stream.h"

#include "stream_misc_defines.h"

#include "ardrone_defines.h"
//#include "ardrone_modules_common.h"

const char video_stream_name_string_[] =
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING);
const char control_stream_name_string_[] =
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_CONTROL_STREAM_NAME_STRING);
const char navdata_stream_name_string_[] =
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_NAVDATA_STREAM_NAME_STRING);
const char mavlink_stream_name_string_[] =
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_MAVLINK_STREAM_NAME_STRING);

std::string 
ARDroneStreamTypeToString (const enum ARDrone_StreamType type_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::ARDroneStreamTypeToString"));

  switch (type_in)
  {
    case ARDRONE_STREAM_CONTROL:
      return ACE_TEXT_ALWAYS_CHAR (ARDRONE_CONTROL_STREAM_NAME_STRING);
    case ARDRONE_STREAM_MAVLINK:
      return ACE_TEXT_ALWAYS_CHAR (ARDRONE_MAVLINK_STREAM_NAME_STRING);
    case ARDRONE_STREAM_NAVDATA:
      return ACE_TEXT_ALWAYS_CHAR (ARDRONE_NAVDATA_STREAM_NAME_STRING);
    case ARDRONE_STREAM_VIDEO:
      return ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING);
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unkown stream type (was: %d), aborting\n"),
                  type_in));
      break;
    }
  } // end SWITCH

  return ACE_TEXT_ALWAYS_CHAR ("");
}

std::string 
ARDroneVideoModeToString (const enum ARDrone_VideoMode mode_in)
{
  ARDRONE_TRACE (ACE_TEXT ("::ARDroneVideoModeToString"));
  
  switch (mode_in)
  {
    case ARDRONE_VIDEOMODE_360P:
      return ACE_TEXT_ALWAYS_CHAR ("H264 360p");
    case ARDRONE_VIDEOMODE_720P:
      return ACE_TEXT_ALWAYS_CHAR ("H264 720p");
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unkown video mode (was: %d), aborting\n"),
                  mode_in));
      break;
    }
  } // end SWITCH

  return ACE_TEXT_ALWAYS_CHAR ("");
}
void
ARDroneVideoModeToResolution (const enum ARDrone_VideoMode mode_in,
                              unsigned int& width_out,
                              unsigned int& height_out)
{
  ARDRONE_TRACE (ACE_TEXT ("::ARDroneVideoModeToResolution"));

  // initialize return value(s)
  width_out = 0;
  height_out = 0;

  switch (mode_in)
  {
    case ARDRONE_VIDEOMODE_360P:
      width_out = ARDRONE_H264_360P_VIDEO_WIDTH;
      height_out = ARDRONE_H264_360P_VIDEO_HEIGHT;
      break;
    case ARDRONE_VIDEOMODE_720P:
      width_out = ARDRONE_H264_720P_VIDEO_WIDTH;
      height_out = ARDRONE_H264_720P_VIDEO_HEIGHT;
      break;
    default:
    {
      ACE_DEBUG ((LM_ERROR,
                  ACE_TEXT ("invalid/unkown video mode (was: %d), aborting\n"),
                  mode_in));
      break;
    }
  } // end SWITCH
}