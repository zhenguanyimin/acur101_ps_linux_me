/*
 * Copyright (C) 2017 - 2019 Xilinx, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 */

/* Connection handle for a UDP Client session */
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <fcntl.h>
#include "udp_perf_client.h"

static struct perf_stats client;
static char send_buf[UDP_SEND_BUFSIZE];
static struct sockaddr_in addr;
static int sock[NUM_OF_PARALLEL_CLIENTS];
#define FINISH	1
/* Report interval time in ms */
#define REPORT_INTERVAL_TIME (INTERIM_REPORT_INTERVAL * 1000)
/* End time in ms */
#define END_TIME (UDP_TIME_INTERVAL * 1000)

//#define UDP_PERF_LOG(...) xil_printf(__VA_ARGS__)

static char *pcLog = NULL;
static uint32_t uiLogLen = 0;
#define UDP_PERF_LOG(...) udp_perf_log(__VA_ARGS__)

static void udp_perf_log(char *fmt, ...)
{
    va_list args;

    if ((NULL == pcLog) || (uiLogLen <= strlen(pcLog)))
	{
    	return ;
	}

    va_start(args, fmt);
    vsnprintf(pcLog+strlen(pcLog), uiLogLen - strlen(pcLog), fmt, args);
    va_end(args);
}

static uint64_t sys_now(void)
{
	uint64_t time_ms;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	time_ms = tv.tv_sec * 1000 + tv.tv_usec/1000;
	return time_ms;
}

void print_app_header(void)
{
	UDP_PERF_LOG("UDP client connecting to %s on port %d\r\n",
			UDP_SERVER_IP_ADDRESS, UDP_CONN_PORT);
	UDP_PERF_LOG("On Host: Run $iperf -s -i %d -u\r\n\r\n",
			INTERIM_REPORT_INTERVAL);
}

static void print_udp_conn_stats(void)
{
#if 0
	UDP_PERF_LOG("[%3d] local %s port %d connected with ",
			client.client_id, inet_ntoa(server_netif.ip_addr),
			UDP_CONN_PORT);
#endif
	UDP_PERF_LOG("%s port %d\r\n", UDP_SERVER_IP_ADDRESS,
			UDP_CONN_PORT);
	UDP_PERF_LOG("[ ID] Interval\t\tTransfer   Bandwidth\n\r");
}

static void stats_buffer(char* outString,
		double data, enum measure_t type)
{
	int conv = KCONV_UNIT;
	const char *format;
	double unit = 1024.0;

	if (type == SPEED)
		unit = 1000.0;

	while (data >= unit && conv <= KCONV_GIGA) {
		data /= unit;
		conv++;
	}

	/* Fit data in 4 places */
	if (data < 9.995) { /* 9.995 rounded to 10.0 */
		format = "%4.2f %c"; /* #.## */
	} else if (data < 99.95) { /* 99.95 rounded to 100 */
		format = "%4.1f %c"; /* ##.# */
	} else {
		format = "%4.0f %c"; /* #### */
	}
	sprintf(outString, format, data, kLabel[conv]);
}


/* The report function of a UDP client session */
static void udp_conn_report(uint64_t diff,
		enum report_type report_type)
{
	uint64_t total_len;
	double duration, bandwidth = 0;
	char data[16], perf[16], time[64];

	if (report_type == INTER_REPORT) {
		total_len = client.i_report.total_bytes;
	} else {
		client.i_report.last_report_time = 0;
		total_len = client.total_bytes;
	}

	/* Converting duration from milliseconds to secs,
	 * and bandwidth to bits/sec .
	 */
	duration = diff / 1000.0; /* secs */
	if (duration)
		bandwidth = (total_len / duration) * 8.0;

	stats_buffer(data, total_len, BYTES);
	stats_buffer(perf, bandwidth, SPEED);

	/* On 32-bit platforms, xil_printf is not able to print
	 * u64_t values, so converting these values in strings and
	 * displaying results
	 */
	sprintf(time, "%4.1f-%4.1f sec",
			(double)client.i_report.last_report_time,
			(double)(client.i_report.last_report_time + duration));
	UDP_PERF_LOG("[%3d] %s  %sBytes  %sbits/sec\n\r", client.client_id,
			time, data, perf);

	if (report_type == INTER_REPORT)
		client.i_report.last_report_time += duration;
	else
		UDP_PERF_LOG("[%3d] sent %llu datagrams\n\r",
				client.client_id, client.cnt_datagrams);
}


static void reset_stats(void)
{
	client.client_id++;

	/* Print connection statistics */
	print_udp_conn_stats();

	/* Save start time for final report */
	client.start_time = sys_now();
	client.total_bytes = 0;
	client.cnt_datagrams = 0;

	/* Initialize Interim report parameters */
	client.i_report.start_time = 0;
	client.i_report.total_bytes = 0;
	client.i_report.last_report_time = 0;
}

