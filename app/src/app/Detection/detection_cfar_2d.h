#ifndef DETECTION_CFAR_2D_H
#define DETECTION_CFAR_2D_H


/*==================================================================================================
*                                        INCLUDE FILES
==================================================================================================*/
#include "../../inc/radar_common.h"
#include "detection_interface.h"
#include <sys/time.h>
#include <stdio.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif


/*==================================================================================================
*                                      DEFINES AND MACROS
==================================================================================================*/
//* 0.initial parameter *//
#define ENABLE_PS_STATIC 				FALSE //TRUE, FALSE

#define MAX_OUTPUT_CFAR_TARGET			64

// CFAR method
#define RANGE_CFAR_METHOD				2  /* cfar range method:1-CA;2-GO;3-OS */
#define DOPPLER_CFAR_METHOD 			2  /* cfar doppler method:1-CA;2-GO */
#define REMOVE_MIN_FLAG                 0  /* whether to remove the minimum value of window:1-yes;0-no */
#define NUM_REMOVE_MAX_R                2  /* The number of maximum units deleted by CMLD */
#define NUM_REMOVE_MIN_R                1  /* The number of minimum units deleted by CMLD */



// guard and win unit
#define DEFAULT_RANGE_GUARD				2 //4/2/3
#define DEFAULT_RANGE_WIN				6 //4/6/5
#define DEFAULT_DOPPLER_GUARD			4 //4/4/5
#define DEFAULT_DOPPLER_WIN				6 //4/6/6

//guard threshold of histogram noise
#define HIST_THRESHOLD_GUARD			3        /* guard threshold of histogram noise, 3,0 */


// for calculate azimuth
#define TABLE_LEN_ZHENG                 200      /* number of add table */
#define TABLE_LEN_FU                    200      /* number of reduce table */
#define ANGLE_STEP                      0.01f    /* step of angle, Deg */

#define AZI_BOUNDARY                    2.0f     /* boundary of azimuth Offset */
#define RATIO_BOUNDARY_FU               0.6f  /* boundary of sub/sum,minus,0.5634f */
#define K_RATIO_FU                      -3.6372f /* k of sub/sum beeline,minus */
#define B_RATIO_FU                      0.1053f  /* b of sub/sum beeline,minus */

#define RATIO_BOUNDARY_ZHENG            0.6f  /* boundary of sub/sum,plus,0.5425f */
#define K_RATIO_ZHENG                   3.7638f  /* k of sub/sum beeline,plus */
#define B_RATIO_ZHENG                   0.0687f  /* b of sub/sum beeline,plus */

#define SYS_PHASE_POLE                  0u       /* system phase pole to calculate the sign of the offset angle */
#define THETA_INCRESS_SUB               0.f      /* compensated Phase of sub channelthetaIncrese, Rad */
#define D_SUBARRAY                      0.0464f  /* distance between A and B subarray */


//* 1.lower limit of CFAR *//

////9dBCFAR！！OK
//#define CFAR_RANGE_THRESHOLD_SEG_ST_1ST	1872	/* cfar range threshold segment 1st, 22(1872) */
//#define CFAR_RANGE_THRESHOLD_SEG_ST_2ND	1361	/* cfar range threshold segment 2nd, 16(1361) */
//#define CFAR_RANGE_THRESHOLD_SEG_ST_3RD	766	/* cfar range threshold segment 3rd, 13(1106),12(1021) */
//#define CFAR_RANGE_THRESHOLD_SEG_ST_4TH	766	/* cfar range threshold segment 4th, 12(1021),12.5(1064),11.5(979),10.5(894),13(1106),11(936) */
//#define CFAR_RANGE_THRESHOLD_SEG_ST_5TH	766	/* cfar range threshold segment 5th, 90 */
//#define CFAR_RANGE_THRESHOLD_SEG_ST_6TH	7660	/* cfar range threshold segment 6th, 90 */
//
//#define CFAR_DOPPLER_THRESHOLD_SEG_ST_1ST	1616	/* cfar doppler threshold segment 1st, 19(1616) */
//#define CFAR_DOPPLER_THRESHOLD_SEG_ST_2ND	1276	/* cfar doppler threshold segment 2nd, 15(1276) */
//#define CFAR_DOPPLER_THRESHOLD_SEG_ST_3RD	766	/* cfar doppler threshold segment 3rd, 13(1106),12(1021) */
//#define CFAR_DOPPLER_THRESHOLD_SEG_ST_4TH	766	/* cfar doppler threshold segment 4th, 11(936),13(1106),12(1021),11.5(979) */
//#define CFAR_DOPPLER_THRESHOLD_SEG_ST_5TH	766	/* cfar doppler threshold segment 5th, 100 */
//#define CFAR_DOPPLER_THRESHOLD_SEG_ST_6TH	8510	/* cfar doppler threshold segment 6th, 100 */
//
//#define CFAR_GLOBAL_THRESHOLD_SEG_ST_1ST	1276	/* cfar global threshold segment 1st, 14(1191), 15(1276) */
//#define CFAR_GLOBAL_THRESHOLD_SEG_ST_2ND	1106	/* cfar global threshold segment 2nd, 11(936),12(1021),13(1106)  */
//#define CFAR_GLOBAL_THRESHOLD_SEG_ST_3RD	766		/* cfar global threshold segment 3rd, 11(936),12(1021),10(851),10.5(894)  */
//#define CFAR_GLOBAL_THRESHOLD_SEG_ST_4TH	766	/* cfar global threshold segment 4th, 11(936),12(1021),10(851),9(766)  */
//#define CFAR_GLOBAL_THRESHOLD_SEG_ST_5TH	766	/* cfar global threshold segment 5th, 90 */
//#define CFAR_GLOBAL_THRESHOLD_SEG_ST_6TH	7660	/* cfar global threshold segment 6th, 90 */

