#include "data_structures.cpp"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <time.h>
using namespace std;

void rounds(int transientPeriod, int customersNumber, int roundNumber, float serviceAverage1, float lambda){
    queue* data_traffic = queue_create(); // Queue where the data packages are stored
    queue* voice_traffic = queue_create(); // Queue where the voice packages are stored
	
	float simulation_time = 0; // current time in the simulator
	
	// Since we must ignore the first transientPeriod customers, only customers considered will be the ones with id >= 0
	// and we'll start counting from -transientPeriod
	Customer::totalCustomers = -transientPeriod; 
	
	Customer customer_being_served = Customer(NONE, 0); // The customer currently in the server. NONE type = no customer there.

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

    //People that came out of the system coming from both Queues;
    int out1 = 0; // data packages
    int out2 = 0; // voice packages
	int out = 0; // out1 + out2
	
    int voiceArrival = 16;
    int silenceTimeAvg = 560;
}

void execution(int transientPeriod, int customersNumber, int roundNumber, float utilization1){
    //The service 1 average time is going to be the package size average divided by the transmission rate
    float serviceAverage1 = (float) (755 * 8) / (float) (2048);
    float lambda = utilization1 / serviceAverage1;
    //get time that starts
    rounds(transientPeriod, customersNumber, roundNumber, serviceAverage1, lambda);
    //get time that ends
}
