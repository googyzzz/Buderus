/*
 * hk1_state_machine.c
 *
 *  Created on: 17.06.2014
 *      Author: googy_000
 */
#include "hk1_state_machine.h"
#include "defines.h"
#include "types.h"
#include "shiftregister.h"

void hk1_state_machine() {
//---------------------------------------------------------------------------------------------
//-----------------state machine Heizkreis 1-----------------------------
/*	switch (hkopt.hk1.state) {
	case 0:
		if (hkopt.hk1.active) {
			hkopt.hk1.state = 1;
			break;
		}
		break;

	case 1:
		// Heizkreis 1 deaktiviert
		if (!hkopt.hk1.active) {
			hkopt.hk1.state = 0;
			shift &= ~(1 << HK1);
			shift_set(shift);
			break;
		}

		if (hkopt.hk1.ist > (hkopt.hk1.soll + hkopt.hk1.diff)) {
			shift &= ~(1 << HK1);	// zu warm, Pumpe aus
			shift_set(shift);
		}
		if (hkopt.hk1.ist < (hkopt.hk1.soll - hkopt.hk1.diff)) {
			shift |= (1 << HK1);	// zu kalt, Pumpe an
			shift_set(shift);
		}
		HK1_timer = 0;
		hkopt.hk1.state = 2;	// warten
		break;

		// Warten
	case 2:
		// Heizkreis 1 deaktiviert
		if (!hkopt.hk1.active) {
			hkopt.hk1.state = 0;
			shift &= ~(1 << HK1);
			shift_set(shift);
			break;
		}
		// timeout
		if (HK1_timer > hkopt.hk1.wait) {
			hkopt.hk1.state = 1;
			break;
		}
		break;
	default:
		hkopt.hk1.state = 0;
	}
	*/
}
