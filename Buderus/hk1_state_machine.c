/*
 * hk1_state_machine.c
 *
 *  Created on: 09.06.2014
 *      Author: googy_000
 */
#include <avr/eeprom.h>		// EEPROM
#include "hk1_state_machine.h"
#include "defines.h"
#include "vars2.h"

void init_hk1_eep() {	// init Parameter
	opz.hk1_opt.soll = eeprom_read_byte((uint8_t *) EEP_HK1_SOLL);
	opz.hk1_opt.active = eeprom_read_byte((uint8_t *) EEP_HK1_ACTIVE);
	opz.hk1_opt.diff = eeprom_read_byte((uint8_t *) EEP_HK1_DIFF);
	opz.hk1_opt.wait = eeprom_read_byte((uint8_t *) EEP_HK1_WAIT);
}

void hk1_state_machine() {
	//---------------------------------------------------------------------------------------------
	//-----------------state machine Heizkreis 1-----------------------------
	/*		switch (HK1_state) {
	 case 0:
	 if (HK1_active) {
	 HK1_state = 1;
	 break;
	 }
	 break;

	 case 1:
	 // Heizkreis 1 deaktiviert
	 if (!HK1_active){
	 HK1_state = 0;
	 shift &= ~(1 << HK1);
	 shift_set(shift);
	 break;
	 }

	 if (HK1_ist > (HK1_soll + HK1_diff)) {
	 shift &= ~(1 << HK1);	// zu warm, Pumpe aus
	 shift_set(shift);
	 }
	 if (HK1_ist < (HK1_soll - HK1_diff)) {
	 shift |= (1 << HK1);	// zu kalt, Pumpe an
	 shift_set(shift);
	 }
	 HK1_timer = 0;
	 HK1_state = 2;	// warten
	 break;

	 // Warten
	 case 2:
	 // Heizkreis 1 deaktiviert
	 if (!HK1_active){
	 HK1_state = 0;
	 shift &= ~(1 << HK1);
	 shift_set(shift);
	 break;
	 }
	 // timeout
	 if (HK1_timer > HK1_wait) {
	 HK1_state = 1;
	 break;
	 }
	 break;
	 default: HK1_state = 0;
	 }
	 */

}
