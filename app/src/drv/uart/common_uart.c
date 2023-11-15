#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <termios.h>
#include <string.h>
#include "common_uart.h"
#include "../../inc/radar_error.h"
#include "../../srv/log/log.h"

static int uart_set_opt(int fd, comm_cfg_t *cct)
{
	struct termios newtio = {0}, oldtio = {0};

	if(!cct) {
		return RET_INVALID_PARAM;
	}
	fcntl(fd,F_SETFL,0);
	if(tcgetattr(fd, &oldtio) != 0) { 
		perror("SetupSerial 1");
		return -1;
	}

	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag |= CLOCAL|CREAD; 
	newtio.c_cflag &= ~CSIZE; 

	switch(cct->nbits)
	{
		case 7:
			newtio.c_cflag |= CS7;
			break;
		case 8:
			newtio.c_cflag |= CS8;
			break;
		default:
			return RET_INVALID_PARAM;
	}

	switch(cct->nevent)
	{
		case 'O':
			newtio.c_cflag |= PARENB;
			newtio.c_cflag |= PARODD;
			newtio.c_iflag |= (INPCK | ISTRIP);
			break;
		case 'E':
			newtio.c_iflag |= (INPCK | ISTRIP);
			newtio.c_cflag |= PARENB;
			newtio.c_cflag &= ~PARODD;
			break;
		case 'N':
			newtio.c_cflag &= ~PARENB;
			break;
		default:
			return RET_INVALID_PARAM;
	}

	switch(cct->nspeed)
	{
		case 2400:
			cfsetispeed(&newtio, B2400);
			cfsetospeed(&newtio, B2400);
			break;
		case 4800:
			cfsetispeed(&newtio, B4800);
			cfsetospeed(&newtio, B4800);
			break;
		case 9600:
			cfsetispeed(&newtio, B9600);
			cfsetospeed(&newtio, B9600);
			break;
		case 115200:
			cfsetispeed(&newtio, B115200);
			cfsetospeed(&newtio, B115200);
			break;
		case 460800:
			cfsetispeed(&newtio, B460800);
			cfsetospeed(&newtio, B460800);
			break;
		default:
			cfsetispeed(&newtio, B9600);
			cfsetospeed(&newtio, B9600);
			break;
	}

	if(cct->nstop == 1) {
		newtio.c_cflag &=  ~CSTOPB;
	} else if (cct->nstop == 2) {
		newtio.c_cflag |=  CSTOPB;
	}

//	newtio.c_cc[VTIME] = 1;
//	newtio.c_cc[VMIN] = 0;
	tcflush(fd,TCIFLUSH);
	if((tcsetattr(fd,TCSANOW,&newtio))!=0) {
		perror("com set error");
		return -1;
	}
	APP_LOG_INFO("common uart set done!\n");

	return 0;
}

static int uart_open_port(char *comport)
{
	int ret = 0;
	long  vdisable = 0;
	int fd = 0;

	if(NULL == comport) {
		return RET_INVALID_PARAM;
	}

	fd = open(comport, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd < 0) {
		perror("Can't Open Serial Port");
		return RET_NOT_SUPPORT;
	}

	if(fcntl(fd, F_SETFL, 0) < 0) {
		APP_LOG_ERROR("fcntl failed!\n");
	} else {
		APP_LOG_INFO("fcntl passed\n") ;
	}

	return fd;
}

int common_uart_init(char *portdev, comm_cfg_t *cct)
{
	int ret = -1;
	int fd = 0;
	comm_cfg_t default_cct = {
		.nspeed = 115200,
		.nbits = 8,
		.nevent = 'N',
		.nstop = 1
	};
	comm_cfg_t *pcct = cct; 

	if(!pcct) {
		pcct = &default_cct;
	}
	ret = uart_open_port(portdev);
	if(0 > ret) {
		return ret;		
	}
	fd = ret;
	ret = uart_set_opt(fd, pcct);

	return fd;
}

int common_uart_uninit(int fd)
{
	if(fd > 0)
		close(fd);

	return 0;
}

int common_uart_read(int fd, char *rcv_buf, int rlen, unsigned int timeout_ms)
{
	int retval = -1;
	fd_set rfds;
	struct timeval tv;
	int ret = -1, pos = 0;

	tv.tv_sec = 0;
	tv.tv_usec = timeout_ms * 1000; //ms
	pos = 0; // point to rceeive buf
	do {
		FD_ZERO(&rfds);
		FD_SET(fd, &rfds);
		retval = select(fd+1 , &rfds, NULL, NULL, &tv);
		if (retval < 0) {
			perror("select error");
			break;
		} else if(retval > 0) {
			ret = read(fd, rcv_buf+pos, rlen);
			pos += ret;
		} else {
	//		APP_LOG_INFO("uart no data\n");
			//break;
		}
	} while(pos < rlen);
	//APP_LOG_INFO("return pos data len = %d\n", pos);

	return pos;
}

int common_uart_send(int fd, char *send_buf, unsigned int slen)
{
	ssize_t ret;

	ret = write(fd, send_buf, slen);
	if (ret < 0) {
		APP_LOG_ERROR("write device error %d\n", ret);
		return -1;
	}

	return slen;
}


