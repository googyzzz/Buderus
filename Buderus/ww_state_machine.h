/*
 * ww_state_machine.h
 *
 *  Created on: 11.02.2013
 *      Author: googy
 */

#ifndef WW_STATE_MACHINE_H_
#define WW_STATE_MACHINE_H_

#define WW 2

extern uint8_t WW_soll; // read/write, Solltemperatur
extern uint8_t WW_ist; // read only, aktuelle Temperatur (nur bei aktiviertem Warmwasser auslesen ansonsten besser gesondert)
extern uint8_t WW_state; // read only, aktueller Zustand
extern uint8_t WW_active; // read/write, Aktivierungsflag
extern uint8_t WW_diff; // read/write, offset für erneute Aufwärmung
extern uint8_t WW_wait; // read/write

//OneWire Sensoren Wertvariable
extern uint8_t Holzkessel;
extern uint8_t Speicher0;
extern uint8_t Speicher1;
extern uint8_t Speicher2;
extern uint8_t Speicher3;
extern uint8_t Speicher4;
extern uint8_t experimentell;

extern volatile uint16_t shift;
extern volatile uint8_t WW_timer;

void ww_state_machine();

#endif /* WW_STATE_MACHINE_H_ */
