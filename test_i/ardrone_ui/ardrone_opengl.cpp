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

#include "ardrone_opengl.h"

#if defined (GTKGL_SUPPORT)
#include "GL/gl.h"
#include "GL/glut.h"
#include "GL/freeglut_ext.h"

#include "ace/Assert.h"
#include "ace/Log_Msg.h"
#include "ace/OS.h"

#include "ardrone_defines.h"
#include "ardrone_macros.h"

//void
//arrow (GLfloat x1_in, GLfloat y1_in, GLfloat z1_in,
//       GLfloat x2_in, GLfloat y2_in, GLfloat z2_in,
//       GLfloat d_in)
//{
//  ARDRONE_TRACE (ACE_TEXT ("::arrow"));
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
//    glRotatef (::atan2 (y, x) / ardrone_OPENGL_RAD_PER_DEG,
//               0.0F, 0.0F, 1.0F);
//    glRotatef (::atan2 (::sqrt ((x * x) + (y * y)), z) / ardrone_OPENGL_RAD_PER_DEG,
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
//  ARDRONE_TRACE (ACE_TEXT ("::axes"));
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
  //ARDRONE_TRACE (ACE_TEXT ("::axes"));

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
  //glutStrokeCharacter (ardrone_OPENGL_FONT_AXES, 'x');
  glutStrokeCharacter (ARDRONE_OPENGL_FONT_AXES, 'y');

  glTranslatef (-380.0F, 300.0F, 0.0F);
  //glColor3f (0.0F, 1.0F, 0.0F);
  glColor3f (0.0F, 0.0F, 1.0F);
  //glutStrokeCharacter (ardrone_OPENGL_FONT_AXES, 'y');
  glutStrokeCharacter (ARDRONE_OPENGL_FONT_AXES, 'z');

  glTranslatef (-105.0F, -305.0F, 250.0F);
  //glColor3f (0.0F, 0.0F, 1.0F);
  glColor3f (1.0F, 0.0F, 0.0F);
  //glutStrokeCharacter (ardrone_OPENGL_FONT_AXES, 'z');
  glutStrokeCharacter (ARDRONE_OPENGL_FONT_AXES, 'x');

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
  {0.2f, 0.f, 2.7f}, {0.2f, -0.112f, 2.7f}, {0.112f, -0.2f, 2.7f}, {0.f,
  -0.2f, 2.7f}, {1.3375f, 0.f, 2.53125f}, {1.3375f, -0.749f, 2.53125f},
  {0.749f, -1.3375f, 2.53125f}, {0.f, -1.3375f, 2.53125f}, {1.4375f,
  0.f, 2.53125f}, {1.4375f, -0.805f, 2.53125f}, {0.805f, -1.4375f,
  2.53125f}, {0.f, -1.4375f, 2.53125f}, {1.5f, 0.f, 2.4f}, {1.5f, -0.84f,
  2.4f}, {0.84f, -1.5f, 2.4f}, {0.f, -1.5f, 2.4f}, {1.75f, 0.f, 1.875f},
  {1.75f, -0.98f, 1.875f}, {0.98f, -1.75f, 1.875f}, {0.f, -1.75f,
  1.875f}, {2.f, 0.f, 1.35f}, {2.f, -1.12f, 1.35f}, {1.12f, -2.f, 1.35f},
  {0.f, -2.f, 1.35f}, {2.f, 0.f, 0.9f}, {2.f, -1.12f, 0.9f}, {1.12f, -2.f,
  0.9f}, {0.f, -2.f, 0.9f}, {-2.f, 0.f, 0.9f}, {2.f, 0.f, 0.45f}, {2.f, -1.12f,
  0.45f}, {1.12f, -2.f, 0.45f}, {0.f, -2.f, 0.45f}, {1.5f, 0.f, 0.225f},
  {1.5f, -0.84f, 0.225f}, {0.84f, -1.5f, 0.225f}, {0.f, -1.5f, 0.225f},
  {1.5f, 0.f, 0.15f}, {1.5f, -0.84f, 0.15f}, {0.84f, -1.5f, 0.15f}, {0.f,
  -1.5f, 0.15f}, {-1.6f, 0.f, 2.025f}, {-1.6f, -0.3f, 2.025f}, {-1.5f,
  -0.3f, 2.25f}, {-1.5f, 0.f, 2.25f}, {-2.3f, 0.f, 2.025f}, {-2.3f, -0.3f,
  2.025f}, {-2.5f, -0.3f, 2.25f}, {-2.5f, 0.f, 2.25f}, {-2.7f, 0.f,
  2.025f}, {-2.7f, -0.3f, 2.025f}, {-3.f, -0.3f, 2.25f}, {-3.f, 0.f,
  2.25f}, {-2.7f, 0.f, 1.8f}, {-2.7f, -0.3f, 1.8f}, {-3.f, -0.3f, 1.8f},
  {-3.f, 0.f, 1.8f}, {-2.7f, 0.f, 1.575f}, {-2.7f, -0.3f, 1.575f}, {-3.f,
  -0.3f, 1.35f}, {-3.f, 0.f, 1.35f}, {-2.5f, 0.f, 1.125f}, {-2.5f, -0.3f,
  1.125f}, {-2.65f, -0.3f, 0.9375f}, {-2.65f, 0.f, 0.9375f}, {-2.f,
  -0.3f, 0.9f}, {-1.9f, -0.3f, 0.6f}, {-1.9f, 0.f, 0.6f}, {1.7f, 0.f,
  1.425f}, {1.7f, -0.66f, 1.425f}, {1.7f, -0.66f, 0.6f}, {1.7f, 0.f,
  0.6f}, {2.6f, 0.f, 1.425f}, {2.6f, -0.66f, 1.425f}, {3.1f, -0.66f,
  0.825f}, {3.1f, 0.f, 0.825f}, {2.3f, 0.f, 2.1f}, {2.3f, -0.25f, 2.1f},
  {2.4f, -0.25f, 2.025f}, {2.4f, 0.f, 2.025f}, {2.7f, 0.f, 2.4f}, {2.7f,
  -0.25f, 2.4f}, {3.3f, -0.25f, 2.4f}, {3.3f, 0.f, 2.4f}, {2.8f, 0.f,
  2.475f}, {2.8f, -0.25f, 2.475f}, {3.525f, -0.25f, 2.49375f},
  {3.525f, 0.f, 2.49375f}, {2.9f, 0.f, 2.475f}, {2.9f, -0.15f, 2.475f},
  {3.45f, -0.15f, 2.5125f}, {3.45f, 0.f, 2.5125f}, {2.8f, 0.f, 2.4f},
  {2.8f, -0.15f, 2.4f}, {3.2f, -0.15f, 2.4f}, {3.2f, 0.f, 2.4f}, {0.f, 0.f,
  3.15f}, {0.8f, 0.f, 3.15f}, {0.8f, -0.45f, 3.15f}, {0.45f, -0.8f,
  3.15f}, {0.f, -0.8f, 3.15f}, {0.f, 0.f, 2.85f}, {1.4f, 0.f, 2.4f}, {1.4f,
  -0.784f, 2.4f}, {0.784f, -1.4f, 2.4f}, {0.f, -1.4f, 2.4f}, {0.4f, 0.f,
  2.55f}, {0.4f, -0.224f, 2.55f}, {0.224f, -0.4f, 2.55f}, {0.f, -0.4f,
  2.55f}, {1.3f, 0.f, 2.55f}, {1.3f, -0.728f, 2.55f}, {0.728f, -1.3f,
  2.55f}, {0.f, -1.3f, 2.55f}, {1.3f, 0.f, 2.4f}, {1.3f, -0.728f, 2.4f},
  {0.728f, -1.3f, 2.4f}, {0.f, -1.3f, 2.4f}, {0.f, 0.f, 0.f}, {1.425f,
  -0.798f, 0.f}, {1.5f, 0.f, 0.075f}, {1.425f, 0.f, 0.f}, {0.798f, -1.425f,
  0.f}, {0.f, -1.5f, 0.075f}, {0.f, -1.425f, 0.f}, {1.5f, -0.84f, 0.075f},
  {0.84f, -1.5f, 0.075f}
};

