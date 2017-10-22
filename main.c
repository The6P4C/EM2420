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

	printf("\n\nInitialization finished.\n");

	uint16_t reg_fsmstate;

	em2420_send_command_strobe(EM2420_STROBE_SNOP);

	_delay_ms(10);

	em2420_send_command_strobe(EM2420_STROBE_SXOSCON);

	_delay_ms(10);

	em2420_read_register(EM2420_REGISTER_FSMSTATE, &reg_fsmstate);

	_delay_ms(10);

	em2420_send_command_strobe(EM2420_STROBE_SRXON);

	_delay_ms(10);

	em2420_read_register(EM2420_REGISTER_FSMSTATE, &reg_fsmstate);

	while (1) {}
}
