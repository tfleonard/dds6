/*
 * uart.c
 *
 * Created: 2/4/2018 11:31:43 AM
 *  Author: tleonard
 */ 
 #include "avr/io.h"
 #include "uart.h"



 void uart_init( void)
 {
 
 //  Set baud rate for 115200 
 	 UBRR0H = (uint8_t)( UBRR_115200 >> 8);
	 UBRR0L = (uint8_t) UBRR_115200	;

//	 Enable receiver and transmitter
	 UCSR0B = (1 << RXEN0) | (1 << TXEN0);

//	8data, 2 stop bits
	 UCSR0C = (1 << USBS0) | (3 << UCSZ00);
}

int uart_getc(FILE *f) {
	while (!(UCSR0A & (1 << RXC0)));
	return UDR0;
}


int  uart_putc(char c, FILE *f) {
	if (c == '\n') {
		uart_putc('\r', f);
	}
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = c;
	return 0;
}


void uart_puts(char *s) {

	while (*s) {
		uart_putc(*s++, NULL);
	}
}


char *uart_gets(char *s) {
uint8_t c;
char *ss = s;	
	
	while(1) {
		c = uart_getc(NULL);
		*s++ = c;
		if (c == '\n') {
			*s = '\0';
			break;
		}
	}
	return ss;
}


uint8_t hasChar(void) {
	return (UCSR0A & (1 << RXC0));
}
