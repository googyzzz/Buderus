/*
 * hk2_state_machine.c
 *
 *  Created on: 11.02.2013
 *      Author: googy
 */
#include <avr/io.h>
#include "shiftregister.h"
#include "hk2_state_machine.h"
#include "vars.h"

void hk2_state_machine() {
	switch (HK2_state) {
	case 0:
		if (HK2_active) {
			shift |= (1 << HK2); // Bodenheizung Pumpe an
			shift_set(shift);
			HK2_state = 1;
			HK2_timer = 0;
			break;
		}
		break;

		//			 * Wartezustand
		//			 * hier wird gewartet zwischen den Korrekturvorgängen und der Zustand geprüfft

	case 1:
		// HK2 deaktiviert
		if (!HK2_active) {
			shift &= ~(1 << HK2); // Bodenheizung Pumpe aus
			shift_set(shift);
			HK2_state = 0; // gehe in default Ruhezustand
			break;
		}

		// FM241 nicht vorhanden
		if (!HK2_present) {
			HK2_state = 4; // Mudul nicht erkannt, Fehlerzustand
			break;
		}

		// timeout erreicht, gehe zur aktuellen Messung
		if (HK2_timer >= HK2_wait) {
			HK2_state = 2;
			HK2_timer = 0; // eigentlich nicht nötig, da in State 2 nicht genutzt
			break;
		}
		break;

		//		 * Messung auswerten

	case 2:
		// falls Temperatur nicht ausreichend Pumpe an lassen, aber Mischer eventuell abschalten

		// Vorlauf OK
		if ((HK2_ist >= (HK2_soll - HK2_diff)) && (HK2_ist <= (HK2_soll
		        + HK2_diff))) {
			HK2_state = 1;
			HK2_timer = 0;
			break;
		}
		if (HK2_ist < (HK2_soll - HK2_diff)) { // Vorlauf kalt
			HK2_rotate = (HK2_soll - HK2_ist) * ROTATION_TIME;
			shift |= (1 << MW); // drehe Mischer auf wärmer
			shift_set(shift);
			HK2_state = 3;
			HK2_timer = 0;
			break;

			// falls Temperatur im Speicher und Kessel zu niedrig ist, Messung verzögern (to-do)
		}
		if (HK2_ist > (HK2_soll + HK2_diff)) { // Vorlauf warm
			HK2_rotate = (HK2_ist - HK2_soll) * ROTATION_TIME;
			shift |= (1 << MK);
			shift_set(shift);
			HK2_state = 3;
			HK2_timer = 0;
			break;
		}
		break;

		//		 * Drehung wird durchgeführt
	case 3:
		if (HK2_timer >= HK2_rotate) {
			HK2_state = 1;
			HK2_timer = 0;
			shift &= ~((1 << MW) | (1 << MK)); // Mischer aus
			shift_set(shift);
		}
		break;

		//		 * Fehlerzustand

	case 4:
		if (!HK2_active) {
			HK2_state = 0;
			shift &= ~(1 << HK2); // Bodenheizung Pumpe aus
			shift_set(shift);
			break;
		}
		//shift &= ~(1 << HK2);	// Bodenheizung Pumpe aus
		//shift_set(shift);
		if (HK2_present) {
			HK2_state = 1;
			HK2_timer = 0;
			break;
		}
		break;
	default:
		HK2_state = 4;
	}
}
