/*
 * energiequelle.c
 *
 *  Created on: 02.10.2015
 *      Author: googy
 */

#include "../types.h"
#include "energiequelle.h"

/// für bessere Modularisierung evtl. einige Werte wie Turn hier als static ablegen

void energiequelle() {
	// Energiequelle
	if (hkopt.source.source_ist != hkopt.source.source_soll) {				// Mischer nicht in Sollposition oder unbekannt
		/* ist Zustand wird zu TURNING geändert daher werden eine nachträgliche Änderung berücksichtigt
		   auch wenn sich soll ändert, denn soll wird extern nie auf TURNING gestellt
		   daher bis die Endposition erreicht ist wird diese Stelle immer erreicht
		*/
		if (hkopt.source.source_ist != TURNING) {							// Es wird noch keine Drehung ausgeführt
			PORTA &= ~((1 << PA3) | (1 << PA2));							// vorsichtshalber Drehung abschalten, vor allem interessant für die Initialisierung des Mischers
			if (hkopt.source.source_soll == HOLZ) {
				PORTA |= (1 << PA3);										// drehe Richtung Holz
			} else if (hkopt.source.source_soll == HEIZOEL) {
				PORTA |= (1 << PA2);										// drehe Richtung Heizöl
			}
			source_timer = 0;												// initialisiere Drehtimer
			hkopt.source.source_ist = TURNING;								// ändere Zustand, Mischer befindet sich in Bewegung
			hkopt.source.source_turn = hkopt.source.source_soll;			// notiere Drehrichtung
			/* auch hier kann es keine Inkonsistenzen geben, dann dieser Teil wird in einem Durchlauf ausgeführt
			   und erst nach dem die Drehrichtung notiert wurde kann soll sich ändern,
			   dies ist auch kein Problem, denn soll wird solange ignoriert bis die Drehung abgeschlossen ist.
			*/
		}
		if (source_timer >= 130 && hkopt.source.source_ist == TURNING) {	// warte bis Drehung abgeschlossen
			PORTA &= ~((1 << PA3) | (1 << PA2));							// Mischer abschalten, da fertig gedreht
			hkopt.source.source_ist = hkopt.source.source_turn;				// notiere neue Position
		}
	}
}
