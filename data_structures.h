#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

#include "Customer.h"

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

#endif