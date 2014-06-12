/*
 * uart.h
 *
 *  Created on: 03.07.2013
 *      Author: googy
 */

#ifndef UART_H_
#define UART_H_

//#ifdef SERIAL_DEBUG
#ifndef F_CPU
#warning "F_CPU war noch nicht definiert, wird nun nachgeholt mit 16000000"
#define F_CPU 16000000UL
#endif
#define BAUD 9600UL      // Baudrate
#define UBRR_VAL ((F_CPU+BAUD*8)/(BAUD*16)-1)   // clever runden
#define BAUD_REAL (F_CPU/(16*(UBRR_VAL+1)))     // Reale Baudrate
#define BAUD_ERROR ((BAUD_REAL*1000)/BAUD) // Fehler in Promille, 1000 = kein Fehler.
#if ((BAUD_ERROR<990) || (BAUD_ERROR>1010))
#error Systematischer Fehler der Baudrate grösser 1% und damit zu hoch!
#endif

void uart_init();
void uart_putc(unsigned char c);
void uart_puts(char *s);
//#endif
#endif /* UART_H_ */
