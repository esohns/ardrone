SUBDIRS = \
  3rd_party

if HAVE_KERNEL_SRC_DIR_ENV
 KDIR=@KERNEL_SRC_DIR@
else !HAVE_KERNEL_SRC_DIR_ENV
  KDIR=@kerneldir@
if !HAVE_KERNEL_SRC
#    echo "kernel src dir not set, setting to default"
    KDIR="/lib/modules/$(shell uname -r)/build"
endif !HAVE_KERNEL_SRC
endif !HAVE_KERNEL_SRC_DIR_ENV
#moduledir=@moduledir@

AM_CPPFLAGS = \
  -I$(KDIR)arch/$(ARCH)/include \
  -I$(KDIR)arch/$(ARCH)/uapi/include \
  -I$(KDIR)arch/sh/include

EXTRA_DIST = \
  Makefile.common \
  Makefile.kernel
EXTRA_PROGRAMS = \
  automake_dummy
automake_dummy_SOURCES = \
  olimex_mod_mpu6050_defines.h \
  olimex_mod_mpu6050_device.c \
  olimex_mod_mpu6050_device.h \
  olimex_mod_mpu6050_irq.c \
  olimex_mod_mpu6050_irq.h \
  olimex_mod_mpu6050_main.c \
  olimex_mod_mpu6050_main.h \
  olimex_mod_mpu6050_netlink.c \
  olimex_mod_mpu6050_netlink.h \
  olimex_mod_mpu6050_pm.c \
  olimex_mod_mpu6050_pm.h \
  olimex_mod_mpu6050_sysfs.c \
  olimex_mod_mpu6050_sysfs.h \
  olimex_mod_mpu6050_timer.c \
  olimex_mod_mpu6050_timer.h \
  olimex_mod_mpu6050_types.h \
  olimex_mod_mpu6050_wq.c \
  olimex_mod_mpu6050_wq.h
automake_dummy_CFLAGS = \
  $(AM_CPPFLAGS) \
  -I$(abs_srcdir)3rd_party/i2cdevlib

module_DATA = @modulename@.o
module_NAME = @modulename@
module_OBJS = \
  olimex_mod_mpu6050_device.o \
  olimex_mod_mpu6050_irq.o \
  olimex_mod_mpu6050_main.o \
  olimex_mod_mpu6050_netlink.o \
  olimex_mod_mpu6050_pm.o \
  olimex_mod_mpu6050_sysfs.o \
  olimex_mod_mpu6050_timer.o \
  olimex_mod_mpu6050_wq.o

include ./Makefile.common
