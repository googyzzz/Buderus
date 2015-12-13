/*
 * hk1_state_machine.c
 *
 *  Created on: 17.06.2014
 *      Author: googy_000
 */
#include "hk1_state_machine.h"
#include "../defines.h"
#include "../types.h"
#include "../shiftregister.h"

#include <stdlib.h>
#include "../uart.h"

void hk1_state_machine() {
	switch (hkopt.hk1.state) {
	/* Idle state, Heizkreis deaktiviert */
	case 0:
		if (hkopt.hk1.active) {
			hkopt.hk1.state = 1;
			break;
		}
		break;
	/**/
	case 1:
		// Heizkreis 1 deaktiviert
		if (!hkopt.hk1.active) {
			hkopt.hk1.state = 0;
			shift &= ~(1 << HK1);	// schalte Pumpe ab
			shift_set(shift);
			break;
		}

		if (hkopt.source.source_ist == HOLZ) {
			if (hkopt.schlafZimmer.active && hkopt.schlafZimmer.ist.degree < hkopt.schlafZimmer.soll.degree) {
					shift |= (1 << HK1);
				} else {
					if ((hkopt.schlafZimmer.ist.degree >= (hkopt.hk1.soll + hkopt.hk1.diff)) && (hkopt.schlafZimmer.ist.millis > 5000)) {
						shift &= ~(1 << HK1);	// zu warm, Pumpe aus
					}
				}
			}
			if (hkopt.arbeitsZimmer.active) {
				if (hkopt.arbeitsZimmer.ist.degree < (hkopt.hk1.soll - hkopt.hk1.diff)) {
					shift |= (1 << HK1);
				} else {
					if ((hkopt.arbeitsZimmer.ist.degree >= (hkopt.hk1.soll + hkopt.hk1.diff)) && (hkopt.schlafZimmer.ist.millis > 5000)) {
						shift &= ~(1 << HK1);	// zu warm, Pumpe aus
					}
				}
			}
		} else {
			shift &= ~(1 << HK1);
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
	shift_set(shift);	// TODO auslagern, hier nicht nötig
}
