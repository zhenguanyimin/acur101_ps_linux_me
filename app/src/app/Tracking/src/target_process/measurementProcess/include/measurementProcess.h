#ifndef MEASUREMENT_PROCESS_H_
#define MEASUREMENT_PROCESS_H_
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "../../../utilities/common_struct.h"
//#include "../../../target_process/dispatch/include/dispatch.h"

/**aa
 * @brief
 *   Measurement point
 *
 * @details
 *  The structure describes measurement point format
 */

#define MEASUREMENT_VECTOR_SIZE (sizeof(sMeasurement_Sph_vector)/sizeof(float))
#define MSIZE_SPH (MEASUREMENT_VECTOR_SIZE)
#define MSIZE_X (2)

#define CLUSTER_POINT_UNKNOWN 250
#define CLUSTER_POINT_VISITED 1
#define CLUSTER_POINT_LONELY 255 //<2^8=256
#define MIN_NUM_POINTS_IN_CLUSTER 2


 //constant parameter
//#define TWS_DOT_COHESION_NUM (30U)
//#define POINTS_IN_CLUSTER_NUM (16U) //(10U)
////#define POINTS_IN_CLUSTER_NUM_TAS (20U)
//#define COCH_BEAM_NUM_TWS (4U)
//#define COCH_BEAM_NUM_TAS (4U)
//#define RANGE_THRESHOLD_WITHIN (8.0f)//(2.0f),2*Rres
//#define VELOCITY_THRESHOLD_WITHIN (2.0f)//(2.0f)
//#define RANGE_THRESHOLD_BETWEEN (8.0f)//(2.0f),2*Rres
//#define VELOCITY_THRESHOLD_BETWEEN (2.0f)//(2.0f)
//#define RANGE_THRESHOLD_TAS (15.0f)//(2.0f)
//#define VELOCITY_THRESHOLD_TAS (10.0f)//(2.0f)

// 2代雷达：2、3、5#
//#define K_AZI_TWS (-0.0516f)//方位比幅测角权值，±2(-0.0987f)，±1(-0.1923f),TR±2(-0.0516f)
//#define DIFFER_SUM_RATIO_THRESHOLD_LIFT_AZI_TWS (-0.6814f)//(0.5f)abs(差和比)阈值，在此区域内的进行测角(0.4455f)，±2(-0.3546f)，±1(-0.0887f),TR±2(-0.6814f)
//#define DIFFER_SUM_RATIO_THRESHOLD_RIGHT_AZI_TWS (0.6723f)//(0.5f)abs(差和比)阈值，在此区域内的进行测角，±2(0.3524f)，±1(0.0929f),TR±2(0.6723f)

#define K_ELE_TWS (-0.1244f)//俯仰比幅测角权值，±5(-0.1671f),±3(-0.1244f)
#define DIFFER_SUM_RATIO_THRESHOLD_LIFT_ELE_TWS (-0.4319f)//(0.5f)abs(差和比)阈值，在此区域内的进行测角(-0.3030f)，±5(-0.3324f)，±3(-0.4319f)
#define DIFFER_SUM_RATIO_THRESHOLD_RIGHT_ELE_TWS (0.4095f)//(0.5f)abs(差和比)阈值，在此区域内的进行测角(0.3326f)，±5(0.3236f)，±3(0.4095f)

//#define K_AZI_TAS (-0.0516f)//方位比幅测角权值，±2(-0.0987f),TR±2(-0.0516f)
//#define DIFFER_SUM_RATIO_THRESHOLD_LIFT_AZI_TAS (-0.6814f)//(0.5f)abs(差和比)阈值，在此区域内的进行测角(0.4455f)，±2(-0.3546f),TR±2(-0.6814f)
//#define DIFFER_SUM_RATIO_THRESHOLD_RIGHT_AZI_TAS (0.6723f)//(0.5f)abs(差和比)阈值，在此区域内的进行测角，±2(0.3524f),TR±2(0.6723f)

#define K_ELE_TAS (-0.2038f)//俯仰比幅测角权值，±4(-0.1794f)，±2(-0.4039f),TR±2( -0.2038f)
#define DIFFER_SUM_RATIO_THRESHOLD_LIFT_ELE_TAS (-0.2173f)//(0.5f)abs(差和比)阈值，在此区域内的进行测角，±4(-0.3373f)，±2(-0.1099f),TR±2(-0.2173f)
#define DIFFER_SUM_RATIO_THRESHOLD_RIGHT_ELE_TAS (0.1253f)//(0.5f)abs(差和比)阈值，在此区域内的进行测角，±4(0.3438f)，±2(0.0628f),TR±2(0.1253f)

// 3代雷达：4#
//#define K_AZI_TWS_3 (-0.0471f)//方位比幅测角权值，TR±2(-0.0471f)
//#define DIFFER_SUM_RATIO_THRESHOLD_LIFT_AZI_TWS_3 (-0.7110f)//(0.5f)abs(差和比)阈值，在此区域内的进行测角，TR±2(-0.7110f)
//#define DIFFER_SUM_RATIO_THRESHOLD_RIGHT_AZI_TWS_3 (0.7225f)//(0.5f)abs(差和比)阈值，在此区域内的进行测角，TR±2(0.7225f)

