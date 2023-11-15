#ifndef __NET_CFG_H__
#define __NET_CFG_H__

#define RADAR_NETCARD_MAC_MAC_LEN	128
#define RADAR_DEFAULT_IP		"192.168.235.55"
#define RADAR_IP_STRING_MAX_LEN 64 
#define RADAR_NET_CFG_FLAG_LEN  8
#define NETLINK_STATUS_ENABLE	1
#define NETLINK_STATUS_DISABLE  2
#define RADAR_NETCARD_NAME_MAX_LEN	16
#define NETLINK_STATUS_CONNECTED    1
#define NETLINK_STATUS_DISCONNECTED	0

int radar_network_cmd_flag_get(char *flag, char *ip);
int radar_network_enable_flag(const char *flag,const char *ip);
int radar_network_init(void);
int get_netlink_connect_status(void);
int radar_network_cfg_uninit(void);
int get_netcard_status(void);
void get_netcard_ip(unsigned char *netcard_ip);
int get_netcard_mac(unsigned char *netcard_mac);
bool get_network_fault_status(void);

#endif

