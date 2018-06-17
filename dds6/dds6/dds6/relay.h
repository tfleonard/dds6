/*
 * relay.h
 *
 * Created: 6/17/2018 11:19:26 AM
 *  Author: tleonard
 */ 


#ifndef RELAY_H_
#define RELAY_H_




class relay {
	
private:
	params *param;
	vfo *curVfo;
	band_t band;

public:
	relay(params *prm);	
	void setVfo(vfo *v);
	void selectTx(void);
	void selectRx(void);
	void setBand(band_t b);
};



#endif /* RELAY_H_ */