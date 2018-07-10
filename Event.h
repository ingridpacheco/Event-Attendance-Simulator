#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "Customer.cpp"
/*
	Only 3 types of event are necessary.
	When a DATA package arrives, it's added to the data queue.
	When a VOICE package arrives, it's added to the voice queue OR interrupts the data being served.
	When a DATA package exits, the next voice package in line is served (if any) OR the next data package in line is served (if any) OR the server becomes empty.
	When a VOICE package exits, the next voice package in line is served (if any) OR the next data package in line is served (if any) OR the server becomes empty.
	Whent the SILENCE period ends, the corresponding voice channel generates the voice package arrival events.
*/

typedef enum {
	ARRIVAL,
	EXIT,
	SILENCE_END
} event_type;

class Event {
	public:
		double time; // the time when it begins
		int customer_id; // the id of the customer related to the event
		int channel_id; // the id of the voice channel related to the event
		customer_type ctype; // type of customer related to event
		event_type etype; // if the event is an arrival in the system or an exit of it
		Event(double time, Customer c, event_type etype);
		Event(double time, int channel_id);
		void treat_event(queue *data_queue, queue *voice_queue, Customer *current);
};

#endif