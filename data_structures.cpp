#include "data_structures.h"
#ifndef STDLIB
#include <stdlib.h>
#define STDLIB
#endif

// Returns a new queue with size 0
queue* queue_create() {
	queue* q = (queue*) malloc(sizeof(queue));
	q->size = 0;
	q->head_of_line = NULL;
	q->end_of_line = NULL;
	return q;
}

// Inserts customer "c" at the end of a queue "q"
void queue_insert(queue *q, Customer c) {
	if (q->end_of_line == NULL) {
		q->end_of_line = (linked_list*) malloc(sizeof(linked_list));
		q->end_of_line->customer = c;
		q->end_of_line->next = NULL;
		q->end_of_line->previous = NULL;
		q->head_of_line = q->end_of_line;
	} else {
		q->end_of_line->next = (linked_list*) malloc(sizeof(linked_list));
		q->end_of_line->next->customer = c;
		q->end_of_line->next->next = NULL;
		q->end_of_line->next->previous = q->end_of_line;
		q->end_of_line = q->end_of_line->next;
	}
	q->size++;
}

// Returns customer "c" back to the head of a queue "q"
void queue_return(queue *q, Customer c) {
	if (q->head_of_line == NULL) {
		q->head_of_line = (linked_list*) malloc(sizeof(linked_list));
		q->head_of_line->customer = c;
		q->head_of_line->next = NULL;
		q->head_of_line->previous = NULL;
		q->end_of_line = q->head_of_line;
	} else {
		q->head_of_line->previous = (linked_list*) malloc(sizeof(linked_list));
		q->head_of_line->previous->customer = c;
		q->head_of_line->previous->previous = NULL;
		q->head_of_line->previous->next = q->head_of_line;
		q->head_of_line = q->head_of_line->previous;
	}
	q->size++;
}

// Removes the head of the line and returns it
// If queue is empty, returns customer of type NONE
Customer queue_remove(queue *q) {
	if (q->head_of_line == NULL) {
		return Customer(NONE,0);
	} else {
		Customer ret = q->head_of_line->customer;
		linked_list *curr_head = q->head_of_line;
		q->head_of_line = q->head_of_line->next;
		free(curr_head);
		q->size--;
		if (q->size == 0) q->end_of_line = NULL;
		return ret;
	}
}

/*
void queue_print(queue *q) {
	linked_list *pointer = q->head_of_line;
	while (pointer != q->end_of_line) {
		cout << pointer->customer.size << "\n";
		pointer = pointer->next;
	}
	if (q->end_of_line != NULL) cout << q->end_of_line->customer.size;
}
*/