#define K_ELE_TWS_3 (-0.1136f)//俯仰比幅测角权值，TR±3(-0.1198f)
#define DIFFER_SUM_RATIO_THRESHOLD_LIFT_ELE_TWS_3 (-0.4551f)//(0.5f)abs(差和比)阈值，在此区域内的进行测角，TR±3(-0.4750f)
#define DIFFER_SUM_RATIO_THRESHOLD_RIGHT_ELE_TWS_3 (0.4663f)//(0.5f)abs(差和比)阈值，在此区域内的进行测角，TR±3(0.3993f)

#define K_ELE_TWS_4 (-0.1198f)//俯仰比幅测角权值，TR±3(-0.1198f),4thGenRadar,1010update
#define DIFFER_SUM_RATIO_THRESHOLD_LIFT_ELE_TWS_4 (-0.4750f)//(0.5f)abs(差和比)阈值，在此区域内的进行测角，TR±3(-0.4750f)
#define DIFFER_SUM_RATIO_THRESHOLD_RIGHT_ELE_TWS_4 (0.3993f)//(0.5f)abs(差和比)阈值，在此区域内的进行测角，TR±3(0.3993f)

//#define K_AZI_TAS_3 (-0.0471f)//方位比幅测角权值，TR±2(-0.0471f)
//#define DIFFER_SUM_RATIO_THRESHOLD_LIFT_AZI_TAS_3 (-0.7110f)//(0.5f)abs(差和比)阈值，在此区域内的进行测角，TR±2(-0.7110f)
//#define DIFFER_SUM_RATIO_THRESHOLD_RIGHT_AZI_TAS_3 (0.7225f)//(0.5f)abs(差和比)阈值，在此区域内的进行测角，TR±2(0.7225f)

#define K_ELE_TAS_3 (-0.1615f)//俯仰比幅测角权值，TR±2(-0.1615f)
#define DIFFER_SUM_RATIO_THRESHOLD_LIFT_ELE_TAS_3 (-0.2115f)//(0.5f)abs(差和比)阈值，在此区域内的进行测角，TR±2(-0.2115f)
#define DIFFER_SUM_RATIO_THRESHOLD_RIGHT_ELE_TAS_3 (0.2206f)//(0.5f)abs(差和比)阈值，在此区域内的进行测角，TR±2(0.2206f)

#define K_ELE_TAS_4 (-0.1734f)//俯仰比幅测角权值，TR±2(-0.1734f),4thGenRadar,1010update
#define DIFFER_SUM_RATIO_THRESHOLD_LIFT_ELE_TAS_4 (-0.2305f)//(0.5f)abs(差和比)阈值，在此区域内的进行测角，TR±2(-0.2305f)
#define DIFFER_SUM_RATIO_THRESHOLD_RIGHT_ELE_TAS_4 (0.1722f)//(0.5f)abs(差和比)阈值，在此区域内的进行测角，TR±2(0.1722f)


#define CLUTTER_PRO_CASE (10U)
#define MIR_FREQ_BIN_DIFF (3U) //镜频抑制中心对称2点的bin格差
#define MIR_FREQ_MAG_RATIO (3U) //镜频抑制中心对称2点的幅值比
#define NUM_DOPPLER_BIN (32U) //Doppler维bin格总数，chirp数

/** characteristic of detections,0:real atrget,1:clutter,2:mirror frequency,3:reflection,4:power supply,5:others  */
#define REAL_POINT (0U)
#define CLUTTER_POINT (1U)
#define MIRROR_FREQUENCE_POINT (2U)
#define REFLECTION_POINT (3U)
#define POWER_SUPPLY_POINT (4U)
#define OTHER_POINT (5U)

#define AZIMUTH_GAP_BEAM_2 575 //((int)(8*DEG2RAD*ONE13FORMAT_SCALE) + 3)=1147,for cohesion judgment
#define AZIMUTH_GAP_BEAM_1 575 //((int)(4*DEG2RAD*ONE13FORMAT_SCALE) + 3)=575,((int)(2*DEG2RAD*ONE13FORMAT_SCALE) + 3)=289,for azimuth calculation
#define ELEVATION_GAP_BEAM 1432 //((int)(10*DEG2RAD*ONE13FORMAT_SCALE) + 3),for elevation calculation

#define RANGE_THRESHOLD 210.f //for cohesion judgment
#define AZIMUTH_GAP_BEAM_3 1718 //((int)(12*DEG2RAD*ONE13FORMAT_SCALE) + 3)=1718,for cohesion judgment


//! @brief The detection list of tracking algorithm
typedef struct {
	uint8_t clusterID[MAX_NUM_DETECTS];
	uint8_t dots_store_Num;//当前帧与存储区目标簇可聚类的目标总数
	uint8_t dots_dots_Num;//当前帧点迹自身可聚类的目标总数
	uint8_t lonelyNum;//当前帧孤点目标总数
	uint8_t dots_tracker_Num;//当前帧点迹与TAS跟踪目标可聚类的目标总数——TAS模式

	uint8_t dots_store_NumIn;//当前帧可聚类的目标--放入存储区的总数
	uint8_t dots_dots_NumIn;//当前帧点迹自身可聚类的目标--放入存储区的总数
	uint8_t lonelyNumIn;//当前帧孤点目标--放入存储区的总数
}sClusterDetect;

 /**
  * @brief
  *  Dots cohesion vector
  * @details
  *  The structure defines dots cohesion vector format
  */
