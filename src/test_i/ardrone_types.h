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

#ifndef ARDRONE_TYPES_H
#define ARDRONE_TYPES_H

#include <deque>
#include <list>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "ace/config-lite.h"
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#include "video_encapsulation.h"
#endif
#undef FFMPEG_SUPPORT
#include "VLIB/video_codec.h"

#include "ace/Message_Block.h"

#include "stream_common.h"

#include "net_common.h"

#include "ardrone_defines.h"

#if defined (__llvm__)
enum ARDrone_MessageType
#else
enum ARDrone_MessageType : int
#endif
{
  ARDRONE_MESSAGE_INVALID = -1,
  ARDRONE_MESSAGE_ATCOMMAND = ACE_Message_Block::MB_PROTO, // outbound navdata
  ARDRONE_MESSAGE_CONTROL,
  ARDRONE_MESSAGE_MAVLINK,
  ARDRONE_MESSAGE_NAVDATA,
  ARDRONE_MESSAGE_VIDEO,
  ///////////////////////////////////////
  ARDRONE_MESSAGE_MAX
};

#if defined (__llvm__)
enum ARDrone_StreamType
#else
enum ARDrone_StreamType : int
#endif
{
  ARDRONE_STREAM_INVALID = -1,
  ARDRONE_STREAM_CONTROL,
  ARDRONE_STREAM_MAVLINK,
  ARDRONE_STREAM_NAVDATA,
  ARDRONE_STREAM_VIDEO,
  ///////////////////////////////////////
  ARDRONE_STREAM_MAX
};

#if defined (__llvm__)
enum ARDrone_EventType
#else
enum ARDrone_EventType : int
#endif
{
  ARDRONE_EVENT_INVALID = -1,
  ARDRONE_EVENT_CONNECT,
  ARDRONE_EVENT_DISCONNECT,
  ARDRONE_EVENT_MESSAGE_DATA,
  ARDRONE_EVENT_MESSAGE_SESSION,
  ARDRONE_EVENT_RESIZE,
  ///////////////////////////////////////
  ARDRONE_EVENT_MAX
};
typedef std::deque<std::pair <enum ARDrone_StreamType,
                              enum ARDrone_EventType> > ARDrone_Events_t;
typedef ARDrone_Events_t::const_iterator ARDrone_EventsIterator_t;

//struct ARDRone_SensorBias
//{
//  gfloat ax_bias;
//  gfloat ay_bias;
//  gfloat az_bias;
//  gfloat gx_bias;
//  gfloat gy_bias;
//  gfloat gz_bias;
//};

//struct ARDrone_Camera
//{
//  //glm::vec3 position;
//  //glm::vec3 looking_at;
//  //glm::vec3 up;

//  float zoom;
//  glm::vec3 rotation;
//  glm::vec3 translation;
//  int last[2];
//};

typedef std::pair<std::string, std::string> ARDrone_DeviceConfigurationKeyValue_t;
typedef std::list<ARDrone_DeviceConfigurationKeyValue_t> ARDrone_DeviceConfigurationCategory_t;
typedef ARDrone_DeviceConfigurationCategory_t::const_iterator ARDrone_DeviceConfigurationCategoryIterator_t;
typedef std::map<std::string, ARDrone_DeviceConfigurationCategory_t> ARDrone_DeviceConfiguration_t;
typedef ARDrone_DeviceConfiguration_t::iterator ARDrone_DeviceConfigurationIterator_t;
typedef ARDrone_DeviceConfiguration_t::const_iterator ARDrone_DeviceConfigurationConstIterator_t;

typedef std::vector<unsigned int> ARDrone_NavDataOptionOffsets_t;
typedef ARDrone_NavDataOptionOffsets_t::const_iterator ARDrone_NavDataOptionOffsetsIterator_t;

