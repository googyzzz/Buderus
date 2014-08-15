#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/delay.h>
//#include <avr/wdt.h>		// Watchdog
//#include <avr/pgmspace.h>	// PROGMEM
#include <avr/eeprom.h>		// EEPROM
#include "main.h"
#include "network/lan.h"
#include "network/ntp.h"
#include "lcd-routines.h"
#include "ow.h"
#include "messung.h"
#include "shiftregister.h"
#include "timers/timer0.h"
#include "network/netcom.h"
#include "state_machines/hk1_state_machine.h"
#include "state_machines/hk2_state_machine.h"
#include "state_machines/ww_state_machine.h"
#include "uart.h"
#include "types.h"
#include "defines.h"

#define SERIAL_DEBUG


uint8_t messtate = 0;	// nur in main bei Buderus Messung

void initialize() {
	// init Watchdog
	//wdt_enable(WDTO_2S);
	//wdt_reset();

	// init Ports
	DDRA = 0xFF;
	PORTA = 0;
	DDRB = 0xFF;
	PORTB = 0;
	DDRC = 0xFF;
	PORTC = 0;
	DDRD = 1;
	PORTD = 0xFF;

	lcd_init();
	uart_init();
	shift_init();

/*	eeprom_write_byte((uint8_t *)0, 0x28);
	eeprom_write_byte((uint8_t *)1, 0xAE);
	eeprom_write_byte((uint8_t *)2, 0x67);
	eeprom_write_byte((uint8_t *)3, 0xAA);
	eeprom_write_byte((uint8_t *)4, 0x03);
	eeprom_write_byte((uint8_t *)5, 0x00);
	eeprom_write_byte((uint8_t *)6, 0x00);
	eeprom_write_byte((uint8_t *)7, 0x47);*/

	// init Parameter
	hkopt.hk1.soll = eeprom_read_byte((uint8_t *) EEP_HK1_SOLL);
	hkopt.hk1.active = eeprom_read_byte((uint8_t *) EEP_HK1_ACTIVE);
	hkopt.hk1.diff = eeprom_read_byte((uint8_t *) EEP_HK1_DIFF);
	hkopt.hk1.wait = eeprom_read_byte((uint8_t *) EEP_HK1_WAIT);

	hkopt.ww.soll = eeprom_read_byte((uint8_t *) EEP_WW_SOLL);
	hkopt.ww.active = eeprom_read_byte((uint8_t *) EEP_WW_ACTIVE);
	hkopt.ww.diff = eeprom_read_byte((uint8_t *) EEP_WW_DIFF);
	hkopt.ww.wait = eeprom_read_byte((uint8_t *) EEP_WW_WAIT);

	hkopt.hk2.soll = eeprom_read_byte((uint8_t *) EEP_HK2_SOLL);
	hkopt.hk2.diff = eeprom_read_byte((uint8_t *) EEP_HK2_DIFF);
	hkopt.hk2.ROTATION_TIME = eeprom_read_byte((uint8_t *) EEP_ROTATION_TIME);
	hkopt.hk2.active = eeprom_read_byte((uint8_t *) EEP_HK2_ACTIVE);
	hkopt.hk2.wait = eeprom_read_byte((uint8_t *) EEP_HK2_WAIT);

	temps.source_soll = eeprom_read_byte((uint8_t *) EEP_ENERGY_SOURCE);

	sei();

	// init ICP für ADC Messung
	messung_init();

	// Zeitgeber ms slots
	timer0_init();

	// Init LAN
	lan_init();

	// init OneWire
	ow_init();
	ow_init2();
}

