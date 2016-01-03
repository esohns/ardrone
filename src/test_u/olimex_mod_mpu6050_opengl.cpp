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

#include "GL/gl.h"
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

  // x --> y
  glBegin (GL_LINE_STRIP);
  glColor3f (1.0F, 1.0F, 1.0F);
  glVertex3f (0.0f, 0.0f, 0.0f);
  //glColor3f (1.0F, 0.0F, 0.0F);
  glColor3f (0.0F, 1.0F, 0.0F);
  glVertex3f (1.0f, 0.0f, 0.0f);
  glVertex3f (0.75f, 0.25f, 0.0f);
  glVertex3f (0.75f, -0.25f, 0.0f);
  glVertex3f (1.0f, 0.0f, 0.0f);
  glVertex3f (0.75f, 0.0f, 0.25f);
  glVertex3f (0.75f, 0.0f, -0.25f);
  glVertex3f (1.0f, 0.0f, 0.0f);
  glEnd ();

  // y --> z
  glBegin (GL_LINE_STRIP);
  glColor3f (1.0F, 1.0F, 1.0F);
  glVertex3f (0.0f, 0.0f, 0.0f);
  //glColor3f (0.0F, 1.0F, 0.0F);
  glColor3f (0.0F, 0.0F, 1.0F);
  glVertex3f (0.0f, 1.0f, 0.0f);
  glVertex3f (0.0f, 0.75f, 0.25f);
  glVertex3f (0.0f, 0.75f, -0.25f);
  glVertex3f (0.0f, 1.0f, 0.0f);
  glVertex3f (0.25f, 0.75f, 0.0f);
  glVertex3f (-0.25f, 0.75f, 0.0f);
  glVertex3f (0.0f, 1.0f, 0.0f);
  glEnd ();

  // z --> x
  glBegin (GL_LINE_STRIP);
  glColor3f (1.0F, 1.0F, 1.0F);
  glVertex3f (0.0f, 0.0f, 0.0f);
  //glColor3f (0.0F, 0.0F, 1.0F);
  glColor3f (1.0F, 0.0F, 0.0F);
  glVertex3f (0.0f, 0.0f, 1.0f);
  glVertex3f (0.25f, 0.0f, 0.75f);
  glVertex3f (-0.25f, 0.0f, 0.75f);
  glVertex3f (0.0f, 0.0f, 1.0f);
  glVertex3f (0.0f, 0.25f, 0.75f);
  glVertex3f (0.0f, -0.25f, 0.75f);
  glVertex3f (0.0f, 0.0f, 1.0f);
  glEnd ();

  // *NOTE*: GLUT_STROKE_ROMAN font size is around 152 units
  glScalef (0.005F, 0.005F, 0.005F);

  glTranslatef (220.0F, -30.0F, 0.0F);
  //glColor3f (1.0F, 0.0F, 0.0F);
  glColor3f (0.0F, 1.0F, 0.0F);
  //glutStrokeCharacter (OLIMEX_MOD_MPU6050_OPENGL_FONT_AXES, 'x');
  glutStrokeCharacter (OLIMEX_MOD_MPU6050_OPENGL_FONT_AXES, 'y');

  glTranslatef (-380.0F, 300.0F, 0.0F);
  //glColor3f (0.0F, 1.0F, 0.0F);
  glColor3f (0.0F, 0.0F, 1.0F);
  //glutStrokeCharacter (OLIMEX_MOD_MPU6050_OPENGL_FONT_AXES, 'y');
  glutStrokeCharacter (OLIMEX_MOD_MPU6050_OPENGL_FONT_AXES, 'z');

  glTranslatef (-105.0F, -305.0F, 250.0F);
  //glColor3f (0.0F, 0.0F, 1.0F);
  glColor3f (1.0F, 0.0F, 0.0F);
  //glutStrokeCharacter (OLIMEX_MOD_MPU6050_OPENGL_FONT_AXES, 'z');
  glutStrokeCharacter (OLIMEX_MOD_MPU6050_OPENGL_FONT_AXES, 'x');

  glEndList ();

  return axes_list;
}

