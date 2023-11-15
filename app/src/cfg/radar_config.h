#ifndef RADAR_CONFIG_H
#define RADAR_CONFIG_H

/* version information */
#ifndef EMBED_SOFTWARE_PS_VERSION_STR
#define EMBED_SOFTWARE_PS_VERSION_STR "ACUR100-T5-V99.99.99"
#endif

#define EMBED_ALINK_VERSION_STR "protocol_alink_upgrade_v0.1.0.2"

// note: radar board IP address is configured in network_thread

/* UDP/TCP communication configuration */
#define IP_ADDR_LOCAL       "192.168.235.55"
#define IP_ADDR_REMOTE      "192.168.235.88"

//#define IP_ADDR_LOCAL       "170.20.10.10"
//#define IP_ADDR_REMOTE      "170.20.10.1"

#define UDP_COM_PORT_LOCAL  (6000) // the udp port of radar
#define UDP_COM_PORT_REMOTE (8000) // the udp port of host
#define UDP_PACKET_LEN_MAX  (1472)
#define UDP_PACKET_HEAD_LEN (20)
#define UDP_INFO_HEAD_LEN   (24)
#define UDP_INFO_TAIL_LEN   (4) // reserved(2B)+CRC16(2B)
#define UDP_INFO_LEN_MAX    ((UDP_PACKET_LEN_MAX) - (UDP_PACKET_HEAD_LEN))
#define TCP_INFO_HEAD_LEN   (24)
#define TCP_INFO_TAIL_LEN   (4) // reserved(2B)+CRC16(2B)

//#define TCP_COM_PORT_REMOTE (7000) // the tcp port of host
#define TCP_COM_PORT_REMOTE (8060) // the tcp port of host

typedef enum radar_task_pri {
	TASK_PRI_APP_INIT = 2,
	TASK_PRI_NETWORK_CFG = 1,
	TASK_PRI_CLI_SERVER = 5,
	TASK_PRI_ETH_RCV = 4,
	TASK_PRI_PTOTOCOL_UDP = 3,
	TASK_PRI_PTOTOCOL_TCP = 3,
	TASK_PRI_ALG_DETECTION = 3,
	TASK_PRI_ALG_TRACKING = 4,
	TASK_PRI_GPS = 2,
	TASK_PRI_IMU = 2,
	TASK_PRI_WIFI_BLE = 3,
} radar_task_pri_t;

typedef enum radar_task_stack_size {
	TASK_STACK_SIZE_APP_INIT = 1024,
	TASK_STACK_SIZE_NETWORK_CFG = 1024,
	TASK_STACK_SIZE_CLI_SERVER = 2048,
	TASK_STACK_SIZE_ETH_RCV = 1024,
	TASK_STACK_SIZE_PTOTOCOL_UDP = 1024,
	TASK_STACK_SIZE_PTOTOCOL_TCP = 1024,
	TASK_STACK_SIZE_ALG_DETECTION = 1024,
	TASK_STACK_SIZE_ALG_TRACKING = 2048,
	TASK_STACK_SIZE_GPS = 1024,
	TASK_STACK_SIZE_IMU = 1024*8,
	TASK_STACK_SIZE_WIFI_BLE = 512,
} radar_task_stack_size_t;

#endif /* RADAR_CONFIG_H */
