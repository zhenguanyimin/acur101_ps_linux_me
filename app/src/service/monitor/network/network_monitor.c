#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#include "../../storage/storage.h"
#include "../../sysport/sysport.h"
#include "../../../srv/net_cfg/net_cfg.h"
#include "network_monitor.h"
#include "../../../hal/led/led.h"

typedef struct network_status 
{
	char ip[RADAR_IP_STRING_MAX_LEN];
	char cable_connect_status; 
	char netcard_status;
	bool network_status;
	char mac[RADAR_NETCARD_MAC_MAC_LEN];

}network_status_t;

typedef struct monitor_network
{
	struct network_status netstatus;
	void*	pvModuleTask;
	void*	pvModuleTimer;
	void*	pvModuledispTimer;
}monitor_network_t;

static struct monitor_network sMonitorNetwork = { 0 };

static const char strNetworkLogName[] = "net";

#define monitor_network_log( strFormat, ...) \
    do{ \
        storage_log( strNetworkLogName, LOG_LEVEL_NOTICE, strFormat, ##__VA_ARGS__); \
    }while(0)

#define monitor_network_debug( strFormat, ...) \
    do{ \
        storage_log( strNetworkLogName, LOG_LEVEL_DEBUG, strFormat, ##__VA_ARGS__); \
    }while(0)

static int get_network_status(struct network_status *pns)
{
	int ret = 0;

	pns->netcard_status = get_netcard_status();
	pns->cable_connect_status = get_netlink_connect_status();
	pns->network_status = get_network_fault_status();
	get_netcard_mac(pns->mac);	
	get_netcard_ip(pns->ip);

	return ret;
}

static void monitor_network_read_handler( void* pvArg, void* pvInfo, uint32_t uSize )
{
	
	get_network_status(&sMonitorNetwork.netstatus);

	monitor_network_log(\
					"\r\nip: %s, mac: %s, netlink status: %s, netcard status: %s, fault:%d\r\n",\
		sMonitorNetwork.netstatus.ip,\
		sMonitorNetwork.netstatus.mac,\
		sMonitorNetwork.netstatus.cable_connect_status == NETLINK_STATUS_CONNECTED?"connected":"unconnected",\
		sMonitorNetwork.netstatus.netcard_status == NETLINK_STATUS_ENABLE?"link up":"link down",\
		sMonitorNetwork.netstatus.network_status);
}

static void monitor_network_read( void* pvArg )
{
	struct monitor_network* psModule = (struct monitor_network*)(&sMonitorNetwork);

	void* pvInfo = sysport_task_malloc( psModule->pvModuleTask, 0 );
	if( pvInfo )
	{
		sysport_task_func( psModule->pvModuleTask, monitor_network_read_handler, pvInfo );
	}
}

static void monitor_network_display_handler( void* pvArg, void* pvInfo, uint32_t uSize )
{
	static int last_netlink_status = -1;
	static bool led_s = false, last_network_status = false;
	
	if(sMonitorNetwork.netstatus.network_status)  {
		if(!last_network_status) {
			led0_red(true);
		}
		last_network_status = true;
		last_netlink_status = NETLINK_STATUS_DISCONNECTED;
		led_s = false;
	} else {	
		if(sMonitorNetwork.netstatus.cable_connect_status == NETLINK_STATUS_CONNECTED) {
			if(last_netlink_status != NETLINK_STATUS_CONNECTED) {
				led0_green(true);
			}
			last_netlink_status = NETLINK_STATUS_CONNECTED;
			led_s = false;
		} else {
			led_s = !led_s;	
			led0_green(led_s);
			last_netlink_status = NETLINK_STATUS_DISCONNECTED;
		}
		last_network_status = false;
	}
}

static void monitor_network_display( void* pvArg )
{
	struct monitor_network* psModule = (struct monitor_network*)(&sMonitorNetwork);

	void* pvInfo = sysport_task_malloc( psModule->pvModuleTask, 0 );
	if( pvInfo )
	{
		sysport_task_func( psModule->pvModuleTask, monitor_network_display_handler, pvInfo );
	}
}

int monitor_network_init( void* pvModuleTask )
{
	int eRet = 0;

	sMonitorNetwork.pvModuleTask = pvModuleTask;
	sMonitorNetwork.pvModuleTimer = sysport_timer_create("/monitor_network_timer", MONITOR_NETWORK_FREQ_TIME, 0, &sMonitorNetwork, monitor_network_read);
	sMonitorNetwork.pvModuledispTimer = sysport_timer_create("/monitor_network_display", MONITOR_NETWORK_DISPLAY_FREQ_TIME, 0, &sMonitorNetwork, monitor_network_display);
		
	return eRet;
}

