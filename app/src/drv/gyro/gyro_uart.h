
/**
 * @file gyro_uart.h
 * @brief gyro driver.
 *		
 * 
 * @author tangsongquan 
 * @date 2023.09.26
 * @see 
 */
#ifndef __GYRO_UART_H__
#define __GYRO_UART_H__

#include <stdint.h>
#include "../../inc/radar_error.h"
#include "../../inc/radar_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct gyro_angle_info
{
	float roll;
	float pitch;
	float yaw;
}gyro_angle_info_t;

/**
 * @brief gyro init func.
 *
 * 
 * @param [in] dev      dev file name,like /dev/ttyXXX
 * @param [in] ev_loop  libev instance pointer.  
 *
 * @return the pointer of gyro object.
 * @retval NULL  		init fail
 *         not NULL  	the pointer of gyro object.
 *
 * @see 
 * @note 
 * @warning 
 */
int gyro_init(void);

/**
 * @brief get gyro angle info
 *
 * 
 * @param [out] angleinfo	 the output angleinfo
 * @param [in] is_need_realtime_data	 1: should be realtime data;  0: the last updated data, maybe already timeout for a long time.	
 *
 * @return 
 * @retval 0		ok ,got info
 *		   <0		fail to get info.
 *
 * @see 
 * @note 
 * @warning 
 */
int gyro_get_angle_info(gyro_angle_info_t *angleinfo, int is_need_realtime_data);


void Gyro_Init();
void Get_gyro_date(int buff[]);
ret_code_t Gyro_Reg_Read(uint8_t reg,  uint8_t *data);
ret_code_t Gyro_Reg_Write(uint8_t reg,  uint8_t data);

#ifdef __cplusplus
}
#endif

#endif

