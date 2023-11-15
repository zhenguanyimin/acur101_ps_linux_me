
#include "sys_data.h"
#include "../../srv/alink/alink.h"
#include "../../srv/protocol/protocol_object.h"
#include "../../srv/protocol/protocol_system.h"
#include "../../srv/protocol/protocol_parameter.h"

ret_code_t sys_data_get_info(int type, void *data, uint32_t len)
{
	ret_code_t ret = RET_OK;

	switch( type )
	{
		case 0:
			if( len == sizeof(protocol_cfg_param_t) )
			{
//				memcpy( param_out->data, gConfigParmInfo, sizeof(protocol_cfg_param_t) );
				ret = RET_OK;
			}
			else
			{
				ret = RET_INVALID_PARAM;
			}
			break;
		default:
			ret = RET_INVALID_PARAM;
			break;
	}

	return ret;
}

ret_code_t sys_data_push_detected( protocol_object_list_detected_t *detected,
		 	 	 	 	 	 	 	 	 protocol_object_list_tracked_t *tracked,
										 protocol_object_list_dot_cohe_t *dot_cohe,
										 protocol_beam_scheduling_t *beam_scheduling,
										 protocol_radar_status_t *status)
{
	ret_code_t ret = RET_OK;

	alink_upload_tracked( tracked );
	alink_upload_beam_scheduling( beam_scheduling );

	return ret;
}

ret_code_t sys_data_push_attitude(void *attitude, uint32_t len)
{
	ret_code_t ret = RET_OK;
	
	ret = alink_upload_attitude( attitude );

	return ret;
}

struct sys_data_cali_t
{
	int32_t iLongitude;
	int32_t iLatitude;
	int32_t iAltitude;
	int32_t iHeading;
	int32_t iPitching;
	int32_t iRolling;
};

ret_code_t sys_data_push_calibrite_data( int32_t iLongitude, int32_t iLatitude, int32_t iAltitude, int32_t iHeading, int32_t iPitching, int32_t iRolling )

{
	ret_code_t ret = RET_OK;

	struct sys_data_cali_t cali = { 0 };

	cali.iLongitude = iLongitude;
	cali.iLatitude = iLatitude;
	cali.iAltitude = iAltitude;
	cali.iHeading = iHeading;
	cali.iPitching = iPitching;
	cali.iRolling = iRolling;
	
	ret = alink_upload_calibrite_data( &cali );

	return ret;
}

