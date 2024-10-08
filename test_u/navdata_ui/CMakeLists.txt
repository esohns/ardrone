include ($ENV{PRJ_ROOT}/Common/scripts/ffmpeg.cmake)

#add_definitions (-DGUI_SUPPORT)
#include (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/ui/scripts/PreUIFramework.cmake)
#include (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/ui/scripts/SetUIFramework.cmake)

if (CURSES_USE)
elseif (GTK_USE)
# include (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/ui/scripts/SetGTKVersion.cmake)

# if (CMAKE_BUILD_TYPE STREQUAL Debug)
#  add_definitions (-DG_ERRORCHECK_MUTEXES)
# endif (CMAKE_BUILD_TYPE STREQUAL Debug)
elseif (QT_USE)
 # Find includes in corresponding build directories
 set(CMAKE_INCLUDE_CURRENT_DIR ON)
 # Instruct CMake to run moc automatically when needed
 set(CMAKE_AUTOMOC ON)
 # Create code from a list of Qt designer ui files
 set(CMAKE_AUTOUIC ON)

 if (UNIX)
  add_compile_options (-fPIC)
 endif (UNIX)

 include_directories (${Qt5Widgets_INCLUDE_DIRS})

 include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../modules/Common/src/ui)
 include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../modules/Common/src/ui/qt)
elseif (WXWIDGETS_USE)
endif ()

include ($ENV{PRJ_ROOT}/Common/scripts/ACE.cmake)

##########################################

if (UNIX)
# include (CheckLibraryExists)
# check_library_exists (pthread pthread_create "" HAVE_LIB_PTHREAD)
 set (CMAKE_THREAD_PREFER_PTHREAD ON)
 find_package (Threads)
endif ()

string (REPLACE ${CMAKE_SOURCE_DIR} "" BUILD_PATH_SUFFIX ${CMAKE_BINARY_DIR})
string (SUBSTRING ${BUILD_PATH_SUFFIX} 1 -1 BUILD_PATH_SUFFIX)
find_package (Common
              REQUIRED
              CONFIG
              PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../../modules
              PATH_SUFFIXES Common/${BUILD_PATH_SUFFIX}
              NO_DEFAULT_PATH)
if (NOT EXISTS ${Common_DIR})
 message (FATAL_ERROR "could not find package \"Common\", aborting")
endif ()
add_definitions (-DCOMMON_HAS_DLL)

find_package (ACEStream
              REQUIRED
              CONFIG
              PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../../modules
              PATH_SUFFIXES ${BUILD_PATH_SUFFIX}
              NO_DEFAULT_PATH)
if (NOT EXISTS ${ACEStream_DIR})
 message (FATAL_ERROR "could not find package \"ACEStream\", aborting")
endif ()
add_definitions (-DSTREAM_HAS_DLL)
#add_definitions (-DSTREAM_DEC_HAS_DLL)

find_package (ACENetwork
              REQUIRED
              CONFIG
              PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../../modules
              PATH_SUFFIXES ${BUILD_PATH_SUFFIX}
              NO_DEFAULT_PATH)
if (NOT EXISTS ${ACENetwork_DIR})
 message (FATAL_ERROR "could not find package \"ACENetwork\", aborting")
endif ()
add_definitions (-DNET_HAS_DLL)

find_package (${PROJECT_NAME}
              REQUIRED
              CONFIG
              PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../..
              PATH_SUFFIXES ${BUILD_PATH_SUFFIX}
              NO_DEFAULT_PATH)
if (NOT EXISTS ${ACENetwork_DIR})
 message (FATAL_ERROR "could not find package \"ACENetwork\", aborting")
endif ()
add_definitions (-DNET_HAS_DLL)

add_definitions (-DHAVE_CONFIG_H)
#set_property (SOURCE PROPERTY COMPILE_DEFINITIONS HAVE_CONFIG_H)

if (UNIX)
 include_directories (${PKG_FFMPEG_INCLUDE_DIRS})

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ARDrone_SDK_2_0_1/ARDroneLib)
 # *NOTE*: headers in Soft/Common include each other 'relatively' :-(
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ARDrone_SDK_2_0_1/ARDroneLib/Soft/Common)
 # *NOTE*: headers in Soft/Common include headers in Soft/Lib :-(
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ARDrone_SDK_2_0_1/ARDroneLib/Soft/Lib)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ARDrone_SDK_2_0_1/ARDroneLib/VP_SDK)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../mavlink/generated/include)
 
 include_directories (/usr/local/src/ACE_wrappers)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/include)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/gl)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/image)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/log)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/signal)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/timer)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/ui)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/ui/qt)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/test_u)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/include)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src/../dec)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src/../lib)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src/modules/net)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src/modules/stat)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src/modules/vis)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACENetwork/cmake)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACENetwork/src)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACENetwork/src/client_server)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACENetwork/src/wlan)
