/*
 * types.h
 *
 *  Created on: 03.02.2014
 *      Author: googy
 */

#ifndef TYPES_H_
#define TYPES_H_
#include <avr/io.h>	// uint8_t types

// Energiequelle
enum sources_t{
	HOLZ,
	HEIZOEL,
	UNBEKANNT,
	OFF
};

// Datentyp für Temperaturen mit Nachkommastellen
// millis entspricht 1/10000
struct temperature_fine{
	uint8_t degree;
	uint16_t millis;
};

struct source_opt {
	enum sources_t source_selected;
	uint8_t buderus_state;
	uint8_t buderus_temp;
	uint8_t buderus_temp_max;
	uint8_t buderus_temp_diff;
	uint8_t buderus_temp_min;
	uint8_t atmos_state;
	uint8_t atmos_temp;
	uint8_t need_energy;
	uint8_t speicher0;
	uint8_t speicher1;
	uint8_t speicher2;
	uint8_t speicher3;
	uint8_t speicher4;
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
	struct source_opt source;
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
	uint16_t brenner_status;
	enum sources_t source_ist;
	uint8_t source_soll;
	uint8_t source_turn;
};

extern struct temperatures temps;

#endif /* TYPES_H_ */





// Heizkreis 1 Arbeitsvariablen
//uint8_t HK1_soll = 0;		// read/write
//uint8_t HK1_ist = 0;		// read
//uint8_t HK1_active = 0x00;	// read/write
//uint8_t HK1_diff = 0;		// read:write
//uint8_t HK1_wait = 0;		// read/write
//uint8_t HK1_state = 0;		// read

// Brauchwasser Arbeitsvariablen
//uint8_t WW_soll = 0;		// read/write, Solltemperatur
//uint8_t WW_ist = 0;			// read only, aktuelle Temperatur (nur bei aktiviertem Warmwasser auslesen ansonsten besser gesondert)
//uint8_t WW_state = 0;		// read only, aktueller Zustand
//uint8_t WW_active = 0x00;	// read/write, Aktivierungsflag
//uint8_t WW_diff = 0;		// read/write, offset für erneute Aufwärmung
//uint8_t WW_wait = 0;		// read/write

// Bodenheizung Arbeitsvariablen
//uint8_t HK2_state = 0;		// read only, aktueller Zustand
//uint8_t HK2_soll = 0;		// read/write, Solltemperatur
//uint8_t HK2_ist = 0;		// read only, aktuelle Temperatur (nur der letzte gemessene Wert, nur bei aktivem Heizkreis verwenden. [state 1, 2 oder 3])
//uint8_t HK2_diff = 0;		// read/write, Regelgenauigkeit in Grad
//uint8_t HK2_rotate = 0;
//uint8_t ROTATION_TIME = 0;
//uint8_t HK2_direction = 0;
//uint8_t HK2_active = 0x00;	// read/write, Aktivierungsflag
//uint8_t HK2_wait = 0;		// read/write, Wartezeit zwischen den Messungen
//uint8_t HK2_present = 0;	// read only
