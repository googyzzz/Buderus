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

extern uint8_t HK1_ist;
extern uint8_t HK1_active;
extern uint8_t HK1_diff;		// read:write
extern uint8_t HK1_soll;
extern uint8_t HK1_wait;		// read/write
extern uint8_t HK1_state;		// read

extern uint8_t HK2_state;		// read only, aktueller Zustand
extern uint8_t HK2_soll;		// read/write, Solltemperatur
extern uint8_t HK2_ist;		// read only, aktuelle Temperatur (nur der letzte gemessene Wert, nur bei aktivem Heizkreis verwenden. [state 1, 2 oder 3])
extern uint8_t HK2_diff;		// read/write, Regelgenauigkeit in Grad
extern uint8_t HK2_active;	// read/write, Aktivierungsflag
extern uint8_t HK2_wait;		// read/write, Wartezeit zwischen den Messungen
extern uint8_t HK2_present;	// read only

extern struct temperature_fine arbeitsZimmer;
extern struct temperature_fine schlafZimmer;

extern uint8_t energy_source;

#define EEP_HK1_SOLL	128
#define EEP_HK1_ACTIVE	129
#define EEP_HK1_DIFF	130
#define EEP_HK1_WAIT	131
// Warmwasser Parameter
#define EEP_WW_SOLL		132
#define EEP_WW_ACTIVE	133
#define EEP_WW_DIFF		134
#define EEP_WW_WAIT		135
// Heizkreis 2 Parameter
#define EEP_HK2_SOLL	136
#define EEP_HK2_DIFF	137
#define EEP_HK2_ACTIVE	139
#define EEP_HK2_WAIT	140

#define EEP_ENERGY_SOURCE 141

#define BRENNER 0
#define HK1 1
#define WW 2
#define PZ 3
#define MW 4
#define MK 5
#define HK2 6

extern uint8_t source_ist;
extern uint8_t source_soll;

void udp_packet(eth_frame_t *frame, uint16_t len);

#endif /* NETCOM_H_ */