elseif (WIN32)
 if (GTK_USE)
  include_directories (${GTK_INCLUDE_DIRS})

  if (OPENGL_SUPPORT AND GTKGLAREA_SUPPORT)
# *NOTE*: check out the 'master' branch for gtk3-based applications
# *NOTE*: check out the 'gtkglarea-2' branch for gtk2-based applications
# *TODO*: verify automatically
   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../gtkglarea)
  endif (OPENGL_SUPPORT AND GTKGLAREA_SUPPORT)

  if (LIBGLADE_SUPPORT)
   include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../libglade-2.6.4/include/libglade-2.0)
  endif (LIBGLADE_SUPPORT)
 elseif (WXWIDGETS_USE)
  include_directories (${wxWidgets_INCLUDE_DIRS})
 endif ()

 if (DEFINED ENV{ACE_ROOT})
  include_directories ($ENV{ACE_ROOT})
 else ()
  include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ATCD/ACE)
 endif (DEFINED ENV{ACE_ROOT})

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../3rd_party/msvc)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/include)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/image)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/log)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/signal)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/timer)
 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/src/ui)

 include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../Common/test_u)

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src/modules/dec)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src/modules/dev)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src/modules/file)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src/modules/lib)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src/modules/misc)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src/modules/stat)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../../ACEStream/src/modules/vis)

endif ()
include_directories (${CMAKE_CURRENT_BINARY_DIR}/../..)
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../src)

############## next target ################
set (EXECUTABLE navdata_ui)
set (${EXECUTABLE}_EXE_SRCS
     stdafx.cpp
     stdafx.h
     test_u.cpp
     test_u_common.h
     test_u_common_modules.h
     test_u_defines.h
     test_u_eventhandler.cpp
     test_u_eventhandler.h
     test_u_eventhandler.inl
     test_u_message.cpp
     test_u_message.h
     test_u_message.inl
     test_u_net_modules.h
     test_u_network.h
     test_u_network_common.h
     test_u_session_message.cpp
     test_u_session_message.h
     test_u_session_message.inl
     test_u_stream.cpp
     test_u_stream.h
     test_u_stream_common.h)
if (GTK_USE)
 set (${EXECUTABLE}_EXE_SRCS
      ${${EXECUTABLE}_EXE_SRCS}
      test_u_gtk_callbacks.cpp
      test_u_gtk_callbacks.h
)
elseif (QT_USE)
 set (${EXECUTABLE}_EXE_SRCS
      ${${EXECUTABLE}_EXE_SRCS}
      mainwindow.cpp
      mainwindow.h
      mainwindow.ui
      mainwindow_2.cpp
      mainwindow_2.h
      mainwindow_2.inl
)
endif ()

if (UNIX)
 add_definitions (-D_GLIBCXX_INCLUDE_NEXT_C_HEADERS)
 if (GUI_SUPPORT)
  if (GTK_USE)
   if (CMAKE_BUILD_TYPE STREQUAL Debug)
    add_definitions (-DG_ERRORCHECK_MUTEXES)
   endif ()
  endif (GTK_USE)
 endif (GUI_SUPPORT)
elseif (WIN32)
 if (WIN32 AND (CMAKE_BUILD_TYPE STREQUAL Debug))
  add_definitions (-DD3D_DEBUG_INFO)
  # *NOTE*: DirectShow uses 'DEBUG'
  add_definitions (-DDEBUG) # activate DirectShow debug logging
 endif (WIN32 AND (CMAKE_BUILD_TYPE STREQUAL Debug))
endif ()

set (LLVM_COMPILER_ID_STRING "Clang")
string (TOLOWER "${LLVM_COMPILER_ID_STRING}" LLVM_COMPILER_ID_STRING_LOWER)
string (TOLOWER "${CMAKE_CXX_COMPILER_ID}" CXX_COMPILER_ID_STRING_LOWER)
if (CXX_COMPILER_ID_STRING_LOWER STREQUAL LLVM_COMPILER_ID_STRING_LOWER)
 add_compile_options (-std=c++11)
#add_compile_options (-fPIC)
 add_definitions (-D_GLIBCXX_USE_CXX11_ABI=0)
endif ()

cmake_policy (SET CMP0065 OLD) # do not add flags to export symbols from executables without the ENABLE_EXPORTS target property

