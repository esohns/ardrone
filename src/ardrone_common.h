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

#include <string>

#include "ace/config-lite.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#include <mfobjects.h>
#include <strmif.h>
#else
#include <cstdint>
#endif // ACE_WIN32 || ACE_WIN64

#ifdef __cplusplus
extern "C"
{
#include "libavcodec/avcodec.h"
}
#endif /* __cplusplus */

#include "mavlink/v2.0/common/mavlink.h"

#include "Soft/Common/navdata_common.h"

#include "ace/OS.h"

#include "common.h"
#include "common_iparser.h"
#include "common_statistic_handler.h"

#include "common_parser_defines.h"

#include "stream_configuration.h"
#include "stream_data_base.h"

#include "net_wlan_imonitor.h"

#include "ardrone_statemachine_navdata.h"
#include "ardrone_types.h"

//struct ARDrone_UserData
// : Net_UserData
//{
//  ARDrone_UserData ()
//   : Net_UserData ()
//  {}
//};

struct ARDrone_NavData
{
  ARDrone_NavData ()
//   : NavData ()
   : NavDataOptionOffsets ()
  {
    ACE_OS::memset (&NavData, 0, sizeof (struct _navdata_t));
    NavDataOptionOffsets.reserve (10);
  }

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
#endif // ACE_WIN32 || ACE_WIN64
  {}

#if defined (ACE_WIN32) || defined (ACE_WIN64)
  IMediaSample* sample;
  double        sampleTime;
  //IMFSample*    sample;
  //LONGLONG      sampleTime;
#endif // ACE_WIN32 || ACE_WIN64
};

