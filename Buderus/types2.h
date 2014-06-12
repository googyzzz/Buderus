/*
 * types2.h
 *
 *  Created on: 07.06.2014
 *      Author: googy_000
 */

#ifndef TYPES2_H_
#define TYPES2_H_

#include <avr/io.h>

typedef struct opt {
	uint8_t ist;
	uint8_t soll;
	uint8_t active;
	uint8_t diff;
	uint8_t wait;
	uint8_t state;
	uint8_t present;
	uint8_t rotate;
	uint8_t rotation_tume;
	uint8_t direction;
} hk_opt;

typedef struct opts {
	hk_opt hk1_opt;
	hk_opt hk2_opt;
	hk_opt ww_opt;
} optz;

typedef struct tempes {
	uint8_t atmos;
	uint8_t sp0;
	uint8_t sp1;
	uint8_t sp2;
	uint8_t sp3;
	uint8_t sp4;
	uint8_t exp;
} tempers;

// Datentyp für Temperaturen mit Nachkommastellen
// millis entspricht 1/10000
struct temperature_fine{
	uint8_t degree;
	uint16_t millis;
};

#endif /* TYPES2_H_ */