/* Teapot */
/* Rim, body, lid, and bottom data must be reflected in x and
y; handle and spout data across the y axis only.  */
static int patchdata[][16] =
{
  /* rim */
  {102, 103, 104, 105, 4, 5, 6, 7, 8, 9, 10, 11,
  12, 13, 14, 15},
  /* body */
  {12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
  24, 25, 26, 27},
  {24, 25, 26, 27, 29, 30, 31, 32, 33, 34, 35, 36,
  37, 38, 39, 40},
  /* lid */
  {96, 96, 96, 96, 97, 98, 99, 100, 101, 101, 101,
  101, 0, 1, 2, 3,},
  {0, 1, 2, 3, 106, 107, 108, 109, 110, 111, 112,
  113, 114, 115, 116, 117},
  /* bottom */
  {118, 118, 118, 118, 124, 122, 119, 121, 123, 126,
  125, 120, 40, 39, 38, 37},
  /* handle */
  {41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52,
  53, 54, 55, 56},
  {53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64,
  28, 65, 66, 67},
  /* spout */
  {68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
  80, 81, 82, 83},
  {80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91,
  92, 93, 94, 95}
};

static float cpdata[][3] =
{
  {0.2, 0, 2.7}, {0.2, -0.112, 2.7}, {0.112, -0.2, 2.7}, {0,
  -0.2, 2.7}, {1.3375, 0, 2.53125}, {1.3375, -0.749, 2.53125},
  {0.749, -1.3375, 2.53125}, {0, -1.3375, 2.53125}, {1.4375,
  0, 2.53125}, {1.4375, -0.805, 2.53125}, {0.805, -1.4375,
  2.53125}, {0, -1.4375, 2.53125}, {1.5, 0, 2.4}, {1.5, -0.84,
  2.4}, {0.84, -1.5, 2.4}, {0, -1.5, 2.4}, {1.75, 0, 1.875},
  {1.75, -0.98, 1.875}, {0.98, -1.75, 1.875}, {0, -1.75,
  1.875}, {2, 0, 1.35}, {2, -1.12, 1.35}, {1.12, -2, 1.35},
  {0, -2, 1.35}, {2, 0, 0.9}, {2, -1.12, 0.9}, {1.12, -2,
  0.9}, {0, -2, 0.9}, {-2, 0, 0.9}, {2, 0, 0.45}, {2, -1.12,
  0.45}, {1.12, -2, 0.45}, {0, -2, 0.45}, {1.5, 0, 0.225},
  {1.5, -0.84, 0.225}, {0.84, -1.5, 0.225}, {0, -1.5, 0.225},
  {1.5, 0, 0.15}, {1.5, -0.84, 0.15}, {0.84, -1.5, 0.15}, {0,
  -1.5, 0.15}, {-1.6, 0, 2.025}, {-1.6, -0.3, 2.025}, {-1.5,
  -0.3, 2.25}, {-1.5, 0, 2.25}, {-2.3, 0, 2.025}, {-2.3, -0.3,
  2.025}, {-2.5, -0.3, 2.25}, {-2.5, 0, 2.25}, {-2.7, 0,
  2.025}, {-2.7, -0.3, 2.025}, {-3, -0.3, 2.25}, {-3, 0,
  2.25}, {-2.7, 0, 1.8}, {-2.7, -0.3, 1.8}, {-3, -0.3, 1.8},
  {-3, 0, 1.8}, {-2.7, 0, 1.575}, {-2.7, -0.3, 1.575}, {-3,
  -0.3, 1.35}, {-3, 0, 1.35}, {-2.5, 0, 1.125}, {-2.5, -0.3,
  1.125}, {-2.65, -0.3, 0.9375}, {-2.65, 0, 0.9375}, {-2,
  -0.3, 0.9}, {-1.9, -0.3, 0.6}, {-1.9, 0, 0.6}, {1.7, 0,
  1.425}, {1.7, -0.66, 1.425}, {1.7, -0.66, 0.6}, {1.7, 0,
  0.6}, {2.6, 0, 1.425}, {2.6, -0.66, 1.425}, {3.1, -0.66,
  0.825}, {3.1, 0, 0.825}, {2.3, 0, 2.1}, {2.3, -0.25, 2.1},
  {2.4, -0.25, 2.025}, {2.4, 0, 2.025}, {2.7, 0, 2.4}, {2.7,
  -0.25, 2.4}, {3.3, -0.25, 2.4}, {3.3, 0, 2.4}, {2.8, 0,
  2.475}, {2.8, -0.25, 2.475}, {3.525, -0.25, 2.49375},
  {3.525, 0, 2.49375}, {2.9, 0, 2.475}, {2.9, -0.15, 2.475},
  {3.45, -0.15, 2.5125}, {3.45, 0, 2.5125}, {2.8, 0, 2.4},
  {2.8, -0.15, 2.4}, {3.2, -0.15, 2.4}, {3.2, 0, 2.4}, {0, 0,
  3.15}, {0.8, 0, 3.15}, {0.8, -0.45, 3.15}, {0.45, -0.8,
  3.15}, {0, -0.8, 3.15}, {0, 0, 2.85}, {1.4, 0, 2.4}, {1.4,
  -0.784, 2.4}, {0.784, -1.4, 2.4}, {0, -1.4, 2.4}, {0.4, 0,
  2.55}, {0.4, -0.224, 2.55}, {0.224, -0.4, 2.55}, {0, -0.4,
  2.55}, {1.3, 0, 2.55}, {1.3, -0.728, 2.55}, {0.728, -1.3,
  2.55}, {0, -1.3, 2.55}, {1.3, 0, 2.4}, {1.3, -0.728, 2.4},
  {0.728, -1.3, 2.4}, {0, -1.3, 2.4}, {0, 0, 0}, {1.425,
  -0.798, 0}, {1.5, 0, 0.075}, {1.425, 0, 0}, {0.798, -1.425,
  0}, {0, -1.5, 0.075}, {0, -1.425, 0}, {1.5, -0.84, 0.075},
  {0.84, -1.5, 0.075}
};

