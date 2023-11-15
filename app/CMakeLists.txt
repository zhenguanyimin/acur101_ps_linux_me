# cmake file for xilinx 7ev
# by chenyili

cmake_minimum_required(VERSION 3.10)
project(linux_app)

set(CMAKE_BUILD_TYPE "Release")

#set cross compile tools and the sysroot
set(CMAKE_SYSTEM_NAME "Linux")

# 使用环境变量CC和CXX指定的编译器时，cmake命令增加-DCI=1选项
if (NOT CI)
set(CMAKE_C_COMPILER /home/a22800/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER /home/a22800/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-g++)
#set(CMAKE_C_COMPILER /opt/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-gcc)
#set(CMAKE_CXX_COMPILER /opt/gcc-arm-10.3-2021.07-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-g++)
endif()

#外部传入版本号
if (VERSION)
	set(CMAKE_C_FLAGS "-DEMBED_SOFTWARE_PS_VERSION_STR='\"${VERSION}\"'")
endif()

include_directories(lib)

#set compile options
add_definitions(-mcpu=cortex-a72.cortex-a53)
add_definitions(-march=armv8-a+crc)
add_definitions(-fstack-protector-strong)
add_definitions(-O2)
add_definitions(-D_FORTIFY_SOURCE=2)
add_definitions(-Wformat -Wformat-security -Werror=format-security -Wdate-time)
add_definitions(-D__XEN_LINUX__)
add_definitions(-Wno-date-time)
#add_definitions(-g)


#add source file directories
add_subdirectory(src)

link_directories(lib)

#general the target file
add_executable(${PROJECT_NAME}.elf src/main.c) 
target_link_libraries(${PROJECT_NAME}.elf 
	-Wl,--start-group 
	${SRC_LIBS}
	pthread
	rt
	m
	-Wl,--end-group
	)


