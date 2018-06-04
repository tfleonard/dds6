/*
 * Events.h
 *
 * Created: 3/28/2018 9:25:34 AM
 *  Author: tleonard
 */ 


#ifndef EVENTS_H_
#define EVENTS_H_

#define EVENT_ENC_CW		(1 << 0)
#define EVENT_ENC_CCW		(1 << 1)
#define EVENT_ENC_MASK	(EVENT_ENC_CW | EVENT_ENC_CCW)

#define EVENT_DOT_CLOSE	(1 << 2)
#define EVENT_DOT_OPEN		(1 << 3)
#define EVENT_DOT_MASK	(EVENT_DOT_CLOSE | EVENT_DOT_OPEN)

#define EVENT_DASH_CLOSE	(1 << 4)
#define EVENT_DASH_OPEN		(1 << 5)
#define EVENT_DASH_MASK		(EVENT_DASH_CLOSE | EVENT_DASH_OPEN)

#define EVENT_PB_CLOSE		(1 << 6)
#define EVENT_PB_OPEN			(1 << 7)
#define EVENT_PB_MASK			(EVENT_PB_OPEN | EVENT_PB_CLOSE)

typedef enum _event {
	EV_NONE,
	EV_OPEN,
	EV_CLOSE
} event;


typedef enum _encEvent {
	ENC_NO_EVENT,
	ENC_CW_EVENT,
	ENC_CCW_EVENT
}encEvent;



#endif /* EVENTS_H_ */