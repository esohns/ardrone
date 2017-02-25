cmake_minimum_required (VERSION 2.8)

# options
#set (CMAKE_VERBOSE_MAKEFILE ON)

set (VERSION_MAJOR 1)
set (VERSION_MINOR 0)
set (VERSION_MICRO 0)
set (VERSION_DEVEL devel)
set (VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO})
if (VERSION_DEVEL)
 unset (VERSION)
 set (VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO}-${VERSION_DEVEL})
endif ()

set_property (GLOBAL PROPERTY USE_FOLDERS ON)

# *NOTE*: if the user specifies -DCMAKE_BUILD_TYPE on the command line, take
#         their definition and dump it in the cache along with proper
#         documentation, otherwise set CMAKE_BUILD_TYPE to Debug prior to
#         calling PROJECT()
set (BUILD_TYPE_CACHE_STRING "choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
if (DEFINED CMAKE_BUILD_TYPE)
 set (CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING ${BUILD_TYPE_CACHE_STRING})
else ()
 set (CMAKE_BUILD_TYPE Debug CACHE STRING ${BUILD_TYPE_CACHE_STRING})
endif ()

cmake_policy (SET CMP0048 NEW) # allow project VERSION
project (ardrone
         VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO}
         LANGUAGES CXX C)

