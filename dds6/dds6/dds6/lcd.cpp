/*
 * Notes:
 * This code uses the _getPixels assembly code in font.s to retrieve
 * a buffer containing the pixels for a specified character.  The FONT array
 * is stored in code space since this processor has very little ram.  This requires
 * the assembly code function which takes a character and a buffer and returns the pixels.
 *
 * 
 */

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "clock.h"
#include "lcd.h"
#include "ST7735.h"


//
// static var instantiations
//
pixColor Lcd::fgColor;
pixColor Lcd::bgColor;
cursor_t Lcd::cursor;
uint8_t Lcd::lcdInitialized = 0;


Lcd::Lcd(void) {
	volatile uint8_t reg;

	if (lcdInitialized) {			// only init the lcd once
		return;
	}
	// set the LCD bits in PORTD to outputs
	reg = DDRD;
	reg |= LCD_DDR;
	DDRD = reg;

	// set the LCD bits to default
	reg = PORTD;
	reg &= ~LCD_DDR;		// turn off all lcd bits
	reg |= LCD_DFLT;
	PORTD = reg;
	fgColor = WHITE;
	bgColor = LTGREEN;
	cursor.col = 0;
	cursor.line = 0;
	reset();
	mem_access();
	Clock::delay(12);
  sleep_out();
  Clock::delay(12);
	on();	
 	clrScrn();
	lcdInitialized = 1;
}


void Lcd::reset(void) {

	volatile uint8_t reg;

	reg = PORTD;
	reg &= ~LCD_nRST;		// set reset low
	PORTD = reg;
	usecDly(RST_LOW);		// reset pulse is 10 usec
	reg |= LCD_nRST;
	PORTD = reg;			// set reset high
	Clock::delay(13);		// wait more than 120 msec
}


void Lcd::sendCmd(uint8_t cmd) {

volatile uint8_t reg;

	reg = PORTD;
	reg = reg & ~(LCD_nCS | LCD_A0);
	PORTD = reg;				// CS, A0 low
	
	// first send cmd
	sendByte(cmd);
}


void Lcd::sendData(uint8_t *buf, uint8_t cnt) {

volatile uint8_t reg = PORTD;
  
  PORTD = reg | LCD_A0;   // A0 hi for data/param

  for (int i=0; i< cnt; i++) {
    sendByte(buf[i]);
  }
}


//
// assumes CS and A0 are set correctly before entry
//
void Lcd::sendByte(uint8_t b) {

volatile uint8_t treg = PORTD;
volatile uint8_t reg = treg;

	for (int i = 0 ; i < 8; i++) {
		if ( b & 0x80) {
			reg |= LCD_SDA;
		} else {
			reg &= ~LCD_SDA;
		}
		PORTD = reg;
		PORTD = reg | LCD_SCK;
		PORTD = reg;
		b = (b << 1) & 0xFF;		   
	}
	PORTD = treg;			// return PORTD as we received it
}


void Lcd::on(void) {

  cli();
	sendCmd(CMD_DISPLAY_ON);
  cmpltCmd();
  sei();
}

	
void Lcd::off(void) {
  
  cli();
	sendCmd(CMD_DISPLAY_OFF);
  cmpltCmd();
  sei();
}


void Lcd::sleep_out(void) {

  cli();
  sendCmd(CMD_SLEEP_OUT);
  cmpltCmd();
  sei();
}

void Lcd::mem_access(void) {
	uint8_t buf = LCD_MEM_CTL;
	cli();

	//
	// flip xy if needed
	//
	sendCmd(CMD_MEM_CTRL);
	sendData(&buf, 1);
	cmpltCmd();

	//
	// set pixel format to 18 bits per pixel
	//
	sendCmd(CMD_PIX_FORMAT);
	buf = FLAG_IFPF_18;
	sendData(&buf, 1);
	cmpltCmd();
	sei();
}


void Lcd::setFgColor(uint8_t r, uint8_t g, uint8_t b) {
	
	fgColor.r = r;
	fgColor.g = g;
	fgColor.b = b;
}