#if defined (ACE_WIN32) || defined (ACE_WIN64)
__pragma (pack (push, 1))
struct parrot_video_encapsulation_t
{
  /*00*/ uint8_t  signature[4];
  /*04*/ uint8_t  version;
  /*05*/ uint8_t  video_codec;
  /*06*/ uint16_t header_size;
  /*08*/ uint32_t payload_size;            /* Amount of data following this PaVE */
  /*12*/ uint16_t encoded_stream_width;    /* ex: 640 */
  /*14*/ uint16_t encoded_stream_height;   /* ex: 368 */
  /*16*/ uint16_t display_width;           /* ex: 640 */
  /*18*/ uint16_t display_height;          /* ex: 360 */
  /*20*/ uint32_t frame_number;            /* frame position inside the current stream */
  /*24*/ uint32_t timestamp;               /* in milliseconds */
  /*28*/ uint8_t  total_chuncks;           /* number of UDP packets containing the current decodable payload */
  /*29*/ uint8_t  chunck_index;            /* position of the packet - first chunk is #0 */
  /*30*/ uint8_t  frame_type;              /* I-frame, P-frame */
  /*31*/ uint8_t  control;                 /* Special commands like end-of-stream or advertised frames */
  /*32*/ uint32_t stream_byte_position_lw; /* Byte position of the current payload in the encoded stream  - lower 32-bit word */
  /*36*/ uint32_t stream_byte_position_uw; /* Byte position of the current payload in the encoded stream  - upper 32-bit word */
  /*40*/ uint16_t stream_id;               /* This ID indentifies packets that should be recorded together */
  /*42*/ uint8_t  total_slices;            /* number of slices composing the current frame */
  /*43*/ uint8_t  slice_index;             /* position of the current slice in the frame */
  /*44*/ uint8_t  header1_size;            /* H.264 only : size of SPS inside payload - no SPS present if value is zero */
  /*45*/ uint8_t  header2_size;            /* H.264 only : size of PPS inside payload - no PPS present if value is zero */
  /*46*/ uint8_t  reserved2[2];            /* Padding to align on 48 bytes */
  /*48*/ uint32_t advertised_size;         /* Size of frames announced as advertised frames */
  /*52*/ uint8_t  reserved3[12];           /* Padding to align on 64 bytes */
};
__pragma (pack (pop))
#endif

struct ARDrone_UserData
 : Net_UserData
{
  ARDrone_UserData ()
   : Net_UserData ()
  {};
};

#if defined (__llvm__)
enum ARDrone_VideoMode
#else
enum ARDrone_VideoMode : int
#endif
{
  ARDRONE_VIDEOMODE_INVALID = -1,
  ARDRONE_VIDEOMODE_360P = H264_360P_CODEC,
  ARDRONE_VIDEOMODE_720P = H264_720P_CODEC,
  ///////////////////////////////////////
  ARDRONE_VIDEOMODE_MAX
};

typedef std::map<enum ARDrone_StreamType, struct Stream_Statistic> ARDroneStreamStatistic_t;
typedef ARDroneStreamStatistic_t::const_iterator ARDroneStreamStatisticConstIterator_t;
typedef ARDroneStreamStatistic_t::iterator ARDroneStreamStatisticIterator_t;
struct ARDrone_Statistic
 : Stream_Statistic
{
  ARDrone_Statistic ()
   : Stream_Statistic ()
   , streamStatistic ()
  {
    streamStatistic.insert (std::make_pair (ARDRONE_STREAM_CONTROL,
                                            Stream_Statistic ()));
    streamStatistic.insert (std::make_pair (ARDRONE_STREAM_MAVLINK,
                                            Stream_Statistic ()));
    streamStatistic.insert (std::make_pair (ARDRONE_STREAM_NAVDATA,
                                            Stream_Statistic ()));
    streamStatistic.insert (std::make_pair (ARDRONE_STREAM_VIDEO,
                                            Stream_Statistic ()));
  };

  struct ARDrone_Statistic operator+ ()
  {
    // *NOTE*: the idea is to merge the data

    Stream_Statistic::operator~ ();
    for (ARDroneStreamStatisticConstIterator_t iterator = streamStatistic.begin ();
         iterator != streamStatistic.end ();
         ++iterator)
    {
      *static_cast<struct Stream_Statistic*> (this) += (*iterator).second;
      bytesPerSecond += (*iterator).second.bytesPerSecond;
      messagesPerSecond += (*iterator).second.messagesPerSecond;

      timeStamp =
          ((timeStamp > (*iterator).second.timeStamp) ? timeStamp
                                                      : (*iterator).second.timeStamp);
    } // end FOR

    return *this;
  };

  ARDroneStreamStatistic_t streamStatistic;
};

#endif // #ifndef ARDRONE_TYPES_H
