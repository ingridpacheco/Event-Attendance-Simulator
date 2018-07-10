#include "Event.h"

Event::Event(double time, Customer c, event_type etype) {
	this->time = time;
	this->customer_id = c.id;
	this->ctype = c.type;
	this->etype = etype;
}

void Event:treat_event(queue *data_queue, queue *voice_queue, Customer *current) {
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
		} else {} // Treat silence period end
	}
}

#endif