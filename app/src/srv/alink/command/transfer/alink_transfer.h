
#ifndef ALINK_TRANSFER_H
#define ALINK_TRANSFER_H

#include "../../alink.h"

#pragma pack(1)
	typedef struct alink_transfer_set_wifi_info
	{
		uint8_t  nameLen;
		uint8_t  name[31];
		uint8_t  passwordLen;
		uint8_t  password[32];
		uint32_t ip;
		uint16_t port;
	}alink_transfer_set_wifi_info_t;

	typedef struct alink_transfer_set_wifi_info_ack
	{
		uint8_t  status;
	}alink_transfer_set_wifi_info_ack_t;
#pragma pack()

#pragma pack(1)
	typedef struct alink_transfer_set_disconnect
	{
		uint8_t  status;
	}alink_transfer_set_disconnect_t;

	typedef struct alink_transfer_set_disconnect_ack
	{
		uint8_t  status;
	}alink_transfer_set_disconnect_ack_t;
#pragma pack()

#pragma pack(1)
	typedef struct alink_transfer_request_channel
	{
		uint8_t  sn[15];
		uint32_t ip;
		uint16_t port;
		uint8_t  proxy;
	}alink_transfer_request_channel_t;

	typedef struct alink_transfer_request_channel_ack
	{
		uint8_t  proxy;
		uint32_t ip;
		uint16_t port;
	}alink_transfer_request_channel_ack_t;
#pragma pack()

#pragma pack(1)
	typedef struct alink_transfer_connect_stutas
	{
		uint8_t  status;
	}alink_transfer_connect_stutas_t;
#pragma pack()

void alink_transfer_init( void );

#endif /* ALINK_TRANSFER_H */

