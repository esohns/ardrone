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
  olimex_mod_mpu6050_eventhandler.cpp \
  olimex_mod_mpu6050_eventhandler.h \
  olimex_mod_mpu6050_inotify.h \
  olimex_mod_mpu6050_macros.h \
  olimex_mod_mpu6050_message.cpp \
  olimex_mod_mpu6050_message.h \
  olimex_mod_mpu6050_message_base.cpp \
  olimex_mod_mpu6050_message_base.h \
  olimex_mod_mpu6050_module_eventhandler.cpp \
  olimex_mod_mpu6050_module_eventhandler.h \
  olimex_mod_mpu6050_types.h \
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