typedef struct{
	uint16_t dotId;	
	float range;/**  @brief   Range, m */	
	float doppler;/**  @brief   Radial velocity, m/s */
	int16_t dopplerChn;	
	float azimuthRad; //rad
	float elevationRad; // rad, the most likely azimuth based on azimuth ambiguity
	int16_t aziBeamRad;	/**  @brief   Azimuth direction beam, rad * 2^13 */	
	int16_t eleBeamRad;	/**  @brief   Elevation direction beam, rad * 2^13 */		
	float mag;/**  @brief   Radial amplitude, dB */	
	sMeasurement_enum_wavetype waveType;
	uint8_t dotCharacteristic; /**  @brief  characteristic of detections,0:real atrget,1:clutter,2:mirror frequency,3:reflection,4:power supply,5:others  */
} sDots_Cohesion_vector;

typedef struct{
	uint16_t tasTargetId;
	uint8_t	stateType;	
	float range;/**  @brief   Range, m */	
	float doppler;/**  @brief   Radial velocity, m/s */	
	float aziRad;/**  @brief   Azimuth, rad */	
	float eleRad;/**  @brief   Elevation, rad*/	
	float mag;/**  @brief   Radial amplitude, dB */
	sMeasurement_enum_wavetype waveType;
} sTrack_Tas_vector;

typedef struct sMeasInput {
	uint32_t frameID;
	uint8_t detNum; //number of clustering targets 
	sScanType scanType;	// scantype,0-TWS,1-TAS
	float timestamp;  //时间戳, in s
	int16_t curAziBeamRad;//the azimuth of current beam ,rad * 2^13
	uint8_t wholeSpaceScanCycleFinalBeamFlag;	// flag of whole space scan cycle's final beam in tws or tas mode, 1-yes; 0-no
	uint8_t tasOrInitTrackFinalBeamFlag;	// flag of each tas or init track's final beam in tas mode, 1-yes; 0-no
	uint8_t tasAndInitTrackFinishBeamFlag;	// flag of tas and init track's finish beam in tas mode, 1-yes; 0-no
	//sMeasurement_enum_wavetype waveType;
	sDots_Cohesion_vector dotInput[MAX_NUM_DETECTS]; //input of detection module
	sTrack_Tas_vector tasTrackStr[1];//current TAS tracker
}sMeasInput;

typedef struct{	
	float range;/**  @brief   Range, m */	
	float azimuthRad;/**  @brief   Azimuth, rad */	
	float pitchRad;/**  @brief   Elevation, rad */	
	float doppler;/**  @brief   Radial velocity, m/s */
} sMeasurement_Sph_vector;

typedef struct{
	uint16_t detId;
	union {
		/**  @brief   Measurement vector */
		sMeasurement_Sph_vector vector;
		float array[MSIZE_SPH];
	};
	float x;
	float y;
	float z;
	/**  @brief   Range detection SNR, linear */
	float mag;
	float snr;
	float rcs;
	float sinAzim;
	float cosAzim;
	float unambigVel;
	uint16_t classification;
	//int16_t aziBeamSin;			//*32767, cache the result within beam, sin value of azimuth direction beam, uint = 1/32767
	//int16_t eleBeamSin;			//*32767, cache the result within beam, sin value of elevation direction beam, uint = 1/32767
	uint8_t flag_moving;
	sMeasurement_enum_wavetype waveType;
	uint8_t unambigVelDone; //Flag of whether velocity unambiguity is done:0/1
} sMeasurement_SphPoint;


// save RCS calcuating parameter  by LYW at 20230513
typedef struct {	
	float mag;
	float range;
	float azimuthRad;
	float elevationRad;
} sMeasurement_SphPoint_Mid;

