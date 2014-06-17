/*
 * hk2_state_machine.c
 *
 *  Created on: 11.02.2013
 *      Author: googy
 */
#include <avr/io.h>
#include "defines.h"
#include "types.h"
#include "shiftregister.h"
#include "hk2_state_machine.h"

void hk2_state_machine() {
	switch (hkopt.hk2.state) {
	case 0:
		if (hkopt.hk2.active) {
			shift |= (1 << HK2); // Bodenheizung Pumpe an
			shift_set(shift);
			hkopt.hk2.state = 1;
			HK2_timer = 0;
			break;
		}
		break;

		//			 * Wartezustand
		//			 * hier wird gewartet zwischen den Korrekturvorgängen und der Zustand geprüfft

	case 1:
		// HK2 deaktiviert
		if (!hkopt.hk2.active) {
			shift &= ~(1 << HK2); // Bodenheizung Pumpe aus
			shift_set(shift);
			hkopt.hk2.state = 0; // gehe in default Ruhezustand
			break;
		}

		// FM241 nicht vorhanden
		if (!hkopt.hk2.present) {
			hkopt.hk2.state = 4; // Mudul nicht erkannt, Fehlerzustand
			break;
		}

		// timeout erreicht, gehe zur aktuellen Messung
		if (HK2_timer >= hkopt.hk2.wait) {
			hkopt.hk2.state = 2;
			HK2_timer = 0; // eigentlich nicht nötig, da in State 2 nicht genutzt
			break;
		}
		break;

		//		 * Messung auswerten

	case 2:
		// falls Temperatur nicht ausreichend Pumpe an lassen, aber Mischer eventuell abschalten

		// Vorlauf OK
		if ((hkopt.hk2.ist >= (hkopt.hk2.soll - hkopt.hk2.diff)) && (hkopt.hk2.ist <= (hkopt.hk2.soll
		        + hkopt.hk2.diff))) {
			hkopt.hk2.state = 1;
			HK2_timer = 0;
			break;
		}
		if (hkopt.hk2.ist < (hkopt.hk2.soll - hkopt.hk2.diff)) { // Vorlauf kalt
			hkopt.hk2.rotate = (hkopt.hk2.soll - hkopt.hk2.ist) * hkopt.hk2.ROTATION_TIME;
			shift |= (1 << MW); // drehe Mischer auf wärmer
			shift_set(shift);
			hkopt.hk2.state = 3;
			HK2_timer = 0;
			break;

			// falls Temperatur im Speicher und Kessel zu niedrig ist, Messung verzögern (to-do)
		}
		if (hkopt.hk2.ist > (hkopt.hk2.soll + hkopt.hk2.diff)) { // Vorlauf warm
			hkopt.hk2.rotate = (hkopt.hk2.ist - hkopt.hk2.soll) * hkopt.hk2.ROTATION_TIME;
			shift |= (1 << MK);
			shift_set(shift);
			hkopt.hk2.state = 3;
			HK2_timer = 0;
			break;
		}
		break;

		//		 * Drehung wird durchgeführt
	case 3:
		if (HK2_timer >= hkopt.hk2.rotate) {
			hkopt.hk2.state = 1;
			HK2_timer = 0;
			shift &= ~((1 << MW) | (1 << MK)); // Mischer aus
			shift_set(shift);
		}
		break;

		//		 * Fehlerzustand

	case 4:
		if (!hkopt.hk2.active) {
			hkopt.hk2.state = 0;
			shift &= ~(1 << HK2); // Bodenheizung Pumpe aus
			shift_set(shift);
			break;
		}
		//shift &= ~(1 << HK2);	// Bodenheizung Pumpe aus
		//shift_set(shift);
		if (hkopt.hk2.present) {
			hkopt.hk2.state = 1;
			HK2_timer = 0;
			break;
		}
		break;
	default:
		hkopt.hk2.state = 4;
	}
}
