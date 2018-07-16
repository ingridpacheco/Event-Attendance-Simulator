#include "run_queues.cpp"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
using namespace std;

int main(){
	unsigned int seed;
    int transiente_period, customers_number, round_number;
    float utilization1;
    bool preemption, allow_logging;
	char input;
    // Gets the seed used for generation pseudo-random values
    cout << "Random seed: ";
    cin >> seed;
    // User chooses whether there will be preemption or not
    cout << "Preemption? (y/n): ";
    cin >> input;
	preemption = (input == 'y' || input == 'Y');
    // User chooses whether there will be data about the simulation stored in disk or not
    cout << "Allow logging? (y/n): ";
    cin >> input;
	allow_logging = (input == 'y' || input == 'Y');
    // It gets the transient period, the customers number, the rounds number and the utilization
	srand(seed);
    cout << "Transiente Period: ";
    cin >> transiente_period;
    cout << "Customers Number: ";
    cin >> customers_number;
    cout << "Round Number: ";
    cin >> round_number;
    cout << "Utilization 1: ";
    cin >> utilization1;

    // If the number is out of the limits, it doesnt execute the simulation
    if(transiente_period <= 0 || customers_number <= 0 || round_number <= 0 || !(0.1 <= utilization1 && utilization1 <= 0.7))
        cout << "This isn't a valid value. Please, choose a value greater than 0 and between 0.1 and 0.7 for utilization.\n";
    else
        execution(transiente_period, customers_number, round_number, utilization1, preemption, allow_logging);

    return 0;
}