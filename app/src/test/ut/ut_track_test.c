

/* FreeRTOS includes. */
#include "ut_track_test.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS+CLI includes. */
#include "../../srv/cli/cli_if.h"
#include "../../app/DataPath/data_path.h"
#include "../../app/Detection/detection_interface.h"
#include "../../app/Tracking/tracking_interface.h"

// extern uint32_t gBeamMode ;

static BaseType_t  track_set_beam_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0 ;
	s32 type = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if(len1>0)
	{
		type = strtoul(param1, NULL, 0);

		if ((1==type)||(2==type))
		{
			set_beam_mode(type);
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
		}else
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");

	}else
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");

	return pdFALSE ;
}

static BaseType_t  track_set_beam_azi_scan_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0, len2 = 0;
	s32 center = -1;
	s32 scope = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	const char *param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);

	if ((len1 > 0) && (len2 > 0))
	{
		center = strtoul(param1, NULL, 0);
		scope = strtoul(param2, NULL, 0);

		if (((center >= -60) && (center <= 60)) && ((scope >= 0) && (scope <= 120)))
		{
			set_beam_azi_scan_para(center, scope);
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
		}else
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");

	}else
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");

	return pdFALSE ;
}

static BaseType_t  track_set_beam_ele_scan_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0, len2 = 0;
	s32 center = -1;
	s32 scope = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	const char *param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);

	if ((len1 > 0) && (len2 > 0))
	{
		center = strtoul(param1, NULL, 0);
		scope = strtoul(param2, NULL, 0);

		if (((center >= -20) && (center <= 20)) && ((scope >= 0) && (scope <= 40)))
		{
			set_beam_ele_scan_para(center, scope);
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
		}else
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");

	}else
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");

	return pdFALSE ;
}

static BaseType_t  track_set_fix_beam_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0, len2 = 0;
	s32 eleCenter = -1;
	s32 aziCenter = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	const char *param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);

	if ((len1 > 0) && (len2 > 0))
	{
		eleCenter = strtoul(param1, NULL, 0);
		aziCenter = strtoul(param2, NULL, 0);

		if (((eleCenter >= -20) && (eleCenter <= 20)) && ((aziCenter >= -60) && (aziCenter <= 60)))
		{
			set_beam_mode(1);
			set_fix_beam_para(eleCenter, aziCenter);
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
		}
		else
		{
			set_beam_mode(2);
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
		}
	}
	else
	{
		set_beam_mode(2);
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE ;
}

static BaseType_t  track_set_scan_beam_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0, len2 = 0, len3 = 0, len4 = 0;
	s32 eleCenter = -1;
	s32 eleScope = -1;
	s32 aziCenter = -1;
	s32 aziScope = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);
	const char *param2 = FreeRTOS_CLIGetParameter(pcCommandString, 2, &len2);
	const char *param3 = FreeRTOS_CLIGetParameter(pcCommandString, 3, &len3);
	const char *param4 = FreeRTOS_CLIGetParameter(pcCommandString, 4, &len4);

	if ((len1 > 0) && (len2 > 0) && (len3 > 0) && (len4 > 0))
	{
		eleCenter = strtoul(param1, NULL, 0);
		eleScope = strtoul(param2, NULL, 0);
		aziCenter = strtoul(param3, NULL, 0);
		aziScope = strtoul(param4, NULL, 0);

		if (((eleCenter >= -20) && (eleCenter <= 20)) && ((eleScope >= 0) && (eleScope <= 40)) && \
			((aziCenter >= -60) && (aziCenter <= 60)) && ((aziScope >= 0) && (aziScope <= 120)))
		{
			set_beam_mode(2);
			set_scan_beam_para(eleCenter, eleScope, aziCenter, aziScope);
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
		}
		else
		{
			set_beam_mode(1);
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
		}
	}
	else
	{
		set_beam_mode(1);
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE ;
}

