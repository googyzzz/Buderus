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
