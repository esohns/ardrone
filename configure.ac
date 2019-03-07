#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# define version information
m4_define([M4_ARDRONE_VERSION_MAJOR], [0])
m4_define([M4_ARDRONE_VERSION_MINOR], [0])
m4_define([M4_ARDRONE_VERSION_MICRO], [1])
m4_define([M4_ARDRONE_VERSION_DEVEL], [devel])
m4_define([M4_ARDRONE_VERSION],
          [M4_ARDRONE_VERSION_MAJOR.M4_ARDRONE_VERSION_MINOR.M4_ARDRONE_VERSION_MICRO[]m4_ifset([M4_ARDRONE_VERSION_DEVEL],-[M4_ARDRONE_VERSION_DEVEL])])
m4_define([M4_ARDRONE_VERSION_API]  , [1])
dnl # Libtool versioning.
dnl m4_define([dc_version_lt_current],[0])
dnl m4_define([dc_version_lt_revision],[0])
dnl m4_define([dc_version_lt_age],[0])
AC_SUBST([ARDRONE_VERSION],[M4_ARDRONE_VERSION])
AC_SUBST([ARDRONE_VERSION_MAJOR],[M4_ARDRONE_VERSION_MAJOR])
AC_SUBST([ARDRONE_VERSION_MINOR],[M4_ARDRONE_VERSION_MINOR])
AC_SUBST([ARDRONE_VERSION_MICRO],[M4_ARDRONE_VERSION_MICRO])
dnl AC_SUBST([DC_VERSION_LIBTOOL],[dc_version_lt_current:dc_version_lt_revision:dc_version_lt_age])
# Version suffix.
m4_ifset([M4_ARDRONE_VERSION_DEVEL],
         [AC_DEFINE(HAVE_VERSION_DEVEL, [1], [Define if a version suffix is present.])])
AC_SUBST([ARDRONE_VERSION_API],[M4_ARDRONE_VERSION_API])

AC_PREREQ([2.69])
AC_INIT([ardrone],
        [M4_ARDRONE_VERSION],
        [eriksohns@fastmail.net],
        [ardrone],
        [http://www.github.com/esohns/ardrone])
AC_CONFIG_SRCDIR([src/test_i/ardrone.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([aux_config])
AC_CONFIG_MACRO_DIR([m4])
dnl AC_CONFIG_SUBDIRS([modules/ATCD
dnl                    modules/libCommon
dnl                    modules/libACENetwork
dnl                    modules/libACEStream])

#AC_USE_SYSTEM_EXTENSIONS

# checks for programs
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LEX
AC_PROG_LN_S
AC_PROG_RANLIB
AC_PROG_YACC
AM_PROG_CC_C_O

# init other tools
AM_INIT_AUTOMAKE
# build time sanity check...
AM_SANITY_CHECK

# prefix config.h
m4_define([ARDRONE_CONFIG_H], [ardrone_config.h])
AX_PREFIX_CONFIG_H([ARDRONE_CONFIG_H], [ardrone], [config.h])

# set up kernel module support
AC_ARG_VAR([KERNEL_SRC_DIR], [kernel src directory (overrides --with-kerneldir)])
AM_CONDITIONAL([HAVE_KERNEL_SRC_DIR_ENV], [test x${KERNEL_SRC_DIR} != x])
AC_PATH_KERNEL_SOURCE
AM_CONDITIONAL([HAVE_KERNEL_SRC], [test x${no_kernel} != xyes])
#AM_CONDITIONAL([HAVE_KERNEL_SRC], [test x${kerneldir} != x -a x${kerneldir} != $(shell pwd)])
moduledir_default="/lib/modules/`uname -r`/extra"
AC_ARG_WITH(moduledir,
            [AS_HELP_STRING([--with-moduledir=DIR], [kernel modules in DIR [default="${moduledir_default}"]])],
            [moduledir=${withval}],
            [moduledir=${moduledir_default}])
AC_SUBST([moduledir])
# *WARNING*: length must be <= 20 !
AC_SUBST([modulename],[ardrone_mod])
# --> enable cross-compilation support ?
AC_MSG_CHECKING(cross-compile (ARM) the kernel module)
cross_compile_default="yes"
AC_ARG_ENABLE([cross-compile],
              [AS_HELP_STRING([--enable-cross-compile], [turn on cross-compilation (ARM) [default="${cross_compile_default}"]])],
              [case "${enableval}" in
                 yes)
                   cross_compile=yes
                   ;;
                 no)
                   cross_compile=no;
                   ;;
                 *) AC_MSG_ERROR([bad value ${enableval} for --enable-cross-compile]) ;;
               esac],
              [cross_compile=$cross_compile_default])
BUILD_PLATFORM=`uname -m | head -c3 -`
AM_CONDITIONAL([DO_CROSS_COMPILE], [test x$cross_compile = xyes -a ${BUILD_PLATFORM} != arm])
AC_SUBST([BUILD_PLATFORM])
AM_COND_IF([DO_CROSS_COMPILE],
           [AC_MSG_RESULT(yes)],
           [AC_MSG_RESULT(no)])

## (re)set the cpp compiler flags
#CXXFLAGS="-fno-implicit-templates"
CXXFLAGS_DEBUG=
# --> enable debug support ?
AC_MSG_CHECKING([enable debugging])
debug_default="no"
AC_ARG_ENABLE([debug],
              [AS_HELP_STRING([--enable-debug], [turn on debugging [default="${debug_default}"]])],
              [case "${enableval}" in
                 yes) debug=yes ;;
                 no)  debug=no  ;;
                 *) AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
               esac],
              [debug=$debug_default])
