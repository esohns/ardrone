include (CMakeDependentOption)
include (FeatureSummary)

if (UNIX)
 option (WEXT_SUPPORT "enable wireless extensions (aka wext) support" ON)
 option (NL80211_SUPPORT "enable nl80211 support" ON)
 option (DBUS_SUPPORT "enable DBus/NetworkManager support" ON)

 option (DHCLIENT_SUPPORT "enable ISC dhclient support" ON)
elseif (WIN32)
 option (WLANAPI_SUPPORT "enable wlanapi support" ON)
endif ()

if (UNIX)
 include (${CMAKE_CURRENT_SOURCE_DIR}/../modules/libACENetwork/src/wlan/scripts/SetMonitorImplementation.cmake)
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
 include (${CMAKE_CURRENT_SOURCE_DIR}/../../libACENetwork/src/wlan/scripts/SetMonitorImplementation.cmake)
 option (DEBUG_HEAP_SUPPORT "enable CRT debug heap" OFF)
 add_feature_info (debug_heap DEBUG_HEAP_SUPPORT "support debug heap")

 if (WLANAPI_USE)
  add_definitions (-DWLANAPI_SUPPORT -DWLANAPI_USE)
 else ()
  message (FATAL_ERROR "select a WLAN monitor implementation")
 endif ()
endif ()

if (UNIX)
 include (${CMAKE_CURRENT_SOURCE_DIR}/../modules/libACENetwork/src/wlan/scripts/SetDHCPClientImplementation.cmake)
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

option (GTK_SUPPORT "enable GTK support" ON)
# option (GTK3_SUPPORT "enable Gtk3 support" ON)
 # *NOTE*: implies DHCLIENT_SUPPORT off
CMAKE_DEPENDENT_OPTION (GTK3_SUPPORT "enable GTK 3 support" ON
                        "GTK_SUPPORT" OFF)
CMAKE_DEPENDENT_OPTION (GTK2_SUPPORT "enable GTK 2 support" ON
                        "GTK_SUPPORT; NOT GTK3_SUPPORT" OFF)
CMAKE_DEPENDENT_OPTION (GTKGL_SUPPORT "enable GTK OpenGL support" ON
                        "GTK_SUPPORT" OFF)
# *IMPORTANT NOTE*: to use gtkglarea on gtk2, check out the 'gtkglarea-2' branch
#                   of the project
#option (GTKGLAREA_SUPPORT "enable GtkGLArea support" OFF)
CMAKE_DEPENDENT_OPTION (GTKGLAREA_SUPPORT "enable GtkGLArea support" OFF
                        "GTKGL_SUPPORT" OFF)

if (UNIX)
# include (CheckLibraryExists)
# check_library_exists (pthread pthread_create "" HAVE_LIB_PTHREAD)
 include (FindThreads)
 set (CMAKE_THREAD_PREFER_PTHREAD ON)
 find_package (Threads)
endif ()

include (FindPkgConfig)
if (UNIX)
 # *NOTE*: libCommon requires libcap
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

 if (DHCLIENT_USE)
  set (JSON_LIB_FILE libjson-c.so.3)
  find_library (JSON_LIBRARY ${JSON_LIB_FILE}
                HINTS /usr/lib
                PATH_SUFFIXES x86_64-linux-gnu
                DOC "searching for ${JSON_LIB_FILE} (system paths)")
  if (NOT JSON_LIBRARY)
   message (FATAL_ERROR "could not find ${JSON_LIB_FILE}, aborting")
  else ()
   message (STATUS "Found json library \"${JSON_LIBRARY}\"")
  endif ()

  set (ISC_LIB_FILE libisc.a)
  find_library (ISC_LIBRARY ${ISC_LIB_FILE}
                HINTS /usr/lib64 /usr/lib
                PATH_SUFFIXES dhcp x86_64-linux-gnu
                DOC "searching for ${ISC_LIB_FILE} (system paths)")
  if (NOT ISC_LIBRARY)
   message (FATAL_ERROR "could not find ${ISC_LIB_FILE}, aborting")
  else ()
   message (STATUS "Found isc library \"${ISC_LIBRARY}\"")
  endif ()

  set (ISC_CFG_LIB_FILE libisccfg.a)
  find_library (ISC_CFG_LIBRARY ${ISC_CFG_LIB_FILE}
#               HINTS ../../../dhcpctl/dhcp-4.3.6/bind /usr/lib
                HINTS /usr/lib64 /usr/lib
                PATH_SUFFIXES dhcp x86_64-linux-gnu
                DOC "searching for ${ISC_CFG_LIB_FILE} (system paths)")
  if (NOT ISC_CFG_LIBRARY)
   message (FATAL_ERROR "could not find ${ISC_CFG_LIB_FILE}, aborting")
  else ()
   message (STATUS "Found isccfg library \"${ISC_CFG_LIBRARY}\"")
  endif ()

  set (GEOIP_LIB_FILE libGeoIP.so.1)
  find_library (GEOIP_LIBRARY ${GEOIP_LIB_FILE}
                HINTS /usr/lib
                PATH_SUFFIXES x86_64-linux-gnu
                DOC "searching for ${GEOIP_LIB_FILE} (system paths)")
  if (NOT GEOIP_LIBRARY)
   message (FATAL_ERROR "could not find ${GEOIP_LIB_FILE}, aborting")
  else ()
   message (STATUS "Found GeoIP library \"${GEOIP_LIBRARY}\"")
  endif ()

  set (KRB5_LIB_FILE libkrb5.so.3)
  find_library (KRB5_LIBRARY ${KRB5_LIB_FILE}
                HINTS /usr/lib
                PATH_SUFFIXES x86_64-linux-gnu
                DOC "searching for ${KRB5_LIB_FILE} (system paths)")
  if (NOT KRB5_LIBRARY)
   message (FATAL_ERROR "could not find ${KRB5_LIB_FILE}, aborting")
  else ()
   message (STATUS "Found MIT kerberos library \"${KRB5_LIBRARY}\"")
  endif ()

  set (GSS_KRB5_LIB_FILE libgssapi_krb5.so.2)
  find_library (GSS_KRB5_LIBRARY ${GSS_KRB5_LIB_FILE}
                HINTS /usr/lib
                PATH_SUFFIXES x86_64-linux-gnu
                DOC "searching for ${GSS_KRB5_LIB_FILE} (system paths)")
  if (NOT GSS_KRB5_LIBRARY)
   message (FATAL_ERROR "could not find ${GSS_KRB5_LIB_FILE}, aborting")
  else ()
   message (STATUS "Found MIT kerberos library \"${GSS_KRB5_LIBRARY}\"")
  endif ()

  # set (GSS_LIB_FILE libgss.so.3)
