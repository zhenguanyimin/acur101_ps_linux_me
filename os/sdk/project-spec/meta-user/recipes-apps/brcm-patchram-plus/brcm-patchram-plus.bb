#
# This file is the brcm-patchram-plus recipe.
#

SUMMARY = "Simple brcm-patchram-plus application"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://brcm-patchram-plus.c \
	   file://Makefile \
	   file://BCM4345C0.hcd \
		  "

S = "${WORKDIR}"

do_compile() {
	     oe_runmake
}

do_install() {
	     install -d ${D}${bindir}
	     install -m 0755 brcm-patchram-plus ${D}${bindir}/brcm-patchram-plus

	     install -d ${D}${base_libdir}/firmware
		 install -m 0666 BCM4345C0.hcd ${D}${base_libdir}/firmware/
}

FILES:${PN} += "/lib /lib/firmware/*"
