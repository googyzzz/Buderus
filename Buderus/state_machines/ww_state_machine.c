/*
 * ww_state_machine.c
 *
 *  Created on: 11.02.2013
 *      Author: googy
 */
#include <avr/io.h>
#include <stdlib.h>
#include "ww_state_machine.h"
#include "../shiftregister.h"
#include "../types.h"
#include "../defines.h"

#include "../uart.h"

void ww_state_machine() {
	//-----------------state machine Warmwasser------------------------------
	switch (hkopt.ww.state) {
	/* Idle state, Warmwasser deaktiviert */
	case 0:
		if (hkopt.ww.active) {
			hkopt.ww.state = 1;
		}
		break;
	case 1:
		if (!hkopt.ww.active) {		// deaktiviert
			hkopt.ww.state = 0;
			break;
		}
		if (hkopt.source.source_ist == HOLZ) {
			hkopt.ww.state = 2;
			break;
		}
		if (hkopt.source.source_ist == HEIZOEL) {
			hkopt.ww.state = 5;
			break;
		}
		break;
	case 2:
		if (!hkopt.ww.active) {
			hkopt.ww.state = 0;
			break;
		}
		if (hkopt.source.source_ist != HOLZ) {	// Quelle nicht mehr Holz
			hkopt.ww.state = 1;
			break;
		}
		if (hkopt.ww.ist < (hkopt.ww.soll - 3)) {	// nicht auf Maximaltemperatur
			hkopt.ww.state = 3;
			break;
		}
		break;
	case 3:
		if (!hkopt.ww.active) {	// Warmwasser deaktiviert
			shift &= ~(1 << WW);
			hkopt.ww.state = 0;
			break;
		}
		if (hkopt.source.source_ist != HOLZ) {	// Quelle nicht mehr Holz
			shift &= ~(1 << WW);
			hkopt.ww.state = 1;
			break;
		}
		if (hkopt.ww.ist >= hkopt.ww.soll) {	// Solltemperatur erreicht
			shift &= ~(1 << WW);
			hkopt.ww.state = 2;
			break;
		}
		if ((hkopt.source.speicher1 - 6) > hkopt.ww.ist) { // genug Wärme vorhanden
			shift |= (1 << WW);
			hkopt.ww.state = 4;
			WW_timer = 0;
			break;
		} else {
			shift &= ~(1 << WW);
			hkopt.ww.state = 4;
			WW_timer = 0;
			break;
		}
		break;
	case 4:
		if (WW_timer > 120) {
			hkopt.ww.state = 3;
			break;
		}
		break;
	case 5:	// Heizoel
		if (!hkopt.ww.active) {
			hkopt.ww.state = 0;
			break;
		}
		if (hkopt.source.source_ist != HEIZOEL) {
			hkopt.ww.state = 1;
			break;
		}
		if (hkopt.ww.ist < (hkopt.ww.soll - hkopt.ww.diff)) {	// nicht auf Maximaltemperatur
			hkopt.ww.state = 6;
			hkopt.source.buderus_fire = 1;	// schalte Brenner hinzu
			break;
		}
		break;
	case 6:
		if (!hkopt.ww.active) {
			hkopt.ww.state = 0;
			shift &= ~(1 << WW);				// Pumpe abschalten
			hkopt.source.buderus_fire = 0;		// Brenner abschalten
			break;
		}
		if (hkopt.source.source_ist != HEIZOEL) {
			hkopt.ww.state = 1;
			shift &= ~(1 << WW);				// Pumpe abschalten
			hkopt.source.buderus_fire = 0;		// Brenner abschalten
			break;
		}
		if ((hkopt.ww.ist + 5) < hkopt.source.buderus_temp) {
			shift |= (1 << WW);					// Pumpe einschalten
		}
		if (hkopt.ww.ist >= hkopt.ww.soll) {
			hkopt.source.buderus_fire = 0;
			hkopt.ww.state = 5;
			shift &= ~(1 << WW);				// Pumpe abschalten
		}
		break;
	default:
		hkopt.ww.state = 0;
	}
}
