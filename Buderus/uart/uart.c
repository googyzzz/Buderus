/*
 * uart.c
 *
 *  Created on: 03.07.2013
 *      Author: googy
 */
#include <avr/io.h>
#include "uart.h"

void uart_init() {	// USART init
	UBRRH = UBRR_VAL >> 8;
	UBRRL = UBRR_VAL & 0xFF;

	UCSRB |= (1 << TXEN); // UART TX einschalten
	UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0); // Asynchron 8N1
}

void uart_putc(unsigned char c) {
	while (!(UCSRA & (1 << UDRE))) {} /* warten bis Senden moeglich */
	UDR = c; /* sende Zeichen */
}

void uart_puts(char *s) {
	while (*s) { /* so lange *s != '\0' also ungleich dem "String-Endezeichen(Terminator)" */
		uart_putc(*s);
		s++;
	}
}
