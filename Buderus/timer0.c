/*
 * timer0.c
 *
 *  Created on: 11.02.2013
 *      Author: googy
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer0.h"
#include "vars.h"


// Timer 0 init (freq = 1 kHz @ CLK = 16 MHz)
void timer0_init() {
	TCCR0 = (1 << WGM01) | (1 << CS01) | (1 << CS00);
	OCR0 = 250;
	TIMSK |= (1 << OCIE0);
}

// 1 Hz Timer Interrupt (ungenau, nicht Uhr-fähig)
ISR(TIMER0_COMP_vect) {
	if (++ms_count >= 1000) {
		++second_count;
		ms_count = 0;
		OW_timer++;
		HK2_timer++;
		HK1_timer++;
		WW_timer++;
		display_timer++;
		refresh_timer++;
		source_timer++;
	}
}
