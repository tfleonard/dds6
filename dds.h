/* 
 * File:   dds.h
 * Author: root
 *
 * Created on April 9, 2009, 7:11 PM
 */

#ifndef _DDS_H
#define	_DDS_H

#ifdef	__cplusplus
extern "C" {
#endif



#define FQUD_CLK		(1 << PB1)		// Freq Update clock												PA4
#define W_CLK      	(1 << PB0)    // shift clock                              PA5
#define DDS_RST  		(1 << PB5)    // reset										                PA3
#define SD	        (1 << PB2)   // serial data big, LSB first               PA6
#define DDS_OUT_BITS	(FQUD_CLK | W_CLK | SD | DDS_RST)		// PB2-PB0
#define DDS_DEFAULT 0


//#define CNTS_PER_HZ 107.3741824
#define CNTS_PER_HZ    34.359738368




//
// function prototypes
//
void dds(float fout);
void DdsInit(void);






#ifdef	__cplusplus
}
#endif

#endif	/* _DDS_H */

