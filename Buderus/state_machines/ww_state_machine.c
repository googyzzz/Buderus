/*
 * ww_state_machine.c
 *
 *  Created on: 11.02.2013
 *      Author: googy
 */
#include <avr/io.h>
#include "ww_state_machine.h"
#include "../shiftregister.h"
#include "../types.h"
#include "../defines.h"

#include "../uart.h"

void ww_state_machine() {
	//-----------------state machine Warmwasser------------------------------
	switch (hkopt.ww.state) {
	case 0:
		uart_puts("wasser aus");
		uart_putc(10);
		uart_putc(13);
		// prüfen ob Warmwasser überhaupt eingeschaltet ist
		if (hkopt.ww.active) {
			hkopt.ww.state = 1;
		}
		break;
	case 1:
		// wenn Warmwasser deaktiviert, gehe in den Startzustand
		if (!hkopt.ww.active) {
			hkopt.ww.state = 4;
			break;
		}

		if (hkopt.ww.ist < (hkopt.ww.soll - hkopt.ww.diff)) {	// wasser kalt
			hkopt.source.need_energy = 1;
			hkopt.ww.state = 3;
			WW_timer = 0;
			if (hkopt.source.buderus_temp > hkopt.source.buderus_temp_min) {
				shift |= (1 << WW); // Warmwasser Ladepumpe an
				shift_set(shift);
			}
			break;
		}

		// Warmwasser warm genug, gehe in den Wartezustand
		if (hkopt.ww.ist >= hkopt.ww.soll) {
			hkopt.source.need_energy = 0;
			hkopt.ww.state = 3;
			WW_timer = 0;
			shift &= ~(1 << WW); // Warmwasser Ladepumpe aus
			shift_set(shift);
			break;
		}

		break;
	case 3:
		// Warmwasser inaktiv
		if (!hkopt.ww.active) {
			hkopt.ww.state = 4;
			break;
		}
		if (WW_timer >= hkopt.ww.wait) {
			hkopt.ww.state = 1;
		}
		break;
	case 4:
#warning Nur im alleinigen warmwasserbetrieb erlaubt, dringend ändern
		hkopt.source.need_energy = 0;	// keine weitere Energie nötig
		hkopt.ww.state = 0;
		shift &= ~(1 << WW); // Warmwasser Ladepumpe aus
		shift_set(shift);
		break;
	default:
		hkopt.ww.state = 255;
	}
}
