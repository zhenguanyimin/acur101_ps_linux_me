
#include "unit_test.h"
#include "ut_0165_drv.h"
#include "ut_chn_ctrl.h"
#include "ut_sys_ctrl.h"
#include "ut_ant_test.h"
#include "ut_protocol.h"
#include "ut_cfg_param.h"
#include "ut_dataPath_test.h"
#include "ut_track_test.h"
#include "ut_perf.h"
#include "ut_adc_test.h"
#include "ut_pl_test.h"
#include "ut_alg_para_config_test.h"
#include "ut_platform_info_test.h"
#include "ut_lsm303ah_test.h"
#include "ut_soc.h"
#include "ut_gyro_test.h"
#include "ut_version.h"
#include "ut_wit_test.h"
#include "ut_net_cfg.h"
#include "ut_power_dissipation.h"

void uint_test_init(void)
{
	ut_0165_drv_init();

	ut_chn_ctrl_init();

	ut_sys_ctrl_init();

	ut_ant_test_init();

	ut_protocol_init();

	ut_cfg_param_init();

	ut_datapath_init();

	ut_track_init();

	ut_perf_init();
	
	ut_adc_test_init();

	ut_pl_test_init();

	ut_alg_para_config_test_init();

	ut_platform_info_test_init();
	
	ut_lsm303ah_init();
	
	ut_soc_init();

	ut_gyro_init();

	ut_version_init();

	ut_wit_test_init();
	
	ut_net_cfg_init();
	
	ut_power_dissipation_init();

	return;
}
