
#ifndef ALINK_RECORD_H
#define ALINK_RECORD_H

#include "../../alink.h"

#pragma pack(1)
	typedef struct alink_record_file_list
	{
		uint32_t	uNameLen;
		uint32_t	uDataLen;
		char		abyData[0];
	}alink_record_file_list_t;

	typedef struct alink_record_get_list_ack
	{
		uint32_t					uPkgSize;
		uint32_t					uPkgCount;
		alink_record_file_list_t	sList[0];
	}alink_record_get_list_ack_t;
#pragma pack()

#pragma pack(1)
	typedef struct alink_record_get_list
	{
		uint32_t	uId;
		uint32_t	uNameLen;
		char		abyData[0];
	}alink_record_get_list_t;

	typedef struct alink_record_get_file
	{
		uint32_t				uNum;
		alink_record_get_list_t	sList[0];
	}alink_record_get_file_t;

	typedef struct alink_record_get_file_ack
	{
		uint32_t	uId;
		uint32_t	uPkgSize;
		uint32_t	uPkgCount;
		uint32_t	uDataLen;
		char		abyData[0];
	}alink_record_get_file_ack_t;
#pragma pack()

#pragma pack(1)
	typedef struct alink_record_delete_file
	{
		uint32_t	uNameLen;
		char		abyName[0];
	}alink_record_delete_file_t;

	typedef struct alink_record_delete_file_ack
	{
		uint8_t		byStatus;
	}alink_record_delete_file_ack_t;
#pragma pack()

void alink_record_init( void );

#endif /* RECORD_H */
