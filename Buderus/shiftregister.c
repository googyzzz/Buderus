#include <avr/io.h>
#include "shiftregister.h"

static uint16_t shift;	// Sollzustand der Schieberegister

// aktualisiere die Schieberegister mit dem Wert in der Variable shift
void shiftCommit() {
	PORTD &= ~((1 << DATA) | (1 << CLOCK) | (1 << STROBE));		// init: Data, Clock, Strobe -> low
	for (uint8_t i = 0; i < 16; i++){
		PORTD &= ~((1 << DATA) | (1 << CLOCK));		// (cycle init) Data, Clock -> low
		if (1 & (shift >> (15 - i))) {					// if
			PORTD |= (1 << DATA);					// setze 1
		}

		// temporär HK1 umkehren
		if ((15 - i) == S0 || (15 - i) == S1 || (15 - i) == S2 || (15 - i) == T5) {
			if (1 & (shift >> (15 - i))) {
				PORTD &= ~(1 << DATA);
			} else {
				PORTD |= (1 << DATA);
			}
		}

		PORTD |= (1 << CLOCK);						// einshiften
	}
	PORTD &= ~((1 << DATA) | (1 << CLOCK));			// aufräumen
	PORTD |= (1 << STROBE);							// aus dem Puffe in die Ausgabe schreiben
	PORTD &= ~(1 << STROBE);						// aufräumen
}

void shift_init() {
	DDRD |= (1 << STROBE) | (1 << DATA) | (1 << CLOCK);	// Richtung einstellen
	PORTD &= ~((1 << STROBE) | (1 << DATA) | (1 << CLOCK));	// alle auf default low setzen
	shift = 0;
	shiftCommit();	// Schieberegister flushen
}

uint16_t shift_getValue() {
	return shift;
}

void shift_set(uint16_t mask) {
	shift |= mask;
	shiftCommit();
}

void shift_unset(uint8_t mask) {
	shift &= ~mask;
	shiftCommit();
}
