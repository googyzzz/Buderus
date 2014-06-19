/*
 * timer0.h
 *
 *  Created on: 11.02.2013
 *      Author: googy
 */

#ifndef TIMER0_H_
#define TIMER0_H_

void timer0_init();

extern volatile uint8_t Mess_timer;
extern volatile uint8_t HK1_timer;
extern volatile uint8_t HK2_timer;
extern volatile uint8_t WW_timer;
extern volatile uint8_t OW_timer;
extern volatile uint8_t display_timer;

volatile uint16_t ms_count;				// Milisekunden Zähler für ISR

extern uint8_t source_timer;

#endif /* TIMER0_H_ */