add_executable (${EXECUTABLE}
                EXCLUDE_FROM_ALL
                ${${EXECUTABLE}_EXE_SRCS})
set_property (TARGET ${EXECUTABLE} PROPERTY FOLDER "test_u")
cmake_policy (SET CMP0012 NEW)
if (WIN32 AND NOT BUILD_TEST_U)
 set_target_properties (${EXECUTABLE} PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD TRUE)
endif (WIN32 AND NOT BUILD_TEST_U)

if (UNIX)
 target_compile_features (${EXECUTABLE} PRIVATE cxx_std_17)
endif (UNIX)

#add_dependencies (${EXECUTABLE} ACE)
add_dependencies (${EXECUTABLE} Common)
add_dependencies (${EXECUTABLE} ACEStream)
add_dependencies (${EXECUTABLE} ACEStream_Decoder)
add_dependencies (${EXECUTABLE} ACEStream_Device)
add_dependencies (${EXECUTABLE} ACEStream_Miscellaneous)
add_dependencies (${EXECUTABLE} ACEStream_Visualization)
add_dependencies (${EXECUTABLE} ACENetwork)

target_link_libraries (${EXECUTABLE} ${CMAKE_THREAD_LIBS_INIT})
if (UNIX)
 target_link_libraries (${EXECUTABLE} GL)
 target_link_libraries (${EXECUTABLE} ${PKG_FFMPEG_LIBRARIES})

 if (QT_USE)
  target_link_libraries(${EXECUTABLE} Qt5::Widgets)
 endif (QT_USE)
elseif (WIN32)
 if (MSVC)
  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:\"libcmt.lib\"")
  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:\"libcpmt.lib\"")
 endif (MSVC)
# target_link_libraries (${EXECUTABLE} dxguid.lib)
 target_link_libraries (${EXECUTABLE} mfplat.lib)

 if (CMAKE_BUILD_TYPE STREQUAL Debug)
#  target_link_libraries (${EXECUTABLE} ${DIRECTSHOW_BASECLASSES_DIRECTORY}/cmake/baseclasses/Debug/strmbasd.lib)
 else ()
#  target_link_libraries (${EXECUTABLE} ${DIRECTSHOW_BASECLASSES_DIRECTORY}/cmake/baseclasses/Release/strmbase.lib)
 endif (CMAKE_BUILD_TYPE STREQUAL Debug)

 target_link_libraries (${EXECUTABLE} OpenGL32.lib)
 #if (GTKGLAREA_SUPPORT)
 # target_link_libraries (${EXECUTABLE} $ENV{LIB_ROOT}/libepoxy/lib/epoxy.lib)
 # target_link_libraries (${EXECUTABLE} $ENV{LIB_ROOT}/gtkglarea/cmake/${CMAKE_BUILD_TYPE}/gtkglarea.lib)
 #endif (GTKGLAREA_SUPPORT)

# *TODO*: repair WIN32 pkgconfig support
# target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../../ffmpeg/libavformat/avformat.lib)
# target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../../ffmpeg/libavformat/libavformat.a)
# target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../../ffmpeg/libavcodec/avcodec.lib)
# target_link_libraries (${EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/../../../ffmpeg/libavcodec/libavcodec.a)
endif ()
target_link_libraries (${EXECUTABLE} ${ACE_LIBRARY})

target_link_libraries (${EXECUTABLE} Common)
target_link_libraries (${EXECUTABLE} CommonEvent)
target_link_libraries (${EXECUTABLE} CommonImage)
target_link_libraries (${EXECUTABLE} CommonLog)
target_link_libraries (${EXECUTABLE} CommonSignal)
target_link_libraries (${EXECUTABLE} CommonTimer)
if (GUI_SUPPORT)
 if (WIN32)
  # *WARNING*: linking CommonUI requires "/FORCE:MULTIPLE"
  set_target_properties (${EXECUTABLE} PROPERTIES LINK_FLAGS /FORCE:MULTIPLE)
 endif (WIN32)
 target_link_libraries (${EXECUTABLE} CommonUI)
 if (GTK_USE)
  target_link_libraries (${EXECUTABLE} CommonUI_GTK)
 elseif (WXWIDGETS_USE)
  target_link_libraries (${EXECUTABLE} CommonUI_wxWidgets)
 endif ()
endif (GUI_SUPPORT)
if (UNIX)
 target_link_libraries (${EXECUTABLE} CommonXML)
endif (UNIX)
#target_link_libraries (${EXECUTABLE} Common_Test_U)

