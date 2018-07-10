#include "data_structures.cpp"
#include "Customer.cpp"
#ifndef IOSTREAM
#include <iostream>
#define IOSTREAM
#endif
using namespace std;
#ifndef STDLIB
#include <stdlib.h>
#define STDLIB
#endif

int main() {
	queue *data_queue = queue_create();
	queue *voice_queue = queue_create();
	char input = '\0';
	int timestamp = 0;
	unsigned int seed;
	
	cout << "Random Seed:";
	cin >> seed;
	srand(seed);
	cout << "\n";
	
	cout << "COMMANDS:\n\tD = New data package\n\tV = New voice package\n\tP = Print next\n\tE or Q = Exit\n\n";
	
	while (input != 'e' && input != 'E' && input != 'q' && input != 'Q') {
		cout << "Data packages: " << data_queue->size << "\n";
		cout << "Voice packages: " << voice_queue->size << "\n";
		cout << "\n";
		cin >> input;
		if (input == 'D' || input == 'd') {
			cout << "Inserting DATA\n";
			Customer c = Customer(DATA, timestamp++);
			queue_insert(data_queue, c);
			cout << "Inserted data package! (size = " << c.size << " bits)\n";
		} else if (input == 'V' || input == 'v') {
			cout << "Inserting VOICE\n";
			Customer c = Customer(VOICE, timestamp++);
			queue_insert(voice_queue, c);
			cout << "Inserted voice package! (size = " << c.size << " bits)\n";
		} else if (input == 'P' || input == 'p') {
			if (voice_queue->size > 0) {
				cout << "Printing VOICE\n";
				Customer c = queue_remove(voice_queue);
				cout << "Type: " << c.type << "\nSize: " << c.size << "\n\n";
			}
			else if (data_queue->size > 0) {
				cout << "Printing DATA\n";
				Customer c = queue_remove(data_queue);
				cout << "Type: " << c.type << "\nSize: " << c.size << "\n\n";
			}
		}
	}
	
	cout << '\n';
	
	
}