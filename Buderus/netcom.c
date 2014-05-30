/*
 * netcom.c
 *
 *  Created on: 11.02.2013
 *      Author: googy
 */
#include <avr/eeprom.h>
#include "lan.h"
#include "ntp.h"
#include "hd44780.h"
#include "netcom.h"
#include "shiftregister.h"
#include "types.h"

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
			data[1] = HK1_active;
			data[2] = HK1_ist;
			data[3] = HK1_soll;
			data[4] = HK1_diff;
			data[5] = HK1_wait;
			data[6] = HK1_state;
			// Heizkreis 2
			data[7] = HK2_active;
			data[8] = HK2_ist;
			data[9] = HK2_soll;
			data[10] = HK2_diff;
			data[11] = HK2_wait;
			data[12] = HK2_state;
			data[13] = HK2_present;
			// Warmwasser
			data[14] = WW_active;
			data[15] = WW_ist;
			data[16] = WW_soll;
			data[17] = WW_diff;
			data[18] = WW_wait;
			data[19] = WW_state;
			// OneWire Sensoren
			data[20] = Holzkessel;
			data[21] = Speicher0;
			data[22] = Speicher1;
			data[23] = Speicher2;
			data[24] = Speicher3;
			data[25] = Speicher4;
			data[26] = arbeitsZimmer.degree;
			data[27] = arbeitsZimmer.millis >> 8;
			data[28] = arbeitsZimmer.millis & 0xFF;
			data[29] = schlafZimmer.degree;
			data[30] = schlafZimmer.millis >> 8;
			data[31] = schlafZimmer.millis & 0xFF;
			data[32] = diesel_t;
			data[33] = (brenner_status >> 8);
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
					HK1_active = data[3];
					eeprom_write_byte((uint8_t *)EEP_HK1_ACTIVE, HK1_active);
					break;
				case 0x11:
					HK1_soll = data[3];
					eeprom_write_byte((uint8_t *)EEP_HK1_SOLL, HK1_soll);
					break;
				case 0x12:
					HK1_diff = data[3];
					eeprom_write_byte((uint8_t *)EEP_HK1_DIFF, HK1_diff);
					break;
				case 0x13:
					HK1_wait = data[3];
					eeprom_write_byte((uint8_t *)EEP_HK1_WAIT, HK1_wait);
					break;
				//temporär RAM
				case 0x15:	HK1_active = data[3];	break;
				case 0x16:	HK1_soll = data[3];		break;
				case 0x17:	HK1_diff = data[3];		break;
				case 0x18:	HK1_wait = data[3];		break;

				// Heizkreis 2
				// EEPROM
				case 0x20:HK2_active = data[3];
					eeprom_write_byte((uint8_t *)EEP_HK2_ACTIVE, HK2_active);
					break;
				case 0x21:
					HK2_soll = data[3];
					eeprom_write_byte((uint8_t *)EEP_HK2_SOLL, HK2_soll);
					break;
				case 0x22:
					HK2_diff = data[3];
					eeprom_write_byte((uint8_t *)EEP_HK2_DIFF, HK2_diff);
					break;
				case 0x23:
					HK2_wait = data[3];
					eeprom_write_byte((uint8_t *)EEP_HK2_WAIT, HK2_wait);
					break;
				// RAM
				case 0x25:	HK2_active = data[3];	break;
				case 0x26:	HK2_soll = data[3];		break;
				case 0x27:	HK2_diff = data[3];		break;
				case 0x28:	HK2_wait = data[3];		break;


				// Warmwasser
				// EEPROM
				case 0x30:
					WW_active = data[3];
					eeprom_write_byte((uint8_t *)EEP_WW_ACTIVE, WW_active);
					break;
				case 0x31:
					WW_soll = data[3];
					eeprom_write_byte((uint8_t *)EEP_WW_SOLL, WW_soll);
					break;
				case 0x32:
					WW_diff = data[3];
					eeprom_write_byte((uint8_t *)EEP_WW_DIFF, WW_diff);
					break;
				case 0x33:
					WW_wait = data[3];
					eeprom_write_byte((uint8_t *)EEP_WW_WAIT, WW_wait);
					break;
				// RAM
				case 0x35:	WW_active = data[3];	break;
				case 0x36:	WW_soll = data[3];		break;
				case 0x37:	WW_diff = data[3];		break;
				case 0x38:	WW_wait = data[3];		break;

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
					source_soll = data[3];
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
				default: data[3] = 0x00;
				}
			}
			length = 4;
			//sende Antwort
			udp_reply(frame, length);
		}
	}
}
