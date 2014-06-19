#pragma once

#include "lan.h"

#define NTP_SRV_PORT		htons(123)
#define NTP_LOCAL_PORT		htons(14444)

typedef struct ntp_timestamp {
	uint32_t seconds;
	uint32_t fraction;
} ntp_timestamp_t;

typedef struct ntp_message {
	uint8_t status;
	uint8_t type;
	uint16_t precision;
	uint32_t est_error;
	uint32_t est_drift_rate;
	uint32_t ref_clock_id;
	ntp_timestamp_t ref_timestamp;
	ntp_timestamp_t orig_timestamp;
	ntp_timestamp_t recv_timestamp;
	ntp_timestamp_t xmit_timestamp;
} ntp_message_t;

uint8_t ntp_request(uint32_t srv_ip);
uint32_t ntp_parse_reply(void *data, uint16_t len);
