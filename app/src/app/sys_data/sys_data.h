#ifndef SYS_DATA_H
#define SYS_DATA_H

#include "../../inc/radar_error.h"
#include "../../inc/radar_common.h"

#include "../../srv/protocol/protocol_object.h"
#include "../../srv/protocol/protocol_system.h"
#include "../../srv/protocol/protocol_parameter.h"

ret_code_t sys_data_get_info(int type, void *data, uint32_t len);

ret_code_t sys_data_push_detected( protocol_object_list_detected_t *detected,
		 	 	 	 	 	 	 	 	 protocol_object_list_tracked_t *tracked,
										 protocol_object_list_dot_cohe_t *dot_cohe,
										 protocol_beam_scheduling_t *beam_scheduling,
										 protocol_radar_status_t *status);

ret_code_t sys_data_push_attitude(void *attitude, uint32_t len);

ret_code_t sys_data_push_calibrite_data( int32_t iLongitude, int32_t iLatitude, int32_t iAltitude, int32_t iHeading, int32_t iPitching, int32_t iRolling );

#endif
