///*
// * netcom.c
// *
// *  Created on: 11.02.2013
// *      Author: googy
// */
//#include <avr/eeprom.h>
#include "netcom.h"
//#include "ntp.h"	// NTP_LOCAL_PORT
//#include "../vars2.h"	// opz
//#include "../shiftregister.h"
//#include "../defines.h"
//
void udp_packet(eth_frame_t *frame, uint16_t len) {			// udp paketereignis
//	ip_packet_t *ip = (void*)(frame->data);
//	udp_packet_t *udp = (void*)(ip->data);
//	uint8_t *data = udp->data;
//	uint32_t timestamp;										// zeitvariable
//
//	if(udp->to_port == NTP_LOCAL_PORT)						// eingehende NTP nachricht
//	{
//		if((timestamp = ntp_parse_reply(udp->data, len)))	// extrahiere zeit, extraktion erfolgreich
//		{
//			time_offset = timestamp - second_count;			// rechne wirkliche zeit aus
//			ntp_next_update = second_count + 12UL*60*60;	// setze n�chste aktualisierung
//		}
//	}
//	else
//	{
//		uint8_t length = 0;
//		// steuerkomando (bulk read)
//		if (data[0] == 0x55) {
//			// Heizkreis 1
//			data[1] = opz.hk1_opt.active;
//			data[2] = opz.hk1_opt.ist;
//			data[3] = opz.hk1_opt.soll;
//			data[4] = opz.hk1_opt.diff;
//			data[5] = opz.hk1_opt.wait;
//			data[6] = opz.hk1_opt.state;
//			// Heizkreis 2
//			data[7] = opz.hk2_opt.active;
//			data[8] = opz.hk2_opt.ist;
//			data[9] = opz.hk2_opt.soll;
//			data[10] = opz.hk2_opt.diff;
//			data[11] = opz.hk2_opt.wait;
//			data[12] = opz.hk2_opt.state;
//			data[13] = opz.hk2_opt.present;
//			// Warmwasser
//			data[14] = opz.ww_opt.active;
//			data[15] = opz.ww_opt.ist;
//			data[16] = opz.ww_opt.soll;
//			data[17] = opz.ww_opt.diff;
//			data[18] = opz.ww_opt.wait;
//			data[19] = opz.ww_opt.state;
//			// OneWire Sensoren
//			data[20] = temps.atmos;
//			data[21] = temps.sp0;
//			data[22] = temps.sp1;
//			data[23] = temps.sp2;
//			data[24] = temps.sp3;
//			data[25] = temps.sp4;
//			data[26] = arbeitsZimmer.degree;
//			data[27] = arbeitsZimmer.millis >> 8;
//			data[28] = arbeitsZimmer.millis & 0xFF;
//			data[29] = schlafZimmer.degree;
//			data[30] = schlafZimmer.millis >> 8;
//			data[31] = schlafZimmer.millis & 0xFF;
//			data[32] = diesel_t;
//			data[33] = (brenner_status >> 8);
//			data[34] = shift;
//			length = 35;
//			//sende Antwort
//			udp_reply(frame, length);
//		}
//		else{
//			// steuerkomando
//			//schreiben
//			if ((data[0] == 0xAA)&&(data[1] == 0xAA)) {
//				switch (data[2]) {
//				// Brenner
//
//				//Heizkreis 1
//				//default EEPROM
//				case 0x10:
//					opz.hk1_opt.active = data[3];
//					eeprom_write_byte((uint8_t *)EEP_HK1_ACTIVE, opz.hk1_opt.active);
//					break;
//				case 0x11:
//					opz.hk1_opt.soll = data[3];
//					eeprom_write_byte((uint8_t *)EEP_HK1_SOLL, opz.hk1_opt.soll);
//					break;
//				case 0x12:
//					opz.hk1_opt.diff = data[3];
//					eeprom_write_byte((uint8_t *)EEP_HK1_DIFF, opz.hk1_opt.diff);
//					break;
//				case 0x13:
//					opz.hk1_opt.wait = data[3];
//					eeprom_write_byte((uint8_t *)EEP_HK1_WAIT, opz.hk1_opt.wait);
//					break;
//				//tempor�r RAM
//				case 0x15:	opz.hk1_opt.active = data[3];	break;
//				case 0x16:	opz.hk1_opt.soll = data[3];		break;
//				case 0x17:	opz.hk1_opt.diff = data[3];		break;
//				case 0x18:	opz.hk1_opt.wait = data[3];		break;
//
//				// Heizkreis 2
//				// EEPROM
//				case 0x20:opz.hk2_opt.active = data[3];
//					eeprom_write_byte((uint8_t *)EEP_HK2_ACTIVE, opz.hk2_opt.active);
//					break;
//				case 0x21:
//					opz.hk2_opt.soll = data[3];
//					eeprom_write_byte((uint8_t *)EEP_HK2_SOLL, opz.hk2_opt.soll);
//					break;
//				case 0x22:
//					opz.hk2_opt.diff = data[3];
//					eeprom_write_byte((uint8_t *)EEP_HK2_DIFF, opz.hk2_opt.diff);
//					break;
//				case 0x23:
//					opz.hk2_opt.wait = data[3];
//					eeprom_write_byte((uint8_t *)EEP_HK2_WAIT, opz.hk2_opt.wait);
//					break;
//				// RAM
//				case 0x25:	opz.hk2_opt.active = data[3];	break;
//				case 0x26:	opz.hk2_opt.soll = data[3];		break;
//				case 0x27:	opz.hk2_opt.diff = data[3];		break;
//				case 0x28:	opz.hk2_opt.wait = data[3];		break;
//
//
//				// Warmwasser
//				// EEPROM
//				case 0x30:
//					opz.ww_opt.active = data[3];
//					eeprom_write_byte((uint8_t *)EEP_WW_ACTIVE, opz.ww_opt.active);
//					break;
//				case 0x31:
//					opz.ww_opt.soll = data[3];
//					eeprom_write_byte((uint8_t *)EEP_WW_SOLL, opz.ww_opt.soll);
//					break;
//				case 0x32:
//					opz.ww_opt.diff = data[3];
//					eeprom_write_byte((uint8_t *)EEP_WW_DIFF, opz.ww_opt.diff);
//					break;
//				case 0x33:
//					opz.ww_opt.wait = data[3];
//					eeprom_write_byte((uint8_t *)EEP_WW_WAIT, opz.ww_opt.wait);
//					break;
//				// RAM
//				case 0x35:	opz.ww_opt.active = data[3];	break;
//				case 0x36:	opz.ww_opt.soll = data[3];		break;
//				case 0x37:	opz.ww_opt.diff = data[3];		break;
//				case 0x38:	opz.ww_opt.wait = data[3];		break;
//
//				/////////////////// direktsteuerung
//				case 0x51:
//					if (data[3]) {
//						shift |= (1 << BRENNER);
//					} else {
//						shift &= ~(1 << BRENNER);
//					}
//					shift_set(shift);
//					break;
//				case 0x52:
//					source_soll = data[3];
//					eeprom_write_byte((uint8_t *)EEP_ENERGY_SOURCE, data[3]);
//					break;
//				case 0x53:
//					if (data[3]) {
//						shift |= (1 << HK1);
//					} else {
//						shift &= ~(1 << HK1);
//					}
//					shift_set(shift);
//					break;
//				case 0x54:
//					if (data[3]) {
//						shift |= (1 << WW);
//					} else {
//						shift &= ~(1 << WW);
//					}
//					shift_set(shift);
//					break;
//				case 0x55:
//					if (data[3]) {
//						shift |= (1 << PZ);
//					} else {
//						shift &= ~(1 << PZ);
//					}
//					shift_set(shift);
//					break;
//				default: data[3] = 0x00;
//				}
//			}
//			length = 4;
//			//sende Antwort
//			udp_reply(frame, length);
//		}
//	}
}