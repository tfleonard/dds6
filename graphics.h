/*
 * graphics.h
 *
 * Created: 2/28/2018 9:07:11 PM
 *  Author: tleonard
 */ 


#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include "lcd.h"

class Graphics : public Lcd {

public:
	void draw2xChar(char c, uint8_t line, uint8_t col);
	void draw2xChar(char cc, uint8_t line, uint8_t col, pixColor fc, pixColor bc);
	void puts2x(char *p, uint8_t line, uint8_t col);
	void rect(pixpsn_t start, uint16_t xlen, uint16_t ylen, pixColor color);
};




#endif /* GRAPHICS_H_ */