typedef struct RCS {
	float Mag0;
	float RCS0;
	int  R0;
} Cor_Reflector_RCS;
//float Gain_IF[] = { 0, 0, 0, 26.9356, 26.1512, 25.3966, 24.6719, 23.9294, 23.2169, 22.6769, 22.3988, 22.2933, 22.2357, 22.1904, 22.1931, 22.2257, 22.2890, 22.3823, 22.4858, 22.6033, 22.7309, 22.8370, 22.9019, 22.9453, 22.9941, 23.0608, 23.1329, 23.2001, 23.2566, 23.3082, 23.3614, 23.4225, 23.4853, 23.5367, 23.5692, 23.5905, 23.6103, 23.6341, 23.6565, 23.6714, 23.6697, 23.6605, 23.6649, 23.6938, 23.7363, 23.7806, 23.8331, 23.8873, 23.9123, 23.8785, 23.8153, 23.7801, 23.8033, 23.8544, 23.8985, 23.9214, 23.9372, 23.9535, 23.9743, 23.9956, 24.0128, 24.0171, 24.0173, 24.0341, 24.0888, 24.1602, 24.2051, 24.1964, 24.1613, 24.1376, 24.1420, 24.1580, 24.1738, 24.1832, 24.1926, 24.2090, 24.2406, 24.2793, 24.3086, 24.3210, 24.3239, 24.3236, 24.3176, 24.3083, 24.3095, 24.3268, 24.3545, 24.3888, 24.4340, 24.4858, 24.5269, 24.5501, 24.5627, 24.5695, 24.5649, 24.5546, 24.5604, 24.5948, 24.6454, 24.6967, 24.7485, 24.8009, 24.8386, 24.8526, 24.8517, 24.8473, 24.8363, 24.8217, 24.8238, 24.8547, 24.9023, 24.9505, 24.9962, 25.0424, 25.0817, 25.1066, 25.1247, 25.1507, 25.1947, 25.2466, 25.2906, 25.3235, 25.3485, 25.3599, 25.3435, 25.3135, 25.3069, 25.3470, 25.4106, 25.4643, 25.4917, 25.5092, 25.5326, 25.5720, 25.6172, 25.6542, 25.6742, 25.6859, 25.7012, 25.7236, 25.7495, 25.7802, 25.8191, 25.8627, 25.9021, 25.9363, 25.9662, 25.9862, 25.9883, 25.9804, 25.9806, 25.9943, 26.0162, 26.0477, 26.0948, 26.1515, 26.2015, 26.2357, 26.2631, 26.2949, 26.3391, 26.3877, 26.4270, 26.4465, 26.4568, 26.4753, 26.5112, 26.5555, 26.5975, 26.6324, 26.6651, 26.7005, 26.7437, 26.7896, 26.8281, 26.8535, 26.8715, 26.8894, 26.9071, 26.9246, 26.9499, 26.9878, 27.0336, 27.0803, 27.1293, 27.1793, 27.2202, 27.2427, 27.2561, 27.2784, 27.3222, 27.3749, 27.4164, 27.4319, 27.4362, 27.4536, 27.4987, 27.5568, 27.6088, 27.6454, 27.6759, 27.7093, 27.7496, 27.7928, 27.8366, 27.8811, 27.9263, 27.9689, 28.0096, 28.0477, 28.0777, 28.0934, 28.1010, 28.1142, 28.1412, 28.1738, 28.2001, 28.2138, 28.2213, 28.2303, 28.2411, 28.2534, 28.2738, 28.3062, 28.3467, 28.3898, 28.4342, 28.4812, 28.5295, 28.5819, 28.6356, 28.6805, 28.7116, 28.7340, 28.7527, 28.7656, 28.7748, 28.7918, 28.8221, 28.8602, 28.9017, 28.9484, 28.9985, 29.0419, 29.0712, 29.0940, 29.1229, 29.1673, 29.2180, 29.2601, 29.2884, 29.3081, 29.3202, 29.3177, 29.3075, 29.3118, 29.3405, 29.3836, 29.4334, 29.4978, 29.5689, 29.6153, 29.6143, 29.5886, 29.5757, 29.5886, 29.6142, 29.6503, 29.7024, 29.7650, 29.8194, 29.8587, 29.8899, 29.9156, 29.9355, 29.9498, 29.9620, 29.9711, 29.9779, 29.9890, 30.0037, 30.0226, 30.0539, 30.1073, 30.1731, 30.2312, 30.2762, 30.3135, 30.3393, 30.3460, 30.3411, 30.3437, 30.3603, 30.3843, 30.4149, 30.4548, 30.5013, 30.5458, 30.5873, 30.6268, 30.6584, 30.6776, 30.6889, 30.6999, 30.7120, 30.7239, 30.7390, 30.7586, 30.7814, 30.8069, 30.8383, 30.8723, 30.8988, 30.9091, 30.9119, 30.9228, 30.9512, 30.9876, 31.0193, 31.0428, 31.0615, 31.0729, 31.0687, 31.0570, 31.0601, 31.0922, 31.1391, 31.1795, 31.2011, 31.2163, 31.2412, 31.2878, 31.3441, 31.3909, 31.4226, 31.4448, 31.4551, 31.4461, 31.4253, 31.4121, 31.4104, 31.4163, 31.4376, 31.4850, 31.5477, 31.6010, 31.6301, 31.6499, 31.6796, 31.7356, 31.8015, 31.8477, 31.8549, 31.8423, 31.8381, 31.8491, 31.8684, 31.9036, 31.9747, 32.0615, 32.1116, 32.0912, 32.0340, 31.9882, 31.9695, 31.9622, 31.9677, 31.9897, 32.0245, 32.0617, 32.0960, 32.1327, 32.1775, 32.2416, 32.3137, 32.3657, 32.3857, 32.3855, 32.3733, 32.3403, 32.2953, 32.2723, 32.2925, 32.3348, 32.3693, 32.3818, 32.3865, 32.3968, 32.4186, 32.4460, 32.4739, 32.4991, 32.5248, 32.5550, 32.5943, 32.6382, 32.6773, 32.7084, 32.7346, 32.7567, 32.7785, 32.7961, 32.7988, 32.7741, 32.7344, 32.7069, 32.7028, 32.7107, 32.7237, 32.7377, 32.7568, 32.7855, 32.8311, 32.8865, 32.9343, 32.9678, 32.9938, 33.0153, 33.0310, 33.0421, 33.0558, 33.0780, 33.1029, 33.1198, 33.1215, 33.1152, 33.1127, 33.1161, 33.1234, 33.1393, 33.1675, 33.2044, 33.2446, 33.2896, 33.3378, 33.3787, 33.4116, 33.4371, 33.4461, 33.4244, 33.3862, 33.3650, 33.3765, 33.4050, 33.4361, 33.4674, 33.5014, 33.5311, 33.5518, 33.5682, 33.5874, 33.6159, 33.6472, 33.6689, 33.6698, 33.6612, 33.6649, 33.6960, 33.7394, 33.7723, 33.7853, 33.7878, 33.7852, 33.7699, 33.7496, 33.7522, 33.8037, 33.8781, 33.9259, 33.9168, 33.8810, 33.8597, 33.8701, 33.8951, 33.9240, 33.9571, 33.9942, 34.0241, 34.0409, 34.0505, 34.0596, 34.0711, 34.0821, 34.0906, 34.0945, 34.0960, 34.0990, 34.1053, 34.1131, 34.1223, 34.1308, 34.1407, 34.1571, 34.1868, 34.2230, 34.2512, 34.2604, 34.2616, 34.2734, 34.3130, 34.3633, 34.3914, 34.3721, 34.3305, 34.3097, 34.3356, 34.3822, 34.4144, 34.4137, 34.3987, 34.3897, 34.3922, 34.4006, 34.4193, 34.4562, 34.5033, 34.5411, 34.5615, 34.5725, 34.5788, 34.5809, 34.5783, 34.5738, 34.5695, 34.5632, 34.5516, 34.5227, 34.4883, 34.4806, 34.5213, 34.5886, 34.6486, 34.6909, 34.7259, 34.7510, 34.7592, 34.7575, 34.7649, 34.7939, 34.8320, 34.8604, 34.8714, 34.8728, 34.8697, 34.8622, 34.8503, 34.8386, 34.8257, 34.8131, 34.8098, 34.8198, 34.8391, 34.8652, 34.9030, 34.9475, 34.9814, 34.9940, 34.9960, 35.0012, 35.0170, 35.0360, 35.0510, 35.0560, 35.0571, 35.0643, 35.0845, 35.1109, 35.1334, 35.1459, 35.1546, 35.1674, 35.1914, 35.2194, 35.2380, 35.2350, 35.2226, 35.2234, 35.2521, 35.2940, 35.3270, 35.3397, 35.3435, 35.3506, 35.3676, 35.3879, 35.4042, 35.4123, 35.4164, 35.4215, 35.4315, 35.4425, 35.4472, 35.4394, 35.4253, 35.4176, 35.4210, 35.4308, 35.4448, 35.4622, 35.4840, 35.5106, 35.5453, 35.5848, 35.6211, 35.6545, 35.6846, 35.7018, 35.6942, 35.6736, 35.6669, 35.6899, 35.7268, 35.7556, 35.7697, 35.7758, 35.7726, 35.7518, 35.7217, 35.7060, 35.7189, 35.7462, 35.7688, 35.7771, 35.7806, 35.7892, 35.8108, 35.8374, 35.8548, 35.8546, 35.8452, 35.8379, 35.8320, 35.8283, 35.8406, 35.8866, 35.9486, 35.9866, 35.9755, 35.9404, 35.9169, 35.9183, 35.9314, 35.9510, 35.9794, 36.0144, 36.0445, 36.0681, 36.0868, 36.0931, 36.0788, 36.0521, 36.0302, 36.0152, 36.0050, 36.0100, 36.0421, 36.0895, 36.1268, 36.1396, 36.1423, 36.1535, 36.1856, 36.2262, 36.2566, 36.2673, 36.2679, 36.2678, 36.2720, 36.2754, 36.2727, 36.2565, 36.2341, 36.2227, 36.2295, 36.2473, 36.2718, 36.3049, 36.3448, 36.3811, 36.4132, 36.4418, 36.4590, 36.4516, 36.4327, 36.4338, 36.4766, 36.5393, 36.5880, 36.6125, 36.6231, 36.6161, 36.5842, 36.5347, 36.4865, 36.4385, 36.3918, 36.3685, 36.3817, 36.4184, 36.4615, 36.5084, 36.5617, 36.6126, 36.6609, 36.7067, 36.7413, 36.7590, 36.7654, 36.7690, 36.7732, 36.7746, 36.7716, 36.7577, 36.7395, 36.7346, 36.7601, 36.7991, 36.8182, 36.7938, 36.7496, 36.7239, 36.7381, 36.7708, 36.7968, 36.8071, 36.8107, 36.8091, 36.7985, 36.7827, 36.7744, 36.7782, 36.7896, 36.8074, 36.8325, 36.8641, 36.8990, 36.9423, 36.9890, 37.0213, 37.0280, 37.0203, 37.0143, 37.0138, 37.0152, 37.0236, 37.0437, 37.0710, 37.0967, 37.1206, 37.1430, 37.1563, 37.1553, 37.1452, 37.1342, 37.1201, 37.1052, 37.1048, 37.1288, 37.1673, 37.2056, 37.2445, 37.2832, 37.3055, 37.3006, 37.2791, 37.2568, 37.2387, 37.2196, 37.1996};




