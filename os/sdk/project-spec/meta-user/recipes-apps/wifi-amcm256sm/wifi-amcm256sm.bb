#
# This file is the wifi-amcm256sm recipe.
#

SUMMARY = "Simple wifi-amcm256sm application"
SECTION = "PETALINUX/apps"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://regulatory.db \
		   file://regulatory.db.p7s \
		   file://cyfmac43455-sdio.xlnx,zynqmp.bin \
		   file://cyfmac43455-sdio.clm_blob \
		   file://cyfmac43455-sdio.txt \
		   file://compat.ko \
		   file://cfg80211.ko \
		   file://brcmutil.ko \
		   file://brcmfmac.ko \
	"

S = "${WORKDIR}"

do_install() {
		install -d ${D}${base_libdir}/firmware/
		install -m 0666 regulatory.db ${D}${base_libdir}/firmware/
		install -m 0666 regulatory.db.p7s ${D}${base_libdir}/firmware/

		install -d ${D}${base_libdir}/firmware/cypress
		install -m 0666 cyfmac43455-sdio.xlnx,zynqmp.bin ${D}${base_libdir}/firmware/cypress/
		install -m 0666 cyfmac43455-sdio.clm_blob ${D}${base_libdir}/firmware/cypress/
		install -m 0666 cyfmac43455-sdio.txt ${D}${base_libdir}/firmware/cypress/

		install -d ${D}${base_libdir}/modules/aw-cm256sm
		install -m 0755 compat.ko ${D}${base_libdir}/modules/aw-cm256sm/
		install -m 0755 cfg80211.ko ${D}${base_libdir}/modules/aw-cm256sm/
		install -m 0755 brcmutil.ko ${D}${base_libdir}/modules/aw-cm256sm/
		install -m 0755 brcmfmac.ko ${D}${base_libdir}/modules/aw-cm256sm/
}

FILES:${PN} += "/lib /lib/firmware/*"
