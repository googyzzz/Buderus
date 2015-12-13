/*
 * ds18b20.h
 *
 *  Created on: 02.10.2015
 *      Author: googy
 */

#ifndef DS18B20_DS18B20_H_
#define DS18B20_DS18B20_H_

extern struct opts hkopt;
extern volatile uint8_t OW_timer;	// stammt aus timers/timer0.c
void ds18b20_init();
void ow_messung();

#endif /* DS18B20_DS18B20_H_ */
