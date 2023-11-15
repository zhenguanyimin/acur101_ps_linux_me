
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>
#include "protocol_dbgdat.h"

ret_code_t protocol_send_adc_data(protocol_adc_data_t *adc_data, send_data_cb_fp cb_fp)
{
	ret_code_t ret = RET_OK;

	if (adc_data == NULL)
	{
		ret = RET_INVALID_PARAM;
		goto out;
	}

	// ret = protocol_udp_send_data_blocking(adc_data, sizeof(*adc_data));
	ret = protocol_udp_send_data_nonblocking(adc_data, sizeof(*adc_data), cb_fp);

out:
//	APP_LOG_DEBUG("[%s:%d] ret=%d\r\n", __FUNCTION__, __LINE__, ret);
	return ret;
}

ret_code_t protocol_send_rdmap_data(protocol_rdmap_data_t *rdmap_data, send_data_cb_fp cb_fp)
{
	ret_code_t ret = RET_OK;

	if (rdmap_data == NULL)
	{
		ret = RET_INVALID_PARAM;
		goto out;
	}

	// ret = protocol_udp_send_data_blocking(adc_data, sizeof(*adc_data));
	ret = protocol_udp_send_data_nonblocking(rdmap_data, sizeof(*rdmap_data), cb_fp);

out:
//	APP_LOG_DEBUG("[%s:%d] ret=%d\r\n", __FUNCTION__, __LINE__, ret);
	return ret;
}

#include <time.h>

unsigned long long current_time_ms() {
    struct timespec ts;
    if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
        perror("clock_gettime");
        exit(1);
    }

    return (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
}


ret_code_t protocol_adcpkg_set_head(protocol_adc_data_t *adc_data, uint32_t frameId, uint32_t type, uint32_t length)
{
	ret_code_t ret = RET_OK;

	if ((adc_data == NULL) || (length != sizeof(adc_data->rawData)))
	{
		ret = RET_INVALID_PARAM;
		goto out;
	}

	adc_data->stInfoHeader.infoSync = htonl(INFO_HEAD_FLAG);
	adc_data->stInfoHeader.infoLength = htonl(sizeof(*adc_data));
	adc_data->stInfoHeader.frameID = htonl(frameId);
	adc_data->stInfoHeader.timestamp = htonl(current_time_ms());
	adc_data->stInfoHeader.infoType = htons(type);
	adc_data->stInfoHeader.terminalID = htons(1);
	adc_data->stInfoHeader.terminalType = 0;
	adc_data->stInfoHeader.subTerminalType = 0;
	adc_data->stInfoHeader.infoVersion = PROTOCOL_ADC_DATA_VER;

	// note: CRC is not used to save time here

out:
	return ret;
}

ret_code_t protocol_rdmappkg_set_head(protocol_rdmap_data_t *rdmap_data, uint32_t frameId, uint32_t type, uint32_t length)
{
	ret_code_t ret = RET_OK;

	if ((rdmap_data == NULL) || (length != sizeof(rdmap_data->rawData)))
	{
		ret = RET_INVALID_PARAM;
		goto out;
	}

	rdmap_data->stInfoHeader.infoSync = htonl(INFO_HEAD_FLAG);
	rdmap_data->stInfoHeader.infoLength = htonl(sizeof(*rdmap_data));
	rdmap_data->stInfoHeader.frameID = htonl(frameId);
	rdmap_data->stInfoHeader.timestamp = htonl(current_time_ms());
	rdmap_data->stInfoHeader.infoType = htons(type);
	rdmap_data->stInfoHeader.terminalID = htons(1);
	rdmap_data->stInfoHeader.terminalType = 0;
	rdmap_data->stInfoHeader.subTerminalType = 0;
	rdmap_data->stInfoHeader.infoVersion = PROTOCOL_ADC_DATA_VER;

	// note: CRC is not used to save time here

out:
	return ret;
}

