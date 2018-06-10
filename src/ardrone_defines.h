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

#include "ace/config-lite.h"

// *** OpenGL-related
#define ARDRONE_OPENGL_RAD_PER_DEG                         0.0174533F
#define ARDRONE_OPENGL_AXES_SIZE                           0.2 // percentage --> 10%
// fonts
//#define ARDRONE_OPENGL_FONT_AXES                          GLUT_BITMAP_HELVETICA_18
#define ARDRONE_OPENGL_FONT_AXES                           GLUT_STROKE_MONO_ROMAN
//#define ARDRONE_OPENGL_FONT_FPS                            GLUT_BITMAP_HELVETICA_12
// camera calibration
#define ARDRONE_OPENGL_CAMERA_ROTATION_FACTOR              0.8F
#define ARDRONE_OPENGL_CAMERA_TRANSLATION_FACTOR           0.01F
#define ARDRONE_OPENGL_CAMERA_ZOOM_FACTOR                  0.03F
// defaults
#define ARDRONE_OPENGL_CAMERA_DEFAULT_Z                    -7.0F
// perspective(s)
#define ARDRONE_OPENGL_PERSPECTIVE_FOVY                    45.0
#define ARDRONE_OPENGL_PERSPECTIVE_ZNEAR                   1.0
#define ARDRONE_OPENGL_PERSPECTIVE_ZFAR                    1000.0
#define ARDRONE_OPENGL_ORTHO_ZNEAR                         -1.0
#define ARDRONE_OPENGL_ORTHO_ZFAR                          1.0

// model
#define ARDRONE_OPENGL_MODEL_ORIENTATION_PRECISION         3
#define ARDRONE_OPENGL_MODEL_DEFAULT_FILE                  "ardrone.3ds"
//#define ARDRONE_OPENGL_TEXTURE_DEFAULT_FILE                  "ardrone.png"

// *** Gtk UI-related ***
#define ARDRONE_UI_DEFINITION_FILE_NAME                    "ardrone.glade"
#define ARDRONE_UI_INITIALIZATION_DELAY                    100 // ms
//#define ARDRONE_UI_WIDGET_CURVE_MAXIMUM_Y                 100.0F
// *IMPORTANT NOTE*: also sets the (maximum) rate of message processing
//#define ARDRONE_UI_WIDGET_GL_REFRESH_INTERVAL              1000.0F / 30.0F // Hz
#define ARDRONE_UI_PROCESSING_THREAD_NAME                  "streams processor"

