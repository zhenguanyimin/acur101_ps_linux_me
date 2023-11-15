

#include<sys/socket.h>
#include "protocol_ack.h"


ret_code_t protocol_send_ack(int32_t sockFd, protocol_ack_t *ack)
{
	ret_code_t ret = RET_OK;

	if ((sockFd < 0) || (ack == NULL))
	{
		ret = RET_INVALID_PARAM;
		goto out;
	}

	(void)send(sockFd, ack, sizeof(*ack), 0);

out:
	APP_LOG_DEBUG("[%s:%d] ret=%d\r\n", __FUNCTION__, __LINE__, ret);
	return ret;
}
