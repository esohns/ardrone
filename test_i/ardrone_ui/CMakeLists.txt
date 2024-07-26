set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/../../modules/Common/src/gl/scripts;${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/gl/scripts")
include (glm)
include (glew)
include (glut)
include (assimp)

set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/../../modules/Common/src/image/scripts;${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/image/scripts")
include (png)

set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/../../modules/Common/scripts;${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/scripts")
include (ImageMagick)
set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/../../modules/Common/src/xml/scripts;${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/xml/scripts")
include (PreXMLImplementation)
include (SetXMLImplementation)

include (CMakeDependentOption)
include (FeatureSummary)

if (UNIX)
# option (WEXT_SUPPORT "enable wireless extensions (aka wext) support" ON)
# option (NL80211_SUPPORT "enable nl80211 support" ON)
# option (DBUS_SUPPORT "enable DBus/NetworkManager support" ON)

 option (DHCLIENT_SUPPORT "enable ISC dhclient support" ON)
elseif (WIN32)
 option (WLANAPI_SUPPORT "enable wlanapi support" ON)
endif ()

if (UNIX)
 include (${CMAKE_CURRENT_SOURCE_DIR}/../../modules/ACENetwork/src/wlan/scripts/PreMonitorImplementation.cmake)
 include (${CMAKE_CURRENT_SOURCE_DIR}/../../modules/ACENetwork/src/wlan/scripts/SetMonitorImplementation.cmake)
 if (WEXT_USE)
  add_definitions (-DWEXT_SUPPORT -DWEXT_USE)
 elseif (NL80211_USE)
  add_definitions (-DNL80211_SUPPORT -DNL80211_USE)
 elseif (DBUS_USE)
  add_definitions (-DDBUS_SUPPORT -DDBUS_USE)
  add_definitions (-DSD_BUS_SUPPORT)
 else ()
  message (FATAL_ERROR "select a WLAN monitor implementation")
 endif ()
elseif (WIN32)

 include (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACENetwork/src/wlan/scripts/PreMonitorImplementation.cmake)
 include (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACENetwork/src/wlan/scripts/SetMonitorImplementation.cmake)
 option (DEBUG_HEAP_SUPPORT "enable CRT debug heap" OFF)
 add_feature_info (debug_heap DEBUG_HEAP_SUPPORT "support debug heap")

 if (WLANAPI_USE)
  add_definitions (-DWLANAPI_SUPPORT -DWLANAPI_USE)
 else ()
  message (FATAL_ERROR "select a WLAN monitor implementation")
 endif ()
endif ()

if (UNIX)
 include (${CMAKE_CURRENT_SOURCE_DIR}/../../modules/ACENetwork/src/protocol/dhcp/scripts/PreDHCPClientImplementation.cmake)
 include (${CMAKE_CURRENT_SOURCE_DIR}/../../modules/ACENetwork/src/protocol/dhcp/scripts/SetDHCPClientImplementation.cmake)
 if (UNIX)
  if (DHCLIENT_USE)
   add_definitions (-DDHCLIENT_SUPPORT -DDHCLIENT_USE)
  elseif (DHCPD_USE)
   add_definitions (-DDHCPD_SUPPORT -DDHCPD_USE)
  elseif (FRAMEWORK_USE)
   add_definitions (-DFRAMEWORK_USE)
  else ()
   message (FATAL_ERROR "select a DHCP client implementation")
  endif ()
# elseif (WIN32)
#  if (WLANAPI_USE)
#   add_definitions (-DWLANAPI_SUPPORT -DWLANAPI_USE)
#  else ()
#   message (FATAL_ERROR "select a WLAN monitor implementation")
#  endif ()
 endif ()
endif (UNIX)

# user interface
option (GUI_SUPPORT "enable graphical user interface support" ON)
if (GUI_SUPPORT)
 add_definitions (-DGUI_SUPPORT)
 include (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/ui/scripts/PreUIFramework.cmake)
 include (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/ui/scripts/SetUIFramework.cmake)

 if (GTK_USE)
  include (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/ui/scripts/SetGTKVersion.cmake)
  CMAKE_DEPENDENT_OPTION (GTKGL_SUPPORT "enable GTK OpenGL support" ON
                          "GUI_SUPPORT" OFF)
# *IMPORTANT NOTE*: to use gtkglarea on gtk2, check out the 'gtkglarea-2' branch
#                   of the project
  CMAKE_DEPENDENT_OPTION (GTKGLAREA_SUPPORT "enable GtkGLArea support" OFF
                          "GUI_SUPPORT; GTKGL_SUPPORT" OFF)
 elseif (WXWIDGETS_SUPPORT)
  message (STATUS "wxWidgets_DEFINITIONS: ${wxWidgets_DEFINITIONS}")
  message (STATUS "wxWidgets_DEFINITIONS_DEBUG: ${wxWidgets_DEFINITIONS_DEBUG}")
  message (STATUS "wxWidgets_CXX_FLAGS: ${wxWidgets_CXX_FLAGS}")
  message (STATUS "wxWidgets_USE_FILE: ${wxWidgets_USE_FILE}")
# # *NOTE*: re-enable ANSI mode, iff available
# add_definitions (-DwxUSE_UNICODE=0)
  foreach (DEFINITION ${wxWidgets_DEFINITIONS})
   add_definitions (-D${DEFINITION})
  endforeach ()
  if (WIN32)
#  add_definitions (-D__WXMSW__)
#  add_definitions (-DWXUSINGDLL)
   if (MSVC)
    add_definitions (-DwxMSVC_VERSION_AUTO)
   endif (MSVC)
  endif (WIN32)
  add_definitions (-DwxUSE_GUI=1)
  if (CMAKE_BUILD_TYPE STREQUAL Debug)
   add_definitions (-D__WXDEBUG__)
  endif (CMAKE_BUILD_TYPE STREQUAL Debug)
 endif ()
endif (GUI_SUPPORT)

if (UNIX)
# include (CheckLibraryExists)
# check_library_exists (pthread pthread_create "" HAVE_LIB_PTHREAD)
 include (FindThreads)
 set (CMAKE_THREAD_PREFER_PTHREAD ON)
 find_package (Threads)
endif ()

include (FindPkgConfig)
if (UNIX)
 # *NOTE*: Common requires libcap
 pkg_check_modules (PKG_CAP REQUIRED libcap)
# pkg_check_modules (PKG_GETTEXT REQUIRED tinygettext)
# pkg_check_modules (PKG_GETTEXT REQUIRED gettext)
 pkg_check_modules (PKG_OPENSSL REQUIRED openssl)
 if (WEXT_USE)
  check_library_exists (iw iw_sockets_open /usr/lib/x86_64-linux-gnu HAVE_LIBIW)
  if (NOT HAVE_LIBIW)
   message (FATAL_ERROR "libiw not found, aborting")
  endif ()
 elseif (NL80211_USE)
  pkg_check_modules (PKG_NL80211 REQUIRED libnl-3.0 libnl-genl-3.0)
  pkg_check_modules (PKG_DBUS REQUIRED dbus-1)
  pkg_check_modules (PKG_LIBSYSTEMD REQUIRED libsystemd)
  add_definitions (-DSD_BUS_SUPPORT)
 elseif (DBUS_USE)
  pkg_check_modules (PKG_DBUS REQUIRED dbus-1)
  pkg_check_modules (PKG_NETWORKMANAGER REQUIRED NetworkManager)
 endif ()
endif (UNIX)

include (ACE)

set (ACE_XML_LIB_FILE libACEXML.so)
if (UNIX)
# message ("CMAKE_FIND_ROOT_PATH is ${CMAKE_FIND_ROOT_PATH}")
 find_library (ACE_XML_LIBRARY ${ACE_XML_LIB_FILE}
               HINTS ${CMAKE_CURRENT_BINARY_DIR}/../../../ATCD/ACE /usr/local
               PATHS ENV ACE_ROOT
               PATH_SUFFIXES lib
               DOC "searching for ${ACE_XML_LIB_FILE}"
               NO_DEFAULT_PATH)
# else ()
#  find_library (ACEXML_LIBRARY ${ACE_XML_LIB_FILE}
#                DOC "searching for ${ACE_XML_LIB_FILE} (system paths)")
# endif ()
elseif (WIN32)
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
  set (LIB_FILE_SUFFIX d)
 endif ()
 unset (ACE_XML_LIB_FILE)
 set (ACE_XML_LIB_FILE ACEXML${LIB_FILE_SUFFIX}.lib)
 find_library (ACE_XML_LIBRARY ${ACE_XML_LIB_FILE}
               PATHS ENV ACE_ROOT
               PATH_SUFFIXES lib lib\\${CMAKE_BUILD_TYPE}\\Win32
               DOC "searching for ${ACE_XML_LIB_FILE}")
endif ()
#if (NOT EXISTS ACE_XML_LIBRARY)
if (NOT ACE_XML_LIBRARY)
 message (FATAL_ERROR "Could not find ${ACE_XML_LIB_FILE} (was: \"${ACE_XML_LIBRARY}\"), aborting")
else ()
 #message (STATUS "Found ACEXML library \"${ACE_XML_LIBRARY}\"")
endif ()
set (ACE_XML_PARSER_LIB_FILE libACEXML_Parser.so)
if (UNIX)
# message ("CMAKE_FIND_ROOT_PATH is ${CMAKE_FIND_ROOT_PATH}")
 find_library (ACE_XML_PARSER_LIBRARY ${ACE_XML_PARSER_LIB_FILE}
               HINTS ${CMAKE_CURRENT_BINARY_DIR}/../../../ATCD/ACE /usr/local
               PATHS ENV ACE_ROOT
               PATH_SUFFIXES lib
               DOC "searching for ${ACE_XML_PARSER_LIB_FILE}"
               NO_DEFAULT_PATH)
# else ()
#  find_library (ACE_XML_PARSER_LIBRARY ${ACE_XML_PARSER_LIB_FILE}
#                DOC "searching for ${ACE_XML_PARSER_LIB_FILE} (system paths)")
# endif ()
elseif (WIN32)
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
  set (LIB_FILE_SUFFIX d)
 endif ()
 unset (ACE_XML_PARSER_LIB_FILE)
 set (ACE_XML_PARSER_LIB_FILE ACEXML_Parser${LIB_FILE_SUFFIX}.lib)
 find_library (ACE_XML_PARSER_LIBRARY ${ACE_XML_PARSER_LIB_FILE}
               PATHS ENV ACE_ROOT
               PATH_SUFFIXES lib lib\\${CMAKE_BUILD_TYPE}\\Win32
               DOC "searching for ${ACE_XML_PARSER_LIB_FILE}")
endif ()
#if (NOT EXISTS ACE_XML_PARSER_LIBRARY)
if (NOT ACE_XML_PARSER_LIBRARY)
 message (FATAL_ERROR "Could not find ${ACE_XML_PARSER_LIB_FILE} (was: \"${ACE_XML_PARSER_LIBRARY}\"), aborting")
else ()
 #message (STATUS "Found ACEXML parser library \"${ACE_XML_PARSER_LIBRARY}\"")
endif ()

if (WIN32)
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
  add_definitions (-DDEBUG) # activate DirectShow debug logging
 endif ()
# set (_PF86 "ProgramFiles(x86)")
 set (DIRECTSHOW_BASECLASSES_DIRECTORY
      "D:/projects/DShowBaseClasses")
#      "$ENV{PROGRAMFILES}/Microsoft SDKs/Windows/v7.0/Samples/multimedia/directshow/baseclasses")
#      "$ENV{${_PF86}}/Microsoft SDKs/Windows/v7.0/Samples/multimedia/directshow/baseclasses")
# message (STATUS "DirectShow baseclasses directory: \"${DIRECTSHOW_BASECLASSES_DIRECTORY}\"")
endif (WIN32)

include (FindGettext)

#pkg_check_modules (PKG_ACE REQUIRED ACE)
if (UNIX)
 pkg_check_modules (PKG_LIBXML REQUIRED libxml-2.0)
 pkg_check_modules (PKG_FFMPEG REQUIRED libavcodec libavutil libavformat libswscale)
 if (GUI_SUPPORT)
  if (GTK_USE)
   pkg_check_modules (PKG_LIBPNG REQUIRED libpng)
   if (GTK3_USE)
    pkg_check_modules (PKG_GTK3 REQUIRED gtk+-3.0)
    if (GTKGL_SUPPORT)
     pkg_check_modules (PKG_OPENGL REQUIRED gl glu)
     pkg_check_modules (PKG_ASSIMP REQUIRED assimp)
    endif (GTKGL_SUPPORT)
   elseif (GTK2_USE)
    pkg_check_modules (PKG_GTK2 REQUIRED gmodule-2.0 gthread-2.0 gtk+-2.0)
    if (GTKGL_SUPPORT)
     pkg_check_modules (PKG_OPENGL REQUIRED gl glu)
     pkg_check_modules (PKG_ASSIMP REQUIRED assimp)
     if (GTKGLAREA_SUPPORT)
#       pkg_check_modules (PKG_GTKGLAREA2 REQUIRED gtkgl-2.0)
      add_definitions (-DGTKGLAREA_SUPPORT)
     else ()
      pkg_check_modules (PKG_GTKGL2 REQUIRED gdkglext-1.0 gtkglext-1.0)
     endif (GTKGLAREA_SUPPORT)
    endif (GTKGL_SUPPORT)
   endif ()
  endif (GTK_USE)
 endif (GUI_SUPPORT)
endif (UNIX)
# *TODO*: repair WIN32 pkgconfig support

if (GUI_SUPPORT)
 if (GTK_USE)
  if (GTKGL_SUPPORT)
   add_definitions (-DGTKGL_SUPPORT)
  endif (GTKGL_SUPPORT)
  if (GTKGLAREA_SUPPORT)
   add_definitions (-DGTKGLAREA_SUPPORT)
  endif (GTKGLAREA_SUPPORT)
  if (LIBGLADE_SUPPORT)
   add_definitions (-DLIBGLADE_SUPPORT)
  endif (LIBGLADE_SUPPORT)
 endif (GTK_USE)
endif (GUI_SUPPORT)

if (GUI_SUPPORT)
 if (GTK_USE)
  if (GTKGL_SUPPORT)
   include (FindOpenGL)
   if (NOT OPENGL_FOUND)
    message (FATAL_ERROR "OpenGL support not found on this system, aborting")
   endif (NOT OPENGL_FOUND)

#   set (GLUT_LIB_FILE libglut.so.3)
#   if (UNIX)
#    find_library (GLUT_LIBRARY ${GLUT_LIB_FILE}
#                  HINTS /usr/lib
#                  PATH_SUFFIXES x86_64-linux-gnu
#                  DOC "searching for ${GLUT_LIB_FILE} (system paths)")
   elseif (WIN32)
#    unset (LIB_FILE_SUFFIX)
#    set (GLUT_LIB_FILE freeglut)
#    if (CMAKE_BUILD_TYPE STREQUAL Debug)
#     set (LIB_FILE_SUFFIX d)
#    endif ()
#    set (GLUT_LIB_FILE ${GLUT_LIB_FILE}${LIB_FILE_SUFFIX}.lib)
#   find_library (GLUT_LIBRARY ${GLUT_LIB_FILE}
#                  PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../../FreeGLUT/freeglut/freeglut/cmake/lib/${CMAKE_BUILD_TYPE}
#                  DOC "searching for ${GLUT_LIB_FILE}")
#   endif ()
#   if (NOT GLUT_LIBRARY)
#    message (FATAL_ERROR "could not find ${GLUT_LIB_FILE} (was: \"${GLUT_LIBRARY}\"), aborting")
#   else ()
 #   message (STATUS "Found Glut library \"${GLUT_LIBRARY}\"")
#   endif () 
#   add_definitions (-DGTKGL_SUPPORT)
  endif (GTKGL_SUPPORT)
 endif (GTK_USE)
endif (GUI_SUPPORT)

get_filename_component (BUILD_PATH ${CMAKE_CURRENT_BINARY_DIR}/.. REALPATH DIRECTORY)
get_filename_component (BUILD_PATH_SUFFIX ${BUILD_PATH} NAME)
#find_package (Common 0.0.1 EXACT
find_package (Common
              REQUIRED
              CONFIG
              PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../../modules ${CMAKE_CURRENT_SOURCE_DIR}/../..
              PATH_SUFFIXES Common/${BUILD_PATH_SUFFIX}
              NO_DEFAULT_PATH)
if (NOT EXISTS ${Common_DIR})
 message (FATAL_ERROR "could not find package \"Common\", aborting")
endif ()
add_definitions (-DCOMMON_HAS_DLL)
if (DBUS_SUPPORT)
 add_definitions (-DCOMMON_DBUS_HAS_DLL)
endif ()

#find_package (ACEStream 0.0.1 EXACT
find_package (ACEStream
              REQUIRED
              CONFIG
              PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../../modules ${CMAKE_CURRENT_SOURCE_DIR}/../..
              PATH_SUFFIXES ACEStream/${BUILD_PATH_SUFFIX}
              NO_DEFAULT_PATH)
if (NOT EXISTS ${ACEStream_DIR})
 message (FATAL_ERROR "could not find package \"ACEStream\", aborting")
endif ()
add_definitions (-DSTREAM_HAS_DLL)
add_definitions (-DSTREAM_DEC_HAS_DLL)

#find_package (ACENetwork 0.0.1 EXACT
find_package (ACENetwork
              REQUIRED
              CONFIG
              PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../../modules ${CMAKE_CURRENT_SOURCE_DIR}/../..
              PATH_SUFFIXES ACENetwork/${BUILD_PATH_SUFFIX}
              NO_DEFAULT_PATH)
if (NOT EXISTS ${ACENetwork_DIR})
 message (FATAL_ERROR "could not find package \"ACENetwork\", aborting")
endif ()
add_definitions (-DNETWORK_HAS_DLL)
add_definitions (-DNETWORK_WLAN_HAS_DLL)

add_definitions (-DHAVE_CONFIG_H)
#add_definitions (-DLIBGLADE_SUPPORT)
if (GUI_SUPPORT)
 if (GTK_USE)
  if (GTKGL_SUPPORT)
   add_definitions (-DGTKGLAREA_SUPPORT)
  endif (GTKGL_SUPPORT)
 endif (GTK_USE)
endif (GUI_SUPPORT)
#set_property (SOURCE PROPERTY COMPILE_DEFINITIONS HAVE_CONFIG_H)

if (WIN32 AND (CMAKE_BUILD_TYPE STREQUAL Debug))
 if (MSVC)
  add_compile_options (/bigobj)
#else ()
#  add_compile_options(-Wa,-mbig-obj)
 endif (MSVC)

 # *NOTE*: DirectShow uses 'DEBUG'
 add_definitions (-DDEBUG) # activate DirectShow debug logging
endif (WIN32 AND (CMAKE_BUILD_TYPE STREQUAL Debug))

if (UNIX)
 include_directories (${PKG_CAP_INCLUDE_DIRS})
 include_directories (${PKG_GETTEXT_INCLUDE_DIRS})
 if (NL80211_USE)
  include_directories (${PKG_NL80211_INCLUDE_DIRS})
 elseif (DBUS_USE)
  include_directories (${PKG_DBUS_INCLUDE_DIRS})
 endif ()

 if (DHCLIENT_USE)
  include_directories (/usr/include/bind-export)
  include_directories (/usr/include/dhcp)
 endif (DHCLIENT_USE)

# include_directories (/usr/share/gettext)
 #include_directories (${PKG_FFMPEG_INCLUDE_DIRS})
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../ffmpeg)

 if (GUI_SUPPORT)
  if (GTK_USE)
   if (GTK3_SUPPORT)
    include_directories (${PKG_GTK3_INCLUDE_DIRS})
   elseif (GTK2_SUPPORT)
    include_directories (${PKG_GTK2_INCLUDE_DIRS})
    include_directories (${PKG_GLADE_INCLUDE_DIRS})
   endif ()
  elseif (WXWIDGETS_USE)
   include_directories (${wxWidgets_INCLUDE_DIRS})
  endif ()
 endif (GUI_SUPPORT)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../mavlink/generated/include)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../ARDrone_SDK_2_0_1/ARDroneLib)
 # *NOTE*: headers in Soft/Common include each other 'relatively' :-(
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../ARDrone_SDK_2_0_1/ARDroneLib/Soft/Common)
 # *NOTE*: headers in Soft/Common include headers in Soft/Lib :-(
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../ARDrone_SDK_2_0_1/ARDroneLib/Soft/Lib)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../ARDrone_SDK_2_0_1/ARDroneLib/VP_SDK)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../ARDrone_SDK_2_0_1/ARDroneLib/VP_SDK/VP_Os/linux)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACE)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/Common/src)
 if (GUI_SUPPORT)
  if (GTK_USE)
   if (GTKGL_SUPPORT)
    include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/Common/src/gl)
   endif (GTKGL_SUPPORT)
  endif (GTK_USE)
 endif (GUI_SUPPORT)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/Common/src/image)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/Common/src/log)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/Common/src/signal)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/Common/src/timer)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/Common/test_u)

 if (GUI_SUPPORT)
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/Common/src/ui)
  if (GTK_USE)
   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/Common/src/ui/gtk)
  elseif (WXWIDGETS_USE)
   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/Common/src/ui/wxwidgets)
  endif ()
 endif (GUI_SUPPORT)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACEStream/src)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACEStream/src/modules/dec)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACEStream/src/modules/dev)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACEStream/src/modules/file)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACEStream/src/modules/lib)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACEStream/src/modules/misc)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACEStream/src/modules/net)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACEStream/src/modules/stat)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACEStream/src/modules/vis)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACEStream/test_u)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACENetwork/3rd_party/bison)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACENetwork/src)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACENetwork/src/client_server)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACENetwork/src/wlan)
