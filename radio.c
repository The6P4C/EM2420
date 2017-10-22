#include <avr/io.h>
#include "em2420.h"
#include "radio.h"

// FIFOP on PE4
#define FIFOP_DDR DDRE
#define FIFOP_PIN PINE
#define FIFOP_BIT (1 << 4)

/**
 * Initializes the radio.
 */
void radio_init() {
	// Set FIFOP as an input
	FIFOP_DDR &= ~FIFOP_BIT;

	em2420_init();

	// Configure modem
	em2420_write_register_16(EM2420_REGISTER_MDMCTRL0, 0b0000001011000010);

	// Enable RX
	em2420_send_command_strobe(EM2420_STROBE_SRXON);

	em2420_send_command_strobe(EM2420_STROBE_SNOP);
}

/**
 * Checks if received data is available.
 * @returns True if data is available.
 */
bool radio_is_data_available() {
	// Check if the FIFOP pin is high
	return (FIFOP_PIN & FIFOP_BIT) == FIFOP_BIT;
}

/**
 * Retrieves a received packet.
 * @param packet A buffer, of minimum size RADIO_MAX_PACKET_SIZE bytes to store
 * the contents of the packet into.
 * @returns The length of the packet
 */
int radio_receive_packet(uint8_t packet[]) {
	uint8_t length;
	em2420_read_register_8(EM2420_REGISTER_RXFIFO, &length);

	for (int i = 0; i < length; ++i) {
		em2420_read_register_8(EM2420_REGISTER_RXFIFO, &packet[i]);
	}

	return length;
}

/**
 * Sends a packet.
 * @param packet A buffer containing the packet to send.
 * @param length The number of bytes in the buffer.
 */
void radio_send_packet(uint8_t packet[], int length) {
	em2420_write_register_8(EM2420_REGISTER_TXFIFO, length);

	for (int i = 0; i < length; ++i) {
		em2420_write_register_8(EM2420_REGISTER_TXFIFO, packet[i]);
	}

	em2420_send_command_strobe(EM2420_STROBE_STXON);
}
