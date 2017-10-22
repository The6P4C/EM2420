#include <stdio.h>
#include "em2420.h"
#include "spi.h"
#include "uart.h"

void main() {
	uart_init();
	spi_init();
	em2420_init();

	printf("\n\nInitialization finished.\n");

	while (1) {}
}
