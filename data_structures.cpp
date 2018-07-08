#include "data_structures.h"
#ifndef STDLIB
#include <stdlib.h>
#define STDLIB
#endif

// Returns a new queue with size 0
queue* queue_create() {
	queue* q = (queue*) malloc(sizeof(queue*));
	q->size = 0;
	q->head_of_line = NULL;
	q->end_of_line = NULL;
	return q;
}

// Inserts value "i" at the end of a queue "q"
void queue_insert(queue *q, int i) {
	if (q->end_of_line == NULL) {
		q->end_of_line = (linked_list*) malloc(sizeof(linked_list));
		q->end_of_line->value = i;
		q->end_of_line->next = NULL;
		q->end_of_line->previous = NULL;
		q->head_of_line = q->end_of_line;
	} else {
		q->end_of_line->next = (linked_list*) malloc(sizeof(linked_list));
		q->end_of_line->next->value = i;
		q->end_of_line->next->next = NULL;
		q->end_of_line->next->previous = q->end_of_line;
		q->end_of_line = q->end_of_line->next;
	}
	q->size++;
}

// Returns value "i" back to the head of a queue "q"
void queue_return(queue *q, int i) {
	if (q->head_of_line == NULL) {
		q->head_of_line = (linked_list*) malloc(sizeof(linked_list));
		q->head_of_line->value = i;
		q->head_of_line->next = NULL;
		q->head_of_line->previous = NULL;
		q->end_of_line = q->head_of_line;
	} else {
		q->head_of_line->previous = (linked_list*) malloc(sizeof(linked_list));
		q->head_of_line->previous->value = i;
		q->head_of_line->previous->previous = NULL;
		q->head_of_line->previous->next = q->head_of_line;
		q->head_of_line = q->head_of_line->previous;
	}
	q->size++;
}

// Removes the head of the line and returns it
// If queue is empty, returns -1
int queue_remove(queue *q) {
	if (q->head_of_line == NULL) {
		return -1;
	} else {
		int ret = q->head_of_line->value;
		q->head_of_line = q->head_of_line->next;
		if (q->head_of_line != NULL) free(q->head_of_line->previous);
		q->size--;
		return ret;
	}
}