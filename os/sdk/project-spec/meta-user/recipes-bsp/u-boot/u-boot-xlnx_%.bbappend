FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append = " file://platform-top.h \
				file://bsp.cfg \
				file://0002-add-mkfs_vfat.patch \
				file://0002-add-tcp-wget.patch \
				file://0003-add-OTA.patch \
				file://0003-add-OTA-download.patch \
				file://0004-add-device_discovery-firmware-header.patch\
				file://0005-add-heartbeat-for-ota.patch \
				file://0006-modifiy-update-load-address.patch\
				file://0007_run_without_xen.patch \
				file://0008_modify_OTA_serverip.patch \
				file://0100_support_eth_phy_dp83869.patch \
				file://0200_fix_gem_tx_bug.patch \
				"

do_configure:append () {
	install ${WORKDIR}/platform-top.h ${S}/include/configs/
}

do_configure:append:microblaze () {
	if [ "${U_BOOT_AUTO_CONFIG}" = "1" ]; then
		install ${WORKDIR}/platform-auto.h ${S}/include/configs/
		install -d ${B}/source/board/xilinx/microblaze-generic/
		install ${WORKDIR}/config.mk ${B}/source/board/xilinx/microblaze-generic/
	fi
}
SRC_URI += "file://user_2023-04-08-09-24-00.cfg \
            file://user_2023-04-14-06-36-00.cfg \
            "


