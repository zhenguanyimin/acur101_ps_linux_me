
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/reboot.h>
#include <assert.h>

#include <pthread.h>
#include "alink_upgrade.h"
#include "../../../version/version.h"
#include "firmware.h"

enum 
{
	UPDATE_STAT_NO_START = 0,
	UPDATE_STAT_REQUEST,
	UPDATE_STAT_DOWNLOAD_IMAGE,
	UPDATE_STAT_CHECK,
	UPDATE_STAT_WRITE_IMAGE,
	UPDATE_STAT_WAIT_REBOOT,
};

#define UPDATE_RET_OK           (0)
#define UPDATE_RET_ERROR        (1)
#define UPDATE_RET_COMPLETE     (2)
#define UPDATE_RET_STATUS_ERROR (10) //the return value

static int update_state = UPDATE_STAT_NO_START;
static int firmware_save_ok = 0;
static uint32_t run_mode = 1; //1: normal mode, app runing. 0:boot mode


static void upgrade_get_version( alink_payload_t *psPayload, alink_resp_t *psResp )
{
	upgrade_version_ack_t *psVersionAck;

	psVersionAck = (upgrade_version_ack_t*)(psResp->pbyBuffer);

	psVersionAck->uRunVersion = run_mode;

	strncpy( psVersionAck->strBootVersion, "N/A", sizeof(psVersionAck->strBootVersion) );
	strncpy( psVersionAck->strAppVersion, get_embed_software_ps_version_string(), (size_t)sizeof(psVersionAck->strAppVersion) );
	strncpy( psVersionAck->strHwVersion, "N/A", sizeof(psVersionAck->strHwVersion) );
	strncpy( psVersionAck->strProtocolVersion, get_embed_alink_version_string(), sizeof(psVersionAck->strProtocolVersion) );
	
	psResp->wCount = sizeof(upgrade_version_ack_t);

	
	printf("%s[%d]in\n", __FUNCTION__, __LINE__);
}

static void multi_boot_run_golden(void)
{
	printf("reboot system....");
	sync();
	reboot(RB_AUTOBOOT);
}

static void upgrade_reboot_system( alink_payload_t *psPayload, alink_resp_t *psResp )
{
	upgrade_reset_t *psReset;
	upgrade_reset_ack_t *psResetAck;

	psReset = (upgrade_reset_t*)(psPayload->pbyData);
	psResetAck = (upgrade_reset_ack_t*)(psResp->pbyBuffer);

	
	printf("%s[%d]in\n", __FUNCTION__, __LINE__);

	if( 0x55AA == psReset->resetCode )
	{
		// get system status and check upgrade flag
		switch( psReset->type )
		{
			case 0x0004:
				run_mode = 0;
				/*
				if( true == firmware_set_upgrade_flag(true) )
				{
					multi_boot_run_golden();
				}
				else
				{
					psResetAck->byStatus = 1;
					psResp->wCount = 1;
				}
				*/
				psResetAck->byStatus = 0;
				psResp->wCount = 1;
				break;
			default:
				break;
		}
	}
	else
	{
		psResetAck->byStatus = 0x0F;
		psResp->wCount = 1;
	}
}

static void upgrade_request_upgrade(alink_payload_t *msg, alink_resp_t *resp)
{
	bool ret;

	upgrade_request_upgrade_t *psRequestDownload;
	upgrade_request_upgrade_ack_t *psRequestDownloadAck;

	psRequestDownload = (upgrade_request_upgrade_t*)(msg->pbyData);
	psRequestDownloadAck = (upgrade_request_upgrade_ack_t*)(resp->pbyBuffer);

	printf("%s[%d]in\n", __FUNCTION__, __LINE__);
	//assert(resp->wSize >= sizeof(upgrade_request_upgrade_ack_t));

	if (UPDATE_STAT_WAIT_REBOOT == update_state )
	{
		psRequestDownloadAck->byStatus = UPDATE_RET_STATUS_ERROR;
		resp->wCount = sizeof(upgrade_request_upgrade_ack_t);
		return;
	}

	ret = firmware_recv_init(&(psRequestDownload->sFirmwareHeader));
	if (ret)
	{
		psRequestDownloadAck->byStatus = UPDATE_RET_OK;
		resp->wCount = sizeof(upgrade_request_upgrade_ack_t);

		update_state = UPDATE_STAT_REQUEST;
	}
	else
	{
		psRequestDownloadAck->byStatus = UPDATE_RET_ERROR;
		resp->wCount = sizeof(upgrade_request_upgrade_ack_t);
	}

	printf("%s[%d]out\n", __FUNCTION__, __LINE__);
}