void RCS_Estimation(sMeasurement_SphPoint_Mid* detect, Cor_Reflector_RCS* Cor_RCS, sMeasurement_SphPoint* decodedBits, float* Gain_IF);






typedef struct sDotCohPointsIdInfo {
	uint16_t detectionNum;		// detection numbers in cluster
	uint16_t detectionId[POINTS_IN_CLUSTER_NUM];   // detection ID in cluster
	uint32_t frameId[POINTS_IN_CLUSTER_NUM];
}sDotCohPointsIdInfo;

typedef struct sCalcAngleInfo {
	uint8_t ifCalcAzi; // flag of whether the azimuth is measured
	uint8_t ifCalcEle; // flag of whether the elevation is measured
	float MagBeamList[COCH_BEAM_NUM_TWS];   // mag for calculate azimuth
	float azimuthRadList[COCH_BEAM_NUM_TWS / 2];// azimuth of the left and right beam for calculate azimuth
	float eleBeamRadList[COCH_BEAM_NUM_TWS / 2];// elevation of the up and down beam for calculate elevation
}sCalcAngleInfo;

typedef struct sMeasConfigParam {
	float rCohGatingTws;		// range cohesion gating of tws
	float dCohGatingTws;		// doppler cohesion gating of tws
	float rCohGatingTas;		// range cohesion gating of tas
	float dCohGatingTas;		// doppler cohesion gating of tas
	float rDetTrackTas;			// range cohesion gating of tas tracker and detection
	float dDetTrackTas;			// doppler cohesion gating of tas tracker and detection
}sMeasConfigParam;