target_link_libraries (${EXECUTABLE} ACEStream)
target_link_libraries (${EXECUTABLE} ACEStream_Decoder)
target_link_libraries (${EXECUTABLE} ACEStream_File)
target_link_libraries (${EXECUTABLE} ACEStream_MediaFramework)
target_link_libraries (${EXECUTABLE} ACEStream_Device)
target_link_libraries (${EXECUTABLE} ACEStream_Miscellaneous)
target_link_libraries (${EXECUTABLE} ACEStream_Network)
target_link_libraries (${EXECUTABLE} ACEStream_Statistic)
target_link_libraries (${EXECUTABLE} ACEStream_Visualization)

target_link_libraries (${EXECUTABLE} ACENetwork)

target_link_libraries (${EXECUTABLE} ${PROJECT_NAME})

# add PCH
target_precompile_headers (${EXECUTABLE}
                           PRIVATE stdafx.h)

set (ui_definition_file "")
if (GTK_USE)
 if (GTK3_USE)
  set (ui_definition_file ${EXECUTABLE}.gtk3)
 elseif (GTK2_USE)
  set (ui_definition_file ${EXECUTABLE}.gtk2)
 endif ()
elseif (QT_USE)
elseif (WXWIDGETS_USE)
endif ()

install (TARGETS ${EXECUTABLE}
         EXPORT ${PROJECT_NAME}Targets
         RUNTIME DESTINATION ${INSTALL_BIN_DIR} COMPONENT test_u)
if (UNIX)
 install (FILES etc/${ui_definition_file}
          DESTINATION "${INSTALL_SHARE_DIR}/${PROJECT_NAME}/${EXECUTABLE}" COMPONENT config)
elseif (WIN32)
 get_filename_component (ACE_LIB_PATH ${ACE_LIBRARY}
                         DIRECTORY)
 set (ACE_DLL_FILE ACE${LIB_FILE_SUFFIX}.dll)
 set (ACE_DLL_FQPATH ${ACE_LIB_PATH}/${ACE_DLL_FILE})
 install (FILES ${ACE_DLL_FQPATH}
                ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/bin/libatk-1.0-0.dll
                ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/bin/freetype6.dll
                ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/bin/intl.dll
                ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/bin/libcairo-2.dll
                ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/bin/libfontconfig-1.dll
                ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/bin/libgio-2.0-0.dll
                ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/bin/libglib-2.0-0.dll
                ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/bin/libgmodule-2.0-0.dll
                ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/bin/libgobject-2.0-0.dll
                ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/bin/libgthread-2.0-0.dll
                ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/bin/libgdk_pixbuf-2.0-0.dll
                ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/bin/libgdk-win32-2.0-0.dll
                ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/bin/libgtk-win32-2.0-0.dll
                ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/bin/libpango-1.0-0.dll
                ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/bin/libpangocairo-1.0-0.dll
                ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/bin/libpangoft2-1.0-0.dll
                ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/bin/libpangowin32-1.0-0.dll
                ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/bin/libpng14-14.dll
                ${CMAKE_CURRENT_SOURCE_DIR}/../../../gtk/bin/zlib1.dll
                ${CMAKE_CURRENT_SOURCE_DIR}/../../../libglade/bin/libglade-2.0-0.dll
                ${CMAKE_CURRENT_SOURCE_DIR}/../../../libxml2-2.9.1/.libs/libxml2-2.dll
                G:/software/Development/dll/libgcc_s_dw2-1.dll
                G:/software/Development/dll/libexpat-1.dll
                G:/software/Development/dll/libiconv-2.dll
                ${Common_DIR}/src/Debug/Common.dll
                ${Common_DIR}/src/ui/Debug/CommonUI.dll
                ${ACENetwork_DIR}/src/Debug/ACENetwork.dll
                ${ACENetwork_DIR}/src/protocol/http/Debug/ACENetwork_Protocol_HTTP.dll
                ${CMAKE_CURRENT_SOURCE_DIR}/../../../libmysql_2/cmake/libmysql/Debug/libmysql.dll
          DESTINATION ${INSTALL_BIN_DIR} COMPONENT test_u)
 install (FILES etc/${EXECUTABLE}.gtk3 ${EXECUTABLE}.gtk2 ${EXECUTABLE}.fbp
          DESTINATION "data/${PROJECT_NAME}/${EXECUTABLE}" COMPONENT config)
#          DESTINATION "$ENV{APPDATA}/${PROJECT_NAME}/${EXECUTABLE}" COMPONENT config)
endif ()

##########################################

set (ARGS "-l")
if (CMAKE_BUILD_TYPE STREQUAL Debug)
 set (ARGS "${ARGS} -t")
