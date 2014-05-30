#ifndef OW_H_
#define OW_H_

#define OW_OUT		PORTA
#define OW_DIR		DDRA
#define OW_PIN		PINA
#define OW			PA0
#define OW2			PA1

void ow_init();
void ow_reset();
void ow_start();
uint16_t ow_temp_id(uint8_t id);
uint8_t read_ow_scratch_id(uint8_t id);
uint8_t get_temp_deg();
uint16_t get_temp_millis();
uint8_t crc8( uint8_t *data);

void ow_init2();
void ow_reset2();
void ow_start2();
uint8_t read_ow_scratch_id2(uint8_t id);
uint8_t get_temp_deg2();
uint16_t get_temp_millis2();

#endif
