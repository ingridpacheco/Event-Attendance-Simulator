#include "Event.h"
#ifndef IOSTREAM
#include <iostream>
#define IOSTREAM
#endif
#ifndef LIST
#include <list>
#define LIST
#endif
using namespace std;

Event::Event(double time, Customer c, event_type etype) {
	this->time = time;
	this->customer_id = c.id;
	this->ctype = c.type;
	this->etype = etype;
}

Event::Event(double time, int channel_id) {
	this->time = time;
	this->channel_id = channel_id;
	this->etype = SILENCE_END;
}

void Event::treat_event(queue *data_queue, queue *voice_queue, Customer *current) {
	if (this->etype == ARRIVAL) {
		if (current->type == NONE) {
			*current = Customer(this->customer_id, this->ctype, this->time);
		} else if (this->ctype == DATA) {
			queue_insert(data_queue, Customer(this->customer_id, DATA, this->time));
		} else if (this->ctype == VOICE) {
			// if not preemptive
			queue_insert(voice_queue, Customer(this->customer_id, VOICE, this->time));
			// if preemptive
			/*
			if (current->type == DATA) {
				queue_return(data_queue, *current);
				*current = Customer(Customer(this->customer_id, VOICE, this->time));
			} else queue_insert(voice_queue, Customer(this->customer_id, VOICE, this->time));
			*/
		}
	} else if (this->etype == EXIT) {
		if (voice_queue->size > 0) {
			*current = queue_remove(voice_queue);
		} else if (data_queue->size > 0) {
			*current = queue_remove(data_queue);
		} else *current = Customer(-99999, NONE, 0);
	} else {} // Treat silence period end
}

// Inserts a new event into the event list in chronological order
void list_insert(list<Event> &event_list, Event event) {
	for (list<Event>::iterator it = event_list.begin(); it != event_list.end(); it++) {
		if (it->time > event.time) {
			cout << "aaaaa\n";
			event_list.insert(it, event);
			return;
		}
	}
	event_list.insert(event_list.end(), event);
}

// Removes an event from the event list, based on the corresponding customer's id
// Events related to voice channels should be removed with event_list.erase(event_list.begin())
void list_remove(list<Event> &event_list, int customer_id) {
	for (list<Event>::iterator it = event_list.begin(); it != event_list.end(); it++) {
		if (it->customer_id == customer_id) {
			event_list.erase(it);
			return;
		}
	}
}

/*
int main() {
	list<Event> event_list;
	char input;
	double input2;
	while(input != 'q' && input != 'q') {
		cout << "\n";
		cout << "[P]rint or [I]nsert: ";
		cin >> input;
		if (input == 'p' || input == 'P') {
			cout << "\nevent_list contains:";
			for (list<Event>::iterator it = event_list.begin(); it != event_list.end(); it++)
				cout << ' ' << it->time;
		} else if (input == 'i' || input == 'I') {
			cout << "Time: ";
			cin >> input2;
			list_insert(event_list, Event(input2, 0));
		}
	}
	cout << "\n";
}
*/