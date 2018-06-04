#ifndef VFO_H
#define VFO_H

#include "Events.h"

typedef struct {
	uint32_t	rxFreq;
	int32_t rxOffset;
	uint8_t rxMult;
	uint32_t txFreq;
	int32_t txOffset;
	uint8_t	txMult;
} vfoParameters;

#define NUM_BANDS	9

typedef enum _band {
	BAND_160, 
	BAND_80,
	BAND_40,
	BAND_30,
	BAND_20,
	BAND_17,
	BAND_15,
	BAND_12,
	BAND_10,
	END_OF_BAND
} band_t;

typedef enum {
	STEP_10HZ, 
	STEP_100HZ, 
	STEP_1KHZ, 
	STEP_10KHZ, 
//	STEP_100KHZ, 
	STEP_1MHZ,
	END_OF_STEP
} step_t;

typedef enum {
	VFOA,
	VFOB,
	SPLIT,
	END_OF_VFO
} vfo_select_t;

typedef enum {
	RX,
	TX,
	END_OF_VFO_ACT
} vfo_actv_t;

#define VFOA_LINE	0
#define VFOB_LINE	36


class vfo {

private:
	vfoParameters vfoData[NUM_BANDS];
	uint32_t fstep;				// increment added for encoder event
	step_t step;			// current step
	Encoder *enc;
	void intToString32(uint32_t c, char *buf);
	void formatDisplayFreq(char *ibuf, char *obuf);
	uint16_t line;
	vfo_actv_t activeVfo;
	 
public:
	vfo(Encoder *enc, vfoParameters *vp, uint16_t line);
	void update(uint8_t band);
	float getRxFreq(band_t band);
	float getTxFreq(band_t band);
	void getRxDisplayFreq(char *buf, band_t band);
	void getTxDisplayFreq(char *buf, band_t band);
	void setStep(step_t step);
	void setActVfo(vfo_actv_t rxtx);
	step_t getStep(void);
	uint16_t getLine(void);

};

extern vfoParameters vfoDataConst[END_OF_BAND];

#endif // VFO_H
