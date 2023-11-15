#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../../inc/radar_error.h"
#include "../../srv/log/log.h"
#include "temp_table.h"

#define TEMPETURE_TABLE_FILE_NAME "storage/file/temp-table.dat"

typedef struct temp_table_member {
	float temp;
	unsigned long convert_val;
}temp_table_member_t;

typedef struct temp_table_file_info {
	temp_table_member_t *temp_table;
	unsigned int temp_table_num;
} temp_table_file_info_t; 

temp_table_file_info_t *tt_file_info = NULL;

static int parse_temp_file_content(char *filebuf, unsigned int filesize, temp_table_file_info_t *tt_fileinfo)
{
	int ret = -1, i = 0;
	unsigned int tt_member_size = 0;
	temp_table_member_t *tt_member = NULL;
	char *temp_table = filebuf;
	char *pos = NULL, *seperator = NULL;
	char *tempval = NULL;
	char *convertval = NULL;

	if(!filebuf || filesize <=0 || !tt_fileinfo) {
		return RET_INVALID_PARAM;
	}
	pos = temp_table;
	do {
		pos = strstr(pos, "\n");
		tt_member_size++;
		pos += 1;
	} while((pos - temp_table) < (filesize - 1));
	tt_member_size += 1;	
	tt_member = (temp_table_member_t *)malloc(tt_member_size * sizeof(temp_table_member_t));
	if(!tt_member) {
		APP_LOG_ERROR("memory error\n");
		return RET_SYSTEM_ERR;
	}	
	APP_LOG_DEBUG("tt_member_size = %d\n", tt_member_size);

	pos = NULL;
	i = 0;
	do {
		pos = strstr(temp_table, "\n");	
		if(NULL == pos) {
			break;
		}
		seperator = strstr(temp_table, ",");
		if(seperator == NULL) {
			temp_table = pos + 1;
			continue;
		}
		tempval = (char*)malloc(seperator-temp_table);
		if(!tempval) {
			break;
		}
		memset(tempval, 0, seperator-temp_table);
		memcpy(tempval, temp_table, seperator-temp_table);
		tt_member[i].temp = atof(tempval);
		convertval = (char*)malloc(pos-seperator);
		if(!convertval) {
			free(tempval);
			break;
		}
		memset(convertval, 0, pos-seperator);
		memcpy(convertval, seperator+1, pos-seperator-1);
		if(convertval[pos-seperator-1-1] == '\r') { //windows end charater maybe \r\n, linux is \n
			convertval[pos-seperator-1-1] = '\0';
		}
		tt_member[i].convert_val = atol(convertval);
	//	APP_LOG_DEBUG("%f,%d\n", tt_member[i].temp, tt_member[i].convert_val);
		temp_table = pos + 1;//overpass \n charater
		free(tempval);
		free(convertval);
		i++;
	} while(temp_table != NULL && tt_member_size > (i+1));

	tt_fileinfo->temp_table = tt_member;
	tt_fileinfo->temp_table_num = i;
	//APP_LOG_INFO("parse file buffer end, lenght = %d\n", tt_fileinfo->temp_table_num);

	return 0;
}

static int load_temp_table_file_to_buf(const unsigned char *table_file_full_name, temp_table_file_info_t *tt_fileinfo)
{
	int ret = -1;
	FILE *fp = NULL;
	unsigned int filesize = -1;
	char *filebuf = NULL;

	if(!table_file_full_name || !tt_fileinfo) {
		return RET_INVALID_PARAM;
	}
	fp = fopen(table_file_full_name, "r");
	if(NULL == fp) {
		APP_LOG_ERROR("open %s fail\n", table_file_full_name);
		ret = -1;
		return ret;
	}
	fseek(fp, 0L, SEEK_END);
	filesize = ftell(fp);
	if(filesize == 0) {
		APP_LOG_INFO("%s file size is 0\n", table_file_full_name);
		ret = -1;
		goto err_exit; 
	}
	filebuf = (char *)malloc(filesize);	
	if(!filebuf) {
		APP_LOG_ERROR("memory error\n");
		ret = RET_SYSTEM_ERR;
		goto err_exit;
	}
	fseek(fp, 0, SEEK_SET);
	ret = fread(filebuf, sizeof(char), filesize, fp);
	if(ret <= 0) {
		APP_LOG_ERROR("read %s error, ret = %d\n", table_file_full_name, ret);
		ret = RET_SYSTEM_ERR;
		goto err_exit;
	}
	
	ret = parse_temp_file_content(filebuf, filesize, tt_fileinfo);

err_exit:
	if(filebuf) {
		free(filebuf);
		filebuf = NULL;
	}
	fclose(fp);

	return ret;
}

