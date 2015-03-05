#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>		// EEPROM

#include "ow.h"

uint8_t scratchpad[9];

//---setup---
void wait_reset() {
	_delay_us(500);
}

void wait_recovery() {
	_delay_us(5);
}

void wait_write_1() {
	_delay_us(5);
}

void wait_write_0() {
	_delay_us(80);
}

void ow_rel() {
	OW_DIR &= ~(1 << OW);	// OW als Eingang
	OW_OUT |= (1 << OW);	// pullup ein
}

void ow_lo() {
	OW_OUT &= ~(1 << OW);	// pullup aus
	OW_DIR |= (1 << OW);	// OW als Ausgang
}

#define IS_OW() (OW_PIN & (1 << OW))

//---functions---

#define CRC8POLY    0x18              //0x18 = X^8+X^5+X^4+X^0

uint8_t crc8( uint8_t *data) {
	uint8_t  crc;
	uint8_t  bit_counter;
	uint8_t  b;
	uint8_t  feedback_bit;

	crc = 0;

	for (uint8_t loop_count = 0; loop_count != 8; loop_count++)
	{
		b = data[loop_count];

		bit_counter = 8;
		do {
			feedback_bit = (crc ^ b) & 0x01;

			if ( feedback_bit == 0x01 ) {
				crc = crc ^ CRC8POLY;
			}
			crc = (crc >> 1) & 0x7F;
			if ( feedback_bit == 0x01 ) {
				crc = crc | 0x80;
			}

			b = b >> 1;
			bit_counter--;

		} while (bit_counter > 0);
	}

	return crc;
}

void ow_init() {
	ow_rel();
}

void ow_reset() {
	ow_lo();
	wait_reset();
	ow_rel();
	wait_reset();
}

void ow_send(int data, uint8_t len) {
	for (int i = 0; i < len; i++) {
		if ((data & 1) > 0) { // send 1
			ow_lo();
			wait_write_1();
			ow_rel();
			wait_write_0();
		} else { // send 0
			ow_lo();
			wait_write_0();
			ow_rel();
		}
		data >>= 1;
		wait_recovery();
	}
}

void ow_write(uint8_t data) {
	ow_send(data, 8);
}

uint64_t ow_read(uint8_t len) {
	uint64_t mask = 1;
	uint64_t result = 0;
	for (int i = 0; i < len; i++) {
		ow_lo();
		wait_recovery();
		ow_rel();
		_delay_us(8);
		//Sample
		if (IS_OW()) result |= mask;
		mask <<= 1;
		wait_write_0();
	}
	return result;
}

uint8_t ow_read_byte() {
	uint8_t mask = 1;
	uint8_t result = 0;
	for (int i = 0; i < 8; i++) {
		ow_lo();
		wait_recovery();
		ow_rel();
		_delay_us(8);
		//Sample
		if (IS_OW()) result |= mask;
		mask <<= 1;
		wait_write_0();
	}
	return result;
}

void ow_start() {
	ow_reset();				//reset
	ow_write(0xCC);			//SKIP ROM [CCh]
	ow_write(0x44);			//CONVERT T [44h]
}

uint16_t ow_temp_id(uint8_t id) {
	ow_reset();				//reset
	ow_write(0x55);			//MATCH ROM [55h]
	//SEND ROM ID
	for (uint8_t i = 0; i < 8; i++) {
		ow_write(eeprom_read_byte((uint8_t *)(id + i)));	// ID aus dem EEPROM lesen
	}
	ow_write(0xBE);			//READ SCRATCHPAD [BEh]
	for (uint8_t i = 0; i < 9; i++) {
		scratchpad[i] = ow_read_byte();
	}
	ow_reset();				//nothing more to read, stop
//	if (crc8(&scratchpad[0]) != scratchpad[8]) {
//		return 0b0000011111110000;	// entspricht 127C, Fehler
//	}
	return scratchpad[1] << 8 | scratchpad[0];
}

uint8_t check_scratch_crc() {
	//uint8_t crc = scratchpd[8];

	return 1;
}

