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

 
ElapsedTime::ElapsedTime(uint16_t nt) {
	event = 0;
	Clock::registerPeriodic(this, nt);
}


void ElapsedTime::callback(void) {
	event++;
}


bool ElapsedTime:: expired(void) {
	if (event) {
		event = 0;
		return true;
	}
	return false;
}