//#define ARDRONE_DEFAULT_UI_WIDGET_DRAWINGAREA_VIDEO_WIDTH  320
//#define ARDRONE_DEFAULT_UI_WIDGET_DRAWINGAREA_VIDEO_HEIGHT 240
#define ARDRONE_UI_WIDGET_NAME_ACTION_CALIBRATE                   "action_calibrate"
#define ARDRONE_UI_WIDGET_NAME_ACTION_CUT                         "action_cut"
#define ARDRONE_UI_WIDGET_NAME_ACTION_TRIM                        "action_trim"
#define ARDRONE_UI_WIDGET_NAME_BOX_ORIENTATION                    "box_orientation"
#define ARDRONE_UI_WIDGET_NAME_BUTTON_ABOUT                       "button_about"
#define ARDRONE_UI_WIDGET_NAME_BUTTON_CLEAR                       "button_clear"
#define ARDRONE_UI_WIDGET_NAME_BUTTON_QUIT                        "button_quit"
#define ARDRONE_UI_WIDGET_NAME_CHECKBUTTON_ASSOCIATE              "checkbutton_associate"
#define ARDRONE_UI_WIDGET_NAME_CHECKBUTTON_ASYNCH                 "checkbutton_asynch"
#define ARDRONE_UI_WIDGET_NAME_CHECKBUTTON_SAVE                   "checkbutton_save"
#define ARDRONE_UI_WIDGET_NAME_CHECKBUTTON_VIDEO                  "checkbutton_video"
#define ARDRONE_UI_WIDGET_NAME_COMBOBOX_DISPLAY_DEVICE            "combobox_display_device"
#define ARDRONE_UI_WIDGET_NAME_COMBOBOX_DISPLAY_FORMAT            "combobox_display_format"
#define ARDRONE_UI_WIDGET_NAME_COMBOBOX_SAVE_FORMAT               "combobox_save_format"
#define ARDRONE_UI_WIDGET_NAME_COMBOBOX_WLAN_INTERFACE            "combobox_wlan_interface"
#define ARDRONE_UI_WIDGET_NAME_DIALOG_ABOUT                       "dialog_about"
#define ARDRONE_UI_WIDGET_NAME_DIALOG_MAIN                        "dialog_main"
#define ARDRONE_UI_WIDGET_NAME_DRAWINGAREA_FULLSCREEN             "drawingarea_fullscreen"
#define ARDRONE_UI_WIDGET_NAME_DRAWINGAREA_OPENGL                 "drawingarea_opengl"
#define ARDRONE_UI_WIDGET_NAME_DRAWINGAREA_VIDEO                  "drawingarea_video"
//#define ARDRONE_UI_WIDGET_NAME_ENTRY_ADDRESS                      "entry_address"
#define ARDRONE_UI_WIDGET_NAME_ENTRY_SSID                         "entry_SSID"
#define ARDRONE_UI_WIDGET_NAME_ENTRYBUFFER_ADDRESS                "entrybuffer_address"
#define ARDRONE_UI_WIDGET_NAME_ENTRYBUFFER_SSID                   "entrybuffer_SSID"
#define ARDRONE_UI_WIDGET_NAME_FILECHOOSERBUTTON_SAVE             "filechooserbutton_save"
#define ARDRONE_UI_WIDGET_NAME_FRAME_CONFIGURATION                "frame_configuration"
#define ARDRONE_UI_WIDGET_NAME_FRAME_DISPLAY                      "frame_display"
#define ARDRONE_UI_WIDGET_NAME_FRAME_OPTIONS                      "frame_options"
#define ARDRONE_UI_WIDGET_NAME_FRAME_OPTIONS_SAVE                 "frame_save_options"
#define ARDRONE_UI_WIDGET_NAME_LABEL_ROLL                         "label_roll_value"
#define ARDRONE_UI_WIDGET_NAME_LABEL_PITCH                        "label_pitch_value"
#define ARDRONE_UI_WIDGET_NAME_LABEL_YAW                          "label_yaw_value"
#define ARDRONE_UI_WIDGET_NAME_LISTSTORE_DISPLAY_DEVICE           "liststore_display_device"
#define ARDRONE_UI_WIDGET_NAME_LISTSTORE_DISPLAY_FORMAT           "liststore_display_format"
#define ARDRONE_UI_WIDGET_NAME_LISTSTORE_SAVE_FORMAT              "liststore_save_format"
#define ARDRONE_UI_WIDGET_NAME_LISTSTORE_WLAN_INTERFACE           "liststore_wlan_interface"
#define ARDRONE_UI_WIDGET_NAME_PROGRESSBAR                        "progressbar"
#define ARDRONE_UI_WIDGET_NAME_PROGRESSBAR_CACHE                  "progressbar_cache"
#define ARDRONE_UI_WIDGET_NAME_PROGRESSBAR_CONTROL                "progressbar_control"
#define ARDRONE_UI_WIDGET_NAME_PROGRESSBAR_NAVDATA                "progressbar_navdata"
#define ARDRONE_UI_WIDGET_NAME_PROGRESSBAR_MAVLINK                "progressbar_mavlink"
#define ARDRONE_UI_WIDGET_NAME_PROGRESSBAR_VIDEO                  "progressbar_video"
#define ARDRONE_UI_WIDGET_NAME_SPINBUTTON_CONNECTIONS             "spinbutton_connections"
#define ARDRONE_UI_WIDGET_NAME_SPINBUTTON_BUFFERSIZE              "spinbutton_buffersize"
#define ARDRONE_UI_WIDGET_NAME_SPINBUTTON_DATA                    "spinbutton_data"
#define ARDRONE_UI_WIDGET_NAME_SPINBUTTON_DATAMESSAGES_CONTROL    "spinbutton_data_messages_control"
#define ARDRONE_UI_WIDGET_NAME_SPINBUTTON_DATAMESSAGES_MAVLINK    "spinbutton_data_messages_mavlink"
#define ARDRONE_UI_WIDGET_NAME_SPINBUTTON_DATAMESSAGES_NAVDATA    "spinbutton_data_messages_navdata"
#define ARDRONE_UI_WIDGET_NAME_SPINBUTTON_DATAMESSAGES_VIDEO      "spinbutton_data_messages_video"
#define ARDRONE_UI_WIDGET_NAME_SPINBUTTON_PORT                    "spinbutton_port"
#define ARDRONE_UI_WIDGET_NAME_SPINBUTTON_SESSIONMESSAGES_CONTROL "spinbutton_session_messages_control"
#define ARDRONE_UI_WIDGET_NAME_SPINBUTTON_SESSIONMESSAGES_MAVLINK "spinbutton_session_messages_mavlink"
#define ARDRONE_UI_WIDGET_NAME_SPINBUTTON_SESSIONMESSAGES_NAVDATA "spinbutton_session_messages_navdata"
#define ARDRONE_UI_WIDGET_NAME_SPINBUTTON_SESSIONMESSAGES_VIDEO   "spinbutton_session_messages_video"
#define ARDRONE_UI_WIDGET_NAME_SPINNER                            "spinner"
#define ARDRONE_UI_WIDGET_NAME_STATUSBAR                          "statusbar"
#define ARDRONE_UI_WIDGET_NAME_TOGGLEACTION_CONNECT               "toggleaction_connect"
#define ARDRONE_UI_WIDGET_NAME_TOGGLEACTION_FULLSCREEN            "toggleaction_fullscreen"
#define ARDRONE_UI_WIDGET_NAME_TOGGLEACTION_ORIENTATION           "toggleaction_orientation"
#define ARDRONE_UI_WIDGET_NAME_WINDOW_FULLSCREEN                  "window_fullscreen"

