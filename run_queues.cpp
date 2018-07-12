#include "Event.cpp"
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <time.h>
#include <fstream>
using namespace std;

// Function that determines the number of voice packages (actual function)
int voice_package_number() {
	long double seed = (long double) rand1();
	if (seed == 1) seed = .999999; // seed cannot be 1 or there would be a log(0), which is infinite, so... we make it a little smaller
	return (int) ceil(log(1.0 - seed) / log(1 - 1.0L / 22.0));
}

// Creates the arrival event of a data customer 
Event createData(int customer_id, float simulation_time, float lambda){
	double arrivalTime = exponential(lambda);
	Customer customer = Customer(customer_id, DATA, simulation_time + arrivalTime);
	Event event = Event(simulation_time + arrivalTime, customer, ARRIVAL);
	return event;
}

// Creates the arrival event of a voice customer 
Event createVoice(int customer_id, float simulation_time, float offset, int channel_id){
	Customer customer = Customer(customer_id, VOICE, simulation_time + offset);
	Event event = Event(simulation_time + offset, customer, ARRIVAL, channel_id);
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
	//Customer::totalCustomers = -transientPeriod; 
	
	Customer customer_being_served = Customer(); // The customer currently in the server. NONE type = no customer there.

    // Expectations/Averages
	float T1[roundNumber]; // Average time a data package stays in the system
	float W1[roundNumber]; // Average time a data package stays in the queue
	float X1[roundNumber]; // Average time a data package stays in the server
    float Nq1[roundNumber]; // Average number of data packages in the queue
	float T2[roundNumber]; // Average time a voice package stays in the system
	float W2[roundNumber]; // Average time a voice package stays in the queue
    float Nq2[roundNumber]; // Average number of voice packages in the queue
	
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
	
	// Variables used for the areas method (Data Queue)
	double time_data = 0; // data queue timestamps
	int size_data; // data queue sizes
	
	// Variables used for the areas method (Voice Queue)
	double time_voice = 0; // data queue timestamps
	int size_voice; // data queue sizes
	
    // People that came out of the system coming from both Queues;
    int out1 = 0; // data packages
    int out2 = 0; // voice packages
	int out = 0; // out1 + out2
	
	// How many voice packages each channel needs to send before entering silence period.
	int voice_channels[30];
	for (int i = 0; i < 30; i++) voice_channels[i] = 0;

	list<Event> event_list;

	list_insert(event_list, createData(Customer::totalCustomers++, simulation_time, lambda));

	// VOICE CHANNELS
	for(int i = 0; i < 30; i++) {
		list_insert(event_list, createSilencePeriod(simulation_time, 0, i));
	}

	//DEBUG FILE
	ofstream myfile;
	myfile.open ("log.txt");

int aaaa = 0;
int bbbb = 0;
	// Main loop of events
	for (int round = 0; round < roundNumber; round++) {
		//--- Variables needed to calculate the statistics for each round ---//
		double round_time = simulation_time;
		int round_data_exits = 0;
		int round_voice_exits = 0;
		//-----------------------------------------------------------------//
		while (Customer::totalCustomers < customersNumber * (round+1)) {
			Event current_event = *event_list.begin();
			
			event_list.erase(event_list.begin());
			Customer c_prev = customer_being_served; // needed to test if "treat_event" will change the customer in the server
			int data_queue_prev = data_traffic->size; // needed to test if "treat_event" will interrupt a data package being served
			int voice_queue_prev = voice_traffic->size; // both of these are needed for the Areas Method
			current_event.treat_event(data_traffic, voice_traffic, &customer_being_served);
			simulation_time = current_event.time;
			
			
			//============ Areas Method ============//
			if (data_queue_prev != data_traffic->size) {
				if (time_data == 0) {
					time_data = simulation_time;
					size_data = data_traffic->size;
				} else {
					Nq1[round] += fabs((simulation_time - time_data) * (size_data));
					time_data = simulation_time;
					size_data = data_traffic->size;
				}
			}
			if (voice_queue_prev != voice_traffic->size) {
				if (time_voice == 0) {
					time_voice = simulation_time;
					size_voice = voice_traffic->size;
				} else {
					Nq2[round] += fabs((simulation_time - time_voice) * (size_voice));
					time_voice = simulation_time;
					size_voice = voice_traffic->size;
				}
			}
			//======================================//
			
			
			if (current_event.etype == ARRIVAL && current_event.customer.type == DATA) {
				list_insert(event_list, createData(-99999, simulation_time, lambda)); // next data package
			} else if (current_event.etype == ARRIVAL && current_event.customer.type == VOICE){
				if (voice_channels[current_event.channel_id] > 0) {
					voice_channels[current_event.channel_id]--;
					list_insert(event_list, createVoice(-99999, simulation_time, 16, current_event.channel_id)); // next voice package of this channel
				} else {
					list_insert(event_list, createSilencePeriod(simulation_time, 16, current_event.channel_id)); // starts next silence period 16ms later
				}
				if (data_queue_prev > data_traffic->size) { // if a voice arrival increased the data queue, that means a data package was interrupted
					Customer removed_customer = data_traffic->head_of_line->customer; // the data package that suffered interruption
					removed_customer.checkpoint_time = simulation_time; // <<<<<<<<<<<<<<<<<<<<<<<<<<<<< PRECISO DAR UM JEITO DE ACESSAR ESSE CARA PRA FAZER ESSA ATUALIZACAO!!!!!!
					list_remove(event_list, removed_customer.id);
				}
				
			} else if (current_event.etype == SILENCE_END) {
				voice_channels[current_event.channel_id] = voice_package_number();
				//cout << "Channel " << current_event.channel_id << " will generate: " << voice_channels[current_event.channel_id] << " voice packages!\n";
				if (voice_channels[current_event.channel_id] > 0) {
					voice_channels[current_event.channel_id]--;
					list_insert(event_list, createVoice(-99999, simulation_time, 0, current_event.channel_id)); // next voice package of this channel
				}
			} else if (current_event.etype == EXIT && current_event.customer.type == DATA){
				T1[round] += (current_event.time - current_event.customer.arrival_time);
				round_data_exits++;
			} else if (current_event.etype == EXIT && current_event.customer.type == VOICE){
				T2[round] += (current_event.time - current_event.customer.arrival_time);
				W2[round] += current_event.customer.time_in_queue;

				round_voice_exits++;
			} 
			if (customer_being_served.id != c_prev.id) { // checks if a new customer arrived at the server due to this event
				if (customer_being_served.type != NONE) {
					customer_being_served.time_in_queue += (simulation_time - customer_being_served.checkpoint_time);
					customer_being_served.checkpoint_time = simulation_time;
					list_insert(event_list, removePackage(simulation_time, customer_being_served));
				}
			}
			
			
			
			//=======
			myfile << "1 -- " << "TIME: " << current_event.time << "ms TYPE: ";
			if (current_event.etype == ARRIVAL && current_event.customer.type == DATA) myfile << "Data Arrival";
			else if (current_event.etype == ARRIVAL && current_event.customer.type == VOICE) myfile << "Voice Arrival";
			else if (current_event.etype == SILENCE_END) myfile << "Silence End";
			else if (current_event.etype == EXIT && current_event.customer.type == DATA) myfile << "Data Departure";
			else if (current_event.etype == EXIT && current_event.customer.type == VOICE) myfile << "Voice Departure";
			if (current_event.etype != SILENCE_END) myfile << " CUSTOMER ID: "<< (current_event.customer.id);
			myfile << " CHANNEL ID: " << current_event.channel_id << "\n";
			//=======
			
		}
		myfile << "End of Round " << round << " ; Duration: " << (simulation_time - round_time) << "ms\n\n";
		// Areas Method requires dividing the area by the time spent
		Nq1[round] /= (simulation_time - round_time);
		Nq2[round] /= (simulation_time - round_time);
		
		// Divide the sum of times by the number of events to find the average
		if (round_data_exits > 0) T1[round] /= round_data_exits;
		if (round_voice_exits > 0) T2[round] /= round_voice_exits;
		if (round_voice_exits > 0) W2[round] /= round_voice_exits;
	}
	
	//cout << "\nqueue size: " << data_traffic->size;
	//cout << "\na: " << aaaa << "\nb: " << bbbb << "\na-b: " << aaaa - bbbb << "\n";
	
	cout << "\nNq1: [ ";
	for (int i = 0; i < roundNumber-1; i++) cout << Nq1[i] << ", "; cout << Nq1[roundNumber-1] << " ]\n";
	float ENq1 = 0;
	for(int i=0; i < roundNumber; i++) ENq1 += Nq1[i];
	ENq1 /= roundNumber;
	
	cout << "\nNq2: [ ";
	for (int i = 0; i < roundNumber-1; i++) cout << Nq2[i] << ", "; cout << Nq2[roundNumber-1] << " ]\n";
	float ENq2 = 0;
	for(int i=0; i < roundNumber; i++) ENq2 += Nq2[i];
	ENq2 /= roundNumber;
	
	cout << "\nT1: [ ";
	for (int i = 0; i < roundNumber-1; i++) cout << T1[i] << ", "; cout << T1[roundNumber-1] << " ]\n";
	float ET1 = 0;
	for(int i=0; i < roundNumber; i++) ET1 += T1[i];
	ET1 /= roundNumber;
	
	cout << "\nT2: [ ";
	for (int i = 0; i < roundNumber-1; i++) cout << T2[i] << ", "; cout << T2[roundNumber-1] << " ]\n";
	float ET2 = 0;
	for(int i=0; i < roundNumber; i++) ET2 += T2[i];
	ET2 /= roundNumber;
	
	cout << "\nW2: [ ";
	for (int i = 0; i < roundNumber-1; i++) cout << W2[i] << ", "; cout << W2[roundNumber-1] << " ]\n";
	float EW2 = 0;
	for(int i=0; i < roundNumber; i++) EW2 += W2[i];
	EW2 /= roundNumber;
	
	
	cout << "\nE[T1]: " << ET1 << "\n";
	cout << "\nE[Nq1]: " << ENq1 << "\n";
	cout << "\nE[T2]: " << ET2 << "\n";
	cout << "\nE[W2]: " << EW2 << "\n";
	cout << "\nE[Nq2]: " << ENq2 << "\n";
	
}

void execution(int transientPeriod, int customersNumber, int roundNumber, float utilization1){
    // The service 1 average time is going to be the package size average divided by the transmission rate
    float serviceAverage1 = (float) (755 * 8) / (float) (0.002 * 1024 * 1024);
    float lambda = utilization1 / serviceAverage1;
    // Starts rounds
    rounds(transientPeriod, customersNumber, roundNumber, serviceAverage1, lambda);
}
