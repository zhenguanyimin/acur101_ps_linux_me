#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../sflog/sflog.h"
#include "table_2d.h"

#define TABLE_2D_FILE_ENTER		"\r\n"
#define TABLE_2D_FILE_DELIMITER "	, "

char* read_table_file( char* strFileName, uint32_t* puFileSize )
{
	char* strFileData = NULL;
	FILE* fp = NULL;
	
	do{
		int ret = access( strFileName, R_OK );
		if( 0 != ret )
		{
			sflog( SFLOG_LEVEL_WARNING, "file %s not find\n", strFileName);
			break;
		}
		
		FILE* fp = fopen( strFileName, "r" );
		if( NULL == fp )
		{
			sflog( SFLOG_LEVEL_ERR, "open %s fail\n", strFileName);
			break;
		}
		
		fseek( fp, 0L, SEEK_END );
		uint32_t filesize = ftell( fp );
		if( puFileSize )
			*puFileSize = filesize;
		if( 0 == filesize )
		{
			sflog( SFLOG_LEVEL_WARNING, "%s file size is 0\n", strFileName);
			break;
		}
		
		strFileData = (char *)malloc( filesize );
		if( !strFileData )
		{
			sflog( SFLOG_LEVEL_ERR, "table file malloc %d error\n", filesize );
			break;
		}
		
		fseek( fp, 0, SEEK_SET );
		ret = fread( strFileData, sizeof(char), filesize, fp );
		if( 0 >= ret )
		{
			sflog( SFLOG_LEVEL_ERR, "read %s error, ret = %d\n", strFileName, ret );
			break;
		}
		
		sflog( SFLOG_LEVEL_DEBUG, "read file data\n%s\n", strFileData );
		goto suc_exit;
	}while( 0 );

	if( strFileData )
	{
		free( strFileData );
		strFileData = NULL;
	}
	
suc_exit:
	if( fp )
	{
		fclose( fp );
	}

	return strFileData;
}

struct table_2d* traverse_table_file( char* strFileData, uint32_t uFileSize, uint32_t	uLineNum, uint32_t	uRowNum )
{
	do{
		if( uLineNum && uRowNum )
			break;

		char* strFileBuff = (char*)malloc( uFileSize );
		if( NULL == strFileBuff )
		{
			sflog( SFLOG_LEVEL_ERR, "table file malloc %d error\n", uFileSize );
			break;
		}
		memcpy( strFileBuff, strFileData, uFileSize );

		uint32_t uRowMax	= uRowNum;
		uint32_t uLineMax	= 0xFFFFFFFF;
		if( uLineNum )
			uLineMax = uLineNum;
		
		uint32_t uLineCount	= 0;
		char* strLineNext	= NULL;
		char* strLine		= strtok_r( strFileBuff, TABLE_2D_FILE_ENTER, &strLineNext );
		for( ; strLine && uLineMax > uLineCount; ++uLineCount )
		{
			if( 0 == uRowMax )
			{
				uint32_t uRowCount	= 0;
				char* strRowNext	= NULL;
				char* strRow		= strtok_r( strLine, TABLE_2D_FILE_DELIMITER, &strRowNext );
				for( ; strRow; ++uRowCount )
				{
					strRow = strtok_r( NULL, TABLE_2D_FILE_DELIMITER, &strRowNext );
				}

				if( uRowNum < uRowCount )
					uRowNum = uRowCount;
			}
			
			strLine = strtok_r( NULL, TABLE_2D_FILE_ENTER, &strLineNext );
		}
		uLineNum = uLineCount;

		free( strFileBuff );
	}while( 0 );

	struct table_2d* psTable = NULL;
	sflog( SFLOG_LEVEL_INFO, "table 2d e number %d, row number %d\n", uLineNum, uRowNum );
	if( uLineNum && uRowNum )
	{
		uint32_t uTableSize = sizeof(struct table_2d) + sizeof(psTable->aiTable[0])*uLineNum*uRowNum;
		psTable = (struct table_2d*)malloc( uTableSize );
		if( psTable )
		{
			memset( psTable, 0, uTableSize );
			psTable->uLineNum = uLineNum;
			psTable->uRowNum  = uRowNum;
		}
		else
		{
			sflog( SFLOG_LEVEL_ERR, "table 2d loc %d error\n", uTableSize );
		}
	}

	return psTable;
}

void convert_table_file( char* strFileData, struct table_2d* psTable )
{
	uint32_t	uLineNum	= psTable->uLineNum;
	uint32_t	uRowNum		= psTable->uRowNum;

	uint32_t uLineCount	= 0;
	char* strLineNext	= NULL;
	char* strLine		= strtok_r( strFileData, TABLE_2D_FILE_ENTER, &strLineNext );
	for( ; strLine && uLineNum > uLineCount; ++uLineCount )
	{
		uint32_t uRowCount	= 0;
		char* strRowNext	= NULL;
		char* strRow		= strtok_r( strLine, TABLE_2D_FILE_DELIMITER, &strRowNext );
		for( ; strRow && uRowNum > uRowCount; ++uRowCount )
		{
			psTable->aiTable[uRowNum*uLineCount+uRowCount] = strtol( strRow, NULL, 0 );
			sflog( SFLOG_LEVEL_DEBUG, "table[%d][%d]=%d\n", uLineCount, uRowCount, psTable->aiTable[uRowNum*uLineCount+uRowCount] );
			
			strRow = strtok_r( NULL, TABLE_2D_FILE_DELIMITER, &strRowNext );
		}
		
		strLine = strtok_r( NULL, TABLE_2D_FILE_ENTER, &strLineNext );
	}
}

struct table_2d* file_to_table_2d_line_row( char* strFileName, uint32_t	uLineNum, uint32_t	uRowNum )
{
	struct table_2d* psTable = NULL;

	do{
		if( NULL == strFileName )
		{
			sflog( SFLOG_LEVEL_WARNING, "file name is null\n" );
			break;
		}

		char*	 strFileData = NULL;
		uint32_t uFileSize	 = 0;
		strFileData = read_table_file( strFileName, &uFileSize );
		if( NULL == strFileData )
			break;

		psTable = traverse_table_file( strFileData, uFileSize, uLineNum, uRowNum );
		if( NULL == psTable )
			break;

		convert_table_file( strFileData, psTable );

		memset( strFileData, 0, uFileSize );
		uint32_t uDebugCount = 0;
		for( uint32_t i = 0; psTable->uLineNum > i; ++i )
		{
			for( uint32_t j = 0; psTable->uRowNum > j; ++j )
			{
				uDebugCount += snprintf( strFileData + uDebugCount, uFileSize - uDebugCount, " %d", psTable->aiTable[psTable->uRowNum*i+j] );
			}
			uDebugCount += snprintf( strFileData + uDebugCount, uFileSize - uDebugCount, "\n" );
		}
		sflog( SFLOG_LEVEL_DEBUG, "table 2d line %d row %d\n %s \n", psTable->uLineNum, psTable->uRowNum, strFileData );
		
		free( strFileData );
	}while( 0 );

	return psTable;
}

struct table_2d* file_to_table_2d( char* strFileName )
{
	return file_to_table_2d_line_row( strFileName, 0, 0 );
}

