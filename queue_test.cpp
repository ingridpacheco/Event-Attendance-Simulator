#include "data_structures.cpp"
#ifndef STDIO
#include <stdio.h>
#define STDIO
#endif
#ifndef STDLIB
#include <stdlib.h>
#define STDLIB
#endif

int main() {
	queue *q = queue_create();
	for (int i = 0; i < 10; i++) {
		queue_insert(q,i+1);
		printf("Size: %d\n",q->size);
	}
	for (int i = 0; i < 5; i++) {
		printf("%d\n", queue_remove(q));
		printf("Size: %d\n",q->size);
	}
	for (int i = 0; i < 10; i++) {
		queue_return(q,i+1);
		printf("Size: %d\n",q->size);
	}
	for (int i = 0; i < 20; i++) {
		printf("%d\n", queue_remove(q));
		printf("Size: %d\n",q->size);
	}
}