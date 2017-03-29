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

#ifndef ARDRONE_DEFINES_H
#define ARDRONE_DEFINES_H

// *** OpenGL-related
#define ARDRONE_OPENGL_DOUBLE_BUFFERED                     true
#define ARDRONE_OPENGL_RAD_PER_DEG                         0.0174533F
#define ARDRONE_OPENGL_AXES_SIZE                           0.2 // percentage --> 10%
// fonts
//#define ARDRONE_OPENGL_FONT_AXES                          GLUT_BITMAP_HELVETICA_18
#define ARDRONE_OPENGL_FONT_AXES                           GLUT_STROKE_MONO_ROMAN
#define ARDRONE_OPENGL_FONT_FPS                            GLUT_BITMAP_HELVETICA_12
// camera calibration
#define ARDRONE_OPENGL_CAMERA_ROTATION_FACTOR              0.8F
#define ARDRONE_OPENGL_CAMERA_TRANSLATION_FACTOR           0.01F
#define ARDRONE_OPENGL_CAMERA_ZOOM_FACTOR                  0.03F
// defaults
#define ARDRONE_OPENGL_CAMERA_DEFAULT_ZOOM                 5.0F
// perspective(s)
#define ARDRONE_OPENGL_PERSPECTIVE_FOVY                    60.0
#define ARDRONE_OPENGL_PERSPECTIVE_ZNEAR                   1.0
#define ARDRONE_OPENGL_PERSPECTIVE_ZFAR                    100.0
#define ARDRONE_OPENGL_ORTHO_ZNEAR                         -1.0
#define ARDRONE_OPENGL_ORTHO_ZFAR                          1.0

// *** Gtk UI-related ***
#define ARDRONE_UI_DEFINITION_FILE_NAME                    "ardrone.glade"
#define ARDRONE_UI_INITIALIZATION_DELAY                    100 // ms
//#define ARDRONE_UI_WIDGET_CURVE_MAXIMUM_Y                 100.0F
// *IMPORTANT NOTE*: also sets the (maximum) rate of message processing
#define ARDRONE_UI_WIDGET_GL_REFRESH_INTERVAL              1000.0F / 30.0F // Hz
#define ARDRONE_UI_PROCESSING_THREAD_NAME                  "stream processor"

//#define ARDRONE_DEFAULT_UI_WIDGET_DRAWINGAREA_VIDEO_WIDTH  320
//#define ARDRONE_DEFAULT_UI_WIDGET_DRAWINGAREA_VIDEO_HEIGHT 240
#define ARDRONE_UI_WIDGET_NAME_ACTION_CALIBRATE            "action_calibrate"
#define ARDRONE_UI_WIDGET_NAME_ACTION_CUT                  "action_cut"
#define ARDRONE_UI_WIDGET_NAME_ACTION_TRIM                 "action_trim"
#define ARDRONE_UI_WIDGET_NAME_BUTTON_ABOUT                "button_about"
#define ARDRONE_UI_WIDGET_NAME_BUTTON_CLEAR                "button_clear"
#define ARDRONE_UI_WIDGET_NAME_BUTTON_QUIT                 "button_quit"
#define ARDRONE_UI_WIDGET_NAME_CHECKBUTTON_FULLSCREEN      "checkbutton_fullscreen"
#define ARDRONE_UI_WIDGET_NAME_CHECKBUTTON_ASYNCH          "checkbutton_asynch"
#define ARDRONE_UI_WIDGET_NAME_CHECKBUTTON_SAVE            "checkbutton_save"
#define ARDRONE_UI_WIDGET_NAME_COMBOBOX_DISPLAY_DEVICE     "combobox_display_device"
#define ARDRONE_UI_WIDGET_NAME_COMBOBOX_DISPLAY_FORMAT     "combobox_display_format"
#define ARDRONE_UI_WIDGET_NAME_COMBOBOX_SAVE_FORMAT        "combobox_save_format"
//#define ARDRONE_UI_WIDGET_NAME_CURVE                      "curve"
#define ARDRONE_UI_WIDGET_NAME_DIALOG_ABOUT                "dialog_about"
#define ARDRONE_UI_WIDGET_NAME_DIALOG_MAIN                 "dialog_main"
#define ARDRONE_UI_WIDGET_NAME_ENTRY_ADDRESS               "entry_address"
#define ARDRONE_UI_WIDGET_NAME_FILECHOOSERBUTTON_SAVE      "filechooserbutton_save"
#define ARDRONE_UI_WIDGET_NAME_FRAME_CONFIGURATION         "frame_configuration"
#define ARDRONE_UI_WIDGET_NAME_FRAME_OPTIONS               "frame_options"
#define ARDRONE_UI_WIDGET_NAME_FRAME_OPTIONS_SAVE          "frame_save_options"
#define ARDRONE_UI_WIDGET_NAME_DRAWINGAREA_VIDEO           "drawing_area"
#define ARDRONE_UI_WIDGET_NAME_LISTSTORE_DISPLAY_DEVICE    "liststore_display_device"
#define ARDRONE_UI_WIDGET_NAME_LISTSTORE_DISPLAY_FORMAT    "liststore_display_format"
#define ARDRONE_UI_WIDGET_NAME_LISTSTORE_SAVE_FORMAT       "liststore_save_format"
#define ARDRONE_UI_WIDGET_NAME_PROGRESS_BAR                "progress_bar"
#define ARDRONE_UI_WIDGET_NAME_SPINBUTTON_CONNECTIONS      "spinbutton_connections"
#define ARDRONE_UI_WIDGET_NAME_SPINBUTTON_BUFFERSIZE       "spinbutton_buffersize"
#define ARDRONE_UI_WIDGET_NAME_SPINBUTTON_DATA             "spinbutton_data"
#define ARDRONE_UI_WIDGET_NAME_SPINBUTTON_DATAMESSAGES     "spinbutton_data_messages"
#define ARDRONE_UI_WIDGET_NAME_SPINBUTTON_PORT             "spinbutton_port"
#define ARDRONE_UI_WIDGET_NAME_SPINBUTTON_SESSIONMESSAGES  "spinbutton_session_messages"
#define ARDRONE_UI_WIDGET_NAME_STATUSBAR                   "statusbar"
#define ARDRONE_UI_WIDGET_NAME_TEXTVIEW_LOG                "textview_log"
#define ARDRONE_UI_WIDGET_NAME_TOGGLEACTION_CONNECT        "toggleaction_connect"
#define ARDRONE_UI_WIDGET_NAME_TOGGLEBUTTON_CONNECT        "togglebutton_connect"
//#define ARDRONE_UI_WIDGET_NAME_VBOX_SAVE_OPTIONS           "vbox_save_options"

