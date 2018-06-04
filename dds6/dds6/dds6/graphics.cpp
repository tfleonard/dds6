/*
 * graphics.cpp
 *
 * Created: 2/28/2018 9:10:02 PM
 *  Author: tleonard
 */ 
 
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "lcd.h"
#include "ST7735.h"
#include "graphics.h"


 
/*
 Draw character c at pixel position (y, x) with foreground color fc,
 *  background color bc, 
 *  node = 1 interpret y,x as a text line, col; 
 *       = 0 interpret y,x as a pixel line, col
 */
void Graphics::draw2xChar(char cc, uint8_t line, uint8_t col, pixColor fc, pixColor bc) {

uint8_t ys; 
uint8_t ye; 
uint8_t xs; 
uint8_t xe; 
char pixBuf[CHAR_HEIGHT];
uint8_t buf[CHAR_WIDTH * 2 * 3];
uint8_t bufIndx;

  ys = line;
  xs = col;
  ye = ys + (2*CHAR_HEIGHT) - 1;
  xe = xs + (2*CHAR_WIDTH) -1; 
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
  
  for (int i=0; i< CHAR_HEIGHT*2; i++) {
    bufIndx = 0;
    uint8_t pixels = pixBuf[i/2];
    for (int j=0; j< CHAR_WIDTH; j++) {
      if (pixels & 0x01) {                // font table assumes right to left scan 
        buf[bufIndx++] = fc.r;
        buf[bufIndx++] = fc.g;
        buf[bufIndx++] = fc.b;
        buf[bufIndx++] = fc.r;
        buf[bufIndx++] = fc.g;
        buf[bufIndx++] = fc.b;

      } else {
        buf[bufIndx++] = bc.r;
        buf[bufIndx++] = bc.g;
        buf[bufIndx++] = bc.b;
        buf[bufIndx++] = bc.r;
        buf[bufIndx++] = bc.g;
        buf[bufIndx++] = bc.b;
      }
      pixels >>= 1;   
    }
    sendData(buf,bufIndx);
  }
  cmpltCmd();
  sei();
}


/*
 Draw character c at pixel position (y, x) with default foreground and background colors
 */
void Graphics::draw2xChar(char cc, uint8_t line, uint8_t col ) {

	draw2xChar(cc, line, col, fgColor, bgColor);
}


void Graphics::	puts2x(char *p, uint8_t line, uint8_t col) {

	while (*p) {
		draw2xChar(*p++, line, col);
		col += (2 * CHAR_WIDTH);
		if (col >= NUM_PIX_PER_LINE) {
			col = 0;
			line += (2 * CHAR_HEIGHT);
			if (line >= NUM_PIX_LINES) {
				return;
			}
		}
	}
}


void Graphics::rect(pixpsn_t start, uint16_t xlen, uint16_t ylen, pixColor color) {

uint16_t ys; 
uint16_t ye; 
uint16_t xs; 
uint16_t xe; 
uint8_t buf[CHAR_WIDTH * 3];
uint8_t bufIndx;
uint16_t len;

  ys = start.line;
  xs = start.col;
  ye = ys + ylen - 1;
  xe = xs + xlen -1;
	 
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

  sendCmd(CMD_MEM_WRITE);

	bufIndx = 0;
//	initialize the line buffer
	for (int i=0; i< CHAR_WIDTH; i++) {
		buf[bufIndx++] = color.r;
    buf[bufIndx++] = color.g;
    buf[bufIndx++] = color.b;
	} 
 
  for (int i=0; i< ylen; i++) {
		len = xlen * 3;
		while(len) {
			if (len > bufIndx) {
				sendData(buf,bufIndx);
				len -= bufIndx;
			} else {
				sendData(buf,len);
				break;			
			}
  }
  }
  cmpltCmd();
  sei();
}






