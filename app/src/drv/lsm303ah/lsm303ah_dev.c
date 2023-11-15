

#include <stdint.h>
#include <stdlib.h>

#include "lsm303ah_dev.h"

void get_acc_mag_data( int32_t *acc_buf , int32_t *mag_buf )
{
//	get_acc_mag_data_rpc_m(acc_buf, mag_buf);
}

void Lsm303ah_init()
{
//	Lsm303ah_init_rpc_m();
}

void Acc_init(void)
{
//	Acc_init_rpc_m();
}

void Mag_init(void)
{
//	Mag_init_rpc_m();
}

ret_code_t LSM303AH_Reg_Read(uint8_t reg,  uint8_t *data)
{
	ret_code_t ret = RET_OK;

//	ret = LSM303AH_Reg_Read_rpc_m(reg, data);

	return ret;
}

ret_code_t LSM303AH_Reg_Write(uint8_t reg,  uint8_t data)
{
	ret_code_t ret = RET_OK;

///	ret = LSM303AH_Reg_Write_rpc_m(reg, data);

	return ret;
}

ret_code_t ACC_sensor_self_test(uint16_t *acc_abs)
{
	ret_code_t ret = RET_OK;

//	ret = ACC_sensor_self_test_rpc_m(acc_abs);

	return ret;
}

ret_code_t MAG_sensor_self_test(uint16_t *mag_abs)
{
	ret_code_t ret = RET_OK;

//	ret = MAG_sensor_self_test_rpc_m(mag_abs);

	return ret;
}




