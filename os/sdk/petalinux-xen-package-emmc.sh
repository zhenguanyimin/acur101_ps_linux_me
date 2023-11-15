#!/bin/bash
#
#petalinux with xen package
#

file_size=0

function get_file_size()
{
	local file_name=$1
	local size=`stat -L --printf="%s" $file_name`
	
	file_size=`printf "0x%X" $(($size))`
}

function BOOT_BIN_packaget()
{
	local path=$1
	
	echo "general BOOT.BIN............."
	petalinux-package --boot --format BIN --fsbl ${path}/zynqmp_fsbl.elf --u-boot ${path}/u-boot.elf --pmufw ${path}/pmufw.elf --force
}

function generate_boot_scr()
{
	local out_boot_scr=$1
	local image=$2
	local boot_scr="emmc_boot_cmd"
	
	echo "general boot.scr for emmc............." 
	
	rm -rf ${boot_scr}
	rm -rf ${out_boot_scr}
	
	get_file_size ${image}
	cat >> "$boot_scr" <<- EOF
	setenv bootargs " earlycon console=ttyPS0,115200 clk_ignore_unused root=/dev/mmcblk0p3 rw rootwait mtdparts=spi0.0:28m(boot),-(user)"
	load mmc 0:1 0xE00000 Image
	load mmc 0:1 0x2600000 system.dtb
	booti 0xE00000 - 0x2600000
	EOF
	
	mkimage -c none -A arm -T script -d ${boot_scr} ${out_boot_scr}

	rm -rf ${boot_scr}
}

function fw_package()
{
	local path=$1
	local out_file=$2
	
	BOOT_BIN_packaget ${path}

	generate_boot_scr ${path}/boot.scr ${path}/Image
}


function usage()
{
	local scripte=`basename $0`
	echo "usage:"
	echo "sudo ./$scripte [-d images_path] "
	echo "$scripte -h"
	echo "	-d images file path, default is \"images/linux/\""
	echo "emmc files"
	echo "BOOT.BIN"
	echo "Image"
	echo "system.dtb"
	echo "boot.scr"
	echo "rootfs.cpio.gz"
}

images_path=./images/linux

while getopts "d:h" opt; do
	case ${opt} in
	d)
		images_path = ${OPTARG}
		;;
	h)
		usage
		exit 0 
		;;
	*)
		echo "unknown option, see \"$0 -h\""
		exit 1
		;;
	esac
done

if [ ! -d $images_path ]
then
	echo "$imaeges_path is not exist!!"
	exit 1
fi

for i in zynqmp_fsbl.elf u-boot.elf pmufw.elf Image system.dtb rootfs.cpio.gz
do
	target_file=${images_path}/${i}
	if [ ! -f $target_file ] 
	then
		echo "can't find $target_file "
		exit 1
	fi
done


fw_package ${images_path}

if [ -a ${fw_name} ] 
then 
	echo "-------------done----------------" 
else
	echo "-----------fail----------------" 
fi
