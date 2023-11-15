
#ifndef FIRMWARE_H
#define FIRMWARE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>


#define FIRMWARE_BUFFER_SIZE	(500*1024*1024)

#pragma pack(1)
	typedef struct firmware_header
	{
		uint32_t	headerLen;
		uint32_t	headerVersion;
		uint32_t	firmwareOffset;
		uint32_t	firmwareLength;
		uint32_t	firmwareCrc;
		uint32_t	corpInfoLen;
		uint8_t		corpInfo[32];
		uint32_t	devNameLen;
		uint8_t		devName[32];
		uint32_t	fwVersionLen;
		uint8_t		fwVersion[64];
		uint8_t		reserve[92];
		uint32_t	headCrc;
	} firmware_header_t;
#pragma pack()


bool firmware_set_upgrade_flag( bool bFlag );
bool firmware_check_upgrade_flag( void );

bool firmware_check( void );

bool firmware_recv_init( firmware_header_t *psFwHeader);
bool firmware_recv_data( uint32_t uOffset, uint8_t* pbyData, uint32_t uLen );
bool firmware_save(void);

#endif
