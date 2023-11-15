#ifndef OBJECT_STRUCT_H_
#define OBJECT_STRUCT_H_

#include "common_define.h"
#include "embed_api.h"

#define DBSCAN_ERROR_CODE_OFFSET 100
//!  \brief   error code for clusteringDBscan.
typedef enum
{
    DBSCAN_OK = 0,                                                  /**< To be added */
    DBSCAN_ERROR_MEMORY_ALLOC_FAILED = DBSCAN_ERROR_CODE_OFFSET,    /**< To be added */
    DBSCAN_ERROR_NOT_SUPPORTED,                                     /**< To be added */
    DBSCAN_ERROR_CLUSTER_LIMIT_REACHED                              /**< To be added */
} clusteringDBscanErrorCodes;

//! @brief The array of target information
typedef struct measUnitData_int
{
	int16_t range; // In xyzQFormat, m
	int16_t angle; // In xyzQFormat, deg
	int16_t velocity; // In xyzQForat, m/s
	int16_t elevation; // In xyzQFormat, m
}sMeasUnitDataInt;

//! @brief The output information of clustering algorithm for each cluster
typedef struct clusteringTarget
{
	float timestamp; // s
	uint16_t id;
    union {
		sMeasUnitDataInt point;
        int16_t pointArray[4]; // In xyzQFormat
    };
	int16_t		 snr; // In oneQFormat
	int16_t		 rcs; // In oneQFormat
    uint16_t     numPoints; /**< number of points in this cluster */
	int16_t		 xCenter; // In xyzQFormat, m
	int16_t		 yCenter; // In xyzQFormat, m
	int16_t		 zCenter; // In xyzQFormat, m
    uint16_t     xSize;  /**< the clustering size on x direction in oneQFormat, m*/
    uint16_t     ySize;  /**< the clustering size on y direction in oneQFormat, m*/
    uint16_t     zSize;  /**< the clustering size on z direction in oneQFormat, m*/
	uint16_t	 measCovVec[4]; //Measurement Covariance, rangeVar, dopplerVar, sinAzimVar, eleVar, in oneQFormat
	uint8_t		 bUseFlag; // ??
}sClusteringTarget;

//! @brief The cluster list of clustering algorithm
typedef struct clusteringTargetList
{
    uint16_t *indexArray;                       /**< Clustering result index array */
    uint16_t numCluster;                        /**< number of cluster detected */
    sClusteringTarget *clusTargetList;           /**< information for each cluster*/
}sClusteringTargetList;



typedef struct{
	float x1;
	float x2;
	float y1;
	float y2;
}sBox;

typedef enum
{
	MODULE_OK = 0,
	MODULE_UNUSED,
	MODULE_INIT_FAIL = ERROR_CODE_OFFSET,
    MODULE_MEMORY_ALLOC_FAILED,
    MODULE_LIMIT_REACHED,
	MODULE_NEED_RESET,
	MODULE_QUIT,
}MODULE_STATUS;

typedef enum
{
	PD_OK = 0, // It looks Okey for performance degradation
	PD_WARNING_LOWLEVEL = ERROR_CODE_OFFSET,
	PD_WARNING_MIDDLELEVEL,
	PD_WARNING_HIGHLEVEL,
	PD_FAULT,
}PERFORMANCEDETERIORATION_STATUS;

typedef struct selfAlgModuleStatus
{
	MODULE_STATUS measProcModuleStatus; // measurement process module status
	MODULE_STATUS trackingModuleStatus; // tracking module status
	MODULE_STATUS dispatchModuleStatus; // dispatch module status
}sSelfAlgModuleStatus;

typedef struct selfCheck
{
	uint8_t flag_algCheck;
	uint8_t flag_radarHWCheck;
	uint8_t flag_performDeteriorationCheck;
	uint8_t flag_electroEnviroCheck;
}sSelfCheck;

typedef enum
{
	CALIB_OK = 0,
	UNKNOWN_DESC = ERROR_CODE_OFFSET,
	BIAS_OVER_BOUNDARY,
	CALIB_ENVI_NOT_MEET, // the calibration environment is not meet
}SELFCHECK_STATECODE;
typedef enum
{
	TWS_SCAN = 0,
	TAS_SCAN,
	//CFM_SCAN
}sScanType;

typedef enum velUnambiType {
	STEP_SCAN = 0,			// first waveType A beam scan, then waveType B
	NO_UNAMBIGIOUS,			// do not unambigious velocity
	WHOLE_SCAN,				// first waveType A whole scan, then waveType B
}sVelUnambiType;
#endif // OBJECT_STRUCT_H_