unsigned long lookup_by_temp(float tempval, temp_table_member_t *tt_member, int tt_len)
{
	unsigned long convert_val = 0;
	float last_low_temp_val = 0, last_high_temp_val = 0;
	unsigned long last_high_cov_val = 0, last_low_cov_val = 0;
	int i = 0;
	
	if(!tt_member || tt_len <= 0) {
		return convert_val;
	}

	for(i=0; i<tt_len; i++) {
		if(tt_member[i].temp == tempval) {
			convert_val = tt_member[i].convert_val;
			break;
		} else if(tt_member[i].temp > tempval) {
			if(last_high_temp_val > tt_member[i].temp || last_high_temp_val == 0) {
				last_high_temp_val = tt_member[i].temp; 
				last_high_cov_val = tt_member[i].convert_val;
			}	
		} else {
			if(last_low_temp_val < tt_member[i].temp) {
				last_low_temp_val = tt_member[i].temp;
				last_low_cov_val = tt_member[i].convert_val;
			}
		}
	}

	if(i == tt_len) {
		if(last_low_temp_val == 0) {
			convert_val = last_high_cov_val;
		} else if(last_high_temp_val == 0) {
			convert_val = last_low_cov_val;
		} else {
			convert_val = (unsigned long)((last_low_cov_val + last_high_cov_val)/2);
		}
	}
	
	return convert_val;
}

unsigned long temp_table_lookup(float tempval)
{
	if(!tt_file_info) {
		return 0;
	}
	return lookup_by_temp(tempval, tt_file_info->temp_table, tt_file_info->temp_table_num);
}

int temp_table_load(void)
{
	int ret = 0;
	
	APP_LOG_DEBUG("%s > %d\n", __FUNCTION__, __LINE__);
	ret = access(TEMPETURE_TABLE_FILE_NAME, R_OK);
	if(0 != ret) {
		APP_LOG_ERROR("warmming!!! temp_table %s isn't exsit\n", TEMPETURE_TABLE_FILE_NAME);
		return -1;
	}
	tt_file_info = (temp_table_file_info_t*)malloc(sizeof(temp_table_file_info_t));
	if(!tt_file_info) {
		return RET_SYSTEM_ERR;
	}
	memset(tt_file_info, 0, sizeof(temp_table_file_info_t));
	ret = load_temp_table_file_to_buf(TEMPETURE_TABLE_FILE_NAME, tt_file_info);
	if(0 != ret) {
		APP_LOG_ERROR("load %s fail, ret = %d\n", TEMPETURE_TABLE_FILE_NAME, ret);
	}

#if 0 //debug
	printf("2.0,%d\n", temp_table_lookup(2.0));
	printf("30.0,%d\n", temp_table_lookup(30.0));
	printf("35.0,%d\n", temp_table_lookup(35.0));
	printf("98.0,%d\n", temp_table_lookup(98.0));
	printf("158.0,%d\n", temp_table_lookup(158.0));
	printf("190.0,%d\n", temp_table_lookup(190.0));
	printf("191.0,%d\n", temp_table_lookup(191.0));
	printf("145.0,%d\n", temp_table_lookup(145.0));
	printf("57.0,%d\n", temp_table_lookup(57.0));
	printf("39.0,%d\n", temp_table_lookup(39.0));
	printf("230.0,%d\n", temp_table_lookup(230.0));
 #endif

	return ret;
}

int temp_table_unload(void)
{
	if(tt_file_info) {
		free(tt_file_info);
		tt_file_info = NULL;
	}

	return 0;
}

