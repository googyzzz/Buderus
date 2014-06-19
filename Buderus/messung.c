#include <avr/io.h>
#include <avr/interrupt.h>
#include "messung.h"
#include "shiftregister.h"
#include <stdlib.h>

void messung_init() {
	// ADC als eingang
	COMPPORT &= ~(1 << COMP);
}

// ADC Messergebnis in Temperaturwert umwandeln
uint8_t convert_mt(uint16_t mes){
	uint32_t h = 14000000/mes;		// Vergleichswert Messung
	uint32_t j = 1;					// Vergleichswert Berechnung
	uint8_t temper = 0;
	while (h > j){					// Näherung des Logarithmus
		j *= 103;
		if (j > 1000000){
			j /= 100;
		}
		temper++;
		if (temper > 100) {
			h = 0;
			j = 1;
			temper = 0;
		}
	}
	return temper;
}

// Timer1 für ADC Messung
void icp_init() {
	TCNT1L = 0;
	TCNT1H = 0;
	TCCR1A = 0;										// normal mode, keine PWM Ausgänge
	TCCR1B = (1 << ICNC1) + (1<<CS10) + (1<<CS12)	// noise canceler, prescaler 1024 64µs
				+ (1 << ICES1);						// fallende Flanke auswählen
	TIFR = 0;
	TIMSK |= (1<<TICIE1) | (1 << TOIE1);			// Input-capture interrupt aktivieren, Mega32: TIMSK
}

uint16_t messung(uint8_t channel) {
	uint16_t ergebnis = 0xFFFF;
	switch (state){
	case 0:
		// Kanal auswählen
		shift |= (((channel & 0b1) << S0) | (((channel & 0b10) >> 1) << S1) | (((channel & 0b100) >> 2) << S2));
		shift &= ~(((~channel & 0b1) << S0) | (((~channel & 0b10) >> 1) << S1) | (((~channel & 0b100) >> 2) << S2));
		shift_set(shift);

		// Kondensator entladen
		shift |= (1 << T5);
		shift_set(shift);		// Kondensator Entladetransistor T5 einschalten
		mval = 0;				// Zähler initialisieren

		Mess_timer = 0;
		state = 1;
		break;
	case 1:
		if (Mess_timer > 200) {
			state = 2;
			break;
		}
		break;
	case 2:
//		// Kondensator laden
		flag = INIT;
		shift &= ~(1 << T5);
		shift_set(shift);		// Kondensator laden
		icp_init();				// Timer starten
		state = 3;
		break;
	case 3:
		// if (flag == 0) Messung wird durchgeführt, warte

		// Messung erfolgreich, Flanke an ICP
		if (flag == OK) {
			state = 0;
			flag = 0;
			ergebnis = mval;
			break;
		}
		// Messung nicht erfolgreich, Timer1 overflow
		if (flag == ERROR) {
			state = 0;
			flag = 0;
			ergebnis = 0xFFF0;
			break;
		}
		break;
	default: state = 0;
	}
	return ergebnis;
}

ISR(TIMER1_CAPT_vect)	//  Flanke an ICP pin
{
	TCCR1B = 0;			// timer stoppen

	// Timer 1 Wert auslesen
	mval = ICR1L;		// low Byte zuerst, high Byte wird gepuffert
	mval |= (ICR1H << 8);
	ICR1L = 0;
	ICR1H = 0;
	// signalisiere erfolgreiche Messung
	flag = OK;
}

// Overflow ohne ein Event an ICP pin vorher
ISR(TIMER1_OVF_vect){
	TCCR1B = 0;	// timer stoppen
	// signalisiere fehlgeschlagene Messung
	flag = ERROR;
}
