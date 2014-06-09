/*
 * ww_state_machine.c
 *
 *  Created on: 11.02.2013
 *      Author: googy
 */
#include <avr/io.h>
#include <avr/eeprom.h>		// EEPROM
#include "vars2.h"
#include "ww_state_machine.h"
#include "shiftregister.h"
#include "defines.h"

void init_ww_from_eep() {
	opz.ww_opt.soll = eeprom_read_byte((uint8_t *) EEP_WW_SOLL);
	opz.ww_opt.active = eeprom_read_byte((uint8_t *) EEP_WW_ACTIVE);
	opz.ww_opt.diff = eeprom_read_byte((uint8_t *) EEP_WW_DIFF);
	opz.ww_opt.wait = eeprom_read_byte((uint8_t *) EEP_WW_WAIT);
	opz.ww_opt.state = 0;
	//source_soll = eeprom_read_byte((uint8_t *) EEP_ENERGY_SOURCE);
}

void ww_state_machine() {
	switch (opz.ww_opt.state) {
	case 0: //
		if (opz.ww_opt.active) {
			opz.ww_opt.state = 1;
			break;
		}
		break;
	case 1:
		// wenn Warmwasser deaktiviert, gehe in den Startzustand
		if (!opz.ww_opt.active) {
			opz.ww_opt.state = 0;
			shift &= ~(1 << WW); // Warmwasser Ladepumpe aus
			shift_set(shift);
			break;
		}

		// Warmwasser warm genug, gehe in den Wartezustand
		if (opz.ww_opt.ist >= opz.ww_opt.soll) {
			opz.ww_opt.state = 3;
			wwtimer = 0;
			shift &= ~(1 << WW); // Warmwasser Ladepumpe aus
			shift_set(shift);
			break;
		}

		// Kessel Messung durchführen
		// WW_ist mit Kesseltemperatur vergleichen und gegebenenfalls in Fehlerzustand oder einen gesonderten wechseln

		if ((temps.sp1 - opz.ww_opt.diff) <= opz.ww_opt.ist) {
			shift &= ~(1 << WW); // Warmwasser Ladepumpe aus
			shift_set(shift);
			wwtimer = 0;
			opz.ww_opt.state = 3;
			break;
		}

		// Holzkessel ist in Betrieb -> speichere bis max. Temp.
		if (temps.atmos >= 70 && temps.sp1 >= (opz.ww_opt.ist + opz.ww_opt.diff)) {
			shift |= (1 << WW); // Warmwasser Ladepumpe an
			shift_set(shift);
			wwtimer = 0;
			opz.ww_opt.state = 3;
			break;
		}
		break;

		if (temps.atmos < 70 && opz.ww_opt.ist < temps.sp2 - 2) {
			shift |= (1 << WW); // Warmwasser Ladepumpe an
			shift_set(shift);
			wwtimer = 0;
			opz.ww_opt.state = 3;
			break;
		}

		if (temps.atmos < 70 && opz.ww_opt.ist >= temps.sp2 - 1) {
			shift &= ~(1 << WW); // Warmwasser Ladepumpe aus
			shift_set(shift);
			wwtimer = 0;
			opz.ww_opt.state = 3;
			break;
		}
		break;
	case 2: // unbenutzt
		// Warmwasser inaktiv
		if (!opz.ww_opt.active) {
			opz.ww_opt.state = 0;
			shift &= ~(1 << WW); // Warmwasser Ladepumpe aus
			shift_set(shift);
			break;
		}

		// Warmwasser zu kalt, heizen
		if (opz.ww_opt.ist < (opz.ww_opt.soll - opz.ww_opt.diff)) {
			opz.ww_opt.state = 1;
			shift |= (1 << WW); // Warmwasser Ladepumpe an
			shift_set(shift);
			break;
		}
		break;

		//		Fehlerzustand
		//		noch keine Aktionen definiert
		//		nicht ausreichende Kesseltemperatur als Fehler

	case 3:
		// Warmwasser inaktiv
		if (!opz.ww_opt.active) {
			opz.ww_opt.state = 0;
			shift &= ~(1 << WW); // Warmwasser Ladepumpe aus
			shift_set(shift);
			break;
		}
		if (wwtimer >= opz.ww_opt.wait) {
			opz.ww_opt.state = 1;
		}
		break;
	default:
		opz.ww_opt.state = 3;
	}
}