static int udp_packet_send(uint8_t finished)
{
	static int packet_id;
	int i, count, *payload;
	uint8_t retries = MAX_SEND_RETRY;
	socklen_t len = sizeof(addr);

	payload = (int*) (send_buf);
	if (finished == FINISH)
		packet_id = -1;
	*payload = htonl(packet_id);

	/* always increment the id */
	packet_id++;

	for (i = 0; i < NUM_OF_PARALLEL_CLIENTS; i++) {
		while (retries) {
			payload[1] = MAX_SEND_RETRY - retries;

#if LWIP_UDP_OPT_BLOCK_TX_TILL_COMPLETE
			count = lwip_sendto_blocking(sock[i], send_buf, sizeof(send_buf), 0,
					(struct sockaddr *)&addr, len);
#else
			count = sendto(sock[i], send_buf, sizeof(send_buf), 0,
								(struct sockaddr *)&addr, len);
#endif
			if (count <= 0) {
				retries--;
				usleep(ERROR_SLEEP);
			} else {
				client.total_bytes += count;
				client.cnt_datagrams++;
				client.i_report.total_bytes += count;
				break;
			}
		}

		if (!retries) {
			/* Terminate this app */
			uint64_t now = sys_now();
			uint64_t diff_ms = now - client.start_time;
			UDP_PERF_LOG("Too many udp_send() retries, ");
			UDP_PERF_LOG("Terminating application\n\r");
			udp_conn_report(diff_ms, UDP_DONE_CLIENT);
			UDP_PERF_LOG("UDP test failed\n\r");
			return -1;
		}
		retries = MAX_SEND_RETRY;

		/* For ZynqMP GEM, At high speed, packets are being
		 * received as Out of order at Iperf server running
		 * on remote host machine.
		 * To avoid this, added delay of 1us between each
		 * packets
		 */
#if !LWIP_UDP_OPT_BLOCK_TX_TILL_COMPLETE
#if defined (__aarch64__) && defined (XLWIP_CONFIG_INCLUDE_GEM)
		///usleep(1);
#endif /* __aarch64__ */
#endif
	}
	return 0;
}

/* Transmit data on a udp session */
int transfer_data(void)
{
	if (END_TIME || REPORT_INTERVAL_TIME) {
		uint64_t now = sys_now();
		if (REPORT_INTERVAL_TIME) {
			if (client.i_report.start_time) {
				uint64_t diff_ms = now - client.i_report.start_time;
				if (diff_ms >= REPORT_INTERVAL_TIME) {
					udp_conn_report(diff_ms, INTER_REPORT);
					client.i_report.start_time = 0;
					client.i_report.total_bytes = 0;
				}
			} else {
				client.i_report.start_time = now;
			}
		}

		if (END_TIME) {
			/* this session is time-limited */
			uint64_t diff_ms = now - client.start_time;
			if (diff_ms >= END_TIME) {
				/* time specified is over,
				 * close the connection */
				udp_packet_send(FINISH);
				udp_conn_report(diff_ms, UDP_DONE_CLIENT);
				UDP_PERF_LOG("UDP test passed Successfully\n\r");
				return FINISH;
			}
		}
	}

	if (udp_packet_send(!FINISH) < 0)
		return FINISH;

	return 0;
}

int udp_perf_client(char *pcWriteBuffer, size_t xWriteBufferLen)
{
	int err = 0;
	uint32_t i;

	pcLog = pcWriteBuffer;
	uiLogLen = xWriteBufferLen;

	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(UDP_CONN_PORT);
	addr.sin_addr.s_addr = inet_addr(UDP_SERVER_IP_ADDRESS);

	for (i = 0; i < NUM_OF_PARALLEL_CLIENTS; i++) {
		sock[i] = -1;
	}

	for (i = 0; i < NUM_OF_PARALLEL_CLIENTS; i++) {
		if ((sock[i] = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			UDP_PERF_LOG("UDP client: Error creating Socket\r\n");
			err = sock[i];
			goto out;
		}

		err = connect(sock[i], (struct sockaddr *)&addr, sizeof(addr));
		if (err != 0) {
			UDP_PERF_LOG("UDP client: Error on connect: %d\r\n", err);
			goto out;
		}
	}

	/* Wait for successful connections */
	usleep(10);

	reset_stats();

	/* initialize data buffer being sent with same as used in iperf */
	for (i = 0; i < UDP_SEND_BUFSIZE; i++)
		send_buf[i] = (i % 10) + '0';

	do
	{
		err = !transfer_data();
	}while (err);

	for (i = 0; i < NUM_OF_PARALLEL_CLIENTS; i++) {
		if (sock[i] >= 0) {
			close(sock[i]);
			sock[i] = -1;
		}
	}

	out:
	pcLog = NULL;
	uiLogLen = 0;
	return (int)err;
}
