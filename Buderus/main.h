#ifndef MAIN_H_
#define MAIN_H_

#include "types.h"

#define SERIAL_DEBUG

#define FALSE 0
#define TRUE 1

// Schieberegister Pins (schieberegidter 1)
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
#define EEP_OW_HOLZ			0	//{0x28, 0xFF, 0x8B, 0xAA, 0x03, 0x00, 0x00, 0xA5}	// Ausgang Holkessel
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


uint8_t energy_source = 0;

//OneWire Sensoren Wertvariable
uint8_t Holzkessel = 0;
uint8_t Speicher0 = 0;
uint8_t Speicher1 = 0;
uint8_t Speicher2 = 0;
uint8_t Speicher3 = 0;
uint8_t Speicher4 = 0;
struct temperature_fine arbeitsZimmer;
struct temperature_fine schlafZimmer;


uint8_t diesel_t = 0;
//uint8_t temp_diesel = 0;
uint8_t temp_warmwasser = 0;
uint8_t temp_hk2_vorlauf = 0;
uint16_t brenner_status = 0;

// Fehler Speichervariable
struct {
	unsigned diesel_t_error:		1;
	unsigned wwasser_t_error:		1;
	unsigned FM241_error:			1;
	unsigned hk2_t_error:			1;
} errors;

// Brenner arbeitsvariablen
uint8_t Kessel_ist = 0;
uint8_t Brenner_status = 0;

// Heizkreis 1 Arbeitsvariablen
uint8_t HK1_soll = 0;		// read/write
uint8_t HK1_ist = 0;		// read
uint8_t HK1_active = 0x00;	// read/write
uint8_t HK1_diff = 0;		// read:write
uint8_t HK1_wait = 0;		// read/write
uint8_t HK1_state = 0;		// read

// Brauchwasser Arbeitsvariablen
uint8_t WW_soll = 0;		// read/write, Solltemperatur
uint8_t WW_ist = 0;			// read only, aktuelle Temperatur (nur bei aktiviertem Warmwasser auslesen ansonsten besser gesondert)
uint8_t WW_state = 0;		// read only, aktueller Zustand
uint8_t WW_active = 0x00;	// read/write, Aktivierungsflag
uint8_t WW_diff = 0;		// read/write, offset für erneute Aufwärmung
uint8_t WW_wait = 0;		// read/write

// Bodenheizung Arbeitsvariablen
uint8_t HK2_state = 0;		// read only, aktueller Zustand
uint8_t HK2_soll = 0;		// read/write, Solltemperatur
uint8_t HK2_ist = 0;		// read only, aktuelle Temperatur (nur der letzte gemessene Wert, nur bei aktivem Heizkreis verwenden. [state 1, 2 oder 3])
uint8_t HK2_diff = 0;		// read/write, Regelgenauigkeit in Grad
uint8_t HK2_rotate = 0;
uint8_t ROTATION_TIME = 0;
uint8_t HK2_direction = 0;
uint8_t HK2_active = 0x00;	// read/write, Aktivierungsflag
uint8_t HK2_wait = 0;		// read/write, Wartezeit zwischen den Messungen
uint8_t HK2_present = 0;	// read only

volatile uint8_t temperatur;
volatile uint8_t messtate = 0;


extern volatile uint16_t shift;

volatile uint8_t debug_timer = 0;
// Variablen für Systemzeit
volatile uint8_t minuten = 0;
volatile uint8_t HK1_timer = 0;

// Timer zur Heizkreis 2 Steuerung
volatile uint8_t HK2_timer = 0;
volatile uint8_t WW_timer = 0;
volatile uint8_t OW_timer = 0;
volatile uint8_t display_timer = 0;
volatile uint8_t refresh_timer = 0;

volatile uint16_t timer;

// IP Adresse des NTP Zeitservers
//#define NTP_SERVER	inet_addr(62,117,76,142) // funktioniert nicht mehr
#define NTP_SERVER	inet_addr(192,53,103,108)
#define TIMEZONE	1

volatile uint16_t ms_count;				// Milisekunden Zähler für ISR
volatile uint32_t second_count;

// Energiequelle
enum {
	HOLZ,
	HEIZOEL,
	UNBEKANNT,
	OFF
};
uint8_t volatile source_timer = 0;
uint8_t source_ist = UNBEKANNT;
uint8_t source_soll = UNBEKANNT;	// in EEPROM ablegen
uint8_t source_turn = OFF;

#endif /* MAIN_H_ */
