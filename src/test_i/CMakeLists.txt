option (GTK3_SUPPORT "enable Gtk3 support"
        ON)
option (GTKGL_SUPPORT "enable Gtk OpenGL support"
        OFF)

if (UNIX)
# include (CheckLibraryExists)
# check_library_exists (pthread pthread_create "" HAVE_LIB_PTHREAD)
 find_package (Threads REQUIRED)
endif ()

set (ACE_LIB_FILE libACE.so)
if (UNIX)
 if (DEFINED ENV{ACE_ROOT})
  find_library (ACE_LIBRARY ${ACE_LIB_FILE}
                PATHS ENV ACE_ROOT
                PATH_SUFFIXES ace build/linux/ace
                DOC "searching for ${ACE_LIB_FILE}")
 else ()
  find_library (ACE_LIBRARY ${ACE_LIB_FILE}
                DOC "searching for ${ACE_LIB_FILE} (system paths)")
 endif ()
elseif (WIN32)
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

include (FindOpenGL)
if (NOT OPENGL_FOUND)
 set (OPENGL_LIB_FILE libGL.so)
 if (UNIX)
#  find_library (OPENGL_LIBRARY
#                NAMES ${OPENGL_LIB_FILE}
#                PATHS /usr/lib/x86_64-linux-gnu
#                DOC "searching for ${OPENGL_LIB_FILE}")
 endif ()
# if (NOT OPENGL_LIBRARY)
#  message (FATAL_ERROR "could not find ${OPENGL_LIB_FILE} (was: \"${OPENGL_LIBRARY}\"), aborting")
# else ()
#  message (STATUS "Found OpenGL library \"${OPENGL_LIBRARY}\"")
# endif ()
endif ()

set (GLUT_LIB_FILE libglut.so.3)
if (UNIX)
 find_library (GLUT_LIBRARY ${GLUT_LIB_FILE}
               DOC "searching for ${GLUT_LIB_FILE} (system paths)")
elseif (WIN32)
 set (GLUT_LIB_FILE freeglut)
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
  set (LIB_FILE_SUFFIX d)
 endif ()
 set (GLUT_LIB_FILE ${GLUT_LIB_FILE}${LIB_FILE_SUFFIX}.lib)
# message (STATUS "${GLUT_LIB_FILE} search path: \"${CMAKE_CURRENT_SOURCE_DIR}/../../../FreeGLUT/freeglut/freeglut/cmake/lib/${CMAKE_BUILD_TYPE}\"")
 find_library (GLUT_LIBRARY ${GLUT_LIB_FILE}
               PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../../../FreeGLUT/freeglut/freeglut/cmake/lib/${CMAKE_BUILD_TYPE}
               DOC "searching for ${GLUT_LIB_FILE}")
endif ()
#if (NOT EXISTS GLUT_LIBRARY)
if (NOT GLUT_LIBRARY)
 message (FATAL_ERROR "could not find ${GLUT_LIB_FILE} (was: \"${GLUT_LIBRARY}\"), aborting")
else ()
 message (STATUS "Found Glut library \"${GLUT_LIBRARY}\"")
endif ()

include (FindGettext)
include (FindPkgConfig)

#pkg_check_modules (PKG_ACE REQUIRED ACE)
if (UNIX)
# *NOTE*: libCommon requires libcap
 pkg_check_modules (PKG_CAP REQUIRED libcap)
 pkg_check_modules (PKG_FFMPEG REQUIRED libavcodec libavutil libswscale)
 if (GTK3_SUPPORT)
  pkg_check_modules (PKG_GTK3 REQUIRED gtk+-3.0)
 else ()
# pkg_check_modules (PKG_GTK_GLADE REQUIRED libpng gmodule-2.0 gthread-2.0 gtk+-2.0 gtkglext-1.0 libglade-2.0)
  pkg_check_modules (PKG_GTK2 REQUIRED libpng gmodule-2.0 gthread-2.0 gtk+-2.0 gtkglext-1.0)
 endif ()
 pkg_check_modules (PKG_OPENGL REQUIRED gl glu)
endif ()
# *TODO*: repair WIN32 pkgconfig support