#define ARDRONE_UI_GTK_PANGO_LOG_FONT_DESCRIPTION          "Monospace 8"
#define ARDRONE_UI_GTK_PANGO_LOG_COLOR_BASE                "#FFFFFF" // white
#define ARDRONE_UI_GTK_PANGO_LOG_COLOR_TEXT                "#000000" // black

#define ARDRONE_UI_STATUSBAR_CONTEXT_DATA                  "data"
#define ARDRONE_UI_STATUSBAR_CONTEXT_INFORMATION           "information"

// *** network-related ***
#define ARDRONE_DEFAULT_NUMBER_OF_DISPATCH_THREADS         5
#define ARDRONE_MAVLINK_PORT                               14550 // UDP (drone --> client)
#define ARDRONE_CONTROL_CONFIGURATION_PORT                 5556 // UDP (client --> drone)
//#define ARDRONE_CONTROL_PORT                               5559 // TCP (client <--> drone)
#define ARDRONE_NAVDATA_PORT                               5554 // UDP (drone --> client)
#define ARDRONE_VIDEO_LIVE_PORT                            5555 // TCP (drone <--> client)
//#define ARDRONE_VIDEO_RECORDING_PORT                       5553 // TCP (drone <--> client)

//#define ARDRONE_SOCKET_RECEIVE_BUFFER_SIZE                 NET_SOCKET_DEFAULT_RECEIVE_BUFFER_SIZE

// *** protocol-related ***
#define ARDRONE_PROTOCOL_AT_PREFIX_STRING                  "AT*"
#define ARDRONE_PROTOCOL_AT_COMMAND_CONFIG_STRING          "CONFIG"
#define ARDRONE_PROTOCOL_AT_COMMAND_FTRIM_STRING           "FTRIM"
#define ARDRONE_PROTOCOL_AT_COMMAND_MAXIMUM_LENGTH         1024 // #characters

#define ARDRONE_PROTOCOL_AT_COMMAND_VIDEO_STRING           "video"
#define ARDRONE_PROTOCOL_AT_COMMAND_PARAMETER_CODEC_STRING "video_codec"

// *** stream-related ***
//#define ARDRONE_MESSAGE_BUFFER_SIZE                        65535 // bytes
#define ARDRONE_MESSAGE_BUFFER_SIZE                        131070 // bytes
//#define ARDRONE_MAXIMUM_QUEUE_SLOTS                       STREAM_QUEUE_MAX_MESSAGES
#define ARDRONE_MAXIMUM_NUMBER_OF_INFLIGHT_MESSAGES        std::numeric_limits<unsigned int>::max ()

#define ARDRONE_STATISTIC_REPORTING_INTERVAL               0 // seconds [0 --> OFF]

// *** device-related ***
//#define ARDRONE_ACCELEROMETER_LSB_FACTOR_2                16384.0F // LSB/g
//#define ARDRONE_THERMOMETER_LSB_FACTOR                    340.0F
//#define ARDRONE_THERMOMETER_OFFSET                        36.53F
//#define ARDRONE_THERMOMETER_RANGE                         125.0F // -40°C - 85°C
//#define ARDRONE_GYROSCOPE_LSB_FACTOR_250                  131.0F // LSB/(°/s)
// *IMPORTANT NOTE*: currently (!) this value must correspond to the actual
//                   device driver setting i.e. TIMER_DELAY_MS (for noint=1)
//#define ARDRONE_DATA_RATE                                 100 // messages/s
// *TODO*: apparently, the Microsoft H264 decoder cannot handle the 720P
//         resolution
//#define ARDRONE_DEFAULT_VIDEO_WIDTH                        1280
//#define ARDRONE_DEFAULT_VIDEO_HEIGHT                       720
#define ARDRONE_DEFAULT_VIDEO_WIDTH                        640
#define ARDRONE_DEFAULT_VIDEO_HEIGHT                       368

// *** application-related ***
//#define ARDRONE_TEMPERATURE_BUFFER_SIZE                   1000
#define ARDRONE_DEFAULT_VIDEO_FULLSCREEN                   false
#define ARDRONE_LOG_FILE_NAME                              "ardrone.log"
#define ARDRONE_MAVLINK_LOG_FILE_PREFIX                    "ardrone_mavlink"
#define ARDRONE_NAVDATA_LOG_FILE_PREFIX                    "ardrone_navdata"
#define ARDRONE_VIDEO_FILE_NAME                            "ardrone.rgb"

#endif // #ifndef ARDRONE_DEFINES_H
