#ACLOCAL_AMFLAGS = \
#  -Im4
AM_CPPFLAGS = \
  -I$(top_srcdir)/src/include \
  -I/usr/share/gettext

bin_PROGRAMS = \
  olimex_mod_mpu6050

noinst_HEADERS = \
  stdafx.h

olimex_mod_mpu6050_SOURCES = \
  olimex_mod_mpu6050.cpp \
  stdafx.cpp \
  stdafx.h
#olimex_mod_mpu6050_CPPFLAGS = \
#  $(AM_CPPFLAGS) \
#  $(pkg_libs_CFLAGS) \
#  -DBASEDIR=\"$(datadir)/@META_PACKAGE@\"
olimex_mod_mpu6050_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  $(pkg_libs_CFLAGS)
#olimex_mod_mpu6050_LDFLAGS = $(pkg_libs_LIBS)
#olimex_mod_mpu6050_LDADD = \
#  $(builddir)/../../sound/libRPG_Sound.la \
#  $(builddir)/../../common/libRPG_Common.la \
#  $(builddir)/../../chance/dice/libRPG_Dice.la \
#  $(pkg_libs_LIBS) \
#  -lSDL_mixer
olimex_mod_mpu6050_LDADD = \
  $(pkg_libs_LIBS)