find_package (Common ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO}
              REQUIRED
              CONFIG
              PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../../modules ${CMAKE_CURRENT_SOURCE_DIR}/../../..
              PATH_SUFFIXES libCommon/cmake
              NO_DEFAULT_PATH)
if (NOT EXISTS ${Common_DIR})
 message (FATAL_ERROR "could not find package \"Common\", aborting")
endif ()
add_definitions (-DCOMMON_HAS_DLL)

find_package (ACEStream ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO}
              REQUIRED
              CONFIG
              PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../..modules ${CMAKE_CURRENT_SOURCE_DIR}/../../..
              PATH_SUFFIXES libACEStream/cmake
              NO_DEFAULT_PATH)
if (NOT EXISTS ${ACEStream_DIR})
 message (FATAL_ERROR "could not find package \"ACEStream\", aborting")
endif ()
add_definitions (-DSTREAM_HAS_DLL)

find_package (ACENetwork ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO}
              REQUIRED
              CONFIG
              PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../..modules ${CMAKE_CURRENT_SOURCE_DIR}/../../..
              PATH_SUFFIXES libACENetwork/cmake
              NO_DEFAULT_PATH)
if (NOT EXISTS ${ACENetwork_DIR})
 message (FATAL_ERROR "could not find package \"ACENetwork\", aborting")
endif ()
add_definitions (-DNETWORK_HAS_DLL)

add_definitions (-DHAVE_CONFIG_H)
#add_definitions (-DLIBGLADE_SUPPORT)
if (GTKGL_SUPPORT)
 if (GTK3_SUPPORT)
  add_definitions (-DGTKGLAREA_SUPPORT)
 endif ()
endif ()
#set_property (SOURCE PROPERTY COMPILE_DEFINITIONS HAVE_CONFIG_H)

if (UNIX)
 include_directories (/usr/share/gettext)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../modules/ACE)
 include_directories (${PKG_CAP_INCLUDE_DIRS})
 if (GTK3_SUPPORT)
  include_directories (${PKG_GTK3_INCLUDE_DIRS})
 else ()
  include_directories (${PKG_GTK2_INCLUDE_DIRS})
 endif ()
 include_directories (${PKG_GLADE_INCLUDE_DIRS})
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../modules/libCommon/src)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../modules/libCommon/src/ui)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../modules/libACEStream/src)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../modules/libACEStream/src/modules/dec)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../modules/libACEStream/src/modules/dev)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../modules/libACEStream/src/modules/file)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../modules/libACEStream/src/modules/misc)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../modules/libACEStream/src/modules/net)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../modules/libACEStream/src/modules/vis)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../modules/libACENetwork/src)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../modules/libACENetwork/src/client_server)
 include_directories (${CMAKE_CURRENT_BINARY_DIR}/../..)
elseif (WIN32)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../FreeGLUT/freeglut/freeglut/include)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../glm)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ffmpeg)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../mavlink/cmake/include/minimal)
 include_directories ("$ENV{PROGRAMFILES}/Microsoft SDKs/Windows/v7.0/Samples/multimedia/directshow/baseclasses")
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ATCD/ACE)
 # *TODO*: repair pkgconfig support
# gtk
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/gtk/Win32/debug/include/atk-1.0)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/gtk/Win32/debug/include/gdk-pixbuf-2.0)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/gtk/Win32/debug/include/cairo)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/gtk/Win32/debug/include/pango-1.0)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/gtk/Win32/debug/lib/glib-2.0/include)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/gtk/Win32/debug/include/glib-2.0)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/gtk/Win32/debug/include/gtk-3.0)
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtkglarea)
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtkglext)
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtkglext/gdk)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../glm)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../freeglut/include)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ardronelib/ARDroneLib/VP_SDK)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ardronelib/ARDroneLib/Soft/Common)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../libCommon/src)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../libCommon/src/ui)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../libACEStream/src)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../libACEStream/src/modules/dec)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../libACEStream/src/modules/dev)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../libACEStream/src/modules/file)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../libACEStream/src/modules/misc)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../libACEStream/src/modules/net)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../libACEStream/src/modules/vis)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../libACENetwork/src)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../libACENetwork/src/client_server)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../libACENetwork/3rd_party/bison)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/gettext)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../include)
endif ()

