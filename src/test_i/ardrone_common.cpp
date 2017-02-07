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
#include "stdafx.h"

#include "ace/Basic_Types.h"
//#include "ace/Log_Msg.h"

#include "ardrone_defines.h"
#include "ardrone_macros.h"

void
extract_data (const char* data_in,
              float& ax_out, float& ay_out, float& az_out,
              float& t_out,
              float& gx_out, float& gy_out, float& gz_out)
{
  ARDRONE_TRACE (ACE_TEXT ("::extract_data"));

  const short int* data_p = reinterpret_cast<const short int*> (data_in);
  short int a_x, a_y, a_z;
  short int g_x, g_y, g_z;
//  short int t;
  // *NOTE*: i2c uses a big-endian transfer syntax
  a_x = ((ACE_BYTE_ORDER == ACE_LITTLE_ENDIAN) ? ACE_SWAP_WORD (*data_p)
         : *data_p);
  // invert two's complement representation
  a_x = (a_x & 0x8000) ? ((a_x & 0x7FFF) - 0x8000) : a_x;
  data_p++;
  a_y = ((ACE_BYTE_ORDER == ACE_LITTLE_ENDIAN) ? ACE_SWAP_WORD (*data_p)
         : *data_p);
  a_y = (a_y & 0x8000) ? ((a_y & 0x7FFF) - 0x8000) : a_y;
  data_p++;
  a_z = ((ACE_BYTE_ORDER == ACE_LITTLE_ENDIAN) ? ACE_SWAP_WORD (*data_p)
         : *data_p);
  a_z = (a_z & 0x8000) ? ((a_z & 0x7FFF) - 0x8000) : a_z;
  data_p++;
//  t = ((ACE_BYTE_ORDER == ACE_LITTLE_ENDIAN) ? ACE_SWAP_WORD (*data_p)
//       : *data_p);
  data_p++;
  g_x = ((ACE_BYTE_ORDER == ACE_LITTLE_ENDIAN) ? ACE_SWAP_WORD (*data_p)
         : *data_p);
  g_x = (g_x & 0x8000) ? ((g_x & 0x7FFF) - 0x8000) : g_x;
  data_p++;
  g_y = ((ACE_BYTE_ORDER == ACE_LITTLE_ENDIAN) ? ACE_SWAP_WORD (*data_p)
         : *data_p);
  g_y = (g_y & 0x8000) ? ((g_y & 0x7FFF) - 0x8000) : g_y;
  data_p++;
  g_z = ((ACE_BYTE_ORDER == ACE_LITTLE_ENDIAN) ? ACE_SWAP_WORD (*data_p)
         : *data_p);
  g_z = (g_z & 0x8000) ? ((g_z & 0x7FFF) - 0x8000) : g_z;
  // translate quantities into absolute values
  //ax_out = a_x / ARDRONE_ACCELEROMETER_LSB_FACTOR_2;
  //ay_out = a_y / ARDRONE_ACCELEROMETER_LSB_FACTOR_2;
  //az_out = a_z / ARDRONE_ACCELEROMETER_LSB_FACTOR_2;
  //t_out = (t / ARDRONE_THERMOMETER_LSB_FACTOR) +
  //  ARDRONE_THERMOMETER_OFFSET;
  //gx_out = g_x / ARDRONE_GYROSCOPE_LSB_FACTOR_250;
  //gy_out = g_y / ARDRONE_GYROSCOPE_LSB_FACTOR_250;
  //gz_out = g_z / ARDRONE_GYROSCOPE_LSB_FACTOR_250;
  //  ACE_DEBUG ((LM_DEBUG,
  //              ACE_TEXT ("%6.3f,%6.3f,%6.3f,%6.2f,%8.3f,%8.3f,%8.3f\n"),
  //              ax_out, ay_out, az_out,
  //              t_out,
  //              gx_out, gy_out, gz_out));
}