////0320-CFAR2！！JiChu OK
//#define CFAR_RANGE_THRESHOLD_SEG_ST_1ST	1872	/* cfar range threshold segment 1st, 22(1872) */
//#define CFAR_RANGE_THRESHOLD_SEG_ST_2ND	1361	/* cfar range threshold segment 2nd, 16(1361) */
//#define CFAR_RANGE_THRESHOLD_SEG_ST_3RD	1106	/* cfar range threshold segment 3rd, 13(1106),12(1021) */
//#define CFAR_RANGE_THRESHOLD_SEG_ST_4TH	1021	/* cfar range threshold segment 4th, 12(1021),12.5(1064),11.5(979),10.5(894),13(1106),11(936) */
//#define CFAR_RANGE_THRESHOLD_SEG_ST_5TH	1064	/* cfar range threshold segment 5th, 90 */
//#define CFAR_RANGE_THRESHOLD_SEG_ST_6TH	7660	/* cfar range threshold segment 6th, 90 */
//
//#define CFAR_DOPPLER_THRESHOLD_SEG_ST_1ST	1616	/* cfar doppler threshold segment 1st, 19(1616) */
//#define CFAR_DOPPLER_THRESHOLD_SEG_ST_2ND	1276	/* cfar doppler threshold segment 2nd, 15(1276) */
//#define CFAR_DOPPLER_THRESHOLD_SEG_ST_3RD	1106	/* cfar doppler threshold segment 3rd, 13(1106),12(1021) */
//#define CFAR_DOPPLER_THRESHOLD_SEG_ST_4TH	1106	/* cfar doppler threshold segment 4th, 11(936),13(1106),12(1021),11.5(979) */
//#define CFAR_DOPPLER_THRESHOLD_SEG_ST_5TH	1106	/* cfar doppler threshold segment 5th, 100 */
//#define CFAR_DOPPLER_THRESHOLD_SEG_ST_6TH	8510	/* cfar doppler threshold segment 6th, 100 */
//
//#define CFAR_GLOBAL_THRESHOLD_SEG_ST_1ST	1276	/* cfar global threshold segment 1st, 14(1191), 15(1276) */
//#define CFAR_GLOBAL_THRESHOLD_SEG_ST_2ND	1106	/* cfar global threshold segment 2nd, 11(936),12(1021),13(1106)  */
//#define CFAR_GLOBAL_THRESHOLD_SEG_ST_3RD	936		/* cfar global threshold segment 3rd, 11(936),12(1021),10(851),10.5(894)  */
//#define CFAR_GLOBAL_THRESHOLD_SEG_ST_4TH	936	/* cfar global threshold segment 4th, 11(936),12(1021),10(851),9(766)  */
//#define CFAR_GLOBAL_THRESHOLD_SEG_ST_5TH	936	/* cfar global threshold segment 5th, 90 */
//#define CFAR_GLOBAL_THRESHOLD_SEG_ST_6TH	7660	/* cfar global threshold segment 6th, 90 */

////0419-CFAR！GO-1
//#define CFAR_RANGE_THRESHOLD_SEG_ST_1ST	1872	/* cfar range threshold segment 1st, 22(1872) */
//#define CFAR_RANGE_THRESHOLD_SEG_ST_2ND	1361	/* cfar range threshold segment 2nd, 16(1361) */
//#define CFAR_RANGE_THRESHOLD_SEG_ST_3RD	1106	/* cfar range threshold segment 3rd, 13(1106),12(1021) */
//#define CFAR_RANGE_THRESHOLD_SEG_ST_4TH	936		/* cfar range threshold segment 4th, 12(1021),12.5(1064),11.5(979),10.5(894),13(1106),11(936) */
//#define CFAR_RANGE_THRESHOLD_SEG_ST_5TH	1064	/* cfar range threshold segment 5th, 90 */
//#define CFAR_RANGE_THRESHOLD_SEG_ST_6TH	7660	/* cfar range threshold segment 6th, 90 */
//
//#define CFAR_DOPPLER_THRESHOLD_SEG_ST_1ST	1616	/* cfar doppler threshold segment 1st, 19(1616) */
//#define CFAR_DOPPLER_THRESHOLD_SEG_ST_2ND	1276	/* cfar doppler threshold segment 2nd, 15(1276) */
//#define CFAR_DOPPLER_THRESHOLD_SEG_ST_3RD	1106	/* cfar doppler threshold segment 3rd, 13(1106),12(1021) */
//#define CFAR_DOPPLER_THRESHOLD_SEG_ST_4TH	1021	/* cfar doppler threshold segment 4th, 11(936),13(1106),12(1021),11.5(979) */
//#define CFAR_DOPPLER_THRESHOLD_SEG_ST_5TH	1106	/* cfar doppler threshold segment 5th, 100 */
//#define CFAR_DOPPLER_THRESHOLD_SEG_ST_6TH	8510	/* cfar doppler threshold segment 6th, 100 */
//
//#define CFAR_GLOBAL_THRESHOLD_SEG_ST_1ST	1276	/* cfar global threshold segment 1st, 14(1191), 15(1276) */
//#define CFAR_GLOBAL_THRESHOLD_SEG_ST_2ND	1106	/* cfar global threshold segment 2nd, 11(936),12(1021),13(1106)  */
//#define CFAR_GLOBAL_THRESHOLD_SEG_ST_3RD	1021	/* cfar global threshold segment 3rd, 11(936),12(1021),10(851),10.5(894)  */
//#define CFAR_GLOBAL_THRESHOLD_SEG_ST_4TH	936		/* cfar global threshold segment 4th, 11(936),12(1021),10(851),9(766) 936,894 */
//#define CFAR_GLOBAL_THRESHOLD_SEG_ST_5TH	936		/* cfar global threshold segment 5th, 90 */
//#define CFAR_GLOBAL_THRESHOLD_SEG_ST_6TH	7660	/* cfar global threshold segment 6th, 90 */