static float tex[2][2][2] =
{
  { {0, 0},
    {1, 0} },
  { {0, 1},
    {1, 1} }
};

static void
teapot (GLint grid, GLfloat scale, GLenum type)
{
  //ARDRONE_TRACE (ACE_TEXT ("::teapot"));

  float p[4][4][3], q[4][4][3], r[4][4][3], s[4][4][3];
  long i, j, k, l;

  glPushAttrib (GL_ENABLE_BIT | GL_EVAL_BIT);
  glEnable (GL_AUTO_NORMAL);
  glEnable (GL_NORMALIZE);
  glEnable (GL_MAP2_VERTEX_3);
  glEnable (GL_MAP2_TEXTURE_COORD_2);
  glPushMatrix ();
  glRotatef (270.0f, 1.0f, 0.0f, 0.0f);
  glScalef (0.5f * scale, 0.5f * scale, 0.5f * scale);
  glTranslatef (0.0f, 0.0f, -1.5f);
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
  //ARDRONE_TRACE (ACE_TEXT ("::frames_per_second"));

  char buffer_a[BUFSIZ];
  ACE_OS::memset (buffer_a, 0, sizeof (char[BUFSIZ]));
  int result =
    ACE_OS::sprintf (buffer_a, ACE_TEXT_ALWAYS_CHAR ("%.2f"), framesPerSecond_in);
  if (result < 0)
  {
    ACE_DEBUG ((LM_ERROR,
                ACE_TEXT ("failed to sprintf(): \"%m\", returning\n")));
    return;
  } // end IF

  glRasterPos2f (5.0F, glutBitmapHeight (ARDRONE_OPENGL_FONT_FPS) / 2.0F);
  glutBitmapString (ARDRONE_OPENGL_FONT_FPS,
                    reinterpret_cast<unsigned char*> (buffer_a));
}
#endif
