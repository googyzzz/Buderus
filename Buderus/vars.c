/*
 * vars.c
 *
 *  Created on: 12.06.2014
 *      Author: googy_000
 */
#include "types.h"

struct opts hkopt;
struct temperatures temps;

uint8_t source_timer = 0;
enum sources_t source_ist = UNBEKANNT;
enum sources_t source_soll = UNBEKANNT;	// in EEPROM ablegen
enum sources_t source_turn = OFF;

volatile uint32_t second_count;