////0419-CFAR！GO-2
//#define CFAR_RANGE_THRESHOLD_SEG_ST_1ST	1872	/* cfar range threshold segment 1st, 22(1872) */
//#define CFAR_RANGE_THRESHOLD_SEG_ST_2ND	1361	/* cfar range threshold segment 2nd, 16(1361) */
//#define CFAR_RANGE_THRESHOLD_SEG_ST_3RD	1106	/* cfar range threshold segment 3rd, 13(1106),12(1021) */
//#define CFAR_RANGE_THRESHOLD_SEG_ST_4TH	936		/* cfar range threshold segment 4th, 12(1021),12.5(1064),11.5(979),10.5(894),13(1106),11(936) */
//#define CFAR_RANGE_THRESHOLD_SEG_ST_5TH	1064	/* cfar range threshold segment 5th, 90 */
//#define CFAR_RANGE_THRESHOLD_SEG_ST_6TH	7660	/* cfar range threshold segment 6th, 90 */
//
//#define CFAR_DOPPLER_THRESHOLD_SEG_ST_1ST	1616	/* cfar doppler threshold segment 1st, 19(1616) */
//#define CFAR_DOPPLER_THRESHOLD_SEG_ST_2ND	1276	/* cfar doppler threshold segment 2nd, 15(1276) */
//#define CFAR_DOPPLER_THRESHOLD_SEG_ST_3RD	1106	/* cfar doppler threshold segment 3rd, 13(1106),12(1021) */
//#define CFAR_DOPPLER_THRESHOLD_SEG_ST_4TH	1021	/* cfar doppler threshold segment 4th, 11(936),13(1106),12(1021),11.5(979) */
//#define CFAR_DOPPLER_THRESHOLD_SEG_ST_5TH	1106	/* cfar doppler threshold segment 5th, 100 */
//#define CFAR_DOPPLER_THRESHOLD_SEG_ST_6TH	8510	/* cfar doppler threshold segment 6th, 100 */
//
//#define CFAR_GLOBAL_THRESHOLD_SEG_ST_1ST	1276	/* cfar global threshold segment 1st, 14(1191), 15(1276) */
//#define CFAR_GLOBAL_THRESHOLD_SEG_ST_2ND	1106	/* cfar global threshold segment 2nd, 11(936),12(1021),13(1106)  */
//#define CFAR_GLOBAL_THRESHOLD_SEG_ST_3RD	1021	/* cfar global threshold segment 3rd, 11(936),12(1021),10(851),10.5(894)  */
//#define CFAR_GLOBAL_THRESHOLD_SEG_ST_4TH	1021		/* cfar global threshold segment 4th, 11(936),12(1021),10(851),9(766) 936,894 */
//#define CFAR_GLOBAL_THRESHOLD_SEG_ST_5TH	936		/* cfar global threshold segment 5th, 90 */
//#define CFAR_GLOBAL_THRESHOLD_SEG_ST_6TH	7660	/* cfar global threshold segment 6th, 90 */

#if 0
//0420-rCA,vGO！2 OK
#define CFAR_RANGE_THRESHOLD_SEG_ST_1ST	1872		/* cfar range threshold segment 1st, 22(1872) */
#define CFAR_RANGE_THRESHOLD_SEG_ST_2ND	1361		/* cfar range threshold segment 2nd, 16(1361) */
#define CFAR_RANGE_THRESHOLD_SEG_ST_3RD	1106		/* cfar range threshold segment 3rd, 13(1106) */
#define CFAR_RANGE_THRESHOLD_SEG_ST_4TH	1021		/* cfar range threshold segment 4th, 11.5(979),10.5(894),13(1106),11(936),12(1021) */
#define CFAR_RANGE_THRESHOLD_SEG_ST_5TH	1021		/* cfar range threshold segment 5th, 90 ,12.5(1064)*/
#define CFAR_RANGE_THRESHOLD_SEG_ST_6TH	7660		/* cfar range threshold segment 6th, 90 */

#define CFAR_DOPPLER_THRESHOLD_SEG_ST_1ST	1616	/* cfar doppler threshold segment 1st, 19(1616) */
#define CFAR_DOPPLER_THRESHOLD_SEG_ST_2ND	1276	/* cfar doppler threshold segment 2nd, 15(1276) */
#define CFAR_DOPPLER_THRESHOLD_SEG_ST_3RD	1106	/* cfar doppler threshold segment 3rd, 12(1021),13(1106) */
#define CFAR_DOPPLER_THRESHOLD_SEG_ST_4TH	1021	/* cfar doppler threshold segment 4th, 11(936),13(1106),11.5(979),12(1021) */
#define CFAR_DOPPLER_THRESHOLD_SEG_ST_5TH	1021	/* cfar doppler threshold segment 5th, 100 ,12(1021)*/
#define CFAR_DOPPLER_THRESHOLD_SEG_ST_6TH	8510	/* cfar doppler threshold segment 6th, 100 */

