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

public:

	ElapsedTime(uint16_t nt);
	void callback(void);
	bool expired(void);

};


#endif /* ELAPSEDTIME_H_ */