AM_CONDITIONAL([DEBUG], [test x$debug = xyes])
if test x$debug = xyes; then
  CXXFLAGS_DEBUG="-g -O0 -fno-inline -Wall -D_DEBUG"
  AC_MSG_RESULT([yes])
else
#  CXXFLAGS_DEBUG="-O3 -Werror -DACE_NDEBUG"
  CXXFLAGS_DEBUG="-O3 -Wall -DACE_NDEBUG"
  AC_MSG_RESULT([no])
fi

# --> enable tracing support ?
AC_MSG_CHECKING([enable tracing])
tracing_default="no"
AC_ARG_ENABLE([tracing],
              [AS_HELP_STRING([--enable-tracing], [turn on tracing [default="${tracing_default}"]])],
              [case "${enableval}" in
                 yes) tracing=yes ;;
                 no)  tracing=no  ;;
                 *) AC_MSG_ERROR([bad value ${enableval} for --enable-tracing]) ;;
               esac],
              [tracing=$tracing_default])
AM_CONDITIONAL([TRACING], [test x$tracing = xyes])
if test x$tracing = xyes; then
  CXXFLAGS_TRACING="-DARDRONE_NTRACE=0"
  AC_MSG_RESULT([yes])
else
  CXXFLAGS_TRACING="-DARDRONE_NTRACE=1"
  AC_MSG_RESULT([no])
fi

# --> enable valgrind support ?
AC_MSG_CHECKING([enable valgrind support])
valgrind_support_default="no"
AC_ARG_ENABLE([valgrind-support],
              [AS_HELP_STRING([--enable-valgrind_support], [turn on valgrind support [default="$valgrind_support_default"]])],
              [case "${enableval}" in
                 yes) valgrind_support=yes ;;
                 no)  valgrind_support=no  ;;
                 *) AC_MSG_ERROR([bad value ${enableval} for --enable-valgrind-support]) ;;
               esac],
              [valgrind_support=$valgrind_support_default])
AM_CONDITIONAL([VALGRIND_SUPPORT], [test x$valgrind_support = xyes])
if test x$valgrind_support = xyes; then
  CXXFLAGS_VALGRIND="-DARDRONE_ENABLE_VALGRIND_SUPPORT"
  AC_MSG_RESULT([yes])
else
  AC_MSG_RESULT([no])
fi

# export variables (so Makefile substitutions can be made)
#CXXFLAGS_BASE="-ansi -D_POSIX_C_SOURCE=199506L -D_REENTRANT -D_GNU_SOURCE"
CXXFLAGS_BASE="-ansi -std=c++11 -Wall -D_GNU_SOURCE"
CXXFLAGS="${CXXFLAGS_BASE} ${CXXFLAGS_DEBUG} ${CXXFLAGS_TRACING} ${CXXFLAGS_VALGRIND}"
AC_SUBST(CXXFLAGS)