#define CFAR_GLOBAL_THRESHOLD_SEG_ST_1ST	1276	/* cfar global threshold segment 1st, 14(1191), 15(1276) */
#define CFAR_GLOBAL_THRESHOLD_SEG_ST_2ND	1106	/* cfar global threshold segment 2nd, 11(936),12(1021),13(1106)  */
#define CFAR_GLOBAL_THRESHOLD_SEG_ST_3RD	1021	/* cfar global threshold segment 3rd, 11(936),10(851),10.5(894) ,12(1021) */
#define CFAR_GLOBAL_THRESHOLD_SEG_ST_4TH	1021	/* cfar global threshold segment 4th, 11(936),10(851),9(766),12(1021)  */
#define CFAR_GLOBAL_THRESHOLD_SEG_ST_5TH	936		/* cfar global threshold segment 5th, 90, 11(936) */
#define CFAR_GLOBAL_THRESHOLD_SEG_ST_6TH	7660	/* cfar global threshold segment 6th, 90 */


//0711-rCA,vGO！2 OK
#define CFAR_RANGE_THRESHOLD_SEG_ST_1ST	1872		/* cfar range threshold segment 1st, 22(1872) */
#define CFAR_RANGE_THRESHOLD_SEG_ST_2ND	1361		/* cfar range threshold segment 2nd, 16(1361) */
#define CFAR_RANGE_THRESHOLD_SEG_ST_3RD	1106		/* cfar range threshold segment 3rd, 13(1106) */
#define CFAR_RANGE_THRESHOLD_SEG_ST_4TH	1021			/* cfar range threshold segment 4th, 11.5(979),10.5(894),13(1106),11(936),12(1021) */
#define CFAR_RANGE_THRESHOLD_SEG_ST_5TH	1021		/* cfar range threshold segment 5th, 90 ,12.5(1064)*/
#define CFAR_RANGE_THRESHOLD_SEG_ST_6TH	7660		/* cfar range threshold segment 6th, 90 */

#define CFAR_DOPPLER_THRESHOLD_SEG_ST_1ST	1616	/* cfar doppler threshold segment 1st, 19(1616) */
#define CFAR_DOPPLER_THRESHOLD_SEG_ST_2ND	1276	/* cfar doppler threshold segment 2nd, 15(1276) */
#define CFAR_DOPPLER_THRESHOLD_SEG_ST_3RD	1106	/* cfar doppler threshold segment 3rd, 12(1021),13(1106) */
#define CFAR_DOPPLER_THRESHOLD_SEG_ST_4TH	1021		/* cfar doppler threshold segment 4th, 11(936),13(1106),11.5(979),12(1021) */
#define CFAR_DOPPLER_THRESHOLD_SEG_ST_5TH	1021	/* cfar doppler threshold segment 5th, 100 ,12(1021)*/
#define CFAR_DOPPLER_THRESHOLD_SEG_ST_6TH	8510	/* cfar doppler threshold segment 6th, 100 */

#define CFAR_GLOBAL_THRESHOLD_SEG_ST_1ST	1276	/* cfar global threshold segment 1st, 14(1191), 15(1276) */
#define CFAR_GLOBAL_THRESHOLD_SEG_ST_2ND	1106	/* cfar global threshold segment 2nd, 11(936),12(1021),13(1106)  */
#define CFAR_GLOBAL_THRESHOLD_SEG_ST_3RD	1021	/* cfar global threshold segment 3rd, 11(936),10(851),10.5(894) ,12(1021) */
#define CFAR_GLOBAL_THRESHOLD_SEG_ST_4TH	1021		/* cfar global threshold segment 4th, 11(936),10(851),9(766),12(1021)  */
#define CFAR_GLOBAL_THRESHOLD_SEG_ST_5TH	936		/* cfar global threshold segment 5th, 90, 11(936) */
#define CFAR_GLOBAL_THRESHOLD_SEG_ST_6TH	7660	/* cfar global threshold segment 6th, 90 */
#endif



//0804--210-501m:rCMLD,vCA,501-801m:rCA,vGO,801-1200m:rCMLD,vGO,others GO, test
#define CFAR_RANGE_THRESHOLD_SEG_ST_1ST	1872		/* cfar range threshold segment 1st, 22(1872) */
#define CFAR_RANGE_THRESHOLD_SEG_ST_2ND	1361		/* cfar range threshold segment 2nd, 16(1361) */
#define CFAR_RANGE_THRESHOLD_SEG_ST_3RD	1361		/* cfar range threshold segment 3rd, 13(1106),15(1276),16(1361)  */
#define CFAR_RANGE_THRESHOLD_SEG_ST_4TH	1021			/* cfar range threshold segment 4th, 11.5(979),10.5(894),13(1106),11(936),12(1021),14(1191) */
#define CFAR_RANGE_THRESHOLD_SEG_ST_5TH	1276		/* cfar range threshold segment 5th, 90 ,12.5(1064),14(1191),15(1276)*/
#define CFAR_RANGE_THRESHOLD_SEG_ST_6TH	7660		/* cfar range threshold segment 6th, 90 */

