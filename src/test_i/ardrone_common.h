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

#ifndef ARDRONE_COMMON_H
#define ARDRONE_COMMON_H

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <mfobjects.h>
#include <strmif.h>
#endif

#include <mavlink.h>

#include <navdata_common.h>

#include <ace/config-lite.h>

#include "stream_data_base.h"

#include "net_iparser.h"

struct ARDrone_MAVLinkMessageData
{
  inline ARDrone_MAVLinkMessageData ()
   : MAVLinkMessage ()
  {};

  struct __mavlink_message MAVLinkMessage;
};
typedef Stream_DataBase_T<struct ARDrone_MAVLinkMessageData> ARDrone_MAVLinkMessageData_t;
struct ARDrone_NavDataMessageData
{
  inline ARDrone_NavDataMessageData ()
   : NavDataMessage ()
  {};

  struct _navdata_t NavDataMessage;
};
typedef Stream_DataBase_T<struct ARDrone_NavDataMessageData> ARDrone_NavDataMessageData_t;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
struct ARDrone_DirectShow_LiveVideoMessageData
{
  inline ARDrone_DirectShow_LiveVideoMessageData ()
   : sample (NULL)
   , sampleTime (0)
  {};

  IMediaSample* sample;
  double        sampleTime;
};
typedef Stream_DataBase_T<struct ARDrone_DirectShow_LiveVideoMessageData> ARDrone_DirectShow_LiveVideoMessageData_t;
struct ARDrone_MediaFoundation_LiveVideoMessageData
{
  inline ARDrone_MediaFoundation_LiveVideoMessageData ()
   : sample (NULL)
   , sampleTime (0)
  {};

  IMFSample* sample;
  LONGLONG   sampleTime;
};
typedef Stream_DataBase_T<struct ARDrone_MediaFoundation_LiveVideoMessageData> ARDrone_MediaFoundation_LiveVideoMessageData_t;
#else
struct ARDrone_LiveVideoMessageData
{
  inline ARDrone_LiveVideoMessageData ()
  {};
};
typedef Stream_DataBase_T<struct ARDrone_LiveVideoMessageData> ARDrone_LiveVideoMessageData_t;
#endif

class ARDrone_MAVLink_IParser
 : public Net_IRecordParser_T<struct Common_ParserConfiguration,
                              struct __mavlink_message>
 , public Net_IScanner_T<ARDrone_MAVLink_IParser>
{
 public:
  // convenient types
  typedef Net_IRecordParser_T<struct Common_ParserConfiguration,
                              struct __mavlink_message> IPARSER_T;

  using IPARSER_T::error;

  inline virtual ~ARDrone_MAVLink_IParser () {};
};

class ARDrone_NavData_IParser
 : public Net_IRecordParser_T<struct Common_ParserConfiguration,
                              struct _navdata_t>
 , public Net_IScanner_T<ARDrone_NavData_IParser>
{
 public:
  // convenient types
  typedef Net_IRecordParser_T<struct Common_ParserConfiguration,
                              struct _navdata_t> IPARSER_T;

  using IPARSER_T::error;

  inline virtual ~ARDrone_NavData_IParser () {};
};

//////////////////////////////////////////

//void extract_data (const char*,             // data
//                   float&, float&, float&,  // return value: accelerometer data
//                   float&,                  // return value: thermometer data
//                   float&, float&, float&); // return value: gyroscope data

#endif
