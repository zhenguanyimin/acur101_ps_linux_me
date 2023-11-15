
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <linux/sockios.h>
#include <linux/ethtool.h>

#include <sys/time.h>
#include <semaphore.h>
#include <mqueue.h>


#include "protocol_common.h"

/* Utils includes */
// #include "sleep.h"

#include "protocol_parameter.h"
#include "protocol_ack.h"
#include "protocol_dbgdat.h"

///extern enum ethernet_link_status eth_link_status;

#define PROTOCOL_COMMOM_MSGQ_LEN (20)

typedef enum protocol_tcp_proc_msg_ret {
	TCP_MSG_PROC_RET_CONTINUE = 0,
	TCP_MSG_PROC_RET_FINISH = 1,
	TCP_MSG_PROC_RET_ERROR = 2
} protocol_tcp_proc_msg_ret_t;

typedef struct protocol_common_msg {
	void *data;
	uint32_t len;
	send_data_cb_fp cb_fp;
} protocol_common_msg_t;

typedef struct protocol_udp_mcb {
	int sockFd;
	pthread_t sendTskHdl; // TaskHandle_t
	pthread_t recvTskHdl; // TaskHandle_t
	pthread_mutex_t * lock;
	mqd_t msgQ;
	protocol_packet_head_t packetHead;
} protocol_udp_mcb_t;

typedef struct protocol_tcp_mcb {
	pthread_t tskHdl; // TaskHandle_t
	sem_t * lock;
	int32_t iSocket;
	void (*pv_cbk_recv)(uint8_t* pbyData, uint32_t uLen);
//	union {
//		protocol_cfg_param_t cfg_param;
//	} buf;
	uint8_t buf[4096+256];
	uint32_t rcvIdx;
} protocol_tcp_mcb_t;

typedef struct protocol_common_mcb {
	bool inited;
	protocol_udp_mcb_t udpMcb;
	protocol_tcp_mcb_t tcpMcb;
} protocol_common_mcb_t;

STATIC protocol_common_mcb_t g_protocolCommonMcb;
STATIC uint8_t gSendBuff[(sizeof(protocol_adc_data_t)/(UDP_INFO_LEN_MAX)+1)*20+sizeof(protocol_adc_data_t)] ={0};

#define INVALD_MSG_HD ((mqd_t) -1)
#define INVALD_SEM_HD (NULL)
#define INVALD_PTHREAD_HD (-1)
#define INVALD_MUTEX_HD (NULL)

static pthread_t sys_thread_new(const char *thread_name, void (*start_routine) (void *), void *param, 
										unsigned int stack_size, unsigned int priority)
{
	int ret;
	pthread_t handle;

	ret = pthread_create(&handle, NULL, (void *(*)(void *))start_routine, param);
	if (0 == ret)
	{
		return handle;
	}
	else
	{
		return (pthread_t)INVALD_PTHREAD_HD;
	}
}

int eth_link_status(const char *if_name, int sockfd)
{
	struct ifreq ifr;
    struct ethtool_value edata;

	edata.cmd = ETHTOOL_GLINK;
    edata.data = 0;

    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, if_name, sizeof(ifr.ifr_name) - 1);
    ifr.ifr_data = (char *) &edata;

	return ioctl( sockfd, SIOCETHTOOL, &ifr );
}

ret_code_t protocol_udp_send_data_log(void *data, uint32_t len)
{
	ret_code_t ret = RET_OK;
	int sockFd = -1;
	struct sockaddr_in remoteAddr = {0};
	int32_t sendLen = 0;
	int32_t totalLen = 0;
	int32_t currLen = 0;
	uint8_t *buf = gSendBuff;
	//uint8_t buf[UDP_PACKET_LEN_MAX] = {0};
	protocol_packet_head_t pktHead = {0};
	protocol_info_head_t *infoHead = (protocol_info_head_t *)data;
	uint8_t *posIdx = (uint8_t *)data;
	uint16_t infoPacketNum = 0;
	uint16_t curInfoPacketOrder = 0;
	int retry = 0;

	sockFd = g_protocolCommonMcb.udpMcb.sockFd;
	if ((!g_protocolCommonMcb.inited) || (sockFd < 0) || (0 != eth_link_status("eth0", sockFd)))
	{
		ret = RET_SYSTEM_ERR;
		goto out;
	}

	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_port = htons(8080);
	inet_aton(IP_ADDR_REMOTE, &remoteAddr.sin_addr);

	if (0 == pthread_mutex_lock(g_protocolCommonMcb.udpMcb.lock))
	{
		while (len > 0)
		{
			sendLen = sendto(sockFd, data, len, 0, (struct sockaddr*)&remoteAddr, sizeof(remoteAddr));
			if (sendLen != len)
			{
				if (retry++ < 10000)
				{
					continue;
				}
			}
			len -= sendLen;
		}
		pthread_mutex_unlock(g_protocolCommonMcb.udpMcb.lock);
	}

out:
	return ret;
}

