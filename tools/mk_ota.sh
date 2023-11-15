#!/bin/sh
set -e

cd $(dirname $0)
cwd=$(pwd)
cd $(git rev-parse --show-toplevel)

if [[ -n "$1" ]];then
    VERSION="$1"
else
    VERSION=$(grep -E -o "EMBED_SOFTWARE_PS_VERSION_STR .*" app/src/cfg/radar_config.h | awk {'print $2'} | grep -o '[^"]*')
fi

rm -rf home/root/
mkdir -p home/root/
mkdir -p home/root/storage/file/


cp app/build/linux_app.elf home/root
cp app/conf home/root/ -rf
cp app/src/app/temp_table/config/temp-table.dat home/root/storage/file/ || true
cp os/sdk/build/tmp/sysroots-components/zynqmp_generic/simple-dcache/lib/modules/5.15.36-xilinx-v2022.2/extra/simple-dcache.ko home/root/ || true
cp fpga/acur101_fpga_*.bin home/root
cp os/sdk/project-spec/meta-user/recipes-bsp/device-tree/files/fpga_source_dts.dtb home/root
tar -czf app.tar.gz home/root

image=os/sdk/images/linux
${cwd}/firmware_package_tool/firmware_package ${image}/BOOT.BIN ${image}/boot.scr ${image}/system.dtb ${image}/Image ${image}/rootfs.cpio.gz app.tar.gz

python3 tools/package_bin/build/build_acur101_OTA.py tools/package_bin/release/header_info.ini out.bin $VERSION
rm -rf out.bin home/root app.tar.gz
echo "........done........"
