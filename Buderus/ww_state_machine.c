/*
 * ww_state_machine.c
 *
 *  Created on: 11.02.2013
 *      Author: googy
 */
#include <avr/io.h>
#include "ww_state_machine.h"
#include "shiftregister.h"
#include "types.h"

void ww_state_machine() {
	//-----------------state machine Warmwasser------------------------------
	switch (hkopt.ww.state) {
	case 0: //
		if (hkopt.ww.active) {
			hkopt.ww.state = 1;
			break;
		}
		break;
	case 1:
		// wenn Warmwasser deaktiviert, gehe in den Startzustand
		if (!hkopt.ww.active) {
			hkopt.ww.state = 0;
			shift &= ~(1 << WW); // Warmwasser Ladepumpe aus
			shift_set(shift);
			break;
		}

		// Warmwasser warm genug, gehe in den Wartezustand
		if (hkopt.ww.ist >= hkopt.ww.soll) {
			hkopt.ww.state = 3;
			WW_timer = 0;
			shift &= ~(1 << WW); // Warmwasser Ladepumpe aus
			shift_set(shift);
			break;
		}

		// Kessel Messung durchführen
		// WW_ist mit Kesseltemperatur vergleichen und gegebenenfalls in Fehlerzustand oder einen gesonderten wechseln

		if ((temps.Speicher1 - hkopt.ww.diff) <= hkopt.ww.ist) {
			shift &= ~(1 << WW); // Warmwasser Ladepumpe aus
			shift_set(shift);
			WW_timer = 0;
			hkopt.ww.state = 3;
			break;
		}

		// Holzkessel ist in Betrieb -> speichere bis max. Temp.
		if (temps.Holzkessel >= 70 && temps.Speicher1 >= (hkopt.ww.ist + hkopt.ww.diff)) {
			shift |= (1 << WW); // Warmwasser Ladepumpe an
			shift_set(shift);
			WW_timer = 0;
			hkopt.ww.state = 3;
			break;
		}
		break;

		if (temps.Holzkessel < 70 && hkopt.ww.ist < temps.Speicher2 - 2) {
			shift |= (1 << WW); // Warmwasser Ladepumpe an
			shift_set(shift);
			WW_timer = 0;
			hkopt.ww.state = 3;
			break;
		}

		if (temps.Holzkessel < 70 && hkopt.ww.ist >= temps.Speicher2 - 1) {
			shift &= ~(1 << WW); // Warmwasser Ladepumpe aus
			shift_set(shift);
			WW_timer = 0;
			hkopt.ww.state = 3;
			break;
		}
		break;
	case 2: // unbenutzt
		// Warmwasser inaktiv
		if (!hkopt.ww.active) {
			hkopt.ww.state = 0;
			shift &= ~(1 << WW); // Warmwasser Ladepumpe aus
			shift_set(shift);
			break;
		}

		// Warmwasser zu kalt, heizen
		if (hkopt.ww.ist < (hkopt.ww.soll - hkopt.ww.diff)) {
			hkopt.ww.state = 1;
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
		if (!hkopt.ww.active) {
			hkopt.ww.state = 0;
			shift &= ~(1 << WW); // Warmwasser Ladepumpe aus
			shift_set(shift);
			break;
		}
		if (WW_timer >= hkopt.ww.wait) {
			hkopt.ww.state = 1;
		}
		break;
	default:
		hkopt.ww.state = 3;
	}
}