PRIVATE ret_code_t protocol_udp_send_data(void *data, uint32_t len)
{
	ret_code_t ret = RET_OK;
	int32_t sockFd = -1;
	struct sockaddr_in remoteAddr = {0};
	int32_t sendLen = 0;
	int32_t totalLen = 0;
	int32_t currLen = 0;
	uint8_t *buf = gSendBuff;
	//uint8_t buf[UDP_PACKET_LEN_MAX] = {0};
	protocol_packet_head_t pktHead = {0};
	protocol_info_head_t *infoHead = (protocol_info_head_t *)data;
	uint8_t *posIdx = (uint8_t *)data;
	uint16_t infoPacketNum = 0;
	uint16_t curInfoPacketOrder = 0;
	int retry = 0;

	sockFd = g_protocolCommonMcb.udpMcb.sockFd;
	if ((!g_protocolCommonMcb.inited) || (sockFd < 0) || (0 != eth_link_status("eth0", sockFd)))
	{
		ret = RET_SYSTEM_ERR;
		goto out;
	}

	if (sizeof(pktHead) != UDP_PACKET_HEAD_LEN)
	{
		ret = RET_SYSTEM_ERR;
		goto out;
	}

	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_port = htons(UDP_COM_PORT_REMOTE);
	inet_aton(IP_ADDR_REMOTE, &remoteAddr.sin_addr);

	if (0 == pthread_mutex_lock(g_protocolCommonMcb.udpMcb.lock))
	{
		memcpy(&pktHead, &g_protocolCommonMcb.udpMcb.packetHead, sizeof(pktHead));
		pktHead.packetFlag = htonl(PACKET_HEAD_FLAG);
		pktHead.terminalType = infoHead->terminalType;
		pktHead.subTerminalType = infoHead->subTerminalType;
		pktHead.infoType = infoHead->infoType;
		infoPacketNum = (len % UDP_INFO_LEN_MAX == 0) ? (len / UDP_INFO_LEN_MAX) : (len / UDP_INFO_LEN_MAX) + 1;
		pktHead.infoPacketNum = htons(infoPacketNum);

		totalLen = len;
		while (totalLen > 0)
		{
			currLen = totalLen > UDP_INFO_LEN_MAX ? UDP_INFO_LEN_MAX : totalLen;
			pktHead.totalPacketNum = htons(g_protocolCommonMcb.udpMcb.packetHead.totalPacketNum);
			pktHead.packetLength = htons(sizeof(pktHead) + currLen);
			pktHead.sendInfoCount = htons(g_protocolCommonMcb.udpMcb.packetHead.sendInfoCount);
			pktHead.curInfoPacketOrder = htons(curInfoPacketOrder);
			memset(buf, 0, sizeof(buf));
			memcpy(buf, &pktHead, sizeof(pktHead));
			memcpy(buf + sizeof(pktHead), posIdx, currLen);
			sendLen = sendto(sockFd, buf, sizeof(pktHead) + currLen, 0, (struct sockaddr*)&remoteAddr, sizeof(remoteAddr));
			if (sendLen != (sizeof(pktHead) + currLen))
			{
				APP_LOG_VERBOSE("[%s:%d] sendLen=%d not same with currLen=%d\r\n", __FUNCTION__, __LINE__, sendLen, sizeof(pktHead) + currLen);
				if (retry++ < 10000)
				{
					continue;
				}
			}

			retry = 0;
			buf = buf + sizeof(pktHead) + currLen ;
			totalLen -= currLen;
			posIdx += currLen;
			g_protocolCommonMcb.udpMcb.packetHead.totalPacketNum++;
			curInfoPacketOrder++;
		}

		g_protocolCommonMcb.udpMcb.packetHead.sendInfoCount++;
		pthread_mutex_unlock(g_protocolCommonMcb.udpMcb.lock);
	}

out:
	return ret;
}

PRIVATE void protocol_udp_send_thread(void *p_arg)
{
	int osRet;
	protocol_common_msg_t msg = {0};
	ret_code_t ret = RET_OK;

	while (!g_protocolCommonMcb.inited)
	{
		usleep(1000);
	}

	while (1)
	{
		memset(&msg, 0, sizeof(msg));
		osRet = mq_receive(g_protocolCommonMcb.udpMcb.msgQ, (char *)&msg, sizeof(msg), NULL);
		if (sizeof(msg) == osRet)
		{
			ret = protocol_udp_send_data(msg.data, msg.len);
			if (msg.cb_fp != NULL)
			{
				msg.cb_fp(msg.data, msg.len, (uint32_t)ret);
			}
		}
		else
		{
			APP_LOG_ERROR("[%s:%d] Unexpected error", __FUNCTION__, __LINE__);
		}
	}
}

