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
extern volatile uint32_t second_count;
extern uint8_t diesel_t;
extern uint16_t brenner_status;
extern uint8_t temp_warmwasser;
extern uint8_t temp_hk2_vorlauf;
extern volatile uint16_t shift;
extern uint8_t source_soll;

extern struct temperature_fine arbeitsZimmer;
extern struct temperature_fine schlafZimmer;

extern uint8_t energy_source;

extern uint8_t source_ist;
extern uint8_t source_soll;

void udp_packet(eth_frame_t *frame, uint16_t len);

#endif /* NETCOM_H_ */
