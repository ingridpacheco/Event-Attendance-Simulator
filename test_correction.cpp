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


// Creates the arrival event of a data customer with deterministic time 
Event create_data_deterministic(int customer_id, int round, float simulation_time, float lambda){
	double arrivalTime = 60;
	Customer customer = Customer(customer_id, round, DATA, simulation_time + arrivalTime);
	Event event = Event(simulation_time + arrivalTime, customer, ARRIVAL);
	return event;
}

// Creates the silence period event with deterministic time
Event create_silence_period_deterministic(float simulation_time, float offset, int channel_id){
	double arrivalTime = 100 + (channel_id * 5);
	Event event = Event(simulation_time + arrivalTime + offset, channel_id);
	return event;
}

// Creates the silence period event with exponencial time
Event create_silence_period_random(float simulation_time, float offset, int channel_id){
	double arrivalTime = exponential(1.0/650);
	Event event = Event(simulation_time + arrivalTime + offset, channel_id);
	return event;
}

// It runs the rounds of the simulation for deterministic silence period if has_data is false and for deterministic data otherwise
void round_test(int transiente_period, int customers_number, int round_number, float lambda, bool preemption, bool allow_logging, bool has_data){
	queue* data_traffic = queue_create(); // Queue where the data packages are stored
    queue* voice_traffic = queue_create(); // Queue where the voice packages are stored

	double simulation_time = 0; // Current time in the simulator
	double last_time[30]; //Keeps the last time a package of each specific channel entered the server

	for (int i = 0; i < 30; i++){
		last_time[i] = 0;
	}

	// Gets the intervals of each channel;
	vector <double> intervals;
	
	// Since we must ignore the first transiente_period customers, only customers considered will be the ones with id >= 0
	// and we'll start counting from -transiente_period
	Customer::total_customers = -transiente_period; 
	
	Customer customer_being_served = Customer(); // The customer currently in the server. NONE type = no customer there.

    // Expectations/Averages
	float T1[round_number+1]; // Average time a data package stays in the system
	float W1[round_number+1]; // Average time a data package stays in the queue
	float X1[round_number+1]; // Average time a data package stays in the server
    float Nq1[round_number+1]; // Average number of data packages in the queue
	float T2[round_number+1]; // Average time a voice package stays in the system
	float W2[round_number+1]; // Average time a voice package stays in the queue
    float Nq2[round_number+1]; // Average number of voice packages in the queue
	
	// Interval between packages Estimator
    float EDelta[round_number+1];
    float VDelta[round_number+1];

	for (int i = 0; i < round_number+1; i++) {
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

	// If this is the deterministic data test, than the data package will be created
	if (has_data) list_insert(event_list, create_data_deterministic(Customer::total_customers++, 0, simulation_time, lambda));

	// VOICE CHANNELS
	for(int i = 0; i < 30; i++) {
		 if (!has_data) list_insert(event_list, create_silence_period_deterministic(simulation_time, 0, i));
		 else list_insert(event_list, create_silence_period_random(simulation_time, 0, i));
	}
	
	// Number of packages created in each round that eventually left the system
	int round_data_exits[round_number];
	for (int i = 0; i < round_number; i++) round_data_exits[i] = 0;
	int round_voice_exits[round_number];
	for (int i = 0; i < round_number; i++) round_voice_exits[i] = 0;

	//DEBUG FILES
	ofstream log_file, averages_file;
	if (allow_logging) {
		// Determines each file for both tests
		if (!has_data) log_file.open ("log_deterministic_test.txt");
		else log_file.open ("log_deterministic_data_test.txt");
	}

	int simulation_percentage = -1;
	cout << endl;

	cout << "Simulation in progress... 0%\r";

// Main loop of events
	for (int round = 0; round < round_number+1; round++) {
		//--- Variables needed to calculate the statistics for each round ---//
		double round_time = simulation_time;
		//-----------------------------------------------------------------//
		
		if (allow_logging) {
			stringstream sstm;
			sstm << "log_" << round << ".txt";
			log_file.open (sstm.str().c_str());
		}
		// Loops until customers_number customers are sampled or, if this is round 0, until the transiente_period customers are sampled
		while ((round > 0 && Customer::total_customers < customers_number * (round+1)) || (round == 0 && Customer::total_customers < 0)) {
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
				list_insert(event_list, create_data_deterministic(-99999, round, simulation_time, lambda)); // next data package
			} else if (current_event.etype == ARRIVAL && current_event.customer.type == VOICE){
				if (voice_channels[current_event.channel_id] > 0) {
					voice_channels[current_event.channel_id]--;
					list_insert(event_list, create_voice(-99999, round, simulation_time, 16, current_event.channel_id)); // next voice package of this channel
				} else {
					if (!has_data) list_insert(event_list, create_silence_period_deterministic(simulation_time, 16, current_event.channel_id)); // starts next silence period 16ms later
					else list_insert(event_list, create_silence_period_random(simulation_time, 16, current_event.channel_id));
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
				if (last_time[current_event.channel_id] != 0) last_time[current_event.channel_id] = 0;
				voice_channels[current_event.channel_id] = voice_package_number();
				if (voice_channels[current_event.channel_id] > 0) {
					voice_channels[current_event.channel_id]--;
					list_insert(event_list, create_voice(-99999, round, simulation_time, 0, current_event.channel_id)); // next voice package of this channel
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
					//If a Voice is being served, gets the interval
					if(customer_being_served.type == VOICE){
						//If it is not the first voice package of the channel
						if (last_time[customer_being_served.channel_id] != 0){
							intervals.push_back(customer_being_served.checkpoint_time - last_time[customer_being_served.channel_id]);
						}
						last_time[customer_being_served.channel_id] = customer_being_served.checkpoint_time;
					}
					list_insert(event_list, remove_package(simulation_time, customer_being_served));
				}
			}
			
			
			// Percentage of simulation complete
			if (Customer::total_customers > 0 && simulation_percentage != (int)((float) Customer::total_customers * 100 / (customers_number * round_number))) {
				cout << "Simulation in progress... " << (int)((float) Customer::total_customers * 100 / (customers_number * round_number)) << "%" << '\r';
				simulation_percentage = (int)((float) Customer::total_customers * 100 / (customers_number * round_number));
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
		
		if (allow_logging) {
			log_file << "End of Round " << round << " ; Duration: " << (simulation_time - round_time) << "ms\n\n";
			log_file.close();
		}
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

		intervals.clear();

	}
	
	// Divide the sum of times by the number of events to find the average
	for (int round = 0; round < round_number+1; round++) {
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
	
	// ----- Finding the averages of the confidence intervals ----- //
	// Only rounds from which at least one package left the system are eligible for certain statistics.
	int eligible_data_rounds = 0, eligible_voice_rounds = 0;
	for(int i=1; i < round_number+1; i++) { 
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
	ENq1 /= round_number;
	ET2 /= eligible_voice_rounds;
	EW2 /= eligible_voice_rounds;
	ENq2 /= round_number;
	EEDelta /= round_number;
	EVDelta /= round_number;
	
	// Finding the standard deviations of the confidence intervals
	for(int i=1; i < round_number+1; i++) { 
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
	ST1 /= (round_number - 1);
	ST1 = sqrt(ST1);
	SW1 /= (round_number - 1);
	SW1 = sqrt(SW1);
	SX1 /= (round_number - 1);
	SX1 = sqrt(SX1);
	SNq1 /= (round_number - 1);
	SNq1 = sqrt(SNq1);
	ST2 /= (round_number - 1);
	ST2 = sqrt(ST2);
	SW2 /= (round_number - 1);
	SW2 = sqrt(SW2);
	SNq2 /= (round_number - 1);
	SNq2 = sqrt(SNq2);
	SEDelta /= (round_number - 1);
	SEDelta = sqrt(SEDelta);
	SVDelta /= (round_number - 1);
	SVDelta = sqrt(SVDelta);
	
	// Finding the confidence intervals.
	// Lower limits must always be non-negative.
	lower_T1 = ET1 - 1.645 * ST1 / sqrt(round_number); lower_T1 = (lower_T1 > 0) ? lower_T1 : 0; upper_T1 = ET1 + 1.645 * ST1 / sqrt(round_number);
	lower_W1 = EW1 - 1.645 * SW1 / sqrt(round_number); lower_W1 = (lower_W1 > 0) ? lower_W1 : 0; upper_W1 = EW1 + 1.645 * SW1 / sqrt(round_number);
	lower_X1 = EX1 - 1.645 * SX1 / sqrt(round_number); lower_X1 = (lower_X1 > 0) ? lower_X1 : 0; upper_X1 = EX1 + 1.645 * SX1 / sqrt(round_number);
	lower_Nq1 = ENq1 - 1.645 * SNq1 / sqrt(round_number); lower_Nq1 = (lower_Nq1 > 0) ? lower_Nq1 : 0; upper_Nq1 = ENq1 + 1.645 * SNq1 / sqrt(round_number);
	lower_T2 = ET2 - 1.645 * ST2 / sqrt(round_number); lower_T2 = (lower_T2 > 0) ? lower_T2 : 0; upper_T2 = ET2 + 1.645 * ST2 / sqrt(round_number);
	lower_W2 = EW2 - 1.645 * SW2 / sqrt(round_number); lower_W2 = (lower_W2 > 0) ? lower_W2 : 0; upper_W2 = EW2 + 1.645 * SW2 / sqrt(round_number);
	lower_Nq2 = ENq2 - 1.645 * SNq2 / sqrt(round_number); lower_Nq2 = (lower_Nq2 > 0) ? lower_Nq2 : 0; upper_Nq2 = ENq2 + 1.645 * SNq2 / sqrt(round_number);
	lower_EDelta = EEDelta - 1.645 * SEDelta / sqrt(round_number); lower_EDelta = (lower_EDelta > 0) ? lower_EDelta : 0; upper_EDelta = EEDelta + 1.645 * SEDelta / sqrt(round_number);
	lower_VDelta = EVDelta - 1.645 * SVDelta / sqrt(round_number); lower_VDelta = (lower_VDelta > 0) ? lower_VDelta : 0; upper_VDelta = EVDelta + 1.645 * SVDelta / sqrt(round_number);	
	
	// Creates a file where all the averages of each round are stored
	if (allow_logging) {
		if (!has_data) averages_file.open ("averages_deterministic_test.txt");
		else {
			averages_file.open ("averages_deterministic_data_test.txt");
			averages_file << "\nT1: [ ";
			for (int i = 0; i < round_number-1; i++) averages_file << T1[i] << ", "; averages_file << T1[round_number-1] << " ]\n";
			
			averages_file << "\nW1: [ ";
			for (int i = 0; i < round_number-1; i++) averages_file << W1[i] << ", "; averages_file << W1[round_number-1] << " ]\n";
			
			averages_file << "\nX1: [ ";
			for (int i = 0; i < round_number-1; i++) averages_file << X1[i] << ", "; averages_file << X1[round_number-1] << " ]\n";
			
			averages_file << "\nNq1: [ ";
			for (int i = 0; i < round_number-1; i++) averages_file << Nq1[i] << ", "; averages_file << Nq1[round_number-1] << " ]\n";
		}
		
		averages_file << "\nT2: [ ";
		for (int i = 0; i < round_number-1; i++) averages_file << T2[i] << ", "; averages_file << T2[round_number-1] << " ]\n";
		
		averages_file << "\nW2: [ ";
		for (int i = 0; i < round_number-1; i++) averages_file << W2[i] << ", "; averages_file << W2[round_number-1] << " ]\n";
		
		averages_file << "\nNq2: [ ";
		for (int i = 0; i < round_number-1; i++) averages_file << Nq2[i] << ", "; averages_file << Nq2[round_number-1] << " ]\n";
		
		averages_file << "\nE[Delta]: [ ";
		for (int i = 0; i < round_number-1; i++) averages_file << EDelta[i] << ", "; averages_file << EDelta[round_number-1] << " ]\n";
		
		averages_file << "\nV[Delta]: [ ";
		for (int i = 0; i < round_number-1; i++) averages_file << VDelta[i] << ", "; averages_file << VDelta[round_number-1] << " ]\n";
		
		averages_file.close();
	}
	
	// Prints the results
	cout << "Intervalos de confianca: " << endl;
	if (has_data){
		cout << "\nE[T1]: " << lower_T1 << " < " << ET1 << " < " << upper_T1 << endl;
		cout << "\nE[W1]: " << lower_W1 << " < " << EW1 << " < " << upper_W1 << endl;
		cout << "\nE[X1]: " << lower_X1 << " < " << EX1 << " < " << upper_X1 << endl;
		cout << "\nE[Nq1]: " << lower_Nq1 << " < " << ENq1 << " < " << upper_Nq1 << endl;
	}
	cout << "\nE[T2]: " << lower_T2 << " < " << ET2 << " < " << upper_T2 << endl;
	cout << "\nE[W2]: " << lower_W2 << " < " << EW2 << " < " << upper_W2 << endl;
	cout << "\nE[Nq2]: " << lower_Nq2 << " < " << ENq2 << " < " << upper_Nq2 << endl;
	cout << "\nE[Delta]: " << lower_EDelta << " < " << EEDelta << " < " << upper_EDelta << endl;
	cout << "\nV[Delta]: " << lower_VDelta << " < " << EVDelta << " < " << upper_VDelta << endl;
	
}


// Tests the queue about receiving new packages
void queue_test(){
	queue *data_queue = queue_create();
	queue *voice_queue = queue_create();
	char input = '\0';
	int timestamp = 0;
	
	cout << "COMMANDS:\n\tD = New data package\n\tV = New voice package\n\tP = Print next\n\tE or Q = Exit test\n\n";
	
	while (input != 'e' && input != 'E' && input != 'q' && input != 'Q') {
		cout << "Data packages: " << data_queue->size << "\n";
		cout << "Voice packages: " << voice_queue->size << "\n";
		cout << "\n";
		cin >> input;
		if (input == 'D' || input == 'd') {
			Customer c = Customer(0, 0, DATA, timestamp++);
			queue_insert(data_queue, c);
			cout << "Inserted data package! (size = " << c.size << " bits)\n";
		} else if (input == 'V' || input == 'v') {
			Customer c = Customer(0, 0, VOICE, timestamp++);
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
		cout << "COMMANDS:\n\tD = New data package\n\tV = New voice package\n\tP = Print next\n\tE or Q = Exit test\n\n";
	}
	
	cout << '\n';
}

// Tests if the insertion and removal in the list are right
void event_list_test(){
	list<Event> event_list;
	char input;
	double input2;
	int input3;
	int id = 0;
	
	cout << "COMMANDS:\n\tP = Print event list\n\tI = Insert event\n\tR = Remove event\n\tE or Q = Exit test\n\n";
	
	while(input != 'e' && input != 'E' && input != 'q' && input != 'Q') {
		cout << "\n";
		cin >> input;
		if (input == 'p' || input == 'P') {
			cout << "\nEvent list:";
			for (list<Event>::iterator it = event_list.begin(); it != event_list.end(); it++)
				cout << ' ' << it->time << "(" << it->customer.id << ")";
			cout << endl;
		} else if (input == 'i' || input == 'I') {
			cout << "Time: ";
			cin >> input2;
			list_insert(event_list, Event(input2, Customer(id++,0,DATA,input2), ARRIVAL));
		} else if (input == 'r' || input == 'R') {
			cout << "ID: ";
			cin >> input3;
			list_remove(event_list, input3);
		}
	
		cout << "COMMANDS:\n\tP = Print event list\n\tI = Insert event\n\tR = Remove event\n\tE or Q = Exit test\n\n";
	
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
	if (seed == 1) seed = .999999; // seed cannot be 1 or there would be a log(0), which is infinite,
	// so... we make it a little smaller
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
	cout << "\t\t64: " << (double)n64/BIGNUM * 100 << "%\n\t\t512: " << 
	(double)n512/BIGNUM * 100 << "%\n\t\t1500: " << (double)n1500/BIGNUM * 100 << "%" << endl;
}

int main() {
	unsigned int random;
	cout << "Random Seed: ";
	cin >> random;
	srand(random);
	
	cout << endl;
	
	cout << "Data Package Size" << endl;
	cout << "\tSample Mean: " << sample_mean(&data_package_size_test) << " bytes\n";
	sample_mean_test(&data_package_size_test);
	
	cout << endl;
	
	cout << "Voice Package Quantity" << endl;
	cout << "\tSample Mean: " << sample_mean(&voice_package_number_test) << endl;
	
	cout << endl;
	
	cout << "Exponential Silence Period" << endl;
	cout << "\tSample Mean: " << sample_mean(&exponential,(double)1.0/650) << "ms" << endl;

	cout << endl;

	cout << "Event List test" << endl;
	event_list_test();

	cout << "Queue System test" << endl;
	queue_test();

	cout << endl;

	cout << "Voice Package test" << endl;
	cout << "\tDeterministic Silence Period: " << endl; 
	round_test(10,200,50,0.16,false,true,false);
	
	cout << endl;

	cout << "\tDeterministic Data Traffic: " << endl;
	round_test(10,200,50,0.16,false,true,true);

}