PRIVATE void protocol_udp_recv_thread(void *p_arg)
{
	struct sockaddr_in remoteAddr = {0};
	socklen_t addrLen = 0;
	int32_t recvLen = 0;
	int sockFd = 0;
	uint8_t buf[UDP_PACKET_LEN_MAX] = {0};

	while (!g_protocolCommonMcb.inited)
	{
		usleep(1000);
	}
	sockFd = g_protocolCommonMcb.udpMcb.sockFd;

	while (1)
	{
		// Note: the udp server is not needed anymore
		usleep(1000);

		memset(buf, 0, sizeof(buf));
		addrLen = sizeof(remoteAddr);
		recvLen = recvfrom(sockFd, buf, sizeof(buf), 0, (struct sockaddr*)&remoteAddr, &addrLen);
		if (recvLen > 0)
		{
			APP_LOG_VERBOSE("[%s:%d] recvLen=%lu from %s %u\r\n",
				__FUNCTION__, __LINE__, recvLen, inet_ntoa(remoteAddr.sin_addr), ntohs(remoteAddr.sin_port));
		}
	}
}

PRIVATE void protocol_udp_deinit(void)
{
	if (g_protocolCommonMcb.udpMcb.msgQ != INVALD_MSG_HD)
	{
		mq_close(g_protocolCommonMcb.udpMcb.msgQ);
		g_protocolCommonMcb.udpMcb.msgQ = INVALD_MSG_HD;
	}

	if (g_protocolCommonMcb.udpMcb.lock != INVALD_MUTEX_HD)
	{
		pthread_mutex_destroy(g_protocolCommonMcb.udpMcb.lock);
		free(g_protocolCommonMcb.udpMcb.lock);
		g_protocolCommonMcb.udpMcb.lock = INVALD_MUTEX_HD;
	}

	if (g_protocolCommonMcb.udpMcb.recvTskHdl != INVALD_PTHREAD_HD)
	{
		pthread_cancel(g_protocolCommonMcb.udpMcb.recvTskHdl);
		g_protocolCommonMcb.udpMcb.recvTskHdl = INVALD_PTHREAD_HD;
	}

	if (g_protocolCommonMcb.udpMcb.sendTskHdl != INVALD_PTHREAD_HD)
	{
		pthread_cancel(g_protocolCommonMcb.udpMcb.sendTskHdl);
		g_protocolCommonMcb.udpMcb.sendTskHdl = INVALD_PTHREAD_HD;
	}

	if (g_protocolCommonMcb.udpMcb.sockFd >= 0)
	{
		close(g_protocolCommonMcb.udpMcb.sockFd);
		g_protocolCommonMcb.udpMcb.sockFd = -1;
	}

	if (g_protocolCommonMcb.tcpMcb.iSocket >= 0)
	{
		close(g_protocolCommonMcb.tcpMcb.iSocket);
		g_protocolCommonMcb.tcpMcb.iSocket = -1;
	}

	if (g_protocolCommonMcb.tcpMcb.lock != INVALD_SEM_HD)
	{
		sem_close(g_protocolCommonMcb.tcpMcb.lock);
		g_protocolCommonMcb.tcpMcb.lock = INVALD_SEM_HD;
	}

	return;
}

PRIVATE ret_code_t protocol_udp_init(void)
{
	ret_code_t ret = RET_OK;
	int sockFd = -1;
	int sockRet = 0;
	struct sockaddr_in svrAddr = {0};
	pthread_mutex_t * lock = INVALD_MUTEX_HD;
	mqd_t msgQ = INVALD_MSG_HD;
	pthread_t sendTskHdl = INVALD_PTHREAD_HD;
	pthread_t recvTskHdl = INVALD_PTHREAD_HD;
	struct mq_attr attr;

	sockFd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockFd < 0)
	{
		ret = RET_NO_RESOURCE;
		goto out;
	}

	svrAddr.sin_family = AF_INET;
	svrAddr.sin_port = htons(UDP_COM_PORT_LOCAL);
	svrAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
