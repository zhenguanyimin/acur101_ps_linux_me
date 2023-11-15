
#ifndef PROTOCOL_OBJECT_H
#define PROTOCOL_OBJECT_H

#include "protocol_common.h"

#define PROTOCOL_OBJECT_LIST_DETECTED_VER (0x00)
#define PROTOCOL_OBJECT_LIST_TRACKED_VER (0x00)

#pragma pack(1)
typedef struct protocol_object_item_detected {
	uint16_t id;
	int16_t azimuth;         //unit:deg INV_ONE6_SCALE
	uint32_t range;          //unit:m INV_ONE6_SCALE
	int16_t elevation;       //unit:deg INV_ONE6_SCALE
	int16_t velocity;        //unit:m/s INV_ONE6_SCALE
	int16_t dopplerChn;      //目标多普勒号
	uint16_t mag;            //unit:db INV_ONE6_SCALE
	uint16_t objConfidence;  //目标置信度 INV_ONE6_SCALE
	uint8_t ambiguous;       //0速度无模糊 1速度存在模糊
	uint8_t classification;  //0未识别 1无人机 2单兵 3车辆 4鸟类 5直升机 其它无效
	uint8_t classifyProb;     //目标类别概率 INV_ONE6_SCALE
	uint8_t cohesionOkFlag;  //凝聚完成标记，0：未完成；1：完成
	uint8_t cohesionPntNum;  //凝聚点数
	uint8_t cohesionBeamNum; //凝聚波位个数
	uint16_t aziBeamID;      //方位波束号，0~29 共30个，对应 -58°：4°：58°
	uint16_t eleBeamID;      //俯仰波束号，0~3 共4个，对应 -15°：10°：15°
	uint16_t reserve[4];     //备用，默认为0
} protocol_object_item_detected_t;

typedef struct protocol_object_list_detected {
	protocol_info_head_t stInfoHeader;

	uint16_t detectObjNum; /* 检测目标个数，最大目标个数64 */
	uint16_t detectObjByte; /*单个检测目标长度，其长度由检测点信息定义决定 */
	uint16_t trackTwsTasFlag; /* 跟踪标识，0：TWS跟踪；1：TAS跟踪 */
	uint16_t waveType;	/* the type of wave, 0-A frame, 1-B frame */

	protocol_object_item_detected_t detectPoint[MAX_NUM_DETECTS];

	protocol_info_tail_t stInfoTail;
} protocol_object_list_detected_t;

typedef struct protocol_object_item_tracked {
	uint16_t id;
	int16_t azimuth;          //INV_ONE6_SCALE
	uint32_t range;           //INV_ONE6_SCALE
	int16_t elevation;        //INV_ONE6_SCALE
	int16_t velocity;         //目标径向速度m/s INV_ONE6_SCALE
	int16_t dopplerChn;       //目标多普勒号
	uint16_t mag;             //目标幅度值 INV_ONE6_SCALE
	uint8_t ambiguous;        //0速度无模糊 1速度存在模糊
	uint8_t classification;   //0未识别 1无人机 2单兵 3车辆 4鸟类 5直升机 其它无效
	uint8_t classifyProb;     //目标类别概率 INV_ONE6_SCALE
	uint8_t existingProb;     //目标存在概率 INV_ONE6_SCALE
	int16_t absVel;           //目标位移速度m/s INV_ONE6_SCALE
	int16_t orientationAngle; //目标航向,目标运动方向与真北夹角（°）INV_ONE6_SCALE
	uint16_t alive;           //目标周期数
	uint16_t twsTasFlag;      //跟踪标识，0：TWS跟踪；1：TAS跟踪

	int32_t x;                //x相对坐标（m）INV_ONE6_SCALE
	int32_t y;                //y相对坐标（m）INV_ONE6_SCALE
	int32_t z;                //z相对坐标（m）INV_ONE6_SCALE

	int16_t vx;               //x方向相对速度（m/s）INV_ONE6_SCALE
	int16_t vy;               //y方向相对速度（m/s）INV_ONE6_SCALE
	int16_t vz;               //z方向相对速度（m/s）INV_ONE6_SCALE
	int16_t ax;               //x方向相对加速度（m/s²）INV_ONE6_SCALE
	int16_t ay;               //y方向相对加速度（m/s²）INV_ONE6_SCALE
	int16_t az;               //z方向相对加速度（m/s²）INV_ONE6_SCALE

	uint16_t x_variance;  //x相对坐标方差 INV_ONE6_SCALE
	uint16_t y_variance;  //y相对坐标方差 INV_ONE6_SCALE
	uint16_t z_variance;  //z相对坐标方差 INV_ONE6_SCALE
	uint16_t vx_variance; //x方向相对速度方差 INV_ONE6_SCALE
	uint16_t vy_variance; //y方向相对速度方差 INV_ONE6_SCALE
	uint16_t vz_variance; //z方向相对速度方差 INV_ONE6_SCALE
	uint16_t ax_variance; //x方向相对加速度方差 INV_ONE6_SCALE
	uint16_t ay_variance; //y方向相对加速度方差 INV_ONE6_SCALE
	uint16_t az_variance; //z方向相对加速度方差 INV_ONE6_SCALE

	uint8_t stateType;   //目标航迹类型，0：暂态航迹；1：稳态航迹
	uint8_t motionType;  //运动类型 0未知 1静止 2悬停 3靠近 4远离

	uint16_t forcastFrameNum; //目标预测帧数
	uint16_t associationNum;  //目标关联的检测点个数
	uint32_t assocBit0;       //关联检测点ID
	uint32_t assocBit1;       //关联检测点ID
	uint16_t reserve[4];      //备用
} protocol_object_item_tracked_t;

