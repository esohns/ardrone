ACLOCAL_AMFLAGS = \
  -I m4

DEFS = \
  -DCOMMON_HAS_DLL \
  -DCOMMON_UI_HAS_DLL \
  -DSTREAM_HAS_DLL \
  -DNET_HAS_DLL \
  -DNET_CLIENT_HAS_DLL \
  -DBASEDIR=\"$(datadir)/@PACKAGE_NAME@\"

AM_CPPFLAGS = \
  -I/usr/share/gettext \
  -I$(top_srcdir)/../ATCD/ACE \
  -I$(top_srcdir)/modules/libCommon/src \
  -I$(top_srcdir)/modules/libCommon/src/ui \
  -I$(top_srcdir)/modules/libACEStream/src \
  -I$(top_srcdir)/modules/libACENetwork/src \
  -I$(top_srcdir)/modules/libACENetwork/src/client \
  -I$(top_srcdir)/src/include

bin_PROGRAMS = \
  olimex_mod_mpu6050

noinst_HEADERS = \
  stdafx.h

olimex_mod_mpu6050_SOURCES = \
  olimex_mod_mpu6050.cpp \
  olimex_mod_mpu6050_callbacks.cpp \
  olimex_mod_mpu6050_callbacks.h \
  olimex_mod_mpu6050_common.cpp \
  olimex_mod_mpu6050_common.h \
  olimex_mod_mpu6050_defines.h \
  olimex_mod_mpu6050_eventhandler.cpp \
  olimex_mod_mpu6050_eventhandler.h \
  olimex_mod_mpu6050_macros.h \
  olimex_mod_mpu6050_message.cpp \
  olimex_mod_mpu6050_message.h \
  olimex_mod_mpu6050_module_eventhandler.cpp \
  olimex_mod_mpu6050_module_eventhandler.h \
  olimex_mod_mpu6050_network.h \
  olimex_mod_mpu6050_opengl.cpp \
  olimex_mod_mpu6050_opengl.h \
  olimex_mod_mpu6050_sessionmessage.cpp \
  olimex_mod_mpu6050_sessionmessage.h \
  olimex_mod_mpu6050_signalhandler.cpp \
  olimex_mod_mpu6050_signalhandler.h \
  olimex_mod_mpu6050_stream_common.h \
  olimex_mod_mpu6050_stream.cpp \
  olimex_mod_mpu6050_stream.h \
  olimex_mod_mpu6050_types.h \
  olimex_mod_mpu6050_uidefinition.cpp \
  olimex_mod_mpu6050_uidefinition.h \
  stdafx.cpp \
  stdafx.h

olimex_mod_mpu6050_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  $(PKGLIB_CFLAGS)

olimex_mod_mpu6050_LDFLAGS = \
  $(PKGLIB_LDFLAGS) \
  -L$(top_srcdir)/modules/ATCD/ACE/build/linux/lib \
  -L$(top_builddir)/modules/libCommon/src/.libs \
  -L$(top_builddir)/modules/libCommon/src/ui/.libs \
  -L$(top_builddir)/modules/libACEStream/src/.libs \
  -L$(top_builddir)/modules/libACENetwork/src/.libs \
  -L$(top_builddir)/modules/libACENetwork/src/client/.libs

olimex_mod_mpu6050_LDADD = \
  $(PKGLIB_LIBS) \
  -lACE \
  -lCommon \
  -lCommon_UI \
  -lACEStream \
  -lACENetwork \
  -lACENetwork_Client

configdir = $(datadir)/@PACKAGE_NAME@
config_DATA = \
  olimex_mod_mpu6050.glade

EXTRA_DIST = $(config_DATA)
