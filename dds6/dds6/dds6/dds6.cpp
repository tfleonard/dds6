//
// Version		Date		Reason
//	1.0								Made from lcdtest
//	1.1				3/12/18	Change clock to allow classes to register for
//										periodic callbacks from the clock tick.
//
//	1.2			5/25/18		First working TT version.
//							Added call sign and elapsed time display.
//							This version is built with Atmel Studio
//	1.3			5/26/18		Remove Elapsed Time task and clock update - causes unstable
//							keying due to too much time spent in fprintf.
//							DO NOT USE PRINTF IN ANY CRITICAL PATH!
//
//	1.4			5/29/18		Change periodic and oneshot timers to take a time in msec.
//										Change callbacks to handle the event, not count down the time.
//										Change clock tick from 10 msec to 1 msec.  This is necessary
//										in order to implement keyer which needs a 1 msec oneshot resolution.
//
//	1.5			5/30/18		Change clock from timer 1 to timer 2 so that timer 1 can be used by
//							the keyer. Timer 2 uses a divide by 1024 perscaler and a count of 157 to
//							generate an approximate 10 msec interrupt.
//
//	1.6			6/18/18		Added keyer and relay classes.  Turn off update of RX/TX when using
//										keyer as the update slows down the keyer.  This version is basically complete.
//										It lacks the bug feature and saving current state to eeprom.


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
#include "timer.h"
#include "relay.h"
#include "keyer.h"


//Beginning of Auto generated function prototypes by Atmel Studio
void mymain(void );
//End of Auto generated function prototypes by Atmel Studio

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
Timer *tmr = new Timer();

#ifndef LCD_TT
Led *led = new Led();
#endif

Lcd *l = new Lcd();
Graphics *g = new Graphics();

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
//	ElapsedTime *et = new ElapsedTime(ONE_SEC);
	Encoder *enc = new Encoder(ENCA, ENCB, PORTB_ADR);

	band_t band = BAND_40;
	vfo *vfoA = new vfo(enc, vfoDataConst, VFOA_LINE);
	vfo *vfoB = new vfo(enc, vfoDataConst, VFOB_LINE);

	vfoA->getTxDisplayFreq(buf, band);
	for (size_t i = 0; i < strlen(buf); i++) {
		g->draw2xChar(buf[i], VFOA_LINE, i*2*CHAR_WIDTH, f,b);
	}

	vfoB->getTxDisplayFreq(buf, band);
	for (size_t i = 0; i < strlen(buf); i++) {
		g->draw2xChar(buf[i], VFOB_LINE, i*2*CHAR_WIDTH, fg_grey,b);
	}

	vfo *curVfo = vfoA;
	params *param = new params(g);
	relay *rly = new relay(param);
	keyer *kyr = new keyer(dot,dash,rly);
	rly->setVfo(curVfo);
	
	//
	// initialize a vfo and frequency
	//
	param->setActiveVfo(RX);

	freq = curVfo->getRxFreq(band);
	dds(freq);

	curMode = param->getMode();

	//
	// initialize keyer and relay
	//
	kyr->setSpeed(param->getSpeed());
	kyr->setMode(param->getKey());
	rly->setBand(band);
	rly->setVfo(curVfo);

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
					g->draw2xChar(buf[i], VFOA_LINE, i*2*CHAR_WIDTH, f,b);
				}

				vfoB->getTxDisplayFreq(buf, band);
				for (size_t i = 0; i < strlen(buf); i++) {
					g->draw2xChar(buf[i], VFOB_LINE, i*2*CHAR_WIDTH, fg_grey,b);
				}

			} else {
				curVfo = vfoB;
				vfoA->getTxDisplayFreq(buf, band);
				for (size_t i = 0; i < strlen(buf); i++) {
					g->draw2xChar(buf[i], VFOA_LINE, i*2*CHAR_WIDTH, fg_grey,b);
				}

				vfoB->getTxDisplayFreq(buf, band);
				for (size_t i = 0; i < strlen(buf); i++) {
					g->draw2xChar(buf[i], VFOB_LINE, i*2*CHAR_WIDTH, f,b);
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
			
			//
			// update the keyer
			//
			kyr->setSpeed(param->getSpeed());
			kyr->setMode(param->getKey()); 
			rly->setBand(band);
			rly->setVfo(curVfo);		
			
			curMode = newMode;

		} else if ((newMode == MODE_PARAMS) && (curMode == MODE_NORMAL)) {
				param->updateEnter(enc, pb, l);
				curMode = newMode;

		} else {

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
							g->draw2xChar(buf[i], line, i*2*CHAR_WIDTH, f,b);
						}
					}
					break;

				case MODE_SETUP:
					break;
			}
		}

#if 1

		if (param->getKey() == KEY_ST) {
			if (dot->hasEvent()) {
				if (dot->getEvent() == EV_CLOSE) {
					rly->selectTx(true);
				} else {
					rly->selectRx(true);
				}
				dot->clearEvent();
				dash->clearEvent();
			}
			
		} else /*if (param->getKey() == KEY_PDLS) */ {

//printf("key: 0x%x\n", param->getKey());

			if (kyr->isIdle()) {
				if ( dot->hasEvent() || dash->hasEvent() ) { 		
					kyr->update();
				}
			} else {
//				printf("keyer state: 0x%x\n", kyr->getState());
			}
		}

#else

		if (dot->hasEvent()) {
			if (dot->getEvent() == EV_CLOSE) {
				rly->selectTx();
			} else {
				rly->selectRx();
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
#endif

#if 0
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
#endif

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




