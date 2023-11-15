#!/bin/bash

set -e
#set -x
##########
#
# usage: ./build_ci.sh boardname, boardname is t4 or t5 currently, for instance, ./build_ci.sh t5
#
#########

cd $(dirname $0)
PROJECT_ROOT=$(git rev-parse --show-toplevel)
BITSTREAM_PATH=build/tmp/work/zynqmp_generic-xilinx-linux/fpga-manager-util/xilinx+gitAUTOINC+24d29888d0-r0/recipe-sysroot/boot/bitstream
RADAR_BOARD="t5" #the default board is t5
FPGA_DTS_LOCATION=project-spec/meta-user/recipes-bsp/device-tree/files
FPGA_DTS_FILE_NAME=fpga_source_dts
FPGA_DTS_FILE=${FPGA_DTS_LOCATION}/${FPGA_DTS_FILE_NAME}.dtsi

function compile_fpga_dts()
{
	#------------------------compile fpga dts--------------------
	 
	local boardname
	local macro

	if [[ "$1" == "" ]];then
		boardname=`echo "${RADAR_BOARD}" | tr a-z A-Z`
	else
		boardname=`echo "$1" | tr a-z A-Z`
	fi
	
	macro="RADAR_BOARD_${boardname}"
	echo "fpga dts using board ${boardname}"
	gcc -I fpga_dts -E -nostdinc -undef -D__DTS__ -x assembler-with-cpp -o ${FPGA_DTS_LOCATION}/pre_tmp.dtsi ${FPGA_DTS_FILE} -D${macro}
	dtc -I dts -O dtb -o  ${FPGA_DTS_LOCATION}/${FPGA_DTS_FILE_NAME}.dtb ${FPGA_DTS_LOCATION}/pre_tmp.dtsi 
	rm ${FPGA_DTS_LOCATION}/pre_tmp.dtsi
}

function board_select_and_varify_env()
{
# hardware board version
# 		T5; radar_board_t5.dtsi
# 		T4: radar_board_t4.dtsi
# 		...
	if [[ "$1" != "" ]]; then
		RADAR_BOARD="$(echo "$1" | tr '[:upper:]' '[:lower:]')"
	fi
	echo "board: ${RADAR_BOARD}"
	BOARD_SPECIFIC_DTS_FILE="radar_board_${RADAR_BOARD}.dtsi"
	export BOARD_SPECIFIC_DTS_FILE
	export RADAR_BOARD
	export BB_ENV_EXTRAWHITE="BOARD_SPECIFIC_DTS_FILE BOARD_NAME"
}

function build_system()
{
	set +e
	rm -rf build/tmp/stamps images || true
	mkdir -p ${BITSTREAM_PATH}
	rm ${BITSTREAM_PATH}/acur101_fpga_*.bit || true
	cp ${PROJECT_ROOT}/fpga/acur101_fpga_*.bit ${BITSTREAM_PATH} || true
	petalinux-build

	if [[ "$?" != "0" ]];then
   		mkdir -p ${BITSTREAM_PATH}
    	cp ${PROJECT_ROOT}/fpga/acur101_fpga_*.bit ${BITSTREAM_PATH}
		set -e
    	petalinux-build
	fi
	set -e
}

function package_lowlevel_fw()
{
#default package type is mmc, which is used currently.

	if [[ "$1" == "flash" ]];then
		echo "package flash firmware...."
		bash petalinux-xen-package-qspiflash.sh
	else
		echo "package emmc firmware...."
		bash petalinux-xen-package-emmc.sh
	fi
}

function usage()
{
	local scripte=`basename $0`
	echo "usage:"
	echo "./$scripte [buildway] [board] [type]"
	echo "$scripte all"
	echo "$scripte help"
	echo "$scripte fdts"
	echo "$scripte all t5 mmc"
	echo "buildway: all/fdts/h (all:build full firmware, fdts:build fpga dts, h: help)"
	echo "board: t5/t4 (t5:T5 hardware board as default config, the same as t4)"
	echo "type: storage type, flash/emmc"
}

function build_all()
{
	echo "build firmware begin..."
	board_select_and_varify_env $1
	compile_fpga_dts $1
	build_system 
	package_lowlevel_fw $2
	echo "build firmware successfully..."
}

function clean_all()
{
	set +e
	petalinux-build -x distclean
	rm build -rf
	rm images -rf
	set -e
	echo "clean project successfully"
}

case $1 in
	fdts)
		compile_fpga_dts $2
		;;
	help)
		usage 
		exit 0 
		;;
	clean)
		clean_all
		;;
	all|*)
		build_all $2 $3
		;;
esac

