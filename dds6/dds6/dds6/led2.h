#ifndef LED2_H
#define LED2_H

#define LED 5
#define LED_MSK (1 << LED)

#include "Callback.h"
#include "timer.h"

extern volatile uint8_t flag;

class Led2 : public Callback {

private:
//	Timer *tmr;

public:
  Led2(void);
  void toggle(void);
	void callback(void);
};


#endif // LED_H