static BaseType_t  track_set_work_mode_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 workMode = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if ((len1 > 0))
	{
		workMode = strtoul(param1, NULL, 0);

		if ((workMode >= 0) && (workMode <= 1))
		{
			set_beam_workmode(workMode);
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK");
		}
		else
		{
			set_beam_mode(1);
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
		}
	}
	else
	{
		set_beam_mode(1);
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE ;
}

static BaseType_t  track_set_abuDhabi_show_scenes_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 scenes = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if ((len1 > 0))
	{
		scenes = strtoul(param1, NULL, 0);

		if (scenes >= 0)
		{
			if( SetAbuDhabiShowScenes(scenes) )
				snprintf(pcWriteBuffer, xWriteBufferLen, "OK, abuDhabi show scenes:%d", scenes);
			else
				snprintf(pcWriteBuffer, xWriteBufferLen, "FAIL");
		}
		else
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
		}
	}
	else
	{
		set_beam_mode(1);
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE ;
}

static BaseType_t  track_set_tracking_mode_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 mode = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if ((len1 > 0))
	{
		mode = strtoul(param1, NULL, 0);

		if (mode >= 0)
		{
			SetTrackingMode(mode);
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK, set tracking mode:%d", mode);
		}
		else
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
		}
	}
	else
	{
		set_beam_mode(1);
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE ;
}

static BaseType_t  print_alg_proc_time_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 enable = -1;
	uint32_t procTimeDetection = 0;
	uint32_t procTimeTracking = 0;
	uint32_t procTimeReport = 0;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if ((len1 > 0))
	{
		enable = strtoul(param1, NULL, 0);

		if (enable == 1)
		{
			procTimeDetection = GetPrcoTimeDetection();
			procTimeTracking = GetPrcoTimeTracking();
			procTimeReport = GetPrcoTimeReport();
			snprintf(pcWriteBuffer, xWriteBufferLen, "alg proc time, Detection:%dus, Tracking:%dus, Report:%dus\r\n",
					procTimeDetection, procTimeTracking, procTimeReport);
		}
		else
		{
			;
		}
	}
	else
	{
		set_beam_mode(1);
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE ;
}

extern ret_code_t enable_wave_generation(void);
extern ret_code_t disable_wave_generation(void);
static BaseType_t  track_set_tracking_en_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 mode = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if ((len1 > 0))
	{
		mode = strtoul(param1, NULL, 0);

		if (mode >= 0)
		{
			if( mode )
			{
				enable_wave_generation();
			}
			else
			{
				disable_wave_generation();
			}
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK, set tracking en:%d", mode);
		}
		else
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
		}
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE ;
}

