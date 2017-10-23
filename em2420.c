#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "em2420.h"
#include "spi.h"

// RESET_N on PD7
#define EM2420_RESET_N_DDR DDRD
#define EM2420_RESET_N_PORT PORTD
#define EM2420_RESET_N_BIT (1 << 7)

// VREG_EN on PB5
#define EM2420_VREG_EN_DDR DDRB
#define EM2420_VREG_EN_PORT PORTB
#define EM2420_VREG_EN_BIT (1 << 5)

// CSn on PB0
#define EM2420_CSn_DDR DDRB
#define EM2420_CSn_PORT PORTB
#define EM2420_CSn_BIT (1 << 0)

/**
 * Initializes the EM2420.
 */
void em2420_init() {
	spi_init();

	// Initialize RESET_N, VREG_EN and CSn as outputs
	EM2420_RESET_N_DDR |= EM2420_RESET_N_BIT;
	EM2420_VREG_EN_DDR |= EM2420_VREG_EN_BIT;
	EM2420_CSn_DDR |= EM2420_CSn_BIT;

	// Enable the EM2420's voltage regulators
	EM2420_VREG_EN_PORT |= EM2420_VREG_EN_BIT;

	// Give the voltage regulator time to enable - according to the datasheet
	// this should only take a max of 0.6ms, but let's wait for 10ms to be safe
	_delay_ms(10);

	// Reset the EM2420 with a low pulse on its reset pin  - 50ms is enough to
	// make sure it's reset
	EM2420_RESET_N_PORT &= ~EM2420_RESET_N_BIT;
	_delay_ms(50);
	EM2420_RESET_N_PORT |= EM2420_RESET_N_BIT;

	// Give the EM2420 time to boot
	_delay_ms(10);

	// Turn on the oscillator
	em2420_send_command_strobe(EM2420_STROBE_SXOSCON);

	_delay_ms(10);
}

/**
 * Selects the EM2420 using the CS pin.
 */
void em2420_select() {
	// CS is active low
	EM2420_CSn_PORT &= ~EM2420_CSn_BIT;
}

/**
 * De-selects the EM2420 using the CS pin.
 */
void em2420_deselect() {
	// CS is active low
	EM2420_CSn_PORT |= EM2420_CSn_BIT;
}

/**
 * Sends a command strobe to the EM2420.
 * @param address The address to send the command strobe to.
 * @returns The status byte.
 */
uint8_t em2420_send_command_strobe(uint8_t address) {
	// Mask to ensure 6-bit address
	uint8_t tx_byte = address & 0b00111111;

	em2420_select();
	uint8_t status = spi_tx_rx(tx_byte);
	em2420_deselect();

	return status;
}

/**
 * Reads a 16-bit register from the EM2420.
 * @param address The address of the register to read.
 * @param register_value A pointer to a 16-bit integer to store the register
 * value in.
 * @returns The status byte.
 */
uint8_t em2420_read_register_16(uint8_t address, uint16_t *register_value) {
	// Mask to ensure 6-bit address, and set bit 6 to signify a read
	uint8_t tx_byte = (address & 0b00111111) | 0b01000000;

	em2420_select();
	uint8_t status = spi_tx_rx(tx_byte);
	*register_value = spi_tx_rx(0) << 8;
	*register_value |= spi_tx_rx(0);
	em2420_deselect();

	return status;
}

/**
 * Writes a 16-bit register to the EM2420.
 * @param address The address of the register to write.
 * @param register_value The value to write to the register.
 * @returns The status byte.
 */
uint8_t em2420_write_register_16(uint8_t address, uint16_t register_value) {
	// Mask to ensure 6-bit address
	uint8_t tx_byte = address & 0b00111111;
	uint8_t register_value_high = (register_value >> 8) & 0xFF;
	uint8_t register_value_low = register_value & 0xFF;

	em2420_select();
	uint8_t status = spi_tx_rx(tx_byte);
	spi_tx_rx(register_value_high);
	spi_tx_rx(register_value_low);
	em2420_deselect();

	return status;
}

/**
 * Reads a 16-bit register from the EM2420.
 * @param address The address of the register to read.
 * @param register_value A pointer to an 8-bit integer to store the register
 * value in.
 * @returns The status byte.
 */
uint8_t em2420_read_register_8(uint8_t address, uint8_t *register_value) {
	// Mask to ensure 6-bit address, and set bit 6 to signify a read
	uint8_t tx_byte = (address & 0b00111111) | 0b01000000;

	em2420_select();
	uint8_t status = spi_tx_rx(tx_byte);
	*register_value = spi_tx_rx(0);
	em2420_deselect();

	return status;
}

/**
 * Writes a 8-bit register to the EM2420.
 * @param address The address of the register to write.
 * @param register_value The value to write to the register.
 * @returns The status byte.
 */
uint8_t em2420_write_register_8(uint8_t address, uint8_t register_value) {
	// Mask to ensure 6-bit address
	uint8_t tx_byte = address & 0b00111111;

	em2420_select();
	uint8_t status = spi_tx_rx(tx_byte);
	spi_tx_rx(register_value);
	em2420_deselect();

	return status;
}