elseif (WIN32)
# include_directories (${DIRECTSHOW_BASECLASSES_DIRECTORY})
 include_directories (${DIRECTSHOW_BASECLASSES_DIRECTORY}/baseclasses)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../openssl/include)

 if (GUI_SUPPORT)
  if (GTK_USE)
   if (GTKGL_SUPPORT)
    include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../assimp/cmake/include)
    include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../assimp/include)

    include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../freeglut/include)
    include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../FreeGLUT/freeglut/freeglut/include)

    include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../glm)
   endif (GTKGL_SUPPORT)
  endif (GTK_USE)
 endif (GUI_SUPPORT)

 include_directories ($ENV{LIB_ROOT}/mavlink/generated/include)
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../mavlink/cmake/include/common)

 include_directories ($ENV{LIB_ROOT}/ARDrone_SDK_2_0_1/ARDroneLib)
 # *NOTE*: headers in Soft/Common include each other 'relatively' :-(
 include_directories ($ENV{LIB_ROOT}/ARDrone_SDK_2_0_1/ARDroneLib/Soft/Common)
 # *NOTE*: headers in Soft/Common include headers in Soft/Lib :-(
 include_directories ($ENV{LIB_ROOT}/ARDrone_SDK_2_0_1/ARDroneLib/Soft/Lib)
 include_directories ($ENV{LIB_ROOT}/ARDrone_SDK_2_0_1/ARDroneLib/VP_SDK)
 include_directories ($ENV{LIB_ROOT}/ARDrone_SDK_2_0_1/ARDroneLib/VP_SDK/VP_Os/win32)
 include_directories ($ENV{LIB_ROOT}/ARDrone_SDK_2_0_1/Examples/Win32/VCProjects/ARDrone)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../ffmpeg)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../ATCD/ACE)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src)
 if (GUI_SUPPORT)
  if (GTK_USE)
   if (GTKGL_SUPPORT)
    include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/gl)
   endif (GTKGL_SUPPORT)
  endif (GTK_USE)
 endif (GUI_SUPPORT)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/image)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/log)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/signal)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/timer)

 if (GUI_SUPPORT)
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/ui)
  if (GTK_USE)
   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/ui/gtk)
  elseif (WXWIDGETS_USE)
   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/ui/wxwidgets)
  endif ()
 endif (GUI_SUPPORT)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/xml)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/test_u)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src/modules/dec)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src/modules/dev)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src/modules/file)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src/modules/lib)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src/modules/misc)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src/modules/net)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src/modules/stat)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src/modules/vis)
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/test_u)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACENetwork/3rd_party)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACENetwork/3rd_party/bison)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACENetwork/src)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACENetwork/src/client_server)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACENetwork/src/wlan)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/gettext)
endif ()
include_directories (${CMAKE_CURRENT_BINARY_DIR}/../..)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src)

