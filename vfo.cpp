//
// Copyright (c) 2018 Tim Leonard
//



#include <ctype.h>
#include <stdint.h>
#include <stdio.h>

#include <avr/io.h>
#include <avr/interrupt.h>

#include "sw.h"

//#include "Callback.h"
//#include "Events.h"
#include "Encoder.h"
#include "vfo.h"


vfoParameters vfoDataConst[END_OF_BAND] = {
{1800000,-100000,1,1800000,0,1},
{3500000,-100000,1,3500000,0,1},
{7000000,-100000,1,7000000,0,1},
{10000000,-100000,1,10000000,0,1},
{14000000,-200000,2,14000000,0,2},
{18068000,-200000,2,18068000,0,2},
{21000000,-300000,3,21000000,0,3},
{24890000,-300000,3,24890000,0,3},
{28000000,-400000,4,28000000,0,4}
};


uint32_t const step_values[6] = {
	10,
	100,
	1000,
	10000,
	100000,
	1000000
};



vfo::vfo(Encoder *enc, vfoParameters *vf, uint16_t l) {

	for (int i=0; i< END_OF_BAND; i++) {
		vfoData[i].rxFreq = vf[i].rxFreq;
		vfoData[i].rxOffset = vf[i].rxOffset;
		vfoData[i].rxMult = vf[i].rxMult;
		vfoData[i].txFreq = vf[i].txFreq;
		vfoData[i].txOffset = vf[i].txOffset;
		vfoData[i].txMult = vf[i].txMult;
	}
	this->enc = enc;
	fstep = 100;
	step = STEP_100HZ;
	line = l;
}


//
// Process events sent to the vfo page
//

void vfo::update(uint8_t band) {

	if (enc->hasEvent()) {
	
		encEvent event = enc->getEvent();
		uint32_t cnts;

		if (event == ENC_CCW_EVENT) {

			cnts = fstep * (1 + enc->getMissedCcwCount());
			vfoData[band].rxFreq -= cnts;
			vfoData[band].txFreq -= cnts;

		} else if (event == ENC_CW_EVENT) {
			cnts = fstep * (1 + enc->getMissedCwCount());
			vfoData[band].rxFreq += cnts;
			vfoData[band].txFreq += cnts;
		}
	}
}


float vfo::getRxFreq(band_t band) {

uint32_t vfo_freq;
uint32_t vfo_offset;
uint8_t	vfo_mult;

	vfo_freq = vfoData[band].rxFreq;
	vfo_offset = vfoData[band].rxOffset;
	vfo_mult = vfoData[band].rxMult;
	vfo_freq += vfo_offset;
	vfo_freq = vfo_freq / vfo_mult;
	return (float)vfo_freq;
}


float vfo::getTxFreq(band_t band) {

	uint32_t vfo_freq;
	uint32_t vfo_offset;
	uint8_t	vfo_mult;

	vfo_freq = vfoData[band].txFreq;
	vfo_offset = vfoData[band].txOffset;
	vfo_mult = vfoData[band].txMult;
	vfo_freq += vfo_offset;
	vfo_freq = vfo_freq / vfo_mult;
	return (float)vfo_freq;
}


void vfo::setStep(step_t step) {
	this->step = step;
	fstep = step_values[step];
}


step_t vfo::getStep(void) {
	return step;
}



void vfo::intToString32(uint32_t c, char *buf) {

	uint32_t digit;

	digit = c / 100000000;
	*buf++ = digit + 0x30;
	c = c - (digit*100000000);

	digit = c / 10000000;
	*buf++ = digit + 0x30;
	c = c - (digit*10000000);

	digit = c / 1000000;
	*buf++ = digit + 0x30;
	c = c - (digit*1000000);

	digit = c / 100000;
	*buf++ = digit + 0x30;
	c = c - (digit*100000);

	digit = c / 10000;
	*buf++ = digit + 0x30;
	c = c - (digit*10000);

	digit = c / 1000;
	*buf++ = digit + 0x30;
	c = c - (digit*1000);

	digit = c / 100;
	*buf++ = digit + 0x30;
	c = c - (digit*100);

	digit = c / 10;
	*buf++ = digit + 0x30;
	digit = c - (digit*10);
	*buf++ = digit + 0x30;
	*buf = '\0';
}


void vfo::getRxDisplayFreq(char *buf, band_t band) {

uint32_t freq = vfoData[band].rxFreq;
char tbuf[16];

	intToString32(freq,tbuf);
	formatDisplayFreq(tbuf,buf);
}


void vfo::formatDisplayFreq(char *ibuf, char *obuf) {

	char *dstptr = obuf;
	int found = 0;

	//
	// strip leading zeros and insert decimal point
	//
	for (int i=0; i< 9; i++) {
		
		if (found) {
			if ( (i == 3) | (i == 6)) {
				*dstptr++ = '.';	
			}
			*dstptr++ = ibuf[i];
		
		} else {
			if ( ibuf[i] == '0') {
				continue;
			} else {
				found = 1;
				*dstptr++ = ibuf[i];
			}
		}
	}
	*dstptr = '\0';
}



void vfo::getTxDisplayFreq(char *buf, band_t band) {

uint32_t freq = vfoData[band].txFreq;
char tbuf[16];

	intToString32(freq,tbuf);
	formatDisplayFreq(tbuf,buf);
}


uint16_t vfo::getLine(void) {
	return line;
}

void vfo::setActVfo(vfo_actv_t rxtx) {
	activeVfo = rxtx;
}