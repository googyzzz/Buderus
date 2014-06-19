/*
 * ww_state_machine.h
 *
 *  Created on: 11.02.2013
 *      Author: googy
 */

#ifndef WW_STATE_MACHINE_H_
#define WW_STATE_MACHINE_H_

#define WW 2

extern volatile uint16_t shift;
extern volatile uint8_t WW_timer;

void ww_state_machine();

#endif /* WW_STATE_MACHINE_H_ */
