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

#include "olimex_mod_mpu6050_opengl.h"

//#include <math.h>

#include "GL/gl.h"
//#include "GL/glu.h"
#include "GL/glut.h"
#include "GL/freeglut_ext.h"

#include "ace/Assert.h"
#include "ace/Log_Msg.h"
#include "ace/OS.h"

#include "olimex_mod_mpu6050_defines.h"
#include "olimex_mod_mpu6050_macros.h"

//void
//arrow (GLfloat x1_in, GLfloat y1_in, GLfloat z1_in,
//       GLfloat x2_in, GLfloat y2_in, GLfloat z2_in,
//       GLfloat d_in)
//{
//  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("::arrow"));
//
//  float x = x2_in - x1_in;
//  float y = y2_in - y1_in;
//  float z = z2_in - z1_in;
//  float L = ::sqrt ((x * x) + (y * y) + (z * z));
//
////  GLUquadricObj* quad_obj_p;
//  GLUquadric* cy1_p = gluNewQuadric ();
//  GLUquadric* cy2_p = gluNewQuadric ();
////  GLUquadric* cy3_p = gluNewQuadric ();
//  ACE_ASSERT (cy1_p && cy2_p);// && cy3_p);
//
//  glPushMatrix ();
//
//  glTranslatef (x1_in, y1_in, z1_in);
//  if (x || y)
//  {
//    glRotatef (::atan2 (y, x) / OLIMEX_MOD_MPU6050_OPENGL_RAD_PER_DEG,
//               0.0F, 0.0F, 1.0F);
//    glRotatef (::atan2 (::sqrt ((x * x) + (y * y)), z) / OLIMEX_MOD_MPU6050_OPENGL_RAD_PER_DEG,
//               0.0F, 1.0F, 0.0F);
//  } // end IF
//  else if (z < 0)
//    glRotatef (180.0F,
//               1.0F, 0.0F, 0.0F);
//
//  glTranslatef (0.0F, 0.0F, L - (4.0F * d_in));
//  gluQuadricDrawStyle (cy1_p, GLU_FILL);
//  gluQuadricNormals (cy1_p, GLU_SMOOTH);
//  glTranslatef (0.0F, 0.0F, 4.0F);
//  glColor3f (1.0F, 0.0F, 0.0F);
//  gluCylinder (cy1_p, 0.04, 0.0, 1.0, 12, 1);
//  gluDeleteQuadric (cy1_p);
//
//  glTranslatef (0.0F, 0.0F, -L + (4.0F * d_in));
//  gluQuadricDrawStyle (cy2_p, GLU_FILL);
//  gluQuadricNormals (cy2_p, GLU_SMOOTH);
//  glTranslatef (0.0F, 4.0F, 0.0F);
//  glColor3f (0.0F, 1.0F, 0.0F);
//  gluCylinder (cy2_p, 0.4, 0.4, L - 1.6, 12, 1);
//  gluDeleteQuadric (cy2_p);
//
////  glTranslatef (0.0F, 0.0F, -L + (4.0F * d_in));
////  gluQuadricDrawStyle (cy3_p, GLU_FILL);
////  gluQuadricNormals (cy3_p, GLU_SMOOTH);
////  glTranslatef (4.0F, 0.0F, 0.0F);
////  glColor3f (1.0F, 1.0F, 1.0F);
////  gluCylinder (cy3_p, 0.4, 0.4, L - 1.6, 12, 1);
////  gluDeleteQuadric (cy3_p);
//
//  glPopMatrix ();
//}

//void
//axes (GLfloat length_in)
//{
//  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("::axes"));
//
//  glPushMatrix ();
//  glTranslatef (-length_in, 0.0F, 0.0F);
//  ::arrow (0.0F, 0.0F, 0.0F, 2.0F * length_in, 0.0F, 0.0F, 0.2F);
//  glPopMatrix ();
//
//  glPushMatrix ();
//  glTranslatef (0.0F, -length_in, 0.0F);
//  ::arrow (0.0F, 0.0F, 0.0F, 0.0F, 2.0F * length_in, 0.0F, 0.2F);
//  glPopMatrix ();
//
//  glPushMatrix ();
//  glTranslatef (0.0F, 0.0F, -length_in);
//  ::arrow (0.0F, 0.0F, 0.0F, 0.0F, 0.0F, 2.0F * length_in, 0.2F);
//  glPopMatrix ();
//}

