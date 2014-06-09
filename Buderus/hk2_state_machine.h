/*
 * hk2_state_machine.h
 *
 *  Created on: 11.02.2013
 *      Author: googy
 */

#ifndef HK2_STATE_MACHINE_H_
#define HK2_STATE_MACHINE_H_

extern volatile uint8_t HK2_timer;

void init_hk2_eep();
void hk2_state_machine();

#endif /* HK2_STATE_MACHINE_H_ */
