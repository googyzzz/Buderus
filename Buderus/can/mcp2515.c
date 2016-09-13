#include <avr/io.h>
#include <util/delay.h>

#include "mcp2515.h"
#include "mcp2515_defs.h"

#include "global.h"
#include "defaults.h"

#include "../uart.h"

// -------------------------------------------------------------------------
// Schreibt/liest ein Byte über den Hardware SPI Bus

uint8_t spi_putc( uint8_t data ) {
	SPDR = data;	// Byte in SPI Buffer schreiben
	while( !( SPSR & (1<<SPIF) ) )	// Warte bis byte gesendet ist	//TODO eventuelles blockieren behandeln
		;
	return SPDR;
}

// -------------------------------------------------------------------------
void mcp2515_write_register( uint8_t adress, uint8_t data ) {
	RESET(MCP2515_CS);
	
	spi_putc(SPI_WRITE);
	spi_putc(adress);
	spi_putc(data);
	
	SET(MCP2515_CS);
}

// -------------------------------------------------------------------------
uint8_t mcp2515_read_register(uint8_t adress) {
	uint8_t data;
	
	RESET(MCP2515_CS);
	
	spi_putc(SPI_READ);
	spi_putc(adress);
	
	data = spi_putc(0xff);	
	
	SET(MCP2515_CS);
	
	return data;
}

// -------------------------------------------------------------------------
void mcp2515_bit_modify(uint8_t adress, uint8_t mask, uint8_t data)
{
	RESET(MCP2515_CS);
	
	spi_putc(SPI_BIT_MODIFY);
	spi_putc(adress);
	spi_putc(mask);
	spi_putc(data);
	
	SET(MCP2515_CS);
}

// ----------------------------------------------------------------------------
uint8_t mcp2515_read_status(uint8_t type)
{
	uint8_t data;
	
	RESET(MCP2515_CS);
	
	spi_putc(type);
	data = spi_putc(0xff);
	
	SET(MCP2515_CS);
	
	return data;
}

// -------------------------------------------------------------------------
bool mcp2515_init(void) {

	// initialize Chip Select Pin
	SET_OUTPUT(MCP2515_CS);
	SET(MCP2515_CS);
	
	// initialize Interrupt Pin
	SET_INPUT(MCP2515_INT);		// Interrupt als input
	SET(MCP2515_INT);			// Pull Up am Interrupt aktivieren
	
	// activate SPI master interface
	// SPI initialization done by LAN module
//	SPCR = (1<<SPE)|(1<<MSTR) | (1<<SPR1)|(1<<SPR0);
//	SPSR = 0;
	
	// reset MCP2515 by software reset.
	// After this he is in configuration mode.
	RESET(MCP2515_CS);
	spi_putc(SPI_RESET);
	SET(MCP2515_CS);

	// wait a little bit until the MCP2515 has restarted
	_delay_us(100);

	// load CNF1..3 Register
	RESET(MCP2515_CS);
	spi_putc(SPI_WRITE);
	spi_putc(CNF3);
	//spi_putc((1<<PHSEG21));		// Bitrate 125 kbps at 16 MHz
	//spi_putc((1<<BTLMODE)|(1<<PHSEG11));
	//spi_putc((1<<BRP2)|(1<<BRP1)|(1<<BRP0));
	//----------------------------------------
	spi_putc(0x05);		// CNF3			// Bitrate 125 kbps at 8MHz
	spi_putc(0xB1);		// CNF2
	spi_putc(0x01);		// CNF1
	//----------------------------------------for other bitrates see defs
	// activate interrupts
	spi_putc((1<<RX1IE)|(1<<RX0IE));
	SET(MCP2515_CS);


	// test if we could read back the value => is the chip accessible?
	//if (mcp2515_read_register(CNF1) != ((1<<BRP2)|(1<<BRP1)|(1<<BRP0))) {
	if (mcp2515_read_register(CNF1) != (0x01)) {
		return false;
	}

	// deaktivate the RXnBF Pins (High Impedance State)
	mcp2515_write_register(BFPCTRL, 0);

	// set TXnRTS as inputs
	mcp2515_write_register(TXRTSCTRL, 0);

	// turn off filters => receive any message
	//mcp2515_write_register(RXB0CTRL, (1<<RXM1)|(1<<RXM0));
	//mcp2515_write_register(RXB1CTRL, (1<<RXM1)|(1<<RXM0));

	// Standard ID Filter einschalten
	mcp2515_write_register(RXB0CTRL, (0 << RXM1) | (1 << RXM0));
	mcp2515_write_register(RXB1CTRL, (0 << RXM1) | (1 << RXM0));
	// Filtermasken einstellen
	uint16_t acceptedID = 0x22;
	uint16_t acceptanceMask = 0x7FF;
	mcp2515_write_register(RXM0SIDH, acceptanceMask >> 3);
	mcp2515_write_register(RXM0SIDL, acceptanceMask << 5);
	mcp2515_write_register(RXM1SIDH, acceptanceMask >> 3);
	mcp2515_write_register(RXM1SIDL, acceptanceMask << 5);
	// eigentliche Filter einstellen
	mcp2515_write_register(RXF0SIDH, acceptedID >> 3);
	mcp2515_write_register(RXF0SIDL, acceptedID << 5);
	mcp2515_write_register(RXF2SIDH, acceptedID >> 3);
	mcp2515_write_register(RXF2SIDL, acceptedID << 5);

	// reset device to normal mode
	mcp2515_write_register(CANCTRL, 0);

	return true;
}

