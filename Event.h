#ifndef EVENT_H
#define EVENT_H

#ifndef LIST
#include <list>
#define LIST
#endif
using namespace std;
#ifndef DATA_STRUCTURES_H
#include "data_structures.cpp"
#endif
/*
	Only 3 types of event are necessary.
	When a DATA package arrives, it's added to the data queue.
	When a VOICE package arrives, it's added to the voice queue OR interrupts the data being served.
	When a DATA package exits, the next voice package in line is served (if any) OR the next data package in line is served (if any) OR the server becomes empty.
	When a VOICE package exits, the next voice package in line is served (if any) OR the next data package in line is served (if any) OR the server becomes empty.
	Whent the SILENCE period ends, the corresponding voice channel generates the voice package arrival events.
*/

// Defines the type of the event
typedef enum {
	ARRIVAL,
	EXIT,
	SILENCE_END
} event_type;

class Event {
	public:
		double time; // the time when it begins
		Customer customer; // the customer related to the event
		int channel_id; // the id of the voice channel related to the event
		event_type etype; // if the event is an arrival in the system or an exit of it
		Event(double time, Customer c, event_type etype, int channel_id = -1);
		Event(double time, int channel_id);
		void treat_event(queue *data_queue, queue *voice_queue, Customer *current);
};

// Inserts a new event into the event list in chronological order
void list_insert(list<Event> *event_list, Event event);
// Removes an event from the event list, based on the corresponding customer's id
// Events related to voice channels should be removed with event_list.erase(event_list.begin())
void list_remove(list<Event> *event_list, int customer_id);

#endif