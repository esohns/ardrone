SUBDIRS = \
  3rd_party

if HAVE_KERNEL_SRC_DIR_ENV
 KDIR = @KERNEL_SRC_DIR@
else !HAVE_KERNEL_SRC_DIR_ENV
  KDIR = @kerneldir@
if !HAVE_KERNEL_SRC
#dnl    echo "kernel src dir not set, setting to default"
  KDIR = "/lib/modules/$(shell uname -r)/build"
endif !HAVE_KERNEL_SRC
endif !HAVE_KERNEL_SRC_DIR_ENV
#moduledir="@moduledir@"

AM_CPPFLAGS = \
  -I$(KDIR)arch/$(ARCH)/include \
  -I$(KDIR)arch/$(ARCH)/uapi/include \
  -I$(KDIR)arch/sh/include \
  -I$(KDIR)arch/$(ARCH)/plat-sunxi/include

EXTRA_DIST = \
  Makefile.common \
  Makefile.kernel
EXTRA_PROGRAMS = \
  automake_dummy
automake_dummy_SOURCES = \
  a.cpp
automake_dummy_CFLAGS = \
  $(AM_CPPFLAGS) \
  -I$(abs_srcdir)3rd_party

module_DATA = @modulename@.o
module_NAME = @modulename@
module_OBJS = \
  a.o
include ./Makefile.common

