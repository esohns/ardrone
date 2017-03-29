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

#include <vector>

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <mfobjects.h>
#include <strmif.h>
#endif

#include <mavlink.h>

#include <navdata_common.h>

#include <ace/config-lite.h>

#include "stream_data_base.h"

#include "net_iparser.h"

typedef std::vector<unsigned int> ARDrone_NavDataMessageOptionOffsets_t;
typedef ARDrone_NavDataMessageOptionOffsets_t::const_iterator ARDrone_NavDataMessageOptionOffsetsIterator_t;
struct ARDrone_MessageData
{
  inline ARDrone_MessageData ()
   : MAVLinkMessage ()
//   , NavDataMessage ()
   , NavDataMessageOptionOffsets ()
#if defined (ACE_WIN32) || defined (ACE_WIN64)
   , sample (NULL)
   , sampleTime (0.0)
//   , sampleTime (0)
#endif
  {
    ACE_OS::memset (&MAVLinkMessage, 0, sizeof (struct __mavlink_message));
    //ACE_OS::memset (&NavDataMessage, 0, sizeof (struct _navdata_t));
  };
  inline ~ARDrone_MessageData ()
  {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
    if (sample)
    {
      sample->Release ();
      sample = NULL;
    } // end IF
#endif
  };
  inline void operator+= (struct ARDrone_MessageData rhs_in)
  { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) };

  union
  {
    struct __mavlink_message            MAVLinkMessage;
    // *IMPORTANT NOTES*: the options are not parsed; use the offsets
    struct _navdata_t                   NavDataMessage;
  };
  ARDrone_NavDataMessageOptionOffsets_t NavDataMessageOptionOffsets;
#if defined (ACE_WIN32) || defined (ACE_WIN64)
  IMediaSample*                         sample;
  double                                sampleTime;
//  IMFSample*                            sample;
//  LONGLONG                              sampleTime;
#endif
};
typedef Stream_DataBase_T<struct ARDrone_MessageData> ARDrone_MessageData_t;

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

  virtual void addOption (unsigned int) = 0; // offset
};

//////////////////////////////////////////

//void extract_data (const char*,             // data
//                   float&, float&, float&,  // return value: accelerometer data
//                   float&,                  // return value: thermometer data
//                   float&, float&, float&); // return value: gyroscope data

#endif