typedef struct sMeasOutput {
	uint8_t num; //number of clustering targets 
	sScanType scanType;				// scantype,0-TWS,1-TAS
	uint16_t tasTargetId;
	float timestamp;  //时间戳, in s
	uint8_t condenceDone;  //聚类是否完成：0未完成，1完成
	int16_t aziBeamSin;			//*32767, cache the result within beam, sin value of azimuth direction beam, uint = 1/32767
	int16_t eleBeamSin;			//*32767, cache the result within beam, sin value of elevation direction beam, uint = 1/32767
	sMeasurement_SphPoint measurement[TWS_DOT_COHESION_NUM];			// output of measurement process module
	sDotCohPointsIdInfo pointsIdInfo[TWS_DOT_COHESION_NUM];
	sCalcAngleInfo calcAngleInfo[TWS_DOT_COHESION_NUM];
}sMeasOutput;

typedef struct {
	uint8_t clusterId;
	uint8_t cohesionFinish;
	uint8_t dotsNum;
	uint32_t frameId[POINTS_IN_CLUSTER_NUM];
	float timestamp;  //时间戳, in s	
	int16_t leftBoundaryAziRad;/**  @brief  left boundary of the azimuth beam,update in each frame, rad * 2^13 */
	uint8_t unambigDone;
	float unambigVel;
	sDots_Cohesion_vector dotCluster[POINTS_IN_CLUSTER_NUM];
	//sDots_Cohesion_vector maxMagPointStr[3];//Save the 2 sets of dots structer with the largest amplitude
}sTwsDotCohesion;

typedef struct {
	uint8_t clusterId;
	uint8_t cohesionFinish;
	uint8_t dotsNum;
	uint32_t frameId[POINTS_IN_CLUSTER_NUM];
	float timestamp;  //时间戳, in s
	uint8_t unambigDone;
	float unambigVel;
	sDots_Cohesion_vector dotCluster[POINTS_IN_CLUSTER_NUM];
	//sDots_Cohesion_vector maxMagPointStr[3];//Save the 2 sets of dots structer with the largest amplitude
}sTasDotCohesion;

typedef struct {
	uint8_t dotCoheNum;
	sTwsDotCohesion TwsDotCohesion[TWS_DOT_COHESION_NUM];//Cumulative detected clusterable target cluster structure , TWS mode
}sTwsDotCoheInst;

typedef struct {
	sMeasInput* measInput;//detection targets output to dots cohesion
	sMeasConfigParam measConfigParam;
	sMeasOutput* measOutput;//cohesived targets output to tracking
	sTwsDotCoheInst* TwsDotCoheStr;//Cumulative detected clusterable target cluster structure , TWS mode
	sTasDotCohesion TasDotCohesion[1];//Cumulative detected clusterable target cluster structure, only current TAS tracker , TAS mode
	sDotsCohesionFrame* dotsCoheOutput;//cohesived targets report
}sMeasProcessInst;







//========================== old =============================//
typedef struct sDetTrackFrame {
	int16_t aziBeamSin[COCH_BEAM_NUM_TAS];			//*32767, cache the result within beam, sin value of azimuth direction beam, uint = 1/32767
	int16_t eleBeamSin[COCH_BEAM_NUM_TAS];			//*32767, cache the result within beam, sin value of elevation direction beam, uint = 1/32767
	sDetectFrame detectFrame;
}sDetTrackFrame;

typedef struct sMultiBeamMeas {
	uint8_t numTws; //number of clustering targets in cache TWS
	uint8_t numTas; //number of clustering targets in cache TAS
	sMeasOutput multiBeam_tws[COCH_BEAM_NUM_TWS];
	sDetTrackFrame multiBeam_tas[COCH_BEAM_NUM_TAS];
}sMultiBeamMeas;


//! @brief The detection list of tracking algorithm
typedef struct {
	uint8_t clusterID[MAX_NUM_DETECTS];
	uint8_t clusterNum;
	uint8_t lonelyNum;
	uint8_t clusterMeasNum;
	sMeasurement_SphPoint measurement[MAX_NUM_DETECTS];			// output of measurement process module
}sClusterMeasure;
//========================== old =============================//


