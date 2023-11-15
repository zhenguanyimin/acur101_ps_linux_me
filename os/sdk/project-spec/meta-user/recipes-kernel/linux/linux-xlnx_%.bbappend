FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

SRC_URI:append = " file://bsp.cfg"
KERNEL_FEATURES:append = " bsp.cfg"
SRC_URI += "file://user_2023-03-14-09-06-00.cfg \
            file://user_2023-05-19-02-40-00.cfg \
            file://user_2023-06-20-09-23-00.cfg \
            file://user_2023-06-26-02-23-00.cfg \
            file://user_2023-06-27-08-35-00.cfg \
            file://user_2023-06-27-12-07-00.cfg \
            file://user_2023-07-19-08-28-00.cfg \
            file://0001_add_xiic_thddat_time.patch \
            "