//	inet_aton(INADDR_ANY, &svrAddr.sin_addr);

	sockRet = bind(sockFd, (struct sockaddr*)&svrAddr, sizeof(svrAddr));
	if (sockRet < 0)
	{
		APP_LOG_ERROR("cannot bind!\r\n");
		ret = RET_SYSTEM_ERR;
		goto out;
	}

	sendTskHdl = sys_thread_new("PROT_T_UDP", protocol_udp_send_thread, 0, TASK_STACK_SIZE_PTOTOCOL_UDP, TASK_PRI_PTOTOCOL_UDP);
	if (sendTskHdl == INVALD_PTHREAD_HD)
	{
		ret = RET_NO_RESOURCE;
		goto out;
	}

	recvTskHdl = sys_thread_new("PROT_R_UDP", protocol_udp_recv_thread, 0, TASK_STACK_SIZE_PTOTOCOL_UDP, TASK_PRI_PTOTOCOL_UDP);
	if (recvTskHdl == INVALD_PTHREAD_HD)
	{
		ret = RET_NO_RESOURCE;
		goto out;
	}

	lock = malloc(sizeof(pthread_mutex_t));
	if (NULL == lock)
	{
		ret = RET_NO_RESOURCE;
		goto out;
	}
	
	ret = pthread_mutex_init(lock, NULL);
	if (0 != ret)
	{
		ret = RET_NO_RESOURCE;
		goto out;
	}

	attr.mq_maxmsg = PROTOCOL_COMMOM_MSGQ_LEN;
	attr.mq_msgsize = sizeof(protocol_common_msg_t);
	msgQ = mq_open("/g_protocolCommonMcb.udpMcb.msgQ", O_CREAT|O_RDWR|O_CLOEXEC, 0666, &attr);
	if (msgQ == INVALD_MSG_HD)
	{
		ret = RET_NO_RESOURCE;
		goto out;
	}

	g_protocolCommonMcb.udpMcb.msgQ = msgQ;
	g_protocolCommonMcb.udpMcb.lock = lock;
	g_protocolCommonMcb.udpMcb.recvTskHdl = recvTskHdl;
	g_protocolCommonMcb.udpMcb.sendTskHdl = sendTskHdl;
	g_protocolCommonMcb.udpMcb.sockFd = sockFd;

out:
	if (ret != RET_OK)
	{
		if (msgQ != INVALD_MSG_HD)
		{
			mq_close(msgQ);
			msgQ = INVALD_MSG_HD;
		}

		if (lock != INVALD_SEM_HD)
		{
			pthread_mutex_destroy(lock);
			free(lock);
			lock = INVALD_SEM_HD;
		}

		if (recvTskHdl != INVALD_PTHREAD_HD)
		{
			pthread_cancel(recvTskHdl);
			recvTskHdl = INVALD_PTHREAD_HD;
		}

		if (sendTskHdl != INVALD_PTHREAD_HD)
		{
			pthread_cancel(sendTskHdl);
			sendTskHdl = INVALD_PTHREAD_HD;
		}

		if (sockFd >= 0)
		{
			close(sockFd);
			sockFd = -1;
		}
	}

	APP_LOG_DEBUG("[%s:%d] ret=%d\r\n", __FUNCTION__, __LINE__, ret);
	return ret;
}

PRIVATE int32_t protocol_tcp_proc_msg(void)
{
	int32_t ret = TCP_MSG_PROC_RET_CONTINUE;
	uint16_t crcCalc = 0;
	uint8_t *buf = (uint8_t *)&g_protocolCommonMcb.tcpMcb.buf;
	protocol_info_head_t *head = (protocol_info_head_t *)&g_protocolCommonMcb.tcpMcb.buf;
	protocol_info_tail_t *tail = NULL;
	ret_code_t funcRet = RET_OK;

	/* check head of the message */
	if (g_protocolCommonMcb.tcpMcb.rcvIdx < sizeof(protocol_info_head_t))
	{
		ret = TCP_MSG_PROC_RET_CONTINUE;
		goto out;
	}

	if (head->infoSync != ntohl(INFO_HEAD_FLAG))
	{
		ret = TCP_MSG_PROC_RET_ERROR;
		goto out;
	}

	if (ntohl(head->infoLength) <= (TCP_INFO_HEAD_LEN + TCP_INFO_TAIL_LEN))
	{
		ret = TCP_MSG_PROC_RET_CONTINUE;
		goto out;
	}

	if (g_protocolCommonMcb.tcpMcb.rcvIdx < ntohl(head->infoLength))
	{
		ret = TCP_MSG_PROC_RET_CONTINUE;
		goto out;
	}

	/* check tail of the message */
	tail = (protocol_info_tail_t *)(buf + ntohl(head->infoLength) - TCP_INFO_TAIL_LEN);
	crcCalc = crc_16bits_compute(buf, ntohl(head->infoLength) - TCP_INFO_TAIL_LEN);
	if (crcCalc != ntohs(tail->crc))
	{
		APP_LOG_INFO("[%s:%d] crcCalc=0x%x isn't same as crcRecv=0x%x\r\n",
			__FUNCTION__, __LINE__, crcCalc, ntohs(tail->crc));
		/* whether the crc is used is decided in the specific info processor */
	}

	/* check body of the message */
	if (ntohs(head->infoType) == PIT_CFG_PARAM)
	{
		funcRet = protocol_cfg_param_process_data(buf, ntohl(head->infoLength));
		ret = (funcRet == RET_OK) ? (TCP_MSG_PROC_RET_FINISH) : (TCP_MSG_PROC_RET_ERROR);
	}
	/* check body of the message */
	if (ntohs(head->infoType) == AMP_CODE_PARAM)
	{
		funcRet = protocol_biaoding_data_process(buf, ntohl(head->infoLength) , ntohs(head->infoType) );
		ret = (funcRet == RET_OK) ? (TCP_MSG_PROC_RET_FINISH) : (TCP_MSG_PROC_RET_ERROR);
	}
	/* check body of the message */
	if (ntohs(head->infoType) == PHASE_DATA_PARAM)
	{
		funcRet = protocol_biaoding_data_process(buf, ntohl(head->infoLength) , ntohs(head->infoType) );
		ret = (funcRet == RET_OK) ? (TCP_MSG_PROC_RET_FINISH) : (TCP_MSG_PROC_RET_ERROR);
	}
	/* check body of the message */
	if (ntohs(head->infoType) == AMP_DATA_PARAM)
	{
		funcRet = protocol_biaoding_data_process(buf, ntohl(head->infoLength) , ntohs(head->infoType) );
		ret = (funcRet == RET_OK) ? (TCP_MSG_PROC_RET_FINISH) : (TCP_MSG_PROC_RET_ERROR);
	}

out:
	return ret;
}

