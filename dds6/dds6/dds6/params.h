/*
 * params.h
 *
 * Created: 4/16/2018 8:41:05 AM
 *  Author: tleonard
 *
 * Update and maintain operating parameters for vfo
 *
 */ 


#ifndef PARAMS_H_
#define PARAMS_H_


#include "Callback.h"
#include "Events.h"
#include "vfo.h"			


typedef enum _mode_t {
	MODE_NORMAL,
	MODE_PARAMS,
	MODE_SETUP
} mode_t;



typedef enum _key_t {
	KEY_ST,
	KEY_BUG,
	KEY_PDLS,
	END_OF_KEY
} key_t;



typedef enum _param {
	PARAM_STEP,
	PARAM_BAND,
	PARAM_VFO,
	PARAM_KEY,
	PARAM_KEY_SPEED,
	PARAM_ACT_VFO,
	PARAM_END
}param_t;


typedef struct _param_entry_t {
	param_t Param;
	pixpsn_t label_psn;
	char *label;
	pixpsn_t val_psn;
} param_entry_t;


class params :public Callback {

private:
	step_t step;
	band_t band;
	vfo_select_t vfo;
	vfo_actv_t activeVfo;
	mode_t	mode;
	key_t key;
	uint8_t speed;
	param_entry_t param_display[PARAM_END];
	void initParams(Lcd *l);
	param_t curParam;
	bool exitParamMode;
	int	timerHandle;
	bool waitForOpen;
	Graphics *lcd;
	pixColor fgc;
	pixColor bgc;
	pixColor tc;

public:
	params(Graphics *g);

	step_t getStep(void);
	band_t getBand(void);
	vfo_select_t getVfo(void);
	vfo_actv_t getActiveVfo(void);
	void setActiveVfo(vfo_actv_t rxtx);
	mode_t	getMode(void);
	void	setMode(mode_t newMode);
	void callback(void);
	void update(Encoder *enc, Sw *pb, Lcd *lcd);
	void updateEnter(Encoder *enc, Sw *pb, Lcd *lcd);
	uint8_t getSpeed(void);
	key_t getKey(void);
	
};






#endif /* UI_H_ */