/*
 * ElapsedTime.cpp
 *
 * Created: 3/28/2018 2:35:41 PM
 *  Author: tleonard
 */ 

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "clock.h"
#include "ElapsedTime.h"
 
ElapsedTime::ElapsedTime(uint8_t nt) {
	event = 0;
	numTicks = nt;
	ticks = 0;
	Clock::registerPeriodic(this);
}


void ElapsedTime::callback(void) {
	ticks++;
	if (ticks >= numTicks) {
		ticks = 0;
		event++;
	}
}


bool ElapsedTime:: expired(void) {
	if (event) {
		event = 0;
		return true;
	}
	return false;
}