static void upgrade_download_image(alink_payload_t *msg, alink_resp_t *resp)
{
	bool ret;

	upgrade_download_image_t *psDownloadImage;
	upgrade_download_image_ack_t *psDownloadImageAck;

	psDownloadImage = (upgrade_download_image_t*)(msg->pbyData);
	psDownloadImageAck = (upgrade_download_image_ack_t*)(resp->pbyBuffer);

	printf("%s[%d]in\n", __FUNCTION__, __LINE__);

	if ((update_state != UPDATE_STAT_DOWNLOAD_IMAGE) && (update_state != UPDATE_STAT_REQUEST))
	{
		psDownloadImageAck->byStatus = UPDATE_RET_STATUS_ERROR;
		resp->wCount = sizeof(upgrade_request_upgrade_ack_t);
		printf("%s[%d]state error:%d\n", __FUNCTION__, __LINE__, update_state);
		return ;
	}

	ret = firmware_recv_data(psDownloadImage->imageOffset, psDownloadImage->imageData, psDownloadImage->imageLength); 
	if (ret)
	{
		psDownloadImageAck->byStatus = UPDATE_RET_OK;
		resp->wCount = sizeof(upgrade_download_image_ack_t);
		update_state = UPDATE_STAT_DOWNLOAD_IMAGE;		
	}
	else
	{
		psDownloadImageAck->byStatus = UPDATE_RET_ERROR;
		resp->wCount = sizeof(upgrade_download_image_ack_t);

		printf("%s[%d]firmware_recv_data error\n", __FUNCTION__, __LINE__);
	}
}

static void upgrade_check_image(alink_payload_t *msg, alink_resp_t *resp)
{
	upgrade_check_image_ack_t *psCheckImageAck;

	psCheckImageAck = (upgrade_check_image_ack_t*)(resp->pbyBuffer);
	resp->wCount = sizeof(upgrade_check_image_ack_t);

	printf("%s[%d]in\n", __FUNCTION__, __LINE__);

	if (firmware_check())
	{
		psCheckImageAck->byStatus = UPDATE_RET_OK;
		update_state = UPDATE_STAT_CHECK;
	}
	else
	{
		psCheckImageAck->byStatus = UPDATE_RET_ERROR;
		printf("%s[%d]check error\n", __FUNCTION__, __LINE__);
	}

	printf("%s[%d]out\n", __FUNCTION__, __LINE__);
}

static void upgrade_write_image(alink_payload_t *msg, alink_resp_t *resp)
{
	bool ret;

	upgrade_write_image_ack_t *psWriteImageAck;

	psWriteImageAck = (upgrade_write_image_ack_t*)(resp->pbyBuffer);

	
	printf("%s[%d]in\n", __FUNCTION__, __LINE__);

	if (update_state < UPDATE_STAT_DOWNLOAD_IMAGE)
	{
		psWriteImageAck->byStatus = UPDATE_RET_STATUS_ERROR;
		goto out;
	}

	ret = firmware_set_upgrade_flag(false);
	if (!ret)
	{
		psWriteImageAck->byStatus = UPDATE_RET_ERROR;
		goto out;
	}

	update_state = UPDATE_STAT_WRITE_IMAGE;

	ret = firmware_save();
	if (ret)
	{
		ret = firmware_set_upgrade_flag(true);
		if (ret)
		{
			firmware_save_ok = 1;
			psWriteImageAck->byStatus = UPDATE_RET_OK;
		}
		else
		{
			psWriteImageAck->byStatus = UPDATE_RET_ERROR;
		}
	}
	else
	{
		psWriteImageAck->byStatus = UPDATE_RET_ERROR;
	}

out:	
	resp->wCount = sizeof(upgrade_write_image_ack_t);	
	printf("%s[%d]out\n", __FUNCTION__, __LINE__);
}

static void upgrade_write_status(alink_payload_t *msg, alink_resp_t *resp)
{
	upgrade_write_status_ack_t *psWriteStatusAck;

	printf("%s[%d]\n", __FUNCTION__, __LINE__);
		
	psWriteStatusAck = (upgrade_write_status_ack_t*)(resp->pbyBuffer);
	resp->wCount = sizeof(upgrade_write_status_ack_t);

	if (update_state != UPDATE_STAT_WRITE_IMAGE)
	{
		psWriteStatusAck->byStatus = UPDATE_RET_STATUS_ERROR;
		psWriteStatusAck->wPermil = 0;
	}
	else 
	{
		if (firmware_save_ok)
		{
			psWriteStatusAck->byStatus = UPDATE_RET_COMPLETE;
			psWriteStatusAck->wPermil = 1000;
		}
		else
		{
			psWriteStatusAck->byStatus = UPDATE_RET_ERROR;
			psWriteStatusAck->wPermil = 0;
		}
	}
}

