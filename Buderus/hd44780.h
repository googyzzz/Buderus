#pragma once

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>


#define HD44780_PIN		PINC
#define HD44780_PORT	PORTC
#define HD44780_DDR		DDRC

#define HD44780_RS		PC5
#define HD44780_RW		PC6
#define HD44780_E		PC7

#define HD44780_DB		PA0//-PA3

#define HD44780_DB_MASK	(0xf<<HD44780_DB)

#define LCD_CLEAR				0x01
#define LCD_HOME				0x02
#define LCD_MOVEDIR(id,s)		(0x04 | (((id) << 1)) | (s))
#define LCD_MODE(d,c,b)			(0x08 | (((d) << 2)) | ((c) << 1) | (b))	// 0b1000 Blank the display (without clearing), 0b0100 home, 0b0010 2-zeilig 5x8-Punkt-Matrix, 0b0001 clear
#define LCD_SHIFT(sc,rl)		(0x10 | ( ((sc) << 3) | ((rl) << 2)) )
#define LCD_GOTO_CGRAM(pos)		(0x40 | ((pos) & 0x3f))
#define LCD_SET_CURSOR(pos)		(0x80 | ((pos) & 0x7f))

void hd44780_init();
void hd44780_cmd(uint8_t byte);
void hd44780_data_write(uint8_t byte);
uint8_t hd44780_data_read();
void hd44780_puts(char *str);

#define hd44780_clear()		hd44780_cmd(LCD_CLEAR)
#define hd44780_home()		hd44780_cmd(LCD_HOME)
#define hd44780_mode(d,c,b) hd44780_cmd(LCD_MODE(d,c,b))
#define hd44780_shift(sc,rl) hd44780_cmd(LCD_SHIFT(sc,rl))
#define hd44780_cgram(adr)	hd44780_cmd(LCD_GOTO_CGRAM(adr))
#define hd44780_ram(adr)	hd44780_cmd(LCD_SET_CURSOR(pos))
#define hd44780_pos(x,y)	hd44780_ram((y<<6) + x)
