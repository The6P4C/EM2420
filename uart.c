#define F_CPU 8000000UL
#define BAUD 9600

#include <stdio.h>
#include "uart.h"
#include <avr/io.h>
#include <util/setbaud.h>

// Forward declare this so uart_output can use it 
void uart_putch(char c);

FILE uart_output = FDEV_SETUP_STREAM(uart_putch, NULL, _FDEV_SETUP_WRITE);

/**
 * Initializes the UART.
 */
void uart_init() {
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;

#if USE_2X
	UCSR0A |= _BV(U2X0);
#else
	UCSR0A &= ~(_BV(U2X0));
#endif

	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);

	stdout = &uart_output;
}

/**
 * Writes a character to the UART.
 * @param c The character to write.
 */
void uart_putch(char c) {
	loop_until_bit_is_set(UCSR0A, UDRE0);
	UDR0 = c;
}
