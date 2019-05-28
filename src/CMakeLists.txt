include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts/Threads.cmake)

include (${CMAKE_CURRENT_SOURCE_DIR}/../../Common/scripts/ACE.cmake)

set (ACE_XML_LIB_FILE libACEXML.so)
if (UNIX)
# message ("CMAKE_FIND_ROOT_PATH is ${CMAKE_FIND_ROOT_PATH}")
 find_library (ACE_XML_LIBRARY ${ACE_XML_LIB_FILE}
               HINTS ${CMAKE_CURRENT_BINARY_DIR}/../../../ATCD/ACE /usr/local/src/ACE_wrappers/ACEXML
               PATHS ENV ACE_ROOT
               PATH_SUFFIXES lib common
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
               HINTS ${CMAKE_CURRENT_BINARY_DIR}/../../../ATCD/ACE /usr/local/src/ACE_wrappers/ACEXML/parser
               PATHS ENV ACE_ROOT
               PATH_SUFFIXES lib parser
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
      $ENV{LIB_ROOT}/DShowBaseClasses)
#      "$ENV{PROGRAMFILES}/Microsoft SDKs/Windows/v7.0/Samples/multimedia/directshow/baseclasses")
#      "$ENV{${_PF86}}/Microsoft SDKs/Windows/v7.0/Samples/multimedia/directshow/baseclasses")
# message (STATUS "DirectShow baseclasses directory: \"${DIRECTSHOW_BASECLASSES_DIRECTORY}\"")

#pkg_check_modules (PKG_ACE REQUIRED ACE)
endif (WIN32)

get_filename_component (BUILD_PATH ${CMAKE_CURRENT_BINARY_DIR}/.. REALPATH DIRECTORY)
get_filename_component (BUILD_PATH_SUFFIX ${BUILD_PATH} NAME)
#find_package (Common 0.0.1 EXACT
find_package (Common
              REQUIRED
              CONFIG
              PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../../modules $ENV{PRJ_ROOT}
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
              PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../../modules $ENV{PRJ_ROOT}
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
              PATHS ${CMAKE_CURRENT_SOURCE_DIR}/../../modules $ENV{PRJ_ROOT}
              PATH_SUFFIXES ACENetwork/${BUILD_PATH_SUFFIX}
              NO_DEFAULT_PATH)
if (NOT EXISTS ${ACENetwork_DIR})
 message (FATAL_ERROR "could not find package \"ACENetwork\", aborting")
endif ()
add_definitions (-DNETWORK_HAS_DLL)
add_definitions (-DNETWORK_WLAN_HAS_DLL)

add_definitions (-DHAVE_CONFIG_H)
#set_property (SOURCE PROPERTY COMPILE_DEFINITIONS HAVE_CONFIG_H)

add_library (${PROJECT_NAME} STATIC "")

target_include_directories (${PROJECT_NAME} PUBLIC
                            $<BUILD_INTERFACE:$ENV{LIB_ROOT}/mavlink/generated/include>)
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../mavlink/cmake/include/common)

target_include_directories (${PROJECT_NAME} PUBLIC
                            $<BUILD_INTERFACE:$ENV{LIB_ROOT}/ARDrone_SDK_2_0_1/ARDroneLib>)
# *NOTE*: headers in Soft/Common include each other 'relatively' :-(
target_include_directories (${PROJECT_NAME} PUBLIC
                            $<BUILD_INTERFACE:$ENV{LIB_ROOT}/ARDrone_SDK_2_0_1/ARDroneLib/Soft/Common>)
# *NOTE*: headers in Soft/Common include headers in Soft/Lib :-(
target_include_directories (${PROJECT_NAME} PUBLIC
                            $<BUILD_INTERFACE:$ENV{LIB_ROOT}/ARDrone_SDK_2_0_1/ARDroneLib/Soft/Lib>)
target_include_directories (${PROJECT_NAME} PUBLIC
                            $<BUILD_INTERFACE:$ENV{LIB_ROOT}/ARDrone_SDK_2_0_1/ARDroneLib/VP_SDK>)

if (UNIX)
# include_directories (${PKG_CAP_INCLUDE_DIRS})
# include_directories (${PKG_GETTEXT_INCLUDE_DIRS})