# override the installation directories ?
set (INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set (INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set (INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for header files")
if (WIN32 AND NOT CYGWIN)
 set (DEF_INSTALL_CMAKE_DIR CMake)
else ()
 set (DEF_INSTALL_CMAKE_DIR lib/CMake/FooBar)
endif ()
set (INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH "Installation directory for CMake files")

# make relative paths absolute (needed later on)
foreach (path LIB BIN INCLUDE CMAKE)
 set (var INSTALL_${path}_DIR)
 if (NOT IS_ABSOLUTE "${${var}}")
  set (${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
 endif ()
endforeach ()

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export (PACKAGE lib${PROJECT_NAME})

# Create the ardroneConfig.cmake and ardroneConfigVersion.cmake files
file (RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}" "${INSTALL_INCLUDE_DIR}")
# ... for the build tree
set (CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")
configure_file (${PROJECT_NAME}Config.cmake.in "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake" @ONLY)
# ... for the install tree
set (CONF_INCLUDE_DIRS "\${LIBACESTREAM_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file (${PROJECT_NAME}Config.cmake.in "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${PROJECT_NAME}Config.cmake" @ONLY)
# ... for both
configure_file (${PROJECT_NAME}ConfigVersion.cmake.in "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake" @ONLY)

# Install the ardroneConfig.cmake and ardroneConfigVersion.cmake
install (FILES
         "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/${PROJECT_NAME}Config.cmake"
         "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
         DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)

# Install the export set for use with the install-tree
install (EXPORT ${PROJECT_NAME}Targets
         DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)

set (PACKAGE ${PROJECT_NAME})
configure_file (${CMAKE_CURRENT_SOURCE_DIR}/config.h.in.cmake
                ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_config.h
                @ONLY)
#add_custom_target (${PROJECT_NAME}_config)
#set_target_properties (${PROJECT_NAME}_config PROPERTIES
#                       PUBLIC_HEADER "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_config.h")

install (FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_config.h"
         DESTINATION "${INSTALL_INCLUDE_DIR}/${PROJECT_NAME}"
         COMPONENT dev)

if (CMAKE_BUILD_TYPE STREQUAL Debug)
 if (DEBUG_DEBUGGER)
  add_definitions (-DDEBUG_DEBUGGER)
 endif ()
 if (CMAKE_COMPILER_IS_GNUCXX)
  add_compile_options (-g)
  add_compile_options (-O0)
  add_compile_options (-fno-inline)
 endif ()
 add_definitions (-D_DEBUG)
#  set_property (SOURCE PROPERTY COMPILE_DEFINITIONS _DEBUG)
else ()
 if (CMAKE_COMPILER_IS_GNUCXX)
  add_compile_options (-O3)
 endif ()
 add_definitions (-DACE_NDEBUG)
#  set_property (SOURCE PROPERTY COMPILE_DEFINITIONS ACE_NDEBUG)
endif ()

option (TRACING
        "enable tracing" OFF)
if (TRACING)
 add_definitions (-DARDRONE_NTRACE=0)
#  set_property (SOURCE PROPERTY COMPILE_DEFINITIONS ARDRONE_NTRACE=0)
else ()
 add_definitions (-DARDRONE_NTRACE=1)
#  set_property (SOURCE PROPERTY COMPILE_DEFINITIONS ARDRONE_NTRACE=1)
endif ()

option (VALGRIND_SUPPORT
        "enable valgrind support" OFF)
if (VALGRIND_SUPPORT)
 include (CheckIncludeFiles)
 check_include_files (valgrind/valgrind.h HAVE_VALGRIND_H)
#  include (CheckLibraryExists)
#  check_library_exists (libvalgrind "" "" HAVE_LIB_VALGRIND)
 include (FindPkgConfig)
 pkg_check_modules (PKGVALGRIND REQUIRED libvalgrind)
 add_definitions (-DLIBACESTREAM_ENABLE_VALGRIND_SUPPORT)
#  set_property (SOURCE PROPERTY COMPILE_DEFINITIONS LIBACESTREAM_ENABLE_VALGRIND_SUPPORT)
endif ()

if (CMAKE_COMPILER_IS_GNUCXX)
 add_compile_options (-ansi)
 add_compile_options (-std=c++11)
 add_compile_options (-Wall)
 set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS} -g")
 add_definitions (-D_GNU_SOURCE)
#  set_property (SOURCE PROPERTY COMPILE_DEFINITIONS _GNU_SOURCE)
endif ()

option (BUILD_MODULES
        "build (git) sub-modules" OFF)

if (CMAKE_COMPILER_IS_GNUCXX)
 add_compile_options (-ansi)
 add_compile_options (-std=c++11)
 add_compile_options (-Wall)
 set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS} -g")
 add_definitions (-D_GNU_SOURCE)
#  set_property (SOURCE PROPERTY COMPILE_DEFINITIONS _GNU_SOURCE)
endif ()
if (MSVC)
 set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

 set (MSVC_INCREMENTAL_YES_FLAG "/INCREMENTAL:NO")
 foreach (FLAG_TYPE EXE MODULE SHARED STATIC)
  string (REGEX REPLACE "/INCREMENTAL* " "/INCREMENTAL:NO " CMAKE_${FLAG_TYPE}_LINKER_FLAGS_DEBUG "${CMAKE_${FLAG_TYPE}_LINKER_FLAGS_DEBUG}")
#  message (STATUS "flag: ${FLAG_TYPE}: ${CMAKE_${FLAG_TYPE}_LINKER_FLAGS_DEBUG}")
  string (REPLACE "/EDITANDCONTINUE"     ""                 CMAKE_${FLAG_TYPE}_LINKER_FLAGS_DEBUG "${CMAKE_${FLAG_TYPE}_LINKER_FLAGS_DEBUG}")
  set (CMAKE_${FLAG_TYPE}_LINKER_FLAGS_DEBUG "${CMAKE_${FLAG_TYPE}_LINKER_FLAGS_DEBUG}"
       CACHE STRING "Overriding default debug ${FLAG_TYPE} linker flags." FORCE)
  mark_as_advanced (CMAKE_${FLAG_TYPE}_LINKER_FLAGS_DEBUG)
 endforeach ()
endif (MSVC)

list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cotire/CMake")
#list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/scripts")
include (cotire)
set_directory_properties (PROPERTIES COTIRE_ADD_UNITY_BUILD FALSE)

################################################################################

if (UNIX)
 include (GNUInstallDirs)
elseif (WIN32)
 include (InstallRequiredSystemLibraries)
endif ()

################################################################################

# build a CPack driven installer package
set (CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set (CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set (CPACK_PACKAGE_VERSION_PATCH ${VERSION_MICRO})

set (CPACK_RESOURCE_FILE_LICENSE ${CMAKE_CURRENT_SOURCE_DIR}/LICENSE)
set (CPACK_RESOURCE_FILE_README ${CMAKE_CURRENT_SOURCE_DIR}/README)

set (CPACK_PACKAGE_DESCRIPTION "this is a prototype for a ARDrone quadcopter VR FPV client")
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "this is a prototype for a ARDrone quadcopter VR FPV client")
#set (CPACK_PACKAGE_VENDOR "Vendor")
set (CPACK_PACKAGE_CONTACT "eriksohns@fastmail.net")

# *NOTE*: this does not work on Win32 using the NSIS installer
set (CPACK_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION OFF)

if (UNIX)
 set (CPACK_GENERATOR "DEB")
# set (CPACK_DEBIAN_PACKAGE_NAME )
# set (CPACK_DEBIAN_PACKAGE_VERSION)
# set (CPACK_DEBIAN_PACKAGE_ARCHITECTURE)
 set (CPACK_DEBIAN_PACKAGE_DEPENDS "ffmpeg")
# set (CPACK_DEBIAN_PACKAGE_MAINTAINER)
# set (CPACK_DEBIAN_PACKAGE_DESCRIPTION)
# set (CPACK_DEBIAN_PACKAGE_SECTION)
# set (CPACK_DEBIAN_COMPRESSION_TYPE)
# set (CPACK_DEBIAN_PACKAGE_PRIORITY)
 set (CPACK_DEBIAN_PACKAGE_HOMEPAGE "http://www.github.com/esohns/ardrone")
# set (CPACK_DEBIAN_PACKAGE_SHLIBDEPS)
# set (CPACK_DEBIAN_PACKAGE_DEBUG)
# set (CPACK_DEBIAN_PACKAGE_PREDEPENDS)
# set (CPACK_DEBIAN_PACKAGE_ENHANCES)
# set (CPACK_DEBIAN_PACKAGE_BREAKS)
# set (CPACK_DEBIAN_PACKAGE_CONFLICTS)
# set (CPACK_DEBIAN_PACKAGE_PROVIDES)
# set (CPACK_DEBIAN_PACKAGE_REPLACES)
# set (CPACK_DEBIAN_PACKAGE_RECOMMENDS)
# set (CPACK_DEBIAN_PACKAGE_SUGGESTS)
# set (CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA)
elseif (WIN32)
 #set (CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64")
 #set (CPACK_PACKAGE_INSTALL_DIRECTORY)
 set (CPACK_NSIS_MUI_ICON ${CMAKE_CURRENT_SOURCE_DIR}/src/test_i/etc/ardrone_small.ico)
 #set (CPACK_NSIS_MUI_UNIICON)
 #set (CPACK_NSIS_INSTALLER_MUI_ICON_CODE)
 #set (CPACK_NSIS_EXTRA_PREINSTALL_COMMANDS)
 #set (CPACK_NSIS_EXTRA_INSTALL_COMMANDS)
 #set (CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS)
 #set (CPACK_NSIS_COMPRESSOR /SOLID zip)
 set (CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
 #set (CPACK_NSIS_MODIFY_PATH)
 set (CPACK_NSIS_DISPLAY_NAME ${PROJECT_NAME})
 set (CPACK_NSIS_PACKAGE_NAME ${PROJECT_NAME})
 #set (CPACK_NSIS_INSTALLED_ICON_NAME )
 set (CPACK_NSIS_HELP_LINK http://www.github.org/esohns/ardrone)
 set (CPACK_NSIS_URL_INFO_ABOUT http://www.github.org/esohns/ardrone)
 set (CPACK_NSIS_CONTACT eriksohns@fastmail.net)
 #set (CPACK_NSIS_CREATE_ICONS_EXTRA)
 #set (CPACK_NSIS_DELETE_ICONS_EXTRA)
 #set (CPACK_NSIS_EXECUTABLES_DIRECTORY)
 #set (CPACK_NSIS_MUI_FINISHPAGE_RUN)
 #set (CPACK_NSIS_MENU_LINKS)
endif ()

set (CPACK_COMPONENTS_ALL bin config)

include (CPack)

################################################################################

list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake-modules")
include (UseBackportedModules)
cmake_policy (SET CMP0026 OLD)
if (WIN32)
 cmake_policy (SET CMP0053 OLD)
endif ()
include (CreateLaunchers)

# enable (unit/integration) tests
#include (CTest)
enable_testing ()

################################################################################

add_subdirectory (src)

################################################################################

if (WIN32)
 add_custom_target (installer
                    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/src/test_i/ardrone.exe)

 #set (NSIS_PROJECT_FILE "${PROJECT_NAME}.nsi")
 add_custom_command (TARGET installer
                     POST_BUILD
                     COMMAND make_installer.bat ${CMAKE_BUILD_TYPE} ${NSIS_PROJECT_FILE}
                     WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/scripts"
                     COMMENT "making installer..."
                     VERBATIM)
endif ()
