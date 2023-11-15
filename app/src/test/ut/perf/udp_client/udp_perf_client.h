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

#ifndef __UDP_PERF_CLIENT_H_
#define __UDP_PERF_CLIENT_H_

#include <stdint.h>

/* used as indices into kLabel[] */
enum {
	KCONV_UNIT,
	KCONV_KILO,
	KCONV_MEGA,
	KCONV_GIGA,
};

/* labels for formats [KMG] */
const char kLabel[] =
{
	' ',
	'K',
	'M',
	'G'
};

/* used as type of print */
enum measure_t {
	BYTES,
	SPEED
};

/* Report Type */
enum report_type {
	/* The Intermediate report */
	INTER_REPORT,
	/* The client side test is done */
	UDP_DONE_CLIENT,
	/* Remote side aborted the test */
	UDP_ABORTED_REMOTE
};

struct interim_report {
	uint64_t start_time;
	uint64_t last_report_time;
	uint32_t total_bytes;
};

struct perf_stats {
	uint8_t client_id;
	uint64_t start_time;
	uint64_t total_bytes;
	uint64_t cnt_datagrams;
	struct interim_report i_report;
};

/* seconds between periodic bandwidth reports */
#define INTERIM_REPORT_INTERVAL 5

/* Client port to connect */
#define UDP_CONN_PORT 5001

/* time in seconds to transmit packets */
#define UDP_TIME_INTERVAL 30

/* Server to connect with */
#define UDP_SERVER_IP_ADDRESS "192.168.235.88"

/* UDP buffer length in bytes */
#define UDP_SEND_BUFSIZE 1440

/* MAX UDP send retries */
#define MAX_SEND_RETRY 100 //10

/* Number of parallel UDP clients */
#define NUM_OF_PARALLEL_CLIENTS 1

/* Sleep in microseconds in case of UDP send errors */
#define ERROR_SLEEP 100

#endif /* __UDP_PERF_CLIENT_H_ */