#    set (GSS_LIB_FILE libgssglue.so)
#    find_library (GSS_LIBRARY ${GSS_LIB_FILE}
#                  HINTS /usr/lib64 /usr/lib
#                  PATH_SUFFIXES x86_64-linux-gnu
#                  DOC "searching for ${GSS_LIB_FILE} (system paths)")
#    if (NOT GSS_LIBRARY)
#     message (FATAL_ERROR "could not find ${GSS_LIB_FILE}, aborting")
#    else ()
#     message (STATUS "Found generic security services library \"${GSS_LIBRARY}\"")
#    endif ()

  set (IRS_LIB_FILE libirs.a)
  find_library (IRS_LIBRARY ${IRS_LIB_FILE}
#               HINTS ../../../dhcpctl/dhcp-4.3.6/bind /usr/lib
                HINTS /usr/lib64 /usr/lib
                PATH_SUFFIXES dhcp x86_64-linux-gnu
                DOC "searching for ${IRS_LIB_FILE} (system paths)")
  if (NOT IRS_LIBRARY)
   message (FATAL_ERROR "could not find ${IRS_LIB_FILE}, aborting")
  else ()
   message (STATUS "Found irs library \"${IRS_LIBRARY}\"")
  endif ()

  set (DNS_LIB_FILE libdns.a)
  find_library (DNS_LIBRARY ${DNS_LIB_FILE}
#               HINTS ../../../dhcpctl/dhcp-4.3.6/bind /usr/lib
                HINTS /usr/lib64 /usr/lib
                PATH_SUFFIXES dhcp x86_64-linux-gnu
                DOC "searching for ${DNS_LIB_FILE} (system paths)")
  if (NOT DNS_LIBRARY)
   message (FATAL_ERROR "could not find ${DNS_LIB_FILE}, aborting")
  else ()
   message (STATUS "Found dns library \"${DNS_LIBRARY}\"")
  endif ()

  set (OMAPI_LIB_FILE libomapi.a)
  find_library (OMAPI_LIBRARY ${OMAPI_LIB_FILE}
                HINTS /usr/lib64 /usr/lib
                PATH_SUFFIXES dhcp x86_64-linux-gnu
                DOC "searching for ${OMAPI_LIB_FILE} (system paths)")
  if (NOT OMAPI_LIBRARY)
   message (FATAL_ERROR "could not find ${OMAPI_LIB_FILE}, aborting")
  else ()
   message (STATUS "Found omapi library \"${OMAPI_LIBRARY}\"")
  endif ()

  set (DHCPCTL_LIB_FILE libdhcpctl.a)
  find_library (DHCPCTL_LIBRARY ${DHCPCTL_LIB_FILE}
                HINTS /usr/lib64 /usr/lib
                PATH_SUFFIXES dhcp x86_64-linux-gnu
                DOC "searching for ${DHCPCTL_LIB_FILE} (system paths)")
  if (NOT DHCPCTL_LIBRARY)
   message (FATAL_ERROR "could not find ${DHCPCTL_LIB_FILE}, aborting")
  else ()
   message (STATUS "Found dhcpctl library \"${DHCPCTL_LIBRARY}\"")
  endif ()
# add_library (dhcpctl STATIC IMPORTED)
# set_property (TARGET dhcpctl PROPERTY IMPORTED_LOCATION /usr/lib/libdhcpctl.a)
 endif (DHCLIENT_USE)
endif (UNIX)