############## next target ################
set (source_files
     ardrone.cpp
     ardrone_common.cpp
     ardrone_common.h
     ardrone_configuration.h
     ardrone_eventhandler.cpp
     ardrone_eventhandler.h
     ardrone_message.cpp
     ardrone_message.h
     ardrone_module_eventhandler.cpp
     ardrone_module_eventhandler.h
     ardrone_module_eventhandler.inl
     ardrone_modules_common.h
     ardrone_network.h
     ardrone_network_common.h
     ardrone_opengl.cpp
     ardrone_opengl.h
     ardrone_sessionmessage.cpp
     ardrone_sessionmessage.h
     ardrone_signalhandler.cpp
     ardrone_signalhandler.h
     ardrone_signalhandler.inl
     ardrone_stream.cpp
     ardrone_stream.h
     ardrone_stream.inl
     ardrone_stream_common.h)
if (WIN32)
 set (source_files
      ${source_files}
      ardrone_module_mediafoundation.cpp
      ardrone_module_mediafoundation.h
      ardrone_module_mediafoundation.inl)
endif (WIN32)
if (GUI_SUPPORT)
 if (GTK_USE)
  set (source_files
       ${source_files}
       ardrone_callbacks.cpp
       ardrone_callbacks.h)
 elseif (WXWIDGETS_USE)
  set (source_files
       ${source_files}
       ardrone_ui.cpp
       ardrone_ui.h
       ardrone_ui.inl
       ardrone_ui_base.cpp
       ardrone_ui_base.h)
 endif ()
