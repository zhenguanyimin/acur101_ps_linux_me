@echo off
cd ..
SET SRC_WORKSPACE=%cd:\=/%
SET VITIS_HOME=%VITIS_HOME:\=/%
SET QEMUDATA=%VITIS_HOME%/data/emulation/platforms/zynqmp/sw/a53_standalone/qemu
SET SPR=%SRC_WORKSPACE%/code/acur101_plt/platform.spr
SET HEADER_FILE=%SRC_WORKSPACE%/release/header_info.ini
SET XSA_DIR=%cd%\tool\hw_spec

REM 删除release目录下所有文件并重新生成
if exist build\release rd /S /Q build\release
	mkdir build\release

REM 获取vitis workspace
for /f "tokens=1,2 delims==" %%i in (build/conf.properties) do (
	set VITIS_WORKSPACE=%%j
)

REM 获取版本号
for /f "tokens=2,3" %%i in (code/acur101_app/src/cfg/radar_config.h) do (
	if "%%i" == "EMBED_SOFTWARE_PS_VERSION_STR" (
		SET VERSION=%%j
	)
)

REM 获取XSA文件名
for /f "delims=:" %%a in ('dir /b %XSA_DIR%\*.xsa ^| findstr /n .*') do SET count=%%a
	if %count% equ 1 (
		for /f "delims=" %%i in ('dir /s /b %XSA_DIR%\*.xsa') do SET XSA_FILE=%%~nxi
	) else (
		color cf & echo 检测到多个xsa文件，系统只允许一个xsa文件。
		pause
		exit 1
	)

REM 获取bif文件名
for /f "delims=:" %%a in ('dir /b %XSA_DIR%\*.bit ^| findstr /n .*') do SET count=%%a
	if %count% equ 1 (
		for /f "delims=" %%i in ('dir /s /b %XSA_DIR%\*.bit') do SET BIF_FILE=%%~nxi
	) else (
		color cf & echo 检测到多个bit文件，系统只允许一个bit文件。
		pause
		exit 1
	)

REM 将xsa同步到平台
if not exist %cd%\code\acur101_plt\hw\%XSA_FILE% (
	cp %XSA_DIR%\%XSA_FILE% %cd%\code\acur101_plt\hw\
)else (
	echo code\acur101_plt\hw\%XSA_FILE%已存在
)

@echo ********修改platform.spr********
python build/build_acur101.py %SPR% %QEMUDATA% %SRC_WORKSPACE%/tool/hw_spec/%XSA_FILE% "<platformDir>/hw"/%XSA_FILE%

@echo ********创建acur101_app_system.bif文件********
echo the_ROM_image:  >build/release/acur101_app_system.bif
echo {>>build/release/acur101_app_system.bif
echo     [bootloader, destination_cpu=a53-0] %SRC_WORKSPACE%/code/acur101_plt/export/acur101_plt/sw/acur101_plt/boot/fsbl.elf>>build/release/acur101_app_system.bif
echo     [destination_cpu=a53-0, exception_level=el-3] %SRC_WORKSPACE%/code/acur101_app/Debug/acur101_app.elf>>build/release/acur101_app_system.bif
echo     [destination_device=pl] %SRC_WORKSPACE%/tool/hw_spec/%BIF_FILE%>>build/release/acur101_app_system.bif
echo }>>build/release/acur101_app_system.bif

@echo ********正在编译中,请稍等几分钟********
call xsct build/acur101.tcl %VITIS_WORKSPACE% %SRC_WORKSPACE%/tool/hw_spec/%XSA_FILE%

if not exist code/acur101_app/Debug/acur101_app.elf (
	color cf & echo ********程序编译出错，请查看程序********
	pause
	exit 1
)else (
	call build/build_acur101.bat %VERSION%
)

if not exist build/release/*.bin (
	echo ********BIN文件生成失败********
	pause
	exit 1
)else (
	@echo ******** BIN文件生成成功 *******
	@echo ******** 开始生产OTA文件 *******
	@echo ******** %HEADER_FILE%  build/release/%VERSION:~1,-1%.bin  %VERSION:~1,-1% *******
	python build/build_acur101_OTA.py %HEADER_FILE% build/release/%VERSION:~1,-1%.bin %VERSION:~1,-1%
	if not exist build/release/%VERSION%_OTA.bin (
		echo ********OTA文件生成失败********
		pause
		exit 1

	)else (
		@echo ******** OTA文件生成成功 *******
		@echo ********拷贝物料********
		cp tool/flash_prog/fsbl_autel.elf build/release/fsbl_autel.elf
		cp %SRC_WORKSPACE%/tool/hw_spec/%BIF_FILE% build/release/%BIF_FILE%
		cp code/acur101_app/Debug/acur101_app.elf build/release/acur101_app.elf
		cp code/acur101_plt/export/acur101_plt/sw/acur101_plt/boot/fsbl.elf  build/release/fsbl.elf
	)
)
pause
@echo *******还原platform.spr文件*******
if exist code\acur101_plt\platform.spr_bak (
	del /q code\acur101_plt\platform.spr
	ren code\acur101_plt\platform.spr_bak platform.spr
)
pause

