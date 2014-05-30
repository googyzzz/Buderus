/*
 * timer2.h
 *
 *  Created on: 11.02.2013
 *      Author: googy
 */

#ifndef TIMER2_H_
#define TIMER2_H_

uint16_t cycles;

extern volatile uint8_t minuten;
extern volatile uint16_t timer;

void timer2_init();

#endif /* TIMER2_H_ */
