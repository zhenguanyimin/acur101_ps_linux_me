
#ifndef COMMON_DEFINE_H_
#define COMMON_DEFINE_H_

/******* Tracking algorithm version **********
Example: TRK_01.110110
           |--------- tracking algorithm
			  |--------- main version number
				|------- version number of measurement process
				 |------- version number of tracking association, tracking filter
				  |------- version number of tracking management, tracking start ...
				   |------- version number of tracking post processer (including classification, merge ...)
					|------- version number of dispatch process 
					 |------- version number of SIL interface ... 
*/
#define DEFAULT_SCAN_TYPE 1
#define TRACK_ALG_VERSION "TRK_1.699020"
#define TRACK_VERSION_STR_LEN (14)
#define false 0
#define true 1

#ifdef _WINDOWS
#define ENABLE_OUTRESULT
#ifdef ENABLE_OUTRESULT
//#define ENABLE_GETCLASSGT
#ifndef ENABLE_GETCLASSGT
//#define ENABLE_OUTMAG
#endif
#endif
#ifndef _DEBUG1
//#define VISUALIZATION_GTRACK
#define VISUAL_RECORD_TRACK
#endif
#endif

#ifdef _WINDOWS
//#define DEBUG_LOG
//#define DEBUG_LOG_HXJ
//#define DEBUG_LOG_HXJ_DEBUG
//#define DEBUG_LOG_ZXL
//#define DEBUG_LOG_ZQ
//#define DEBUG_LOG_TAS
//#define PRINT_TXT
//#define DEBUG_LOG_V2
//#define DEBUG_LOG_TTA
//#define DEBUG_LOG_TRACKER_EXISTING
//#define DEBUG_LOG_TRAJDISTANCE
//#define PINGSHIZADIAN
//#define DEBUG_COHE_TRACK
//#define DEBUG_LOG_RPE // radar pose estimation
//#define DEBUG_LOG_GYRO
//#define DEBUG_LOG_THREATLEVEL
//#define DUMMY_TEST_PRINT_TXT
//#define DEBUG_LOG_TRK2GLOBAL
//#define FIX_DUMMYTEST_LOG
//#define DEBUG_LOG_TASSIL
//#define DEBUG_TIMESTAMP
//#define DEBUG_UNAMBIVEL
//#define DEBUG_SAMEROOT
#endif

#define STATE_DETECTION_OUTPUT
#define TRANSFORM_COORDINATE

#define MMWAVE_3D
#define DEMO_ACUR
//#define USING_DUMMY_TEST
#ifdef USING_DUMMY_TEST
#define ABUDHABI_SHOW_SCENES 0
#endif // USING_DUMMY_TEST

extern volatile int32_t g_byDummyTest;
extern volatile int32_t g_byAbudhabiShowScenes;
extern volatile int32_t g_byDevHwGen;
extern volatile int32_t g_byMagClutRemove;

// For JPDAF
#define JPDAF_ENABLE
#define JPDAF_SIGMA 0.1f
//#define JPDAF_DEBUGLOG
//#define DATA_ASSOCIATION_USING_CLUSTERING
// For existing probability of tracker
//#define TRACKER_PROB_DEBUGLOG

#ifdef M_PI
#undef M_PI
#endif
#define M_PI (3.141592653589793f)//3.141592653589793, (3.1415926f)

#define DEG2RAD (0.017453292519943f) //M_PI/180.0f,0.017453292519943,(0.01745329f)
#define RAD2DEG (57.29577951308232f) //180.0f/M_PI,57.295779513082320,(57.2957795f)
#define const_e (2.71828f)

#define ERROR_CODE_OFFSET 100

#define iRANGE 0
#define iAZIMUTH 1
#define iDOPPLER 2

#define NUM_EXTERN_DOPPLER (2)
#define LIGHT_SPEED 3E+08
#define FREQ_WAVE 24.07E+09
#define LAMBDA (LIGHT_SPEED/FREQ_WAVE)
#define T_CHIRP_0 (315E-6)
#define T_CHIRP_1 (265E-6)
#define AMBI_NUM_T0 (2U)
#define AMBI_NUM_T1 (2U)
//#define T_CHIRP_0 (235E-6)
//#define T_CHIRP_1 (265E-6)
//#define AMBI_NUM_T0 (1U)
//#define AMBI_NUM_T1 (2U)
#define MAXVEL 50.f 
#define DOPPLER_SCOPE_WAVETYPE_0 (LAMBDA/2.f/T_CHIRP_0)
#define DOPPLER_SCOPE_WAVETYPE_1 (LAMBDA/2.f/T_CHIRP_1)
#define FRAME_TIME_TWS 0.8272f
#define FRAME_TIME_TAS 0.2256f

#define NUMBER_RADAR 2
// OnlineExterCalibration
#define EXTERCALIBR_FRAMES_NUM 120 //100
#define EXTERCALIBR_FRAMES_RESET 20
#define EXTERCALIBR_FRAMES_OBSERVER 200 //200

#define CONST_DELTA_T  (1.f/120.f) //0.05f //0.075f

// For existing probability of tracker
#define TRACKEREXISTING_MAXPROBCLUSNUM 10
#define TRACKEREXISTING_MAXPROBLIFENUM 50
#define TRACKEREXISTING_MAXPROBMOTIONNUM 10
#define TRACKEREXISTING_W1 0.4f
#define TRACKEREXISTING_W2 0.6f
//another method,by hxj
#define TRACKEREXISTING_OR_MAXPROBMOTIONNUM 10 //12
#define TRACKEREXISTING_OR_MAXPROBCLUSNUM 5
#define TRACKEREXISTING_OR_W1 0.f
#define TRACKEREXISTING_OR_W2 1.0f
#define TRACKEREXISTING_OR_W3 0.f

// For grid map and visualizer
#define IMG_WIDTH  1600 //720 //1440 //720 //480 // The times of 12
#define IMG_HEIGHT  800 //2400 //400 //600 // The times of 10
#define IMG_WIDTH_SCALE  0.8//10 //20 //5
#define IMG_HEIGHT_SCALE  0.8 //5 //20 //5


#define OUTRADARCOORRAVH

#define MAXNUMOFPROTECTSPOTS 3



#endif // COMMON_DEFINE_H_
