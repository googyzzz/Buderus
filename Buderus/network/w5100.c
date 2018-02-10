/*
 * w5100.c
 *
 *  Created on: 10.02.2018
 *      Author: googy
 */

#include "w5100.h"
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

// Schreibt/liest ein Byte über den Hardware SPI Bus
uint8_t spi_putc( uint8_t data ) {
    SPDR = data;                        // Byte in SPI Buffer schreiben
    while( !( SPSR & (1<<SPIF) ) )      // Warte bis byte gesendet ist  //TODO eventuelles blockieren behandeln
        ;
    return SPDR;
}

void W5100_Write(uint16_t addr, uint8_t data) {
    SPI_PORT &= ~(1 << SPI_CS);         // activate CS pin to select Controller
    spi_putc(WIZNET_WRITE_OPCODE);      // Start Wiznet W5100 Write OpCode transmission
    spi_putc((addr & 0xFF00) >> 8);     // Start Wiznet W5100 Address High Bytes transmission
    spi_putc(addr & 0x00FF);            // Start Wiznet W5100 Address Low Bytes transmission
    spi_putc(data);                     // Start Data transmission
    SPI_PORT |= (1 << SPI_CS);          // release CS pin
}

unsigned char W5100_Read(uint16_t addr) {
    SPI_PORT &= ~(1 << SPI_CS);         // select slave
    spi_putc(WIZNET_READ_OPCODE);       // Start Wiznet W5100 Read OpCode transmission
    spi_putc((addr & 0xFF00) >> 8);     // Start Wiznet W5100 Address High Bytes transmission
    spi_putc(addr & 0x00FF);            // Start Wiznet W5100 Address Low Bytes transmission
    uint8_t result = spi_putc(0x00);    // Send Dummy transmission for reading the data
    SPI_PORT |= (1 << SPI_CS);          // release slave
    return (result);
}

void W5100_Init(void) {
    // Initial the AVR ATMega328 SPI Peripheral
    // Set MOSI (PORTB3),SCK (PORTB5) and PORTB2 (SS) as output, others as input
    SPI_DDR = (1 << PB3) | (1 << PB5) | (1 << PB2);
    // CS pin is not active
    SPI_PORT |= (1 << SPI_CS);
    // Enable SPI, Master Mode 0, set the clock rate fck/2
    SPCR = (1 << SPE) | (1 << MSTR);
    SPSR |= (1 << SPI2X);

    // Ethernet Setup
    unsigned char mac_addr[] = { 0x00, 0x16, 0x36, 0xDE, 0x58, 0xF6 };
    unsigned char ip_addr[] = { 192, 168, 0, 15 };
    unsigned char sub_mask[] = { 255, 255, 255, 0 };
    unsigned char gtw_addr[] = { 192, 168, 0, 1 };
    // Setting the Wiznet W5100 Mode Register: 0x0000
    W5100_Write(MR, 0x80);            // MR = 0b10000000;
    // Setting the Wiznet W5100 Gateway Address (GAR): 0x0001 to 0x0004
    W5100_Write(GAR + 0, gtw_addr[0]);
    W5100_Write(GAR + 1, gtw_addr[1]);
    W5100_Write(GAR + 2, gtw_addr[2]);
    W5100_Write(GAR + 3, gtw_addr[3]);
    // Setting the Wiznet W5100 Source Address Register (SAR): 0x0009 to 0x000E
    W5100_Write(SAR + 0, mac_addr[0]);
    W5100_Write(SAR + 1, mac_addr[1]);
    W5100_Write(SAR + 2, mac_addr[2]);
    W5100_Write(SAR + 3, mac_addr[3]);
    W5100_Write(SAR + 4, mac_addr[4]);
    W5100_Write(SAR + 5, mac_addr[5]);
    // Setting the Wiznet W5100 Sub Mask Address (SUBR): 0x0005 to 0x0008
    W5100_Write(SUBR + 0, sub_mask[0]);
    W5100_Write(SUBR + 1, sub_mask[1]);
    W5100_Write(SUBR + 2, sub_mask[2]);
    W5100_Write(SUBR + 3, sub_mask[3]);
    // Setting the Wiznet W5100 IP Address (SIPR): 0x000F to 0x0012
    W5100_Write(SIPR + 0, ip_addr[0]);
    W5100_Write(SIPR + 1, ip_addr[1]);
    W5100_Write(SIPR + 2, ip_addr[2]);
    W5100_Write(SIPR + 3, ip_addr[3]);

    // Setting the Wiznet W5100 RX and TX Memory Size (2KB),
    W5100_Write(RMSR, NET_MEMALLOC);
    W5100_Write(TMSR, NET_MEMALLOC);
}

void close(uint8_t sock) {
    if (sock != 0)
        return;

    // Send Close Command
    W5100_Write(S0_CR, CR_CLOSE);
    // Waiting until the S0_CR is clear
    while (W5100_Read(S0_CR))
        ;
}

