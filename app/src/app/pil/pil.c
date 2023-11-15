/**
* file: pil.c
* author: tangsongquan
* brief
* 		pil driver file.
*/
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "pil.h"
#include "../DataPath/data_path.h"

#define PIL_PACKAGE_INFO_SIZE 128
#define PIL_PACKAGE_ADC_SIZE  1024*512
#define PIL_PACKAGE_SIZE (PIL_PACKAGE_INFO_SIZE + PIL_PACKAGE_ADC_SIZE)//ADC + HEAD INFO

typedef struct pil_info
{
	volatile bool 		bFlagOpen;
	volatile bool 		bFlagInject;
	uint8_t 			awBuffer[PIL_PACKAGE_SIZE];
	uint8_t 			awAdcHeader[PIL_PACKAGE_INFO_SIZE];
	uint32_t 			uOffset;
	uint32_t			uCrc;
	uint32_t 			uSize;
}pil_info_t;

static pil_info_t sHilInfo = { 0 };

bool get_pil_status( void )
{
	return sHilInfo.bFlagOpen;
}

static void set_pil_inject_status(bool status)
{
	uint32_t uCount = sHilInfo.uSize - sHilInfo.uOffset;
	
	if(!status) {
		sHilInfo.uOffset += uCount;
	}

	sHilInfo.bFlagInject = status;
}

uint8_t pil_inject_status( void )
{
	if( sHilInfo.bFlagInject )
	{
		return 0;
	}
	else
	{
		if( sHilInfo.bFlagInject )
			return 0;
		else
			return 1;
	}
}

static void inject_data_prepare(data_path_pil_data *dppd, pil_info_t *spinfo)
{
	if(!dppd || ! spinfo) {
		return;
	}

	dppd->pilinfodat = spinfo->awAdcHeader;
	dppd->pilinfodat_len = PIL_PACKAGE_INFO_SIZE;
	dppd->piladcdat = spinfo->awBuffer + PIL_PACKAGE_INFO_SIZE;
	dppd->piladcdat_len = PIL_PACKAGE_ADC_SIZE;
	dppd->frameid = 0;//not used currently 

	return;
}

void pil_inject_adc( uint8_t *pbyBuf, uint32_t uSize )
{
	uint32_t uCount = sHilInfo.uSize - sHilInfo.uOffset;
	if( uCount > uSize )
	{
		uSize /= 2;
		memcpy( pbyBuf, ((uint8_t*)(sHilInfo.awBuffer) + sHilInfo.uOffset), 2*uSize );
		memset( pbyBuf, 0, 20 );
		memset( pbyBuf + 2*uSize - 12, 0, 12 );
		sHilInfo.uOffset += 2*uSize;
	}
	else
	{
		uSize /= 2;
		memcpy( pbyBuf, ((uint8_t*)(sHilInfo.awBuffer) + sHilInfo.uOffset), 2*uSize );
		memset( pbyBuf, 0, 20 );
		memset( pbyBuf + 2*uSize - 12, 0, 12 );
		sHilInfo.uOffset += uCount;
		sHilInfo.bFlagInject = false;
	}
}

void pil_rdmap_header( void *pRdmap )
{
	if( sHilInfo.bFlagOpen )
	{
		memcpy( pRdmap, sHilInfo.awAdcHeader, sizeof(sHilInfo.awAdcHeader) );
	}
}

extern volatile bool bFlagPlEn;
ret_code_t pil_set_open( uint8_t *pErrCode )
{
	uint8_t errCode;
	ret_code_t eRet = RET_OK;

	do{
		if( sHilInfo.bFlagInject )
		{
			errCode = 3;
			break;
		}

		if( sHilInfo.bFlagOpen )
		{
			errCode = 2;
			break;
		}
		sHilInfo.bFlagOpen = true;
		errCode = 0;
		
		if(data_path_pil_init(set_pil_inject_status) != 0) {
			APP_LOG_INFO("warmming!!! data_path pil init fail\n");
		} 

	}while(0);

	if( pErrCode )
		*pErrCode = errCode;

	return eRet;
}

ret_code_t pil_set_close( uint8_t *pErrCode )
{
	uint8_t errCode;
	ret_code_t eRet = RET_OK;

	do{
		if( sHilInfo.bFlagInject )
		{
			errCode = 3;
			break;
		}

		if( false == sHilInfo.bFlagOpen )
		{
			errCode = 2;
			break;
		}
		errCode = 0;
		eRet = data_path_pil_uinit();
		sHilInfo.bFlagOpen = false; 
	}while(0);

	if( pErrCode )
		*pErrCode = errCode;

	return eRet;
}

