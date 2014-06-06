/*
 * types.h
 *
 *  Created on: 03.02.2014
 *      Author: googy
 */

#ifndef TYPES_H_
#define TYPES_H_

// Datentyp für Temperaturen mit Nachkommastellen
// millis entspricht 1/10000
struct temperature_fine{
	uint8_t degree;
	uint16_t millis;
};


// Fehler Speichervariable
struct {
	unsigned diesel_t_error:		1;
	unsigned wwasser_t_error:		1;
	unsigned FM241_error:			1;
	unsigned hk2_t_error:			1;
} errors;



#endif /* TYPES_H_ */