############## next target ################
set (ardrone_EXE_SRCS
     ardrone.cpp
     ardrone_callbacks.cpp
     ardrone_callbacks.h
     ardrone_common.cpp
     ardrone_common.h
     ardrone_configuration.h
     ardrone_defines.h
     ardrone_eventhandler.cpp
     ardrone_eventhandler.h
     ardrone_macros.h
     ardrone_message.cpp
     ardrone_message.h
     ardrone_mavlink_scanner.cpp
     ardrone_mavlink_scanner.h
     ardrone_module_eventhandler.cpp
     ardrone_module_eventhandler.h
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
     ardrone_opengl.cpp
     ardrone_opengl.h
     ardrone_sessionmessage.cpp
     ardrone_sessionmessage.h
     ardrone_signalhandler.cpp
     ardrone_signalhandler.h
     ardrone_stream.cpp
     ardrone_stream.h
     ardrone_stream.inl
     ardrone_stream_common.h
     ardrone_types.h
     stdafx.cpp
     stdafx.h)
set (ardrone_SCRIPTS
     scripts/mavlink_scanner.l
     scripts/navdata_scanner.l)

set (EXECUTABLE ardrone)
add_executable (${EXECUTABLE}
                ${ardrone_EXE_SRCS}
                ${ardrone_SCRIPTS}
                etc/ardrone.glade)

if (WIN32 AND (CMAKE_BUILD_TYPE STREQUAL Debug))
 # *NOTE*: DirectShow uses 'DEBUG'
 add_definitions (-DDEBUG)
endif ()

#add_dependencies (${ACESTREAM_TARGET_NAME} ACE)
#add_dependencies (${PROJECT_NAME} Common)
#add_dependencies (${PROJECT_NAME} ACEStream)
#add_dependencies (${PROJECT_NAME} ACENetwork)

target_link_libraries (${EXECUTABLE} ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries (${EXECUTABLE} ${ACE_LIBRARY})
if (UNIX)
 target_link_libraries (${EXECUTABLE} rt)
 target_link_libraries (${EXECUTABLE} ${PKG_CAP_LIBRARIES})
 target_link_libraries (${EXECUTABLE} ${PKG_FFMPEG_LIBRARIES})
 target_link_libraries (${EXECUTABLE} X11)
 if (GTKGL_SUPPORT)
  target_link_libraries (${EXECUTABLE} ${PKG_OPENGL_LIBRARIES})
 endif ()
 if (GTK3_SUPPORT)
  target_link_libraries (${EXECUTABLE} ${PKG_GTK3_LIBRARIES})
 else ()
  target_link_libraries (${EXECUTABLE} ${PKG_GTK2_LIBRARIES})
 endif ()
elseif (WIN32)
 target_link_libraries (${EXECUTABLE} ntdll.lib)
 target_link_libraries (${EXECUTABLE} evr.lib)
# *TODO*: repair WIN32 pkgconfig support
 target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/gtk/Win32/debug/lib/gio-2.0.lib)
 target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/gtk/Win32/debug/lib/glib-2.0.lib)
 target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/gtk/Win32/debug/lib/gobject-2.0.lib)
 target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/gtk/Win32/debug/lib/gthread-2.0.lib)
 target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/gtk/Win32/debug/lib/gdk-3.0.lib)
 target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/gtk/Win32/debug/lib/gtk-3.0.lib)
 target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/gtk/Win32/debug/lib/pango-1.0.lib)
# target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtkglext/gtk/.libs/libgtkglext-win32-1.0.dll.a)
# target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtkglext/gdk/.libs/libgdkglext-win32-1.0.dll.a)
endif ()
target_link_libraries (${EXECUTABLE} ${GLUT_LIBRARY})
target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../../ffmpeg/libavcodec/avcodec.lib)
target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../../ffmpeg/libavformat/avformat.lib)
target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../../ffmpeg/libswscale/swscale.lib)
target_link_libraries (${EXECUTABLE} Common)
target_link_libraries (${EXECUTABLE} CommonUI)
if (WIN32)
 # *WARNING*: linking CommonUI requires "/FORCE:MULTIPLE""
 set_target_properties (${EXECUTABLE} PROPERTIES LINK_FLAGS "/FORCE:MULTIPLE /SAFESEH:NO")
