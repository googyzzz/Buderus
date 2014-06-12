/*
 * netcom.h
 *
 *  Created on: 11.02.2013
 *      Author: googy
 */

#ifndef NETCOM_H_
#define NETCOM_H_

//#include "types.h"
#include "lan.h"

uint32_t time_offset;	// netcom, main uncommented
volatile uint32_t second_count;	// netcom, timer0, main uncommented
volatile uint32_t ntp_next_update;	//netcom, main uncommented

void udp_packet(eth_frame_t *frame, uint16_t len);

#endif /* NETCOM_H_ */
