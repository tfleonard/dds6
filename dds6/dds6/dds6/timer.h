/*
 * timer.h
 *
 * Created: 6/13/2018 3:16:51 PM
 *  Author: tleonard
 */ 


#ifndef TIMER_H_
#define TIMER_H_

#define OCR1A_DIVISOR 157
#define T1_USEC_PER_TICK 64		// 64 usec/tick
#define T1_USEC_PER_MSEC	1000		// 1000 usec per msec
#define T1_CS_1024	0x05


class Timer {

private:
	static Callback *cb;
	static volatile uint8_t initialized;


public:
	Timer(void);
	static void startTimer(Callback *cb, uint16_t time);
	static void cancelTimer(void);
	static void expired(void);

};




#endif /* TIMER_H_ */