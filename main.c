#define F_CPU 8000000UL

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include "em2420.h"
#include "spi.h"
#include "uart.h"

void main() {
	uart_init();
	spi_init();
	em2420_init();

	printf("\n\nInitialization finished.\n");

	em2420_send_command_strobe(EM2420_STROBE_SNOP);

	_delay_ms(10);

	em2420_send_command_strobe(EM2420_STROBE_SXOSCON);

	_delay_ms(10);

	em2420_send_command_strobe(EM2420_STROBE_SRXON);

	_delay_ms(10);

	em2420_write_register_8(EM2420_REGISTER_RXFIFO, 0xAA);
	em2420_write_register_8(EM2420_REGISTER_RXFIFO, 0xBB);
	em2420_write_register_8(EM2420_REGISTER_RXFIFO, 0xCC);
	em2420_write_register_8(EM2420_REGISTER_RXFIFO, 0xDD);

	_delay_ms(10);

	uint8_t reg_rxfifo;
	em2420_read_register_8(EM2420_REGISTER_RXFIFO, &reg_rxfifo);
	em2420_read_register_8(EM2420_REGISTER_RXFIFO, &reg_rxfifo);
	em2420_read_register_8(EM2420_REGISTER_RXFIFO, &reg_rxfifo);
	em2420_read_register_8(EM2420_REGISTER_RXFIFO, &reg_rxfifo);

	while (1) {}
}
