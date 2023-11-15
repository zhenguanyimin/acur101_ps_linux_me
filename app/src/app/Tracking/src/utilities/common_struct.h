
#ifndef COMMON_STRUCT_H_
#define COMMON_STRUCT_H_

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "object_struct.h"
#include "myMath.h"
#include "common_define.h"

//! @brief The important data in algorithm
typedef struct sAlgObjData {

	sDetectFrame *detectsListOutput; // the output of signal process module

	void *measurementProcess_inst;		// for the initialization of measurement process module

	void *tracking_inst; // for the initialization of tracking module

	void *dispatch_inst;	// for the initialization of dispatch module

	void* targetThreatAssessment_inst; // for the initialization of target threat assessment module

	sScanType scanType;
	sVelUnambiType velUnambigType;
	sPlatformInfo *platformInfo;

	sDebugAndCtrlParam *debugAndCtrlParam;
	sSelfAlgModuleStatus *allModuleStatus; // the module status for all algorithm

	// The memory size of scratchPad for each module
	uint32_t measScratchPad_memSize;
	uint32_t trackingScratchPad_memSize;
	//uint8_t* trackerExisting_scratchPad; // the scratch pad for tracker existing probability module
}sAlgObjData;

#endif // COMMON_STRUCT_H_
