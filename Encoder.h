/*
 * Encoder.h
 *
 * Created: 3/28/2018 11:53:17 AM
 *  Author: tleonard
 */ 


#ifndef ENCODER_H_
#define ENCODER_H_

typedef enum _encState {
	ENC_NONE,
	ENC_A,
	ENC_B,
	ENC_AB
}encState;


class Encoder : public Callback {

private:
	Sw *encA;
	Sw* encB;
	int curState;
	encEvent lastEvent;
	void update(void);
	int	missedCwCount;
	int missedCcwCount;
	int eventRate;

public:
	Encoder(uint8_t a, uint8_t b, uint8_t port);
	bool	hasEvent(void);
	encEvent getEvent(void);
	void	clearEvent(void);
	void	callback(void);
	int getMissedCwCount(void);
	int getMissedCcwCount(void);
	void setEventRate(uint8_t rate);

};



#endif /* ENCODER_H_ */