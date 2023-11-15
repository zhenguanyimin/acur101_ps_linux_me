firmware_package是一个将多个文件打包的工具。由main.c编译得到。
用法：
./firmware_package 文件名1 文件名2 文件名n....
打包后输出为out.bin

将out.bin通过rtos_app/build/build_acur101_OTA.py再次打包后，可得到用于OTA的升级包。

示例：
./tools/firmware_package_tool/firmware_package sdk/images/linux/BOOT.BIN sdk/images/linux/boot.scr sdk/images/linux/xen sdk/images/linux/system.dtb sdk/images/linux/Image rootfs.cpio.gz
python rtos_app/build/build_acur101_OTA.py rtos_app/release/header_info.ini out.bin V1.00

最后会生成V1.00_OTA.bin