void disconnect(uint8_t sock) {
    if (sock != 0)
        return;
    // Send Disconnect Command
    W5100_Write(S0_CR, CR_DISCON);
    // Wait for Disconecting Process
    while (W5100_Read(S0_CR))
        ;
}

uint8_t socket(uint8_t sock, uint8_t eth_protocol, uint16_t tcp_port) {
    uint8_t retval = 0;
    if (sock != 0)
        return retval;

    // Make sure we close the socket first
    if (W5100_Read(S0_SR) == SOCK_CLOSED) {
        close(sock);
    }
    // Assigned Socket 0 Mode Register
    W5100_Write(S0_MR, eth_protocol);

    // Now open the Socket 0
    W5100_Write(S0_PORT, ((tcp_port & 0xFF00) >> 8));
    W5100_Write(S0_PORT + 1, (tcp_port & 0x00FF));
    W5100_Write(S0_CR, CR_OPEN);                   // Open Socket
    // Wait for Opening Process
    while (W5100_Read(S0_CR))
        ;
    // Check for Init Status
    if (W5100_Read(S0_SR) == SOCK_INIT)
        retval = 1;
    else
        close(sock);

    return retval;
}

uint8_t listen(uint8_t sock) {
    uint8_t retval = 0;
    if (sock != 0)
        return retval;
    if (W5100_Read(S0_SR) == SOCK_INIT) {
        // Send the LISTEN Command
        W5100_Write(S0_CR, CR_LISTEN);

        // Wait for Listening Process
        while (W5100_Read(S0_CR))
            ;
        // Check for Listen Status
        if (W5100_Read(S0_SR) == SOCK_LISTEN)
            retval = 1;
        else
            close(sock);
    }
    return retval;
}

uint16_t send(uint8_t sock, const uint8_t *buf, uint16_t buflen) {
    uint16_t ptr, offaddr, realaddr, txsize, timeout;

    if (buflen <= 0 || sock != 0)
        return 0;
    // Make sure the TX Free Size Register is available
    txsize = W5100_Read(SO_TX_FSR);
    txsize = (((txsize & 0x00FF) << 8) + W5100_Read(SO_TX_FSR + 1));
    timeout = 0;
    while (txsize < buflen) {
        _delay_ms(1);
        txsize = W5100_Read(SO_TX_FSR);
        txsize = (((txsize & 0x00FF) << 8) + W5100_Read(SO_TX_FSR + 1));
        // Timeout for approx 1000 ms
        if (timeout++ > 1000) {
            // Disconnect the connection
            disconnect(sock);
            return 0;
        }
    }

    // Read the Tx Write Pointer
    ptr = W5100_Read(S0_TX_WR);
    offaddr = (((ptr & 0x00FF) << 8) + W5100_Read(S0_TX_WR + 1));

    while (buflen) {
        buflen--;
        // Calculate the real W5100 physical Tx Buffer Address
        realaddr = TXBUFADDR + (offaddr & TX_BUF_MASK);
        // Copy the application data to the W5100 Tx Buffer
        W5100_Write(realaddr, *buf);
        offaddr++;
        buf++;
    }

    // Increase the S0_TX_WR value, so it point to the next transmit
    W5100_Write(S0_TX_WR, (offaddr & 0xFF00) >> 8);
    W5100_Write(S0_TX_WR + 1, (offaddr & 0x00FF));

    // Now Send the SEND command
    W5100_Write(S0_CR, CR_SEND);

    // Wait for Sending Process
    while (W5100_Read(S0_CR))
        ;

    return 1;
}

uint16_t recv(uint8_t sock, uint8_t *buf, uint16_t buflen) {
    uint16_t ptr, offaddr, realaddr;

    if (buflen <= 0 || sock != 0)
        return 1;

    // If the request size > MAX_BUF,just truncate it
    if (buflen > MAX_BUF)
        buflen = MAX_BUF - 2;
    // Read the Rx Read Pointer
    ptr = W5100_Read(S0_RX_RD);
    offaddr = (((ptr & 0x00FF) << 8) + W5100_Read(S0_RX_RD + 1));

    while (buflen) {
        buflen--;
        realaddr = RXBUFADDR + (offaddr & RX_BUF_MASK);
        *buf = W5100_Read(realaddr);
        offaddr++;
        buf++;
    }
    *buf = '\0';        // String terminated character

    // Increase the S0_RX_RD value, so it point to the next receive
    W5100_Write(S0_RX_RD, (offaddr & 0xFF00) >> 8);
    W5100_Write(S0_RX_RD + 1, (offaddr & 0x00FF));

    // Now Send the RECV command
    W5100_Write(S0_CR, CR_RECV);
    _delay_us(5);    // Wait for Receive Process

    return 1;
}

uint16_t recv_size(void) {
    return ((W5100_Read(S0_RX_RSR) & 0x00FF) << 8) + W5100_Read(S0_RX_RSR + 1);
}

int strindex(char *s, char *t) {
    uint16_t i, n;

    n = strlen(t);
    for (i = 0; *(s + i); i++) {
        if (strncmp(s + i, t, n) == 0)
            return i;
    }
    return -1;
}