uint8_t read_ow_scratch_id(uint8_t id) {
	ow_reset();				//reset
	ow_write(0x55);			//MATCH ROM [55h]
	//SEND ROM ID
	for (uint8_t i = 0; i < 8; i++) {
		ow_write(eeprom_read_byte((uint8_t *)(id + i)));	// ID aus dem EEPROM lesen
	}
	ow_write(0xBE);			//READ SCRATCHPAD [BEh]
	for (uint8_t i = 0; i < 9; i++) {
		scratchpad[i] = ow_read_byte();
	}
	ow_reset();				//nothing more to read, stop
	return check_scratch_crc();
}

uint8_t get_temp_deg() {
	return (scratchpad[1] << 4) | (scratchpad[0] >> 4);
}

uint16_t get_temp_millis() {
	uint16_t erg = 0;
	if (scratchpad[0] & 0b1000) {
		erg += 5000;
	}
	if (scratchpad[0] & 0b100) {
		erg += 2500;
	}
	if (scratchpad[0] & 0b10) {
		erg += 1250;
	}
	if (scratchpad[0] & 0b1) {
		erg += 625;
	}
	return erg;
}
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////



void ow_rel2() {
	OW_DIR &= ~(1 << OW2);	// OW als Eingang
	OW_OUT |= (1 << OW2);	// pullup ein
}

void ow_lo2() {
	OW_OUT &= ~(1 << OW2);	// pullup aus
	OW_DIR |= (1 << OW2);	// OW als Ausgang
}

#define IS_OW2() (OW_PIN & (1 << OW2))

//---functions---
void ow_init2() {
	ow_rel2();
}

void ow_reset2() {
	ow_lo2();
	wait_reset();
	ow_rel2();
	wait_reset();
}

void ow_send2(int data, uint8_t len) {
	for (int i = 0; i < len; i++) {
		if ((data & 1) > 0) { // send 1
			ow_lo2();
			wait_write_1();
			ow_rel2();
			wait_write_0();
		} else { // send 0
			ow_lo2();
			wait_write_0();
			ow_rel2();
		}
		data >>= 1;
		wait_recovery();
	}
}

void ow_write2(uint8_t data) {
	ow_send2(data, 8);
}

uint64_t ow_read2(uint8_t len) {
	uint64_t mask = 1;
	uint64_t result = 0;
	for (int i = 0; i < len; i++) {
		ow_lo2();
		wait_recovery();
		ow_rel2();
		_delay_us(8);
		//Sample
		if (IS_OW2()) result |= mask;
		mask <<= 1;
		wait_write_0();
	}
	return result;
}

uint8_t ow_read_byte2() {
	uint8_t mask = 1;
	uint8_t result = 0;
	for (int i = 0; i < 8; i++) {
		ow_lo2();
		wait_recovery();
		ow_rel2();
		_delay_us(8);
		//Sample
		if (IS_OW2()) result |= mask;
		mask <<= 1;
		wait_write_0();
	}
	return result;
}

void ow_start2() {
	ow_reset2();				//reset
	ow_write2(0xCC);			//SKIP ROM [CCh]
	ow_write2(0x44);			//CONVERT T [44h]
}

uint8_t read_ow_scratch_id2(uint8_t id) {
	ow_reset2();				//reset
	ow_write2(0x55);			//MATCH ROM [55h]
	//SEND ROM ID
	for (uint8_t i = 0; i < 8; i++) {
		ow_write2(eeprom_read_byte((uint8_t *)(id + i)));	// ID aus dem EEPROM lesen
	}
	ow_write2(0xBE);			//READ SCRATCHPAD [BEh]
	for (uint8_t i = 0; i < 9; i++) {
		scratchpad[i] = ow_read_byte2();
	}
	ow_reset2();				//nothing more to read, stop
	return 1;
}

uint8_t get_temp_deg2() {
	return (scratchpad[1] << 4) | (scratchpad[0] >> 4);
}

uint16_t get_temp_millis2() {
	uint16_t erg = 0;
	if (scratchpad[0] & 0b1000) {
		erg += 5000;
	}
	if (scratchpad[0] & 0b100) {
		erg += 2500;
	}
	if (scratchpad[0] & 0b10) {
		erg += 1250;
	}
	if (scratchpad[0] & 0b1) {
		erg += 625;
	}
	return erg;
}
