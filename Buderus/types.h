/*
 * types.h
 *
 *  Created on: 03.02.2014
 *      Author: googy
 */

#ifndef TYPES_H_
#define TYPES_H_
#include <avr/io.h>	// uint8_t types

// Datentyp für Temperaturen mit Nachkommastellen
// millis entspricht 1/10000
struct temperature_fine{
	uint8_t degree;
	uint16_t millis;
};

struct hk1_opt {
	uint8_t soll;
	uint8_t ist;
	uint8_t active;
	uint8_t diff;
	uint8_t wait;
	uint8_t state;
};

struct ww_opt {
	uint8_t	soll;
	uint8_t	ist;
	uint8_t	state;
	uint8_t	active;
	uint8_t	diff;
	uint8_t	wait;
};

struct hk2_opt {
	uint8_t state;
	uint8_t soll;
	uint8_t ist;
	uint8_t diff;
	uint8_t rotate;
	uint8_t ROTATION_TIME;
	uint8_t direction;
	uint8_t active;
	uint8_t wait;
	uint8_t present;
};

struct opts {
	struct hk1_opt	hk1;
	struct ww_opt	ww;
	struct hk2_opt	hk2;
};

extern struct opts hkopt;

struct temperatures{
	uint8_t Holzkessel;
	uint8_t Speicher0;
	uint8_t Speicher1;
	uint8_t Speicher2;
	uint8_t Speicher3;
	uint8_t Speicher4;
	uint8_t experimentell;
};

extern struct temperatures temps;

#endif /* TYPES_H_ */
