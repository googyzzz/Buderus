#include "hd44780.h"

#define high(p)		HD44780_PORT |= 1<<HD44780_##p
#define low(p)		HD44780_PORT &= ~(1<<HD44780_##p)

#define nop()		asm("nop")

void hd44780_nibble_write(uint8_t nibble) {
	high(E);
	HD44780_PORT &= ~HD44780_DB_MASK;
	HD44780_PORT |= (nibble & 0xf) << HD44780_DB;
	nop();
	nop();
	nop();
	nop();
	low(E);
}

uint8_t hd44780_nibble_read() {
	uint8_t nib;

	high(E);
	HD44780_PORT &= ~HD44780_DB_MASK;
	nop();
	nop();
	nop();
	nop();
	nib = (HD44780_PIN >> HD44780_DB) & 0xf;
	low(E);
	return nib;
}

uint8_t hd44780_busy_read() {
	uint8_t byte;

	high(RW);
	HD44780_DDR &= ~HD44780_DB_MASK;
	byte = (hd44780_nibble_read() << 4) | hd44780_nibble_read();
	low(RW);
	HD44780_DDR |= HD44780_DB_MASK;
	uart_putc(49);
	return byte & 0x80;
}

void hd44780_cmd(uint8_t byte) {
	while (hd44780_busy_read())
		;

	hd44780_nibble_write(byte >> 4);
	hd44780_nibble_write(byte);
}

uint8_t hd44780_data_read() {
	uint8_t byte;
	while (hd44780_busy_read())
		;
	HD44780_PORT |= (1 << HD44780_RS) | (1 << HD44780_RW);
	HD44780_DDR &= ~HD44780_DB_MASK;
	byte = (hd44780_nibble_read() << 4) | hd44780_nibble_read();
	HD44780_PORT &= ~((1 << HD44780_RS) | (1 << HD44780_RW));
	HD44780_DDR |= HD44780_DB_MASK;
	return byte;
}

void hd44780_data_write(uint8_t byte) {
	while (hd44780_busy_read())
		;
	high(RS);
	hd44780_nibble_write(byte >> 4);
	hd44780_nibble_write(byte);
	low(RS);
}

void hd44780_init() {
	HD44780_DDR |= HD44780_DB_MASK | (1 << HD44780_RS) | (1 << HD44780_RW) | (1
	        << HD44780_E);

	hd44780_nibble_write(3);
	_delay_ms(5);
	hd44780_nibble_write(3);
	_delay_us(160);
	hd44780_nibble_write(3);
	_delay_us(160);
	hd44780_nibble_write(2); // 4Bit Interface
}

void hd44780_puts(char *str) {
	while (*str)
		hd44780_data_write(*(str++));
}

//void lcd_init() {
//	hd44780_init(); // initialisierung 4Bit Modus
//	hd44780_mode(1,0,0); // display on/off, cursor on/off, cursor blinking on/off
//	hd44780_clear(); // display löschen
//}
