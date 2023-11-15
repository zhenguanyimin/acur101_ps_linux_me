#ifndef __LED_H__
#define __LED_H__

typedef enum {
	LED_OFF = 0x30,
	LED_ON,

} LEDX_STATUS;

typedef enum {
	LED0_GREEN = 0,
	LED0_RED,
	LED0_BLUE,
	LED1_GREEN,
	LED1_RED,
	LED1_BLUE,

	LEDX_UNDEFINED = -1,
} LEDX; 

int led_on_off(LEDX ledx, LEDX_STATUS is_on);
void led1_blue(bool on_off);
void led0_blue(bool on_off);
void led1_red(bool on_off);
void led0_red(bool on_off);
void led1_green(bool on_off);
void led0_green(bool on_off);

#endif

