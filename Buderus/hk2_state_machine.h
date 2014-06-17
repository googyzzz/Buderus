/*
 * hk2_state_machine.h
 *
 *  Created on: 11.02.2013
 *      Author: googy
 */

#ifndef HK2_STATE_MACHINE_H_
#define HK2_STATE_MACHINE_H_

extern uint8_t HK2_state;		// read only, aktueller Zustand
extern uint8_t HK2_soll;		// read/write, Solltemperatur
extern uint8_t HK2_ist;		// read only, aktuelle Temperatur (nur der letzte gemessene Wert, nur bei aktivem Heizkreis verwenden. [state 1, 2 oder 3])
extern uint8_t HK2_diff;		// read/write, Regelgenauigkeit in Grad
extern uint8_t HK2_active;	// read/write, Aktivierungsflag
extern uint8_t HK2_wait;		// read/write, Wartezeit zwischen den Messungen
extern uint8_t HK2_present;	// read only
extern uint8_t HK2_rotate;
extern uint8_t ROTATION_TIME;

extern volatile uint16_t shift;

extern volatile uint8_t HK2_timer;

void hk2_state_machine();

#endif /* HK2_STATE_MACHINE_H_ */
