#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include "../../inc/radar_error.h"
#include "../../srv/log/log.h"
#include "led.h"

#define LED_SYS_NODE_NODE 		"/sys/class/leds"
#define LED_NODE_STR_MAX_LEN	256
#define LED0_GREEN_DIR_NAME 	"green:"
#define LED1_GREEN_DIR_NAME 	"green:_1"
#define LED0_RED_DIR_NAME 		"red:"
#define LED1_RED_DIR_NAME 		"red:_1"
#define LED0_BLUE_DIR_NAME 		"blue:"
#define LED1_BLUE_DIR_NAME 		"blue:_1"
#define LEDX_OPR_FILENAME		"brightness"

static int led_node_set(char *node, LEDX_STATUS val)
{
	char sval = val & 0xff;
	int fd = 0;
	int ret = -1;

	fd = open(node, O_RDWR, 0777);
	if(0 <= fd) {
		ret = write(fd, &sval, sizeof(char));
		if(ret <= 0) {
			APP_LOG_ERROR("write error, %d\n", ret);
			perror("led write:");
		}
	} else {
		return fd;
	}
	close(fd);

	return 0;
}

static int led_turn_status(LEDX ledx, LEDX_STATUS is_on)
{
	char node[LED_NODE_STR_MAX_LEN] = {0};
	int ret = -1;

	switch(ledx) {
		case LED0_GREEN:
			sprintf(node, "%s/%s/%s", LED_SYS_NODE_NODE, LED0_GREEN_DIR_NAME, LEDX_OPR_FILENAME);		
			break;
		case LED0_RED:
			sprintf(node, "%s/%s/%s", LED_SYS_NODE_NODE, LED0_RED_DIR_NAME, LEDX_OPR_FILENAME);		
			break;
		case LED0_BLUE:
			sprintf(node, "%s/%s/%s", LED_SYS_NODE_NODE, LED0_BLUE_DIR_NAME, LEDX_OPR_FILENAME);		
			break;
		case LED1_GREEN:
			sprintf(node, "%s/%s/%s", LED_SYS_NODE_NODE, LED1_GREEN_DIR_NAME, LEDX_OPR_FILENAME);		
			break;
		case LED1_RED:
			sprintf(node, "%s/%s/%s", LED_SYS_NODE_NODE, LED1_RED_DIR_NAME, LEDX_OPR_FILENAME);		
			break;
		case LED1_BLUE:
			sprintf(node, "%s/%s/%s", LED_SYS_NODE_NODE, LED1_BLUE_DIR_NAME, LEDX_OPR_FILENAME);		
			break;
		default:
			return -1;
	}
//	APP_LOG_DEBUG("led operation node: %s\n", node);
	ret = led_node_set(node, is_on);

	return ret;
}

int led_on_off(LEDX ledx, LEDX_STATUS is_on)
{
	return led_turn_status(ledx, is_on);
}

void led0_green(bool on_off)
{
	led_turn_status(LED0_GREEN, on_off?LED_ON:LED_OFF);
}

void led1_green(bool on_off)
{
	led_turn_status(LED1_GREEN, on_off?LED_ON:LED_OFF);
}

void led0_red(bool on_off)
{
	led_turn_status(LED0_RED, on_off?LED_ON:LED_OFF);
}

void led1_red(bool on_off)
{
	led_turn_status(LED1_RED, on_off?LED_ON:LED_OFF);
}

void led0_blue(bool on_off)
{
	led_turn_status(LED0_BLUE, on_off?LED_ON:LED_OFF);
}

void led1_blue(bool on_off)
{
	led_turn_status(LED1_BLUE, on_off?LED_ON:LED_OFF);
}