// pango
//#define ARDRONE_UI_GTK_PANGO_LOG_FONT_DESCRIPTION          "Monospace 8"
//#define ARDRONE_UI_GTK_PANGO_LOG_COLOR_BASE                "#FFFFFF" // white
//#define ARDRONE_UI_GTK_PANGO_LOG_COLOR_TEXT                "#000000" // black

#define ARDRONE_UI_STATUSBAR_CONTEXT_DATA                         "data"
#define ARDRONE_UI_STATUSBAR_CONTEXT_INFORMATION                  "information"

// *** network-related ***
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#elif defined (ACE_LINUX)
#if defined (NL80211_SUPPORT)
#define ARDRONE_DEFAULT_WLAN_AUTHENTICATION                NL80211_AUTHTYPE_OPEN_SYSTEM
#endif // NL80211_SUPPORT
#endif // ACE_LINUX
#define ARDRONE_DEFAULT_WLAN_SSID                          "ardrone2_291480"
#define ARDRONE_DEFAULT_WLAN_FREQUENCY                     2437 // MHz
#if defined (ACE_WIN32) || defined (ACE_WIN64)
#else
#define ARDRONE_DEFAULT_WLAN_SSID_BYTE_ARRAY               { 'a', 'r', 'd', 'r', 'o', 'n', 'e', '2', '_', '2', '9', '1', '4', '8', '0' }
#endif
#define ARDRONE_DEFAULT_IP_ADDRESS                         "192.168.1.1"

#define ARDRONE_DEFAULT_NUMBER_OF_DISPATCH_THREADS         5
#define ARDRONE_PORT_TCP_CAPTURE                           5557
#define ARDRONE_PORT_TCP_CONSOLE                           5558
#define ARDRONE_PORT_TCP_CONTROL                           5559
#define ARDRONE_PORT_TCP_FTP                               5551
#define ARDRONE_PORT_TCP_TELNET                            23
#define ARDRONE_PORT_TCP_VIDEO                             5555
#define ARDRONE_PORT_UDP_CONTROL_CONFIGURATION             5556  // UDP (drone <-- client)
#define ARDRONE_PORT_UDP_MAVLINK                           14550 // UDP (drone --> client)
// *NOTE*: this isn't really documented anywhere, but the procedure resembles a
//         subscriber pattern: the client sends 4 bytes '0x0001' to this port;
//         the drone will then send NavData to the clients' 'source' port, i.e.
//         the port the subscription originated from
//         --> setting the source port to 5556 (see above) will give the nice
//             impression of a 'bidirectional' NavData communication 'channel'
#define ARDRONE_PORT_UDP_NAVDATA                           5554  // UDP (drone <-- client)

//#define ARDRONE_SOCKET_RECEIVE_BUFFER_SIZE                 NET_SOCKET_DEFAULT_RECEIVE_BUFFER_SIZE

// *** protocol-related ***
//#define ARDRONE_PROTOCOL_DEFAULT_SESSION_ID                1
//#define ARDRONE_PROTOCOL_DEFAULT_USER_ID                   1
//#define ARDRONE_PROTOCOL_DEFAULT_APPLICATION_ID            1

// *** stream-related ***
//#define ARDRONE_STREAM_MDOULE_CONTROLLER_NAME_STRING        "Controller"
#define ARDRONE_STREAM_MDOULE_HANDLER_NAME_STRING           "EventHandler"
#define ARDRONE_STREAM_MDOULE_PAVE_DECODER_NAME_STRING      "PaVEDecoder"

// state-machine
#define ARDRONE_STATEMACHINE_NAVDATA_NAME                   "ARDroneStateMachineNavData"

// control
#define ARDRONE_PROTOCOL_AT_COMMAND_MAXIMUM_LENGTH          1024 // #characters

