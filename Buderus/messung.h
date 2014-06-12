#ifndef MESSUNG_H_
#define MESSUNG_H_

// Comparator PORT
#define COMP PD6
#define COMPDR DDRD
#define COMPPORT PORTD

// Multiplexer
#define S0 8
#define S1 9
#define S2 10
// ADC entladetransistor
#define T5 11

#define INIT	0
#define OK		0xFF
#define ERROR	0x0F

uint8_t convert_mt(uint16_t mes);
uint16_t messung(uint8_t channel);
void messung_init();

#endif /* MESSUNG_H_ */