#define CFAR_DOPPLER_THRESHOLD_SEG_ST_1ST	1616	/* cfar doppler threshold segment 1st, 19(1616) */
#define CFAR_DOPPLER_THRESHOLD_SEG_ST_2ND	1276	/* cfar doppler threshold segment 2nd, 15(1276) */
#define CFAR_DOPPLER_THRESHOLD_SEG_ST_3RD	1191	/* cfar doppler threshold segment 3rd, 12(1021),13(1106), 14(1191) */
#define CFAR_DOPPLER_THRESHOLD_SEG_ST_4TH	1021		/* cfar doppler threshold segment 4th, 11(936),13(1106),11.5(979),12(1021) */
#define CFAR_DOPPLER_THRESHOLD_SEG_ST_5TH	1021	/* cfar doppler threshold segment 5th, 100 ,12(1021)*/
#define CFAR_DOPPLER_THRESHOLD_SEG_ST_6TH	8510	/* cfar doppler threshold segment 6th, 100 */

#define CFAR_GLOBAL_THRESHOLD_SEG_ST_1ST	1276	/* cfar global threshold segment 1st, 14(1191), 15(1276) */
#define CFAR_GLOBAL_THRESHOLD_SEG_ST_2ND	1106	/* cfar global threshold segment 2nd, 11(936),12(1021),13(1106)  */
#define CFAR_GLOBAL_THRESHOLD_SEG_ST_3RD	1021	/* cfar global threshold segment 3rd, 11(936),10(851),10.5(894) ,12(1021) */
#define CFAR_GLOBAL_THRESHOLD_SEG_ST_4TH	1021		/* cfar global threshold segment 4th, 11(936),10(851),9(766),12(1021)  */
#define CFAR_GLOBAL_THRESHOLD_SEG_ST_5TH	936		/* cfar global threshold segment 5th, 90, 11(936) */
#define CFAR_GLOBAL_THRESHOLD_SEG_ST_6TH	7660	/* cfar global threshold segment 6th, 90 */



//* 2.upper limit of CFAR *//
//0619 test -- 3rdGenRadar
#define CFAR_RANGE_THRESHOLD_SEG_ED_1ST	6804		/* cfar range threshold segment 1st, 60(5103) */
#define CFAR_RANGE_THRESHOLD_SEG_ED_2ND	6804		/* cfar range threshold segment 2nd, 50(4253) */
#define CFAR_RANGE_THRESHOLD_SEG_ED_3RD	6804		/* cfar range threshold segment 3rd, 35(2977) */
#define CFAR_RANGE_THRESHOLD_SEG_ED_4TH	6804		/* cfar range threshold segment 4th, 26(2212) */
#define CFAR_RANGE_THRESHOLD_SEG_ED_5TH	6804		/* cfar range threshold segment 5th, 24(2042) */
#define CFAR_RANGE_THRESHOLD_SEG_ED_6TH	7660		/* cfar range threshold segment 6th, 90 */

#define CFAR_DOPPLER_THRESHOLD_SEG_ED_1ST	6804	/* cfar doppler threshold segment 1st, 52(4423) */
#define CFAR_DOPPLER_THRESHOLD_SEG_ED_2ND	6804	/* cfar doppler threshold segment 2nd, 45(3828),35(2977) */
#define CFAR_DOPPLER_THRESHOLD_SEG_ED_3RD	6804	/* cfar doppler threshold segment 3rd, 38(3232) */
#define CFAR_DOPPLER_THRESHOLD_SEG_ED_4TH	6804	/* cfar doppler threshold segment 4th, 26(2212),30(2552) */
#define CFAR_DOPPLER_THRESHOLD_SEG_ED_5TH	6804	/* cfar doppler threshold segment 5th, 24(2042) */
#define CFAR_DOPPLER_THRESHOLD_SEG_ED_6TH	8510	/* cfar doppler threshold segment 6th, 100 */

#define CFAR_GLOBAL_THRESHOLD_SEG_ED_1ST	6804	/* cfar global threshold segment 1st, 55(4678) */
#define CFAR_GLOBAL_THRESHOLD_SEG_ED_2ND	6804	/* cfar global threshold segment 2nd, 48(4083),43(3658)  */
#define CFAR_GLOBAL_THRESHOLD_SEG_ED_3RD	6804	/* cfar global threshold segment 3rd, 36(3062)  */
#define CFAR_GLOBAL_THRESHOLD_SEG_ED_4TH	6804	/* cfar global threshold segment 4th, 28(2382)  */
#define CFAR_GLOBAL_THRESHOLD_SEG_ED_5TH	6804	/* cfar global threshold segment 5th, 24(2042) */
#define CFAR_GLOBAL_THRESHOLD_SEG_ED_6TH	7660	/* cfar global threshold segment 6th, 90 */

