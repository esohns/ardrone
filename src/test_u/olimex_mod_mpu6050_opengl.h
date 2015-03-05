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

#ifndef OLIMEX_MOD_MPU6050_OPENGL_H
#define OLIMEX_MOD_MPU6050_OPENGL_H

#include "GL/gl.h"

//void arrow (GLfloat,  // x1
//            GLfloat,  // y1
//            GLfloat,  // z1
//            GLfloat,  // x2
//            GLfloat,  // y2
//            GLfloat,  // z2
//            GLfloat); // D
//void axes (GLfloat); // length

GLuint axes ();
void frames_per_second (const float); // fps

#endif
