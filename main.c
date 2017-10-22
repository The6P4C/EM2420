#define F_CPU 8000000UL

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <util/delay.h>
#include "radio.h"
#include "spi.h"
#include "uart.h"

void main() {
	uart_init();
	radio_init();

#if 1
	while (true) {
		if (radio_is_data_available()) {
			uint8_t packet[RADIO_MAX_PACKET_SIZE];
			int length = radio_receive_packet(packet);

			printf("Received length 0x%02X packet:", length);
			for (int i = 0; i < length; ++i) {
				printf(" %02X", packet[i]);
			}
			printf("\n");
		}
	}
#else
	uint8_t packet[] = { 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0xEE, 0xDD, 0xCC, 0xBB, 0xAA };
	int packet_length = sizeof(packet) / sizeof(packet[0]);
	while (true) {
		radio_send_packet(packet, packet_length);
		
		printf("Sent packet.");

		_delay_ms(1000);
	}
#endif
}
