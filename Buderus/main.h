#ifndef MAIN_H_
#define MAIN_H_

struct temperature_fine arbeitsZimmer;	// main und netcom
struct temperature_fine schlafZimmer;	// main und netcom

extern volatile uint8_t OW_timer;	// von timer0 getrieben
extern volatile uint8_t display_timer;	// für refreshrate des LCD, von timer0 getrieben

uint8_t hour;

#endif /* MAIN_H_ */
