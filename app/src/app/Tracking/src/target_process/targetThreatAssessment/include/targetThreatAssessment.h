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
	uint16_t classification; // 0:�״�����
	float x;
	float y;
	float z;
	float alert_radius; // ����뾶
	float weight; //�ñ�����Ŀ�����Ҫ��
}sProtectSpot;

typedef struct sIdandThreat {
	uint16_t id;
	float threat;
}sIDandThreat;

typedef struct sTargetThreatAssessmentInst {
	float position_weight; //��factorռ����в�ȵı��أ�Ĭ��Ϊ1����factor�ó˷���������в��
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