ret_code_t protocol_tcp_send_data( uint8_t *pbyData, uint32_t uLen, uint32_t uTimeOut )
{
	ret_code_t ret = RET_OK;
	struct timespec time;
	struct timeval cur_time;
	uint32_t msec;

	if ((g_protocolCommonMcb.tcpMcb.iSocket < 0) || (pbyData == NULL))
	{
		ret = RET_INVALID_PARAM;
		goto out;
	}

	gettimeofday(&cur_time, NULL);
	msec = uTimeOut%1000;
	time.tv_sec = cur_time.tv_sec + uTimeOut/1000 + (cur_time.tv_usec + msec*1000)/1000;
	time.tv_nsec = (cur_time.tv_usec + msec*1000)%(1000*1000)*1000;
	if (0 == sem_timedwait(g_protocolCommonMcb.tcpMcb.lock, &time))
	{
		(void)send(g_protocolCommonMcb.tcpMcb.iSocket, pbyData, uLen, 0);
		sem_post(g_protocolCommonMcb.tcpMcb.lock);
	}

out:
	return ret;
}

PRIVATE ret_code_t protocol_tcp_send_ack(int sockFd, int32_t procRet)
{
	ret_code_t ret = RET_OK;
	protocol_ack_t ackMsg = {0};
	uint32_t result = 0;
	uint16_t crcCalc = 0;	
	struct timespec time;
	struct timeval cur_time;

	/* fill head */
	memcpy(&ackMsg.stInfoHeader, &g_protocolCommonMcb.tcpMcb.buf, sizeof(ackMsg.stInfoHeader));
	ackMsg.stInfoHeader.infoLength = htonl(sizeof(ackMsg));
	ackMsg.stInfoHeader.infoType = htons(PIT_ACK_INFO);
	ackMsg.stInfoHeader.infoVersion = PROTOCOL_ACK_VER;

	/* fill body */
	result = (procRet == TCP_MSG_PROC_RET_FINISH) ? 0 : 1;
	ackMsg.revResult = htonl(result);

	/* fill tail */
	crcCalc = crc_16bits_compute((uint8_t *)&ackMsg, sizeof(ackMsg) - TCP_INFO_TAIL_LEN);
	ackMsg.stInfoTail.crc = htons(crcCalc);
	
	gettimeofday(&cur_time, NULL);
	time.tv_sec = cur_time.tv_sec + 1;
	time.tv_nsec = cur_time.tv_usec * 1000;
	if (0 == sem_timedwait(g_protocolCommonMcb.tcpMcb.lock, &time))
	{
		ret = protocol_send_ack(sockFd, &ackMsg);
		sem_post(g_protocolCommonMcb.tcpMcb.lock);
	}

	return ret;
}

#define CREATE_TCP_SOCKET(sockFd)\
	if (sockFd >= 0) {\
		close(sockFd);\
		sockFd = -1;\
	}\
	do {\
		sockFd = socket(AF_INET, SOCK_STREAM, 0);\
		usleep(1000);\
	} while (sockFd < 0)

volatile static bool g_bFlagConnect = false;
volatile struct sockaddr_in channelAddr = {0};

ret_code_t protocol_tcp_send_data_net( uint8_t *pbyData, uint32_t uLen, uint32_t uTimeOut )
{
	ret_code_t ret = RET_OK;
	struct timespec time;
	struct timeval cur_time;
	uint32_t msec;

	if ((g_protocolCommonMcb.tcpMcb.iSocket < 0) || (pbyData == NULL) || (false == g_bFlagConnect) )
	{
		ret = RET_INVALID_PARAM;
		goto out;
	}
	
	gettimeofday(&cur_time, NULL);
	msec = uTimeOut%1000;
	time.tv_sec = cur_time.tv_sec + uTimeOut/1000 + (cur_time.tv_usec + msec*1000)/1000;
	time.tv_nsec = (cur_time.tv_usec + msec*1000)%(1000*1000)*1000;
	if (0 == sem_timedwait(g_protocolCommonMcb.tcpMcb.lock, &time))
	{
		(void)send(g_protocolCommonMcb.tcpMcb.iSocket, pbyData, uLen, 0);
		sem_post(g_protocolCommonMcb.tcpMcb.lock);
	}

out:
	return ret;
}

