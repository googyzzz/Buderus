#ifndef MAIN_H_
#define MAIN_H_

struct temperature_fine arbeitsZimmer;	// main und netcom
struct temperature_fine schlafZimmer;	// main und netcom

uint8_t diesel_t = 0;	// main und netcom
uint16_t brenner_status = 0;	// main netcom

// Fehler Speichervariable
struct {
	unsigned diesel_t_error:		1;
	unsigned wwasser_t_error:		1;
	unsigned FM241_error:			1;
	unsigned hk2_t_error:			1;
} errors;	// nur in main


// Heizkreis 1 Arbeitsvariablen
//uint8_t HK1_soll = 0;		// read/write
//uint8_t HK1_ist = 0;		// read
//uint8_t HK1_active = 0x00;	// read/write
//uint8_t HK1_diff = 0;		// read:write
//uint8_t HK1_wait = 0;		// read/write
//uint8_t HK1_state = 0;		// read

// Brauchwasser Arbeitsvariablen
//uint8_t WW_soll = 0;		// read/write, Solltemperatur
//uint8_t WW_ist = 0;			// read only, aktuelle Temperatur (nur bei aktiviertem Warmwasser auslesen ansonsten besser gesondert)
//uint8_t WW_state = 0;		// read only, aktueller Zustand
//uint8_t WW_active = 0x00;	// read/write, Aktivierungsflag
//uint8_t WW_diff = 0;		// read/write, offset für erneute Aufwärmung
//uint8_t WW_wait = 0;		// read/write

// Bodenheizung Arbeitsvariablen
//uint8_t HK2_state = 0;		// read only, aktueller Zustand
//uint8_t HK2_soll = 0;		// read/write, Solltemperatur
//uint8_t HK2_ist = 0;		// read only, aktuelle Temperatur (nur der letzte gemessene Wert, nur bei aktivem Heizkreis verwenden. [state 1, 2 oder 3])
//uint8_t HK2_diff = 0;		// read/write, Regelgenauigkeit in Grad
//uint8_t HK2_rotate = 0;
//uint8_t ROTATION_TIME = 0;
//uint8_t HK2_direction = 0;
//uint8_t HK2_active = 0x00;	// read/write, Aktivierungsflag
//uint8_t HK2_wait = 0;		// read/write, Wartezeit zwischen den Messungen
//uint8_t HK2_present = 0;	// read only

uint8_t messtate = 0;	// nur in main bei Buderus Messung

// Variablen für Systemzeit
volatile uint8_t minuten = 0;	// main und timer2
volatile uint8_t HK1_timer = 0;	// hk1 und timer0

extern volatile uint8_t OW_timer;	// von timer0 getrieben
extern volatile uint8_t display_timer;	// für refreshrate des LCD, von timer0 getrieben

#endif /* MAIN_H_ */