set (ACE_LIB_FILE libACE.so)
if (UNIX)
 if (DEFINED ENV{ACE_ROOT})
  find_library (ACE_LIBRARY ${ACE_LIB_FILE}
                PATHS ENV ACE_ROOT
                PATH_SUFFIXES lib build/linux/lib
                DOC "searching for ${ACE_LIB_FILE}")
 else ()
  find_library (ACE_LIBRARY ${ACE_LIB_FILE}
                HINTS ${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACE
#                PATHS ENV ACE_ROOT
                PATH_SUFFIXES lib
                DOC "searching for ${ACE_LIB_FILE} (system paths)")
 endif ()
elseif (WIN32)
 unset (LIB_FILE_SUFFIX)
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
  set (LIB_FILE_SUFFIX d)
 endif ()
 unset (ACE_LIB_FILE)
 set (ACE_LIB_FILE ACE${LIB_FILE_SUFFIX}.lib)
 find_library (ACE_LIBRARY ${ACE_LIB_FILE}
               PATHS ENV ACE_ROOT
               PATH_SUFFIXES lib lib\\${CMAKE_BUILD_TYPE}\\Win32
               DOC "searching for ${ACE_LIB_FILE}")
endif ()
  #if (NOT EXISTS ACE_LIBRARY)
if (NOT ACE_LIBRARY)
 message (FATAL_ERROR "could not find ${ACE_LIB_FILE} (was: \"${ACE_LIBRARY}\"), aborting")
else ()
 message (STATUS "Found ACE library \"${ACE_LIBRARY}\"")
endif ()
add_definitions (-DACE_HAS_DLL)

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
 if (GTK_SUPPORT)
  pkg_check_modules (PKG_LIBPNG REQUIRED libpng)
#  pkg_check_modules (PKG_FFMPEG REQUIRED libavcodec libavutil libavformat libswscale)
  pkg_check_modules (PKG_ASSIMP REQUIRED assimp)
  if (GTK3_SUPPORT)
   pkg_check_modules (PKG_GTK3 REQUIRED gtk+-3.0)
   if (GTKGL_SUPPORT)
    pkg_check_modules (PKG_OPENGL REQUIRED gl glu)
   endif (GTKGL_SUPPORT)
   #  if (GTKGL_SUPPORT AND GTKGLAREA_SUPPORT)
#   pkg_check_modules (PKG_GTKGLAREA3 REQUIRED gtkgl-3.0)
#  endif ()
  else ()
   pkg_check_modules (PKG_GTK2 REQUIRED gmodule-2.0 gthread-2.0 gtk+-2.0)
   if (GTKGL_SUPPORT)
     pkg_check_modules (PKG_OPENGL REQUIRED gl glu)
     if (GTKGLAREA_SUPPORT)
#    pkg_check_modules (PKG_GTKGLAREA2 REQUIRED gtkgl-2.0)
     add_definitions (-DGTKGLAREA_SUPPORT)
    else ()
     pkg_check_modules (PKG_GTKGL2 REQUIRED gdkglext-1.0 gtkglext-1.0)
    endif (GTKGLAREA_SUPPORT)
   endif (GTKGL_SUPPORT)
  endif (GTK3_SUPPORT)
 endif (GTK_SUPPORT)
endif (UNIX)
# *TODO*: repair WIN32 pkgconfig support

if (GTKGL_SUPPORT)
 add_definitions (-DGTKGL_SUPPORT)
endif (GTKGL_SUPPORT)
if (GTKGLAREA_SUPPORT)
 add_definitions (-DGTKGLAREA_SUPPORT)
endif (GTKGLAREA_SUPPORT)
if (LIBGLADE_SUPPORT)
 add_definitions (-DLIBGLADE_SUPPORT)
endif (LIBGLADE_SUPPORT)

if (GTKGL_SUPPORT)
 include (FindOpenGL)
 if (NOT OPENGL_FOUND)
  message (FATAL_ERROR "OpenGL support not found on this system, aborting")
 endif ()

 set (GLUT_LIB_FILE libglut.so.3)
 if (UNIX)
  find_library (GLUT_LIBRARY ${GLUT_LIB_FILE}
                HINTS /usr/lib
                PATH_SUFFIXES x86_64-linux-gnu
                DOC "searching for ${GLUT_LIB_FILE} (system paths)")
 elseif (WIN32)
  unset (LIB_FILE_SUFFIX)
  set (GLUT_LIB_FILE freeglut)
  if (CMAKE_BUILD_TYPE STREQUAL Debug)
   set (LIB_FILE_SUFFIX d)
  endif ()
  set (GLUT_LIB_FILE ${GLUT_LIB_FILE}${LIB_FILE_SUFFIX}.lib)
 # message (STATUS "${GLUT_LIB_FILE} search path: \"${CMAKE_CURRENT_SOURCE_DIR}/../../../FreeGLUT/freeglut/freeglut/cmake/lib/${CMAKE_BUILD_TYPE}\"")
  find_library (GLUT_LIBRARY ${GLUT_LIB_FILE}
                PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../../FreeGLUT/freeglut/freeglut/cmake/lib/${CMAKE_BUILD_TYPE}
                DOC "searching for ${GLUT_LIB_FILE}")
 endif ()
 #if (NOT EXISTS GLUT_LIBRARY)
 if (NOT GLUT_LIBRARY)
  message (FATAL_ERROR "could not find ${GLUT_LIB_FILE} (was: \"${GLUT_LIBRARY}\"), aborting")
 else ()
  message (STATUS "Found Glut library \"${GLUT_LIBRARY}\"")
 endif ()
 add_definitions (-DGTKGL_SUPPORT)
endif (GTKGL_SUPPORT)

get_filename_component (BUILD_PATH ${CMAKE_CURRENT_BINARY_DIR}/.. REALPATH DIRECTORY)
get_filename_component (BUILD_PATH_SUFFIX ${BUILD_PATH} NAME)
#find_package (Common 0.0.1 EXACT
find_package (Common
              REQUIRED
              CONFIG
              PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../../modules ${CMAKE_CURRENT_SOURCE_DIR}/../..
              PATH_SUFFIXES libCommon/${BUILD_PATH_SUFFIX}
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
              PATH_SUFFIXES libACEStream/${BUILD_PATH_SUFFIX}
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
              PATH_SUFFIXES libACENetwork/${BUILD_PATH_SUFFIX}
              NO_DEFAULT_PATH)
if (NOT EXISTS ${ACENetwork_DIR})
 message (FATAL_ERROR "could not find package \"ACENetwork\", aborting")
endif ()
add_definitions (-DNETWORK_HAS_DLL)
add_definitions (-DNETWORK_WLAN_HAS_DLL)

add_definitions (-DHAVE_CONFIG_H)
#add_definitions (-DLIBGLADE_SUPPORT)
if (GTKGL_SUPPORT)
 if (GTK3_SUPPORT)
  add_definitions (-DGTKGLAREA_SUPPORT)
 endif ()
endif ()
#set_property (SOURCE PROPERTY COMPILE_DEFINITIONS HAVE_CONFIG_H)

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
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../ffmpeg)

 if (GTK3_SUPPORT)
  include_directories (${PKG_GTK3_INCLUDE_DIRS})
 else ()
  include_directories (${PKG_GTK2_INCLUDE_DIRS})
  include_directories (${PKG_GLADE_INCLUDE_DIRS})
 endif (GTK3_SUPPORT)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../mavlink/generated/include)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../ARDrone_SDK_2_0_1/ARDrone_SDK_2_0_1/ARDroneLib)
 # *NOTE*: headers in Soft/Common include each other 'relatively' :-(
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../ARDrone_SDK_2_0_1/ARDrone_SDK_2_0_1/ARDroneLib/Soft/Common)
 # *NOTE*: headers in Soft/Common include headers in Soft/Lib :-(
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../ARDrone_SDK_2_0_1/ARDrone_SDK_2_0_1/ARDroneLib/Soft/Lib)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../ARDrone_SDK_2_0_1/ARDrone_SDK_2_0_1/ARDroneLib/VP_SDK)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../ARDrone_SDK_2_0_1/ARDrone_SDK_2_0_1/ARDroneLib/VP_SDK/VP_Os/linux)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/ACE)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/libCommon/src)
 #include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../modules/libCommon/src/dbus)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/libCommon/src/gl)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/libCommon/src/image)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/libCommon/src/timer)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/libCommon/src/ui)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/libCommon/src/ui/gtk)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/libACEStream/src)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/libACEStream/src/modules/dec)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/libACEStream/src/modules/dev)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/libACEStream/src/modules/file)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/libACEStream/src/modules/lib)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/libACEStream/src/modules/misc)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/libACEStream/src/modules/net)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/libACEStream/src/modules/stat)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/libACEStream/src/modules/vis)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/libACENetwork/3rd_party/bison)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/libACENetwork/src)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/libACENetwork/src/client_server)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/libACENetwork/src/wlan)