#include "../../bsp/reg/pl_reg.h"
#define TRACK_UT_WRITE_REG(addr,data)		PL_REG_WRITE(0xFFFF&addr,data)
#define TRACK_UT_READ_REG(addr)			PL_REG_READ(0xFFFF&addr)
int aaaaaaamode = 0;
static BaseType_t  track_set_tracking_mode_handler_a(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{
	BaseType_t len1 = 0;
	s32 mode = -1;
	const char *param1 = FreeRTOS_CLIGetParameter(pcCommandString, 1, &len1);

	if ((len1 > 0))
	{
		mode = strtoul(param1, NULL, 0);

		if (mode >= 0)
		{
			aaaaaaamode = mode;
			if( 1 == mode )
			{
				TRACK_UT_WRITE_REG( 0x800203a0, 0x189c07d0 );
                		TRACK_UT_WRITE_REG( 0x800203a8, 0x10010898 );
		                TRACK_UT_WRITE_REG( 0x8002042c, 0x01 );
                		TRACK_UT_WRITE_REG( 0x8002042c, 0x00 );
			}
			snprintf(pcWriteBuffer, xWriteBufferLen, "OK, set velunambi mode:%d", mode);
		}
		else
		{
			snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
		}
	}
	else
	{
		snprintf(pcWriteBuffer, xWriteBufferLen, "ERROR");
	}

	return pdFALSE ;
}


static const CLI_Command_Definition_t track_set_beam =
{
	"TrackBeamSet",
	"\r\n TrackBeamSet <type>:\r\n type:1 for fix beamform ,2 for beam scanning\r\n",
	track_set_beam_handler,
	1
};

static const CLI_Command_Definition_t track_set_beam_azi_scan =
{
	"TrackBeamAziScanSet",
	"\r\n TrackBeamAziScanSet <center> <scope>:\r\n set azimuth scanning center and scope\r\n",
	track_set_beam_azi_scan_handler,
	2
};

static const CLI_Command_Definition_t track_set_beam_ele_scan =
{
	"TrackBeamEleScanSet",
	"\r\n TrackBeamEleScanSet <center> <scope>:\r\n set elevation scanning center and scope\r\n",
	track_set_beam_ele_scan_handler,
	2
};

static const CLI_Command_Definition_t track_set_fix_beam =
{
	"TrackFixBeamSet",
	"\r\n TrackFixBeamSet <eleCenter> <aziCenter>:\r\n set fix beam center\r\n",
	track_set_fix_beam_handler,
	2
};

static const CLI_Command_Definition_t track_set_scan_beam =
{
	"TrackScanBeamSet",
	"\r\n TrackScanBeamSet <eleCenter> <eleScope> <aziCenter> <aziScope>:\r\n set scan beam center and scope\r\n",
	track_set_scan_beam_handler,
	4
};

static const CLI_Command_Definition_t track_set_work_mode =
{
	"TrackWorkModeSet",
	"\r\n TrackWorkModeSet <workMode>:\r\n set tracking work mode, 0-TWS searching; 1-TAS tracking\r\n",
	track_set_work_mode_handler,
	1
};

static const CLI_Command_Definition_t track_set_abuDhabi_show_scenes =
{
	"TrackAbuDhabiShowScenesSet",
	"\r\n TrackAbuDhabiShowScenesSet <scenes>:\r\n set AbuDhabi show scenes\r\n",
	track_set_abuDhabi_show_scenes_handler,
	1
};

static const CLI_Command_Definition_t track_set_tracking_mode =
{
	"TrackModeSet",
	"\r\n TrackModeSet <scenes>:\r\n set tracking mode\r\n",
	track_set_tracking_mode_handler,
	1
};

static const CLI_Command_Definition_t track_set_tracking_mode_a =
{
	"TrackModeSetvelunambi",
	"\r\n TrackModeSetvelunambi <scenes>:\r\n set tracking velunambi\r\n",
	track_set_tracking_mode_handler_a,
	1
};

static const CLI_Command_Definition_t print_alg_proc_time_en =
{
	"PrintAlgProcTimeEn",
	"\r\n PrintAlgProcTimeEn <enable>:\r\n print algorithm process time, 1-enable; 0-disable\r\n",
	print_alg_proc_time_handler,
	1
};

static const CLI_Command_Definition_t track_set_tracking_en =
{
	"TrackSetEn",
	"\r\n TrackSetEn <scenes>:\r\n set tracking en\r\n",
	track_set_tracking_en_handler,
	1
};

void ut_track_init(void)
{
	FreeRTOS_CLIRegisterCommand(&track_set_beam);
	FreeRTOS_CLIRegisterCommand(&track_set_beam_azi_scan);
	FreeRTOS_CLIRegisterCommand(&track_set_beam_ele_scan);
	FreeRTOS_CLIRegisterCommand(&track_set_fix_beam);
	FreeRTOS_CLIRegisterCommand(&track_set_scan_beam);
	FreeRTOS_CLIRegisterCommand(&track_set_work_mode);
	FreeRTOS_CLIRegisterCommand(&track_set_abuDhabi_show_scenes);
	FreeRTOS_CLIRegisterCommand(&track_set_tracking_mode);
	FreeRTOS_CLIRegisterCommand(&print_alg_proc_time_en);
	FreeRTOS_CLIRegisterCommand(&track_set_tracking_en);
	FreeRTOS_CLIRegisterCommand(&track_set_tracking_mode_a);
}
