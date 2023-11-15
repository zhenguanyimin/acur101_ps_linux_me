#
# This file is the apps.rc recipe.
#

SUMMARY = "Simple apps.rc application"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://apps.rc \
	"

S = "${WORKDIR}"

FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

#inherit update-rc.d
#INITSCRIPT_NAME = "apps.rc"
#INITSCRIPT_PARAMS = "start 99 3 ."

do_install() {
		install -d ${D}${sysconfdir}
		install -m 0755 ${S}/apps.rc ${D}${sysconfdir}/rc.local
#	     install -d ${D}${sysconfdir}/init.d
#	     install -m 0755 ${S}/apps.rc ${D}${sysconfdir}/init.d/apps.rc
}

FILES_${PN} += "${sysconfdir}/*"