PRIVATE void protocol_tcp_thread(void *p_arg)
{
	struct sockaddr_in remoteAddr = {0};
	int32_t leftSpace = 0;
	int32_t recvLen = 0;
	int sockFd = -1;
	int32_t sockRet = 0;
	fd_set readSet;
	struct timeval timeout;
	uint8_t *buf = (uint8_t *)&g_protocolCommonMcb.tcpMcb.buf;
	int32_t procRet = TCP_MSG_PROC_RET_CONTINUE;
	ret_code_t ackRet = RET_OK;

	while (!g_protocolCommonMcb.inited)
	{
		usleep(1);
	}
	CREATE_TCP_SOCKET(sockFd);

	remoteAddr.sin_family = AF_INET;
	remoteAddr.sin_port = htons(TCP_COM_PORT_REMOTE);
	remoteAddr.sin_addr.s_addr = inet_addr(IP_ADDR_REMOTE);
	// inet_aton(IP_ADDR_REMOTE, &remoteAddr.sin_addr);

	timeout.tv_sec = 2;
	timeout.tv_usec = 0;

	while (1)
	{
		// connect to the server
		sockRet = connect(sockFd, (struct sockaddr *)&remoteAddr, sizeof(remoteAddr));
		if (sockRet < 0)
		{
			// re-create the socket when failed
			CREATE_TCP_SOCKET(sockFd);
			continue;
		}

		g_protocolCommonMcb.tcpMcb.iSocket = sockFd;
		FD_ZERO(&readSet);
		FD_SET(sockFd, &readSet);
		memset(&g_protocolCommonMcb.tcpMcb.buf, 0, sizeof(g_protocolCommonMcb.tcpMcb.buf));
		g_protocolCommonMcb.tcpMcb.rcvIdx = 0;
		buf = (uint8_t *)&g_protocolCommonMcb.tcpMcb.buf;

		while (1)
		{
			///alink_upload_request_channel(); //TODO: ??
			
			/* waiting for data */
			sockRet = select(sockFd + 1, &readSet, NULL, NULL, &timeout);
			if (sockRet == 0)
			{
				if(0 != eth_link_status("eth0", sockFd))
				{
					close(sockFd);
					sockFd = -1;
					g_protocolCommonMcb.tcpMcb.iSocket = sockFd;
					break;
				}
				// timeout, try again
				FD_ZERO(&readSet);
				FD_SET(sockFd, &readSet);
				continue;
			}
			else if (sockRet < 0)
			{
				close(sockFd);
				sockFd = -1;
				g_protocolCommonMcb.tcpMcb.iSocket = sockFd;
				// error, reconnect
				APP_LOG_ERROR("[%s:%d] sockRet=%d\r\n", __FUNCTION__, __LINE__, sockRet);
				break;
			}
			else
			{
				g_bFlagConnect = false;
				// read the data
				leftSpace = sizeof(g_protocolCommonMcb.tcpMcb.buf) - g_protocolCommonMcb.tcpMcb.rcvIdx;
				recvLen = recv(sockFd, buf, leftSpace, 0);
				if (recvLen > 0)
				{
					if( g_protocolCommonMcb.tcpMcb.pv_cbk_recv )
						g_protocolCommonMcb.tcpMcb.pv_cbk_recv( buf, recvLen );

//					g_protocolCommonMcb.tcpMcb.rcvIdx += recvLen;
//					buf += recvLen;

					int32_t sockRetTemp = sockFd;
					uint8_t byCountErr = 0;
					for( ; g_bFlagConnect; )
					{
						g_protocolCommonMcb.tcpMcb.iSocket = -1;
						remoteAddr.sin_family = channelAddr.sin_family;
						remoteAddr.sin_port = channelAddr.sin_port;
						remoteAddr.sin_addr.s_addr = channelAddr.sin_addr.s_addr;
						// connect to the server
						sockRet = connect(sockFd, (struct sockaddr *)&remoteAddr, sizeof(remoteAddr));
						if (sockRet < 0)
						{
							if( 5 < ++byCountErr )
							{
								remoteAddr.sin_family = AF_INET;
								remoteAddr.sin_port = htons(TCP_COM_PORT_REMOTE);
								remoteAddr.sin_addr.s_addr = inet_addr(IP_ADDR_REMOTE);
								close(sockFd);
								sockFd = sockRetTemp;
								g_protocolCommonMcb.tcpMcb.iSocket = sockFd;
								g_bFlagConnect = false;
								break;
							}
							else
							{
								// re-create the socket when failed
								CREATE_TCP_SOCKET(sockFd);
								usleep( 500*1000 );
								continue;
							}
						}

						g_protocolCommonMcb.tcpMcb.iSocket = sockFd;
						FD_ZERO(&readSet);
						FD_SET(sockFd, &readSet);
						memset(&g_protocolCommonMcb.tcpMcb.buf, 0, sizeof(g_protocolCommonMcb.tcpMcb.buf));
						g_protocolCommonMcb.tcpMcb.rcvIdx = 0;
						buf = (uint8_t *)&g_protocolCommonMcb.tcpMcb.buf;

						for( ; ; )
						{
							/* waiting for data */
							sockRet = select(sockFd + 1, &readSet, NULL, NULL, &timeout);
							if (sockRet == 0)
							{
								if (0 != eth_link_status("eth0", sockFd))
								{
									remoteAddr.sin_family = AF_INET;
									remoteAddr.sin_port = htons(TCP_COM_PORT_REMOTE);
									remoteAddr.sin_addr.s_addr = inet_addr(IP_ADDR_REMOTE);
									close(sockFd);
									sockFd = sockRetTemp;
									g_protocolCommonMcb.tcpMcb.iSocket = sockFd;
									g_bFlagConnect = false;
									break;
								}
								// timeout, try again
								FD_ZERO(&readSet);
								FD_SET(sockFd, &readSet);
								continue;
							}
							else if (sockRet < 0)
							{
								// error, reconnect
								APP_LOG_ERROR("[%s:%d] sockRet=%d\r\n", __FUNCTION__, __LINE__, sockRet);
								remoteAddr.sin_family = AF_INET;
								remoteAddr.sin_port = htons(TCP_COM_PORT_REMOTE);
								remoteAddr.sin_addr.s_addr = inet_addr(IP_ADDR_REMOTE);
								close(sockFd);
								sockFd = sockRetTemp;
								g_protocolCommonMcb.tcpMcb.iSocket = sockFd;
								g_bFlagConnect = false;
								break;
							}
							else
							{
								// read the data
								leftSpace = sizeof(g_protocolCommonMcb.tcpMcb.buf) - g_protocolCommonMcb.tcpMcb.rcvIdx;
								recvLen = recv(sockFd, buf, leftSpace, 0);
								if (recvLen > 0)
								{
									if( g_protocolCommonMcb.tcpMcb.pv_cbk_recv )
										g_protocolCommonMcb.tcpMcb.pv_cbk_recv( buf, recvLen );

//									g_protocolCommonMcb.tcpMcb.rcvIdx += recvLen;
//									buf += recvLen;
								}
								else
								{
									// disconnect, reconnect
									APP_LOG_INFO("[%s:%d] %d disconnect\r\n", __FUNCTION__, __LINE__, sockFd);
									remoteAddr.sin_family = AF_INET;
									remoteAddr.sin_port = htons(TCP_COM_PORT_REMOTE);
									remoteAddr.sin_addr.s_addr = inet_addr(IP_ADDR_REMOTE);
									close(sockFd);
									sockFd = sockRetTemp;
									g_protocolCommonMcb.tcpMcb.iSocket = sockFd;
									g_bFlagConnect = false;
									break;
								}
							}
						}
						break;
					}

					// check and process the data
//					procRet = protocol_tcp_proc_msg();
//					LOG_VERBOSE("[%s:%d] recvLen=%d, procRet=%d\r\n", __FUNCTION__, __LINE__, recvLen, procRet);
//					if (procRet != TCP_MSG_PROC_RET_CONTINUE)
//					{
//						/* send response to the host */
//						ackRet = protocol_tcp_send_ack(sockFd, procRet);
//						if (ackRet == RET_OK)
//						{
//							memset(&g_protocolCommonMcb.tcpMcb.buf, 0, sizeof(g_protocolCommonMcb.tcpMcb.buf));
//							g_protocolCommonMcb.tcpMcb.rcvIdx = 0;
//							buf = (uint8_t *)&g_protocolCommonMcb.tcpMcb.buf;
//							continue;
//						}
//						else
//						{
//							LOG_ERROR("[%s:%d] ackRet=%d\r\n", __FUNCTION__, __LINE__, ackRet);
//							break;
//						}
//					}
				}
				else
				{
					close(sockFd);
					sockFd = -1;
					g_protocolCommonMcb.tcpMcb.iSocket = sockFd;
					// disconnect, reconnect
					APP_LOG_INFO("[%s:%d] %d disconnect\r\n", __FUNCTION__, __LINE__, sockFd);
					break;
				}
			}
		}
	}
}