# include_directories (/usr/share/gettext)
 #include_directories (${PKG_FFMPEG_INCLUDE_DIRS})
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../../ffmpeg)

 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/ARDrone_SDK_2_0_1/ARDroneLib/VP_SDK/VP_Os/linux>)

 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:/usr/local/src/ACE_wrappers>)

 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../Common/src>)
 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../Common/src/image>)
 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../Common/src/log>)
 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../Common/src/signal>)
 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../Common/src/timer>)
# include_directories (${CMAKE_CURRENT_SOURCE_DIR}/../modules/Common/test_u)

 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../ACEStream/src>)
 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../ACEStream/src/modules/dec>)
 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../ACEStream/src/modules/dev>)
 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../ACEStream/src/modules/file>)
 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../ACEStream/src/modules/lib>)
 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../ACEStream/src/modules/misc>)
 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../ACEStream/src/modules/net>)
 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../ACEStream/src/modules/stat>)
 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../ACEStream/src/modules/vis>)
 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../ACEStream/test_u>)

 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../ACENetwork/3rd_party/bison>)
 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../ACENetwork/src>)
 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../ACENetwork/src/client_server>)
 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../ACENetwork/src/wlan>)
elseif (WIN32)
 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:${DIRECTSHOW_BASECLASSES_DIRECTORY}/baseclasses>)

# target_include_directories (${PROJECT_NAME} PUBLIC
#                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../openssl/include>)

 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/ARDrone_SDK_2_0_1/ARDroneLib/VP_SDK/VP_Os/win32>)
 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/ARDrone_SDK_2_0_1/Examples/Win32/VCProjects/ARDrone>)

 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:$ENV{LIB_ROOT}/ffmpeg>)

 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:$ENV{PRJ_ROOT}/ACE_TAO/ACE>)

 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:$ENV{PRJ_ROOT}/Common/src>)
 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:$ENV{PRJ_ROOT}/Common/src/image>)
 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:$ENV{PRJ_ROOT}/Common/src/log>)
 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:$ENV{PRJ_ROOT}/Common/src/signal>)
 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:$ENV{PRJ_ROOT}/Common/src/timer>)

 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:$ENV{PRJ_ROOT}/Common/src/xml>)
# target_include_directories (${PROJECT_NAME} PUBLIC
#                             $<BUILD_INTERFACE:$ENV{PRJ_ROOT}/Common/test_u>)

 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:$ENV{PRJ_ROOT}/ACEStream/src>)
 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:$ENV{PRJ_ROOT}/ACEStream/src/modules/dec>)
 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:$ENV{PRJ_ROOT}/ACEStream/src/modules/dev>)
 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:$ENV{PRJ_ROOT}/ACEStream/src/modules/file>)
 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:$ENV{PRJ_ROOT}/ACEStream/src/modules/lib>)
 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:$ENV{PRJ_ROOT}/ACEStream/src/modules/misc>)
 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:$ENV{PRJ_ROOT}/ACEStream/src/modules/net>)
 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:$ENV{PRJ_ROOT}/ACEStream/src/modules/stat>)
 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:$ENV{PRJ_ROOT}/ACEStream/src/modules/vis>)
 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:$ENV{PRJ_ROOT}/ACEStream/test_u>)

 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:$ENV{PRJ_ROOT}/ACENetwork/3rd_party>)
 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:$ENV{PRJ_ROOT}/ACENetwork/3rd_party/bison>)
 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:$ENV{PRJ_ROOT}/ACENetwork/src>)
 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:$ENV{PRJ_ROOT}/ACENetwork/src/client_server>)
 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:$ENV{PRJ_ROOT}/ACENetwork/src/wlan>)

 target_include_directories (${PROJECT_NAME} PUBLIC
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/3rd_party/gettext>)
endif ()
target_include_directories (${PROJECT_NAME} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/..>)

############## next target ################
set (source_files
     ardrone_common.cpp
     ardrone_common.h
     ardrone_configuration.h
     ardrone_control_scanner.cpp
     ardrone_control_scanner.h
     ardrone_defines.h
     ardrone_macros.h
     ardrone_mavlink_scanner.cpp
     ardrone_mavlink_scanner.h
     ardrone_module_control_decoder.cpp
     ardrone_module_control_decoder.h
     ardrone_module_control_decoder.inl
     ardrone_module_controller.cpp
     ardrone_module_controller.h
     ardrone_module_controller.inl
     ardrone_module_mavlink_decoder.cpp
     ardrone_module_mavlink_decoder.h
     ardrone_module_mavlink_decoder.inl
     ardrone_module_navdata_decoder.cpp
     ardrone_module_navdata_decoder.h
     ardrone_module_navdata_decoder.inl
     ardrone_module_pave_decoder.cpp
     ardrone_module_pave_decoder.h
     ardrone_module_pave_decoder.inl
     ardrone_navdata_scanner.cpp
     ardrone_navdata_scanner.h
#     ardrone_opengl.cpp
#     ardrone_opengl.h
     ardrone_statemachine_navdata.cpp
     ardrone_statemachine_navdata.h
     ardrone_tools.cpp
     ardrone_tools.h
     ardrone_types.h)