endif ()
target_link_libraries (${EXECUTABLE} ACEStream)
target_link_libraries (${EXECUTABLE} ACEStream_Device)
#target_link_libraries (${EXECUTABLE} ACEStream_Miscellaneous)
target_link_libraries (${EXECUTABLE} ACENetwork)

# add PCH
set_target_properties (${EXECUTABLE} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
cotire (${EXECUTABLE})

install (TARGETS ${EXECUTABLE}
         EXPORT ${PROJECT_NAME}Targets
         RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
         COMPONENT dev)
install (FILES
         etc/ardrone.glade
         scripts/mavlink_scanner.l
         scripts/navdata_scanner.l
         DESTINATION etc/lib${PROJECT_NAME}/ardrone COMPONENT config)

##########################################

if (UNIX)
 create_target_launcher (${EXECUTABLE}
                         ARGS "-a 127.0.0.1 -l -p 5555 -t"
#                         FORWARD_ARGS
                         RUNTIME_LIBRARY_DIRS ${PROJECT_SOURCE_DIR}/modules/ATCD/ACE/lib
                                              ${PROJECT_SOURCE_DIR}/../gtk/gtk/Win32/debug/bin
                                              ${PROJECT_SOURCE_DIR}/../libCommon/cmake/src/Debug
                                              ${PROJECT_SOURCE_DIR}/../libCommon/cmake/src/ui/Debug
                                              ${PROJECT_SOURCE_DIR}/cmake/src/Debug
                                              ${PROJECT_SOURCE_DIR}/cmake/src/modules/dev/Debug
                                              /usr/local/lib
                         WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/../src/test_i
                         ENVIRONMENT foo=bar)
elseif (WIN32)
 create_target_launcher (${EXECUTABLE}
                         ARGS "-a 127.0.0.1 -c -l -p 5555 -t"
#                        FORWARD_ARGS
                         RUNTIME_LIBRARY_DIRS ${PROJECT_SOURCE_DIR}/../ATCD/ACE/lib
#                                              ${PROJECT_SOURCE_DIR}/../openssl/bin
                                              ${PROJECT_SOURCE_DIR}/../FreeGLUT/freeglut/freeglut/cmake/bin/Debug
                                              ${PROJECT_SOURCE_DIR}/../ffmpeg/libavcodec
                                              ${PROJECT_SOURCE_DIR}/../ffmpeg/libavformat
                                              ${PROJECT_SOURCE_DIR}/../ffmpeg/libavutil
                                              ${PROJECT_SOURCE_DIR}/../ffmpeg/libswscale
                                              ${PROJECT_SOURCE_DIR}/../ffmpeg/libswresample
                                              ${PROJECT_SOURCE_DIR}/../gtk/gtk/Win32/debug/bin
                                              G:/software/Development/dll
                                              ${PROJECT_SOURCE_DIR}/../libCommon/cmake/src/Debug
                                              ${PROJECT_SOURCE_DIR}/../libCommon/cmake/src/err/Debug
#                                              ${PROJECT_SOURCE_DIR}/../libCommon/cmake/src/image/Debug
#                                              ${PROJECT_SOURCE_DIR}/../libCommon/cmake/src/math/Debug
                                              ${PROJECT_SOURCE_DIR}/../libCommon/cmake/src/ui/Debug
                                              ${PROJECT_SOURCE_DIR}/../libACEStream/cmake/src/Debug
                                              ${PROJECT_SOURCE_DIR}/../libACEStream/cmake/src/modules/dec/Debug
                                              ${PROJECT_SOURCE_DIR}/../libACEStream/cmake/src/modules/dev/Debug
                                              ${PROJECT_SOURCE_DIR}/cmake/src/Debug
                         WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/../src/test_i
                         ENVIRONMENT foo=bar)
endif ()
