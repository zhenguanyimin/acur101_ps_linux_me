SUMMARY = "Recipe for  build an external ism303dac-accel Linux kernel module"
SECTION = "PETALINUX/modules"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=12f884d2ae1ff87c09e5b7ccc2c4ca7e"

inherit module

INHIBIT_PACKAGE_STRIP = "1"

SRC_URI = "file://Makefile \
           file://st_ism303dac_accel_buffer.c \
           file://st_ism303dac_accel_i2c.c \
           file://st_ism303dac_accel_spi.c \
           file://st_ism303dac_accel_core.c \
           file://st_ism303dac_accel_trigger.c \
           file://st_ism303dac_accel.h \
           file://ism303dac.h \
           file://types.h \
	   file://COPYING \
          "

S = "${WORKDIR}"

# The inherit of module.bbclass will automatically name module packages with
# "kernel-module-" prefix as required by the oe-core build environment.
