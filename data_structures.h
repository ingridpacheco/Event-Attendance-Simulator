#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

typedef struct LINKED_LIST {
	int value;
	struct LINKED_LIST *next;
	struct LINKED_LIST *previous;
} linked_list;

typedef struct QUEUE {
	int size;
	linked_list *head_of_line;
	linked_list *end_of_line;
} queue;

queue* queue_create();
void queue_insert(queue *q, int i);
void queue_return(queue *q, int i);
int queue_remove(queue *q);

#endif