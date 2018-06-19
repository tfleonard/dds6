/*
 * keyer.cpp
 *
 *  Created on: Jun 4, 2018
 *      Author: tleonard
 */
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include "clock.h"
#include "sw.h"
#include "Encoder.h"
#include "vfo.h"
#include "lcd.h"
#include "graphics.h"
#include "params.h"
#include "relay.h"
#include "keyer.h"
#include "timer.h"



keyer::keyer(Sw *dot_sw, Sw *dash_sw, relay *rlyd) {
	dot = dot_sw;
	dash = dash_sw;
	rly = rlyd;
	speed = 15;
	dot_time = (6000) / ((uint16_t)speed * 5);
	mode =  KEY_ST; 	
	state = 0;
}

bool keyer::isIdle(void) {
	return (state == 0);	
}


//
// called when a new key event is detected and the
// keyer is idle
//
void keyer::update(void) {
	
	if (isIdle()) {
//printf("update\n");
		callback();
	}	
}


uint8_t keyer::getState(void) {
	return state;
}

//
// called from interrupt by the timer
//
void keyer::callback(void) {
	switch (state) {
		case 0:
			if (dot->hasEvent()) {
				if (dot->getEvent() == EV_CLOSE) {
					rly->selectTx();
					state = 1;
					Timer::startTimer(this, dot_time);
				} else {
					rly->selectRx();				
				}
				dot->clearEvent();
				dash->clearEvent();

			} else if (dash->hasEvent()) {
				if (dash->getEvent() == EV_CLOSE) {
					rly->selectTx();
					state = 3;
					Timer::startTimer(this, dot_time*WEIGHT);
				} else {
					rly->selectRx();	
				}
				dash->clearEvent();					
			}
			break;

			case 1:
				state = 2;
				rly->selectRx();
				Timer::startTimer(this, dot_time);
				break;			

			case 2:
				if (!dot->isClosed() && !dash->isClosed()) {
					state = 0;
					dot->clearEvent();
					dash->clearEvent();
				
				} else if (dot->isClosed() && !dash->isClosed()) {
					state = 1;
					rly->selectTx();
					Timer::startTimer(this, dot_time);
					
				} else {
					state = 3;
					rly->selectTx();
					Timer::startTimer(this, dot_time*WEIGHT);
				} 
				break;
				
			case 3:
				state = 4;
				rly->selectRx();
				Timer::startTimer(this, dot_time);
				break;
			
			case 4:
				if (!dot->isClosed() && !dash->isClosed()) {
					state = 0;
					dot->clearEvent();
					dash->clearEvent();
				
				} else if (!dot->isClosed() && dash->isClosed()) {
					state = 3;
					rly->selectTx();
					Timer::startTimer(this, dot_time*WEIGHT);
					
				} else {
					state = 1;
					rly->selectTx();
					Timer::startTimer(this, dot_time);
				} 
				break;

			default:
				state = 0;
				rly->selectRx();
				dot->clearEvent();
				dash->clearEvent();			
				break;
	}
}

void keyer::setSpeed(uint8_t spd) {
	speed = spd;
	dot_time = (6000) / ((uint16_t)speed * 5);
}


	
void keyer::setMode(key_t md) {
	mode = md;
}