file (GLOB script_files scripts/*.l)

#set (LLVM_COMPILER_ID_STRING "Clang")
#string (TOLOWER "${LLVM_COMPILER_ID_STRING}" LLVM_COMPILER_ID_STRING_LOWER)
#string (TOLOWER "${CMAKE_CXX_COMPILER_ID}" CXX_COMPILER_ID_STRING_LOWER)
#if (CXX_COMPILER_ID_STRING_LOWER STREQUAL LLVM_COMPILER_ID_STRING_LOWER)
# add_compile_options (-std=c++11)
# add_definitions (-D_GLIBCXX_USE_CXX11_ABI=0)
#endif ()

target_sources (${PROJECT_NAME}
                PRIVATE stdafx.cpp
                PRIVATE stdafx.h
                ${source_files})

#add_dependencies (${ACESTREAM_TARGET_NAME} ACE)
add_dependencies (${PROJECT_NAME} Common)
add_dependencies (${PROJECT_NAME} CommonImage)
add_dependencies (${PROJECT_NAME} CommonLog)
add_dependencies (${PROJECT_NAME} CommonSignal)
add_dependencies (${PROJECT_NAME} CommonTimer)
add_dependencies (${PROJECT_NAME} Common_Test_U)

add_dependencies (${PROJECT_NAME} ACEStream)

add_dependencies (${PROJECT_NAME} ACENetwork)

#if (UNIX)
#  message (STATUS "CMAKE_CXX_COMPILER_ID: \"${CMAKE_CXX_COMPILER_ID}\"")
# if (CMAKE_COMPILER_IS_GNUCXX)
# if (CMAKE_CXX_COMPILER_ID STREQUAL GNU)
#  set (CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed")
# endif ()
#endif (UNIX)

target_link_libraries (${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
if (UNIX)
 target_link_libraries (${PROJECT_NAME} rt)
 target_link_libraries (${PROJECT_NAME} ${PKG_CAP_LIBRARIES})

 target_link_libraries (${PROJECT_NAME} ${PKG_FFMPEG_LIBRARIES})

 target_link_libraries (${PROJECT_NAME} X11)
 target_link_libraries (${PROJECT_NAME} dl)
# target_link_libraries (${PROJECT_NAME} ${PKG_LIBPNG_LIBRARIES})
elseif (WIN32)
 set (CMAKE_EXE_LINKER_FLAGS /NODEFAULTLIB:\"libcmt.lib\")

 target_link_libraries (${PROJECT_NAME} evr.lib)

# target_link_libraries (${PROJECT_NAME} ntdll.lib)
 #link_directories (${DIRECTSHOW_BASECLASSES_DIRECTORY}/${CMAKE_BUILD_TYPE})
 target_link_libraries (${PROJECT_NAME} ${DIRECTSHOW_BASECLASSES_DIRECTORY}/cmake/baseclasses/${CMAKE_BUILD_TYPE}/BaseClassesd.lib)

# target_link_libraries (${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/../../ffmpeg/libavcodec/avcodec.lib)
 target_link_libraries (${PROJECT_NAME} $ENV{LIB_ROOT}/ffmpeg/lib/avcodec.lib)
# target_link_libraries (${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/../../ffmpeg/libavformat/avformat.lib)
 target_link_libraries (${PROJECT_NAME} $ENV{LIB_ROOT}/ffmpeg/lib/avformat.lib)
# target_link_libraries (${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/../../ffmpeg/libswresample/swresample.lib)
 target_link_libraries (${PROJECT_NAME} $ENV{LIB_ROOT}/ffmpeg/lib/swresample.lib)
# target_link_libraries (${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/../../ffmpeg/libswscale/swscale.lib)
 target_link_libraries (${PROJECT_NAME} $ENV{LIB_ROOT}/ffmpeg/lib/swscale.lib)

# if (MSXML_USE)
  target_link_libraries (${PROJECT_NAME} msxml2.lib)
# elseif (LIBXML2_USE)
  target_link_libraries (${PROJECT_NAME} ${LIBXML2_LIBRARY})
# endif ()
endif ()

target_link_libraries (${PROJECT_NAME} ${ACE_LIBRARY})

target_link_libraries (${PROJECT_NAME} ${ACE_XML_LIBRARY})
target_link_libraries (${PROJECT_NAME} ${ACE_XML_PARSER_LIBRARY})

target_link_libraries (${PROJECT_NAME} Common)
target_link_libraries (${PROJECT_NAME} CommonImage)
target_link_libraries (${PROJECT_NAME} CommonLog)
target_link_libraries (${PROJECT_NAME} CommonSignal)
target_link_libraries (${PROJECT_NAME} CommonTimer)
if (WIN32)
 # *WARNING*: linking CommonUI requires "/FORCE:MULTIPLE""
 set_target_properties (${PROJECT_NAME} PROPERTIES LINK_FLAGS "/FORCE:MULTIPLE /SAFESEH:NO")
endif (WIN32)
target_link_libraries (${PROJECT_NAME} CommonXML)
target_link_libraries (${PROJECT_NAME} Common_Test_U)

target_link_libraries (${PROJECT_NAME} ACEStream)
target_link_libraries (${PROJECT_NAME} ACEStream_Decoder)
if (WIN32)
 target_link_libraries (${PROJECT_NAME} ACEStream_Device)
endif (WIN32)
target_link_libraries (${PROJECT_NAME} ACEStream_File)
if (WIN32)
 target_link_libraries (${PROJECT_NAME} ACEStream_MediaFramework)
endif (WIN32)
target_link_libraries (${PROJECT_NAME} ACEStream_Miscellaneous)
target_link_libraries (${PROJECT_NAME} ACEStream_Network)
target_link_libraries (${PROJECT_NAME} ACEStream_Statistic)
target_link_libraries (${PROJECT_NAME} ACEStream_Visualization)

target_link_libraries (${PROJECT_NAME} ACENetwork)
target_link_libraries (${PROJECT_NAME} ACENetwork_Protocol_DHCP)
target_link_libraries (${PROJECT_NAME} ACENetwork_WLAN)

if (UNIX)
 target_link_libraries (${PROJECT_NAME} ${PKG_CAP_LIBRARIES})
endif (UNIX)

# add PCH
set_target_properties (${PROJECT_NAME} PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
cotire (${PROJECT_NAME})

#if (UNIX)
# add_custom_command (TARGET ${PROJECT_NAME}
#                     POST_BUILD
#                     COMMAND ./set_capabilities.sh
#                     WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../../scripts
#                     COMMENT "adding file capabilites to \"${PROJECT_NAME}\"..." VERBATIM)
#endif (UNIX)

#########################################

set (${PROJECT_NAME}_LIB_HDRS
  ardrone_common.h
  ardrone_configuration.h
  ardrone_control_scanner.h
  ardrone_defines.h
  ardrone_macros.h
  ardrone_mavlink_scanner.h
  ardrone_module_control_decoder.h
  ardrone_module_control_decoder.inl
  ardrone_module_controller.h
  ardrone_module_controller.inl
  ardrone_module_mavlink_decoder.h
  ardrone_module_mavlink_decoder.inl
  ardrone_module_navdata_decoder.h
  ardrone_module_navdata_decoder.inl
  ardrone_module_pave_decoder.h
  ardrone_module_pave_decoder.inl
  ardrone_navdata_scanner.h
#     ardrone_opengl.h
  ardrone_statemachine_navdata.h
  ardrone_tools.h
  ardrone_types.h)

install (FILES
         ${PROJECT_NAME}_LIB_HDRS
         DESTINATION "${INSTALL_INCLUDE_DIR}/${PROJECT_NAME}"
         COMPONENT mod_include)
install (FILES
         scripts/control_scanner.l
         scripts/mavlink_scanner.l
         scripts/navdata_scanner.l
         DESTINATION etc
         COMPONENT dev)
install (TARGETS ${PROJECT_NAME}
         EXPORT ${PROJECT_NAME}Targets
         ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT mod_lib
         LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT mod_lib
         RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT mod_lib)