endif (CMAKE_BUILD_TYPE STREQUAL Debug)
if (UNIX)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${EXECUTABLE}" COMMAND)
 if (GUI_SUPPORT)
  if (GTK_USE)
   set (ARGS "-g./etc/${ui_definition_file} ${ARGS}")
  elseif (WXWIDGETS_USE)
   set (ENVIRONMENT "DISPLAY=:0")
  endif ()
 else ()
  set (ARGS "-g ${ARGS}")
 endif (GUI_SUPPORT)

 set (RUNTIME_LIBRARY_DIRS ${PROJECT_SOURCE_DIR}/modules/ATCD/ACE/lib)
# set (ENVIRONMENT "GDK_BACKEND=x11")
elseif (WIN32)
 file (TO_NATIVE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE}/${EXECUTABLE}.exe" COMMAND)
 string (REPLACE "\\" "\\\\" COMMAND ${COMMAND})
 if (GUI_SUPPORT)
  set (ARGS "-g.\\\\etc\\\\${ui_definition_file} ${ARGS}")

  if (GTK_USE)
   if (GTK3_USE)
    file (TO_CMAKE_PATH $ENV{LIB_ROOT}/gtk3/bin GTK_LIB_PATH)
    set (RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBRARY_DIRS}
                              ${GTK_LIB_PATH})
   elseif (GTK2_USE)
    file (TO_CMAKE_PATH $ENV{LIB_ROOT}/gtk2/bin GTK_LIB_PATH)
    set (RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBRARY_DIRS}
                              ${GTK_LIB_PATH})
   endif ()
  elseif (WXWIDGETS_USE)
   set (RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBRARY_DIRS}
                             ${wxWidgets_LIB_DIR})
  endif ()
 else ()
  set (ARGS "-g ${ARGS}")
 endif (GUI_SUPPORT)
 # *NOTE*: ffmpeg was built with (vcpkg-)"libpng" support, so now there is a new dependency to zlib1.dll, liblzma.dll, iconv-2.dll :-(
 file (TO_CMAKE_PATH "$ENV{VCPKG_INSTALLATION_ROOT}/installed/x64-windows/bin" VCPKG_PATH)
 file (TO_CMAKE_PATH "${ffmpeg_LIB_DIR}" FFMPEG_PATH)
 file (TO_CMAKE_PATH $ENV{LIB_ROOT}/openssl SSL_PATH)
 file (TO_CMAKE_PATH "${ACE_LIB_DIR}" ACE_PATH)
 set (RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBRARY_DIRS}
                           ${VCPKG_PATH}
                           ${FFMPEG_PATH}
                           ${SSL_PATH}
                           ${ACE_PATH})
endif ()
create_target_launcher (${EXECUTABLE}
                        COMMAND ${COMMAND}
                        ARGS ${ARGS}
#                        FORWARD_ARGS
                        RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBRARY_DIRS}
                        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test_u/navdata_ui
                        ENVIRONMENT ${ENVIRONMENT})

##########################################

add_test (NAME ${EXECUTABLE}_test
          COMMAND ${PROJECT_BINARY_DIR}/${EXECUTABLE} -l -t
          WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})

set_tests_properties (${EXECUTABLE}_test
                      PROPERTIES
                      ENVIRONMENT "PATH=D:\\projects\\ATCD\\ACE\\lib;D:\\projects\\gtk\\bin;D:\\projects\\libglade\\bin;D:\\projects\\libxml2-2.9.1\\.libs;G:\\software\\Development\\dll;${PROJECT_SOURCE_DIR}\\..\\Common\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\..\\Common\\cmake\\src\\ui\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\Debug;${PROJECT_SOURCE_DIR}\\cmake\\src\\modules\\dev\\Debug;${PROJECT_SOURCE_DIR}\\..\\libACENetwork\\cmake\\src\\Debug;%PATH%")
#PATH=%PATH%;$(ProjectDir)..\..\..\..\ATCD\ACE\lib;$(ProjectDir)..\..\..\..\gtk3\bin;$(ProjectDir)..\..\..\..\libav-11.7\libavcodec;$(ProjectDir)..\..\..\..\libav-11.7\libavresample;$(ProjectDir)..\..\..\..\libav-11.7\libavutil;$(ProjectDir)..\..\..\..\Common\cmake\src\Debug;$(ProjectDir)..\..\..\..\Common\cmake\src\err\Debug;$(ProjectDir)..\..\..\..\Common\cmake\src\ui\Debug;$(ProjectDir)..\..\src\Debug;$(ProjectDir)..\..\src\modules\dev\Debug;$(ProjectDir)..\..\src\modules\vis\Debug
