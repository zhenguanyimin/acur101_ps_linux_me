/**
 * @file gyro_uart.c
 * @brief gyro driver.
 *
 * 
 * @author tangsongquan 
 * @date 2023.09.26
 * @see 
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>

#include "gyro_uart.h"
#include "../uart/common_uart.h"
#include "../../srv/log/log.h"

#define DEBUG_UNITTEST  1
#define GYRO_READ_DATA_TIMEOUT 10	//ms

#if 0
#define DEBUG_PRINTF_MSG_DATA_ON
#define DEBUG_PRINTF(arg...) printf(arg)
#else
#define DEBUG_PRINTF(arg...) 
#endif

typedef struct SAngle
{
	int16_t Angle[3];
	int16_t T;
}SAngle_t;

typedef struct gyro_res {
	int fd;
	pthread_mutex_t mutex;
	bool is_inited;
}gyro_res_t;

#ifdef __cplusplus
extern "C" {
#endif

#define SENSOR_ACCELERATION_TYPE    0x51
#define SENSOR_ANGULAR_TYPE         0x52
#define SENSOR_ANGLE_HEAD    		0x53
#define SENSOR_COMPASS_HEAD    		0x54
#define GYRO_MSG_MAGIC     0x55
#define GYRO_MSG_LEN                        (11)
static gyro_res_t *sg_gyro = NULL;

/**
 * @brief check gyro msg integrity.
 *   
 * @param [in]   data            msg addr.
 * @param [in]   data_len        msg len.     
 *
 * @return check result
 * @retval <0  msg error;  
 *         0   msg ok.
 *
 * @see 
 * @note 
 * @warning 
 */
static int _gyro_msg_check(char *data,int data_len)
{
	uint8_t sum = 0x00;
	int i;
	//check magic.
	if(data_len < GYRO_MSG_LEN || data[0] != GYRO_MSG_MAGIC){
		return -1;
	}

	//check crc
	for(i = 0; i < (GYRO_MSG_LEN - 1); i++){
		sum += (uint8_t)data[i];
	}
	if(sum != data[GYRO_MSG_LEN - 1]){
		return -1;
	}
	return 0;
}

/**
 * @brief sync read data
 *   在此处理角度和指南针
 *	 Sensor上传了4类数据，且每一类数据长度都是11.
 *	 Type 0x50  时间; 0x51 加速度；Type 0x52 角速度；Type 0x53 角度；Type 0x54 磁场；
 *   数据报头是以0x55 开头，紧接着是类型...，最后一个是累加校验合；
 *   |0x55|  data 9bytes(1bytes Type + 8Bytes data) | 累加校验和1字节|
 * 
 *   如果接收一条完整消息的时间，超过GRYO_MSG_DROP_TIMEOUT_MS，则认为之前缓存的数据不全，需要丢弃。
 *   55 54 CE E9 BA 07 7D FF 00 00 9D
 *   55 53 3C F7 5E 08 56 CD E6 46 90
 *   55 52 09 00 00 00 00 00 9A 13 5D
 *   55 51 5A FE 50 FE A9 07 9A 13 A9 
 *   
 * @param [in]   cb resource pointer, target_magic and target_type 
 * @param [out]  tpac: readback package 
 *
 * @return 
 * @retval 
 *			0: success, other: fail
 * @see 
 * @note 
 * @warning 
 */
int gyro_get_target_packet(gyro_res_t *cb, char *tpac, char target_magic, char target_type)
{
	int rlen = 0, ret = -1;
	char gyro_buf[GYRO_MSG_LEN + 1] = {0};
	char *gyro_p = gyro_buf;
	int offset = 0;
	int i=0, dest_len = GYRO_MSG_LEN;
	bool need_check_head = true;
	int timeout_times = GYRO_READ_DATA_TIMEOUT;

	do {
		pthread_mutex_lock(&cb->mutex);
		rlen = common_uart_read(cb->fd, gyro_p, dest_len, GYRO_READ_DATA_TIMEOUT);
		pthread_mutex_unlock(&cb->mutex);
		if(rlen <= 0) {
			APP_LOG_ERROR("read uart fail, ret = %d\n", rlen);
			return rlen;
		}
		if(need_check_head) {
			if(rlen == 1) {
				gyro_p = gyro_buf + 1;
				continue;
			} else {
				gyro_p = gyro_buf;
			}

			for(i=0; i<rlen; i++) {
				if(gyro_p[i] == target_magic && gyro_p[i + 1] == target_type) {
					memcpy(tpac + offset, gyro_buf, rlen - i);
					need_check_head = false;
					break;
				}
			}
			offset += (rlen - i);
			
		} else {
			memcpy(tpac + offset, gyro_p, rlen);
			offset += rlen;
		}	

		dest_len = GYRO_MSG_LEN - offset;
		if(dest_len == GYRO_MSG_LEN) { // timeout counter, read GYRO_READ_DATA_TIMEOUT times as timeout.
			timeout_times --;
		}
		if(offset >= GYRO_MSG_LEN) {
			ret = _gyro_msg_check(tpac, GYRO_MSG_LEN);
			break;
		}
		
	}while(timeout_times > 0);

	return ret;
}