////0419 test OK --2ndGenRadar
//#define CFAR_RANGE_THRESHOLD_SEG_ED_1ST	5103		/* cfar range threshold segment 1st, 60(5103) */
//#define CFAR_RANGE_THRESHOLD_SEG_ED_2ND	4253		/* cfar range threshold segment 2nd, 50(4253) */
//#define CFAR_RANGE_THRESHOLD_SEG_ED_3RD	2977		/* cfar range threshold segment 3rd, 35(2977) */
//#define CFAR_RANGE_THRESHOLD_SEG_ED_4TH	2212		/* cfar range threshold segment 4th, 26(2212) */
//#define CFAR_RANGE_THRESHOLD_SEG_ED_5TH	2042		/* cfar range threshold segment 5th, 24(2042) */
//#define CFAR_RANGE_THRESHOLD_SEG_ED_6TH	7660		/* cfar range threshold segment 6th, 90 */
//
//#define CFAR_DOPPLER_THRESHOLD_SEG_ED_1ST	4423	/* cfar doppler threshold segment 1st, 52(4423) */
//#define CFAR_DOPPLER_THRESHOLD_SEG_ED_2ND	3828	/* cfar doppler threshold segment 2nd, 45(3828),35(2977) */
//#define CFAR_DOPPLER_THRESHOLD_SEG_ED_3RD	3232	/* cfar doppler threshold segment 3rd, 38(3232) */
//#define CFAR_DOPPLER_THRESHOLD_SEG_ED_4TH	2552	/* cfar doppler threshold segment 4th, 26(2212),30(2552) */
//#define CFAR_DOPPLER_THRESHOLD_SEG_ED_5TH	2042	/* cfar doppler threshold segment 5th, 24(2042) */
//#define CFAR_DOPPLER_THRESHOLD_SEG_ED_6TH	8510	/* cfar doppler threshold segment 6th, 100 */
//
//#define CFAR_GLOBAL_THRESHOLD_SEG_ED_1ST	4678	/* cfar global threshold segment 1st, 55(4678) */
//#define CFAR_GLOBAL_THRESHOLD_SEG_ED_2ND	4083	/* cfar global threshold segment 2nd, 48(4083),43(3658)  */
//#define CFAR_GLOBAL_THRESHOLD_SEG_ED_3RD	3062	/* cfar global threshold segment 3rd, 36(3062)  */
//#define CFAR_GLOBAL_THRESHOLD_SEG_ED_4TH	2382	/* cfar global threshold segment 4th, 28(2382)  */
//#define CFAR_GLOBAL_THRESHOLD_SEG_ED_5TH	2042	/* cfar global threshold segment 5th, 24(2042) */
//#define CFAR_GLOBAL_THRESHOLD_SEG_ED_6TH	7660	/* cfar global threshold segment 6th, 90 */

////0320 test OK
//#define CFAR_RANGE_THRESHOLD_SEG_ED_1ST	5103	/* cfar range threshold segment 1st, 60(5103) */
//#define CFAR_RANGE_THRESHOLD_SEG_ED_2ND	5103	/* cfar range threshold segment 2nd, 60(5103) */
//#define CFAR_RANGE_THRESHOLD_SEG_ED_3RD	3828	/* cfar range threshold segment 3rd, 45(3828) */
//#define CFAR_RANGE_THRESHOLD_SEG_ED_4TH	2212	/* cfar range threshold segment 4th, 26(2212) */
//#define CFAR_RANGE_THRESHOLD_SEG_ED_5TH	2212	/* cfar range threshold segment 5th, 26(2212) */
//#define CFAR_RANGE_THRESHOLD_SEG_ED_6TH	7660	/* cfar range threshold segment 6th, 90 */
//
//#define CFAR_DOPPLER_THRESHOLD_SEG_ED_1ST	4253	/* cfar doppler threshold segment 1st, 50(4253) */
//#define CFAR_DOPPLER_THRESHOLD_SEG_ED_2ND	2977	/* cfar doppler threshold segment 2nd, 50(4253),35(2977) */
//#define CFAR_DOPPLER_THRESHOLD_SEG_ED_3RD	3573	/* cfar doppler threshold segment 3rd, 42(3573) */
//#define CFAR_DOPPLER_THRESHOLD_SEG_ED_4TH	2212	/* cfar doppler threshold segment 4th, 26(2212) */
//#define CFAR_DOPPLER_THRESHOLD_SEG_ED_5TH	2212	/* cfar doppler threshold segment 5th, 26(2212) */
//#define CFAR_DOPPLER_THRESHOLD_SEG_ED_6TH	8510	/* cfar doppler threshold segment 6th, 100 */
//
//#define CFAR_GLOBAL_THRESHOLD_SEG_ED_1ST	4253	/* cfar global threshold segment 1st, 50(4253) */
//#define CFAR_GLOBAL_THRESHOLD_SEG_ED_2ND	3658	/* cfar global threshold segment 2nd, 50(4253),43(3658)  */
//#define CFAR_GLOBAL_THRESHOLD_SEG_ED_3RD	3573	/* cfar global threshold segment 3rd, 42(3573)  */
//#define CFAR_GLOBAL_THRESHOLD_SEG_ED_4TH	2552	/* cfar global threshold segment 4th, 30(2552)  */
//#define CFAR_GLOBAL_THRESHOLD_SEG_ED_5TH	2552	/* cfar global threshold segment 5th, 30(2552) */
//#define CFAR_GLOBAL_THRESHOLD_SEG_ED_6TH	7660	/* cfar global threshold segment 6th, 90 */



//* 3.distance segment of CFAR *//
#define RANGE_CUT_INDEX_1ST				2		/* rangeBin cut index 1st 2(6m) */
#define RANGE_CUT_INDEX_2ND				17		/* rangeBin cut index 2nd 10(30m),17(51) */
#define RANGE_CUT_INDEX_3RD				70		/* rangeBin cut index 3rd 100(300m),70(210m) */
#define RANGE_CUT_INDEX_4TH				167		/* rangeBin cut index 4th 105(315),167(501m) */
#define RANGE_CUT_INDEX_5TH				267		/* rangeBin cut index 5th 240(720m),210(630m),267(801m) */
#define RANGE_CUT_INDEX_6TH				400		/* rangeBin cut index 6th 430(1290m)240(720),350(1050m) */
#define RANGE_CUT_INDEX_6TH_GEN3		400		/* rangeBin cut index 6th 430(1290m)240(720),350(1050m) */