# build modules ?
AC_MSG_CHECKING([enable module support])
module_support_default="yes"
AC_ARG_ENABLE([module-support],
              [AS_HELP_STRING([--enable-module_support], [compile submodules [default="$module_support_default"]])],
              [case "${enableval}" in
                 yes) module_support=yes ;;
                 no)  module_support=no  ;;
                 *) AC_MSG_ERROR([bad value ${enableval} for --enable-module-support]) ;;
               esac],
              [module_support=$module_support_default])
AM_CONDITIONAL([BUILD_MODULES], [test x$module_support = xyes])
if test x$module_support = xyes; then
  AC_MSG_RESULT([yes])
else
  AC_MSG_RESULT([no])
fi
AM_COND_IF([BUILD_MODULES],
           [AC_CONFIG_SUBDIRS([modules/libCommon
                               modules/libACEStream
                               modules/libACENetwork])])
dnl AC_CONFIG_SUBDIRS([modules/ATCD
dnl                    modules/libCommon
dnl                    modules/libACEStream
dnl                    modules/libACENetwork])])
AM_COND_IF([BUILD_MODULES],
           [AC_CONFIG_FILES([modules/Makefile])])

# checks for typedefs, structures, and compiler characteristics
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_INT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

# checks for header files
AM_COND_IF([VALGRIND_SUPPORT],
           [AC_CHECK_HEADER([valgrind/valgrind.h])])
AC_LANG_PUSH([C++])
AC_CHECK_HEADER([glm/glm.hpp])
AC_LANG_POP([C++])
AC_CHECK_HEADERS([libintl.h])
AC_CHECK_HEADERS([locale.h])
AC_CHECK_HEADER_STDBOOL

# checks for library functions
AC_CHECK_FUNCS([getcwd])
AC_CHECK_FUNCS([memset])
AC_CHECK_FUNCS([setlocale])
AC_CHECK_FUNCS([strerror])
AC_FUNC_MALLOC
AC_FUNC_ERROR_AT_LINE

# checks for libraries
AM_COND_IF([VALGRIND_SUPPORT],
           [PKG_CHECK_MODULES([PKGVALGRIND], [valgrind])])
PKG_CHECK_MODULES([PKGGTK], [libpng gmodule-export-2.0 gthread-2.0 gtk+-2.0 gtkglext-1.0 libglade-2.0])
PKG_CHECK_MODULES([PKGGL], [gl glu])

# checks for other libraries (non-pkg-config)
# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [main])
# FIXME: Replace `main' with a function in `-lACE':
AC_CHECK_LIB([ACE], [main])
# FIXME: Replace `main' with a function in `-lglm':
AC_CHECK_LIB([glm], [main])
# FIXME: Replace `main' with a function in `-lglut':
AC_CHECK_LIB([glut], [main])

# checks for tools
dnl MAKE=`which gnumake 2>/dev/null`
dnl if test ! -x "$MAKE" ; then MAKE=`which gmake 2>/dev/null` ; fi
dnl if test ! -x "$MAKE" ; then MAKE=`which make 2>/dev/null` ; fi
dnl AM_CONDITIONAL([HAVE_GNU_MAKE], [`$MAKE --version|grep -c "Free Software Foundation"` = "0"])
AM_GNU_GETTEXT_VERSION([0.18])
AM_GNU_GETTEXT([external], [need-formatstring-macros], [$(top_builddir)/intl/])
#AM_GNU_GETTEXT_INTL_SUBDIR

AC_CONFIG_FILES([Makefile
                 m4/Makefile
                 po/Makefile
                 scripts/Makefile
                 src/Makefile
                 src/include/Makefile
                 src/ko/Makefile
                 src/ko/3rd_party/Makefile
                 src/test_i/Makefile
                 src/test_i/etc/Makefile])
AC_CONFIG_FILES([scripts/ardrone.pc
                 scripts/ardrone.spec])
#AC_CONFIG_FILES([ardrone-${ARDRONE_VERSION_API}.pc:scripts/ardrone.pc.in],
#                [],
#                [ARDRONE_VERSION_API=$ARDRONE_VERSION_API])

AC_OUTPUT