GLuint
axes ()
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("::axes"));

  GLuint axes_list = glGenLists (1);
  ACE_ASSERT (axes_list);

  glNewList (axes_list, GL_COMPILE);

  //glDisable (GL_LIGHTING);

  glPushMatrix ();
  //glScalef (1.0F, 1.0F, 0.0F);

  glBegin (GL_LINE_STRIP);
  glColor3f (0.0F, 0.0F, 0.0F);
  glVertex3f (0.0f, 0.0f, 0.0f);
  glColor3f (1.0F, 0.0F, 0.0F);
  glVertex3f (1.0f, 0.0f, 0.0f);
  glVertex3f (0.75f, 0.25f, 0.0f);
  glVertex3f (0.75f, -0.25f, 0.0f);
  glVertex3f (1.0f, 0.0f, 0.0f);
  glVertex3f (0.75f, 0.0f, 0.25f);
  glVertex3f (0.75f, 0.0f, -0.25f);
  glVertex3f (1.0f, 0.0f, 0.0f);
  glEnd ();

  glBegin (GL_LINE_STRIP);
  glColor3f (0.0F, 0.0F, 0.0F);
  glVertex3f (0.0f, 0.0f, 0.0f);
  glColor3f (0.0F, 1.0F, 0.0F);
  glVertex3f (0.0f, 1.0f, 0.0f);
  glVertex3f (0.0f, 0.75f, 0.25f);
  glVertex3f (0.0f, 0.75f, -0.25f);
  glVertex3f (0.0f, 1.0f, 0.0f);
  glVertex3f (0.25f, 0.75f, 0.0f);
  glVertex3f (-0.25f, 0.75f, 0.0f);
  glVertex3f (0.0f, 1.0f, 0.0f);
  glEnd ();

  glBegin (GL_LINE_STRIP);
  glColor3f (0.0F, 0.0F, 0.0F);
  glVertex3f (0.0f, 0.0f, 0.0f);
  glColor3f (0.0F, 0.0F, 1.0F);
  glVertex3f (0.0f, 0.0f, 1.0f);
  glVertex3f (0.25f, 0.0f, 0.75f);
  glVertex3f (-0.25f, 0.0f, 0.75f);
  glVertex3f (0.0f, 0.0f, 1.0f);
  glVertex3f (0.0f, 0.25f, 0.75f);
  glVertex3f (0.0f, -0.25f, 0.75f);
  glVertex3f (0.0f, 0.0f, 1.0f);
  glEnd ();

  glPopMatrix ();

  glPushMatrix ();
  // *NOTE*: GLUT_STROKE_ROMAN font size is around 152 units
  glScalef (0.005F, 0.005F, 0.005F);

  glTranslatef (220.0F, -30.0F, 0.0F);
  glColor3f (1.0F, 0.0F, 0.0F);
  glutStrokeCharacter (OLIMEX_MOD_MPU6050_OPENGL_FONT_AXES, 'x');

  glTranslatef (-380.0F, 300.0F, 0.0F);
  glColor3f (0.0F, 1.0F, 0.0F);
  glutStrokeCharacter (OLIMEX_MOD_MPU6050_OPENGL_FONT_AXES, 'y');

  glTranslatef (-105.0F, -305.0F, 250.0F);
  glColor3f (0.0F, 0.0F, 1.0F);
  glutStrokeCharacter (OLIMEX_MOD_MPU6050_OPENGL_FONT_AXES, 'z');

  glPopMatrix ();

  //glDisable (GL_COLOR_MATERIAL);
  //glEnable (GL_LIGHTING);

  glEndList ();

  return axes_list;
}

void
frames_per_second (unsigned int framesPerSecond_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("::frames_per_second"));

  char buffer[BUFSIZ];
  ACE_OS::memset (buffer, 0, sizeof (buffer));
  int result = -1;
  result = ACE_OS::sprintf (buffer,
                            ACE_TEXT_ALWAYS_CHAR ("%d"),
                            framesPerSecond_in);
  if (result < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to sprintf(): \"%m\", returning\n")));
    return;
  } // end IF

  //glDisable (GL_LIGHTING);

  //glColor3f (1.0F, 1.0F, 1.0F);
  glRasterPos2f (5.0F, glutBitmapHeight (OLIMEX_MOD_MPU6050_OPENGL_FONT_FPS) / 2.0F);
  glutBitmapString (OLIMEX_MOD_MPU6050_OPENGL_FONT_FPS,
                    reinterpret_cast<unsigned char*> (buffer));

  //glEnable (GL_LIGHTING);
}