//========================== new =============================//
void measurementConfigLoad(sMeasConfigParam* measConfigParam, sDebugAndCtrlParam* ctrlInfo);

void measurement_process(sAlgObjData* algObj);

//** 1.loading detection data  **//
void loadDetectionData(sDetectFrame* detInst, sDispatchOutput* dispatchOutput, sTrackingFrame* trajInfoOutput, 
	sMeasInput* measInput, void *handle);


//** 2.clutter process**//
void clutterProcess(sMeasInput* measInput);
void ifMirrorFreqPoint(uint8_t dotNum, sDots_Cohesion_vector* dotInput, uint8_t* isMirrorFreqPoint);
uint8_t isReflectPoint(float range, float doppler, float mag, uint8_t n_det, uint8_t dotNum, sDots_Cohesion_vector* dotInput);
uint8_t isPowerSupplyPoint(float range, float doppler, float mag, uint8_t n_det, uint8_t dotNum, sDots_Cohesion_vector* dotInput);
uint8_t isInPowerSupplyClutterRange(float range);
uint8_t isInPowerSupplyClutterRangeAndVelocity(float range, float doppler);

//** 3.dots clustering and judge whether target scan is finished,just TWS ,TODO TAS**//
void dotsClustering(sMeasInput* measInput, sMeasProcessInst* measInst, sClusterDetect* clusterDet);
void dotsClustering_tws(sMeasInput* measInput, sMeasConfigParam* measConfigParam, sClusterDetect* clusterDet, sTwsDotCoheInst* TwsDotCoheStr);
void clusteringWithinBeam_tws(sMeasInput* measInput, sMeasConfigParam* measConfigParam, sClusterDetect* clusterDet, sTwsDotCoheInst* TwsDotCoheStr);
void dots_dots_clustering_tws(sMeasInput* measInput, sMeasConfigParam* measConfigParam, sClusterDetect* clusterDet, sTwsDotCoheInst* TwsDotCoheStr, uint8_t* visited_in, uint8_t* dots_dots_ID);
uint8_t dot_dot_DBscan_findNeighbors(sDots_Cohesion_vector* dotInput, sMeasConfigParam* measConfigParam, uint8_t n, uint8_t* neigh, uint8_t numPoints, uint8_t* visited_in, uint8_t* newCount);
void clusteringFunInBeam_tws(sClusterDetect* clusterDet, sMeasInput* measInput, uint8_t detNum, sTwsDotCoheInst* TwsDotCoheStr);
void clusterTargetFun_1st_tws(uint8_t clustNumEnd, uint8_t detNum, uint8_t* clustID, sMeasInput* measInput, sTwsDotCoheInst* TwsDotCoheStr);
void lonelyTargetFun_tws(uint8_t clustIdEnd, uint8_t lonelyNumEnd, uint8_t detNum, uint8_t* clustID, sMeasInput* measInput, sTwsDotCoheInst* TwsDotCoheStr);
void clusteringBetweenBeam_tws(sMeasInput* measInput, sMeasConfigParam* measConfigParam, sClusterDetect* clusterDet, sTwsDotCoheInst* TwsDotCoheStr);
void dots_store_clustering_tws(sMeasInput* measInput, sMeasConfigParam* measConfigParam, sClusterDetect* clusterDet, sTwsDotCoheInst* TwsDotCoheStr, uint8_t* visited_between, uint8_t* dots_store_ID);
uint8_t dot_store_DBscan_findNeighbors(sMeasInput* measInput, sMeasConfigParam* measConfigParam, sDots_Cohesion_vector* dotCluster, uint8_t n_dot, uint8_t* unambigDone, float* unambigVel, uint8_t* neigh, uint8_t* visited, uint8_t* newCount);
void clusteringFunBetweenBeam_tws(sClusterDetect* clusterDet, sMeasInput* measInput, uint8_t detNum, sTwsDotCoheInst* TwsDotCoheStr);
void clusterTargetFun_store_tws(uint8_t clustNumEnd, uint8_t detNum, uint8_t* clustID, sMeasInput* measInput, sTwsDotCoheInst* TwsDotCoheStr);

void dotsClustering_tas(sMeasInput* measInput, sMeasConfigParam* measConfigParam, sClusterDetect* clusterDet, sTasDotCohesion* TasDotCohesion);
void dot_tracker_clustering_tas(sMeasInput* measInput, sMeasConfigParam* measConfigParam, sClusterDetect* clusterDet, sTasDotCohesion* TasDotCohesion, uint8_t* visited, uint8_t* dots_tracker_ID);
uint8_t dot_tracker_DBscan_findNeighbors(sMeasInput* measInput, sMeasConfigParam* measConfigParam, sDots_Cohesion_vector* dotCluster, uint8_t* neigh, uint8_t* visited, uint8_t* newCount);
void dots_store_clustering_tas(sMeasInput* measInput, sMeasConfigParam* measConfigParam, sClusterDetect* clusterDet, sTasDotCohesion* TasDotCohesion, uint8_t* visited, uint8_t* dots_store_ID);
void clusteringFunBetweenBeam_tas(sClusterDetect* clusterDet, sMeasInput* measInput, uint8_t detNum, sTasDotCohesion* TasDotCohesion);


