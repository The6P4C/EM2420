#include <avr/io.h>
#include "spi.h"

/**
 * Initializes the SPI transceiver.
 */
void spi_init() {
	// Initialize MOSI (PB2) and SCK (PB1) as outputs
	DDRB |= (1 << 2) | (1 << 1);

	// Enable SPI as a master
	SPCR = (1 << SPE) | (1 << MSTR);
	SPSR = (1 << SPI2X);
}

/**
 * Transmits one byte of data via SPI, then returns the byte received.
 * @param data The byte to transmit.
 * @returns The byte received.
 */
uint8_t spi_tx_rx(uint8_t data) {
	SPDR = data;

	loop_until_bit_is_set(SPSR, SPIF);

	return SPDR;
}
