
#include <avr/io.h>
#include <avr/interrupt.h>

#include "clock.h"
#include "led.h"

//
// constructor
//
Led::Led(void) {
  // set up led (PB5)
  DDRB |= LED_MSK;    // PB5 is output
  PORTB |= LED_MSK;    // turn on the output
	Clock::registerPeriodic(this, FIVE_HUNDRED_MSEC);
}


void Led::callback(void) {
	toggle();
}


//
// toggle the led
void Led::toggle(void) {
volatile uint8_t reg = PORTB;

  reg ^= LED_MSK;
  PORTB = reg;
}

//
// return the state of the led
//
uint8_t Led::is_led_set(void) {
  return (PORTB & LED_MSK);
}

//
// turn on the led
//
void Led::on(void) {
  volatile uint8_t reg = PORTB;

  reg |= LED_MSK;
  PORTB = reg;
}

//
// turn off the led
//
void Led::off(void) {
  volatile uint8_t reg = PORTB;

  reg &= ~LED_MSK;
  PORTB = reg;
}


