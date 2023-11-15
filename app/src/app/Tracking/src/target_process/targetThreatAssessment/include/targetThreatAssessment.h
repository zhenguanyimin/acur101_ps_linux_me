#ifndef TARGETPROCESS_TARGET_THREAT_ASSESSMENT_H_
#define TARGETPROCESS_TARGET_THREAT_ASSESSMENT_H_

#include "../../../utilities/common_struct.h"
#include "../../tracking/include/tracking.h"
#include "../../dispatch/include/dispatch.h"


typedef struct sTargetThreat {
	uint16_t uid;
	uint16_t classification;
	float position_threat[MAXNUMOFPROTECTSPOTS];
	float direction_threat[MAXNUMOFPROTECTSPOTS];
	float velocity_threat[MAXNUMOFPROTECTSPOTS];
	float threat[MAXNUMOFPROTECTSPOTS];
	float threat_max;
	uint16_t threat_max_spot;
}sTargetThreat;

typedef struct sProtectSpot {
	uint16_t id;
	uint16_t classification; // 0:雷达自身
	float x;
	float y;
	float z;
	float alert_radius; // 警戒半径
	float weight; //该被保护目标的重要性
}sProtectSpot;

typedef struct sIdandThreat {
	uint16_t id;
	float threat;
}sIDandThreat;

typedef struct sTargetThreatAssessmentInst {
	float position_weight; //各factor占总威胁度的比重，默认为1，各factor用乘法计算总威胁度
	float direction_weight;
	float velocity_weight;
	uint16_t numOfProtectSpot;
	sTargetThreat target_threat[MAX_NUM_TRAJS];
	sProtectSpot protect_spots[MAXNUMOFPROTECTSPOTS];
	sIDandThreat threat_descend[MAX_NUM_TRAJS];
}sTargetThreatAssessmentInst;

void targetThreatAssessment_process(sAlgObjData* algObj);
void tastwsListConstruct(sAlgObjData* algObj);
void sortTrackListTTA(sTracking_ListObj* trackList, sTargetThreatAssessmentInst* inst_TTA, uint16_t trajNum);
void sortTrackListManual(sTracking_ListObj* trackList, uint16_t* manualTasTraj, uint16_t* trajNum, sTracking_moduleInstance* trackingModule_inst);

typedef enum
{
	THREAT_LEVEL1 = 1, 
	THREAT_LEVEL2, 
	THREAT_LEVEL3 
} THREAT_LEVEL;

#endif