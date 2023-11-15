
#include "ut_soc.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* FreeRTOS+CLI includes. */
#include "../../srv/cli/cli_if.h"
// #include "../../srv/flash_nv/flash_nv.h"
#include "../../app/beam_calculate/bk_cal.h"

static BaseType_t bk_cal_info_get_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t bk_cal_info_erase_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t bk_cal_info_store_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);


static const CLI_Command_Definition_t bk_cal_info_get_cmd =
{
	"BkCalGet",
	"\r\nBkCalGet :\r\n Get Bk Cal\r\n",
	bk_cal_info_get_cmd_handler,
	0
};
static const CLI_Command_Definition_t bk_cal_info_erase_cmd =
{
	"BkCalErase",
	"\r\nBkCalErase :\r\n erase Bk Cal\r\n",
	bk_cal_info_erase_cmd_handler,
	0
};
static const CLI_Command_Definition_t bk_cal_info_store_cmd =
{
	"BkCalStore",
	"\r\nBkCalStore :\r\n Store Bk Cal\r\n",
	bk_cal_info_store_cmd_handler,
	0
};

static BaseType_t bk_cal_info_get_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{

	uint16_t l_Amp_Code[12];
	int16_t l_amp_phase[384];

	size_t len = 0x00 ;
	static uint8_t step = 0x00 ;
	static uint8_t loop = 0x00 ;
	BaseType_t status = pdTRUE ;

	// switch( step )
	// {
	// 	case 0x00 :
	// 	{
	// 		FlashNV_Get( FLASH_NV_ID_AMP_CODE , l_Amp_Code, sizeof(l_Amp_Code) , &len );
	// 		if( len <= 0x00 )
	// 		{
	// 			snprintf( pcWriteBuffer, xWriteBufferLen,"\rERROR : FLASH_NV_ID_AMP_CODE len <= 0x00 ;\r\n");
	// 		}
	// 		else
	// 		{
	// 			snprintf( pcWriteBuffer, xWriteBufferLen,
	// 					"\rtx_com_Amp_Code[4]	= { 0x%x,	0x%x,	0x%x,	0x%x };\r\n"
	// 					"\rx_u19_Amp_Code[4]	= { 0x%x,	0x%x,	0x%x,	0x%x };\r\n"
	// 					"\rx_u23_Amp_Code[4]	= { 0x%x,	0x%x,	0x%x,	0x%x };\r\n",
	// 					(int)l_Amp_Code[0],(int)l_Amp_Code[1],(int)l_Amp_Code[2],(int)l_Amp_Code[3],
	// 					(int)l_Amp_Code[4],(int)l_Amp_Code[5],(int)l_Amp_Code[6],(int)l_Amp_Code[7],
	// 					(int)l_Amp_Code[8],(int)l_Amp_Code[9],(int)l_Amp_Code[10],(int)l_Amp_Code[11]
	// 					);
	// 		}
	// 		step = 0x01;
	// 		break;
	// 	}
	// 	case 0x01 :
	// 	{
	// 		FlashNV_Get( FLASH_NV_ID_PHASE_DATA , l_amp_phase, sizeof(l_amp_phase) , &len );
	// 		if( len <= 0x00 )
	// 		{
	// 			snprintf( pcWriteBuffer, xWriteBufferLen,"\rERROR : FLASH_NV_ID_PHASE_DATA len <= 0x00 ;\r\n");
	// 			step = 0x03;
	// 		}
	// 		else
	// 		{
	// 			memcpy( INIT_PHASE_DATA , &l_amp_phase[0] , sizeof(l_amp_phase) );
	// 			snprintf( pcWriteBuffer, xWriteBufferLen,"\rINIT_PHASE_DATA :\r\n");
	// 			step = 0x02;
	// 		}
	// 		loop = 0x00;
	// 		break;
	// 	}
	// 	case 0x02 :
	// 	{
	// 		if( loop < sizeof(INIT_PHASE_DATA)/2/8 )
	// 		{
	// 			snprintf( pcWriteBuffer, xWriteBufferLen,
	// 					"\r %d	%d	%d	%d	%d	%d	%d	%d	\r\n",
	// 					INIT_PHASE_DATA[loop*8+0],
	// 					INIT_PHASE_DATA[loop*8+1],
	// 					INIT_PHASE_DATA[loop*8+2],
	// 					INIT_PHASE_DATA[loop*8+3],
	// 					INIT_PHASE_DATA[loop*8+4],
	// 					INIT_PHASE_DATA[loop*8+5],
	// 					INIT_PHASE_DATA[loop*8+6],
	// 					INIT_PHASE_DATA[loop*8+7]
	// 					);
	// 			vTaskDelay( 100 );
	// 			loop++;
	// 		}
	// 		else
	// 		{
	// 			loop = 0x00;
	// 			step = 0x03;
	// 		}
	// 		break;
	// 	}
	// 	case 0x03 :
	// 	{
	// 		FlashNV_Get( FLASH_NV_ID_AMP_DATA , l_amp_phase, sizeof(l_amp_phase) , &len );
	// 		if( len <= 0x00 )
	// 		{
	// 			snprintf( pcWriteBuffer, xWriteBufferLen,"\rERROR : FLASH_NV_ID_AMP_DATA len <= 0x00 ;\r\n");
	// 			step = 0x00;
	// 			status = pdFALSE;
	// 		}
	// 		else
	// 		{
	// 			memcpy( INIT_AMP_DATA , &l_amp_phase[0] , sizeof(l_amp_phase) );
	// 			snprintf( pcWriteBuffer, xWriteBufferLen,"\r INIT_AMP_DATA :\r\n");
	// 			step = 0x04;
	// 			status = pdTRUE;
	// 		}
	// 		loop = 0x00;
	// 		break;
	// 	}
	// 	case 0x04 :
	// 	{
	// 		if( loop < sizeof(INIT_AMP_DATA)/2/8 )
	// 		{
	// 			snprintf( pcWriteBuffer, xWriteBufferLen,
	// 					"\r %d	%d	%d	%d	%d	%d	%d	%d	\r\n",
	// 					INIT_AMP_DATA[loop*8+0],
	// 					INIT_AMP_DATA[loop*8+1],
	// 					INIT_AMP_DATA[loop*8+2],
	// 					INIT_AMP_DATA[loop*8+3],
	// 					INIT_AMP_DATA[loop*8+4],
	// 					INIT_AMP_DATA[loop*8+5],
	// 					INIT_AMP_DATA[loop*8+6],
	// 					INIT_AMP_DATA[loop*8+7]
	// 					);
	// 			vTaskDelay( 100 );
	// 			loop++;
	// 		}
	// 		else
	// 		{
	// 			loop = 0x00;
	// 			step = 0x00;
	// 			status = pdFALSE ;
	// 		}
	// 		break;
	// 	}
	// 	default:
	// 		step = 0x00;
	// 		loop = 0x00;
	// 		status = pdFALSE ;
	// 		break;
	// }

	return status;
}

