
#ifndef ALINK_CONTROL_H
#define ALINK_CONTROL_H

#include "../../alink.h"

#pragma pack(1)
	typedef struct alink_control_start_detect_ack
	{
		uint8_t  status;
	}alink_control_start_detect_ack_t;
#pragma pack()

#pragma pack(1)
	typedef struct alink_control_close_detect_ack
	{
		uint8_t  status;
	}alink_control_close_detect_ack_t;
#pragma pack()

#pragma pack(1)
	typedef struct alink_control_set_beam_scheduling_size
	{
		uint8_t	eleScanCenter;
		uint8_t	eleScanScope;
		uint8_t	aziScanCenter;
		uint8_t	aziScanScope;
	}alink_control_set_beam_scheduling_size_t;
	typedef struct alink_control_set_beam_scheduling_size_ack
	{
		uint8_t  status;
	}alink_control_set_beam_scheduling_size_ack_t;
#pragma pack()

#pragma pack(1)
	typedef struct alink_control_mag_calibrite_ack
	{
		uint8_t  status;
	}alink_control_mag_calibrite_ack_t;
#pragma pack()

#pragma pack(1)
	typedef struct alink_control_amp_code
	{
		uint16_t	txComAmpCode[4];
		uint16_t	rxU19AmpCode[4];
		uint16_t	rxU23AmpCode[4];
	}alink_control_amp_code_t;
	typedef struct alink_control_amp_code_ack
	{
		uint8_t  status;
	}alink_control_amp_code_ack_t;
#pragma pack()

#pragma pack(1)
	typedef struct alink_control_amp_data
	{
		int16_t	initAmpData[384];
	}alink_control_amp_data_t;
	typedef struct alink_control_amp_data_ack
	{
		uint8_t  status;
	}alink_control_amp_data_ack_t;
#pragma pack()

#pragma pack(1)
	typedef struct alink_control_phase_data
	{
		int16_t	initPhaseData[384];
	}alink_control_phase_data_t;
	typedef struct alink_control_phase_data_ack
	{
		uint8_t  status;
	}alink_control_phase_data_ack_t;
#pragma pack()

#pragma pack(1)
	typedef struct alink_control_gyro_calibrite_ack
	{
		uint8_t  status;
	}alink_control_gyro_calibrite_ack_t;
#pragma pack()

#pragma pack(1)
	typedef struct alink_control_acc_calibrite
	{
		uint8_t byPara;
	}alink_control_acc_calibrite_t;
	typedef struct alink_control_acc_calibrite_ack
	{
		uint8_t  status;
	}alink_control_acc_calibrite_ack_t;
#pragma pack()

#pragma pack(1)
	typedef struct alink_control_angle_calibrite_ack
	{
		uint8_t  status;
	}alink_control_angle_calibrite_ack_t;
#pragma pack()

#pragma pack(1)
	typedef struct alink_control_manual_calibrite_data
	{
		int32_t iLongitude;
		int32_t iLatitude;
		int32_t iAltitude;
		int32_t iHeading;
		int32_t iPitching;
		int32_t iRolling;
	}alink_control_manual_calibrite_data_t;
	typedef struct alink_control_manual_calibrite_data_ack
	{
		uint8_t  status;
	}alink_control_manual_calibrite_data_ack_t;
#pragma pack()

#pragma pack(1)
		typedef struct alink_control_input_platform_data
		{
			uint8_t cbStatus;
			uint64_t timestamp;
			
			int32_t iLongitude;
			int32_t iLatitude;
			int32_t iAltitude;
			int32_t iHeading;
			int32_t iPitching;
			int32_t iRolling;

			int32_t	velocityNavi;
			int32_t headingNavi;
			int32_t heading_v;
			int32_t pitching_v;
			int32_t rolling_v;
			int32_t heading_a;
			int32_t pitching_a;
			int32_t rolling_a;			
		}alink_control_input_platform_data_t;
		typedef struct alink_control_input_platform_data_ack
		{
			uint8_t  status;
		}alink_control_input_platform_data_ack_t;
#pragma pack()

#pragma pack(1)
	typedef struct alink_control_auto_calibrite_data
	{
		int32_t iLongitude;
		int32_t iLatitude;
		int32_t iAltitude;
		int32_t iHeading;
		int32_t iPitching;
		int32_t iRolling;
	}alink_control_auto_calibrite_data_t;
#pragma pack()

void alink_control_init( void );

#endif /* ALINK_CONTROL_H */
