/*
 * defines.h
 *
 *  Created on: 17.06.2014
 *      Author: googy_000
 */

#ifndef DEFINES_H_
#define DEFINES_H_

//// Schieberegister Pins (schieberegidter 1)
#define BRENNER 0
#define HK1 1
#define WW 2
#define PZ 3
#define MW 4
#define MK 5
#define HK2 6
// Schieberegister 2 ist für S0, S1, S2 und T5 zuständig
// S0, S1 und S2 wählen den Temperaturfühler über ein Multiplexer aus
// T5 ist für das entladen des ADC Kondensators zuständig

// EEPROM OneWire ROM Adressen jeweils 8 Byte
#define EEP_OW_HOLZ			0	//{0x28, 0xAE, 0x67, 0xAA, 0x03, 0x00, 0x00, 0x47}	// Ausgang Holzkessel
// defekt #define EEP_OW_HOLZ			0	//{0x28, 0xFF, 0x8B, 0xAA, 0x03, 0x00, 0x00, 0xA5}	// Ausgang Holkessel
#define EEP_OW_SPEICHER_0	8	//{0x28, 0xFA, 0x70, 0xAA, 0x03, 0x00, 0x00, 0x98}	// Pufferspeicher oben
#define EEP_OW_SPEICHER_1	16	//{0x28, 0x68, 0xAC, 0xAA, 0x03, 0x00, 0x00, 0x38}	// Pufferspeicher 1. Anschluss (oben)
#define EEP_OW_SPEICHER_2	24	//{0x28, 0x19, 0x98, 0xAA, 0x03, 0x00, 0x00, 0x0C}	// Pufferspeicher 2. Anschluss
#define EEP_OW_SPEICHER_3	32	//{0x28, 0xF2, 0xAC, 0xAA, 0x03, 0x00, 0x00, 0x46}	// Pufferspeicher 3. Anschluss
#define EEP_OW_SPEICHER_4	40	//{0x28, 0x86, 0x6D, 0xAA, 0x03, 0x00, 0x00, 0xED}	// Pufferspeicher 4. Anschluss (unten)
#define EEP_OW_EXPERIMENT	48	//{0x28, 0x94, 0x80, 0xAA, 0x03, 0x00, 0x00, 0xEE}	// Testsensor Zimmer
#define EEP_OW_SCHLAFZIMMER	56	//{0x28, 0x6D, 0x5F, 0xAA, 0x03, 0x00, 0x00, 0x3B}	// Schlafzimmer
#define EEP_OW_NOT_IN_EEP2	64	//{0x28, 0x8F, 0x87, 0xAA, 0x03, 0x00, 0x00, 0x1C}	// nicht angeschlossen
#define EEP_OW_NOT_IN_EEP3	72	//{0x28, 0x6A, 0x63, 0xAA, 0x03, 0x00, 0x00, 0x1B}	// nicht angeschlossen

// Speicherplätze für Parameter im EEPROM
// Heizkreis 1 Parameter
#define EEP_HK1_SOLL	128
#define EEP_HK1_ACTIVE	129
#define EEP_HK1_DIFF	130
#define EEP_HK1_WAIT	131
// Warmwasser Parameter
#define EEP_WW_SOLL		132
#define EEP_WW_ACTIVE	133
#define EEP_WW_DIFF		134
#define EEP_WW_WAIT		135
// Heizkreis 2 Parameter
#define EEP_HK2_SOLL	136
#define EEP_HK2_DIFF	137
#define EEP_ROTATION_TIME 138
#define EEP_HK2_ACTIVE	139
#define EEP_HK2_WAIT	140
// Diesel/Holz
#define EEP_ENERGY_SOURCE 141

// IP Adresse des NTP Zeitservers
//#define NTP_SERVER	inet_addr(62,117,76,142) // funktioniert nicht mehr
#define NTP_SERVER	inet_addr(192,53,103,108)
#define TIMEZONE	1

#endif /* DEFINES_H_ */