void Lcd::setBgColor(uint8_t r, uint8_t g, uint8_t b) {
	
	bgColor.r = r;
	bgColor.g = g;
	bgColor.b = b;
}


void Lcd::lcd_putchar(char c) {

uint8_t ys = cursor.line * CHAR_HEIGHT;
uint8_t ye = ys + CHAR_HEIGHT - 1;
uint8_t xs = cursor.col * CHAR_WIDTH;
uint8_t xe = xs + CHAR_WIDTH -1;
char pixBuf[CHAR_HEIGHT];
uint8_t buf[CHAR_WIDTH * 3];
uint8_t bufIndx;
  
  cli();
	buf[0] = (xs >> 8) & 0xff;
	buf[1] = (xs & 0xff);
	buf[2] = (xe >> 8) & 0xff;
	buf[3] = (xe & 0xff);
	sendCmd(CMD_COL_ADDR_SET);
	sendData(buf, 4);
  cmpltCmd();
  
	buf[0] = (ys >> 8) & 0xff;
	buf[1] = (ys & 0xff);
	buf[2] = (ye >> 8) & 0xff;
	buf[3] = (ye & 0xff);
	sendCmd(CMD_ROW_ADDR_SET);
	sendData(buf, 4);
  cmpltCmd();

	_getPixels(c, pixBuf); 

  sendCmd(CMD_MEM_WRITE);
  
	for (int i=0; i< CHAR_HEIGHT; i++) {
    bufIndx = 0;
		uint8_t pixels = pixBuf[i];
		for (int j=0; j< CHAR_WIDTH; j++) {
			if (pixels & 0x01) {								// font table assumes right to left scan 
				buf[bufIndx++] = fgColor.r;
				buf[bufIndx++] = fgColor.g;
				buf[bufIndx++] = fgColor.b;
			} else {
				buf[bufIndx++] = bgColor.r;
				buf[bufIndx++] = bgColor.g;
				buf[bufIndx++] = bgColor.b;
			}
			pixels >>= 1;		
		}
    sendData(buf,bufIndx);
	}
  cmpltCmd();
  cursor.col++;
  if (cursor.col == NUM_CHAR_PER_LINE) {
    cursor.col = 0;
    cursor.line++;
    if (cursor.line == NUM_LINES) {
      cursor.line = 0;
    }  
  }
  sei();
}

void Lcd::cmpltCmd(void) {
uint8_t reg;
  
  reg = PORTD & ~LCD_DDR;
  PORTD = reg | LCD_DFLT;   // CS, RST hi
}


void Lcd::clrEol(void) {
	uint8_t col = cursor.col;
	while (col++ < NUM_CHAR_PER_LINE) {
		lcd_putchar(' ');
	}
}


/*
 Draw character c at pixel position (y, x) with foreground color fc,
 *  background color bc, 
 *  node = 1 interpret y,x as a text line, col; 
 *       = 0 interpret y,x as a pixel line, col
 */
