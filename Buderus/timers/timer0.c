/*
 * timer0.c
 *
 *  Created on: 11.02.2013
 *      Author: googy
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer0.h"

volatile uint8_t display_timer = 0;
volatile uint8_t OW_timer = 0;
volatile uint8_t WW_timer = 0;
volatile uint8_t HK2_timer = 0;
volatile uint8_t HK1_timer = 0;
volatile uint8_t Mess_timer = 0;
volatile uint16_t ms_count = 0;				// Milisekunden Zähler für ISR
volatile uint32_t second_count = 0;

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
		source_timer++;
	}
	Mess_timer++;
}
