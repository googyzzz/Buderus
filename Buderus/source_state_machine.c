///*
// * source_state_machine.c
// *
// *  Created on: 09.06.2014
// *      Author: googy_000
// */
//#include <avr/io.h>
//#include "source_state_machine.h"
////#include ""
////#include "vars.h"
//
//void source_state_machine() {
//	// source_turn hat 3 Zustände
//	// zu Holz drehen, zu Diesel drehen, aus
//	if ((source_ist != source_soll) || (source_turn != OFF)) {	// Quelle hat sich geändert oder Mischer in Bewegung
//		if (source_turn != source_soll) {
//			source_turn = OFF;
//		}
//		if (source_turn == OFF) {
//			source_turn = source_soll;
//			source_timer = 0;
//			PORTA &= ~((1 << PA3) | (1 << PA2));
//			if (source_turn == HOLZ) {
//				PORTA |= (1 << PA3);
//			} else if (source_turn == HEIZOEL) {
//				PORTA |= (1 << PA2);
//			}
//		}
//		// Motor dreht, warte
//		if (source_timer >= 130) {
//			PORTA &= ~((1 << PA3) | (1 << PA2));
//			source_ist = source_turn;
//			source_turn = OFF;
//		}
//	}
//}
