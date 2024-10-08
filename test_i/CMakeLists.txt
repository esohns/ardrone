﻿include (CMakeDependentOption)

option (BUILD_TEST_I "build test_i" OFF)

set (ACE_LIB_FILE libACE.so)
if (UNIX)
 find_library (ACE_LIBRARY ${ACE_LIB_FILE}
               PATHS $ENV{PRJ_ROOT}/ACE_TAO/ACE /usr/local
    #           HINTS ENV ACE_ROOT
               PATH_SUFFIXES lib
               DOC "searching for ${ACE_LIB_FILE}"
               NO_DEFAULT_PATH)
# else ()
#  find_library (ACE_LIBRARY ${ACE_LIB_FILE}
#                DOC "searching for ${ACE_LIB_FILE} (system paths)")
# endif ()
elseif (WIN32)
 if (CMAKE_BUILD_TYPE STREQUAL Debug)
  set (LIB_FILE_SUFFIX d)
 endif ()
 unset (ACE_LIB_FILE)
 set (ACE_LIB_FILE ACE${LIB_FILE_SUFFIX}.lib)
 find_library (ACE_LIBRARY ${ACE_LIB_FILE}
               PATHS ENV ACE_ROOT
               PATHS ${CMAKE_CURRENT_BINARY_DIR}/../../../ATCD/ACE
               PATH_SUFFIXES lib lib\\${CMAKE_BUILD_TYPE}\\Win32
               DOC "searching for ${ACE_LIB_FILE}")
endif ()
#if (NOT EXISTS ACE_LIBRARY)
if (NOT ACE_LIBRARY)
 message (FATAL_ERROR "Could not find ${ACE_LIB_FILE} (was: \"${ACE_LIBRARY}\"), aborting")
else ()
 #message (STATUS "Found ACE library \"${ACE_LIBRARY}\"")
endif ()
#add_definitions (-DACE_HAS_DLL)
#pkg_check_modules (PKG_ACE REQUIRED ACE)

# user interface
option (GUI_SUPPORT "enable graphical user interface support" ON)
if (GUI_SUPPORT)
 add_definitions (-DGUI_SUPPORT)
 include ($ENV{PRJ_ROOT}/Common/src/ui/scripts/PreUIFramework.cmake)
 include ($ENV{PRJ_ROOT}/Common/src/ui/scripts/SetUIFramework.cmake)

 if (GTK_USE)
  include ($ENV{PRJ_ROOT}/Common/src/ui/scripts/SetGTKVersion.cmake)
  CMAKE_DEPENDENT_OPTION (GTKGL_SUPPORT "enable GTK OpenGL support" ON
                          "GUI_SUPPORT" OFF)
# *IMPORTANT NOTE*: to use gtkglarea on gtk2, check out the 'gtkglarea-2' branch
#                   of the project
  CMAKE_DEPENDENT_OPTION (GTKGLAREA_SUPPORT "enable GtkGLArea support" OFF
                          "GUI_SUPPORT; GTKGL_SUPPORT" OFF)
 elseif (WXWIDGETS_USE)
  message (STATUS "wxWidgets_DEFINITIONS: ${wxWidgets_DEFINITIONS}")
  message (STATUS "wxWidgets_DEFINITIONS_DEBUG: ${wxWidgets_DEFINITIONS_DEBUG}")
  message (STATUS "wxWidgets_CXX_FLAGS: ${wxWidgets_CXX_FLAGS}")
  message (STATUS "wxWidgets_USE_FILE: ${wxWidgets_USE_FILE}")
  # *NOTE*: re-enable ANSI mode
#  add_definitions (-DwxUSE_UNICODE=0)
  if (WIN32)
#   add_definitions(-DUNICODE -D_UNICODE)
   add_definitions (-D__WXMSW__)
   add_definitions (-DWXUSINGDLL)
   if (MSVC)
    add_definitions (-DwxMSVC_VERSION_AUTO)
   endif (MSVC)
  endif (WIN32)
  add_definitions (-DwxUSE_GUI=1)
  if (CMAKE_BUILD_TYPE STREQUAL Debug)
   foreach (DEFINITION ${wxWidgets_DEFINITIONS_DEBUG})
    add_definitions (-D${DEFINITION})
   endforeach ()
   add_definitions (-D__WXDEBUG__)
  else ()
   foreach (DEFINITION ${wxWidgets_DEFINITIONS})
    add_definitions (-D${DEFINITION})
   endforeach ()
  endif (CMAKE_BUILD_TYPE STREQUAL Debug)
 endif ()
endif (GUI_SUPPORT)

##########################################

add_subdirectory (ardrone_ui)

# phonies
add_custom_target (test_i ALL
                   COMMENT "building test_i...")
add_dependencies (test_i ardrone_ui)

if (NOT BUILD_TEST_I)
 set_target_properties (test_i PROPERTIES EXCLUDE_FROM_ALL 1)
 cmake_policy (SET CMP0012 NEW)
 if (WIN32)
  set_target_properties (test_i PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD TRUE)
 endif (WIN32)
endif ()

# ----------------------------------------

#include (CreateLaunchers)

# enable (unit/integration) tests
#include (CTest)
enable_testing ()
