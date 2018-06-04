

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include "clock.h"
#include "sw.h"


//
// initialize switches
//
Sw::Sw(uint8_t s, volatile uint8_t p) {
	uint8_t temp;
	uint8_t msk = (1 << s);

	port = (volatile uint8_t *)p;
	pin = (volatile uint8_t *)(p-2);
	ddr = (volatile uint8_t *)(p-1);
	swNum = s;

	// set bit as input
	temp = *ddr;
	temp &= ~msk;
	*ddr = temp;

	// enable input pullup
	temp = *port;
	temp |= msk;
	*port = temp;

	// get the current state of the switch
	temp = *pin & msk;
	if (temp) {
		state = PB_OPEN;
	}	else {
		state = PB_CLOSE;
	}
	lastEvent = EV_NONE;
	Clock::registerPeriodic(this);
}


void Sw::update(void) {

uint8_t temp = *pin & (1 << swNum);

	switch (state) {
	
		case PB_OPEN:
			if (!temp) {
				state = PB_CLOSE_DLY;
			}
			break;

		case PB_CLOSE_DLY:
			if (!temp) {
				state = PB_CLOSE;
				lastEvent = EV_CLOSE;
			}
			break;	
	
		case PB_CLOSE:
			if (temp) {
				state = PB_OPEN_DLY;
			}	
			break;

		case PB_OPEN_DLY:
			if (temp) {
				state = PB_OPEN;
				lastEvent = EV_OPEN;
			}
			break;
	}				
	
}


bool	Sw::hasEvent(void) {
	if (lastEvent != EV_NONE) {
		return true;
	}
	return false;
}


event Sw::getEvent(void) {
	return lastEvent;
}

void Sw::clearEvent(void) {
	lastEvent = EV_NONE;
}


void Sw::callback(void) {
	update();
}


bool Sw::isClosed(void) {

	return ((state == PB_CLOSE) || (state == PB_OPEN_DLY));
}