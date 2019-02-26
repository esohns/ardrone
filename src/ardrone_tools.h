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

#ifndef ARDRONE_TOOLS_H
#define ARDRONE_TOOLS_H

#include <string>

#include "common_image_common.h"

#include "ardrone_types.h"

// *IMPORTANT NOTE*: these are defined in ardrone_stream.cpp
std::string ARDroneStreamTypeToString (const enum ARDrone_StreamType);
std::string ARDroneVideoModeToString (const enum ARDrone_VideoMode);
// *TODO*: use libav here
void ARDroneVideoModeToResolution (const enum ARDrone_VideoMode,
                                   Common_Image_Resolution_t&);

#endif // #ifndef ARDRONE_TOOLS_H