//** 4.dots cohesion and output **//
//void dotsCohesionAndOutput(sMeasInput* measInput, sMeasProcessInst* measInst);
//void dotsCohesionAndOutput_tws(sMeasInput* measInput, sTwsDotCoheInst* TwsDotCoheStr, sMeasOutput* measOutput);
void dotsCohesionAndOutput(sMeasInput* measInput, sMeasProcessInst* measInst, sMeasConfigParam* measConfigParam);
void dotsCohesionAndOutput_tws(sMeasInput* measInput, sTwsDotCoheInst* TwsDotCoheStr, sMeasOutput* measOutput, sMeasConfigParam* measConfigParam);
void dotsCohesionFun_tws(sMeasInput* measInput, uint8_t i_Cohe, sTwsDotCohesion* TwsDotCohesion, uint8_t id_CoheOut, sMeasOutput* measOutput);

void dotsCohesionAndOutput_tas(sMeasInput* measInput, sTasDotCohesion* TasDotCohesion, sMeasOutput* measOutput);
void dotsCohesionFun_tas(sMeasInput* measInput, sTasDotCohesion* TasDotCohesion, sMeasOutput* measOutput);
//void dotsCohesionFun_tas(sMeasInput* measInput, sTasDotCohesion* TasDotCohesion, sMeasOutput* measOutput);


//** 5.Target Cluster Store Update **//
void tarClusterStoreUpdate(sMeasInput* measInput, sMeasProcessInst* measInst, sClusterDetect* clusterDet);
void tarClusterStoreUpdate_tws(sMeasInput* measInput, sClusterDetect* clusterDet, sTwsDotCoheInst* TwsDotCoheStr);
void clusterTargetFun_new_tws(uint8_t clustIdEnd, uint8_t storeNewNum, uint8_t dotClustId, uint8_t detNum, uint8_t* clustID, sMeasInput* measInput, sTwsDotCoheInst* TwsDotCoheStr);
void tarClusterStoreUpdate_tas(sMeasInput* measInput, sClusterDetect* clusterDet, sTasDotCohesion* TasDotCohesion);


//** 6.report the dots cohesion **//
void dotsCohesion_unitReport(sMeasProcessInst* measInst);


// ==================== old ===============================//
//void tas_measurement_process(sAlgObjData* algObj);
//void tws_measurement_process(sAlgObjData* algObj);
//
//void clutterProcessing(sDetectFrame* detInst, sDetectFrame* detClutProOut);
//uint8_t isReflectPoint(float range, float doppler, float mag, uint8_t n_det, sDetectPoint* detPointStr, uint8_t nNum);
//uint8_t isPowerSupplyPoint(float range, float doppler, float mag, uint8_t n_det, sDetectPoint* detPointStr, uint8_t nNum);
//void ifMirrorFreqPoint(sDetectPoint* detPointStr, uint8_t nNum, uint8_t* isMirrorFreqPoint);
//void clusteringWithinBeam_tws(sDetectFrame* detClutProOut, sMultiBeamMeas* cacheBeam, sMeasOutput* clusterObjIn);
////void clusteringBetweenBeam_tws(uint8_t calcAngleMode, sMeasOutput* cacheMeasBeam, sMeasOutput* clusterObjIn, sMeasOutput* measObjOut);
//void clusteringBetweenBeam_tws(uint8_t calcAngleMode, sMeasOutput* cacheMeasBeam, sMeasOutput* measObjOut);
//void detChoseWithinBeam_tas(sDetectFrame* detClutProOut, sTrackingTarget* tasTrackStr, sMultiBeamMeas* cacheBeam, uint8_t tasBeamTotal);
////void detClusteringBetweenBeam_tas(sDetTrackFrame* TrackDetStr, sTrackingTarget* tasTrackStr, uint8_t* cacheBeamTarNum, sMeasOutput* measObjOut);
//void detClusteringBetweenBeam_tas(sDetTrackFrame* TrackDetStr, sTrackingTarget* tasTrackStr, uint8_t* cacheBeamTarNum, uint8_t tasBeamTotal, sMeasOutput* measObjOut);
//void clusteringFunInBeam(sClusterDetect* clusterDet, sDetectFrame* detClutProOut, sMeasOutput* clusterObjIn);
//void clusteringFunBetweenBeams(uint8_t N_curObj, uint8_t N_cacheObj, uint8_t calcAngleMode, sClusterMeasure* clusterMeas, sMeasOutput* measObjOut);
//void cluster_DBscan(sClusterMeasure* clusterMeas, uint8_t N);
//uint8_t detTrack_DBscan(sDetectFrame* detClutProOut, sTrackingTarget* tasTrackStr, uint8_t* clusterID);
//uint8_t detect_DBscan_findNeighbors(sDetectPoint* detPointStr, uint8_t n, uint8_t* neigh, uint8_t numPoints, uint8_t* visited_in, uint8_t* newCount);
//uint8_t det_store_DBscan_findNeighbors(sMeasurement_SphPoint* measPointStr, uint8_t n, uint8_t* neigh, uint8_t numPoints, uint8_t* visited_between, uint8_t* newCount);
//uint8_t dot_tracker_DBscan_findNeighbors(sDetectPoint* detPointStr, sTrackingTarget* tasTrackStr, uint8_t* neigh, uint8_t numPoints, \
//	uint8_t* visited_in, uint8_t* newCount, sMeasurement_enum_wavetype waveType);




#endif // !MEASUREMENT_PROCESS_H_
