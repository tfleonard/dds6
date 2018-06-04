/*
 * ElapsedTime.h
 *
 * Created: 3/28/2018 2:36:26 PM
 *  Author: tleonard
 */ 


#ifndef ELAPSEDTIME_H_
#define ELAPSEDTIME_H_



class ElapsedTime :public Callback {

private:
	uint8_t event;
	uint8_t ticks;
	uint8_t	numTicks;

public:

	ElapsedTime(uint8_t nt);
	void callback(void);
	bool expired(void);

};


#endif /* ELAPSEDTIME_H_ */