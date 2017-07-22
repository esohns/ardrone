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

#include "ace/config-lite.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <mfobjects.h>
#include <strmif.h>
#endif

#include "mavlink.h"

#include "navdata_common.h"

#include "stream_data_base.h"

#include "net_iparser.h"
#include "net_iwlanmonitor.h"

#include "ardrone_statemachine_navdata.h"
#include "ardrone_types.h"

typedef std::vector<unsigned int> ARDrone_NavDataOptionOffsets_t;
typedef ARDrone_NavDataOptionOffsets_t::const_iterator ARDrone_NavDataOptionOffsetsIterator_t;
struct ARDrone_NavData
{
  ARDrone_NavData ()
   : NavData ()
   , NavDataOptionOffsets ()
  {
    ACE_OS::memset (&NavData, 0, sizeof (struct _navdata_t));
    NavDataOptionOffsets.reserve (10);
  };

  // *IMPORTANT NOTE*: the options are not parsed; use the offsets
  struct _navdata_t              NavData;
  ARDrone_NavDataOptionOffsets_t NavDataOptionOffsets;
};

struct ARDrone_VideoFrame
{
  ARDrone_VideoFrame ()
#if defined (ACE_WIN32) || defined (ACE_WIN64)
   : sample (NULL)
   , sampleTime (0.0)
#endif
  {};

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  IMediaSample* sample;
  double        sampleTime;
  //IMFSample*    sample;
  //LONGLONG      sampleTime;
#endif
};

struct ARDrone_MessageData
{
  ARDrone_MessageData ()
   : messageType (ARDRONE_MESSAGE_INVALID)
  {};
  ARDrone_MessageData (const ARDrone_MessageData& data_in)
  {
    messageType = data_in.messageType;
    switch (messageType)
    {
      case ARDRONE_MESSAGE_NAVDATAMESSAGE:
        NavData = data_in.NavData;
        break;
      case ARDRONE_MESSAGE_MAVLINKMESSAGE:
        MAVLinkData = data_in.MAVLinkData;
        break;
      case ARDRONE_MESSAGE_VIDEOFRAME:
        videoFrame = data_in.videoFrame;
        break;
      default:
      {
        ACE_DEBUG ((LM_ERROR,
                    ACE_TEXT ("invalid/unknown message data type (was: %d), continuing\n"),
                    messageType));
        break;
      }
    } // end SWITCH
  };
  ~ARDrone_MessageData ()
  {
    switch (messageType)
    {
      case ARDRONE_MESSAGE_NAVDATAMESSAGE:
        break;
      case ARDRONE_MESSAGE_MAVLINKMESSAGE:
        break;
      case ARDRONE_MESSAGE_VIDEOFRAME:
      {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
        if (videoFrame.sample)
          videoFrame.sample->Release ();
#endif
        break;
      }
      default:
        break;
    } // end SWITCH
    messageType = ARDRONE_MESSAGE_INVALID;
  };
  inline void operator+= (struct ARDrone_MessageData rhs_in)
  { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) };

  enum ARDrone_MessageType  messageType;

  struct __mavlink_message  MAVLinkData;
  struct ARDrone_NavData    NavData;
  struct ARDrone_VideoFrame videoFrame;
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

  virtual void addOption (unsigned int) = 0; // offset
};

class ARDrone_IMAVLinkNotify
{
 public:
  virtual void messageCB (const struct __mavlink_message&, // message record
                          void*) = 0;                      // payload handle
};
class ARDrone_INavDataNotify
{
 public:
  virtual void messageCB (const struct _navdata_t&,              // message record
                          const ARDrone_NavDataOptionOffsets_t&, // option offsets
                          void*) = 0;                            // payload handle
};

class ARDrone_IController
 : virtual public ARDrone_IStateMachine_NavData_t
{
 public:
  virtual void ids (uint8_t,      // session id
                    uint8_t,      // user id
                    uint8_t) = 0; // application id

  virtual void init () = 0; // send initial packet
  virtual void start () = 0; // switch from 'bootstrap' to 'demo' mode
  virtual void resetWatchdog () = 0; // reset com watchdog (every 50ms)

  virtual void trim () = 0;

  virtual void takeoff () = 0;
  virtual void land () = 0;

  virtual void set (enum ARDrone_VideoMode) = 0;
};

//////////////////////////////////////////

//void extract_data (const char*,             // data
//                   float&, float&, float&,  // return value: accelerometer data
//                   float&,                  // return value: thermometer data
//                   float&, float&, float&); // return value: gyroscope data

#endif
