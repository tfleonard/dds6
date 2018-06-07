/*
 * keyer.h
 *
 *  Created on: Jun 4, 2018
 *      Author: tleonard
 */

#ifndef KEYER_H_
#define KEYER_H_



class keyer {

private:
	Sw	dot;
	Sw	dash;
	params p;
	uint8_t speed;
	key_t mode;
	uint8_t state;

public:
	keyer(Sw dot_sw, Sw dash_sw, params pm);
	bool isIdle(void);
	void update(void);
	void setSpeed(uint8_t spd);
	void setMode(key_t md);






};







#endif /* KEYER_H_ */
