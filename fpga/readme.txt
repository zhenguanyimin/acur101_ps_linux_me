----------------------------------------------------------
文件名称：acur101_fpga_01010029.bin
		acur101_fpga_01010029.bit
版本号：01010029
发布时间： 2022/09/05
git 版本号 : 01010029 (tag)
描述： 
	T4 新版雷达的正式固件 
修改点：
	1. fix pil  cha chb bug 
逻辑自测试：
	
对其他方向影响:
	版本正常升级，不需要算法和软件版本适配（兼容升级）
----------------------------------------------------------
文件名称：acur101_fpga_01010028.bin
		acur101_fpga_01010028.bit
版本号：01010027
发布时间： 2022/08/24
git 版本号 : 01010028 (tag)
描述： 
	T4 新版雷达的正式固件 
修改点：
	1. fix pil bug 
逻辑自测试：
	1.adc 测试正常
对其他方向影响:
	版本正常升级，不需要算法和软件版本适配（兼容升级）
----------------------------------------------------------
文件名称：acur101_fpga_01010027.bin
		acur101_fpga_01010027.bit
版本号：01010027
发布时间： 2022/08/24
git 版本号 : 01010027 (tag)
描述： 
	T4 新版雷达的正式固件 
修改点：
	1.修复帧号异常问题（info 上报功能单独一路数据写入DDR）
	2.适配128*1024 采数
逻辑自测试：
	1.adc 测试正常
对其他方向影响:
	版本正常升级，不需要算法和软件版本适配（兼容升级）
----------------------------------------------------------
文件名称：acur101_fpga_01010026.bin
		acur101_fpga_01010026.bit
版本号：01010026
发布时间： 2022/08/01
git 版本号 : 01010026 (tag)
描述： 
	T4 新版雷达的正式固件 
修改点：
	1.添加IIC中断
	2.修改cpi周期tag信息错误
逻辑自测试：
	1.adc 测试正常
对其他方向影响:
	版本正常升级，不需要算法和软件版本适配
----------------------------------------------------------
文件名称：acur101_fpga_01010025.bin
		acur101_fpga_01010025.bit
版本号：01010025
发布时间： 2022/07/28
git 版本号 : 01010025 (tag)
描述： 
	T4 新版雷达的正式固件 
修改点：
	1. 新增加tag信息上报
	2. 修复波位号上报错误
	3. 新增加ADC 饱和阈值检测（待软件实现）
	4. 新增ADC 读取寄存器功能，后需做adc异常检测和相位校准（待软件实现）
逻辑自测试：
	1.adc 测试正常
对其他方向影响:
	版本正常升级，不需要算法和软件版本适配
---------------------------------------------------------
文件名称：acur101_fpga_01010023.bin
		acur101_fpga_01010023.bit
版本号：01010023
发布时间： 2022/07/13
git 版本号 : 01010023 (tag)
描述： 
	T4 新版雷达的正式固件 
修改点：
	1.修改相位校准默认值。
	2.新增adc数据调试信息寄存器控制输出
	3.修复lvds fclk idelay语法错误
逻辑自测试：
	1.adc 测试正常
对其他方向影响:
	版本正常升级，不需要算法和软件版本适配
----------------------------------------------------------
文件名称：acur101_fpga_01010022.bin
		acur101_fpga_01010022.bit
版本号：01010022
发布时间： 2022/07/08
git 版本号 : 01010022 (tag)
描述： 
	T4 新版雷达的正式固件
修改点：
	1.新增pil 功能
	2.相位校准修改点：
		1）校准系数范围修改为（-8,8)
		2)校准系数默认值修改为 0x80022050 :0xFD780c32  , 0x80022054 : 1000 ;
	3.修改二维fft_iq 上传ps ddr 数据格式
逻辑自测试：
	1.adc 测试正常
----------------------------------------------------------

文件名称：acur101_fpga_01010019.bit
版本号：01010019
发布时间： 2022/04/24
git 版本号 : 01010018

修改点：
	1.新增rfft 截位
	2.修复杂波抑制版本问题：
		1) 滤波器系数转转置问题
		2）内部计算符号位扩展问题。
		3) 调整log2模块定点转浮点位宽。
	3.修改前两个chirp diasble。
逻辑自测试：
	1.adc 测试正常
	2.ad 数据注入三帧adc 数据结果和matlab 数据保持一致。

----------------------------------------------------------
----------------------------------------------------------

文件名称：acur101_fpga_01010018.bit
版本号：01010018
发布时间： 2022/04/17
git 版本号 : 01010018

修改点：
	1.修改时钟网络，芯片外设配置采用ps 时钟，并减少系统中整体的时钟个数，优化时钟结构1
	2.优化资源，修改row2col 模块节省 9 个uram 存储空间，并支持多种模式。
	3.adc_rx_top 优化结构，模块解耦。
	4.支持 4096*32 2048*64 1024*128 三种模式。（2048*64 后面需要调整）
	5.支持 4096*32  正常模式，adc 采集，以及adc数据注入 （PIL ）
	6.支持 1024*128 正常模式，adc 采集，以及adc数据注入（PIL  PS 需要提供支持）
	7.支持FIR 杂波抑制版本（1. matlab 版本需要对齐，2.杂波系数需要同步）
	8.支持第一个chirp disable 功能。（需要配置波形寄存）
	9.支持adc 抽取模式，（现在adc 只能工作在抽取模式，系统起来需要配置寄存器）

逻辑自测试：
	1.adc 测试正常
	2.ad 数据注入三帧adc 数据结果和matlab 数据保持一致。

----------------------------------------------------------

文件名称：acur101_fpga_01010015.bit
版本号：01010015
发布时间： 2022/02/23
git 版本号 : 01010015

修改点：
	1. 新增dma1 adc 上传，满足算法将adc数据和检测结果同时上传
	2. 修改pri 默认周期为265us
	3.修改log2 取对数模块和matlab 结果保持一致
	4.删除部分mark_debug 信号

逻辑自测试：
	1.adc 测试正常
	2.ad 数据注入三帧adc 数据结果和matlab 数据保持一致。

----------------------------------------------------------
----------------------------------------------------------

文件名称：acur101_fpga_01010014.bit
版本号：01010014 
发布时间： 2022/02/13
git 版本号 :

新增功能：
		1.修复adc解串异常问题
		2.新增dsp二维fft
		3.新增sysmon模块
		4.新增杂波图功能

逻辑自测试：
		1.adc 测试正常

----------------------------------------------------------

文件名称：acur101_fpga_01010011.bit
版本号：01010011 
发布时间： 2022/01/10
git 版本号 : SHA-1: 04f094cad35d7f1d389979744c35c835b62b1613

新增功能：
		1.dma adc 数据回环注入功能修改 pL tready 拉低的问题。

逻辑自测试：
		1.dma 数据回环注入逻辑功能已测试。

----------------------------------------------------------

文件名称：acur101_fpga_01010010.bit
版本号：01010010 
发布时间： 2022/12/26
新增功能：
		1.dma adc 数据回环注入功能修改 pL tready 拉低的问题。

逻辑自测试：
		1.dma 数据回环注入逻辑功能已测试。

----------------------------------------------------------

文件名称：acur101_fpga_01010009.bit
版本号：01010009 
发布时间： 2022/12/26
新增功能：
		1. acc_magint 新增管脚
		2. ADC 电压监控
		3. 0165控制寄存器
逻辑自测试：
		1.adc 电压监控功能已验证。
		
