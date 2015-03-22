/*  I2C kernel module driver for the Olimex MOD-MPU6050 UEXT module
    (see https://www.olimex.com/Products/Modules/Sensors/MOD-MPU6050/open-source-hardware,
         http://www.invensense.com/mems/gyro/mpu6050.html)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#ifndef OLIMEX_MOD_MPU6050_DEFINES_H
#define OLIMEX_MOD_MPU6050_DEFINES_H

//#include "ace/Default_Constants.h"

#include "GL/glut.h"

#include "net_defines.h"

// *** OpenGL-related
#define OLIMEX_MOD_MPU6050_OPENGL_DOUBLE_BUFFERED                    true
#define OLIMEX_MOD_MPU6050_OPENGL_RAD_PER_DEG                        0.0174533F
#define OLIMEX_MOD_MPU6050_OPENGL_AXES_SIZE                          0.2 // percentage --> 10%
// fonts
//#define OLIMEX_MOD_MPU6050_OPENGL_FONT_AXES                          GLUT_BITMAP_HELVETICA_18
#define OLIMEX_MOD_MPU6050_OPENGL_FONT_AXES                          GLUT_STROKE_MONO_ROMAN
#define OLIMEX_MOD_MPU6050_OPENGL_FONT_FPS                           GLUT_BITMAP_HELVETICA_12
// camera calibration
#define OLIMEX_MOD_MPU6050_OPENGL_CAMERA_ROTATION_FACTOR             0.8F
#define OLIMEX_MOD_MPU6050_OPENGL_CAMERA_TRANSLATION_FACTOR          0.01F
#define OLIMEX_MOD_MPU6050_OPENGL_CAMERA_ZOOM_FACTOR                 0.03F
// defaults
#define OLIMEX_MOD_MPU6050_OPENGL_CAMERA_DEFAULT_ZOOM                5.0F
// perspective(s)
#define OLIMEX_MOD_MPU6050_OPENGL_PERSPECTIVE_FOVY                   60.0
#define OLIMEX_MOD_MPU6050_OPENGL_PERSPECTIVE_ZNEAR                  1.0
#define OLIMEX_MOD_MPU6050_OPENGL_PERSPECTIVE_ZFAR                   100.0
#define OLIMEX_MOD_MPU6050_OPENGL_ORTHO_ZNEAR                        -1.0
#define OLIMEX_MOD_MPU6050_OPENGL_ORTHO_ZFAR                         1.0

// *** glade UI-related ***
#define OLIMEX_MOD_MPU6050_UI_DEFINITION_FILE_NAME                   "olimex_mod_mpu6050.glade"
#define OLIMEX_MOD_MPU6050_DEFAULT_UI_WIDGET_WINDOW_MAIN_SIZE_WIDTH  640
#define OLIMEX_MOD_MPU6050_DEFAULT_UI_WIDGET_WINDOW_MAIN_SIZE_HEIGHT 480
#define OLIMEX_MOD_MPU6050_UI_INITIALIZATION_DELAY                   100 // ms
#define OLIMEX_MOD_MPU6050_UI_WIDGET_GL_REFRESH_INTERVAL             1000 / 60 // Hz
#define OLIMEX_MOD_MPU6050_UI_WIDGET_NAME_DIALOG_ABOUT               "about_dialog"
#define OLIMEX_MOD_MPU6050_UI_WIDGET_NAME_DRAWING_AREA               "drawing_area"
#define OLIMEX_MOD_MPU6050_UI_WIDGET_NAME_MENU_FILE_QUIT             "quit"
#define OLIMEX_MOD_MPU6050_UI_WIDGET_NAME_MENU_HELP_ABOUT            "about"
#define OLIMEX_MOD_MPU6050_UI_WIDGET_NAME_MENU_VIEW_CALIBRATE        "calibrate"
#define OLIMEX_MOD_MPU6050_UI_WIDGET_NAME_STATUS_BAR                 "status_bar"
#define OLIMEX_MOD_MPU6050_UI_WIDGET_NAME_WINDOW_MAIN                "main_window"
#define OLIMEX_MOD_MPU6050_UI_WIDGET_STATUS_BAR_CONTEXT_CONNECTIVITY "connectivity"
#define OLIMEX_MOD_MPU6050_UI_WIDGET_STATUS_BAR_CONTEXT_DATA         "data"

// *** network-related ***
#define OLIMEX_MOD_MPU6050_USE_ASYNCH_CONNECTOR                      false
// *PORTABILITY*: interface names are not portable, so let the user choose the
//                interface from a list on Windows (see select_interface())...
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#define OLIMEX_MOD_MPU6050_DEFAULT_NETWORK_INTERFACE                 ""
#else
#define OLIMEX_MOD_MPU6050_DEFAULT_NETWORK_INTERFACE                 NET_DEFAULT_NETWORK_INTERFACE
#endif

#define OLIMEX_MOD_MPU6050_DEFAULT_PORT                              NET_DEFAULT_PORT

// default event dispatcher (default: use asynch I/O (proactor))
#define OLIMEX_MOD_MPU6050_CONNECTION_HANDLER_THREAD_NAME            NET_CONNECTION_HANDLER_THREAD_NAME
#define OLIMEX_MOD_MPU6050_CONNECTION_HANDLER_THREAD_GROUP_ID        1

//#define SOCKET_RECEIVE_BUFFER_SIZE                    ACE_DEFAULT_MAX_SOCKET_BUFSIZ
#define OLIMEX_MOD_MPU6050_SOCKET_RECEIVE_BUFFER_SIZE                NET_DEFAULT_SOCKET_RECEIVE_BUFFER_SIZE
//#define DEFAULT_SOCKET_TCP_NODELAY                  true
//#define DEFAULT_SOCKET_TCP_KEEPALIVE                false
//#define DEFAULT_SOCKET_LINGER                       10 // seconds {0 --> off}

// *** pro/reactor-related ***
#define OLIMEX_MOD_MPU6050_USE_REACTOR                               true
#define OLIMEX_MOD_MPU6050_TASK_GROUP_ID                             11

// *** stream-related ***
#define OLIMEX_MOD_MPU6050_STREAM_BUFFER_SIZE                        14
// *IMPORTANT NOTE*: any of these COULD seriously affect performance
#define OLIMEX_MOD_MPU6050_MAXIMUM_QUEUE_SLOTS                       1000
#define OLIMEX_MOD_MPU6050_MAXIMUM_NUMBER_OF_INFLIGHT_MESSAGES       1000

#define OLIMEX_MOD_MPU6050_STATISTICS_REPORTING_INTERVAL             0 // seconds [0 --> OFF]

// *** device-related ***
#define OLIMEX_MOD_MPU6050_ACCELEROMETER_LSB_FACTOR_2                16384 // LSB/g
#define OLIMEX_MOD_MPU6050_THERMOMETER_LSB_FACTOR                    340
#define OLIMEX_MOD_MPU6050_THERMOMETER_OFFSET                        36.53F
#define OLIMEX_MOD_MPU6050_GYROSCOPE_LSB_FACTOR_250                  131 // LSB/°/s

#define OLIMEX_MOD_MPU6050_LOG_FILE_NAME                             "olimex_mod_mpu6050.log"

#endif // #ifndef OLIMEX_MOD_MPU6050_DEFINES_H
