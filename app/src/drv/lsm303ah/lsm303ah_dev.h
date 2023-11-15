
#ifndef LSM303AH_DEV_H
#define LSM303AH_DEV_H


#include <stdint.h>
#include "../../inc/radar_error.h"

#define IS_LSM303AH_REG_ADDR_VALID(val) (((val) >= 0) && ((val) <= 0x6F))

void Lsm303ah_init();
void get_acc_mag_data(int32_t *acc_buf, int32_t *mag_buf);
void Acc_init(void);
void Mag_init(void);
ret_code_t LSM303AH_Reg_Read(uint8_t reg,  uint8_t *data);
ret_code_t LSM303AH_Reg_Write(uint8_t reg,  uint8_t data);
ret_code_t ACC_sensor_self_test(uint16_t *acc_abs);
ret_code_t MAG_sensor_self_test(uint16_t *mag_abs);


#endif

