#include "run_queues.cpp"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
using namespace std;

int main(){
    int transientPeriod, customersNumber, roundNumber;
    float utilization1;
    cout << "Período Transiente: ";
    cin >> transientPeriod;
    cout << "Número de fregueses: ";
    cin >> customersNumber;
    cout << "Número de rodadas: ";
    cin >> roundNumber;
    cout << "Utilização1: ";
    cin >> utilization1;

    if(transientPeriod <= 0 || customersNumber <= 0 || roundNumber <= 0 || !(0.1 <= utilization1 && utilization1 <= 0.7))
        cout << "Esse valor não é válido, todos os valores devem ser maior que 0 e a utilização deve estar entre 0.1 e 0.7\n";
    else
        execution(transientPeriod, customersNumber, roundNumber, utilization1);

    return 0;
}