void prog(){
	initialize();

	char buf[20];

	uint8_t ow_state = 0;

	uint32_t display_next_update = 0;
	uint32_t loctime;
	uint8_t s = 0, m = 0, h = 0;

	uint16_t erg;

	while (1) {
//		itoa(ms_count,buf,10);
//		uart_puts(buf);
//		uart_putc(10);
//		uart_putc(13);

		lan_poll();

		// Time to send NTP request?
		if (second_count >= ntp_next_update) {
			if (!ntp_request(NTP_SERVER)) {
				ntp_next_update = second_count + 2;
			} else {
				ntp_next_update = second_count + 60;
			}
		}

		// Time to refresh display?
		if ((time_offset) && (second_count >= display_next_update)) {
			display_next_update = second_count + 1;

			loctime = time_offset + second_count + 60UL * 60 * TIMEZONE;
			s = loctime % 60;
			m = (loctime / 60) % 60;
			h = (loctime / 3600) % 24;
		}

		if (display_timer) {
			display_timer = 0;
			lcd_setcursor(0, 1);

			if (h < 10) {
				lcd_data('0');
			}

			itoa(h, buf, 10);
			lcd_string(buf);
			lcd_data(':');
			if (m < 10) {
				lcd_data('0');
			}
			itoa(m, buf, 10);
			lcd_string(buf);
			lcd_data(':');
			if (s < 10) {
				lcd_data('0');
			}

			itoa(s, buf, 10);
			lcd_string(buf);
		}

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
					temps.brenner_status = 0;
					messtate = 6; // probiere nächsten Sensor
				} else { // Messung erfolgreich
					temps.brenner_status = erg;
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

		//-----------------OneWire-Messung----------------------------------------------
		switch (ow_state) {
		case 0: // starte Messung
			ow_start();
			ow_start2();
			ow_state = 1;
			OW_timer = 0;
			break;
		case 1: // warte bis Messung abgeschlossen ist
			if (OW_timer >= 2) {
				ow_state = 2;
			}
			break;
		case 2: // lese Sensor aus
			temps.Holzkessel = (ow_temp_id(EEP_OW_HOLZ) >> 4);
			ow_state = 3;
			break;
		case 3:
			temps.Speicher0 = (ow_temp_id(EEP_OW_SPEICHER_0) >> 4);
			ow_state = 4;
			break;
		case 4:
			temps.Speicher1 = (ow_temp_id(EEP_OW_SPEICHER_1) >> 4);
			ow_state = 5;
			break;
		case 5:
			temps.Speicher2 = (ow_temp_id(EEP_OW_SPEICHER_2) >> 4);
			ow_state = 6;
			break;
		case 6:
			temps.Speicher3 = (ow_temp_id(EEP_OW_SPEICHER_3) >> 4);
			ow_state = 7;
			break;
		case 7:
			temps.Speicher4 = (ow_temp_id(EEP_OW_SPEICHER_4) >> 4);
			ow_state = 8;
			break;
		case 8:
			read_ow_scratch_id2(EEP_OW_EXPERIMENT);
			arbeitsZimmer.degree = get_temp_deg2();
			arbeitsZimmer.millis = get_temp_millis2();
			ow_state = 9;
			break;
		case 9:
			read_ow_scratch_id2(EEP_OW_SCHLAFZIMMER);
			schlafZimmer.degree = get_temp_deg2();
			schlafZimmer.millis = get_temp_millis2();
			ow_state = 0;
			break;
		default:
			ow_state = 0;
		}

		hk1_state_machine();
		ww_state_machine();
		hk2_state_machine();

		// Energiequelle
		// source_turn hat 3 Zustände
		// zu Holz drehen, zu Diesel drehen, aus
		if ((temps.source_ist != temps.source_soll) || (temps.source_turn != OFF)) {	// Quelle hat sich geändert oder Mischer in Bewegung
			if (temps.source_turn != temps.source_soll) {
				temps.source_turn = OFF;
			}
			if (temps.source_turn == OFF) {
				temps.source_turn = temps.source_soll;
				source_timer = 0;
				PORTA &= ~((1 << PA3) | (1 << PA2));
				if (temps.source_turn == HOLZ) {
					PORTA |= (1 << PA3);
				} else if (temps.source_turn == HEIZOEL) {
					PORTA |= (1 << PA2);
				}
			}
			// Motor dreht, warte
			if (source_timer >= 130) {
				PORTA &= ~((1 << PA3) | (1 << PA2));
				temps.source_ist = temps.source_turn;
				temps.source_turn = OFF;
			}
		}

		hkopt.source.buderus_temp_min = 50;
		hkopt.source.buderus_temp_diff = 5;
		hkopt.source.buderus_temp_max = 70;
		if (hkopt.source.need_energy && (temps.source_ist == HEIZOEL)) {
			if (!(shift & (1 << BRENNER)) && hkopt.source.buderus_temp < (hkopt.source.buderus_temp_max - hkopt.source.buderus_temp_diff)) {
				shift |= (1 << BRENNER);
				shift_set(shift);
			} else if ((shift & (1 << BRENNER)) && hkopt.source.buderus_temp >= hkopt.source.buderus_temp_max){
				shift &= ~(1 << BRENNER);
				shift_set(shift);
			}
		}
	}
}

int main() {
	prog();
	return 0;
}
