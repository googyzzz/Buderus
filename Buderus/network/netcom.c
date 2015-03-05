/*
 * netcom.c
 *
 *  Created on: 11.02.2013
 *      Author: googy
 */
#include <avr/eeprom.h>
#include "lan.h"
#include "ntp.h"
#include "netcom.h"
#include "../shiftregister.h"
#include "../types.h"
#include "../defines.h"
#include "../timers/timer0.h"
#include "../main.h"

void udp_packet(eth_frame_t *frame, uint16_t len)			// udp paketereignis
{
	ip_packet_t *ip = (void*)(frame->data);
	udp_packet_t *udp = (void*)(ip->data);
	uint8_t *data = udp->data;
	uint32_t timestamp;										// zeitvariable

	if(udp->to_port == NTP_LOCAL_PORT)						// eingehende NTP nachricht
	{
		if((timestamp = ntp_parse_reply(udp->data, len)))	// extrahiere zeit, extraktion erfolgreich
		{
			time_offset = timestamp - second_count;			// rechne wirkliche zeit aus
			ntp_next_update = second_count + 12UL*60*60;	// setze nächste aktualisierung
		}
	}
	else
	{
		uint8_t length = 0;
		// steuerkomando (bulk read)
		if (data[0] == 0x55) {
			// Heizkreis 1
			data[1] = hkopt.hk1.active;
			data[2] = hkopt.hk1.ist;
			data[3] = hkopt.hk1.soll;
			data[4] = hkopt.hk1.diff;
			data[5] = hkopt.hk1.wait;
			data[6] = hkopt.hk1.state;
			// Heizkreis 2
			data[7] = hkopt.hk2.active;
			data[8] = hkopt.hk2.ist;
			data[9] = hkopt.hk2.soll;
			data[10] = hkopt.hk2.diff;
			data[11] = hkopt.hk2.wait;
			data[12] = hkopt.hk2.state;
			data[13] = hkopt.hk2.present;
			// Warmwasser
			data[14] = hkopt.ww.active;
			data[15] = hkopt.ww.ist;
			data[16] = hkopt.ww.soll;
			data[17] = hkopt.ww.diff;
			data[18] = hkopt.ww.wait;
			data[19] = hkopt.ww.state;
			// OneWire Sensoren
			data[20] = hkopt.source.atmos_temp;
			data[21] = hkopt.source.speicher0;
			data[22] = hkopt.source.speicher1;
			data[23] = hkopt.source.speicher2;
			data[24] = hkopt.source.speicher3;
			data[25] = hkopt.source.speicher4;
			data[26] = arbeitsZimmer.degree;
			data[27] = arbeitsZimmer.millis >> 8;
			data[28] = arbeitsZimmer.millis & 0xFF;
			data[29] = schlafZimmer.degree;
			data[30] = schlafZimmer.millis >> 8;
			data[31] = schlafZimmer.millis & 0xFF;
			data[32] = hkopt.source.buderus_temp;
			data[33] = (hkopt.source.brenner_state >> 8);
			data[34] = shift;
			length = 35;
			//sende Antwort
			udp_reply(frame, length);
		}
		else{
			// steuerkomando
			//schreiben
			if ((data[0] == 0xAA)&&(data[1] == 0xAA)) {
				switch (data[2]) {
				// Brenner

				//Heizkreis 1
				//default EEPROM
				case 0x10:
					hkopt.hk1.active = data[3];
					eeprom_write_byte((uint8_t *)EEP_HK1_ACTIVE, hkopt.hk1.active);
					break;
				case 0x11:
					hkopt.hk1.soll = data[3];
					eeprom_write_byte((uint8_t *)EEP_HK1_SOLL, hkopt.hk1.soll);
					break;
				case 0x12:
					hkopt.hk1.diff = data[3];
					eeprom_write_byte((uint8_t *)EEP_HK1_DIFF, hkopt.hk1.diff);
					break;
				case 0x13:
					hkopt.hk1.wait = data[3];
					eeprom_write_byte((uint8_t *)EEP_HK1_WAIT, hkopt.hk1.wait);
					break;
				//temporär RAM
				case 0x15:	hkopt.hk1.active = data[3];		break;
				case 0x16:	hkopt.hk1.soll = data[3];		break;
				case 0x17:	hkopt.hk1.diff = data[3];		break;
				case 0x18:	hkopt.hk1.wait = data[3];		break;

				// Heizkreis 2
				// EEPROM
				case 0x20:hkopt.hk2.active = data[3];
					eeprom_write_byte((uint8_t *)EEP_HK2_ACTIVE, hkopt.hk2.active);
					break;
				case 0x21:
					hkopt.hk2.soll = data[3];
					eeprom_write_byte((uint8_t *)EEP_HK2_SOLL, hkopt.hk2.soll);
					break;
				case 0x22:
					hkopt.hk2.diff = data[3];
					eeprom_write_byte((uint8_t *)EEP_HK2_DIFF, hkopt.hk2.diff);
					break;
				case 0x23:
					hkopt.hk2.wait = data[3];
					eeprom_write_byte((uint8_t *)EEP_HK2_WAIT, hkopt.hk2.wait);
					break;
				// RAM
				case 0x25:	hkopt.hk2.active = data[3];	break;
				case 0x26:	hkopt.hk2.soll = data[3];		break;
				case 0x27:	hkopt.hk2.diff = data[3];		break;
				case 0x28:	hkopt.hk2.wait = data[3];		break;


				// Warmwasser
				// EEPROM
				case 0x30:
					hkopt.ww.active = data[3];
					eeprom_write_byte((uint8_t *)EEP_WW_ACTIVE, hkopt.ww.active);
					break;
				case 0x31:
					hkopt.ww.soll = data[3];
					eeprom_write_byte((uint8_t *)EEP_WW_SOLL, hkopt.ww.soll);
					break;
				case 0x32:
					hkopt.ww.diff = data[3];
					eeprom_write_byte((uint8_t *)EEP_WW_DIFF, hkopt.ww.diff);
					break;
				case 0x33:
					hkopt.ww.wait = data[3];
					eeprom_write_byte((uint8_t *)EEP_WW_WAIT, hkopt.ww.wait);
					break;
				// RAM
				case 0x35:	hkopt.ww.active = data[3];	break;
				case 0x36:	hkopt.ww.soll = data[3];		break;
				case 0x37:	hkopt.ww.diff = data[3];		break;
				case 0x38:	hkopt.ww.wait = data[3];		break;

				/////////////////// direktsteuerung
				case 0x51:
					if (data[3]) {
						shift |= (1 << BRENNER);
					} else {
						shift &= ~(1 << BRENNER);
					}
					shift_set(shift);
					break;
				case 0x52:
					hkopt.source.source_soll = data[3];
					eeprom_write_byte((uint8_t *)EEP_ENERGY_SOURCE, data[3]);
					break;
				case 0x53:
					if (data[3]) {
						shift |= (1 << HK1);
					} else {
						shift &= ~(1 << HK1);
					}
					shift_set(shift);
					break;
				case 0x54:
					if (data[3]) {
						shift |= (1 << WW);
					} else {
						shift &= ~(1 << WW);
					}
					shift_set(shift);
					break;
				case 0x55:
					if (data[3]) {
						shift |= (1 << PZ);
					} else {
						shift &= ~(1 << PZ);
					}
					shift_set(shift);
					break;
				case 0x56:
					notreset = 0;
					break;
				default: data[3] = 0x00;
				}
			}
			length = 4;
			//sende Antwort
			udp_reply(frame, length);
		}
	}
}
