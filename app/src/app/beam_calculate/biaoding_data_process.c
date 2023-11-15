#include "biaoding_data_process.h"
#include "../../srv/flash_nv/flash_raw.h"

ret_code_t amp_code_store( uint16_t *data_ptr , uint16_t len )
{
	int ret = RET_OK;
	uint16_t l_Amp_Code[12];

	if( len == sizeof(tx_com_Amp_Code)*3 )
	{
		for( uint16_t i = 0x00 ; i < sizeof(l_Amp_Code)/2 ; i++ )
		{
			l_Amp_Code[i] = data_ptr[i] ;
		}
		memcpy( tx_com_Amp_Code , &l_Amp_Code[0] , sizeof(tx_com_Amp_Code) );
		memcpy( rx_u19_Amp_Code , &l_Amp_Code[4] , sizeof(rx_u19_Amp_Code) );
		memcpy( rx_u23_Amp_Code , &l_Amp_Code[8] , sizeof(rx_u23_Amp_Code) );
		ret = FlashRaw_Set( "AMP_CODE", l_Amp_Code, sizeof(l_Amp_Code) );
	}
	else
	{
		ret = RET_INVALID_PARAM;
	}

	return ret ;

}

ret_code_t amp_data_store( uint16_t *data_ptr , uint16_t len )
{
	int ret = RET_OK;

	if( len == sizeof(INIT_AMP_DATA) )
	{
		for( uint16_t i = 0x00 ; i < sizeof(INIT_AMP_DATA)/2 ; i++ )
		{
			INIT_AMP_DATA[i] = data_ptr[i] ;
		}
		ret = FlashRaw_Set( "AMP_DATA", INIT_AMP_DATA, sizeof(INIT_AMP_DATA) );
	}
	else
	{
		ret = RET_INVALID_PARAM;
	}

	return ret ;

}

ret_code_t phase_data_store( uint16_t *data_ptr , uint16_t len )
{
	int ret = RET_OK;

	if( len == sizeof(INIT_PHASE_DATA) )
	{
		for( uint16_t i = 0x00 ; i < sizeof(INIT_PHASE_DATA)/2 ; i++ )
		{
			INIT_PHASE_DATA[i] = data_ptr[i] ;
		}
		ret = FlashRaw_Set( "PHASE_DATA", INIT_PHASE_DATA, sizeof(INIT_PHASE_DATA) );
	}
	else
	{
		ret = RET_INVALID_PARAM;
	}

	return ret ;

}

ret_code_t get_amp_code_store( uint16_t *data_ptr, uint16_t *pwLen )
{
	size_t len = 0;
	memset( data_ptr, 0, 24 );
	FlashRaw_Get( "AMP_CODE", data_ptr, 24, &len );
	*pwLen = 24;
	if( 24 == len )
		return RET_OK;
	else
		return RET_GENERAL_ERR;
}

ret_code_t get_amp_data_store( uint16_t *data_ptr , uint16_t *pwLen )
{
	size_t len = 0;
	memset( data_ptr, 0, 768 );
	FlashRaw_Get( "AMP_DATA", data_ptr, 768, &len );
	*pwLen = 768;
	if( 768 == len )
		return RET_OK;
	else
		return RET_GENERAL_ERR;
}

ret_code_t get_phase_data_store( uint16_t *data_ptr , uint16_t *pwLen )
{
	size_t len = 0;
	memset( data_ptr, 0, 768 );
	FlashRaw_Get( "PHASE_DATA", data_ptr, 768, &len );
	*pwLen = 768;
	if( 768 == len )
		return RET_OK;
	else
		return RET_GENERAL_ERR;
}