static float tex[2][2][2] =
{
  {{0, 0},
  {1, 0}},
  {{0, 1},
  {1, 1}}
};

static void
teapot (GLint grid, GLfloat scale, GLenum type)
{
  float p[4][4][3], q[4][4][3], r[4][4][3], s[4][4][3];
  long i, j, k, l;

  glPushAttrib (GL_ENABLE_BIT | GL_EVAL_BIT);
  glEnable (GL_AUTO_NORMAL);
  glEnable (GL_NORMALIZE);
  glEnable (GL_MAP2_VERTEX_3);
  glEnable (GL_MAP2_TEXTURE_COORD_2);
  glPushMatrix ();
  glRotatef (270.0, 1.0, 0.0, 0.0);
  glScalef (0.5F * scale, 0.5F * scale, 0.5F * scale);
  glTranslatef (0.0, 0.0, -1.5);
  for (i = 0; i < 10; i++)
  {
    for (j = 0; j < 4; j++)
    {
      for (k = 0; k < 4; k++)
      {
        for (l = 0; l < 3; l++)
        {
          p[j][k][l] = cpdata[patchdata[i][j * 4 + k]][l];
          q[j][k][l] = cpdata[patchdata[i][j * 4 + (3 - k)]][l];
          if (l == 1)
            q[j][k][l] *= -1.0;
          if (i < 6)
          {
            r[j][k][l] =
              cpdata[patchdata[i][j * 4 + (3 - k)]][l];
            if (l == 0)
              r[j][k][l] *= -1.0;
            s[j][k][l] = cpdata[patchdata[i][j * 4 + k]][l];
            if (l == 0)
              s[j][k][l] *= -1.0;
            if (l == 1)
              s[j][k][l] *= -1.0;
          }
        }
      }
    }
    glMap2f (GL_MAP2_TEXTURE_COORD_2, 0, 1, 2, 2, 0, 1, 4, 2,
             &tex[0][0][0]);
    glMap2f (GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4,
             &p[0][0][0]);
    glMapGrid2f (grid, 0.0, 1.0, grid, 0.0, 1.0);
    glEvalMesh2 (type, 0, grid, 0, grid);
    glMap2f (GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4,
             &q[0][0][0]);
    glEvalMesh2 (type, 0, grid, 0, grid);
    if (i < 6)
    {
      glMap2f (GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4,
               &r[0][0][0]);
      glEvalMesh2 (type, 0, grid, 0, grid);
      glMap2f (GL_MAP2_VERTEX_3, 0, 1, 3, 4, 0, 1, 12, 4,
               &s[0][0][0]);
      glEvalMesh2 (type, 0, grid, 0, grid);
    }
  }
  glPopMatrix ();
  glPopAttrib ();
}
void
draw_teapot (bool solid,
             float scale)
{
  if (solid)
    teapot (7, scale, GL_FILL);
  else
    teapot (10, scale, GL_LINE);
}

void
frames_per_second (float framesPerSecond_in)
{
  OLIMEX_MOD_MPU6050_TRACE (ACE_TEXT ("::frames_per_second"));

  char buffer[BUFSIZ];
  ACE_OS::memset (buffer, 0, sizeof (buffer));
  int result = -1;
  result = ACE_OS::sprintf (buffer,
                            ACE_TEXT_ALWAYS_CHAR ("%.2f"),
                            framesPerSecond_in);
  if (result < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to sprintf(): \"%m\", returning\n")));
    return;
  } // end IF

  glRasterPos2f (5.0F, glutBitmapHeight (OLIMEX_MOD_MPU6050_OPENGL_FONT_FPS) / 2.0F);
  glutBitmapString (OLIMEX_MOD_MPU6050_OPENGL_FONT_FPS,
                    reinterpret_cast<unsigned char*> (buffer));
}