ret_code_t protocol_tcp_connect_channal( char* pstrIp, uint16_t wPort, uint32_t uTimeout )
{
	g_bFlagConnect = true;
	channelAddr.sin_family = AF_INET;
	channelAddr.sin_port = htons(wPort);
	channelAddr.sin_addr.s_addr = inet_addr(pstrIp);
}

PRIVATE void protocol_tcp_deinit(void)
{
	if (g_protocolCommonMcb.tcpMcb.tskHdl != INVALD_PTHREAD_HD)
	{
		pthread_cancel(g_protocolCommonMcb.tcpMcb.tskHdl);
		g_protocolCommonMcb.tcpMcb.tskHdl = INVALD_PTHREAD_HD;
	}

	return;
}

PRIVATE ret_code_t protocol_tcp_init(void)
{
	ret_code_t ret = RET_OK;
	sem_t* lock = INVALD_SEM_HD;
	pthread_t tskHdl = INVALD_PTHREAD_HD;

	tskHdl = sys_thread_new("PROT_TCP", protocol_tcp_thread, 0, TASK_STACK_SIZE_PTOTOCOL_TCP, TASK_PRI_PTOTOCOL_TCP);
	if (tskHdl == INVALD_PTHREAD_HD)
	{
		ret = RET_NO_RESOURCE;
		goto out;
	}

	lock = sem_open("/g_protocolCommonMcb.tcpMcb.lock", O_CREAT|O_RDWR|O_CLOEXEC, 0666, 1);
	if (SEM_FAILED == lock)
	{
		ret = RET_NO_RESOURCE;
		goto out;
	}

	g_protocolCommonMcb.tcpMcb.tskHdl = tskHdl;
	g_protocolCommonMcb.tcpMcb.iSocket = -1;
	g_protocolCommonMcb.tcpMcb.lock = lock;

out:
	if (ret != RET_OK)
	{
		if (tskHdl != INVALD_PTHREAD_HD)
		{
			pthread_cancel(tskHdl);
			tskHdl = INVALD_PTHREAD_HD;
		}
		if (lock != INVALD_SEM_HD)
		{
			sem_close(lock);
			lock = INVALD_SEM_HD;
		}
	}

	APP_LOG_DEBUG("[%s:%d] ret=%d\r\n", __FUNCTION__, __LINE__, ret);
	return ret;
}

