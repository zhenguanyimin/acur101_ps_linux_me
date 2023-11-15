
#ifndef TARGETPROCESS_TRACKEREXISTING_CONFCOEF_H_
#define TARGETPROCESS_TRACKEREXISTING_CONFCOEF_H_


#include "../../../utilities/common_struct.h"
//#include "../../tracking/include/tracking.h" // tracking.h中又包含了tracking_common.h，会造成重复包含
//#include "../../tracking/include/tracking_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct sTrackerExisting_probClus {
	int n;
	float prob;
}sTrackerExisting_probClus;

typedef struct sTrackerExisting_probLife {
	int n;
	float prob;
}sTrackerExisting_probLife;

typedef struct sTrackerExisting_probMotion {
	float dev;
	float prob;
}sTrackerExisting_probMotion;

typedef struct sTrackerExistingInst {
	int clusLamda;
	int lifeLamda;
	sTrackerExisting_probClus* probClus_v; // MAXPROBCLUSNUM
	sTrackerExisting_probLife* probLife_v; // MAXPROBLIFENUM
	sTrackerExisting_probMotion* probMotion_v; // MAXPROBMOTIONNUM
	sTrackerExisting_probMotion* probMotion_or_v;
	int id_sorted[MAX_NUM_TRAJS];
	float existingProb_ascent[MAX_NUM_TRAJS];
}sTrackerExistingInst;

/* Initialize the Possion distribution for the probability of cluster and 
   trajectory stability of lifetime.
   Initialize the Gaussian distribution for the trajectory stability of motion.

   Input: detLamda: The parameter of Possion distribution for detection
   Input: lifeLamda: The parameter of Possion distribution for trajectory stability of lifetime
*/
float CalcProb_init(int clusNum, int ObjNumMax);
int trackerExisting_init(sAlgObjData *algObj);
// Calculte the confidence coefficient of current trajectory
void trackerExisting_process(sAlgObjData *algObj);


#ifdef __cplusplus
}
#endif

#endif /* TARGETPROCESS_TRACKEREXISTING_CONFCOEF_H_ */
