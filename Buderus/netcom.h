/*
 * netcom.h
 *
 *  Created on: 11.02.2013
 *      Author: googy
 */

#ifndef NETCOM_H_
#define NETCOM_H_

#include "types.h"

volatile uint32_t ntp_next_update;
uint32_t time_offset;
extern volatile uint16_t shift;

extern struct temperature_fine arbeitsZimmer;
extern struct temperature_fine schlafZimmer;

extern uint8_t source_ist;

void udp_packet(eth_frame_t *frame, uint16_t len);

#endif /* NETCOM_H_ */