ret_code_t protocol_common_init(void)
{
	ret_code_t ret = RET_OK;

	if (g_protocolCommonMcb.inited)
	{
		ret = RET_OK;
		goto out;
	}

	memset(&g_protocolCommonMcb, 0, sizeof(g_protocolCommonMcb));
	g_protocolCommonMcb.udpMcb.sockFd = -1;

	ret = protocol_udp_init();
	if (ret != RET_OK)
	{
		goto out;
	}

//	ret = protocol_tcp_init();
	if (ret != RET_OK)
	{
		goto out;
	}

	g_protocolCommonMcb.inited = true;

out:
 	if (ret != RET_OK)
 	{
 		protocol_tcp_deinit();
 		protocol_udp_deinit();
 	}

	APP_LOG_DEBUG("[%s:%d] ret=%d\r\n", __FUNCTION__, __LINE__, ret);
	return ret;
}

ret_code_t protocol_udp_send_data_blocking(void *data, uint32_t len)
{
	ret_code_t ret = RET_OK;

	//APP_LOG_DEBUG("[%s:%d] enter\r\n", __FUNCTION__, __LINE__);
	if ((data == NULL) || (len <= (UDP_INFO_HEAD_LEN + UDP_INFO_TAIL_LEN)))
	{
		ret = RET_INVALID_PARAM;
		goto out;
	}

	ret = protocol_udp_send_data(data, len);
	if (ret != RET_OK)
	{
		APP_LOG_ERROR("Send data failed %d\r\n", ret);
		goto out;
	}

out:
	return ret;
}

ret_code_t protocol_udp_send_data_nonblocking(void *data, uint32_t len, send_data_cb_fp cb_fp)
{
	ret_code_t ret = RET_OK;
	int osRet = 0;
	protocol_common_msg_t msg = {0};
	struct timeval cur_time;
	struct timespec time;

	///APP_LOG_DEBUG("[%s:%d] enter\r\n", __FUNCTION__, __LINE__);
	if ((data == NULL) || (len <= (UDP_INFO_HEAD_LEN + UDP_INFO_TAIL_LEN)))
	{
		ret = RET_INVALID_PARAM;
		goto out;
	}

	msg.data = data;
	msg.len = len;
	msg.cb_fp = cb_fp;

	gettimeofday(&cur_time, NULL);
	time.tv_sec = cur_time.tv_sec + (cur_time.tv_usec + 100*1000)/(1000*1000);
	time.tv_nsec = (cur_time.tv_usec + 100*1000)%(1000*1000)*1000;

	osRet = mq_timedsend(g_protocolCommonMcb.udpMcb.msgQ, (char *)&msg, sizeof(msg), 0, &time);
	if (osRet != 0)
	{
		APP_LOG_ERROR("Send msg to q failed %d.\r\n", osRet);
		ret = RET_SYSTEM_ERR;
	}

out:
	return ret;
}

ret_code_t protocol_tcp_register_recv_cbk( void (*pv_cbk)(uint8_t *pbyData, uint32_t uLen) )
{
	g_protocolCommonMcb.tcpMcb.pv_cbk_recv = pv_cbk;
}

