#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <util/delay.h>
//#include <avr/wdt.h>		// Watchdog
//#include <avr/pgmspace.h>	// PROGMEM
#include <avr/eeprom.h>		// EEPROM
#include "network/lan.h"
#include "network/ntp.h"
#include "DS18B20/ds18b20.h"		// one wire
#include "shiftregister.h"
#include "timers/timer0.h"
#include "network/netcom.h"
#include "state_machines/hk1_state_machine.h"
#include "state_machines/hk2_state_machine.h"
#include "state_machines/ww_state_machine.h"
#include "state_machines/brenner.h"
#include "state_machines/energiequelle.h"
#include "uart.h"
#include "types.h"
#include "defines.h"
#include "buderus/buderus.h"
#include "buderus/messung.h"		// messung_init

#define SERIAL_DEBUG

void initialize() {
	// init Ports
	DDRA = 0xFF;
	PORTA = 0;
	DDRB = 0xFF;
	PORTB = 0;
	DDRC = 0xFF;
	PORTC = 0;
	DDRD = 1;		// Port D0 als eingang???
	PORTD = 0xFF;

	uart_init();
	shift_init();

	//write EEPROM
//	eeprom_write_byte((uint8_t *) 48, 0x28);

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
//	hkopt.source.buderus_temp_min = 50;
	hkopt.source.buderus_temp_diff = 5;
	hkopt.source.buderus_temp_max = 70;

	hkopt.source.buderus_on = 0;

	sei();			// interrupts aktivieren
	messung_init();	// init ICP f�r ADC Messung
	timer0_init();	// Zeitgeber ms slots
	lan_init();		// Init LAN
	ds18b20_init();	// init ds18b20
}

void prog(){
	uart_puts("_initdone_");

	uint32_t display_next_update = 0;
	uint32_t loctime;
	uint8_t s = 0, m = 0, h = 0;

	// heartbeat
	uint8_t zuzu = 0;
	char bufu[5];

	while (1) {
		zuzu++;
		itoa(zuzu, bufu, 10);
		uart_puts(bufu);
		uart_putc(10);
		uart_putc(13);

		uart_puts("_lanpoll_");
		//wdt_reset();
		lan_poll();

		uart_puts("_ntp_");
		// Time to send NTP request?
		if (second_count >= ntp_next_update) {
			if (!ntp_request(NTP_SERVER)) {
				ntp_next_update = second_count + 2;
			} else {
				ntp_next_update = second_count + 60;
			}
		}

		uart_puts("_time_");
		// Time to refresh display?
		if ((time_offset) && (second_count >= display_next_update)) {
			display_next_update = second_count + 1;

			loctime = time_offset + second_count + 60UL * 60 * TIMEZONE;
			s = loctime % 60;
			m = (loctime / 60) % 60;
			h = (loctime / 3600) % 24;
		}

		// sensors
		uart_puts("_buderus_");
		buderus_messung();		// Werte mit Buderus Steuerung auslesen
		uart_puts("_ow_");
		ow_messung();			// OneWire Messung auf beiden Str�ngen
		uart_puts("_hk1_");

		// state machines
		hk1_state_machine();	// Heizk�rper
		ww_state_machine();		// Warmwasser
		hk2_state_machine();	// Bodenheizung
		brenner();				// Heiz�lbrenner
		energiequelle();		// Umschaltung Holz/Heiz�l
	}
}

int main() {
	initialize();
	prog();
	return 0;
}
