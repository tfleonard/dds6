/*
 * relay.cpp
 *
 * Created: 6/17/2018 11:19:03 AM
 *  Author: tleonard
 */ 

 
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
#include "Encoder.h"
#include "vfo.h"
#include "params.h"
#include "relay.h"


relay::relay(params *prm) {
	param = prm;
}


void relay::setVfo(vfo *v) {
	curVfo = v;
}


void relay::setBand(band_t b) {
	band = b;
}


void relay::selectTx(bool update) {
	float freq;
	volatile uint8_t reg;

	freq = curVfo->getTxFreq(band);
	dds(freq);
	reg = PORTD;
	PORTD = reg | RLY_MSK;
	if (update) {
		param->setActiveVfo(TX);
	}
}


void relay::selectRx(bool update) {
	float freq;
	volatile uint8_t reg;

	freq = curVfo->getRxFreq(band);
	dds(freq);
	reg = PORTD;
	PORTD = reg & ~RLY_MSK;
	if (update) {
		param->setActiveVfo(RX);
	}
}
	
	
	
