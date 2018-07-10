#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#ifndef CUSTOMER_H
#include "Customer.cpp"
#endif
#ifndef EVENT_H
#include "Event.cpp"
#endif

typedef struct LINKED_LIST {
	Customer customer;
	struct LINKED_LIST *next;
	struct LINKED_LIST *previous;
} linked_list;

typedef struct QUEUE {
	int size;
	linked_list *head_of_line;
	linked_list *end_of_line;
} queue;

queue* queue_create();
void queue_insert(queue *q, Customer c);
void queue_return(queue *q, Customer c);
Customer queue_remove(queue *q);

// Inserts a new event into the event list in chronological order
void list_insert(list<Event> *event_list, Event event);
// Removes an event from the event list, based on the corresponding customer's id
// Events related to voice channels should be removed with event_list.erase(event_list.begin())
void list_remove(list<Event> *event_list, int customer_id);

#endif