#include <avr/io.h>
#include "shiftregister.h"


uint16_t shiftGetValue() {
	return shift;
}

void shiftSet(uint16_t mask) {
	shift |= mask;
}

void shiftClear(uint16_t mask){
	shift &= ~mask;
}

void shiftCommit() {
	PORTD &= ~((1 << DATA) | (1 << CLOCK) | (1 << STROBE));		// (init) Data, Clock, Strobe -> low
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

	PORTD |= (1 << STROBE);
	PORTD &= ~(1 << STROBE);
}

// schreibt die Ausgangspins in den Schieberegister
// Bedeutung der einzelnen Stellen ist der main.h zu entnehmen
void shift_set(uint16_t v){
	PORTD &= ~((1 << DATA) | (1 << CLOCK) | (1 << STROBE));		// (init) Data, Clock, Strobe -> low
	for (uint8_t i = 0; i < 16; i++){
		PORTD &= ~((1 << DATA) | (1 << CLOCK));		// (cycle init) Data, Clock -> low
		if (1 & (v >> (15 - i))) {					// if
			PORTD |= (1 << DATA);					// setze 1
		}

		// temporär HK1 umkehren
		if ((15 - i) == S0 || (15 - i) == S1 || (15 - i) == S2 || (15 - i) == T5) {
			if (1 & (v >> (15 - i))) {
				PORTD &= ~(1 << DATA);
			} else {
				PORTD |= (1 << DATA);
			}
		}


		PORTD |= (1 << CLOCK);						// einshiften
	}
	PORTD &= ~((1 << DATA) | (1 << CLOCK));			// aufräumen

	PORTD |= (1 << STROBE);
	PORTD &= ~(1 << STROBE);
}

void shift_init() {
	DDRD |= (1 << STROBE) | (1 << DATA) | (1 << CLOCK);
	PORTD &= ~((1 << STROBE) | (1 << DATA) | (1 << CLOCK));
	shift_set(0);
}