static BaseType_t bk_cal_info_erase_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{

	// FlashNV_Del( FLASH_NV_ID_AMP_CODE);
	// FlashNV_Del( FLASH_NV_ID_AMP_DATA);
	// FlashNV_Del( FLASH_NV_ID_PHASE_DATA);

	snprintf( pcWriteBuffer, xWriteBufferLen,"\r OK \r\n");
	return pdFALSE;
}

static BaseType_t bk_cal_info_store_cmd_handler(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString)
{

	uint16_t l_Amp_Code[12];

	// memcpy( &l_Amp_Code[0] , tx_com_Amp_Code , sizeof(tx_com_Amp_Code) );
	// memcpy( &l_Amp_Code[4] , rx_u19_Amp_Code , sizeof(rx_u19_Amp_Code) );
	// memcpy( &l_Amp_Code[8] , rx_u23_Amp_Code , sizeof(rx_u23_Amp_Code) );

	// FlashNV_Set( FLASH_NV_ID_AMP_CODE , l_Amp_Code, sizeof(l_Amp_Code) );

	// FlashNV_Set( FLASH_NV_ID_PHASE_DATA , INIT_PHASE_DATA, sizeof(INIT_PHASE_DATA) );

	// FlashNV_Set( FLASH_NV_ID_AMP_DATA , INIT_AMP_DATA, sizeof(INIT_AMP_DATA) );

	snprintf( pcWriteBuffer, xWriteBufferLen,"\r OK \r\n");

	return pdFALSE;
}
void ut_soc_init(void)
{
	FreeRTOS_CLIRegisterCommand(&bk_cal_info_get_cmd);
	FreeRTOS_CLIRegisterCommand(&bk_cal_info_erase_cmd);
	FreeRTOS_CLIRegisterCommand(&bk_cal_info_store_cmd);
}
