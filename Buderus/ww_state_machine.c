/*
 * ww_state_machine.c
 *
 *  Created on: 11.02.2013
 *      Author: googy
 */
#include <avr/io.h>
#include "ww_state_machine.h"
#include "shiftregister.h"
#include "vars.h"

void ww_state_machine() {
	//-----------------state machine Warmwasser------------------------------
	switch (WW_state) {
	case 0: //
		if (WW_active) {
			WW_state = 1;
			break;
		}
		break;
	case 1:
		// wenn Warmwasser deaktiviert, gehe in den Startzustand
		if (!WW_active) {
			WW_state = 0;
			shift &= ~(1 << WW); // Warmwasser Ladepumpe aus
			shift_set(shift);
			break;
		}

		if (source_ist == UNBEKANNT) {

		}
		//source_turn

		// Warmwasser warm genug, gehe in den Wartezustand
		if (WW_ist >= WW_soll) {
			WW_state = 3;
			WW_timer = 0;
			shift &= ~(1 << WW); // Warmwasser Ladepumpe aus
			shift_set(shift);
			break;
		}

		// Kessel Messung durchführen
		// WW_ist mit Kesseltemperatur vergleichen und gegebenenfalls in Fehlerzustand oder einen gesonderten wechseln

		if ((Speicher1 - WW_diff) <= WW_ist) {
			shift &= ~(1 << WW); // Warmwasser Ladepumpe aus
			shift_set(shift);
			WW_timer = 0;
			WW_state = 3;
			break;
		}

		// Holzkessel ist in Betrieb -> speichere bis max. Temp.
		if (Holzkessel >= 70 && Speicher1 >= (WW_ist + WW_diff)) {
			shift |= (1 << WW); // Warmwasser Ladepumpe an
			shift_set(shift);
			WW_timer = 0;
			WW_state = 3;
			break;
		}
		break;

		if (Holzkessel < 70 && WW_ist < Speicher2 - 2) {
			shift |= (1 << WW); // Warmwasser Ladepumpe an
			shift_set(shift);
			WW_timer = 0;
			WW_state = 3;
			break;
		}

		if (Holzkessel < 70 && WW_ist >= Speicher2 - 1) {
			shift &= ~(1 << WW); // Warmwasser Ladepumpe aus
			shift_set(shift);
			WW_timer = 0;
			WW_state = 3;
			break;
		}

	case 2: // unbenutzt
		// Warmwasser inaktiv
		if (!WW_active) {
			WW_state = 0;
			shift &= ~(1 << WW); // Warmwasser Ladepumpe aus
			shift_set(shift);
			break;
		}

		// Warmwasser zu kalt, heizen
		if (WW_ist < (WW_soll - WW_diff)) {
			WW_state = 1;
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
		if (!WW_active) {
			WW_state = 0;
			shift &= ~(1 << WW); // Warmwasser Ladepumpe aus
			shift_set(shift);
			break;
		}
		if (WW_timer >= WW_wait) {
			WW_state = 1;
		}
		break;
	default:
		WW_state = 3;
	}
}
