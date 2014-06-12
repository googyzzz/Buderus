/*
 * source_state_machine.h
 *
 *  Created on: 09.06.2014
 *      Author: googy_000
 */

#ifndef SOURCE_STATE_MACHINE_H_
#define SOURCE_STATE_MACHINE_H_

//uint8_t volatile source_timer = 0;

// Energiequelle
enum source_type{
	HOLZ,
	HEIZOEL,
	UNBEKANNT,
	OFF
};

enum source_type source_ist = UNBEKANNT;
enum source_type source_soll = UNBEKANNT;	// in EEPROM ablegen
enum source_type source_turn = OFF;

void source_state_machine();

#endif /* SOURCE_STATE_MACHINE_H_ */
