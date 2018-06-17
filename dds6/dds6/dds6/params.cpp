/*
 * params.cpp
 *
 * Created: 4/16/2018 12:11:06 PM
 *  Author: tleonard
 */ 
 
 
 #include <ctype.h>
 #include <stdint.h>
 #include <stdio.h>

 #include <avr/io.h>
 #include <avr/interrupt.h>

 #include "clock.h"
 #include "uart.h"
 #include "led.h"
 #include "lcd.h"
 #include "graphics.h"
 #include "dds.h"
 #include "sw.h"
 #include "ElapsedTime.h"
 #include "Encoder.h"
 #include "vfo.h"
 #include "params.h"


param_entry_t default_param_display[PARAM_END] = {
{PARAM_STEP,{6,0},"STEP ",{6,5}},
{PARAM_BAND,{6,12},"BAND ",{6,17}},
{PARAM_VFO, {7,0},"VFO ",{7,4}},
{PARAM_KEY,{7,7},"KY ",{7,10}},
{PARAM_KEY_SPEED,{7,14},"SPD ",{7,18}},
{PARAM_ACT_VFO,{8,0},"MODE ",{8,5}}
};


char *band_strings[END_OF_BAND]  {
	"160",
	"80 ",
	"40 ",
	"30 ",
	"20 ",
	"17 ",
	"15 ",
	"12 ",
	"10 "
};


char *step_strings[END_OF_STEP] {
	"10 ",
	"100",
	"1K ",
	"10K",
	"1M "
};


char *vfo_strings[END_OF_VFO] {
	"A  ",
	"B  ",
	"SP "
};


char *vfoact_strings[END_OF_VFO_ACT] {
	"RX ",
	"TX "
};

char *key_strings[END_OF_KEY] {
	"ST",
	"BG",
	"PD"
};



void * memcpy(void *dest, const void* src, size_t cnt) {
	char *pdst = (char *) dest;
	char *psrc = (char *)src;

	while (cnt--) {
		*pdst++ = *psrc++;
	}
	return dest;
}


params::params(Graphics *l) {
	step = STEP_100HZ;
	band = BAND_40;
	vfo = VFOA;
	activeVfo = RX;
	mode = MODE_NORMAL;
	key = KEY_ST;
	speed = 18;
	exitParamMode = false;
	lcd = l;
	tc = BLACK;
	fgc = WHITE;
	bgc = LTGREEN;

	memcpy(param_display,default_param_display, sizeof(param_display));
	initParams(l);
}


void params::callback(void) {
	exitParamMode = true;
}

void params::initParams(Lcd *lcd) {
	uint16_t line;
	uint16_t col;
	char *valp = NULL;
	char buf[16];

	curParam = PARAM_STEP;

	while (curParam != PARAM_END) {
		line = param_display[curParam].label_psn.line;
		col =  param_display[curParam].label_psn.col;
		lcd->gotoxy(line,col);
		lcd->puts(param_display[curParam].label, fgc, bgc);

		line = param_display[curParam].val_psn.line;
		col =  param_display[curParam].val_psn.col;
		lcd->gotoxy(line,col);
		switch(curParam) {
			case PARAM_STEP:
				valp = step_strings[step];
				break;	
				
			case PARAM_BAND:
				valp = band_strings[band];
				break;
				
			case PARAM_VFO:
				valp = vfo_strings[vfo];
				break;
			
			case PARAM_KEY:
				valp = key_strings[key];
				break;
				
			case PARAM_KEY_SPEED:
				sprintf(buf,"%d", speed);
				valp = buf;
				break;
				
			case PARAM_ACT_VFO:
				valp =  vfoact_strings[activeVfo];
				break;
						
			default:
				break;
		}
		lcd->puts(valp, fgc,bgc);
		curParam = (param_t)((int)curParam + 1);
	}	
	curParam = PARAM_STEP;
}

step_t params::getStep(void) {
	return step;
}
 	
	
band_t params::getBand(void) {
	return band;
}
 	
	
vfo_select_t params::getVfo(void) {
	return vfo;
}


vfo_actv_t params::getActiveVfo(void) {
	return activeVfo;
}
 	
	
mode_t	params::getMode(void) {
	return mode;
}


void	params::setMode(mode_t newMode) {
	mode = newMode;
}


uint8_t params::getSpeed(void) {
	return speed;
}
	

key_t params::getKey(void) {
	return key;
}

void params::setActiveVfo(vfo_actv_t rxtx) {
char *valp = NULL;
uint16_t line;
uint16_t col;
pixColor pc;
	activeVfo = rxtx;
	valp =  vfoact_strings[activeVfo];
	line = param_display[PARAM_ACT_VFO].val_psn.line;
	col =  param_display[PARAM_ACT_VFO].val_psn.col;
	lcd->gotoxy(line,col);
	if (activeVfo == RX) {
		pc = GREEN;
	} else {
		pc = RED;
	}
	lcd->puts(valp,pc,bgc);
} 



