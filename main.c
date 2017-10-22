#define F_CPU 8000000UL

#include <stdio.h>
#include <util/delay.h>
#include "em2420.h"
#include "spi.h"
#include "uart.h"

void uart_putch(char c);

void main() {
	uart_init();
	spi_init();
	em2420_init();

	printf("Initialization finished.\n");

	uint8_t status;

	status = em2420_send_command_strobe(EM2420_STROBE_SNOP);
	printf("SNOP\n\tStatus: 0x%02X\n", status);

	_delay_ms(10);

	status = em2420_send_command_strobe(EM2420_STROBE_SXOSCON);
	printf("SXOSCON\n\tStatus: 0x%02X\n", status);

	_delay_ms(10);

	status = em2420_send_command_strobe(EM2420_STROBE_SNOP);
	printf("SNOP\n\tStatus: 0x%02X\n", status);

	while (1) {}
}
