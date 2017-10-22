#define F_CPU 8000000UL

#include <stdio.h>
#include <util/delay.h>
#include "uart.h"

void uart_putch(char c);

void main() {
	uart_init();

	while (1) {
		printf("Hello, world!\n");
		_delay_ms(1000);
	}
}
