//
// Version		Date		Reason
//	1.0								Made from lcdtest
//	1.1				3/12/18	Change clock to allow classes to register for
//										periodic callbacks from the clock tick.
//
//	1.2			5/25/18		First working TT version.
//							Added call sign and elapsed time display.
//							This version is built with Atmel Studio


/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>

/*End of auto generated code by Atmel studio */


 
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include "clock.h"
#include "uart.h"
#include "led.h"
#include "lcd.h"
#include "graphics.h"
#include "dds.h"
#include "sw.h"
#include "ElapsedTime.h"
#include "Encoder.h"
#include "vfo.h"
#include "params.h"

 using namespace std;

FILE uart_str = FDEV_SETUP_STREAM_CPP(uart_putc, uart_getc, _FDEV_SETUP_RW);
FILE lcd_str  = FDEV_SETUP_STREAM_CPP(lcd_putc, NULL, _FDEV_SETUP_WRITE);
FILE *lcdfp = &lcd_str;






// my main, called from loop
void mymain(void) {

volatile uint8_t reg;
uint8_t secs = 0;
uint8_t mins = 0;
uint8_t hours = 0;
Clock *cl = new Clock();

#ifndef LCD_TT
Led *led = new Led();
#endif

Lcd *l = new Lcd();
Graphics *g = new Graphics();

int pass = 0;
pixColor  f = GREEN;
pixColor pix_green = GREEN;
pixColor  b = LTGREEN;
pixColor  fg_grey = GRAY;
pixColor pix_blk = BLACK;

volatile float freq;
char buf[16];
mode_t curMode;

#ifndef LCD_TT
  uart_init();
  stdout = stdin = &uart_str;
  stderr = &uart_str;
  printf("main: Starting loop\n");
#endif

	DdsInit();
	// init RLY as output, 
	reg = DDRD;
	DDRD = reg | RLY_MSK;

	reg = PORTD;
	PORTD = reg & ~RLY_MSK;
	 
	Sw *dot = new Sw(DOT, PORTC_ADR);
	Sw *dash = new Sw(DASH, PORTC_ADR);
	Sw *pb = new Sw(PB, PORTD_ADR);
	ElapsedTime *et = new ElapsedTime(100);
	Encoder *enc = new Encoder(ENCA, ENCB, PORTB_ADR);

	band_t band = BAND_40;
	vfo *vfoA = new vfo(enc, vfoDataConst, VFOA_LINE);
	vfo *vfoB = new vfo(enc, vfoDataConst, VFOB_LINE);

	vfoA->getTxDisplayFreq(buf, band);
	for (size_t i = 0; i < strlen(buf); i++) {
		g->draw2xChar(buf[i], VFOA_LINE, i*2*CHAR_WIDTH, f, b);
	}

	vfoB->getTxDisplayFreq(buf, band);
	for (size_t i = 0; i < strlen(buf); i++) {
		g->draw2xChar(buf[i], VFOB_LINE, i*2*CHAR_WIDTH, fg_grey, b);
	}

	vfo *curVfo = vfoA;
	params *param = new params(g);

	//
	// initialize a vfo and frequency
	//
	param->setActiveVfo(RX);

	freq = curVfo->getRxFreq(band);
	dds(freq);

	curMode = param->getMode();
	g->gotoxy(9,0);
	fprintf(lcdfp, "AA6DQ");

	while(1) {

		mode_t newMode = param->getMode();

		if ( (curMode == MODE_PARAMS) && (newMode == MODE_NORMAL)) {

			band = param->getBand();

			if (param->getVfo() == VFOA) {
				curVfo = vfoA;
				vfoA->getTxDisplayFreq(buf, band);
				for (size_t i = 0; i < strlen(buf); i++) {
					g->draw2xChar(buf[i], VFOA_LINE, i*2*CHAR_WIDTH, f, b);
				}

				vfoB->getTxDisplayFreq(buf, band);
				for (size_t i = 0; i < strlen(buf); i++) {
					g->draw2xChar(buf[i], VFOB_LINE, i*2*CHAR_WIDTH, fg_grey, b);
				}

			} else {
				curVfo = vfoB;
				vfoA->getTxDisplayFreq(buf, band);
				for (size_t i = 0; i < strlen(buf); i++) {
					g->draw2xChar(buf[i], VFOA_LINE, i*2*CHAR_WIDTH, fg_grey, b);
				}

				vfoB->getTxDisplayFreq(buf, band);
				for (size_t i = 0; i < strlen(buf); i++) {
					g->draw2xChar(buf[i], VFOB_LINE, i*2*CHAR_WIDTH, f, b);
				}

			}
			curVfo->setStep(param->getStep());
			if (param->getActiveVfo() == RX) {
				freq = curVfo->getRxFreq(band);
				dds(freq);
			} else {
				freq - curVfo->getTxFreq(band);
				dds(freq);
			}
			curMode = newMode;

		} else if ((newMode == MODE_PARAMS) && (curMode == MODE_NORMAL)) {
				param->updateEnter(enc, pb, l);
				curMode = newMode;

		}	else {

 		curMode = newMode;
		
  	switch (curMode) {

			case MODE_PARAMS:
					param->update(enc, pb, l);
				break;

			//
			// for now, no keyer, just straight key
			//

			case MODE_NORMAL:
				if (pb->hasEvent()) {
					if (pb->getEvent() == EV_CLOSE) {

					} else if (pb->getEvent() == EV_OPEN) {
						param->setMode(MODE_PARAMS);
					}
					pb->clearEvent();
					break;
				}
		
				if (enc->hasEvent()) {
					uint16_t line;
					curVfo->update(band);
					enc->clearEvent();

freq = curVfo->getTxFreq(band);
dds(freq);

					curVfo->getTxDisplayFreq(buf, band);
					line = curVfo->getLine();
					for (size_t i = 0; i < strlen(buf); i++) {
						g->draw2xChar(buf[i], line, i*2*CHAR_WIDTH, f, b);
					}
				}
				break;

			case MODE_SETUP:
				break;
			}
		}

		if (dot->hasEvent()) {
			if (dot->getEvent() == EV_CLOSE) {
				freq = curVfo->getTxFreq(band);
				dds(freq);
				reg = PORTD;
				PORTD = reg | RLY_MSK;
				param->setActiveVfo(TX);
			} else {
				freq = curVfo->getRxFreq(band);
				dds(freq);
				reg = PORTD;
				PORTD = reg & ~RLY_MSK;
				param->setActiveVfo(RX);
			}
			dot->clearEvent();
		}

		if (dash->hasEvent()) {
			if (dash->getEvent() == EV_CLOSE) {
				sprintf(buf,"DASH_CLOSE");
				} else {
				sprintf(buf,"DASH_OPEN ");
			}
			dash->clearEvent();
			l->gotoxy(9,0);
			l->puts(buf);
		}

		if (et->expired()) {
			secs++;
			if (secs > 59) {
				secs = 0;
				mins++;
				if (mins > 59) {
					mins = 0;
					hours++;
				}
			}
	
			g->gotoxy(9,12);
			fprintf(lcdfp, "%02d:%02d:%02d", hours,mins, secs);
		}
	}

#if 0
	delete l;
	delete cl;
	delete dot;
	delete dash;
	delete pb;
	delete et;
	delete led;
#endif
}


void setup() {

}


void loop() {
  mymain();
}





