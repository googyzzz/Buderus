/*
 * brenner.c
 *
 *  Created on: 02.10.2015
 *      Author: googy
 */

#include "../defines.h"
#include "../types.h"
#include "../shiftregister.h"
#include "brenner.h"

// TODO Reine Brennerlogik hier unterbringen, rest auslagern

void brenner() {
		// Brenner
		switch (hkopt.source.buderus_state) {
		case 0:
			if (hkopt.source.buderus_on && hkopt.source.source_ist == HEIZOEL) {
				hkopt.source.buderus_state = 1;
			}
			break;
		case 1:	// warte bis Energie nötig ist
			// Buderus abgeschaltet oder Energiequelle geändert, schalte Brenner ab
			if (!hkopt.source.buderus_on || hkopt.source.source_ist != HEIZOEL) {
				shift &= ~(1 << BRENNER);
				hkopt.source.buderus_state = 0;
				break;
			}
			// Brenner eingeschaltet, Quelle auf Heizöl und Energie benötigt, Brenner starten
			if (hkopt.source.buderus_fire) {
				shift |= (1 << BRENNER);
				hkopt.source.buderus_state = 2;
				break;
			}
			break;
		case 2:
			// Buderus abgeschaltet oder Energiequelle geändert, schalte Brenner ab
			if (!hkopt.source.buderus_on || hkopt.source.source_ist != HEIZOEL) {
				shift &= ~(1 << BRENNER);
				hkopt.source.buderus_state = 0;
				break;
			}
			// keine Energie mehr benötigt
			if (!hkopt.source.buderus_fire) {
				shift &= ~(1 << BRENNER);
				hkopt.source.buderus_state = 1;
				break;
			}
			// heize bis maximale Temperatur erreicht ist
			if (hkopt.source.buderus_temp > hkopt.source.buderus_temp_max) {	// maximalle Kesseltemperatur erreicht
				shift &= ~(1 << BRENNER);
				hkopt.source.buderus_state = 3;
				break;
			}
			break;
		case 3:
			// Buderus abgeschaltet oder Energiequelle geändert, schalte Brenner ab
			if (!hkopt.source.buderus_on || hkopt.source.source_ist != HEIZOEL) {
				shift &= ~(1 << BRENNER);
				hkopt.source.buderus_state = 0;
				break;
			}
			// keine Energie mehr benötigt
			if (!hkopt.source.buderus_fire) {
				shift &= ~(1 << BRENNER);
				hkopt.source.buderus_state = 1;
				break;
			}
			// warte bis wieder geheizt werden muss
			if (hkopt.source.buderus_temp < (hkopt.source.buderus_temp_max - hkopt.source.buderus_temp_diff)) {
				shift |= (1 << BRENNER);
				hkopt.source.buderus_state = 2;
				break;
			}
			break;
		default:
			hkopt.source.buderus_state = 0;
		}
}