static void *auto_reboot_task(void *param)
{
	sleep(3);
	printf("OTA reboot!!!!!!!!!!!!\n");
	multi_boot_run_golden();
	return NULL;
}

static void upgrade_goto_app(alink_payload_t *msg, alink_resp_t *resp)
{
	upgrade_goto_app_ack_t *psGotoAppAck;
	int ret;
	pthread_t tid;

	printf("%s[%d]\n", __FUNCTION__, __LINE__);

	psGotoAppAck = (upgrade_goto_app_ack_t*)(resp->pbyBuffer);


	ret = pthread_create(&tid, NULL, auto_reboot_task, NULL);
	if (ret)
	{
		psGotoAppAck->byStatus = UPDATE_RET_ERROR;
		resp->wCount = sizeof(upgrade_goto_app_ack_t);
	}
	else
	{
		psGotoAppAck->byStatus = UPDATE_RET_OK;
		resp->wCount = sizeof(upgrade_goto_app_ack_t);

		update_state = UPDATE_STAT_WAIT_REBOOT;
		firmware_save_ok = 0;
	}

	///multi_boot_run_golden();
}

static void upgrade_get_timeout_info(alink_payload_t *msg, alink_resp_t *resp)
{
	upgrade_timeout_info_ack_t *psTimeoutInfoAck;

	assert(msg->wLength >= ALINK_UPGRADE_RECV_BUFFER_SIZE);
	psTimeoutInfoAck = (upgrade_timeout_info_ack_t*)(resp->pbyBuffer);

	psTimeoutInfoAck->chechTimeoutS = 60;
	psTimeoutInfoAck->imagePkgLenMax = ALINK_UPGRADE_RECV_BUFFER_SIZE;
	psTimeoutInfoAck->writeTimeoutS = 10*60;
	psTimeoutInfoAck->cmdTimeoutMs = 1000;
	resp->wCount = sizeof(upgrade_timeout_info_ack_t);
}

static alink_cmd_list_t sAlinkCmd_UpgradeGetVersion = { 0 };
static alink_cmd_list_t sAlinkCmd_UpgradeReboot = { 0 };
static alink_cmd_list_t sAlinkCmd_upgrade_request_upgrade = { 0 };
static alink_cmd_list_t sAlinkCmd_upgrade_download_image = { 0 };
static alink_cmd_list_t sAlinkCmd_upgrade_check_image = { 0 };
static alink_cmd_list_t sAlinkCmd_upgrade_write_image = { 0 };
static alink_cmd_list_t sAlinkCmd_upgrade_write_status = { 0 };
static alink_cmd_list_t sAlinkCmd_upgrade_goto_app = { 0 };
static alink_cmd_list_t sAlinkCmd_upgrade_get_timeout_info = { 0 };

void alink_upgrade_init( void )
{
	firmware_save_ok = 0;
	run_mode = 1;
	update_state = UPDATE_STAT_NO_START;

	alink_register_cmd( &sAlinkCmd_UpgradeGetVersion, 	        0xAB,	0,							       upgrade_get_version );
	alink_register_cmd( &sAlinkCmd_UpgradeReboot, 		        0xA1,	sizeof(upgrade_reset_t),	       upgrade_reboot_system );
	alink_register_cmd( &sAlinkCmd_upgrade_request_upgrade, 	0xA7,	sizeof(upgrade_request_upgrade_t), upgrade_request_upgrade );
	alink_register_cmd( &sAlinkCmd_upgrade_download_image, 		0xA8,	0,                                 upgrade_download_image );
	alink_register_cmd( &sAlinkCmd_upgrade_check_image, 		0xAC,	0,	                               upgrade_check_image );
	alink_register_cmd( &sAlinkCmd_upgrade_write_image, 		0xA9,	0,                                 upgrade_write_image );
	alink_register_cmd( &sAlinkCmd_upgrade_write_status, 		0xA6,	0,                                 upgrade_write_status );
	alink_register_cmd( &sAlinkCmd_upgrade_goto_app, 		    0xAD,	0,                                 upgrade_goto_app );
	alink_register_cmd( &sAlinkCmd_upgrade_get_timeout_info, 	0xAE,	0,	                               upgrade_get_timeout_info );
}

