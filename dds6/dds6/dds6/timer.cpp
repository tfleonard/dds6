/*
 * timer.cpp
 *
 * Created: 6/13/2018 3:16:26 PM
 *  Author: tleonard
 */ 
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include "Callback.h"
#include "timer.h"

volatile uint8_t Timer::initialized = 0;
Callback *Timer::cb;

//
// timer isr
//
ISR( TIMER1_COMPA_vect ) {
	Timer::expired();
}



Timer::Timer(void) {
	if (!initialized) {
		TCCR1A  = 0;                          // wgm10 = 0, no outputs
//		TCCR1B = (1 << CS11) | (1 << WGM12);   // clock divide by 8, WGM30 = 4, CTC mode
		TCCR1B = (1 << WGM12);   // clock off, WGM30 = 4, CTC mode
		TCCR1C = 0;
		TCNT1H = 0;
		TCNT1L = 0;
//		OCR1A =  OCR1A_DIVISOR;               // divide by 2000, 1 msec, do this last!!
		TIMSK1 = (1 << OCIE1A);               // enable compare register 1 int


		initialized = 1;
	}
}


//
// time in msec
//
void Timer::startTimer(Callback *cbb, uint16_t time) {

uint32_t ticks = (((uint32_t)time) * T1_USEC_PER_MSEC) / T1_USEC_PER_TICK;
uint16_t ticks16 = (uint16_t)(ticks & 0xFFFF);

	cb = cbb;

	TCCR1B = T1_CS_1024 | (1 << WGM12);
	TCNT1H = 0;
	TCNT1L = 0;
	OCR1A = ticks16;
	TIMSK1 = (1 << OCIE1A);               // enable compare register 1 int
}


void Timer::cancelTimer(void) {

//
// disable timer by setting clock source to 0
//
	TCCR1B = (1 << WGM12);   // clock off, WGM30 = 4, CTC mode
	TIMSK1 = 0;
}



void Timer::expired(void) {

	// disable timer
	TCCR1B = (1 << WGM12);   // clock off, WGM30 = 4, CTC mode
	TIMSK1 = 0;

	// callback	
	if (cb) {
		cb->callback();
	} 
}



