#ifndef MESSUNG_H_
#define MESSUNG_H_

#define COMP PD6
#define COMPPORT DDRD

// Multiplexer
#define S0 8
#define S1 9
#define S2 10
// ADC entladetransistor
#define T5 11

#define INIT	0
#define OK		0xFF
#define ERROR	0x0F

extern volatile uint16_t timer;
uint8_t state;
// Messergebnis ADC
volatile uint16_t mval;
volatile uint8_t flag;

uint8_t convert_mt(uint16_t mes);
uint16_t messung(uint8_t channel);
void messung_init();

#endif /* MESSUNG_H_ */
