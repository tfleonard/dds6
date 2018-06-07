#ifndef CLOCK_H
#define CLOCK_H

//
// uncomment the following line for TT which has an 8 mhz clock
// the UNO has a 16 mhz clock
//#define TT_CLK	1


#include "Callback.h"

#define MSEC_TO_TICKS(x) (x/10)
#define TICKS_TO_MSEC(x) (x*10)


#if 0
#define TEN_MSEC	10
#define ONE_SEC		1000
#define TWO_SEC		2000
#define FIVE_HUNDRED_MSEC	500
#define SHORT_CLOSE	2000		// msec
#else
#define TEN_MSEC	1
#define ONE_SEC		100
#define TWO_SEC		200
#define FIVE_HUNDRED_MSEC	50
#define SHORT_CLOSE	200		// msec
#endif

#define NUM_PERIODIC_TASKS	8
#define NUM_SINGLE_TASKS	2

#define OCR2A_DIVISOR 157

void usecDly(int x);

class Clock {
private:
  static volatile uint32_t sysclock;
  static volatile uint32_t delayTime;
  static uint8_t count;
	static Callback *periodicTasks[NUM_PERIODIC_TASKS];
	static volatile uint16_t reloadTime[NUM_PERIODIC_TASKS];
	static volatile uint16_t periodicTimeLeft[NUM_PERIODIC_TASKS];
  static Callback *singleTasks[NUM_SINGLE_TASKS];
	static volatile uint16_t oneshotTimeLeft[NUM_SINGLE_TASKS];

public:
  Clock(void);
  ~Clock(void);
  static uint32_t getTicks(void);
	static void tick(void);
  static void delay(uint32_t dly);
	static int registerPeriodic(Callback *cb, uint16_t rt);
	static int registerOneshot(Callback *cb, uint16_t rt);
	static void cancelPeriodic(int indx);
	static void cancelOneshot(int indx); 
};


#endif // CLOCK_H
