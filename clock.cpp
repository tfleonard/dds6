 
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include "Callback.h"
#include "clock.h"

// declare the static var 
volatile uint32_t Clock::sysclock = 0;
volatile uint32_t Clock::delayTime = 0;
uint8_t Clock::count = 0;
Callback *Clock::periodicTasks[NUM_PERIODIC_TASKS];
Callback *Clock::singleTasks[NUM_SINGLE_TASKS];

//
// tick isr
//
ISR( TIMER1_COMPA_vect ) {
  Clock::tick();
}


//
// set the tic event count
//
void Clock::tick(void) {
  sysclock++;;
  if (delayTime) {
    delayTime--;
  }

	for (int i=0; i < NUM_PERIODIC_TASKS; i++) {
		if (periodicTasks[i]) {
				periodicTasks[i]->callback();
		}
	}

	for (int i=0; i < NUM_SINGLE_TASKS; i++) {
		if (singleTasks[i]) {
			singleTasks[i]->callback();
			singleTasks[i] = NULL;
		}
	}

}


// delay time in 
void Clock::delay(uint32_t dly) {
  Clock::delayTime = dly;
  while (Clock::delayTime);
}

//
// return number of ticks that occurred
//
uint32_t Clock::getTicks(void) {
  return Clock::sysclock;
}


//
// returns 0 on failure, handle on success
//
int Clock::registerPeriodic(Callback *cb) {

	for (int i=0; i < NUM_PERIODIC_TASKS; i++) {
		if (!periodicTasks[i]) {
			periodicTasks[i] = cb;
			return (i+1);
		}
	}
	return 0;
}

//
// returns 0 on failure, handle on success
//
int Clock::registerOneshot(Callback *cb) {

	for (int i=0; i < NUM_SINGLE_TASKS; i++) {
		if (!singleTasks[i]) {
			singleTasks[i] = cb;
			return (i+1);
		}
	}
	return 0;
}


//
// input - handle 
//
void Clock::cancelPeriodic(int indx) {
	if (!indx) {
		return;
	}
	if (indx <= NUM_PERIODIC_TASKS) {
		periodicTasks[indx-1] = NULL;
	}
}


void Clock::cancelOneshot(int indx) {
	if (!indx) {
		return;
	}
	if (indx <= NUM_SINGLE_TASKS) {
		singleTasks[indx-1] = NULL;
	}
}






//
// constructor
//
Clock::Clock(void) {
  cli();
  TCCR1A  = 0;                          // wgm10 = 0, no outputs
  TCCR1B = (1 << CS11) | (1 << WGM12);   // clock divide by 8, WGM30 = 4, CTC mode
  TCCR1C = 0;
  TCNT1H = 0;
  TCNT1L = 0;
  OCR1A =  OCR1A_DIVISOR;               // divide by 20000, 10 msec, do this last!!
  TIMSK1 = (1 << OCIE1A);               // enable compare register 1 int
  sysclock = 0;
  count = 0;

	for (int i=0; i< NUM_PERIODIC_TASKS; i++) {
		periodicTasks[i] = NULL;
	}

	for (int i=0; i< NUM_PERIODIC_TASKS; i++) {
		singleTasks[i] = NULL;
	}

  sei();
}


Clock::~Clock(void) {
}


void usecDly(int x) {
	while (x--);
}