elseif (WIN32)
# include_directories (${DIRECTSHOW_BASECLASSES_DIRECTORY})
 include_directories (${DIRECTSHOW_BASECLASSES_DIRECTORY}/baseclasses)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../openssl/include)

 # *TODO*: repair pkgconfig support
# gtk
 if (GTK_SUPPORT)
  if (GTK3_SUPPORT)
   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../gtk3/include/atk-1.0)
   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../gtk3/include/gdk-pixbuf-2.0)
   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../gtk3/include/cairo)
   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../gtk3/include/pango-1.0)
   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../gtk3/lib/glib-2.0/include)
   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../gtk3/include/glib-2.0)
   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../gtk3/include/gtk-3.0)
  else ()
   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../gtk2/include/atk-1.0)
   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../gtk2/include/gdk-pixbuf-2.0)
   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../gtk2/include/cairo)
   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../gtk2/include/pango-1.0)
   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../gtk2/lib/glib-2.0/include)
   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../gtk2/include/glib-2.0)
   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../gtk2/lib/gtk-2.0/include)
   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../gtk2/include/gtk-2.0)
  endif ()
  if (GTKGL_SUPPORT)
   if (GTKGLAREA_SUPPORT)
    include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtkglarea)
   else ()
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtkglext)
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtkglext/gdk)
   endif (GTKGLAREA_SUPPORT)
  endif (GTKGL_SUPPORT)
 endif (GTK_SUPPORT)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../freeglut/include)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../FreeGLUT/freeglut/freeglut/include)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../glm)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../assimp/cmake/include)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../assimp/include)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../mavlink/generated/include)
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../mavlink/cmake/include/common)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../ARDrone_SDK_2_0_1/ARDrone_SDK_2_0_1/ARDroneLib)
 # *NOTE*: headers in Soft/Common include each other 'relatively' :-(
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../ARDrone_SDK_2_0_1/ARDrone_SDK_2_0_1/ARDroneLib/Soft/Common)
 # *NOTE*: headers in Soft/Common include headers in Soft/Lib :-(
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../ARDrone_SDK_2_0_1/ARDrone_SDK_2_0_1/ARDroneLib/Soft/Lib)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../ARDrone_SDK_2_0_1/ARDrone_SDK_2_0_1/ARDroneLib/VP_SDK)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../ARDrone_SDK_2_0_1/ARDrone_SDK_2_0_1/ARDroneLib/VP_SDK/VP_Os/win32)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../ARDrone_SDK_2_0_1/ARDrone_SDK_2_0_1/Examples/Win32/VCProjects/ARDrone)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../ffmpeg)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../ATCD/ACE)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../libCommon/src)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../libCommon/src/gl)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../libCommon/src/image)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../libCommon/src/timer)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../libCommon/src/ui)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../libCommon/src/ui/gtk)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../libCommon/src/xml)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../libACEStream/src)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../libACEStream/src/modules/dec)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../libACEStream/src/modules/dev)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../libACEStream/src/modules/file)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../libACEStream/src/modules/lib)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../libACEStream/src/modules/misc)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../libACEStream/src/modules/net)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../libACEStream/src/modules/stat)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../libACEStream/src/modules/vis)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../libACENetwork/3rd_party)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../libACENetwork/3rd_party/bison)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../libACENetwork/src)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../libACENetwork/src/client_server)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../libACENetwork/src/wlan)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/gettext)
endif ()
include_directories (${CMAKE_CURRENT_BINARY_DIR}/..)

