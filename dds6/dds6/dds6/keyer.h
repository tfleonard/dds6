/*
 * keyer.h
 *
 *  Created on: Jun 4, 2018
 *      Author: tleonard
 */

#ifndef KEYER_H_
#define KEYER_H_

#include "Callback.h"
#include "timer.h"

#define WEIGHT		3


class keyer : public Callback {

private:
	Sw	*dot;
	Sw	*dash;
	uint8_t speed;
	key_t mode;
	uint16_t dot_time;
	uint8_t state;
	relay	*rly;
	
public:
	keyer(Sw *dot_sw, Sw *dash_sw, relay *rly);
	bool isIdle(void);
	void update(void);
	void setSpeed(uint8_t spd);
	void setMode(key_t md);
	void callback(void);






};







#endif /* KEYER_H_ */