#define ARDRONE_PROTOCOL_AT_PREFIX_STRING                   "AT*"
#define ARDRONE_PROTOCOL_AT_COMMAND_CONFIG_STRING           "CONFIG"
#define ARDRONE_PROTOCOL_AT_COMMAND_CONFIG_IDS_STRING       "CONFIG_IDS"
#define ARDRONE_PROTOCOL_AT_COMMAND_CONTROL_STRING          "CTRL"
#define ARDRONE_PROTOCOL_AT_COMMAND_FTRIM_STRING            "FTRIM"
#define ARDRONE_PROTOCOL_AT_COMMAND_RESET_WATCHDOG_STRING   "COMWDG"

#define ARDRONE_PROTOCOL_AT_COMMAND_CATEGORY_GENERAL_STRING "general"
#define ARDRONE_PROTOCOL_AT_COMMAND_CATEGORY_VIDEO_STRING   "video"
#define ARDRONE_PROTOCOL_AT_COMMAND_SETTING_MODE_STRING     "navdata_demo"
#define ARDRONE_PROTOCOL_AT_COMMAND_SETTING_CODEC_STRING    "video_codec"
#define ARDRONE_PROTOCOL_AT_COMMAND_SETTING_OPTIONS_STRING  "navdata_options"

#define ARDRONE_CONTROL_STREAM_NAME_STRING                  "ControlStream"

// navdata
#if defined (_DEBUG)
#define ARDRONE_PROTOCOL_FULL_NAVDATA_OPTIONS               true
#else
#define ARDRONE_PROTOCOL_FULL_NAVDATA_OPTIONS               false
#endif

// *** stream-related ***
//#define ARDRONE_MESSAGE_BUFFER_SIZE                        65535 // bytes
#define ARDRONE_MESSAGE_BUFFER_SIZE                        131070 // bytes
//#define ARDRONE_MAXIMUM_QUEUE_SLOTS                       STREAM_QUEUE_MAX_MESSAGES
#define ARDRONE_MAXIMUM_NUMBER_OF_INFLIGHT_MESSAGES        std::numeric_limits<unsigned int>::max ()

#define ARDRONE_NAVDATA_STREAM_NAME_STRING                 "NavDataStream"

//#define ARDRONE_STATISTIC_REPORTING_INTERVAL               0 // seconds [0 --> OFF]

#define ARDRONE_MAVLINK_STREAM_NAME_STRING                 "MAVLinkStream"
#define ARDRONE_VIDEO_STREAM_NAME_STRING                   "VideoStream"

// *** device-related ***
//#define ARDRONE_ACCELEROMETER_LSB_FACTOR_2                16384.0F // LSB/g
//#define ARDRONE_THERMOMETER_LSB_FACTOR                    340.0F
//#define ARDRONE_THERMOMETER_OFFSET                        36.53F
//#define ARDRONE_THERMOMETER_RANGE                         125.0F // -40°C - 85°C
//#define ARDRONE_GYROSCOPE_LSB_FACTOR_250                  131.0F // LSB/(°/s)
// *IMPORTANT NOTE*: currently (!) this value must correspond to the actual
//                   device driver setting i.e. TIMER_DELAY_MS (for noint=1)
//#define ARDRONE_DATA_RATE                                 100 // messages/s
// *TODO*: do not use static values; compute these with ffmpeg
#define ARDRONE_H264_360P_VIDEO_WIDTH                      640
// *TODO*: for some reason, the drone MPEG codec returns 368 lines, not 360;
//         --> find out why
#define ARDRONE_H264_360P_VIDEO_HEIGHT                     368
#define ARDRONE_H264_720P_VIDEO_WIDTH                      1280
#define ARDRONE_H264_720P_VIDEO_HEIGHT                     720

// *** application-related ***
#define ARDRONE_CONFIGURATION_DIRECTORY                    "etc"
#define ARDRONE_LOG_FILE_NAME                              "ardrone.log"
#define ARDRONE_CONTROL_LOG_FILE_PREFIX                    "ardrone_control"
#define ARDRONE_MAVLINK_LOG_FILE_PREFIX                    "ardrone_mavlink"
#define ARDRONE_NAVDATA_LOG_FILE_PREFIX                    "ardrone_navdata"
#define ARDRONE_VIDEO_FILE_NAME                            "ardrone.avi"
#define ARDRONE_VIDEO_RAW_FILE_NAME                        "ardrone.rgb"

#if defined (ACE_WIN32) || defined (ACE_WIN64)
#define ARDRONE_DEFAULT_WLAN_ENABLE_MEDIASTREAMING         true
#endif
#define ARDRONE_DEFAULT_WLAN_SSID_AUTOASSOCIATE            true
#define ARDRONE_DEFAULT_VIDEO_DISPLAY                      true
#define ARDRONE_DEFAULT_VIDEO_FULLSCREEN                   false
#define ARDRONE_DEFAULT_VIDEO_MODE                         ARDRONE_VIDEOMODE_360P

#endif // #ifndef ARDRONE_DEFINES_H