// ----------------------------------------------------------------------------
// check if there are any new messages waiting

uint8_t mcp2515_check_message(void) {
	return (!IS_SET(MCP2515_INT));
}

// ----------------------------------------------------------------------------
// check if there is a free buffer to send messages

bool mcp2515_check_free_buffer(void)
{
	uint8_t status = mcp2515_read_status(SPI_READ_STATUS);
	
	if ((status & 0x54) == 0x54) {
		// all buffers used
		return false;
	}
	
	return true;
}

// ----------------------------------------------------------------------------
uint8_t mcp2515_get_message(tCAN *message)
{
	// read status
	uint8_t status = mcp2515_read_status(SPI_RX_STATUS);
	uint8_t addr;
	
	if (status & (1 << 6)) {
		// message in buffer 0
		addr = SPI_READ_RX;
	}
	else if (status & (1 << 7)) {
		// message in buffer 1
		addr = SPI_READ_RX | 0x04;
	}
	else {
		// Error: no message available
		return 0;
	}

	RESET(MCP2515_CS);
	spi_putc(addr);
	
	// read id
	message->id  = (uint16_t) spi_putc(0xff) << 3;
	message->id |=            spi_putc(0xff) >> 5;
	
	spi_putc(0xff);
	spi_putc(0xff);
	
	// read DLC
	uint8_t length = spi_putc(0xff) & 0x0f;
	
	message->header.length = length;
	message->header.rtr = (bit_is_set(status, 3)) ? 1 : 0;
	
	// read data
	for (uint8_t i=0;i<length;i++) {
		message->data[i] = spi_putc(0xff);
	}
	SET(MCP2515_CS);
	
	// clear interrupt flag
	if (bit_is_set(status, 6)) {
		mcp2515_bit_modify(CANINTF, (1<<RX0IF), 0);
	}
	else {
		mcp2515_bit_modify(CANINTF, (1<<RX1IF), 0);
	}
	
	return (status & 0x07) + 1;
}

// ----------------------------------------------------------------------------
uint8_t mcp2515_send_message(tCAN *message)
{
	uint8_t status = mcp2515_read_status(SPI_READ_STATUS);
	
	/* Statusbyte:
	 *
	 * Bit	Function
	 *  2	TXB0CNTRL.TXREQ
	 *  4	TXB1CNTRL.TXREQ
	 *  6	TXB2CNTRL.TXREQ
	 */
	uint8_t address;
	if (bit_is_clear(status, 2)) {
		address = 0x00;
	}
	else if (bit_is_clear(status, 4)) {
		address = 0x02;
	} 
	else if (bit_is_clear(status, 6)) {
		address = 0x04;
	}
	else {
		// all buffer used => could not send message
		return 0;
	}
	
	RESET(MCP2515_CS);
	spi_putc(SPI_WRITE_TX | address);
	
	spi_putc(message->id >> 3);
    spi_putc(message->id << 5);
	
	spi_putc(0);
	spi_putc(0);
	
	uint8_t length = message->header.length & 0x0f;
	
	if (message->header.rtr) {
		// a rtr-frame has a length, but contains no data
		spi_putc((1<<RTR) | length);
	}
	else {
		// set message length
		spi_putc(length);
		
		// data
		for (uint8_t i=0;i<length;i++) {
			spi_putc(message->data[i]);
		}
	}
	SET(MCP2515_CS);
	
	_delay_us(1);
	
	// send message
	RESET(MCP2515_CS);
	address = (address == 0) ? 1 : address;
	spi_putc(SPI_RTS | address);
	SET(MCP2515_CS);
	
	return address;
}