void Lcd::drawChar(char cc, uint8_t mode, uint8_t line, uint8_t col, pixColor fc, pixColor bc) {

uint8_t ys; 
uint8_t ye; 
uint8_t xs; 
uint8_t xe; 
char pixBuf[CHAR_HEIGHT];
uint8_t buf[CHAR_WIDTH * 3];
uint8_t bufIndx;

  if (mode) { 
    ys = cursor.line * CHAR_HEIGHT;
    xs = cursor.col * CHAR_WIDTH;
  } else {
    ys = line;
    xs = col;
  }
  ye = ys + CHAR_HEIGHT - 1;
  xe = xs + CHAR_WIDTH -1; 
  cli();
  buf[0] = (xs >> 8) & 0xff;
  buf[1] = (xs & 0xff);
  buf[2] = (xe >> 8) & 0xff;
  buf[3] = (xe & 0xff);
  sendCmd(CMD_COL_ADDR_SET);
  sendData(buf, 4);
  cmpltCmd();
  
  buf[0] = (ys >> 8) & 0xff;
  buf[1] = (ys & 0xff);
  buf[2] = (ye >> 8) & 0xff;
  buf[3] = (ye & 0xff);
  sendCmd(CMD_ROW_ADDR_SET);
  sendData(buf, 4);
  cmpltCmd();

  _getPixels(cc, pixBuf); 

  sendCmd(CMD_MEM_WRITE);
  
  for (int i=0; i< CHAR_HEIGHT; i++) {
    bufIndx = 0;
    uint8_t pixels = pixBuf[i];
    for (int j=0; j< CHAR_WIDTH; j++) {
      if (pixels & 0x01) {                // font table assumes right to left scan 
        buf[bufIndx++] = fc.r;
        buf[bufIndx++] = fc.g;
        buf[bufIndx++] = fc.b;
      } else {
        buf[bufIndx++] = bc.r;
        buf[bufIndx++] = bc.g;
        buf[bufIndx++] = bc.b;
      }
      pixels >>= 1;   
    }
    sendData(buf,bufIndx);
  }
  cmpltCmd();
  if (mode) {
    cursor.col++;
    if (cursor.col == NUM_CHAR_PER_LINE) {
      cursor.col = 0;
      cursor.line++;
      if (cursor.line == NUM_LINES) {
        cursor.line = 0;
      }  
    }
  }
  sei();
}


/*
 Draw character c at pixel position (y, x) with default foreground and background colors
 */
void Lcd::drawChar(char cc, uint8_t mode, uint8_t line, uint8_t col ) {

	drawChar(cc, mode, line, col, fgColor, bgColor);
}


void Lcd::clrScrn(void) {

uint8_t pixBuf[3];
uint8_t buf[4];

  pixColor oldBg = bgColor;
  bgColor = LTGREEN;
  
  cli();
  
  buf[0] = 0;
  buf[1] = 0;
  buf[2] = (NUM_PIX_PER_LINE >> 8) & 0xff;
  buf[3] = (NUM_PIX_PER_LINE & 0xff);
  sendCmd(CMD_COL_ADDR_SET);
  sendData(buf, 4);
  cmpltCmd();
  
  buf[0] = 0;
  buf[1] = 0;
  buf[2] = (NUM_PIX_LINES >> 8) & 0xff;
  buf[3] = (NUM_PIX_LINES & 0xff);
  sendCmd(CMD_ROW_ADDR_SET);
  sendData(buf, 4);
  cmpltCmd();

  pixBuf[0] = bgColor.r;
  pixBuf[1] = bgColor.g;
  pixBuf[2] = bgColor.b; 

  sendCmd(CMD_MEM_WRITE);
  
  for (int i=0; i< NUM_PIX_LINES; i++) {
    for (int j=0; j< NUM_PIX_PER_LINE; j++) {
      sendData(pixBuf,3);
    }
  }
  sei();
	
	cursor.line = 0;
	cursor.col = 0;
  bgColor = oldBg;
}


void Lcd::puts(char *p) {
	while (*p) {
		drawChar(*p++, 1, cursor.line, cursor.col);
	}	
}


void Lcd::puts(char *p, pixColor c) {
	while (*p) {
		drawChar(*p++, 1, cursor.line, cursor.col, c, bgColor);
	}
}

void Lcd::puts(char *p, pixColor fgc, pixColor bgc) {
	while (*p) {
		drawChar(*p++, 1, cursor.line, cursor.col, fgc, bgc);
	}
}


void Lcd::gotoxy(uint8_t line, uint8_t col) {	
	cursor.line = line;
	cursor.col = col;
}


//
// erase = 0, draw inverted space as cursor
// erase = 1, draw normal space as cursor
//
void Lcd::drawCursor(uint8_t erase) {
  if (erase) {
    drawChar(' ', 1, cursor.line, cursor.col, fgColor, bgColor);    
  } else {
  	drawChar(' ', 1, cursor.line, cursor.col, bgColor, fgColor);
  }
}


//
// fprintf wrapper
//
int lcd_putc(char c, FILE *f) {
	Lcd::lcd_putchar(c);
	return 0;
}