endif (GUI_SUPPORT)
file (GLOB script_files scripts/*.l)

set (LLVM_COMPILER_ID_STRING "Clang")
string (TOLOWER "${LLVM_COMPILER_ID_STRING}" LLVM_COMPILER_ID_STRING_LOWER)
string (TOLOWER "${CMAKE_CXX_COMPILER_ID}" CXX_COMPILER_ID_STRING_LOWER)
if (CXX_COMPILER_ID_STRING_LOWER STREQUAL LLVM_COMPILER_ID_STRING_LOWER)
 add_compile_options (-std=c++11)
 add_definitions (-D_GLIBCXX_USE_CXX11_ABI=0)
endif ()

set (EXECUTABLE ardrone_ui)
set (ui_definition_file "")
if (GUI_SUPPORT)
 if (GTK_USE)
  if (GTK3_USE)
   set (ui_definition_file ${EXECUTABLE}.gtk3)
  elseif (GTK2_USE)
   set (ui_definition_file ${EXECUTABLE}.gtk2)
  endif ()
 elseif (WXWIDGETS_USE)
  if (WIN32)
   add_definitions(-DUNICODE -D_UNICODE)
  endif (WIN32)
  set (ui_definition_file ${EXECUTABLE}.xrc)
 endif ()
endif (GUI_SUPPORT)
add_executable (${EXECUTABLE}
                stdafx.cpp
                stdafx.h
                ${source_files}
                etc/${ui_definition_file}
                ${script_files}
                etc/ardrone.3ds
                etc/ardrone.jpg
                etc/ardrone.png
                etc/ardrone_small.ico
                etc/ardrone_small.png
                etc/settings.ini)
set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_i")
cmake_policy (SET CMP0012 NEW)
if (WIN32 AND NOT BUILD_TEST_I)
 set_target_properties (${EXECUTABLE} PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD TRUE)
endif (WIN32 AND NOT BUILD_TEST_I)

if (WIN32)
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
  add_definitions (-D_UCRT_HEAP_MISMATCH_DETECTION)
 # *NOTE*: DirectShow uses 'DEBUG'
  add_definitions (-DDEBUG)
 endif ()

 if (MSVC)
  add_definitions (-DSTRSAFE_NO_DEPRECATE)
 endif (MSVC)
endif (WIN32)

add_definitions (-DFFMPEG_SUPPORT)

#add_dependencies (${ACESTREAM_TARGET_NAME} ACE)
add_dependencies (${EXECUTABLE} Common)
#if (UNIX)
# add_dependencies (${EXECUTABLE} CommonDBus)
#endif ()
add_dependencies (${EXECUTABLE} CommonImage)
add_dependencies (${EXECUTABLE} CommonLog)
if (GUI_SUPPORT)
 if (GTK_USE)
  if (GTKGL_SUPPORT)
   add_dependencies (${EXECUTABLE} CommonOpenGL)
  endif (GTKGL_SUPPORT)
 endif (GTK_USE)
endif (GUI_SUPPORT)
add_dependencies (${EXECUTABLE} CommonSignal)
add_dependencies (${EXECUTABLE} CommonTimer)
if (GUI_SUPPORT)
 add_dependencies (${EXECUTABLE} CommonUI)
endif (GUI_SUPPORT)
add_dependencies (${EXECUTABLE} Common_Test_U)

add_dependencies (${EXECUTABLE} ACEStream)

add_dependencies (${EXECUTABLE} ACENetwork)

if (UNIX)
#  message (STATUS "CMAKE_CXX_COMPILER_ID: \"${CMAKE_CXX_COMPILER_ID}\"")
# if (CMAKE_COMPILER_IS_GNUCXX)
 if (CMAKE_CXX_COMPILER_ID STREQUAL GNU)
  set (CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed")
 endif ()
endif (UNIX)

target_link_libraries (${EXECUTABLE} ${CMAKE_THREAD_LIBS_INIT})
if (UNIX)
 target_link_libraries (${EXECUTABLE} rt)
 target_link_libraries (${EXECUTABLE} ${PKG_CAP_LIBRARIES})
 if (WEXT_USE)
  target_link_libraries (${EXECUTABLE} iw)
 elseif (NL80211_USE)
  target_link_libraries (${EXECUTABLE} ${PKG_NL80211_LIBRARIES})
  target_link_libraries (${EXECUTABLE} ${PKG_DBUS_LIBRARIES})
  target_link_libraries (${EXECUTABLE} ${PKG_LIBSYSTEMD_LIBRARIES})
 elseif (DBUS_USE)
  target_link_libraries (${EXECUTABLE} ${PKG_DBUS_LIBRARIES})
# link_directories (/usr/local/lib/dbus-1.0)
# target_link_libraries (${EXECUTABLE} /usr/local/lib/dbus-1.0/libdbus-1.so)
# target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/../../../../dbus/dbus/.libs/libdbus-1.so.3)
 endif ()

 if (DHCLIENT_USE)
  target_link_libraries (${EXECUTABLE} ${PKG_OPENSSL_LIBRARIES})
  target_link_libraries (${EXECUTABLE} ${PKG_LIBXML_LIBRARIES})
#  target_link_libraries (${EXECUTABLE} ${DHCPCTL_LIBRARY})
#  target_link_libraries (${EXECUTABLE} ${OMAPI_LIBRARY})
 # target_link_libraries (${EXECUTABLE} ${IRS_LIBRARY})
#  target_link_libraries (${EXECUTABLE} ${DNS_LIBRARY})
#  target_link_libraries (${EXECUTABLE} ${ISC_CFG_LIBRARY})
#  target_link_libraries (${EXECUTABLE} ${ISC_LIBRARY})
#  target_link_libraries (${EXECUTABLE} ${JSON_LIBRARY})
 endif (DHCLIENT_USE)

 if (GUI_SUPPORT)
  if (GTK_USE)
   target_link_libraries (${EXECUTABLE} ${PKG_LIBPNG_LIBRARIES})
  endif (GTK_USE)
 endif (GUI_SUPPORT)
 target_link_libraries (${EXECUTABLE} ${PKG_FFMPEG_LIBRARIES})

# target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../ffmpeg/libavcodec/libavcodec.a)
# target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../ffmpeg/libavformat/libavformat.a)
# target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../ffmpeg/libswresample/libswresample.a)
# target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../ffmpeg/libswscale/libswscale.a)

 if (GUI_SUPPORT)
  if (GTK_USE)
   if (GTKGL_SUPPORT)
    target_link_libraries (${EXECUTABLE} ${PKG_ASSIMP_LIBRARIES})
   endif (GTKGL_SUPPORT)
  endif (GTK_USE)
 endif (GUI_SUPPORT)

 target_link_libraries (${EXECUTABLE} X11)
 target_link_libraries (${EXECUTABLE} dl)
 if (GUI_SUPPORT)
  if (GTK_USE)
   if (GTK3_SUPPORT)
    target_link_libraries (${EXECUTABLE} ${PKG_GTK3_LIBRARIES})
   elseif (GTK2_SUPPORT)
    target_link_libraries (${EXECUTABLE} ${PKG_GTK2_LIBRARIES})
   endif ()
   if (GTKGL_SUPPORT)
    target_link_libraries (${EXECUTABLE} ${PKG_OPENGL_LIBRARIES})
    target_link_libraries (${EXECUTABLE} ${GLUT_LIBRARY})
   endif (GTKGL_SUPPORT)
  endif (GTK_USE)
 endif (GUI_SUPPORT)
# target_link_libraries (${EXECUTABLE} ${PKG_LIBPNG_LIBRARIES})
elseif (WIN32)
 set (CMAKE_EXE_LINKER_FLAGS /NODEFAULTLIB:\"libcmt.lib\")

# target_link_libraries (${EXECUTABLE} ntdll.lib)
 link_directories (${DIRECTSHOW_BASECLASSES_DIRECTORY}/${CMAKE_BUILD_TYPE})
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
#  link_directories (${DIRECTSHOW_BASECLASSES_DIRECTORY}/Debug)
#  target_link_libraries (${EXECUTABLE} ${DIRECTSHOW_BASECLASSES_DIRECTORY}/cmake/baseclasses/Debug/strmbasd.lib)
 else ()
#  link_directories (${DIRECTSHOW_BASECLASSES_DIRECTORY}/Release)
 # target_link_libraries (${EXECUTABLE} ${DIRECTSHOW_BASECLASSES_DIRECTORY}/cmake/baseclasses/Release/strmbase.lib)
 endif ()
 target_link_libraries (${EXECUTABLE} evr.lib)


 target_link_libraries (${EXECUTABLE} ${ffmpeg_LIBRARIES})

# target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../ffmpeg/libavcodec/avcodec.lib)
# target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../ffmpeg/libavcodec/libavcodec.a)
# target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../ffmpeg/libavformat/avformat.lib)
# target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../ffmpeg/libavformat/libavformat.a)
# target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../ffmpeg/libswresample/swresample.lib)
# target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../ffmpeg/libswresample/libswresample.a)
# target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../ffmpeg/libswscale/swscale.lib)
# target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../ffmpeg/libswscale/libswscale.a)

# if (MSXML_USE)
  target_link_libraries (${EXECUTABLE} msxml2.lib)
# elseif (LIBXML2_USE)
  target_link_libraries (${EXECUTABLE} ${LIBXML2_LIBRARY})
# endif ()

 if (GUI_SUPPORT)
  if (GTK_USE)
   if (GTKGL_SUPPORT)
#    target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../assimp/cmake/code/${CMAKE_BUILD_TYPE}/assimpd.lib)
# target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../assimp/cmake/code/${CMAKE_BUILD_TYPE}/assimp-vc140-mt.lib)

    target_link_libraries (${EXECUTABLE} OpenGL32.lib)
    target_link_libraries (${EXECUTABLE} ${GLUT_LIBRARY})
#    target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../libepoxy/cmake/${CMAKE_BUILD_TYPE}/epoxy1_static.lib)
 #   target_link_libraries (${EXECUTABLE} "C:/Documents and Settings/Erik/My Documents/Visual Studio 2010/Projects/gtkglarea/${CMAKE_BUILD_TYPE}/gtkglarea.lib")
   endif (GTKGL_SUPPORT)
  elseif (WXWIDGETS_USE)
   target_link_libraries (${EXECUTABLE} ${wxWidgets_LIBRARIES})
  endif ()
 endif (GUI_SUPPORT)
endif ()

target_link_libraries (${EXECUTABLE} ${ACE_LIBRARY})

target_link_libraries (${EXECUTABLE} ${ACE_XML_LIBRARY})
target_link_libraries (${EXECUTABLE} ${ACE_XML_PARSER_LIBRARY})

target_link_libraries (${EXECUTABLE} Common)
if (UNIX)
 if (DBUS_USE OR NL80211_USE)
  target_link_libraries (${EXECUTABLE} CommonDBus)
 endif (DBUS_USE OR NL80211_USE)
endif (UNIX)
target_link_libraries (${EXECUTABLE} CommonEvent)
target_link_libraries (${EXECUTABLE} CommonImage)
target_link_libraries (${EXECUTABLE} CommonLog)
if (GUI_SUPPORT)
 if (GTK_USE)
  if (GTKGL_SUPPORT)
   target_link_libraries (${EXECUTABLE} CommonOpenGL)
  endif (GTKGL_SUPPORT)
 endif (GTK_USE)
endif (GUI_SUPPORT)
target_link_libraries (${EXECUTABLE} CommonSignal)
target_link_libraries (${EXECUTABLE} CommonTimer)
if (WIN32)
 # *WARNING*: linking CommonUI requires "/FORCE:MULTIPLE""
 set_target_properties (${EXECUTABLE} PROPERTIES LINK_FLAGS "/FORCE:MULTIPLE /SAFESEH:NO")
endif (WIN32)
if (GUI_SUPPORT)
 target_link_libraries (${EXECUTABLE} CommonUI)
 if (GTK_USE)
  target_link_libraries (${EXECUTABLE} CommonUI_GTK)
 elseif (WXWIDGETS_USE)
  target_link_libraries (${EXECUTABLE} CommonUI_wxWidgets)
 endif ()
endif (GUI_SUPPORT)
target_link_libraries (${EXECUTABLE} CommonXML)
#target_link_libraries (${EXECUTABLE} Common_Test_U)

target_link_libraries (${EXECUTABLE} ACEStream)
target_link_libraries (${EXECUTABLE} ACEStream_Decoder)
if (WIN32)
 target_link_libraries (${EXECUTABLE} ACEStream_Device)
endif (WIN32)
target_link_libraries (${EXECUTABLE} ACEStream_File)
if (WIN32)
 target_link_libraries (${EXECUTABLE} ACEStream_MediaFramework)
endif (WIN32)
target_link_libraries (${EXECUTABLE} ACEStream_Miscellaneous)
target_link_libraries (${EXECUTABLE} ACEStream_Network)
target_link_libraries (${EXECUTABLE} ACEStream_Statistic)
target_link_libraries (${EXECUTABLE} ACEStream_Visualization)

target_link_libraries (${EXECUTABLE} ACENetwork)
target_link_libraries (${EXECUTABLE} ACENetwork_Protocol_DHCP)
target_link_libraries (${EXECUTABLE} ACENetwork_WLAN)

target_link_libraries (${EXECUTABLE} ${PROJECT_NAME})

if (UNIX)
 target_link_libraries (${EXECUTABLE} ${PKG_CAP_LIBRARIES})
 if (GUI_SUPPORT)
  if (GTK_USE)
   target_link_libraries (${EXECUTABLE} ${PKG_LIBPNG_LIBRARIES})
  endif (GTK_USE)
 endif (GUI_SUPPORT)

 if (DBUS_USE OR NL80211_USE)
  target_link_libraries (${EXECUTABLE} ${PKG_DBUS_LIBRARIES})
 endif (DBUS_USE OR NL80211_USE)

 if (DHCLIENT_USE)
  target_link_libraries (${EXECUTABLE} ${GEOIP_LIBRARY})
  target_link_libraries (${EXECUTABLE} ${GSS_KRB5_LIBRARY})
  target_link_libraries (${EXECUTABLE} ${KRB5_LIBRARY})
 endif (DHCLIENT_USE)
endif (UNIX)

# add PCH
#set_target_properties (${EXECUTABLE} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
#cotire (${EXECUTABLE})
target_precompile_headers (${EXECUTABLE}
                           PRIVATE stdafx.h)

#if (UNIX)
# add_custom_command (TARGET ${EXECUTABLE}
#                     POST_BUILD
#                     COMMAND ./set_capabilities.sh
#                     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts
#                     COMMENT "adding file capabilites to \"${EXECUTABLE}\"..." VERBATIM)
#endif (UNIX)

#########################################

if (UNIX)
 install (TARGETS ${EXECUTABLE}
          EXPORT ${PROJECT_NAME}Targets
          RUNTIME
          DESTINATION "${INSTALL_BIN_DIR}"
          COMPONENT bin)
elseif (WIN32)
 install (TARGETS ${EXECUTABLE}
          EXPORT ${PROJECT_NAME}Targets
          RUNTIME DESTINATION .
          COMPONENT bin)
endif ()
install (FILES
         etc/ardrone.3ds
         etc/ardrone.gtk2
         etc/ardrone.gtk3
         etc/ardrone.jpg
         etc/ardrone.png
         etc/ardrone_small.ico
         etc/ardrone_small.png
         DESTINATION etc COMPONENT config)
install (FILES
         etc/settings.ini
         DESTINATION etc/gtk-3.0 COMPONENT config)
install (FILES
         scripts/control_scanner.l
         scripts/mavlink_scanner.l
         scripts/navdata_scanner.l
         DESTINATION etc COMPONENT dev)

##########################################

#cmake_policy (SET CMP0053 OLD)
file (TO_NATIVE_PATH ${PROJECT_SOURCE_DIR}/test_i/${EXECUTABLE} WORKING_DIRECTORY)

if (WIN32 AND CMAKE_BUILD_TYPE STREQUAL Debug)
 set (ARGS "-c")
endif (WIN32 AND CMAKE_BUILD_TYPE STREQUAL Debug)
set (ARGS "${ARGS} -l")
if (CMAKE_BUILD_TYPE STREQUAL Debug)
 set (ARGS "${ARGS} -t")
endif (CMAKE_BUILD_TYPE STREQUAL Debug)
if (UNIX)
 if (GUI_SUPPORT)
  set (ARGS "-u./etc/${ui_definition_file} ${ARGS}")
 else ()
  set (ARGS "-u ${ARGS}")
 endif (GUI_SUPPORT)
 set (ENVIRONMENT "GDK_BACKEND=x11")
 set (RUNTIME_LIBRARY_DIRS ${PROJECT_SOURCE_DIR}/../ffmpeg/libavcodec
                           ${PROJECT_SOURCE_DIR}/../ffmpeg/libavutil
                           ${PROJECT_SOURCE_DIR}/../ffmpeg/libswresample
                           ${PROJECT_SOURCE_DIR}/../ffmpeg/libswscale
                           ${PROJECT_SOURCE_DIR}/modules/ATCD/ACE/lib)
elseif (WIN32)
 string (REPLACE "/" "\\\\" WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_i/${EXECUTABLE}) 
# if (LIBXML2_USE)
  set (RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBRARY_DIRS}
                            ${PROJECT_SOURCE_DIR}/../libiconv-1.9.2-1/bin
                            ${PROJECT_SOURCE_DIR}/../libxml2/win32/bin.msvc)
# endif (LIBXML2_USE)
 if (GUI_SUPPORT)
  set (ARGS "-u.\\\\etc\\\\${ui_definition_file} ${ARGS}")

  if (GTK_USE)
   file (TO_CMAKE_PATH "${GTK_LIB_DIRS}" GTK_PATH)
   set (RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBRARY_DIRS}
                              ${GTK_PATH})
  elseif (WXWIDGETS_USE)
   set (RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBRARY_DIRS}
                             ${wxWidgets_LIB_DIR})
  endif ()
 else ()
  set (ARGS "-g ${ARGS}")
 endif (GUI_SUPPORT)
 file (TO_CMAKE_PATH "${GLEW_LIB_DIR}" GLEW_PATH)
 file (TO_CMAKE_PATH "${ASSIMP_LIB_DIR}" ASSIMP_PATH)
 file (TO_CMAKE_PATH "${PNG_LIB_DIR}" PNG_PATH)
  # *NOTE*: ffmpeg was built with (vcpkg-)"libpng" support, so now there is a new dependency to zlib1.dll, liblzma.dll, iconv-2.dll :-(
 file (TO_CMAKE_PATH "$ENV{VCPKG_INSTALLATION_ROOT}/installed/x64-windows/bin" VCPKG_PATH)
 file (TO_CMAKE_PATH "${ffmpeg_LIB_DIR}" FFMPEG_PATH)
 file (TO_CMAKE_PATH "${ImageMagick_LIB_DIR}" IMAGEMAGICK_PATH)
 file (TO_CMAKE_PATH "${ACE_LIB_DIR}" ACE_PATH)
 file (TO_CMAKE_PATH $ENV{LIB_ROOT}/openssl SSL_LIB_PATH)
 set (RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBRARY_DIRS}
                           ${GLEW_PATH}
                           ${ASSIMP_PATH}
                           ${PNG_PATH}
                           ${VCPKG_PATH}
                           ${FFMPEG_PATH}
                           ${IMAGEMAGICK_PATH}
                           ${ACE_PATH}
                           ${SSL_LIB_PATH})
endif ()

if (UNIX)
 set (SHELL_WRAPPER "${PROJECT_SOURCE_DIR}/../Common/scripts/shell_wrapper.sh")
 if (NOT EXISTS ${SHELL_WRAPPER})
  message (FATAL_ERROR "could not find shell wrapper (was: \"${SHELL_WRAPPER}\"), aborting")
 endif ()
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE}" COMMAND)
elseif (WIN32)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE}.exe" COMMAND)
 string (REPLACE "\\" "\\\\" COMMAND ${COMMAND})
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
  if (DEBUG_HEAP_SUPPORT)
  else ()
   set (ENVIRONMENT "${ENVIRONMENT} _NO_DEBUG_HEAP=1")
  endif ()
 endif ()
endif ()
create_target_launcher (${EXECUTABLE}
                        COMMAND ${COMMAND}
                        ARGS ${ARGS}
#                         FORWARD_ARGS
                        RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBRARY_DIRS}
                        WORKING_DIRECTORY ${WORKING_DIRECTORY}
                        ENVIRONMENT ${ENVIRONMENT})
