#ifndef __COMMON_UART_H__
#define __COMMON_UART_H__

#define GYRO_UART_DEV "/dev/ttyUL1"

typedef struct comm_cfg {
	int nspeed;
	int nbits;
	char nevent;
	int nstop;
}comm_cfg_t;

int common_uart_read(int fd, char *rcv_buf, int rlen, unsigned int timeout_ms);
int common_uart_init(char *portdev, comm_cfg_t *cct);
int common_uart_send(int fd, char *send_buf, unsigned int slen);
int common_uart_uninit(int fd);

#endif

