/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   lcd.h
 * Author: tleonard
 *
 */

#ifndef LCD_H
#define LCD_H

#include "colors.h"

//
// uncomment the next line for the Transistor Tester LCD
// comment it out for the 1.8 LCD
//
//#define LCD_18	1		// new LCD display to UNO test bed (Red)
#define LCD_TT	1		// transistor tester PINS
//#define LCD_TB	1		// tt lcd to UNO test bed


//
// uncomment the next line to flip the display
//
#define FLIP_XY	1


#ifndef FLIP_XY
#define LCD_MEM_CTL (FLAG_RGB_RGB)
#define NUM_PIX_LINES		160
#define	NUM_PIX_PER_LINE	128
#elif LCD_TT
#define LCD_MEM_CTL (FLAG_RGB_RGB | FLAG_MV_REV | FLAG_MX_REV)
#define NUM_PIX_LINES    128
#define NUM_PIX_PER_LINE  160
#else
#define LCD_MEM_CTL (FLAG_RGB_RGB | FLAG_MV_REV | FLAG_MY_REV )
#define NUM_PIX_LINES		128
#define	NUM_PIX_PER_LINE	160
#endif

#define	CHAR_WIDTH	8
#define	CHAR_HEIGHT	12

#define NUM_BYTES_PER_PIXEL	3
#define NUM_BITS_PER_PIXEL (NUM_BYTES_PER_PIXEL * 8)
#define NUM_BYTES_PER_CHAR (CHAR_WIDTH * CHAR_HEIGHT * NUM_BYTES_PER_PIXEL)

#define NUM_CHAR_PER_LINE	(NUM_PIX_PER_LINE/CHAR_WIDTH)
#define	NUM_LINES		(NUM_PIX_LINES / CHAR_HEIGHT)


#define USEC	16

#define RST_LOW	(10 * USEC)	// 10 usec delay in cpu clocks) 	

//
// lcd signals
//
#ifdef LCD_18

#define LCD_SCK	(1 << 2)			// PD2
#define LCD_SDA	(1 << 3)			// PD3
#define LCD_A0	(1 << 4)			// PD4
#define LCD_nRST	(1 << 5)		// PD5
#define LCD_nCS		(1 << 6)		// PD6
#define LCD_DDR	(0x7C)				// PD2-PD6 outputs
#define LCD_DFLT	(LCD_nRST | LCD_nCS)  // nRST and nCS high
#define LCD_RST_LOW	2		// 

#elif LCD_TT

#define LCD_SCK	(1 << 2)			// PD2
#define LCD_SDA	(1 << 3)			// PD3
#define LCD_A0	(1 << 1)			// PD1
#define LCD_nRST	(1 << 0)		// PD0
#define LCD_nCS		(1 << 5)		// PD5
#define LCD_DDR	(0x2F)				// PD0-PD3,PD5 outputs
#define LCD_DFLT	(LCD_nRST | LCD_nCS)  // nRST and nCS high

#else

#define LCD_SCK	(1 << 5)			// PD5
#define LCD_SDA	(1 << 4)			// PD4
#define LCD_A0	(1 << 3)			// PD3
#define LCD_nRST	(1 << 2)		// PD2
#define LCD_nCS		(1 << 6)		// PD6
#define LCD_DDR	(0x7C)				// PD2-PD6 outputs
#define LCD_DFLT	(LCD_nRST | LCD_nCS)  // nRST and nCS high

#endif


//Rotary Encoder:

#define ENCODER_A        PD1       /* rotary encoder A signal */
#define ENCODER_B        PD3       /* rotary encoder B signal */


typedef struct _cursor {
	int line;		// y
	int col;		// x
} cursor_t;


typedef struct _pixpsn {
	uint16_t line;		// y
	uint16_t col;		// x
} pixpsn_t;




typedef struct _pixColor {
	uint8_t	r;
	uint8_t g;
	uint8_t b;
}pixColor;

//
// wrapper for fprintf
//
int lcd_putc(char c, FILE *f);


class Lcd {

private:
	static pixColor fgColor;
	static pixColor bgColor;
	static cursor_t cursor;
	static uint8_t lcdInitialized;

	static void sendCmd(uint8_t cmd);
	static void sendData(uint8_t *buf, uint8_t cnt);
	static void cmpltCmd(void);
	static void sendByte(uint8_t b);

// control commands
	void on(void);
	void off(void);
	void sleep_out(void);
	void mem_access(void);
	void reset(void);

public:
	Lcd(void);

// char display commands
	static void lcd_putchar(char p);
	void puts(char *p);
	void puts(char *p, pixColor c);
	void puts(char *p, pixColor fgc, pixColor bgc);
	void clrEol(void);
	void drawCursor(uint8_t erase);

// pixel commands
	void drawChar(char c, uint8_t mode, uint8_t line, uint8_t col);
	void drawChar(char cc, uint8_t mode, uint8_t line, uint8_t col, pixColor fc, pixColor bc);
	void clrScrn(void);
	void gotoxy(uint8_t line, uint8_t col);
	void setFgColor(uint8_t r, uint8_t g, uint8_t b);
	void setBgColor(uint8_t r, uint8_t g, uint8_t b);


	friend class Graphics;


};

//
// all functions implemented in assembler must be demangled
//
#if defined (__cplusplus)
extern "C" {
  #endif

void _getPixels(char c, char *buf);

#if defined (__cplusplus)
}
#endif





#endif /* LCD_H */