#if 0
#define RANGE_CUT_INDEX_1ST				2		/* rangeBin cut index 1st 2(6m) */
#define RANGE_CUT_INDEX_2ND				17		/* rangeBin cut index 2nd 10(30m),17(51) */
#define RANGE_CUT_INDEX_3RD				70		/* rangeBin cut index 3rd 100(300m),70(210m),63(189m),77(231) */
#define RANGE_CUT_INDEX_4TH				109		/* rangeBin cut index 4th 167(501m),135(405m),121(363),174(522)109(327)105(315) */
#define RANGE_CUT_INDEX_5TH				210		/* rangeBin cut index 5th 267(801m),240(720m),212(636m),300(900m),334(1002),174(522),210(630m) */
#define RANGE_CUT_INDEX_6TH				240		/* rangeBin cut index 6th 430(1290m)240(720),370(1110m) */
#define RANGE_CUT_INDEX_6TH_GEN3		350		/* rangeBin cut index 6th 430(1290m)240(720),350(1050m) */
#endif

//* 4.other parameters (include OS CFAR) *//
#define DEFAULT_OS_RANGE				150

#define RANGE_CUT_INDEX_REAR			500		/* range cut index rear */
#define DOPPLER_STATIC_SCOPE			6		/* doppler static scope */
#define OS_K							0.5f
#define APPROXIMATE_ZERO				0.000000001f
#define COEF_OF_SNR_CALC				(0.0039*3.0103*100)	/* log2 to log10 */
#define COEF_OF_HIST_MAG				256

//* 5.waveform and resolution parameters *//
#define DEFAULT_C_LIGHTSPEED_MPS		300000000.0f	// c=3.0*1e8m/s
#define DEFAULT_B_BANDWIDTH_HZ			50000000.0f//39961000.0f		// B=4096/25MHz/205us*50MHz=41059732.0f
#define DEFAULT_F_FREQ_CENTER_HZ		24100000000.0f	// Fc=24.0GHz,Fc=24.1GHz
#define DEFAULT_TIME_CHIRP_0_S			(0.000315f)		// Tc_0=315us
#define DEFAULT_TIME_CHIRP_1_S			(0.000265f)		// Tc_1=265us
#define DEFAULT_LAMBDA_M				(DEFAULT_C_LIGHTSPEED_MPS / DEFAULT_F_FREQ_CENTER_HZ)	// c/Fc
#define DEFAULT_RANGE_RESO				(DEFAULT_C_LIGHTSPEED_MPS / (2 * DEFAULT_B_BANDWIDTH_HZ))	// c/(2*B)
#define DEFAULT_VELOCITY_RESO_0			(DEFAULT_LAMBDA_M / (2 * DEFAULT_DOPPLER_BINS * DEFAULT_TIME_CHIRP_0_S))	// lambda/(2*N*Tc)
#define DEFAULT_VELOCITY_RESO_1			(DEFAULT_LAMBDA_M / (2 * DEFAULT_DOPPLER_BINS * DEFAULT_TIME_CHIRP_1_S))	// lambda/(2*N*Tc)
#define DEFAULT_MAG_RESO				(0.0039*3.0103f)	// 1/256*3.0103f

#define MAX_OUTPUT_PS_POINTS 			1024
#define DETECT_RANGE_START_INDEX		4	//start index of range direction peak search

#define RDM_MAG_PER_HIST_BIN_EXP		8	//2^8
#define RDM_MAG_PER_HIST_BIN			(1 << RDM_MAG_PER_HIST_BIN_EXP) //2^8

#ifndef DEG2RAD
#define DEG2RAD 						(0.01745329f) //M_PI/180.0f
#endif
#ifndef RAD2DEG
#define RAD2DEG 						(57.2957795f) //180.0f/M_PI
#endif

/*==================================================================================================
*                                          CONSTANTS
==================================================================================================*/

/*=================================================================================================
*                                             ENUMS
=================================================================================================*/

/*=================================================================================================
*                                STRUCTURES AND OTHER TYPEDEFS
=================================================================================================*/
typedef struct sResolutionDetect
{
    float RangeResotion;				/* m, range resolution */
    float VelocityResotion[2];			/* m/s, velocity resolution */
    float MagResotion;					/* dB, magnitude resolution */
}sResolutionDetect_t;

typedef struct sCfarParameter
{
    int16_t rangeBins;					/* range dimension bins */
    int16_t dopplerBins;				/* doppler dimension bins */
    uint16_t rangeWin;					/* range dimension win cell */
    uint16_t rangeGuard;				/* range dimension guard cell */
    uint16_t dopplerWin;				/* doppler dimension win cell */
    uint16_t dopplerGuard;				/* doppler dimension guard cell */
    uint16_t rangeThresholdLen;			/* range dimension threshold length */
    uint16_t dopplerThresholdLen;		/* doppler dimension threshold length */
    uint16_t osRange;					/* range dimension oscfar bin */
    uint16_t osRangeK;					/* range dimension oscfar noise cell, biggest is K */
    uint16_t *pRangeThresholdStatic;	/* range dimension threshold for static target */
    uint16_t *pDopplerThresholdStatic;	/* doppler dimension threshold for static target */
    uint16_t *pRangeThresholdMoving;	/* range dimension threshold for moving target */
    uint16_t *pDopplerThresholdMoving;	/* doppler dimension threshold for moving target */
}sCfarParameter_t;