void params::updateEnter(Encoder *enc, Sw *pb, Lcd *lcd) {
	//
	// hilight current parameter on entry to update
	//
	uint16_t line = param_display[curParam].label_psn.line;
	uint16_t col =  param_display[curParam].label_psn.col;

	tc = RED;
	lcd->gotoxy(line,col);
	lcd->puts(param_display[curParam].label, tc, bgc);
	enc->setEventRate(1);
	waitForOpen = false;
	update(enc, pb, lcd);
}


void params::update(Encoder *enc, Sw *pb, Lcd *lcd) {
	uint16_t line;
	uint16_t col;
	char *valp = NULL;
	char buf[16];

	tc = RED;
	if(exitParamMode) {
		exitParamMode = false;
		uint16_t line = param_display[curParam].label_psn.line;
		uint16_t col =  param_display[curParam].label_psn.col;

		lcd->gotoxy(line,col);
		lcd->puts(param_display[curParam].label, fgc, bgc);
		waitForOpen = true;
		enc->setEventRate(4);
	}

	if (pb->hasEvent()) {
		if (pb->getEvent() == EV_CLOSE) {
			pb->clearEvent();
			timerHandle = Clock::registerOneshot(this,TWO_SEC);
			exitParamMode = false;

		} else if (pb->getEvent() == EV_OPEN) {
			pb->clearEvent();
			if (waitForOpen) {
				waitForOpen = false;
				mode = MODE_NORMAL;
				return;

			} else {
				Clock::cancelOneshot(timerHandle);
				uint16_t line = param_display[curParam].label_psn.line;
				uint16_t col =  param_display[curParam].label_psn.col;

				lcd->gotoxy(line,col);
				lcd->puts(param_display[curParam].label, fgc, bgc);
				curParam = (param_t)((int)curParam + 1);
				if (curParam == PARAM_END) {
					curParam = PARAM_STEP;
				}					
				line = param_display[curParam].label_psn.line;
				col =  param_display[curParam].label_psn.col;
				lcd->gotoxy(line,col);
				lcd->puts(param_display[curParam].label, tc, bgc);
			}
		}
	}
	//		
	// now process encoder events which change the actual parameter value
	//
	if (enc->hasEvent()) {
		switch(curParam) {
			case PARAM_STEP:
				if (enc->getEvent() == ENC_CW_EVENT) {
					step = (step_t)((int)step + 1);
					if (step == END_OF_STEP) {
						step = STEP_10HZ;
					}

				} else if (enc->getEvent() == ENC_CCW_EVENT) {
					if (step == STEP_10HZ) {
						step = STEP_1MHZ;
					} else {
						step = (step_t)((int)step - 1);
					}
				}
				valp = step_strings[step];
				break;

			case PARAM_BAND:
				if (enc->getEvent() == ENC_CW_EVENT) {
					band = (band_t)((int)band + 1);
					if (band == END_OF_BAND) {
						band = BAND_160;
					}
				} else if (enc->getEvent() == ENC_CCW_EVENT) {
					if (band == BAND_160) {
						band = BAND_10;
					} else {
						band = (band_t)((int)band - 1);
					}
				}
				valp = band_strings[band];
				break;

			case PARAM_VFO:
				if (enc->getEvent() == ENC_CW_EVENT) {
					vfo = (vfo_select_t)((int)vfo + 1);
					if (vfo == END_OF_VFO) {
						vfo = VFOA;
					}
				} else if (enc->getEvent() == ENC_CCW_EVENT) {
					if (vfo == VFOA) {
						vfo = SPLIT;
					} else {
						vfo = (vfo_select_t)((int)vfo - 1);
					}
				}
				valp = vfo_strings[vfo];
				break;

			case PARAM_KEY:
				if (enc->getEvent() == ENC_CW_EVENT) {
					key = (key_t)((int)key + 1);
					if (key == END_OF_KEY) {
						key = KEY_ST;
					}
				} else if (enc->getEvent() == ENC_CCW_EVENT) {
					if (key == KEY_ST) {
						key = KEY_PDLS;
					} else {
						key = (key_t)((int)key - 1);
					}
				}
				valp = key_strings[key];
				break;

			case PARAM_KEY_SPEED:
				if (enc->getEvent() == ENC_CW_EVENT) {
					speed++;
					if (speed > 25) {
						speed = 25;
					}
				} else if (enc->getEvent() == ENC_CCW_EVENT) {
					speed--;
					if (speed < 5) {
						speed = 5;
					}
				}
				sprintf(buf,"%2i", speed);
				valp = buf;
				break;

			case PARAM_ACT_VFO:
				if (enc->getEvent() != ENC_NO_EVENT) {
					if (activeVfo == RX) {
						activeVfo = TX;
					} else {
						activeVfo = RX;
					}
				}
				valp =  vfoact_strings[activeVfo];
				break;

			default:
				break;
		}
		enc->clearEvent();
		line = param_display[curParam].val_psn.line;
		col =  param_display[curParam].val_psn.col;
		lcd->gotoxy(line,col);
		lcd->puts(valp, fgc, bgc);
	}


}





