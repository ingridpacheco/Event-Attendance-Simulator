#include "Event.cpp"
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <time.h>
#include <fstream>
#include <vector>
using namespace std;

// Function that determines the number of voice packages (actual function)
int voice_package_number() {
	long double seed = (long double) rand1();
	if (seed == 1) seed = .999999; // seed cannot be 1 or there would be a log(0), which is infinite, so... we make it a little smaller
	return (int) ceil(log(1.0 - seed) / log(1 - 1.0L / 22.0));
}

// Creates the arrival event of a data customer 
Event createData(int customer_id, int round, float simulation_time, float lambda){
	double arrivalTime = exponential(lambda);
	Customer customer = Customer(customer_id, round, DATA, simulation_time + arrivalTime);
	Event event = Event(simulation_time + arrivalTime, customer, ARRIVAL);
	return event;
}

// Creates the arrival event of a voice customer 
Event createVoice(int customer_id, int round, float simulation_time, float offset, int channel_id){
	Customer customer = Customer(customer_id, round, VOICE, simulation_time + offset, channel_id);
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
void rounds(int transientPeriod, int customersNumber, int roundNumber, float serviceAverage1, float lambda, bool preemption, bool allow_logging){
    queue* data_traffic = queue_create(); // Queue where the data packages are stored
    queue* voice_traffic = queue_create(); // Queue where the voice packages are stored

	double simulation_time = 0; // Current time in the simulator
	double lastTime[30]; //Keeps the last time a package of each specific channel entered the server

	for (int i = 0; i < 30; i++){
		lastTime[i] = 0;
	}

	// Gets the intervals of each channel;
	vector <double> intervals;
	
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
	
	// Averages, lower limits and upper limits of the confidence intervals
	float ET1 = 0, lower_T1, upper_T1;
	float EW1 = 0, lower_W1, upper_W1;
	float EX1 = 0, lower_X1, upper_X1;
	float ENq1 = 0, lower_Nq1, upper_Nq1;
	float ET2 = 0, lower_T2, upper_T2;
	float EW2 = 0, lower_W2, upper_W2;
	float ENq2 = 0, lower_Nq2, upper_Nq2;
	float EEDelta = 0, lower_EDelta, upper_EDelta;
	float EVDelta = 0, lower_VDelta, upper_VDelta;
	
	// Standard deviations of the confidence intervals
	float ST1 = 0;
	float SW1 = 0;
	float SX1 = 0;
	float SNq1 = 0;
	float ST2 = 0;
	float SW2 = 0;
	float SNq2 = 0;
	float SEDelta = 0;
	float SVDelta = 0;
	
	// Variables used for the areas method (Data Queue)
	double time_data = 0; // data queue timestamps
	int size_data; // data queue sizes
	
	// Variables used for the areas method (Voice Queue)
	double time_voice = 0; // voice queue timestamps
	int size_voice; // voice queue sizes
	
    // People that came out of the system coming from both Queues;
    int out1 = 0; // data packages
    int out2 = 0; // voice packages
	int out = 0; // out1 + out2
	
	// How many voice packages each channel needs to send before entering silence period.
	int voice_channels[30];
	for (int i = 0; i < 30; i++) voice_channels[i] = 0;

	list<Event> event_list;

	list_insert(event_list, createData(Customer::totalCustomers++, 0, simulation_time, lambda));

	// VOICE CHANNELS
	for(int i = 0; i < 30; i++) {
		list_insert(event_list, createSilencePeriod(simulation_time, 0, i));
	}
	
	// Number of packages created in each round that eventually left the system
	int round_data_exits[roundNumber];
	for (int i = 0; i < roundNumber; i++) round_data_exits[i] = 0;
	int round_voice_exits[roundNumber];
	for (int i = 0; i < roundNumber; i++) round_voice_exits[i] = 0;

	//DEBUG FILES
	ofstream log_file, averages_file;
	if (allow_logging) {
		log_file.open ("log.txt");
	}

	int simulation_percentage = -1;
	cout << endl;
	// Main loop of events
	for (int round = 0; round < roundNumber; round++) {
		//--- Variables needed to calculate the statistics for each round ---//
		double round_time = simulation_time;
		//-----------------------------------------------------------------//
		while (Customer::totalCustomers < customersNumber * (round+1)) {
			Event current_event = *event_list.begin();
			
			event_list.erase(event_list.begin());
			Customer c_prev = customer_being_served; // needed to test if "treat_event" will change the customer in the server
			int data_queue_prev = data_traffic->size; // needed to test if "treat_event" will interrupt a data package being served
			int voice_queue_prev = voice_traffic->size; // both of these are needed for the Areas Method
			current_event.treat_event(data_traffic, voice_traffic, &customer_being_served, preemption);
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
				list_insert(event_list, createData(-99999, round, simulation_time, lambda)); // next data package
			} else if (current_event.etype == ARRIVAL && current_event.customer.type == VOICE){
				if (voice_channels[current_event.channel_id] > 0) {
					voice_channels[current_event.channel_id]--;
					list_insert(event_list, createVoice(-99999, round, simulation_time, 16, current_event.channel_id)); // next voice package of this channel
				} else {
					list_insert(event_list, createSilencePeriod(simulation_time, 16, current_event.channel_id)); // starts next silence period 16ms later
				}
				if (data_queue_prev < data_traffic->size) { // if a voice arrival increased the data queue, that means a data package was interrupted
					Customer removed_customer = queue_remove(data_traffic); // the data package that suffered interruption
					removed_customer.time_in_service += (simulation_time - removed_customer.checkpoint_time);
					removed_customer.checkpoint_time = simulation_time;
					queue_return(data_traffic, removed_customer);
					if (allow_logging) log_file << "1 -- " << "REMOVED " << removed_customer.id << " FROM DATA QUEUE" << endl;
					list_remove(event_list, removed_customer.id);
				}
				
			} else if (current_event.etype == SILENCE_END) {
				if (lastTime[current_event.channel_id] != 0) lastTime[current_event.channel_id] = 0;
				voice_channels[current_event.channel_id] = voice_package_number();
				//cout << "Channel " << current_event.channel_id << " will generate: " << voice_channels[current_event.channel_id] << " voice packages!\n";
				if (voice_channels[current_event.channel_id] > 0) {
					voice_channels[current_event.channel_id]--;
					list_insert(event_list, createVoice(-99999, round, simulation_time, 0, current_event.channel_id)); // next voice package of this channel
				}
			} else if (current_event.etype == EXIT && current_event.customer.type == DATA){
				T1[current_event.customer.round] += (current_event.time - current_event.customer.arrival_time);
				W1[current_event.customer.round] += current_event.customer.time_in_queue;
				current_event.customer.time_in_service += (simulation_time - current_event.customer.checkpoint_time);
				X1[current_event.customer.round] += current_event.customer.time_in_service;
				
				round_data_exits[current_event.customer.round]++;
			} else if (current_event.etype == EXIT && current_event.customer.type == VOICE){
				T2[current_event.customer.round] += (current_event.time - current_event.customer.arrival_time);
				W2[current_event.customer.round] += current_event.customer.time_in_queue;

				round_voice_exits[current_event.customer.round]++;
			}
			if (customer_being_served.id != c_prev.id) { // checks if a new customer arrived at the server due to this event
				if (customer_being_served.type != NONE) {
					customer_being_served.time_in_queue += (simulation_time - customer_being_served.checkpoint_time);
					customer_being_served.checkpoint_time = simulation_time;
					if(customer_being_served.type == VOICE){
						if (lastTime[customer_being_served.channel_id] != 0){
							intervals.push_back(customer_being_served.checkpoint_time - lastTime[customer_being_served.channel_id]);
						}
						//cout << "CHANNEL ID: " << customer_being_served.channel_id << " LAST TIME: " << lastTime[customer_being_served.channel_id] << " CHECKPOINT TIME: " << customer_being_served.checkpoint_time <<" INTERVAL: " << customer_being_served.checkpoint_time - lastTime[customer_being_served.channel_id] << "\n";
						lastTime[customer_being_served.channel_id] = customer_being_served.checkpoint_time;
					}
					list_insert(event_list, removePackage(simulation_time, customer_being_served));
				}
			}
			
			
			// Percentage of simulation complete
			if (simulation_percentage != (int)((float) Customer::totalCustomers * 100 / (customersNumber * roundNumber))) {
				cout << "Simulation in progress... " << (int)((float) Customer::totalCustomers * 100 / (customersNumber * roundNumber)) << "%" << '\r';
				simulation_percentage = (int)((float) Customer::totalCustomers * 100 / (customersNumber * roundNumber));
			}
			
			//=======
			if (allow_logging) {
				log_file << "1 -- " << "TIME: " << current_event.time << "ms TYPE: ";
				if (current_event.etype == ARRIVAL && current_event.customer.type == DATA) log_file << "Data Arrival";
				else if (current_event.etype == ARRIVAL && current_event.customer.type == VOICE) log_file << "Voice Arrival";
				else if (current_event.etype == SILENCE_END) log_file << "Silence End";
				else if (current_event.etype == EXIT && current_event.customer.type == DATA) log_file << "Data Departure";
				else if (current_event.etype == EXIT && current_event.customer.type == VOICE) log_file << "Voice Departure";
				if (current_event.etype != SILENCE_END) log_file << " CUSTOMER ID: "<< (current_event.customer.id);
				log_file << " CHANNEL ID: " << current_event.channel_id << "\n";
			}
			//=======
			
		}
		
		if (allow_logging) log_file << "End of Round " << round << " ; Duration: " << (simulation_time - round_time) << "ms\n\n";
		// Areas Method requires dividing the area by the time spent
		Nq1[round] /= (simulation_time - round_time);
		Nq2[round] /= (simulation_time - round_time);

		// Sum all the intervals into delta of the round
		for (int i = 0; i < intervals.size(); i++){
			EDelta[round] += intervals[i];
		}

		// If there is some interval, divide by the quantity to get the average
		if (!intervals.empty()) EDelta[round] = EDelta[round]/intervals.size();

		for (int i = 0; i < intervals.size(); i++){
			VDelta[round] += pow((intervals[i] - EDelta[round]), 2);
		}

		if (intervals.size() <= 1) VDelta[round] = 0;
		else VDelta[round] = VDelta[round]/(intervals.size() - 1);

		// cout << "ROUND: " << round << " INTERVAL SIZE: " << intervals.size() << " EDelta: " << EDelta[round] << " VDelta: " << VDelta[round] << "\n";

		intervals.clear();

	}
	
	// Divide the sum of times by the number of events to find the average
	for (int round = 0; round < roundNumber; round++) {
		if (round_data_exits[round] > 0) {
			T1[round] /= round_data_exits[round];
			W1[round] /= round_data_exits[round];
			X1[round] /= round_data_exits[round];
		}
		if (round_voice_exits[round] > 0) {
			T2[round] /= round_voice_exits[round];
			W2[round] /= round_voice_exits[round];
		}
	}
	
	cout << endl;
	
	if (allow_logging) log_file.close();
	
	// ----- Finding the averages of the confidence intervals ----- //
	// Only rounds from which at least one package left the system are eligible for certain statistics.
	int eligible_data_rounds = 0, eligible_voice_rounds = 0;
	for(int i=0; i < roundNumber; i++) { 
		if (round_data_exits[i] > 0) eligible_data_rounds++;
		if (round_voice_exits[i] > 0) eligible_voice_rounds++;
		ET1 += T1[i];
		EW1 += W1[i];
		EX1 += X1[i];
		ENq1 += Nq1[i];
		ET2 += T2[i];
		EW2 += W2[i];
		ENq2 += Nq2[i];
		EEDelta += EDelta[i];
		EVDelta += VDelta[i];
	}
	ET1 /= eligible_data_rounds;
	EW1 /= eligible_data_rounds;
	EX1 /= eligible_data_rounds;
	ENq1 /= roundNumber;
	ET2 /= eligible_voice_rounds;
	EW2 /= eligible_voice_rounds;
	ENq2 /= roundNumber;
	EEDelta /= roundNumber;
	EVDelta /= roundNumber;
	
	// Finding the standard deviations of the confidence intervals
	for(int i=0; i < roundNumber; i++) { 
		ST1 += pow(T1[i] - ET1, 2);
		SW1 += pow(W1[i] - EW1, 2);
		SX1 += pow(X1[i] - EX1, 2);
		SNq1 += pow(Nq1[i] - ENq1, 2);
		ST2 += pow(T2[i] - ET2, 2);
		SW2 += pow(W2[i] - EW2, 2);
		SNq2 += pow(Nq2[i] - ENq2, 2);
		SEDelta += pow(EDelta[i] - EEDelta, 2);
		SVDelta += pow(VDelta[i] - EVDelta, 2);
	}
	ST1 /= (roundNumber - 1);
	ST1 = sqrt(ST1);
	SW1 /= (roundNumber - 1);
	SW1 = sqrt(SW1);
	SX1 /= (roundNumber - 1);
	SX1 = sqrt(SX1);
	SNq1 /= (roundNumber - 1);
	SNq1 = sqrt(SNq1);
	ST2 /= (roundNumber - 1);
	ST2 = sqrt(ST2);
	SW2 /= (roundNumber - 1);
	SW2 = sqrt(SW2);
	SNq2 /= (roundNumber - 1);
	SNq2 = sqrt(SNq2);
	SEDelta /= (roundNumber - 1);
	SEDelta = sqrt(SEDelta);
	SVDelta /= (roundNumber - 1);
	SVDelta = sqrt(SVDelta);
	
	// Finding the confidence intervals.
	// Lower limits must always be non-negative.
	lower_T1 = ET1 - 1.645 * ST1 / sqrt(roundNumber); lower_T1 = (lower_T1 > 0) ? lower_T1 : 0; upper_T1 = ET1 + 1.645 * ST1 / sqrt(roundNumber);
	lower_W1 = EW1 - 1.645 * SW1 / sqrt(roundNumber); lower_W1 = (lower_W1 > 0) ? lower_W1 : 0; upper_W1 = EW1 + 1.645 * SW1 / sqrt(roundNumber);
	lower_X1 = EX1 - 1.645 * SX1 / sqrt(roundNumber); lower_X1 = (lower_X1 > 0) ? lower_X1 : 0; upper_X1 = EX1 + 1.645 * SX1 / sqrt(roundNumber);
	lower_Nq1 = ENq1 - 1.645 * SNq1 / sqrt(roundNumber); lower_Nq1 = (lower_Nq1 > 0) ? lower_Nq1 : 0; upper_Nq1 = ENq1 + 1.645 * SNq1 / sqrt(roundNumber);
	lower_T2 = ET2 - 1.645 * ST2 / sqrt(roundNumber); lower_T2 = (lower_T2 > 0) ? lower_T2 : 0; upper_T2 = ET2 + 1.645 * ST2 / sqrt(roundNumber);
	lower_W2 = EW2 - 1.645 * SW2 / sqrt(roundNumber); lower_W2 = (lower_W2 > 0) ? lower_W2 : 0; upper_W2 = EW2 + 1.645 * SW2 / sqrt(roundNumber);
	lower_Nq2 = ENq2 - 1.645 * SNq2 / sqrt(roundNumber); lower_Nq2 = (lower_Nq2 > 0) ? lower_Nq2 : 0; upper_Nq2 = ENq2 + 1.645 * SNq2 / sqrt(roundNumber);
	lower_EDelta = EEDelta - 1.645 * SEDelta / sqrt(roundNumber); lower_EDelta = (lower_EDelta > 0) ? lower_EDelta : 0; upper_EDelta = EEDelta + 1.645 * SEDelta / sqrt(roundNumber);
	lower_VDelta = EVDelta - 1.645 * SVDelta / sqrt(roundNumber); lower_VDelta = (lower_VDelta > 0) ? lower_VDelta : 0; upper_VDelta = EVDelta + 1.645 * SVDelta / sqrt(roundNumber);
	
	
	// Creates a file where all the averages of each round are stored
	if (allow_logging) {
		averages_file.open ("averages.txt");
		averages_file << "\nT1: [ ";
		for (int i = 0; i < roundNumber-1; i++) averages_file << T1[i] << ", "; averages_file << T1[roundNumber-1] << " ]\n";
		
		averages_file << "\nW1: [ ";
		for (int i = 0; i < roundNumber-1; i++) averages_file << W1[i] << ", "; averages_file << W1[roundNumber-1] << " ]\n";
		
		averages_file << "\nX1: [ ";
		for (int i = 0; i < roundNumber-1; i++) averages_file << X1[i] << ", "; averages_file << X1[roundNumber-1] << " ]\n";
		
		averages_file << "\nNq1: [ ";
		for (int i = 0; i < roundNumber-1; i++) averages_file << Nq1[i] << ", "; averages_file << Nq1[roundNumber-1] << " ]\n";
		
		averages_file << "\nT2: [ ";
		for (int i = 0; i < roundNumber-1; i++) averages_file << T2[i] << ", "; averages_file << T2[roundNumber-1] << " ]\n";
		
		averages_file << "\nW2: [ ";
		for (int i = 0; i < roundNumber-1; i++) averages_file << W2[i] << ", "; averages_file << W2[roundNumber-1] << " ]\n";
		
		averages_file << "\nNq2: [ ";
		for (int i = 0; i < roundNumber-1; i++) averages_file << Nq2[i] << ", "; averages_file << Nq2[roundNumber-1] << " ]\n";
		
		averages_file << "\nE[Delta]: [ ";
		for (int i = 0; i < roundNumber-1; i++) averages_file << EDelta[i] << ", "; averages_file << EDelta[roundNumber-1] << " ]\n";
		
		averages_file << "\nV[Delta]: [ ";
		for (int i = 0; i < roundNumber-1; i++) averages_file << VDelta[i] << ", "; averages_file << VDelta[roundNumber-1] << " ]\n";
		
		averages_file.close();
	}
	
	// Prints the results
	cout << "Intervalos de confianca: " << endl;
	cout << "\nE[T1]: " << lower_T1 << " < " << ET1 << " < " << upper_T1 << endl;
	cout << "\nE[W1]: " << lower_W1 << " < " << EW1 << " < " << upper_W1 << endl;
	cout << "\nE[X1]: " << lower_X1 << " < " << EX1 << " < " << upper_X1 << endl;
	cout << "\nE[Nq1]: " << lower_Nq1 << " < " << ENq1 << " < " << upper_Nq1 << endl;
	cout << "\nE[T2]: " << lower_T2 << " < " << ET2 << " < " << upper_T2 << endl;
	cout << "\nE[W2]: " << lower_W2 << " < " << EW2 << " < " << upper_W2 << endl;
	cout << "\nE[Nq2]: " << lower_Nq2 << " < " << ENq2 << " < " << upper_Nq2 << endl;
	cout << "\nE[Delta]: " << lower_EDelta << " < " << EEDelta << " < " << upper_EDelta << endl;
	cout << "\nV[Delta]: " << lower_VDelta << " < " << EVDelta << " < " << upper_VDelta << endl;
	
}

void execution(int transientPeriod, int customersNumber, int roundNumber, float utilization1, bool preemption, bool allow_logging){
    // The service 1 average time is going to be the package size average divided by the transmission rate
    float serviceAverage1 = (float) (755 * 8) / (float) (0.002 * 1024 * 1024);
    float lambda = utilization1 / serviceAverage1;
    // Starts rounds
    rounds(transientPeriod, customersNumber, roundNumber, serviceAverage1, lambda, preemption, allow_logging);
}
