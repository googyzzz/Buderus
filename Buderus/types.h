/*
 * types.h
 *
 *  Created on: 03.02.2014
 *      Author: googy
 */

#ifndef TYPES_H_
#define TYPES_H_
#include <avr/io.h>	// uint8_t types

// Energiequellen
enum sources_t{
	HOLZ,				// Atmos Holzkessel
	HEIZOEL,			// Buderus Heizölkessel
	UNBEKANNT,			// aktuelle Position nicht bekannt
	TURNING				// Ventil befindet sich in Bewegung
};

// Datentyp für Temperaturen mit Nachkommastellen
struct temperature_fine{
	uint8_t degree;		// volle Grade
	uint16_t millis;	// Nachkommastellen, 1/10000, 0,5°C => 5000, 0,0625°C => 625
};

// Zimmer mit Temperatursensoren als Zonen
struct zone {
	uint8_t	active;					// Aktivierung der Überwachung
	struct temperature_fine	soll;	// Soll-Temperatur
	struct temperature_fine	ist;	// Ist-Temperatur
};

struct heizoel_kessel {
	uint8_t aktiv;		// aktivierung des Brenners/Kessels
	uint16_t brenner;	// Zustand des Brenners // TODO, jetzt der Rohwert, später dedizierte Zustände
	uint8_t ist;		// momentane Kesseltemperatur
	const uint8_t max;	// (ca. 65-70°) maximale Kesseltemperatur für Heizung
	const uint8_t min;	// (cs. 60°) minimale Kesseltemperatur für Heizung
	uint8_t max_ww;		// (ca. 75-80°) maximale Kesseltemperatur für Warmwassererwärmung
	uint8_t min_ww;		// (ww + 5°) minimale Kesseltemperatur für Warmwassererwärmung
	uint8_t abs_min;	// (ca. 55°) minimale Kesseltemperatur zur Korosionsminderung
};

struct holz_kessel {
	uint8_t abgas;		// jetzt Abgastemperatur, später auch Betriebszustand
	uint8_t ist;		// momentane Kesseltemperatur
	uint8_t min;		// minimale Kesseltemperatur zur Nutzung
};

struct speicher {
	uint8_t deckel;		// höchste Stelle im Speicher, weitgehend irrelevant
	uint8_t A1;			// oberer Anschluss
	uint8_t A2;			// zweithöchster Anschluss
	uint8_t A3;			// zweittiefster Anschluss
	uint8_t A4;			// tiefster Anschluss
};

struct source_opt {
	// nur diese drei enums beeinflussen den Energie-Mischer, sonst nichts
	enum sources_t source_soll;	// read write
	enum sources_t source_turn;	// read only
	enum sources_t source_ist;	// read only

	uint8_t buderus_on;
	uint8_t buderus_state;
	uint8_t buderus_fire;
	uint8_t buderus_temp;
	uint8_t buderus_temp_max;
	uint8_t buderus_temp_diff;
	uint8_t buderus_temp_min;

	uint16_t brenner_state;
	uint8_t atmos_state;
	uint8_t atmos_temp;
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
	uint8_t soll_h;
	uint8_t soll_d;
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
	struct zone arbeitsZimmer;
	struct zone schlafZimmer;
};

extern struct opts hkopt;

char buf[20];

#endif /* TYPES_H_ */
