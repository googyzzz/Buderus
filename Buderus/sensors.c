///*
// * sensors.c
// *
// *  Created on: 09.06.2014
// *      Author: googy_000
// */
//
//#include <avr/io.h>
//#include "sensors.h"
//#include "ow.h"
//#include "messung.h"
//#include "defines.h"
//#include "vars.h"
//#include "vars2.h"
//
//void sample() {
//	//-------------------------------------------Sensoren-update-----------------------------------
//	// 0 Kessel
//	// 1 Außenfühler
//	// 2 Brauchwasser
//	// 3 HK2 anwesend
//	// 4 ???
//	// 5 Brenner status
//	// 6 Vorlauf
//	// 7 ???
//
//	uint16_t erg = 0;
//
//	switch (messtate) {
//	// Diesel-Kessel
//	case 0:
//			erg = messung(0);
//			if (!(erg == 0xFFFF)) { // Messung abgeschlossen
//				if (erg == 0xFFF0) { // Messung fehlgeschlagen, Timmer overflow
//					diesel_t = 0;
//					errors.diesel_t_error = TRUE;
//					messtate = 2; // probiere (über)nächsten Sensor
//				} else { // Messung erfolgreich
//					errors.diesel_t_error = FALSE;
//					diesel_t = convert_mt(erg);
//					messtate = 2;
//				}
//			}
//			break;
//
//			// Außenfühler (abgeschaltet)
//		case 1:
//			messtate = 2;
//			break;
//
//			// Warmwasser
//		case 2:
//			erg = messung(2);
//			if (!(erg == 0xFFFF)) {
//				if (erg == 0xFFF0) {
//					opz.ww_opt.ist = 255;
//					errors.wwasser_t_error = TRUE;
//					messtate = 3;
//				} else {
//					opz.ww_opt.ist = convert_mt(erg);
//					errors.wwasser_t_error = FALSE;
//					messtate = 3;
//				}
//			}
//			break;
//
//			// HK2 FM241 Modul anwesend
//		case 3:
//			erg = messung(3);
//			if (!(erg == 0xFFFF)) {
//				if (erg == 0xFFF0) {
//					opz.hk2_opt.present = FALSE;
//					errors.FM241_error = TRUE;
//					messtate = 5;
//				} else {
//					errors.FM241_error = FALSE;
//					if (erg < 400) {
//						opz.hk2_opt.present = TRUE;
//					} else {
//						opz.hk2_opt.present = FALSE;
//					}
//					messtate = 5;
//				}
//			}
//			break;
//
//			// Brenner Status
//		case 5:
//			erg = messung(5);
//			if (!(erg == 0xFFFF)) { // Messung abgeschlossen
//				if (erg == 0xFFF0) { // Messung fehlgeschlagen, Timmer overflow
//					brenner_status = 0;
//					messtate = 6; // probiere nächsten Sensor
//				} else { // Messung erfolgreich
//					brenner_status = erg;
//					messtate = 6;
//				}
//			}
//			break;
//
//			// HK2 Temperatur
//		case 6:
//			erg = messung(6);
//			if (!(erg == 0xFFFF)) {// Messung abgeschlossen
//				if (erg == 0xFFF0) {// Messung fehlgeschlagen, Timmer overflow
//					opz.hk2_opt.ist = 0xFF;
//					errors.hk2_t_error = TRUE;
//					messtate = 0; // probiere nächsten Sensor
//				} else {// Messung erfolgreich
//					errors.hk2_t_error = FALSE;
//					opz.hk2_opt.ist = convert_mt(erg);
//					messtate = 0;
//				}
//			}
//			break;
//		default:
//			messtate = 0;
//		}
//
//
//	uint8_t ow_state = 0;
//	//-----------------OneWire-Messung----------------------------------------------
//	switch (ow_state) {
//	case 0: // starte Messung
//		ow_start();
//		ow_start2();
//		ow_state = 1;
//		OW_timer = 0;
//		break;
//	case 1: // warte bis Messung abgeschlossen ist
//		if (OW_timer >= 2) {
//			ow_state = 2;
//		}
//		break;
//	case 2: // lese Sensor aus
//		temps.atmos = (ow_temp_id(EEP_OW_HOLZ) >> 4);
//		ow_state = 3;
//		break;
//	case 3:
//			temps.sp0 = (ow_temp_id(EEP_OW_SPEICHER_0) >> 4);
//			ow_state = 4;
//			break;
//		case 4:
//			temps.sp1 = (ow_temp_id(EEP_OW_SPEICHER_1) >> 4);
//			ow_state = 5;
//			break;
//		case 5:
//			temps.sp2 = (ow_temp_id(EEP_OW_SPEICHER_2) >> 4);
//			ow_state = 6;
//			break;
//		case 6:
//			temps.sp3 = (ow_temp_id(EEP_OW_SPEICHER_3) >> 4);
//			ow_state = 7;
//			break;
//		case 7:
//			temps.sp4 = (ow_temp_id(EEP_OW_SPEICHER_4) >> 4);
//			ow_state = 8;
//			break;
//		case 8:
//			read_ow_scratch_id2(EEP_OW_EXPERIMENT);
//			arbeitsZimmer.degree = get_temp_deg2();
//			arbeitsZimmer.millis = get_temp_millis2();
//			ow_state = 9;
//			break;
//		case 9:
//			read_ow_scratch_id2(EEP_OW_SCHLAFZIMMER);
//			schlafZimmer.degree = get_temp_deg2();
//			schlafZimmer.millis = get_temp_millis2();
//			ow_state = 0;
//			break;
//		default:
//			ow_state = 0;
//		}
//}
