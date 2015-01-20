SUBDIRS = \
  3rd_party

if HAVE_KERNEL_SRC_DIR_ENV
 KDIR=@KERNEL_SRC_DIR@
else !HAVE_KERNEL_SRC_DIR_ENV
  KDIR=@kerneldir@
if !HAVE_KERNEL_SRC
    echo "kernel src dir not set, setting to default"
    KDIR="/lib/modules/$(shell uname -r)/build"
endif !HAVE_KERNEL_SRC
endif !HAVE_KERNEL_SRC_DIR_ENV
#moduledir=@moduledir@

AM_CPPFLAGS = \
  -I$(KDIR)arch/$(ARCH)/include \
  -I$(KDIR)arch/$(ARCH)/uapi/include \
  -I$(KDIR)arch/sh/include
#-I$(KDIR)include \
#-I$(KDIR)arch/$(ARCH)/mach-sun4i/include \
#-I$(KDIR)arch/$(ARCH)/plat-sunxi/include

EXTRA_DIST = \
  Makefile.kernel \
  Makefile.common
EXTRA_PROGRAMS = \
  automake_dummy
automake_dummy_SOURCES = \
  olimex_mod_mpu6050_defines.h \
  olimex_mod_mpu6050_pm.c \
  olimex_mod_mpu6050_pm.h \
  olimex_mod_mpu6050_types.h \
  olimex_mod_mpu6050_sysfs.c \
  olimex_mod_mpu6050_sysfs.h \
  olimex_mod_mpu6050_wq.c \
  olimex_mod_mpu6050_wq.h \
  olimex_mod_mpu6050_main.c \
  olimex_mod_mpu6050_main.h
automake_dummy_CFLAGS = \
  $(AM_CPPFLAGS) \
  -I$(abs_srcdir)3rd_party/i2cdevlib

module_DATA = @modulename@.o
module_NAME = @modulename@
module_OBJS = olimex_mod_mpu6050_pm.o olimex_mod_mpu6050_sysfs.o olimex_mod_mpu6050_wq.o olimex_mod_mpu6050_main.o

include ./Makefile.common
