
#include <avr/io.h>
#include <avr/interrupt.h>

#include "clock.h"
#include "led2.h"
#include "timer.h"

//
// constructor
//
Led2::Led2(void) {
  // set up led (PB5)
  DDRB |= LED_MSK;    // PB5 is output
  PORTB |= LED_MSK;    // turn on the output
	flag = 0;
	Timer::startTimer(this,500);
}


void Led2::callback(void) {
	volatile uint8_t reg = PORTB;

	reg ^= LED_MSK;
	PORTB = reg;
	Timer::startTimer(this, 500);
	flag++;
}