############## next target ################
set (source_files
     ardrone.cpp
     ardrone_callbacks.cpp
     ardrone_callbacks.h
     ardrone_common.cpp
     ardrone_common.h
     ardrone_configuration.h
     ardrone_control_scanner.cpp
     ardrone_control_scanner.h
     ardrone_defines.h
     ardrone_eventhandler.cpp
     ardrone_eventhandler.h
     ardrone_macros.h
     ardrone_message.cpp
     ardrone_message.h
     ardrone_mavlink_scanner.cpp
     ardrone_mavlink_scanner.h
     ardrone_module_control_decoder.cpp
     ardrone_module_control_decoder.h
     ardrone_module_control_decoder.inl
     ardrone_module_controller.cpp
     ardrone_module_controller.h
     ardrone_module_controller.inl
     ardrone_module_eventhandler.cpp
     ardrone_module_eventhandler.h
     ardrone_module_eventhandler.inl
     ardrone_module_mavlink_decoder.cpp
     ardrone_module_mavlink_decoder.h
     ardrone_module_mavlink_decoder.inl
     ardrone_module_navdata_decoder.cpp
     ardrone_module_navdata_decoder.h
     ardrone_module_navdata_decoder.inl
     ardrone_module_pave_decoder.cpp
     ardrone_module_pave_decoder.h
     ardrone_module_pave_decoder.inl
     ardrone_modules_common.h
     ardrone_navdata_scanner.cpp
     ardrone_navdata_scanner.h
     ardrone_network.h
     ardrone_network_common.h
     ardrone_opengl.cpp
     ardrone_opengl.h
     ardrone_sessionmessage.cpp
     ardrone_sessionmessage.h
     ardrone_signalhandler.cpp
     ardrone_signalhandler.h
     ardrone_signalhandler.inl
     ardrone_statemachine_navdata.cpp
     ardrone_statemachine_navdata.h
     ardrone_stream.cpp
     ardrone_stream.h
     ardrone_stream.inl
     ardrone_stream_common.h
     ardrone_types.h)
