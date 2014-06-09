#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/wdt.h>		// Watchdog
#include <avr/pgmspace.h>	// PROGMEM
#include <avr/eeprom.h>		// EEPROM
#include "main.h"
#include "lan.h"
#include "ntp.h"
//#include "hd44780.h"
#include "lcd-routines.h"
#include "ow.h"
#include "messung.h"
#include "shiftregister.h"
#include "timer0.h"
#include "timer2.h"
#include "netcom.h"
#include "hk1_state_machine.h"
#include "hk2_state_machine.h"
#include "ww_state_machine.h"
#include "uart.h"
#include "types.h"
#include "vars.h"
#include "defines.h"

#include "vars2.h"

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

	init_hk1_eep();
	init_ww_from_eep();
	init_hk2_eep();

	// Zähler für Uhr
	timer = 0;
	timer2_init();

	sei();

	// init ICP für ADC Messung
	messung_init();

	// Zeitgeber ms slots
	timer0_init();

	// Init LAN
	lan_init();
	lan_poll();

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
		itoa(opz.ww_opt.soll,buf,10);
		uart_puts(buf);
		uart_putc(10);
		uart_putc(13);

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

		if (minuten >= 1) {
			lcd_init();
			minuten = 0;
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
					diesel_t = 0;
					errors.diesel_t_error = TRUE;
					messtate = 2; // probiere (über)nächsten Sensor
				} else { // Messung erfolgreich
					errors.diesel_t_error = FALSE;
					diesel_t = convert_mt(erg);
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
					opz.ww_opt.ist = 255;
					errors.wwasser_t_error = TRUE;
					messtate = 3;
				} else {
					opz.ww_opt.ist = convert_mt(erg);
					errors.wwasser_t_error = FALSE;
					messtate = 3;
				}
			}
			break;

			// HK2 FM241 Modul anwesend
		case 3:
			erg = messung(3);
			if (!(erg == 0xFFFF)) {
				if (erg == 0xFFF0) {
					opz.hk2_opt.present = FALSE;
					errors.FM241_error = TRUE;
					messtate = 5;
				} else {
					errors.FM241_error = FALSE;
					if (erg < 400) {
						opz.hk2_opt.present = TRUE;
					} else {
						opz.hk2_opt.present = FALSE;
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
					brenner_status = 0;
					messtate = 6; // probiere nächsten Sensor
				} else { // Messung erfolgreich
					brenner_status = erg;
					messtate = 6;
				}
			}
			break;

			// HK2 Temperatur
		case 6:
			erg = messung(6);
			if (!(erg == 0xFFFF)) {// Messung abgeschlossen
				if (erg == 0xFFF0) {// Messung fehlgeschlagen, Timmer overflow
					opz.hk2_opt.ist = 0xFF;
					errors.hk2_t_error = TRUE;
					messtate = 0; // probiere nächsten Sensor
				} else {// Messung erfolgreich
					errors.hk2_t_error = FALSE;
					opz.hk2_opt.ist = convert_mt(erg);
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
			temps.atmos = (ow_temp_id(EEP_OW_HOLZ) >> 4);
			ow_state = 3;
			break;
		case 3:
			temps.sp0 = (ow_temp_id(EEP_OW_SPEICHER_0) >> 4);
			ow_state = 4;
			break;
		case 4:
			temps.sp1 = (ow_temp_id(EEP_OW_SPEICHER_1) >> 4);
			ow_state = 5;
			break;
		case 5:
			temps.sp2 = (ow_temp_id(EEP_OW_SPEICHER_2) >> 4);
			ow_state = 6;
			break;
		case 6:
			temps.sp3 = (ow_temp_id(EEP_OW_SPEICHER_3) >> 4);
			ow_state = 7;
			break;
		case 7:
			temps.sp4 = (ow_temp_id(EEP_OW_SPEICHER_4) >> 4);
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
		if ((source_ist != source_soll) || (source_turn != OFF)) {	// Quelle hat sich geändert oder Mischer in Bewegung
			if (source_turn != source_soll) {
				source_turn = OFF;
			}
			if (source_turn == OFF) {
				source_turn = source_soll;
				source_timer = 0;
				PORTA &= ~((1 << PA3) | (1 << PA2));
				if (source_turn == HOLZ) {
					PORTA |= (1 << PA3);
				} else if (source_turn == HEIZOEL) {
					PORTA |= (1 << PA2);
				}
			}
			// Motor dreht, warte
			if (source_timer >= 130) {
				PORTA &= ~((1 << PA3) | (1 << PA2));
				source_ist = source_turn;
				source_turn = OFF;
			}
		}
	}
}

int main() {
	prog();
	return 0;
}
