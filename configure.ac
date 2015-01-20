#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

# define version information
m4_define([M4_OLINUXINO_VERSION_MAJOR], [0])
m4_define([M4_OLINUXINO_VERSION_MINOR], [0])
m4_define([M4_OLINUXINO_VERSION_MICRO], [1])
m4_define([M4_OLINUXINO_VERSION_DEVEL], [devel])
m4_define([M4_OLINUXINO_VERSION],
          [M4_OLINUXINO_VERSION_MAJOR.M4_OLINUXINO_VERSION_MINOR.M4_OLINUXINO_VERSION_MICRO[]m4_ifset([M4_OLINUXINO_VERSION_DEVEL],-[M4_OLINUXINO_VERSION_DEVEL])])
m4_define([M4_OLINUXINO_VERSION_API]  , [1])
dnl # Libtool versioning.
dnl m4_define([dc_version_lt_current],[0])
dnl m4_define([dc_version_lt_revision],[0])
dnl m4_define([dc_version_lt_age],[0])
AC_SUBST([OLINUXINO_VERSION],[M4_OLINUXINO_VERSION])
AC_SUBST([OLINUXINO_VERSION_MAJOR],[M4_OLINUXINO_VERSION_MAJOR])
AC_SUBST([OLINUXINO_VERSION_MINOR],[M4_OLINUXINO_VERSION_MINOR])
AC_SUBST([OLINUXINO_VERSION_MICRO],[M4_OLINUXINO_VERSION_MICRO])
dnl AC_SUBST([DC_VERSION_LIBTOOL],[dc_version_lt_current:dc_version_lt_revision:dc_version_lt_age])
# Version suffix.
m4_ifset([M4_OLINUXINO_VERSION_DEVEL],
         [AC_DEFINE(HAVE_VERSION_DEVEL, [1], [Define if a version suffix is present.])])
AC_SUBST([OLINUXINO_VERSION_API],[M4_OLINUXINO_VERSION_API])

AC_PREREQ([2.69])
AC_INIT([olinuxino],
        [M4_OLINUXINO_VERSION],
        [eriksohns@123mail.org],
        [OLinuxIno],
        [http://www.github.com/esohns/olinuxino])
AC_CONFIG_SRCDIR([src/ko/olimex_mod_mpu6050.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR([aux_config])
AC_CONFIG_MACRO_DIR([m4])

AC_USE_SYSTEM_EXTENSIONS

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AM_PROG_CC_C_O

# init other tools
AM_INIT_AUTOMAKE
# build time sanity check...
AM_SANITY_CHECK

# prefix config.h
AC_SUBST([OLINUXINO_CONFIG_H],[olinuxino_config.h])
AX_PREFIX_CONFIG_H([$OLINUXINO_CONFIG_H], [olinuxino], [config.h])

# Checks for libraries.

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T

# Checks for library functions.

# set up kernel module support
AC_ARG_VAR([KERNEL_SRC_DIR], [kernel src directory (overrides --with-kerneldir)])
AM_CONDITIONAL([HAVE_KERNEL_SRC_DIR_ENV], [test x${KERNEL_SRC_DIR} != x])
AC_PATH_KERNEL_SOURCE
AM_CONDITIONAL([HAVE_KERNEL_SRC], [test x${kerneldir} != x])
moduledir_default="/lib/modules/`uname -r`/misc"
AC_ARG_WITH(moduledir,
            [AS_HELP_STRING([--with-moduledir=DIR], [kernel modules in DIR [default="${moduledir_default}"]])],
            [moduledir=${withval}],
            [moduledir=${moduledir_default}])
AC_SUBST([moduledir])
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
AM_CONDITIONAL([DO_CROSS_COMPILE], [test x$cross_compile = xyes])
AM_COND_IF([DO_CROSS_COMPILE],
           [AC_MSG_RESULT(yes)],
           [AC_MSG_RESULT(no)])

## (re)set the cpp compiler flags
#CXXFLAGS="-fno-implicit-templates"
CXXFLAGS=
# --> enable debug support ?
AC_MSG_CHECKING(enable debugging)
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
  CXXFLAGS="${CXXFLAGS} -g -O0 -Wall -D_DEBUG -DDEBUG_DEBUGGER"
  AC_MSG_RESULT(yes)
else
  CXXFLAGS="${CXXFLAGS} -O3 -DNDEBUG -Wall -Werror"
  AC_MSG_RESULT(no)
fi

# --> enable tracing support ?
AC_MSG_CHECKING(enable tracing)
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
  CXXFLAGS="${CXXFLAGS} -DOLINUXINO_NTRACE=0"
  AC_MSG_RESULT(yes)
else
  CXXFLAGS="${CXXFLAGS} -DOLINUXINO_NTRACE=1"
  AC_MSG_RESULT(no)
fi

# --> enable valgrind support ?
AC_MSG_CHECKING(enable valgrind support)
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
  CXXFLAGS="${CXXFLAGS} -DOLINUXINO_ENABLE_VALGRIND_SUPPORT"
  AC_MSG_RESULT(yes)
else
  AC_MSG_RESULT(no)
fi

# export these variable (so Makefile substitutions can be made
AC_SUBST([CXXFLAGS])

dnl # checks for tools
dnl MAKE=`which gnumake 2>/dev/null`
dnl if test ! -x "$MAKE" ; then MAKE=`which gmake 2>/dev/null` ; fi
dnl if test ! -x "$MAKE" ; then MAKE=`which make 2>/dev/null` ; fi
dnl AM_CONDITIONAL([HAVE_GNU_MAKE], [`$MAKE --version|grep -c "Free Software Foundation"` = "0"])

# checks for libraries
#PKG_CHECK_MODULES(pkg_libs, xerces-c libpng sdl SDL_ttf libglade-2.0 libgnomeui-2.0 >= 1.110.0 gmodule-export-2.0 gthread-2.0 gtk+-2.0)
#AC_SEARCH_LIBS([Mix_Init], [SDL_mixer])

# checks for other libraries (non-pkg-config)
# FIXME: Replace `main' with a function in `-lACE':
AC_CHECK_LIB([ACE], [main])
# FIXME: Replace `main' with a function in `-lpthread':
#AC_CHECK_LIB([pthread], [main])
# FIXME: Replace `main' with a function in `-lxerces-c':
#AC_CHECK_LIB([xerces-c], [main])
AM_GNU_GETTEXT_VERSION(0.18.3)
AM_GNU_GETTEXT([external], [need-formatstring-macros], [$(top_builddir)/intl/])
#AM_GNU_GETTEXT_INTL_SUBDIR

# checks for functions
AC_CHECK_FUNCS([memset])
AC_CHECK_FUNCS([setenv])
AC_CHECK_FUNCS([setlocale])
AC_FUNC_MALLOC

# checks for headers
AC_CHECK_HEADERS([libintl.h])
AC_CHECK_HEADERS([locale.h])
AC_CHECK_HEADER_STDBOOL

# checks for types
AC_TYPE_INT8_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T

AC_CONFIG_FILES([Makefile
                 m4/Makefile
                 src/Makefile
                 src/include/Makefile
                 src/ko/Makefile
                 src/ko/3rd_party/Makefile
                 src/ko/3rd_party/i2cdevlib/Makefile
                 src/test_u/Makefile
                 po/Makefile
                 scripts/Makefile])
AC_CONFIG_FILES([olinuxino-${OLINUXINO_VERSION_API}.pc:scripts/olinuxino.pc.in],
                [],
                [OLINUXINO_VERSION_API=$OLINUXINO_VERSION_API])

AC_OUTPUT
