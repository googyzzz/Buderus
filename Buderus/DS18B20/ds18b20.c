/*
 * ds18b20.c
 *
 *  Created on: 04.12.2014
 *      Author: googy_000
 */

#include <avr/io.h>
#include "ow.h"
#include "../types.h"		// data structs
#include "../defines.h"

#include "ds18b20.h"
//static uint8_t scratchpad[9];

static uint8_t ow_state = 0;

uint8_t t() {
	return 0;
}

void ds18b20_init() {
	ow_init();
	ow_init2();
}

//-----------------OneWire-Messung----------------------------------------------
void ow_messung() {
	switch (ow_state) {
	case 0: // starte Messung
		ow_start();
		ow_start2();
		ow_state = 1;
		OW_timer = 0;
		break;
	case 1: // warte 2s bis Messung abgeschlossen ist
		if (OW_timer >= 2) {
			ow_state = 2;
		}
		break;
	case 2: // lese Sensor aus
		// FIXME Plausibilitätsprüfung, Fehler notieren!!!
		hkopt.source.atmos_temp = (ow_temp_id(EEP_OW_HOLZ) >> 4);
//			if (hkopt.source.atmos_temp == 127) {
//				uart_puts("OW CRC");
//			}
		ow_state = 3;
		break;
	case 3:
		hkopt.source.speicher0 = (ow_temp_id(EEP_OW_SPEICHER_0) >> 4);
		ow_state = 4;
		break;
	case 4:
		hkopt.source.speicher1 = (ow_temp_id(EEP_OW_SPEICHER_1) >> 4);
		ow_state = 5;
		break;
	case 5:
		hkopt.source.speicher2 = (ow_temp_id(EEP_OW_SPEICHER_2) >> 4);
		ow_state = 6;
		break;
	case 6:
		hkopt.source.speicher3 = (ow_temp_id(EEP_OW_SPEICHER_3) >> 4);
		ow_state = 7;
		break;
	case 7:
		hkopt.source.speicher4 = (ow_temp_id(EEP_OW_SPEICHER_4) >> 4);
		ow_state = 8;
		break;
	case 8:
		read_ow_scratch_id2(EEP_OW_EXPERIMENT);
		hkopt.arbeitsZimmer.ist.degree = get_temp_deg2();
		hkopt.arbeitsZimmer.ist.millis = get_temp_millis2();
		ow_state = 9;
		break;
	case 9:
		read_ow_scratch_id2(EEP_OW_SCHLAFZIMMER);
		hkopt.schlafZimmer.ist.degree = get_temp_deg2();
		hkopt.schlafZimmer.ist.millis = get_temp_millis2();
		ow_state = 0;
		break;
	default:
		ow_state = 0;
	}
}
