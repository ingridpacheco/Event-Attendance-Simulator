#include "Event.cpp"
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <time.h>
using namespace std;

// Function that determines the number of voice packages (actual function)
int voice_package_number() {
	long double seed = (long double) rand1();
	if (seed == 1) seed = .999999; // seed cannot be 1 or there would be a log(0), which is infinite, so... we make it a little smaller
	return (int) ceil(log(1.0 - seed) / log(1 - 1.0L / 22.0));
}

// Creates the arrival event of a data customer 
Event createData(float simulation_time, float lambda){
	double arrivalTime = exponential(lambda);
	Customer customer = Customer(DATA, simulation_time + arrivalTime);
	Event event = Event(simulation_time + arrivalTime, customer, ARRIVAL);
	return event;
}

// Creates the arrival event of a voice customer 
Event createVoice(float simulation_time, float offset, int channel_id){
	Customer customer = Customer(VOICE, simulation_time + offset);
	Event event = Event(simulation_time, customer, ARRIVAL, channel_id);
	return event;
}

// Creates the silence period event  
Event createSilencePeriod(float simulation_time, float offset, int channel_id){
	double arrivalTime = exponential(1.0/650);
	Event event = Event(simulation_time + arrivalTime + offset, channel_id);
	return event;
}

// Creates the exit event of any package
Event removePackage(float simulation_time, Customer customer){
	Event event = Event(simulation_time + customer.service_time, customer, EXIT);
	return event;
}

// It runs the rounds of the simulation
void rounds(int transientPeriod, int customersNumber, int roundNumber, float serviceAverage1, float lambda){
    queue* data_traffic = queue_create(); // Queue where the data packages are stored
    queue* voice_traffic = queue_create(); // Queue where the voice packages are stored
	
	float simulation_time = 0; // Current time in the simulator
	
	// Since we must ignore the first transientPeriod customers, only customers considered will be the ones with id >= 0
	// and we'll start counting from -transientPeriod
	Customer::totalCustomers = -transientPeriod; 
	
	Customer customer_being_served = Customer(-99999, NONE, 0); // The customer currently in the server. NONE type = no customer there.

    // Expectations/Averages
	float T1[roundNumber];
	float W1[roundNumber];
	float X1[roundNumber];
    float Nq1[roundNumber];
	float T2[roundNumber];
	float W2[roundNumber];
    float Nq2[roundNumber];
	
	// Interval between packages Estimator
    float EDelta[roundNumber];
    float VDelta[roundNumber];

	for (int i = 0; i < roundNumber; i++) {
		T1[i] = 0;
		W1[i] = 0;
		X1[i] = 0;
		Nq1[i] = 0;
		T2[i] = 0;
		W2[i] = 0;
		Nq2[i] = 0;
		
		EDelta[i] = 0;
		VDelta[i] = 0;
	}

    // People that came out of the system coming from both Queues;
    int out1 = 0; // data packages
    int out2 = 0; // voice packages
	int out = 0; // out1 + out2
	
    int voiceArrival = 16;
    int silenceTimeAvg = 560;
	
	// How many voice packages each channel needs to send before entering silence period.
	int voice_channels[30];
	for (int i = 0; i < 30; i++) voice_channels[i] = 0;

	list<Event> event_list;

	list_insert(event_list, createData(simulation_time, lambda));

	// VOICE CHANNELS
	for(int i = 0; i < 30; i++) {
		list_insert(event_list, createSilencePeriod(simulation_time, 0, i));
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

void execution(int transientPeriod, int customersNumber, int roundNumber, float utilization1){
    // The service 1 average time is going to be the package size average divided by the transmission rate
    float serviceAverage1 = (float) (755 * 8) / (float) (2 * 1024 * 1024);
    float lambda = utilization1 / serviceAverage1;
    // Starts rounds
    rounds(transientPeriod, customersNumber, roundNumber, serviceAverage1, lambda);
}
