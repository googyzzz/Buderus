#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
//#include <util/delay.h>
//#include <util/atomic.h>
//#include <avr/wdt.h>		// Watchdog
//#include <avr/pgmspace.h>	// PROGMEM
#include "uart/uart.h"
#include "lcd/lcd-routines.h"
#include "shiftregister.h"
#include "messung.h"

//#include "lan.h"
//#include "lan/ntp.h"
//#include "hd44780.h"
//#include "ow.h"
//

//#include "timer0.h"
//#include "timer2.h"
//#include "lan/netcom.h"
//#include "hk1_state_machine.h"
//#include "hk2_state_machine.h"
//#include "ww_state_machine.h"
//#include "types.h"
//#include "vars.h"
//#include "defines.h"
//#include "vars2.h"

void initialize() {
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
	// init ICP für ADC Messung
	messung_init();


//	init_hk1_eep();
//	init_ww_from_eep();
//	init_hk2_eep();

	// Zähler für Uhr
//	timer = 0;
//	timer2_init();

//	sei();

	// Zeitgeber ms slots
//	timer0_init();

	// Init LAN
//	lan_init();
//	lan_poll();

	// init OneWire
//	ow_init();
//	ow_init2();
}

void prog(){
	initialize();

	while (1) {
//		lan_poll();


//		uint32_t display_next_update = 0;
//		uint32_t loctime;
//		uint8_t s = 0, m = 0, h = 0;

//		// Time to send NTP request?
//		if (second_count >= ntp_next_update) {
//			if (!ntp_request(NTP_SERVER)) {
//				ntp_next_update = second_count + 2;
//			} else {
//				ntp_next_update = second_count + 60;
//			}
//		}
//
//		// Time to refresh display?
//		if ((time_offset) && (second_count >= display_next_update)) {
//			display_next_update = second_count + 1;
//
//			loctime = time_offset + second_count + 60UL * 60 * TIMEZONE;
//			s = loctime % 60;
//			m = (loctime / 60) % 60;
//			h = (loctime / 3600) % 24;
//		}
//
//		if (minuten >= 1) {
//			lcd_init();
//			minuten = 0;
//		}
//
//		if (display_timer) {
//			display_timer = 0;
//			lcd_setcursor(0, 1);
//
//			if (h < 10) {
//				lcd_data('0');
//			}
//
//			itoa(h, buf, 10);
//			lcd_string(buf);
//			lcd_data(':');
//			if (m < 10) {
//				lcd_data('0');
//			}
//			itoa(m, buf, 10);
//			lcd_string(buf);
//			lcd_data(':');
//			if (s < 10) {
//				lcd_data('0');
//			}
//
//			itoa(s, buf, 10);
//			lcd_string(buf);
//		}

		// sensors einlesen
//		hk1_state_machine();
//		ww_state_machine();
//		hk2_state_machine();
		//source
	}
}

int main() {
	prog();
	return 0;
}