ret_code_t pil_start_transmit( pil_header_t *psHeader, uint8_t *pErrCode )
{
	uint8_t errCode;
	ret_code_t eRet = RET_OK;

	do{
		if( false == sHilInfo.bFlagOpen )
		{
			errCode = 5;
			break;
		}

		if( sizeof(sHilInfo.awBuffer) < psHeader->uDataSize )
		{
			errCode = 2;
			break;
		}

		if( sHilInfo.bFlagInject )
		{
			errCode = 3;
			break;
		}

		sHilInfo.uOffset = 0;
		sHilInfo.uCrc = psHeader->uDataCrc;
		sHilInfo.uSize = psHeader->uDataSize;
		errCode = 0;
	}while(0);

	if( pErrCode )
		*pErrCode = errCode;

	return eRet;
}

ret_code_t pil_transmit_data( uint8_t byStatus, uint32_t uOffset, uint8_t* pbyData, uint32_t uLength, uint8_t *pErrCode )
{
	uint8_t errCode = 0;
	ret_code_t eRet = RET_OK;
	data_path_pil_data dppd = {0};
	
	do{
		
		if( false == sHilInfo.bFlagOpen )
		{
			errCode = 5;
			break;
		}

		if( sHilInfo.bFlagInject || check_datapath_pil_status() != DATA_PATH_PIL_STATUS_DONE )
		{
			errCode = 3;
			break;
		}

		if( 0xFFFF == uLength )
		{
			uLength = PIL_PACKAGE_SIZE;
		}

		if( byStatus )
		{
			errCode = 5;
			if( sHilInfo.uOffset < uOffset + uLength )
			{
				errCode = 2;
			}
			else
			{
				memcpy( (uint8_t*)(sHilInfo.awBuffer) + uOffset, pbyData, uLength );
				errCode = 0;
			}
		}
		else
		{
			if( sHilInfo.uOffset < uOffset )
			{
				errCode = 2;
			}
			else if( sHilInfo.uOffset > uOffset )
			{
				errCode = 1;
			}
			if( sizeof(sHilInfo.awBuffer) < sHilInfo.uOffset + uLength )
			{
				errCode = 4;
			}
			else
			{
				memcpy( (uint8_t*)(sHilInfo.awBuffer) + sHilInfo.uOffset, pbyData, uLength );
				sHilInfo.uOffset += uLength;
				errCode = 0;
				if( PIL_PACKAGE_SIZE == sHilInfo.uOffset )
				{
					memcpy( sHilInfo.awAdcHeader, sHilInfo.awBuffer, sizeof(sHilInfo.awAdcHeader) );
					sHilInfo.bFlagInject = true;
					sHilInfo.uSize = sHilInfo.uOffset;
					sHilInfo.uOffset = 0;
					inject_data_prepare(&dppd, &sHilInfo);			
					eRet = data_path_set_pil_data(&dppd);
					if(0 != eRet) {
						sHilInfo.bFlagInject = false;
						sHilInfo.uOffset = sHilInfo.uSize;
					}
				}
			}
		}
	}while(0);

	if( pErrCode )
		*pErrCode = errCode;

	return eRet;
}

ret_code_t pil_inject_data( uint8_t *pErrCode )
{
	uint8_t errCode;
	ret_code_t eRet = RET_OK;

	if( sHilInfo.bFlagInject )
	{
		errCode = 1;
	}
	else if( sHilInfo.uSize != sHilInfo.uOffset )
	{
		errCode = 3;
	}
	else if( false == sHilInfo.bFlagOpen )
	{
		errCode = 4;
	}
	else
	{
		errCode = 0;
		sHilInfo.bFlagInject = true;
		sHilInfo.uOffset = 0;
	}

	if( pErrCode )
		*pErrCode = errCode;

	return eRet;
}

uint8_t pil_read_status( void )
{
	uint8_t byStatus;

	if( sHilInfo.bFlagInject )
	{
		byStatus = 0;
	}
	else
	{
		byStatus = 1;
	}

	return byStatus;
}

uint32_t pil_read_data_offset( void )
{
	return sHilInfo.uOffset;
}
