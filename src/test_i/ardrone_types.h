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

#include <glm/glm.hpp>

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#include <video_encapsulation.h>
#endif

#include <ace/Global_Macros.h>
#include <ace/Time_Value.h>

#include <gtk/gtk.h>

#include "common_ui_common.h"

#include "net_common.h"
#include "net_iconnector.h"

#include "ardrone_defines.h"

enum ARDrone_MessageType : int
{
  ARDRONE_MESSAGE_INVALID = -1,
  ARDRONE_MESSAGE_SENSOR_DATA = ACE_Message_Block::MB_PROTO,
  ARDRONE_MESSAGE_VIDEO_FRAME,
  ///////////////////////////////////////
  ARDRONE_MESSAGE_MAX
};

enum ARDrone_Event : int
{
  ARDRONE_EVENT_INVALID = -1,
  ARDRONE_EVENT_CONNECT,
  ARDRONE_EVENT_DISCONNECT,
  ARDRONE_EVENT_MESSAGE,
  ARDRONE_EVENT_SESSION_MESSAGE,
  ///////////////////////////////////////
  ARDRONE_EVENT_MAX
};
typedef std::deque<enum ARDrone_Event> ARDrone_Events_t;
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

struct ARDrone_Camera
{
  //glm::vec3 position;
  //glm::vec3 looking_at;
  //glm::vec3 up;

  float zoom;
  glm::vec3 rotation;
  glm::vec3 translation;
  int last[2];
};

#if defined (ACE_WIN32) || defined (ACE_WIN64)
__pragma (pack (push, 1))
struct ARDrone_ParrotVideoEncapsulation_Header
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

struct ARDrone_ConnectionConfiguration;
struct ARDrone_UserData
 : Net_UserData
{
  inline ARDrone_UserData ()
   : Net_UserData ()
   , connectionConfiguration (NULL)
  {};

  struct ARDrone_ConnectionConfiguration* connectionConfiguration;
};

#endif // #ifndef ARDRONE_TYPES_H