typedef struct sAlgoParameter
{
	int32_t rangeCfarMethod;
	int32_t dopplerCfarMethod;
	int32_t rangeCfarThreshold[6];
	int32_t dopplerCfarThreshold[6];
	int32_t globalCfarThreshold[6];
	int32_t rangeBinCutIdx[6];
	sCfarParameter_t cfarPara;
}sAlgoParameter_t;

void get_algo_parameter(sAlgoParameter_t *algoPara);

/*==================================================================================================
*                                GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/
extern int32_t gRangeCfarMethod;
extern int32_t gDopplerCfarMethod;
extern int32_t gRangeCfarThreshold[6];
extern int32_t gDopplerCfarThreshold[6];
extern int32_t gGlobalCfarThreshold[6];
extern int32_t gRangeBinCutIdx[6];
extern int32_t gRangeCfarThreshold_end[6];
extern int32_t gDopplerCfarThreshold_end[6];
extern int32_t gGlobalCfarThreshold_end[6];
extern sResolutionDetect_t gResolutionDetect;
extern sCfarParameter_t gCfarParameter;

extern struct timespec g_tUsDetectProcCur[32];
extern struct timespec g_tUsDetectProcEnd[32];
extern uint32_t g_tUsDetectProcUsed[32];


/*==================================================================================================
*                                    FUNCTION PROTOTYPES
==================================================================================================*/
void GetRdmapData(uint32_t *rdmBuff);
ret_code_t CalcHistogram(uint32_t *rdmBuff, uint16_t *histBuff);
ret_code_t CalcThreshold(uint16_t *histBuff, uint16_t *histMaxIndex, uint16_t *threshBuff, int16_t nrRangeBins);
ret_code_t CalcTransposeRdm(uint32_t *rdmBuff, uint32_t *rdmTransposeBuff, int32_t rangeBins, int32_t dopplerBins);
ret_code_t CalcPeakSearchBitmap(uint32_t *rdmBuff, uint32_t *rdmTransposeBuff, uint16_t *threshBuff, \
		uint32_t *detectBitmap, uint8_t *rdmapDooplerDimPeakFlag, int32_t rangeBins, int32_t dopplerBins);
int32_t Range_OSCFAR(int32_t idx, int32_t ROW, int32_t COL, int32_t CFAR_WINDOW, int32_t CFAR_GUARD, uint32_t *RangeData, int32_t osRangeK);
int32_t Range_CACFAR(int32_t idx, int32_t ROW, int32_t COL, int32_t CFAR_WINDOW, int32_t CFAR_GUARD, uint32_t *RangeData);
int32_t Range_GOCFAR(int32_t idx, int32_t ROW, int32_t COL, int32_t CFAR_WINDOW, int32_t CFAR_GUARD, uint32_t *RangeData);
int32_t Range_CACFAR_new(int32_t idx, int32_t ROW, int32_t COL, int32_t CFAR_WINDOW, int32_t CFAR_GUARD, uint32_t *RangeData, bool RemoveMin_Flag);
int32_t Range_GOCFAR_new(int32_t idx, int32_t ROW, int32_t COL, int32_t CFAR_WINDOW, int32_t CFAR_GUARD, uint32_t *RangeData, bool RemoveMin_Flag);

int32_t Range_CMLD_CFAR(int32_t idx, int32_t ROW, int32_t COL, int32_t CFAR_WINDOW, int32_t CFAR_GUARD, uint32_t *RangeData, uint8_t removeMax_r, uint8_t removeMin_r);


int32_t Doppler_CACFAR(int32_t idx, int32_t ROW, int32_t COL, int32_t CFAR_WINDOW, int32_t CFAR_GUARD, uint32_t *DopplerData, bool RemoveMax_Flag);
int32_t Doppler_GOCFAR(int32_t idx, int32_t ROW, int32_t COL, int32_t CFAR_WINDOW, int32_t CFAR_GUARD, uint32_t *DopplerData, bool RemoveMax_Flag);
int32_t Doppler_CACFAR_new(int32_t idx, int32_t ROW, int32_t COL, int32_t CFAR_WINDOW, int32_t CFAR_GUARD, uint32_t *DopplerData, bool RemoveMax_Flag, bool RemoveMin_Flag);
int32_t Doppler_GOCFAR_new(int32_t idx, int32_t ROW, int32_t COL, int32_t CFAR_WINDOW, int32_t CFAR_GUARD, uint32_t *DopplerData, bool RemoveMax_Flag, bool RemoveMin_Flag);
// ret_code_t CfarDetection(uint32_t *cfarInput, uint32_t *detectBitmapInput, protocol_object_list_detected_t *detectList);
ret_code_t CfarDetection(uint32_t *cfarInput, uint32_t *detectBitmapInput, uint16_t *threshBuff,
		protocol_object_list_detected_t *detectList, data_path_data_t *pDataPathDataBuf);
ret_code_t calcAzimuth_heCha(protocol_object_list_detected_t *detectList, int16_t *sumSubData);

ret_code_t microDopplerDetect(uint32_t *cfarInput,protocol_object_list_detected_t *detectList);

extern ret_code_t DetectionAlgProcess(sDetectObjData_t *detectObj, data_path_data_t *pDataPathDataBuf);

/**
 * @}
 * @endif
 */

#ifdef __cplusplus
}
#endif

#endif /*DETECTION_CFAR_2D_H*/
