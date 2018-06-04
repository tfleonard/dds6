/* 
 * File:   dds.c
 * Author: root
 *
 * Created on April 9, 2009, 7:09 PM
 */
 
 #include <ctype.h>
 #include <stdint.h>
 #include <stdio.h>

 #include <avr/io.h>
 #include <avr/interrupt.h>

#include <math.h>
#include "clock.h"
#include "dds.h"


//
// DDS uses bits PB0 - PB2, PB5
//

void DdsInit(void) {

volatile uint8_t reg;

	reg = DDRB;
	reg |= DDS_OUT_BITS;		// set Led pin to output
	DDRB = reg;

	reg = PORTB;
	reg &= ~DDS_OUT_BITS;		
	PORTB = reg;

#ifdef LCD_TT	
	reg |= DDS_RST;
	PORTB = reg;
	Clock::delay(50);
	reg &= ~DDS_RST;
	PORTB = reg;
#endif

	reg |= W_CLK;
	PORTB = reg;

	reg &= ~ W_CLK;
	PORTB = reg;

	reg |= FQUD_CLK;     // make sure we are in serial mode
	PORTB = reg;
	
	reg &= ~FQUD_CLK;    
	PORTB = reg;
	
}

//
// with a 40 mhz clock and 32 bits of resolution the phase change 
// per dds clock is 107.3741824 clocks per hz
// 
// for a dds output frequency of 7.000000 mhz, this is a phase
// increment of 0x2ccccccc.  For 7.000001 mhz this is a phase
// increment of 0x2ccccd38.
// 
//
// with a 125 mhz clock and 32 bits of resolution the phase change 
// per dds clock is 34.359738368 clocks per hz
// 
// for a dds output frequency of 7.000000 mhz, this is a phase
// increment of 0x0E560418.  For 7.000001 mhz this is a phase
// increment of 0x0E56043B.
// 



void dds(float fout) {

float n;
uint32_t phaseInc;
int i;
volatile uint8_t temp;

	cli();
//
// the ports need to be protected with a semaphore if we
// are going to make this code multi-tasking
//
	temp = PORTB;
	temp &= ~DDS_OUT_BITS;
	PORTB = temp;

	n = fout * (float)CNTS_PER_HZ;		// phase change per clock
	phaseInc = (uint32_t)n;

	for (i=0;i<32;i++) {

		if (phaseInc & 0x01) {
			temp |= SD;
    } else {
			temp &= ~SD;
		}            
		PORTB = temp;								// data bit

		temp |= W_CLK;					// set clk
		PORTB = temp;
		temp &= ~W_CLK;					// clear clk
		PORTB = temp;	

		phaseInc = phaseInc >> 1;		// shift next data bit to msb
	}

	temp &= ~SD;
	PORTB = temp;								// data bit
	for (i=0;i<8;i++) {
		temp |= W_CLK;					// set clk
		PORTB = temp;
		temp &= ~W_CLK;					// clear clk
		PORTB = temp;

	}

	temp |= FQUD_CLK;            // Load the phase accumulator
	PORTB = temp;

	temp &= ~FQUD_CLK;            // transfer shift reg to phase accumulator
	PORTB = temp;

	sei();
}

