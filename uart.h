#ifndef UART_H
#define UART_H

#define UBRR_115200	8

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>

#if defined (__cplusplus)
extern "C" {
	#endif

void uart_init(void);
int uart_getc(FILE *f);
int uart_putc(char c, FILE *f);
void uart_puts(char *s);

char *uart_gets(char *s);	
uint8_t hasChar(void);

#if defined (__cplusplus)
}
#endif


#define FDEV_SETUP_STREAM_CPP(p, g, f) \
{ \
	.buf = NULL, \
	.unget = 0, \
	.flags = f, \
	.size = 0, \
	.len = 0, \
	.put = p, \
	.get = g, \
	.udata = 0 \
}

#endif