typedef struct protocol_object_list_tracked {
	protocol_info_head_t stInfoHeader;

	uint16_t trackObjNum;
	uint16_t trackTwsNum;
	uint16_t trackTasNum;
	uint16_t trackObjByte;
	uint16_t trackTwsTasFlag; //0 TWS跟踪  1 TAS跟踪
	uint16_t reserved;

	protocol_object_item_tracked_t trackPoint[MAX_NUM_TRAJS];

	protocol_info_tail_t stInfoTail;
} protocol_object_list_tracked_t;

typedef struct protocol_object_item_dot_cohe {
	uint16_t id;
	int16_t  azimuth;				//目标方位角（°）
	uint32_t range;         		//目标距离加权结果（m）
	int16_t  elevation;				//目标俯仰角（°）
	int16_t	 velocity;				//目标径向运动速度（m/s）
	int32_t	 x;						//x相对坐标（m）
	int32_t	 y;						//y相对坐标（m）
	int32_t	 z;						//z相对坐标（m）
	uint16_t mag;					//目标幅度值(dB)
	uint16_t snr;					//目标信噪比(dB)
	uint16_t rcs;					//目标反射截面积RCS(dB)
	int16_t  sinAzim;				//目标方位角正弦值
	int16_t	 cosAzim;				//目标方位角余弦值
	int16_t	 unambigVel;			//目标无模糊径向运动速度（m/s）
	uint16_t classification;		//0未识别 1无人机 2单兵 3车辆 4鸟类 5直升机 其它无效
	int16_t  aziBeamSin;			//当前波位扫描方位角正弦值
	int16_t  eleBeamSin;			//当前波位扫描俯仰角余弦值
	uint16_t waveType;				//波形编码，0：A帧（PRI-235us）；1：B帧（PRI-265us）
	uint8_t  flag_moving;			//目标运动标志字
	uint8_t  unambigVelDone;		//目标是否完成速度解模糊的标志字：1-是，0-否
	uint16_t detectionNum;			//点迹凝聚输出目标包含的凝聚检测点个数
	uint16_t detectionId[16];		//点迹凝聚输出目标包含的凝聚检测点ID，包含POINTS_IN_CLUSTER_NUM（=16）个uint16的值
	uint32_t detectionFrameId[16];	//点迹凝聚输出目标包含的凝聚检测点帧号，包含POINTS_IN_CLUSTER_NUM（=16）个uint32的值
	int16_t  MagBeamList[4];		//目标测方位角和俯仰角的2个点迹幅值（dB）
	int16_t  aziBeamList[2];		//目标测方位角的2个点迹方位角信息（°）
	int16_t  eleBeamList[2];		//目标测方位角的2个点迹俯仰角信息（°）
	uint8_t  ifClacAzi;				//目标是否完成测方位角的标志字
	uint8_t  ifClacEle;				//目标是否完成测俯仰角的标志字
	uint16_t reserve[3];			//备用，默认为0
} protocol_object_item_dot_cohe_t;

typedef struct protocol_object_list_dot_cohe {
	protocol_info_head_t stInfoHeader;

	uint16_t dotCoheObjNum;		//点迹凝聚目标个数，最大目标个数30（MaxObjNum）
	uint16_t dotCoheObjByte;	//单个点迹凝聚目标数据长度，由凝聚点数据定义决定
	uint16_t tasTargetId;		//TAS跟踪目标ID，TWS模式默认为-1；
	uint8_t  scanType;			//跟踪标识，0：TWS跟踪；1：TAS跟踪
	uint8_t  condceDone;		//点迹凝聚完成标识，0：未完成；1：已完成

	protocol_object_item_dot_cohe_t dotCohePoint[TWS_DOT_COHESION_NUM];

	protocol_info_tail_t stInfoTail;
} protocol_object_list_dot_cohe_t;
#pragma pack()

ret_code_t protocol_send_object_list_detected(protocol_object_list_detected_t *obj_list);
ret_code_t protocol_send_object_list_tracked(protocol_object_list_tracked_t *obj_list);
ret_code_t protocol_send_object_list_dot_cohe(protocol_object_list_dot_cohe_t *obj_list);

#endif /* PROTOCOL_OBJECT_H */
