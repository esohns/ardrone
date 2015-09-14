ACLOCAL_AMFLAGS = \
  -I m4

#  -I/usr/include/c++/3.4.6/backward
#  -I$(top_srcdir)/modules/ATCD/ACE
#  *NOTE*: -frepo is broken in gcc 4.9.2
#  *TODO*: repair test_i_template_instantiations.cpp so it works with
#          -fno-implicit-templates
AM_CPPFLAGS = \
  -I/usr/share/gettext \
  -I$(top_srcdir)/../ATCD/ACE \
  $(PKGSDL_CFLAGS) \
  $(PKGGTK_CFLAGS)
if VALGRIND_SUPPORT
AM_CPPFLAGS += \
  $(PKGVALGRIND_CFLAGS)
endif

DEFS = \
  -DHAVE_CONFIG_H \
  -DBASEDIR=\"$(datadir)/@PACKAGE_NAME@\" \
  -DCOMMON_HAS_DLL \
  -DCOMMON_UI_HAS_DLL \
  -DSTREAM_HAS_DLL \
  -DNET_HAS_DLL \
  -DNET_CLIENT_HAS_DLL
if DEBUG
DEFS += \
  -DDEBUG_DEBUGGER
endif

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
  stdafx.cpp \
  stdafx.h

olimex_mod_mpu6050_CPPFLAGS = \
  $(AM_CPPFLAGS) \
  -I$(top_srcdir)/modules/libCommon/src \
  -I$(top_srcdir)/modules/libCommon/src/ui \
  -I$(top_srcdir)/modules/libACEStream/src \
  -I$(top_srcdir)/modules/libACEStream/src/modules/misc \
  -I$(top_srcdir)/modules/libACEStream/src/modules/net \
  -I$(top_srcdir)/modules/libACENetwork/src \
  -I$(top_srcdir)/modules/libACENetwork/src/client_server \
  -I$(top_srcdir)/src/include
olimex_mod_mpu6050_LDFLAGS = \
  -L$(top_srcdir)/modules/ATCD/ACE/build/linux/lib \
  $(PKGGTK_LDFLAGS) \
  -L$(abs_builddir)/../../modules/libCommon/src/.libs \
  -L$(abs_builddir)/../../modules/libCommon/src/ui/.libs \
  -L$(abs_builddir)/../../modules/libACEStream/src/.libs \
  -L$(abs_builddir)/../../modules/libACEStream/src/modules/misc/.libs \
  -L$(abs_builddir)/../../modules/libACEStream/src/modules/net/.libs \
  -L$(abs_builddir)/../../modules/libACENetwork/src/.libs \
  -L$(abs_builddir)/../../modules/libACENetwork/src/client_server/.libs
olimex_mod_mpu6050_LDADD = \
  -lACE \
  $(PKGGTK_LIBS) \
  -lCommon \
  -lCommon_UI \
  -lACEStream \
  -lACEStream_Misc \
  -lACEStream_Net \
  -lACENetwork \
  -lACENetwork_Client

SUBDIRS = \
  etc
