#ifndef SW_H
#define SW_H

#include "Callback.h"
#include "Events.h"


#define ENCA	(PB3)								// encoder a switch
#define ENCB	(PB4)								// encoder b switch
#define PB		(PD7)								// Push button switch
#define DOT		(PC1)								// Dot Paddle
#define DASH	(PC3)								// Dash Paddle
#define RLY		(PD6)								// output, one to close key relay
#define RLY_MSK (1 << RLY)

//
// address definitions for PORTB,PORTC, PORTD
//
// access them as *((volatile uint8_t *)PORTx_ADR)
//
#define PORTB_ADR		0x25		//_SFR_IO8(0x05)
#define PORTC_ADR		0x28		//_SFR_IO8(0x08)
#define PORTD_ADR		0x2b		//_SFR_IO8(0x0b)


typedef enum _pbState {
	PB_OPEN,
	PB_CLOSE_DLY,
	PB_CLOSE,
	PB_OPEN_DLY
}pbState;


class Sw : public Callback {

private:
	uint8_t swNum;
	volatile uint8_t *port;		
	volatile uint8_t *pin;		// PORT - 2
	volatile uint8_t *ddr;		// PORT - 1
	pbState state;
	event lastEvent;
	void update(void);

public:

	Sw(uint8_t swNum, uint8_t port);
	bool	hasEvent(void);
	event getEvent(void);
	void	clearEvent(void);
	void	callback(void);
	bool	isClosed(void);
};



#endif