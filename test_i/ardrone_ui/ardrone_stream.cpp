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
#include "ardrone_sessionmessage.h"
#include "ardrone_stream.h"

//#include "stream_misc_defines.h"

#include "ardrone_defines.h"

const char video_stream_name_string_[] =
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_VIDEO_STREAM_NAME_STRING);
const char control_stream_name_string_[] =
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_CONTROL_STREAM_NAME_STRING);
const char navdata_stream_name_string_[] =
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_NAVDATA_STREAM_NAME_STRING);
const char mavlink_stream_name_string_[] =
    ACE_TEXT_ALWAYS_CHAR (ARDRONE_MAVLINK_STREAM_NAME_STRING);
