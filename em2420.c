#define F_CPU 8000000UL

#include <stdio.h>
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

const char *register_strings[] = {
	"SNOP",
	"SXOSCON",
	"STXCAL",
	"SRXON",
	"STXON",
	"STXONCCA",
	"SRFOFF",
	"SXOSCOFF",
	"SFLUSHRX",
	"SFLUSHTX",
	"SACK",
	"SACKPEND",
	"SRXDEC",
	"STXENC",
	"SAES",
	0, // Not used
	"MAIN",
	"MDMCTRL0",
	"MDMCTRL1",
	"RSSI",
	"SYNCWORD",
	"TXCTRL",
	"RXCTRL0",
	"RXCTRL1",
	"FSCTRL",
	"SECCTRL0",
	"SECCTRL1",
	"BATTMON",
	"IOCFG0",
	"IOCFG1",
	"MANFIDL",
	"MANFIDH",
	"FSMTC",
	"MANAND",
	"MANOR",
	"AGCCTRL",
	"AGCTST0",
	"AGCTST1",
	"AGCTST2",
	"FSTST0",
	"FSTST1",
	"FSTST2",
	"FSTST3",
	"RXBPFTST",
	"FSMSTATE",
	"ADCTST",
	"DACTST",
	"TOPTST",
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	"TXFIFO",
	"RXFIFO"
};

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
 * Prints a status byte for debugging purposes.
 * @param status The status byte to print.
 */
void print_status(uint8_t status) {
	static const char *status_byte_bit_strings[] = {
		"RESERVED",
		"RSSI_VALID",
		"LOCK",
		"TX_ACTIVE",
		"ENC_BUSY",
		"TX_UNDERFLOW",
		"MOSC16M_STABLE",
		"RESERVED"
	};

	printf("Status: 0x%02X", status);

	for (int i = 0; i < 7; ++i) {
		if (status & (1 << i)) {
			printf(" ");
			printf(status_byte_bit_strings[i]);
		}
	}
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

#ifdef DEBUG
	printf("Sent command strobe 0x%02X %s\n\t", tx_byte, register_strings[address]);
	print_status(status);
	printf("\n");
#endif

	return status;
}

/**
 * Reads a register from the EM2420.
 * @param address The address of the register to read.
 * @param register_value A pointer to a 16-bit integer to store the register
 * value in.
 * @returns The status byte.
 */
uint8_t em2420_read_register(uint8_t address, uint16_t *register_value) {
	// Mask to ensure 6-bit address, and set bit 6 to signify a read
	uint8_t tx_byte = (address & 0b00111111) | 0b01000000;

	em2420_select();
	uint8_t status = spi_tx_rx(tx_byte);
	*register_value = spi_tx_rx(0) << 8;
	*register_value |= spi_tx_rx(0);
	em2420_deselect();

#ifdef DEBUG
	printf("Read register 0x%02X %s\n\tValue: 0x%02X\n\t", tx_byte, register_strings[address], *register_value);
	print_status(status);
	printf("\n");
#endif

	return status;
}