/**
 * @brief get gyro angle info
 * 
 * @param [out] angleinfo    the output angleinfo
 * @param [in] is_need_realtime_data     1: should be realtime data;  0: the last updated data, maybe already timeout for a long time.  
 *
 * @return 
 * @retval 0  		ok ,got info
 *         <0  	    fail to get info.
 * @see 
 * @note 
 * @warning 
 */
int gyro_get_angle_info(gyro_angle_info_t *angleinfo, int is_need_realtime_data)
{
	gyro_res_t *cb = sg_gyro;	
	int ret = -1;
	char gyro_buf[GYRO_MSG_LEN + 1] = {0};
	SAngle_t angle = {0};
	float l_roll = 0, l_pitch = 0, l_yaw = 0;

	if(!angleinfo || NULL == cb || !cb->is_inited){
		return RET_INVALID_PARAM;
	}

	ret = gyro_get_target_packet(cb, gyro_buf, GYRO_MSG_MAGIC, SENSOR_ANGLE_HEAD);
	if(0 != ret) {
		APP_LOG_INFO("get dest packet %02x,%02x fail, ret %d\n", GYRO_MSG_MAGIC, SENSOR_ANGLE_HEAD, ret);
		return ret;
	}

	angle = *(SAngle_t*)&gyro_buf[2];
	//转换为角度
	l_roll = (float)angle.Angle[0];
	l_roll = l_roll / 32768 * 180 ;
	l_pitch = (float)angle.Angle[1];
	l_pitch = l_pitch / 32768 * 180 ;
	l_yaw = (float)angle.Angle[2];
	l_yaw = l_yaw / 32768 * 180 ;
	angleinfo->roll = l_roll ;
	angleinfo->pitch = l_pitch ;
	angleinfo->yaw = l_yaw ;
	ret = 0;

	return ret;
}

/**
 * @brief gyro init func.
 * @param none
 *
 * @return initialization result.
 * @retval not 0  		init fail
 *         0  			init success.
 * @see 
 * @note 
 * @warning 
 */
int gyro_init(void)
{	
	int ret = 0;

	sg_gyro = malloc(sizeof(gyro_res_t));
	if(NULL == sg_gyro) {
		ret = RET_SYSTEM_ERR;
		goto err;
	}
	ret = common_uart_init(GYRO_UART_DEV, NULL);//use default uart configuration
	if(ret < 0) {
		goto err;
	}
	sg_gyro->fd = ret;
	pthread_mutex_init(&sg_gyro->mutex, NULL); 
	sg_gyro->is_inited = true;
	APP_LOG_DEBUG("gyro init sucessfully, is_inited=%d\n", sg_gyro->is_inited);

	return 0;
err:
	common_uart_uninit(sg_gyro->fd);
	if(sg_gyro) {
		free(sg_gyro);
		sg_gyro = NULL;
	}
	return ret;	
}

/**
 * @func gyro_uninit
 * @brief release gyro resources.
 * @param none
 *
 * @return initialization result.
 * @retval not 0  		init fail
 *         0  			init success.
 * @see 
 * @note 
 * @warning 
 */
int gyro_uninit(void)
{
	int ret = 0;

	if(sg_gyro) {
		ret = common_uart_uninit(sg_gyro->fd);
		free(sg_gyro);
		sg_gyro = NULL;
	}

	return ret;
}

void Gyro_Init()
{
//	Gyro_Init_rpc_m();
}

void Get_gyro_date(int buff[])
{
//	Get_gyro_date_rpc_m(buff);
}

ret_code_t Gyro_Reg_Read(uint8_t reg,  uint8_t *data)
{
	ret_code_t ret = RET_OK;

//	ret = Gyro_Reg_Read_rpc_m(reg, data);

	return ret;
}

ret_code_t Gyro_Reg_Write(uint8_t reg, uint8_t data)
{
	ret_code_t ret = RET_OK;

//	ret = Gyro_Reg_Write_rpc_m(reg, data);

	return ret;
}

#ifdef __cplusplus
}
#endif

