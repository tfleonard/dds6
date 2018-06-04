#ifndef CLOCK_H
#define CLOCK_H

#include "led.h"


#define MSEC_TO_TICKS(x) (x*10)

class Clock {
private:
  static volatile uint32_t sysclock;
  static volatile uint32_t delayTime;
  static uint8_t count;
  static Led *led;
  
public:
  Clock(void);
  ~Clock(void);
  static uint32_t getTicks(void);
  static void setTick(void);
  static void delay(uint32_t dly);
};


#endif // CLOCK_H
