/*
 * Encoder.cpp
 *
 * Created: 3/28/2018 11:35:24 AM
 *  Author: tleonard
 */ 


#include <ctype.h>
#include <stdint.h>
#include <stdio.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include "Events.h"
#include "clock.h"
#include "sw.h"
#include "Encoder.h"


Encoder::Encoder(uint8_t a, uint8_t b, uint8_t port) {

	eventRate = 4;				// default 4 pulses per cycle
	encA = new Sw(a, port);
	encB = new Sw(b, port);
		
	if (encA->isClosed()) {
		if (encB->isClosed()) {
			curState = 2;
			} else {
			curState = 1;
		}
		} else {
		if (encB->isClosed()) {
			curState = 4;
			} else {
			curState = 0;
		}
	}
	lastEvent = ENC_NO_EVENT;
	Clock::registerPeriodic(this);
	missedCwCount = 0;
	missedCcwCount = 0;
}


//
// this encoder looks for a rising or falling edge on each
// of the encoder a and b inputs.  It samples the alternate encoder 
// input to determine direction.
// For instance, if an encoder a rising is detected and encoder b
// is low, then this is a cw event.  On the other hand if this is an
// encoder a rising event and encoder b is detected hi, then this is a
// ccw event. 
//
// This algorithm produces 4 events per encoder a/b cycle.  For
// a 2 event case, use only encoder a events/
//
void Encoder::update(void) {

	encEvent nxtEvent = ENC_NO_EVENT;

	if ( !encA->hasEvent() && !encB->hasEvent() ){
		return;
	}

	if (encA->getEvent() == EV_CLOSE) {
		if (encB->isClosed()) {
			nxtEvent = ENC_CCW_EVENT;
		} else {
			nxtEvent = ENC_CW_EVENT;
		}
		 
	} else if ( (encB->getEvent() == EV_CLOSE) && (eventRate == 4))  {
		if (encA->isClosed()) {
			nxtEvent = ENC_CW_EVENT;
		} else {
			nxtEvent = ENC_CCW_EVENT;
		}

	} else if ( (encA->getEvent() == EV_OPEN) && (eventRate != 1)) {
		if (encB->isClosed()) {
			nxtEvent = ENC_CW_EVENT;
		} else {
			nxtEvent = ENC_CCW_EVENT;
		}

	} else if ( (encB->getEvent() == EV_OPEN) && (eventRate == 4) ) {
		if (encA->isClosed()) {
			nxtEvent = ENC_CCW_EVENT;
		} else {
			nxtEvent = ENC_CW_EVENT;
		}

	}

	encA->clearEvent();
	encB->clearEvent();

	//
	// process new event if any
	//
	if (nxtEvent == ENC_NO_EVENT) {
		return;
	} else if (nxtEvent == ENC_CW_EVENT) {		// handle overrun
		if (nxtEvent == lastEvent) {
			missedCwCount++;
		} else {
			missedCwCount = 0;
			lastEvent = nxtEvent;
		}
	} else {
		if (nxtEvent == lastEvent) {
			missedCcwCount++;
		} else {
			missedCcwCount = 0;
			lastEvent = nxtEvent;
		}
	}
}



bool Encoder::hasEvent(void) {
	return (lastEvent != ENC_NO_EVENT);
}


encEvent Encoder::getEvent(void) {
	return lastEvent;
}


void Encoder::clearEvent(void) {
	lastEvent = ENC_NO_EVENT;
}


void Encoder::callback(void) {
	update();
}


int Encoder::getMissedCwCount(void) {
	int result = missedCwCount;
	missedCwCount = 0;
	return result;
}


int Encoder::getMissedCcwCount(void) {
	int result = missedCcwCount;
	missedCcwCount = 0;
	return result;
}

void Encoder::setEventRate(uint8_t r) {
	eventRate = r;
}

