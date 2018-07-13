//#include "Event.cpp"
#include "run_queues.cpp"
#ifndef STDIO
#include <stdio.h>
#define STDIO
#endif
#ifndef STDLIB
#include <stdlib.h>
#define STDLIB
#endif
#ifndef MATH
#include <math.h>
#define MATH
#endif

// Creates the silence period event  
Event createSilencePeriod_deterministic(float simulation_time, float offset, int channel_id){
	double arrivalTime = 300;
	Event event = Event(simulation_time + arrivalTime + offset, channel_id);
	return event;
}

// It runs the rounds of the simulation for deterministics silence intervals
void deterministic_silence_test(int transientPeriod, int customersNumber, int roundNumber, float serviceAverage1){
	float lambda = 0.15;
    queue* voice_traffic = queue_create(); // Queue where the voice packages are stored
	
	float simulation_time = 0; // Current time in the simulator
	
	// Since we must ignore the first transientPeriod customers, only customers considered will be the ones with id >= 0
	// and we'll start counting from -transientPeriod
	Customer::totalCustomers = -transientPeriod; 
	
	Customer customer_being_served = Customer(-99999, NONE, 0); // The customer currently in the server. NONE type = no customer there.

    // Expectations/Averages
	float T2[roundNumber];
	float W2[roundNumber];
    float Nq2[roundNumber];
	
	// Interval between packages Estimator
    float EDelta[roundNumber];
    float VDelta[roundNumber];

	for (int i = 0; i < roundNumber; i++) {
		T2[i] = 0;
		W2[i] = 0;
		Nq2[i] = 0;
		
		EDelta[i] = 0;
		VDelta[i] = 0;
	}

    // People that came out of the system coming from both Queues;
    int out2 = 0; // voice packages
	int out = 0; // out1 + out2
	
	// How many voice packages each channel needs to send before entering silence period.
	int voice_channels[30];
	for (int i = 0; i < 30; i++) voice_channels[i] = 0;

	list<Event> event_list;

	// VOICE CHANNELS
	for(int i = 0; i < 30; i++) {
		list_insert(event_list, createSilencePeriod_deterministic(simulation_time, 0, i));
	}

	// In this loop, "i" means the number of packages created.
	for (int i = 0; i < customersNumber * roundNumber;) {
		Event current_event = *event_list.begin();
		event_list.erase(event_list.begin());
		Customer c_prev = customer_being_served; // needed to test if "treat_event" will change the customer in the server
		int data_queue_prev = data_traffic->size; // needed to test if "treat_event" will interrupt a data package being served
		current_event.treat_event(data_traffic, voice_traffic, &customer_being_served);
		simulation_time = current_event.time;
		if (current_event.etype == ARRIVAL && current_event.ctype == DATA) {
			i++;
			list_insert(event_list, createData(simulation_time, lambda)); // next data package
		} else if (current_event.etype == ARRIVAL && current_event.ctype == VOICE){
			if (voice_channels[current_event.channel_id] > 0) {
				voice_channels[current_event.channel_id]--;
				i++;
				list_insert(event_list, createVoice(simulation_time, 16, current_event.channel_id)); // next voice package of this channel
			} else {
				list_insert(event_list, createSilencePeriod(simulation_time, 16, current_event.channel_id)); // starts next silence period 16ms later
			}
			if (data_queue_prev > data_traffic->size) { // if a voice arrival increased the data queue, that means a data package was interrupted
				list_remove(event_list, data_traffic->head_of_line->customer.id);
			}
		} else if (current_event.etype == SILENCE_END) {
			voice_channels[current_event.channel_id] = voice_package_number();
			list_insert(event_list, createVoice(simulation_time, 0, current_event.channel_id)); // next voice package of this channel
		} else if (current_event.etype == EXIT && current_event.ctype == DATA){
			// UPDATE STATISTICS
		} else if (current_event.etype == EXIT && current_event.ctype == VOICE){
			// UPDATE STATISTICS
		} 
		if (customer_being_served.id != c_prev.id) { // checks if a new customer arrived at the server due to this event
			list_insert(event_list, removePackage(simulation_time, customer_being_served));
		}
	}
}

// Tests the queue about receiving new packages
void queue_test(){
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
			Customer c = Customer(DATA, timestamp++);
			queue_insert(data_queue, c);
			cout << "Inserted data package! (size = " << c.size << " bits)\n";
		} else if (input == 'V' || input == 'v') {
			Customer c = Customer(VOICE, timestamp++);
			queue_insert(voice_queue, c);
			cout << "Inserted voice package! (size = " << c.size << " bits)\n";
		} else if (input == 'P' || input == 'p') {
			if (voice_queue->size > 0) {
				Customer c = queue_remove(voice_queue);
				cout << "Type: " << c.type << "\nSize: " << c.size << "\n\n";
			}
			else if (data_queue->size > 0) {
				Customer c = queue_remove(data_queue);
				cout << "Type: " << c.type << "\nSize: " << c.size << "\n\n";
			}
		}
	}
	
	cout << '\n';
}

// Tests if the insertion in the queue is right
void insert_queue_test(){
	list<Event> event_list;
	char input;
	double input2;
	while(input != 'Q' && input != 'q') {
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

// Function that determines the size of a data package (actual function)
int data_package_size_test() {
	long double seed = rand1();
	if (seed < .3) return 64;
	else if (seed < .3 + .1) return 512;
	else if (seed < .3 + .1 + .3) return 1500;
	else {
		seed = (seed - .3 - .1 - .3) / (1 - .3 - .1 - .3);
		return seed * (1500 - 64) + 64;
	}
}

// Function that determines the number of voice packages (actual function)
int voice_package_number_test() {
	long double seed = (long double) rand1();
	if (seed == 1) seed = .999999; // seed cannot be 1 or there would be a log(0), which is infinite, so... we make it a little smaller
	return (int) ceil(log(1.0 - seed) / log(1 - 1.0L / 22.0));
}

// This function only exists to make sure that the percentages are correct
void sample_mean_test(int (*function)()) {
	int current_sample;
	int n64 = 0;
	int n512 = 0;
	int n1500 = 0;
	for (int i = 0; i < BIGNUM; i++) {
		current_sample = (*function)();
		if (current_sample == 64) n64++;
		else if (current_sample == 512) n512++;
		else if (current_sample == 1500) n1500++;
	}
	printf("\t\t64: %lf%%\n\t\t512: %lf%%\n\t\t1500: %lf%%\n", (double)n64/BIGNUM * 100, (double)n512/BIGNUM * 100, (double)n1500/BIGNUM * 100);
}

int main() {
	unsigned int random;
	printf("Random Seed: ");
	scanf("%int", &random);
	srand(random);
	
	putchar('\n');
	
	printf("Data Package Size\n");
	printf("\tSample Mean: %lf\n", sample_mean(&data_package_size_test));
	sample_mean_test(&data_package_size_test);
	
	putchar('\n');
	
	printf("Voice Package Quantity\n");
	printf("\tSample Mean: %lf\n", sample_mean(&voice_package_number_test));
	
	putchar('\n');
	
	printf("Exponential Silence Period\n");
	printf("\tSample Mean: %Lf\n", sample_mean(&exponential,(long double)1/.650));

	putchar('\n');

	printf("Insertion in List\n");
	insert_queue_test();

	printf("Queue\n");
	queue_test();

	putchar('\n');
/* 
	print("Voice Package:\n");
	printf() */

}