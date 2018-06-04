/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   term.h
 * Author: tleonard
 *
 * Created on July 14, 2016, 12:39 PM
 */

#ifndef TERM_H
#define TERM_H

#if 1
#define NUM_PIX_LINES		1080
#define	NUM_PIX_PER_LINE	1920
#else
#define NUM_PIX_LINES		480
#define	NUM_PIX_PER_LINE	640
#endif

#define	CHAR_WIDTH	8
//#define	CHAR_HEIGHT	8
#define	CHAR_HEIGHT	12

#define NUM_BYTES_PER_PIXEL	3
#define NUM_BITS_PER_PIXEL (NUM_BYTES_PER_PIXEL * 8)

#if 1
#define NUM_CHAR_PER_LINE	(NUM_PIX_PER_LINE/CHAR_WIDTH)
#define	NUM_LINES		(NUM_PIX_LINES / CHAR_HEIGHT)
#else
#define NUM_CHAR_PER_LINE	10
#define NUM_LINES			10
#endif

#define NUM_BYTES_PER_SCAN_LINE (NUM_PIX_PER_LINE * NUM_BYTES_PER_PIXEL)
#define NUM_BYTES_PER_CHAR_LINE (CHAR_WIDTH * NUM_BYTES_PER_PIXEL)
#define NUM_BYTES_PER_BLNK_LINE (NUM_BYTES_PER_SCAN_LINE * CHAR_HEIGHT)

typedef struct _cursor {
	int line;		// y
	int col;		// x
} cursor_t;


typedef struct _pixColor {
	u8	rgb[NUM_BYTES_PER_PIXEL];
}pixColor;


//#define FBOFS_FRM_XY(x,y,) (y*1920)


#ifdef __cplusplus
extern "C" {
#endif

//u8 term_getc(void);
void term_putc(char c);
void term_puts(char *p);
//u32 uart_has_char(void);
void term_crlf(void);
//u32 uart_getw(void);
void term_putw(u32 w);
void term_putb(u8 b);
void term_puthw(u16 h);    
void term_gotoxy(u32 line, u32 col);
void clrScrn(void);






void putc(char c);
void puts(char *s);


#ifdef __cplusplus
}
#endif

#endif /* TERM_H */

