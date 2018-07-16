#include "run_queues.cpp"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
using namespace std;

int main(){
	unsigned int seed;
    int transientPeriod, customersNumber, roundNumber;
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
    cout << "Periodo Transiente: ";
    cin >> transientPeriod;
    cout << "Numero de fregueses: ";
    cin >> customersNumber;
    cout << "Numero de rodadas: ";
    cin >> roundNumber;
    cout << "Utilizacao1: ";
    cin >> utilization1;

    // If the number is out of the limits, it doesnt execute the simulation
    if(transientPeriod < 0 || customersNumber <= 0 || roundNumber <= 0 || !(0.1 <= utilization1 && utilization1 <= 0.7))
        cout << "Esse valor não é válido, todos os valores devem ser maior que 0 e a utilização deve estar entre 0.1 e 0.7\n";
    else
        execution(seed, transientPeriod, customersNumber, roundNumber, utilization1, preemption, allow_logging);

    return 0;
}