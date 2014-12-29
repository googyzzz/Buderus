#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/wdt.h>		// Watchdog
//#include <avr/pgmspace.h>	// PROGMEM
#include <avr/eeprom.h>		// EEPROM
#include "main.h"
#include "network/lan.h"
#include "network/ntp.h"
#include "lcd-routines.h"
#include "DS18B20/ow.h"
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
	wdt_enable(WDTO_2S);
	//wdt_reset();
	notreset = 1;

	// init Ports
	DDRA = 0xFF;
	PORTA = 0;
	DDRB = 0xFF;
	PORTB = 0;
	DDRC = 0xFF;
	PORTC = 0;
	DDRD = 1;		// Port D0 als eingang???
	PORTD = 0xFF;

	lcd_init();
	uart_init();
	shift_init();

	//write EEPROM
//	eeprom_write_byte((uint8_t *) 48, 0x28);

	// read EEPROM
//	uint8_t z = 0;
//	char bu[3];
//	z = eeprom_read_byte((uint8_t *) 16);
//	itoa(z, bu, 16);
//	uart_puts(bu);

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

	hkopt.source.source_soll = eeprom_read_byte((uint8_t *) EEP_ENERGY_SOURCE);
	// nachdem Start ist die aktuelle Mischerposition unbekannt
	hkopt.source.source_ist = UNBEKANNT;

	// FIXME brennerwerte aus dem eeprom:
	hkopt.source.buderus_temp_min = 50;
	hkopt.source.buderus_temp_diff = 5;
	hkopt.source.buderus_temp_max = 70;

	hkopt.source.buderus_on = 0;


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

	uint8_t ow_state = 0;

	uint32_t display_next_update = 0;
	uint32_t loctime;
	uint8_t s = 0, m = 0, h = 0;

	uint16_t erg;

	// heartbeat
	uint8_t zuzu = 0;
	char bufu[5];

	while (1) {
		zuzu++;
		itoa(zuzu, bufu, 10);
		uart_puts(bufu);
		uart_putc(10);
		uart_putc(13);

		if (notreset) {
			wdt_reset();
		}
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
			hour = h;
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
					// TODO zentraler Fehlerspeicher
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
					hkopt.source.brenner_state = 0;
					messtate = 6; // probiere nächsten Sensor
				} else { // Messung erfolgreich
					hkopt.source.brenner_state = erg;
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

		// Brenner
		switch (hkopt.source.buderus_state) {
		case 0:
			if (hkopt.source.buderus_on && hkopt.source.source_ist == HEIZOEL) {
				hkopt.source.buderus_state = 1;
			}
			break;
		case 1:	// warte bis Energie nötig ist
			if (hkopt.source.buderus_fire) {
				shift |= (1 << BRENNER);
				hkopt.source.buderus_state = 2;
				break;
			}
			if (!hkopt.source.buderus_on ||
					hkopt.source.source_ist != HEIZOEL) {
				shift &= ~(1 << BRENNER);
				hkopt.source.buderus_state = 0;
				break;
			}
			break;
		case 2:	// heize bis maximale Temperatur erreicht ist
			if (hkopt.source.buderus_temp > hkopt.source.buderus_temp_max) {
				shift &= ~(1 << BRENNER);
				hkopt.source.buderus_state = 3;
				break;
			}
			if (!hkopt.source.buderus_fire) {
				shift &= ~(1 << BRENNER);
				hkopt.source.buderus_state = 1;
				break;
			}
			if (!hkopt.source.buderus_on ||
					hkopt.source.source_ist != HEIZOEL) {
				shift &= ~(1 << BRENNER);
				hkopt.source.buderus_state = 0;
				break;
			}
			break;
		case 3:	// warte bis wieder geheizt werden muss
			if (hkopt.source.buderus_temp < (hkopt.source.buderus_temp_max - hkopt.source.buderus_temp_diff)) {
				shift |= (1 << BRENNER);
				hkopt.source.buderus_state = 2;
				break;
			}
			if (!hkopt.source.buderus_fire) {
				shift &= ~(1 << BRENNER);
				hkopt.source.buderus_state = 1;
				break;
			}
			if (!hkopt.source.buderus_on ||
					hkopt.source.source_ist != HEIZOEL) {
				shift &= ~(1 << BRENNER);
				hkopt.source.buderus_state = 0;
				break;
			}
			break;
		default:
			hkopt.source.buderus_state = 0;
		}

		// Energiequelle
		if (hkopt.source.source_ist != hkopt.source.source_soll) {
			if (hkopt.source.source_ist != TURNING) {
				PORTA &= ~((1 << PA3) | (1 << PA2));
				if (hkopt.source.source_soll == HOLZ) {
					PORTA |= (1 << PA3);
				} else if (hkopt.source.source_soll == HEIZOEL) {
					PORTA |= (1 << PA2);
				}
				source_timer = 0;
				hkopt.source.source_ist = TURNING;
				hkopt.source.source_turn = hkopt.source.source_soll;
			}
			if (source_timer >= 130 && hkopt.source.source_ist == TURNING) {
				PORTA &= ~((1 << PA3) | (1 << PA2));
				hkopt.source.source_ist = hkopt.source.source_turn;
			}
		}
	}
}

int main() {
	prog();
	return 0;
}