file (GLOB script_files scripts/*.l)

set (LLVM_COMPILER_ID_STRING "Clang")
string (TOLOWER "${LLVM_COMPILER_ID_STRING}" LLVM_COMPILER_ID_STRING_LOWER)
string (TOLOWER "${CMAKE_CXX_COMPILER_ID}" CXX_COMPILER_ID_STRING_LOWER)
if (CXX_COMPILER_ID_STRING_LOWER STREQUAL LLVM_COMPILER_ID_STRING_LOWER)
 add_compile_options (-std=c++11)
 add_definitions (-D_GLIBCXX_USE_CXX11_ABI=0)
endif ()

set (EXECUTABLE ardrone)
add_executable (${EXECUTABLE}
                stdafx.cpp
                stdafx.h
                ${source_files}
                ${script_files}
                etc/ardrone.glade
                etc/settings.ini)

if (WIN32)
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
  add_definitions (-D_UCRT_HEAP_MISMATCH_DETECTION)
 # *NOTE*: DirectShow uses 'DEBUG'
  add_definitions (-DDEBUG)
 endif ()
endif (WIN32)

add_definitions (-DFFMPEG_SUPPORT)

#add_dependencies (${ACESTREAM_TARGET_NAME} ACE)
add_dependencies (${EXECUTABLE} Common)
#if (UNIX)
# add_dependencies (${EXECUTABLE} CommonDBus)
#endif ()
add_dependencies (${EXECUTABLE} CommonImage)
add_dependencies (${EXECUTABLE} CommonOpenGL)
add_dependencies (${EXECUTABLE} CommonTimer)
add_dependencies (${EXECUTABLE} CommonUI)

add_dependencies (${EXECUTABLE} ACEStream)

add_dependencies (${EXECUTABLE} ACENetwork)

if (UNIX)
  message (STATUS "CMAKE_CXX_COMPILER_ID: \"${CMAKE_CXX_COMPILER_ID}\"")
# if (CMAKE_COMPILER_IS_GNUCXX)
 if (CMAKE_CXX_COMPILER_ID STREQUAL GNU)
  set (CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed")
 endif ()
endif (UNIX)

target_link_libraries (${EXECUTABLE} ${CMAKE_THREAD_LIBS_INIT})
if (UNIX)
 target_link_libraries (${EXECUTABLE} rt)
 target_link_libraries (${EXECUTABLE} ${PKG_CAP_LIBRARIES})
 target_link_libraries (${EXECUTABLE} iw)
 if (NL80211_USE)
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
  target_link_libraries (${EXECUTABLE} ${DHCPCTL_LIBRARY})
  target_link_libraries (${EXECUTABLE} ${OMAPI_LIBRARY})
  target_link_libraries (${EXECUTABLE} ${IRS_LIBRARY})
  target_link_libraries (${EXECUTABLE} ${DNS_LIBRARY})
  target_link_libraries (${EXECUTABLE} ${ISC_CFG_LIBRARY})
  target_link_libraries (${EXECUTABLE} ${ISC_LIBRARY})
  target_link_libraries (${EXECUTABLE} ${JSON_LIBRARY})
 endif (DHCLIENT_USE)

 if (GTK_SUPPORT)
  target_link_libraries (${EXECUTABLE} ${PKG_LIBPNG_LIBRARIES})
 # target_link_libraries (${EXECUTABLE} ${PKG_FFMPEG_LIBRARIES})
  target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../ffmpeg/libavcodec/libavcodec.a)
  target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../ffmpeg/libavformat/libavformat.a)
  target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../ffmpeg/libswresample/libswresample.a)
  target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../ffmpeg/libswscale/libswscale.a)
  target_link_libraries (${EXECUTABLE} ${PKG_ASSIMP_LIBRARIES})
  target_link_libraries (${EXECUTABLE} X11)
  target_link_libraries (${EXECUTABLE} dl)
  if (GTK3_SUPPORT)
   target_link_libraries (${EXECUTABLE} ${PKG_GTK3_LIBRARIES})
  else ()
   target_link_libraries (${EXECUTABLE} ${PKG_GTK2_LIBRARIES})
  endif (GTK3_SUPPORT)
  if (GTKGL_SUPPORT)
   target_link_libraries (${EXECUTABLE} ${PKG_OPENGL_LIBRARIES})
   target_link_libraries (${EXECUTABLE} ${GLUT_LIBRARY})
  endif (GTKGL_SUPPORT)
 endif (GTK_SUPPORT)
# target_link_libraries (${EXECUTABLE} ${PKG_LIBPNG_LIBRARIES})
elseif (WIN32)
 set (CMAKE_EXE_LINKER_FLAGS /NODEFAULTLIB:\"libcmt.lib\")

# target_link_libraries (${EXECUTABLE} ntdll.lib)
 link_directories (${DIRECTSHOW_BASECLASSES_DIRECTORY}/${CMAKE_BUILD_TYPE})
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
#  link_directories (${DIRECTSHOW_BASECLASSES_DIRECTORY}/Debug)
  target_link_libraries (${EXECUTABLE} ${DIRECTSHOW_BASECLASSES_DIRECTORY}/cmake/baseclasses/Debug/strmbasd.lib)
 else ()
#  link_directories (${DIRECTSHOW_BASECLASSES_DIRECTORY}/Release)
  target_link_libraries (${EXECUTABLE} ${DIRECTSHOW_BASECLASSES_DIRECTORY}/cmake/baseclasses/Release/strmbase.lib)
 endif ()
 target_link_libraries (${EXECUTABLE} evr.lib)

# target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../ffmpeg/libavcodec/avcodec.lib)
 target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../ffmpeg/libavcodec/libavcodec.a)
# target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../ffmpeg/libavformat/avformat.lib)
 target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../ffmpeg/libavformat/libavformat.a)
# target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../ffmpeg/libswresample/swresample.lib)
 target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../ffmpeg/libswresample/libswresample.a)
# target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../ffmpeg/libswscale/swscale.lib)
 target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../ffmpeg/libswscale/libswscale.a)

 target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../assimp/cmake/code/${CMAKE_BUILD_TYPE}/assimpd.lib)
# target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../assimp/cmake/code/${CMAKE_BUILD_TYPE}/assimp-vc140-mt.lib)
# *TODO*: repair WIN32 pkgconfig support
 if (GTK_SUPPORT)
  if (GTK3_SUPPORT)
   target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../gtk3/lib/gio-2.0.lib)
   target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../gtk3/lib/glib-2.0.lib)
   target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../gtk3/lib/gobject-2.0.lib)
   target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../gtk3/lib/gthread-2.0.lib)
   target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../gtk3/lib/gdk-3.0.lib)
   target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../gtk3/lib/gtk-3.0.lib)
   target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../gtk3/lib/pango-1.0.lib)
  else ()
   target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../gtk2/lib/libgio-2.0.dll.a)
   target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../gtk2/lib/libglib-2.0.dll.a)
   target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../gtk2/lib/libgobject-2.0.dll.a)
   target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../gtk2/lib/libgthread-2.0.dll.a)
   target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../gtk2/lib/libgdk-win32-2.0.dll.a)
   target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../gtk2/lib/libgtk-win32-2.0.dll.a)
   target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../gtk2/lib/libpango-1.0.dll.a)
  endif ()
  if (GTKGL_SUPPORT)
   target_link_libraries (${EXECUTABLE} ${OPENGL_LIBRARIES})
#   target_link_libraries (${EXECUTABLE} OpenGL32.lib)
   target_link_libraries (${EXECUTABLE} "C:/Documents and Settings/Erik/My Documents/Visual Studio 2010/Projects/gtkglarea/${CMAKE_BUILD_TYPE}/gtkglarea.lib")
  endif (GTKGL_SUPPORT)
 endif (GTK_SUPPORT)

 # target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtkglext/gtk/.libs/libgtkglext-win32-1.0.dll.a)
 # target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtkglext/gdk/.libs/libgdkglext-win32-1.0.dll.a)
 target_link_libraries (${EXECUTABLE} ${GLUT_LIBRARY})
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
target_link_libraries (${EXECUTABLE} CommonImage)
target_link_libraries (${EXECUTABLE} CommonOpenGL)
target_link_libraries (${EXECUTABLE} CommonTimer)
if (WIN32)
 # *WARNING*: linking CommonUI requires "/FORCE:MULTIPLE""
 set_target_properties (${EXECUTABLE} PROPERTIES LINK_FLAGS "/FORCE:MULTIPLE /SAFESEH:NO")
endif (WIN32)
target_link_libraries (${EXECUTABLE} CommonUI)
target_link_libraries (${EXECUTABLE} CommonUI_GTK)

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
target_link_libraries (${EXECUTABLE} ACENetwork_WLAN)

if (UNIX)
 target_link_libraries (${EXECUTABLE} ${PKG_CAP_LIBRARIES})
 target_link_libraries (${EXECUTABLE} ${PKG_LIBPNG_LIBRARIES})

 if (DBUS_USE OR NL80211_USE)
  target_link_libraries (${EXECUTABLE} ${PKG_DBUS_LIBRARIES})
 endif (DBUS_USE OR NL80211_USE)

 if (DHCLIENT_USE)
  target_link_libraries (${EXECUTABLE} ${GEOIP_LIBRARY})
  target_link_libraries (${EXECUTABLE} ${GSS_KRB5_LIBRARY})
  target_link_libraries (${EXECUTABLE} ${KRB5_LIBRARY})
 endif (DHCLIENT_USE)

# target_link_libraries (${EXECUTABLE} ACENetwork_WLAN)
# target_link_libraries (${EXECUTABLE} ACENetwork)
# target_link_libraries (${EXECUTABLE} Common)
endif (UNIX)

# add PCH
set_target_properties (${EXECUTABLE} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
cotire (${EXECUTABLE})

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
         etc/ardrone.glade
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

if (UNIX)
 set (SHELL_WRAPPER "${PROJECT_SOURCE_DIR}/../libCommon/scripts/shell_wrapper.sh")
 if (NOT EXISTS ${SHELL_WRAPPER})
  message (FATAL_ERROR "could not find shell wrapper (was: \"${SHELL_WRAPPER}\"), aborting")
 endif ()
 create_target_launcher (${EXECUTABLE}
                         ARGS "-l -t"
#                         FORWARD_ARGS
                         RUNTIME_LIBRARY_DIRS ${PROJECT_SOURCE_DIR}/modules/ATCD/ACE/lib
                                              ${PROJECT_SOURCE_DIR}/../gtk/gtk/Win32/debug/bin
                                              ${PROJECT_SOURCE_DIR}/../libCommon/cmake/src/Debug
                                              ${PROJECT_SOURCE_DIR}/../libCommon/cmake/src/ui/Debug
                                              ${PROJECT_SOURCE_DIR}/cmake/src/Debug
                                              ${PROJECT_SOURCE_DIR}/cmake/src/modules/dev/Debug
                                              /usr/local/lib
                         WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/../src
                         ENVIRONMENT foo=bar)
elseif (WIN32)
 set (ENVIRONMENT "")
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
  if (DEBUG_HEAP_SUPPORT)
  else ()
   set (ENVIRONMENT "${ENVIRONMENT} _NO_DEBUG_HEAP=1 foo=bar")
  endif ()
 endif ()

 if (GTK3_SUPPORT)
  create_target_launcher (${EXECUTABLE}
                          ARGS "-c -l -t"
#                        FORWARD_ARGS
                          RUNTIME_LIBRARY_DIRS ${PROJECT_SOURCE_DIR}/../ATCD/ACE/lib
#                                              ${PROJECT_SOURCE_DIR}/../openssl/bin
                                               ${PROJECT_SOURCE_DIR}/../FreeGLUT/freeglut/freeglut/cmake/bin/${CMAKE_BUILD_TYPE}
                                               ${PROJECT_SOURCE_DIR}/../ffmpeg/libavcodec
                                               ${PROJECT_SOURCE_DIR}/../ffmpeg/libavformat
                                               ${PROJECT_SOURCE_DIR}/../ffmpeg/libavutil
                                               ${PROJECT_SOURCE_DIR}/../ffmpeg/libswscale
                                               ${PROJECT_SOURCE_DIR}/../ffmpeg/libswresample
                                               ${PROJECT_SOURCE_DIR}/../gtk/gtk/Win32/debug/bin
                                               ${PROJECT_SOURCE_DIR}/../assimp/cmake/code/${CMAKE_BUILD_TYPE}
                                               G:/software/Development/dll
                                               ${PROJECT_SOURCE_DIR}/../libCommon/cmake/src/${CMAKE_BUILD_TYPE}
                                               ${PROJECT_SOURCE_DIR}/../libCommon/cmake/src/err/${CMAKE_BUILD_TYPE}
#                                               ${PROJECT_SOURCE_DIR}/../libCommon/cmake/src/image/${CMAKE_BUILD_TYPE}
#                                               ${PROJECT_SOURCE_DIR}/../libCommon/cmake/src/math/${CMAKE_BUILD_TYPE}
                                               ${PROJECT_SOURCE_DIR}/../libCommon/cmake/src/ui/${CMAKE_BUILD_TYPE}
                                               ${PROJECT_SOURCE_DIR}/../libACEStream/cmake/src/${CMAKE_BUILD_TYPE}
                                               ${PROJECT_SOURCE_DIR}/../libACEStream/cmake/src/modules/dec/${CMAKE_BUILD_TYPE}
                                               ${PROJECT_SOURCE_DIR}/../libACEStream/cmake/src/modules/dev/${CMAKE_BUILD_TYPE}
                                               ${PROJECT_SOURCE_DIR}/../libACEStream/cmake/src/modules/file/${CMAKE_BUILD_TYPE}
                                               ${PROJECT_SOURCE_DIR}/../libACEStream/cmake/src/modules/misc/${CMAKE_BUILD_TYPE}
                                               ${PROJECT_SOURCE_DIR}/../libACEStream/cmake/src/modules/net/${CMAKE_BUILD_TYPE}
                                               ${PROJECT_SOURCE_DIR}/../libACEStream/cmake/src/modules/stat/${CMAKE_BUILD_TYPE}
                                               ${PROJECT_SOURCE_DIR}/../libACEStream/cmake/src/modules/vis/${CMAKE_BUILD_TYPE}
#                                              ${PROJECT_SOURCE_DIR}/cmake/src/${CMAKE_BUILD_TYPE}
                          WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/../src
                          ENVIRONMENT ${ENVIRONMENT})
 else ()
  create_target_launcher (${EXECUTABLE}
                          ARGS "-c -l -t"
#                        FORWARD_ARGS
                          RUNTIME_LIBRARY_DIRS ${PROJECT_SOURCE_DIR}/../ATCD/ACE/lib
#                                              ${PROJECT_SOURCE_DIR}/../openssl/bin
                                               ${PROJECT_SOURCE_DIR}/../FreeGLUT/freeglut/freeglut/cmake/bin/${CMAKE_BUILD_TYPE}
                                               ${PROJECT_SOURCE_DIR}/../ffmpeg/libavcodec
                                               ${PROJECT_SOURCE_DIR}/../ffmpeg/libavformat
                                               ${PROJECT_SOURCE_DIR}/../ffmpeg/libavutil
                                               ${PROJECT_SOURCE_DIR}/../ffmpeg/libswscale
                                               ${PROJECT_SOURCE_DIR}/../ffmpeg/libswresample
                                               ${PROJECT_SOURCE_DIR}/../gtk2/bin
                                               G:/software/Development/dll
                                               ${PROJECT_SOURCE_DIR}/../libCommon/cmake/src/${CMAKE_BUILD_TYPE}
                                               ${PROJECT_SOURCE_DIR}/../libCommon/cmake/src/err/${CMAKE_BUILD_TYPE}
#                                               ${PROJECT_SOURCE_DIR}/../libCommon/cmake/src/image/${CMAKE_BUILD_TYPE}
#                                               ${PROJECT_SOURCE_DIR}/../libCommon/cmake/src/math/${CMAKE_BUILD_TYPE}
                                               ${PROJECT_SOURCE_DIR}/../libCommon/cmake/src/ui/${CMAKE_BUILD_TYPE}
                                               ${PROJECT_SOURCE_DIR}/../libACEStream/cmake/src/${CMAKE_BUILD_TYPE}
                                               ${PROJECT_SOURCE_DIR}/../libACEStream/cmake/src/modules/dec/${CMAKE_BUILD_TYPE}
                                               ${PROJECT_SOURCE_DIR}/../libACEStream/cmake/src/modules/dev/${CMAKE_BUILD_TYPE}
                                               ${PROJECT_SOURCE_DIR}/../libACEStream/cmake/src/modules/file/${CMAKE_BUILD_TYPE}
                                               ${PROJECT_SOURCE_DIR}/../libACEStream/cmake/src/modules/misc/${CMAKE_BUILD_TYPE}
                                               ${PROJECT_SOURCE_DIR}/../libACEStream/cmake/src/modules/net/${CMAKE_BUILD_TYPE}
                                               ${PROJECT_SOURCE_DIR}/../libACEStream/cmake/src/modules/stat/${CMAKE_BUILD_TYPE}
                                               ${PROJECT_SOURCE_DIR}/../libACEStream/cmake/src/modules/vis/${CMAKE_BUILD_TYPE}
                                               ${PROJECT_SOURCE_DIR}/cmake/src/${CMAKE_BUILD_TYPE}
                          WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/../src
                          ENVIRONMENT ${ENVIRONMENT})
 endif ()
endif ()
