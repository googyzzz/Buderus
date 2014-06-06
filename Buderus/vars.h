/*
 * vars.h
 *
 *  Created on: 03.06.2014
 *      Author: googy_000
 */

#ifndef VARS_H_
#define VARS_H_

#include "types.h"

uint8_t energy_source = 0;

//OneWire Sensoren Wertvariable
uint8_t Holzkessel = 0;
uint8_t Speicher0 = 0;
uint8_t Speicher1 = 0;
uint8_t Speicher2 = 0;
uint8_t Speicher3 = 0;
uint8_t Speicher4 = 0;
struct temperature_fine arbeitsZimmer;
struct temperature_fine schlafZimmer;

uint8_t diesel_t = 0;
//uint8_t temp_diesel = 0;
uint8_t temp_warmwasser = 0;
uint8_t temp_hk2_vorlauf = 0;
uint16_t brenner_status = 0;


// Brenner arbeitsvariablen
uint8_t Kessel_ist = 0;
uint8_t Brenner_status = 0;


// Heizkreis 1 Arbeitsvariablen
uint8_t HK1_soll = 0;		// read/write
uint8_t HK1_ist = 0;		// read
uint8_t HK1_active = 0x00;	// read/write
uint8_t HK1_diff = 0;		// read:write
uint8_t HK1_wait = 0;		// read/write
uint8_t HK1_state = 0;		// read

// Brauchwasser Arbeitsvariablen
uint8_t WW_soll = 0;		// read/write, Solltemperatur
uint8_t WW_ist = 0;			// read only, aktuelle Temperatur (nur bei aktiviertem Warmwasser auslesen ansonsten besser gesondert)
uint8_t WW_state = 0;		// read only, aktueller Zustand
uint8_t WW_active = 0x00;	// read/write, Aktivierungsflag
uint8_t WW_diff = 0;		// read/write, offset für erneute Aufwärmung
uint8_t WW_wait = 0;		// read/write

// Bodenheizung Arbeitsvariablen
uint8_t HK2_state = 0;		// read only, aktueller Zustand
uint8_t HK2_soll = 0;		// read/write, Solltemperatur
uint8_t HK2_ist = 0;		// read only, aktuelle Temperatur (nur der letzte gemessene Wert, nur bei aktivem Heizkreis verwenden. [state 1, 2 oder 3])
uint8_t HK2_diff = 0;		// read/write, Regelgenauigkeit in Grad
uint8_t HK2_rotate = 0;
uint8_t ROTATION_TIME = 0;
uint8_t HK2_direction = 0;
uint8_t HK2_active = 0x00;	// read/write, Aktivierungsflag
uint8_t HK2_wait = 0;		// read/write, Wartezeit zwischen den Messungen
uint8_t HK2_present = 0;	// read only


volatile uint8_t temperatur;
volatile uint8_t messtate = 0;

volatile uint8_t debug_timer = 0;
// Variablen für Systemzeit
volatile uint8_t minuten = 0;
volatile uint8_t HK1_timer = 0;

// Timer zur Heizkreis 2 Steuerung
volatile uint8_t HK2_timer = 0;
volatile uint8_t WW_timer = 0;
volatile uint8_t OW_timer = 0;
volatile uint8_t display_timer = 0;
volatile uint8_t refresh_timer = 0;

volatile uint32_t ntp_next_update;
uint32_t time_offset;

volatile uint16_t timer;

// IP Adresse des NTP Zeitservers
//#define NTP_SERVER	inet_addr(62,117,76,142) // funktioniert nicht mehr
#define NTP_SERVER	inet_addr(192,53,103,108)
#define TIMEZONE	1

//OneWire Sensoren Wertvariable
struct temperature_fine arbeitsZimmer;
struct temperature_fine schlafZimmer;

// Energiequelle
enum source_type{
	HOLZ,
	HEIZOEL,
	UNBEKANNT,
	OFF
};

uint8_t volatile source_timer = 0;
enum source_type source_ist = UNBEKANNT;
enum source_type source_soll = UNBEKANNT;	// in EEPROM ablegen
enum source_type source_turn = OFF;

volatile uint16_t ms_count;				// Milisekunden Zähler für ISR
volatile uint32_t second_count;


#endif /* VARS_H_ */
