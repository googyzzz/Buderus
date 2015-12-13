/*
 * buderus.c
 *
 *  Created on: 02.10.2015
 *      Author: googy
 */

#include <avr/io.h>

#include "buderus.h"
#include "messung.h"
#include "../types.h"

static uint8_t messtate = 0;
static uint16_t erg;

void buderus_messung() {
	//-------------------------------------------Sensoren-update-----------------------------------
	// 0 Kessel
	// 1 Außenfühler
	// 2 Brauchwasser
	// 3 HK2 anwesend
	// 4 ???
	// 5 Brenner status
	// 6 Vorlauf
	// 7 ???
	erg = 0;
	switch (messtate) {
	// Diesel-Kessel
		case 0:
			erg = messung(0);
			if (!(erg == 0xFFFF)) { // Messung abgeschlossen
				if (erg == 0xFFF0) { // Messung fehlgeschlagen, Timmer overflow
					hkopt.source.buderus_temp = 0;
					// TODO zentraler Fehlerspeicher
					//errors.diesel_t_error = 1;
					messtate = 2; // probiere (über)nächsten Sensor
				} else { // Messung erfolgreich
					//errors.diesel_t_error = 0;
					hkopt.source.buderus_temp = convert_mt(erg);
					messtate = 2;
				}
			}
			break;

			// Außenfühler (abgeschaltet)
		case 1:
			messtate = 2;
			break;

			// Warmwasser
		case 2:
			erg = messung(2);
			if (!(erg == 0xFFFF)) {
				if (erg == 0xFFF0) {
					hkopt.ww.ist = 255;
					//errors.wwasser_t_error = 1;
					messtate = 3;
				} else {
					hkopt.ww.ist = convert_mt(erg);
					//errors.wwasser_t_error = 0;
					messtate = 3;
				}
			}
			break;

			// HK2 FM241 Modul anwesend
		case 3:
			erg = messung(3);
			if (!(erg == 0xFFFF)) {
				if (erg == 0xFFF0) {
					hkopt.hk2.present = 0;
					//errors.FM241_error = 1;
					messtate = 5;
				} else {
					//errors.FM241_error = 0;
					if (erg < 400) {
						hkopt.hk2.present = 1;
					} else {
						hkopt.hk2.present = 0;
					}
					messtate = 5;
				}
			}
			break;

			// Brenner Status
		case 5:
			erg = messung(5);
			if (!(erg == 0xFFFF)) { // Messung abgeschlossen
				if (erg == 0xFFF0) { // Messung fehlgeschlagen, Timmer overflow
					hkopt.source.brenner_state = 0;
					messtate = 6; // probiere nächsten Sensor
				} else { // Messung erfolgreich
					hkopt.source.brenner_state = erg;
					messtate = 6;
				}
			}
			break;

			// HK2 Temperatur
		case 6:
			erg = messung(6);
			if (!(erg == 0xFFFF)) {// Messung abgeschlossen
				if (erg == 0xFFF0) {// Messung fehlgeschlagen, Timmer overflow
					hkopt.hk2.ist = 0xFF;
					//errors.hk2_t_error = 1;
					messtate = 0; // probiere nächsten Sensor
				} else {// Messung erfolgreich
					//errors.hk2_t_error = 0;
					hkopt.hk2.ist = convert_mt(erg);
					messtate = 0;
				}
			}
			break;
		default:
			messtate = 0;
		}
}
