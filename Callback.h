/*
 * Callback.h
 *
 * Created: 3/12/2018 9:12:54 AM
 *  Author: tleonard
 */ 


#ifndef CALLBACK_H_
#define CALLBACK_H_


//
// This class defines a callback interface
// to be used by functions to register callbacks with other functions
// The first use is to provide callbacks to the clock routines.
//

class Callback
{
	public:
	virtual void callback(void) = 0;
};



#endif /* CALLBACK_H_ */