/*
* skyfend 
*/
#include <stdlib.h>
#include <stdint.h>

/* Standard includes. */
#include "srv/cli/cli_if.h"
#include "srv/alink/alink.h"
#include "app/c2_network/c2_network.h"
#include "test/ut/unit_test.h"
#include "srv/protocol/protocol_if.h"
#include "srv/eth_link/eth_link.h"

#include "app/beam_calculate/bk_cal.h"
#include "app/Tracking/src/alg_init/alg_init.h"
#include "app/Wave_Configuration/wave_config.h"
#include "drv/awmf_0165/awmf_0165_drv.h"
#include "app/Detection/detection_interface.h"
#include "bsp/reg/pl_reg.h"
#include "srv/net_cfg/net_cfg.h"

#include "app/DataPath/data_path.h"
#include "srv/wit/wit_service.h"
#include "service/service.h"
#include "app/temp_table/temp_table.h"
#include "app/DevInfoProcess/DevInfoProcess.h"

//test
//#include "common/table_2d/table_2d.h"

//#include <gps.h>
#include <stdio.h>

int main(int argc, char **argv)
{

	simple_dcache_init();//disable PL share memory to use dcache, this must be initialized alone, we put it at begin of program without any disturbing.
	
	pl_reg_init();
	
	service_init();

	radar_network_init(); //network configuration may rely on storage sevice, will init next to service.
	
	temp_table_load(); //the other app will use tempeture table, will initialize in advance. 

	log_init();
	APP_LOG_INFO("%s start....", argv[0]);

	init_amp_phase_para();

	VCOPLLInitFunction();
	(void)awmf_0165_dev_init();

	alg_init_init();

	DetectAlgInit();

	wit_service_start("witd");

	eth_link_init();
	alink_init();
	c2_network_init();

    uint_test_init();
	
	protocol_init();
	cli_init();

	data_path_init();

	DevInfo_init();
	
	while(1)
	{
#if 0
		struct gps_data_t gpsdata;
		int status;
	
		// 初始化gps_data结构
//		gps_init();
		
		// 连接到gpsd守护进程
		if (gps_open("localhost", "2947", &gpsdata) != 0) {
			fprintf(stderr, "无法连接到gpsd.\n");
			return 1;
		}
	
		// 设置需要的GPS数据类型
		gps_stream(&gpsdata, WATCH_ENABLE | WATCH_JSON, NULL);
	
		// 读取位置信息
		while (1) {
			// 等待新的GPS数据可用
			if (gps_waiting(&gpsdata, 500)) {
				// 读取最新的GPS数据
				if ((status = gps_read(&gpsdata, NULL, 0)) > 0) {
					if (status == -1) {
						fprintf(stderr, "读取GPS数据错误.\n");
					} else {
						printf("纬度: %.6f, 经度: %.6f\n", gpsdata.fix.latitude, gpsdata.fix.longitude);
					}
				}
			}
		}
	
		// 断开与gpsd的连接
		gps_stream(&gpsdata, WATCH_DISABLE, NULL);
		gps_close(&gpsdata);
#endif
		sleep(1);
	}

	return 0;
}

