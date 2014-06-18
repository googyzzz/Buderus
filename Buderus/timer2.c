/*
 * timer2.c
 *
 *  Created on: 11.02.2013
 *      Author: googy
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include "timer2.h"

// Zeitinterrupt, nur zur temporären Verwendung (62,5 Hz, 3750 x/min, 16ms delta)
void timer2_init(){
	timer = 0;
	TCCR2 = ((1 << WGM21) | (1 << CS22) | (1 << CS21) | (1 << CS20));	// CTC mode, prescaler 1024
	OCR2 = 250;				// CTC TOP
	TIMSK = (1 << OCIE2);	// CTC Interrupt aktivieren
}
// 3750 Aktivierungen pro Minute
ISR(TIMER2_COMP_vect)
{
	if(++cycles >= 3750)
	{
		minuten++;
		cycles = 0;
		// reset LCD
	}
	timer++;
}
