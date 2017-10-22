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
	// Initialize RESET_N, VREG_EN and CSn as outputs
	EM2420_RESET_N_DDR |= EM2420_RESET_N_BIT;
	EM2420_VREG_EN_DDR |= EM2420_VREG_EN_BIT;
	EM2420_CSn_DDR |= EM2420_CSn_BIT;

	// Enable the EM2420's voltage regulators
	EM2420_VREG_EN_PORT |= EM2420_VREG_EN_BIT;

	// Give the voltage regulator time to enable
	_delay_ms(10);

	// Reset the EM2420 with a low pulse on its reset pin
	EM2420_RESET_N_PORT &= ~EM2420_RESET_N_BIT;
	_delay_ms(10);
	EM2420_RESET_N_PORT |= EM2420_RESET_N_BIT;

	// Give the EM2420 time to boot
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
 */
uint8_t em2420_send_command_strobe(uint8_t address) {
	// Mask to ensure 6-bit address
	uint8_t tx_byte = address & 0b00111111;

	em2420_select();
	uint8_t status = spi_tx_rx(tx_byte);
	em2420_deselect();

	return status;
}
