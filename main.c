#define F_CPU 8000000UL

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include "em2420.h"
#include "spi.h"
#include "uart.h"

// FIFO on PE5
#define FIFOP_DDR DDRE
#define FIFOP_PIN PINE
#define FIFOP_BIT (1 << 4)

#define BUZZER_DDR DDRB
#define BUZZER_PORT PORTB
#define BUZZER_BIT (1 << 7)

#define BUTTON0_DDR DDRD
#define BUTTON0_PIN PIND
#define BUTTON0_BIT (1 << 0)

void do_rx() {
	int enable = 0;
	int state = 0;
	uint8_t packet[128];

	// Set FIFO as an input
	FIFOP_DDR &= ~FIFOP_BIT;

	// Set buzzer as output
	BUZZER_DDR |= BUZZER_BIT;

	em2420_write_register_16(EM2420_REGISTER_MDMCTRL0, 0b0000001011000010);

	_delay_ms(10);

	em2420_send_command_strobe(EM2420_STROBE_SRXON);

	_delay_ms(10);

	em2420_send_command_strobe(EM2420_STROBE_SNOP);

	_delay_ms(10);

	while (1) {
		if (FIFOP_PIN & FIFOP_BIT) {
			uint8_t len;
			em2420_read_register_8(EM2420_REGISTER_RXFIFO, &len);

			for (int i = 0; i < len; ++i) {
				em2420_read_register_8(EM2420_REGISTER_RXFIFO, &packet[i]);
			}

			printf("Received length 0x%02X packet:", len);
			for (int i = 0; i < len; ++i) {
				printf(" %02X", packet[i]);
			}
			printf("\n");

			enable = packet[0];
		}

		if (enable) {
			state = 1 - state;
			if (state) {
				BUZZER_PORT |= BUZZER_BIT;
			} else {
				BUZZER_PORT &= ~BUZZER_BIT;
			}
			_delay_ms(1);
		} else {
			BUZZER_PORT &= ~BUZZER_BIT;
		}
	}
}

void do_tx() {
	int prev_state = 0;

	// Set button 0 as an input
	BUTTON0_DDR &= ~BUTTON0_BIT;

	em2420_write_register_16(EM2420_REGISTER_MDMCTRL0, 0b0000001011000010);

	_delay_ms(10);

	while (1) {
		if (!!(BUTTON0_PIN & BUTTON0_BIT) != prev_state) {
			em2420_write_register_8(EM2420_REGISTER_TXFIFO, 0x01);
			em2420_write_register_8(EM2420_REGISTER_TXFIFO, prev_state);

			em2420_send_command_strobe(EM2420_STROBE_STXON);

			prev_state = 1 - prev_state;
		}
	}
}

void main() {
	uart_init();

	spi_init();
	em2420_init();

	printf("\n\nInitialization finished.\n");

	em2420_send_command_strobe(EM2420_STROBE_SNOP);

	_delay_ms(10);

	em2420_send_command_strobe(EM2420_STROBE_SXOSCON);

	_delay_ms(10);

#if 1
	do_rx();
#else
	do_tx();
#endif

	while (1) {}
}