struct ARDrone_MessageData
{
  ARDrone_MessageData ()
   : messageType (ARDRONE_MESSAGE_INVALID)
  {}
  ARDrone_MessageData (const ARDrone_MessageData& data_in)
  {
    messageType = data_in.messageType;
    switch (messageType)
    {
      case ARDRONE_MESSAGE_CONTROL:
        controlData = data_in.controlData;
        break;
      case ARDRONE_MESSAGE_NAVDATA:
        NavData = data_in.NavData;
        break;
      case ARDRONE_MESSAGE_MAVLINK:
        MAVLinkData = data_in.MAVLinkData;
        break;
      case ARDRONE_MESSAGE_VIDEO:
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
  }
  virtual ~ARDrone_MessageData ()
  {
    switch (messageType)
    {
      case ARDRONE_MESSAGE_CONTROL:
        controlData.clear ();
        break;
      case ARDRONE_MESSAGE_NAVDATA:
        break;
      case ARDRONE_MESSAGE_MAVLINK:
        break;
      case ARDRONE_MESSAGE_VIDEO:
      {
#if defined (ACE_WIN32) || defined (ACE_WIN64)
        if (videoFrame.sample)
        {
          videoFrame.sample->Release (); videoFrame.sample = NULL;
        } // end IF
#endif // ACE_WIN32 || ACE_WIN64
        break;
      }
      default:
        break;
    } // end SWITCH
    messageType = ARDRONE_MESSAGE_INVALID;
  }
  inline void operator+= (struct ARDrone_MessageData rhs_in) { ACE_ASSERT (false); ACE_NOTSUP; ACE_NOTREACHED (return;) }

  enum ARDrone_MessageType      messageType;

  ARDrone_DeviceConfiguration_t controlData;
  struct __mavlink_message      MAVLinkData;
  struct ARDrone_NavData        NavData;
  struct ARDrone_VideoFrame     videoFrame;
};
typedef Stream_DataBase_T<struct ARDrone_MessageData> ARDrone_MessageData_t;

typedef Common_IYaccRecordParser_T<struct Common_ParserConfiguration,
                                   ARDrone_DeviceConfiguration_t> ARDrone_Control_IParser_t;
typedef Common_ILexScanner_T<struct Common_ScannerState,
                             ARDrone_Control_IParser_t> ARDrone_Control_IScanner_t;
class ARDrone_IControlNotify
{
 public:
  virtual void messageCB (const ARDrone_DeviceConfiguration_t&) = 0; // device configuration
};

typedef Common_IYaccStreamParser_T<struct Common_ParserConfiguration,
                                   struct __mavlink_message> ARDrone_MAVLink_IParser_t;
typedef Common_ILexScanner_T<struct Common_ScannerState,
                             ARDrone_MAVLink_IParser_t> ARDrone_MAVLink_IScanner_t;
class ARDrone_IMAVLinkNotify
{
 public:
  virtual void messageCB (const struct __mavlink_message&, // message record
                          void*) = 0;                      // payload handle
};

class ARDrone_NavData_IParser
 : public Common_IYaccStreamParser_T<struct Common_ParserConfiguration,
                                     struct _navdata_t>
{
 public:
  virtual void addOption (unsigned int) = 0; // offset
};
typedef Common_ILexScanner_T<struct Common_ScannerState,
                             ARDrone_NavData_IParser> ARDrone_NavData_IScanner_t;
class ARDrone_INavDataNotify
{
 public:
  virtual void messageCB (const struct _navdata_t&,              // message record
                          const ARDrone_NavDataOptionOffsets_t&, // option offsets
                          void*) = 0;                            // payload handle
};

class ARDrone_IDeviceConfiguration
 : public Common_IGetR_3_T<ARDrone_DeviceConfiguration_t>
 , public Common_ISetP_T<ARDrone_DeviceConfiguration_t>
{};

class ARDrone_IController
 : virtual public ARDrone_IStateMachine_NavData_t
 , public ARDrone_IDeviceConfiguration
 , public Common_IGet_T<uint32_t>
 , public Common_IGet_2_T<struct _navdata_demo_t>
{
 public:
  // *NOTE*: calibrate accelerometer (only when not (!) airborne)
  virtual void trim () = 0;
  // *NOTE*: calibrate gyroscope (only when airborne)
  virtual void calibrate () = 0;

  virtual void leds () = 0;

  // *NOTE*: dump device configuration
  virtual void dump () = 0;

  virtual void takeoff () = 0;
  virtual void land () = 0;
  virtual void reset () = 0;

  virtual void set (enum ARDrone_VideoMode) = 0;

 protected:
  virtual void ids (const std::string&,      // session id
                    const std::string&,      // user id
                    const std::string&) = 0; // application id
  virtual void init () = 0; // send initial packet
  virtual void start () = 0; // switch from 'bootstrap' to 'demo' mode
  virtual void resetWatchdog () = 0; // reset com watchdog (every 50ms)
};

typedef Common_StatisticHandler_T<struct ARDrone_Statistic> ARDrone_StatisticHandler_t;

// *TODO*: move this into ardrone_configuration.h ASAP
struct ARDrone_AllocatorConfiguration
 : Stream_AllocatorConfiguration
{
  ARDrone_AllocatorConfiguration ()
   : Stream_AllocatorConfiguration ()
  {
    defaultBufferSize = ARDRONE_MESSAGE_BUFFER_SIZE;

    // *NOTE*: facilitate (message block) data buffers to be scanned with
    //         (f)lexs' yy_scan_buffer() method, and (!) support 'padding' in
    //         ffmpeg
    paddingBytes =
#if defined (ACE_WIN32) || defined (ACE_WIN64)
        AV_INPUT_BUFFER_PADDING_SIZE;
#else
//      FF_INPUT_BUFFER_PADDING_SIZE;
        AV_INPUT_BUFFER_PADDING_SIZE;
#endif // ACE_WIN32 || ACE_WIN64
    paddingBytes =
      std::max (static_cast<unsigned int> (COMMON_PARSER_FLEX_BUFFER_BOUNDARY_SIZE),
                paddingBytes);
  }
};

//////////////////////////////////////////

//void extract_data (const char*,             // data
//                   float&, float&, float&,  // return value: accelerometer data
//                   float&,                  // return value: thermometer data
//                   float&, float&, float&); // return value: gyroscope data

#endif
