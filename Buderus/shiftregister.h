/*
 * shiftregister.h
 *
 *  Created on: 24.04.2012
 *      Author: googy
 */

#ifndef SHIFTREGISTER_H_
#define SHIFTREGISTER_H_

#define S0 8
#define S1 9
#define S2 10
#define T5 11
// Portkonfiguration
#define STROBE PD3
#define DATA PD4
#define CLOCK PD5

void shift_init();
uint16_t shift_getValue();
void shift_set(uint16_t mask);
void shift_unset(uint8_t mask);


#endif /* SHIFTREGISTER_H_ */
