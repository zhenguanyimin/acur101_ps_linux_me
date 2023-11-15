#!/bin/bash
#
#petalinux with xen package
#

file_size=0
boot_size=2M
kernel_size=10M
xen_size=1M
system_dtb_size=1M
boot_scr_offset=64000K

kernel_offset=2M
xen_offset=12M
system_dtb_offset=13M
rootfs_offset=14M
boot_scr_offset=64000K

rootfs_jffs2=mini-rootfs.jffs2

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
	local image_gz=$3
	local xen=$4
	local system_dtb=$5
	local rootfs=$6
	local boot_scr="qflash_boot_cmd"
	
	echo "general boot.scr for qspi flash............." 
	
	rm -rf ${boot_scr}
	rm -rf ${out_boot_scr}
	
	cat >> "$boot_scr" <<- EOF
		echo probe qspi flash with clock frequency 50Mhz on bus spi0
		sf probe 0 5000000 0
	EOF
	
	get_file_size ${image_gz}
	cat >> "$boot_scr" <<- EOF
		echo load kernel image.gz 0x200000->0x3000000
		sf read 0x3000000 0x200000 $file_size
	EOF
	
	get_file_size ${image}
	cat >> "$boot_scr" <<- EOF
		echo unzip kernel image.gz to 0xE00000
		unzip 0x3000000 0xE00000 $file_size
	EOF
	
	get_file_size ${xen}
	cat >> "$boot_scr" <<- EOF
		echo load xen 0xC00000->0x2400000
		sf read 0x2400000 0xC00000 $file_size
	EOF
	
	get_file_size ${system_dtb}
	cat >> "$boot_scr" <<- EOF
		echo load system.dtb 0xD00000->0x2600000
		sf read 0x2600000 0xD00000 $file_size
	EOF
	
	get_file_size ${image}
	cat >> "$boot_scr" <<- EOF
	fdt addr 0x2600000
	fdt resize 1024
	fdt set /chosen \#address-cells <0x2>
	fdt set /chosen \#size-cells <0x2>
	fdt set /chosen xen,xen-bootargs "console=dtuart dtuart=serial0 dom0_mem=1024M dom0_max_vcpus=2 bootscrub=0 vwfi=native sched=null  "
	fdt mknod /chosen dom0
	fdt set /chosen/dom0 compatible  "xen,linux-zimage" "xen,multiboot-module" "multiboot,module"
	fdt set /chosen/dom0 reg <0x0 0xE00000 0x0 $file_size >
	fdt set /chosen xen,dom0-bootargs "console=hvc0 earlycon=xen earlyprintk=xen clk_ignore_unused rootfstype=jffs2 root=/dev/mtdblock4 rw mtdids=nor0:spi0.0 mtdparts=spi0.0:2m(boot),10m(kernel),1m(xen),1m(system-dtb),48m(rootfs), 2m(others)"
	setenv fdt_high 0xffffffffffffffff
	booti 0x2400000 - 0x2600000
	EOF
	
	mkimage -c none -A arm -T script -d ${boot_scr} ${out_boot_scr}
}

function generate_root_jfss2()
{
	local path=$1
	local rootfs_dir=${path}/mini-rootfs
	
	echo "generate rootfs jffs2 for qspi flash............." 
	rm -rf ${path}/${rootfs_jffs2}
	rm -rf ${rootfs_dir}
	mkdir ${rootfs_dir}
	tar xzf ${path}/rootfs.tar.gz -C ${rootfs_dir}
	rm -rf ${rootfs_dir}/usr/share/qemu 
	rm -rf ${rootfs_dir}/usr/bin/qemu-system-aarch64
	rm -rf ${rootfs_dir}/usr/bin/qemu-system-i386
	
	./mkfs.jffs2 -n -l -s 0x4000 -e 0x10000 -p 0x3000000 -r ${rootfs_dir} -o ${path}/${rootfs_jffs2}
	
	get_file_size ${path}/${rootfs_jffs2}
	echo "rootfs.jffs2 size is $file_size"
}

function fw_package()
{
	local path=$1
	local out_file=$2
	
	BOOT_BIN_packaget ${path}
		
	generate_root_jfss2 ${path}

	generate_boot_scr ${path}/qflash_boot.scr ${path}/Image ${path}/Image.gz ${path}/xen ${path}/system.dtb ${path}/${rootfs_jffs2}
	
	echo "firmwave package............." 
	
	dd if=/dev/zero bs=1M count=64 | tr "\000" "\377" > ${out_file}
	dd if=${path}/BOOT.BIN of=${out_file} seek=0 bs=1 conv=notrunc 
	dd if=${path}/Image.gz of=${out_file} seek=${kernel_offset} bs=1 conv=notrunc
	dd if=${path}/xen of=${out_file} seek=${xen_offset} bs=1 conv=notrunc 
	dd if=${path}/system.dtb of=${out_file} seek=${system_dtb_offset} bs=1 conv=notrunc 
	dd if=${path}/${rootfs_jffs2} of=${out_file} seek=${rootfs_offset} bs=1 conv=notrunc 
	dd if=${path}/qflash_boot.scr of=${out_file} seek=${boot_scr_offset} bs=1 conv=notrunc 
}


function usage()
{
	local scripte=`basename $0`
	echo "usage:"
	echo "sudo ./$scripte [-d images_path] [-o fw_name]"
	echo "$scripte -h"
	echo "	-d images file path, default is \"images/linux/\""
	echo "	-o fw_name : the output firmware file name"
	echo "QSPI Flash partitions"
	echo "[2 M]   boot"
	echo "[10M]   kernel"
	echo "[1 M]   xen"
	echo "[1 M]   system.dtb"
	echo "[48M]   rootfs"
	echo "[2 M]   others"
	echo "boot.src base on others partition, offset is 0x80000, lenght is 0x80000"
}

images_path=./images/linux
fw_name=petalinux-xen-fw.bin

while getopts "d:h" opt; do
	case ${opt} in
	d)
		images_path = ${OPTARG}
		;;
	o)
		fw_name = ${OPTARG}
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

#files=zynqmp_fsbl.elf,u-boot.elf,pmwfw.elf,Image,Image.gz,xen,system.dtb,rootfs.jffs2
files=zynqmp_fsbl.elf
for i in $files 
do
	target_file=${images_path}/${i}
	if [ ! -f $target_file ] 
	then
		echo "can't find $target_file "
		exit 1
	fi
done



rm -rf ${fw_name0}
fw_package ${images_path} ${fw_name}

if [ -a ${fw_name} ] 
then 
	echo "-------------done----------------" 
else
	echo "-----------fail----------------" 
fi



