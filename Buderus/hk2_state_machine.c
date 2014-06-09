/*
 * hk2_state_machine.c
 *
 *  Created on: 11.02.2013
 *      Author: googy
 */
#include <avr/io.h>
#include <avr/eeprom.h>		// EEPROM
#include "shiftregister.h"
#include "hk2_state_machine.h"
#include "defines.h"
#include "vars2.h"

void init_hk2_eep() {
	opz.hk2_opt.soll = eeprom_read_byte((uint8_t *) EEP_HK2_SOLL);
	opz.hk2_opt.diff = eeprom_read_byte((uint8_t *) EEP_HK2_DIFF);
	opz.hk2_opt.rotation_tume = eeprom_read_byte((uint8_t *) EEP_ROTATION_TIME);
	opz.hk2_opt.active = eeprom_read_byte((uint8_t *) EEP_HK2_ACTIVE);
	opz.hk2_opt.wait = eeprom_read_byte((uint8_t *) EEP_HK2_WAIT);
}

void hk2_state_machine() {
	switch (opz.hk2_opt.state) {
	case 0:
		if (opz.hk2_opt.active) {
			shift |= (1 << HK2); // Bodenheizung Pumpe an
			shift_set(shift);
			opz.hk2_opt.state = 1;
			HK2_timer = 0;
			break;
		}
		break;

		//			 * Wartezustand
		//			 * hier wird gewartet zwischen den Korrekturvorgängen und der Zustand geprüfft

	case 1:
		// HK2 deaktiviert
		if (!opz.hk2_opt.active) {
			shift &= ~(1 << HK2); // Bodenheizung Pumpe aus
			shift_set(shift);
			opz.hk2_opt.state = 0; // gehe in default Ruhezustand
			break;
		}

		// FM241 nicht vorhanden
		if (!opz.hk2_opt.present) {
			opz.hk2_opt.state = 4; // Mudul nicht erkannt, Fehlerzustand
			break;
		}

		// timeout erreicht, gehe zur aktuellen Messung
		if (HK2_timer >= opz.hk2_opt.wait) {
			opz.hk2_opt.state = 2;
			HK2_timer = 0; // eigentlich nicht nötig, da in State 2 nicht genutzt
			break;
		}
		break;

		//		 * Messung auswerten

	case 2:
		// falls Temperatur nicht ausreichend Pumpe an lassen, aber Mischer eventuell abschalten

		// Vorlauf OK
		if ((opz.hk2_opt.ist >= (opz.hk2_opt.soll - opz.hk2_opt.diff)) && (opz.hk2_opt.ist <= (opz.hk2_opt.soll + opz.hk2_opt.diff))) {
			opz.hk2_opt.state = 1;
			HK2_timer = 0;
			break;
		}
		if (opz.hk2_opt.ist < (opz.hk2_opt.soll - opz.hk2_opt.diff)) { // Vorlauf kalt
			opz.hk2_opt.rotate = (opz.hk2_opt.soll - opz.hk2_opt.ist) * opz.hk2_opt.rotation_tume;
			shift |= (1 << MW); // drehe Mischer auf wärmer
			shift_set(shift);
			opz.hk2_opt.state = 3;
			HK2_timer = 0;
			break;

			// falls Temperatur im Speicher und Kessel zu niedrig ist, Messung verzögern (to-do)
		}
		if (opz.hk2_opt.ist > (opz.hk2_opt.soll + opz.hk2_opt.diff)) { // Vorlauf warm
			opz.hk2_opt.rotate = (opz.hk2_opt.ist - opz.hk2_opt.soll) * opz.hk2_opt.rotation_tume;
			shift |= (1 << MK);
			shift_set(shift);
			opz.hk2_opt.state = 3;
			HK2_timer = 0;
			break;
		}
		break;

		//		 * Drehung wird durchgeführt
	case 3:
		if (HK2_timer >= opz.hk2_opt.rotate) {
			opz.hk2_opt.state = 1;
			HK2_timer = 0;
			shift &= ~((1 << MW) | (1 << MK)); // Mischer aus
			shift_set(shift);
		}
		break;

		//		 * Fehlerzustand

	case 4:
		if (!opz.hk2_opt.active) {
			opz.hk2_opt.state = 0;
			shift &= ~(1 << HK2); // Bodenheizung Pumpe aus
			shift_set(shift);
			break;
		}
		//shift &= ~(1 << HK2);	// Bodenheizung Pumpe aus
		//shift_set(shift);
		if (opz.hk2_opt.present) {
			opz.hk2_opt.state = 1;
			HK2_timer = 0;
			break;
		}
		break;
	default:
		opz.hk2_opt.state = 4;
	}
}
