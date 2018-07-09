#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <time.h>
using namespace std;

class Queue{
    private:
        int v;
        Queue* prox;
    public:
        Queue(int v) // construtor
        {
            this->v = v;
            this->prox = NULL;
        }

        int obterValor() // obtém o valor
        {
            return v;
        }

        Queue* obterProx() // obtém o próximo No
        {
            return prox;
        }

        void setProx(Queue* p) // seta o próximo No
        {
            prox = p;
        }
};

void execution(int transientPeriod, int customersNumber, int roundNumber, float utilization1){
    //The service 1 average time is going to be the package size average divided by the transmission rate
    float serviceAverage1 = (float) (755 * 8) / (float) (2048);
    float lambda = utilization1 / serviceAverage1;
    //get time that starts
    rounds(transientPeriod, customersNumber, roundNumber, serviceAverage1, lambda);
    //get time that ends
}

void rounds(int transientPeriod, int customersNumber, int roundNumber, float serviceAverage1, float lambda){
    Queue* dataTraffic;
    Queue* voiceTraffic;

    //Esperanças
    float N1q[customersNumber];
    float N2q[customersNumber];
    float Ns1[customersNumber];
	float N1[customersNumber];
	float Ns2[customersNumber];
	float N2[customersNumber];
	float W1[customersNumber];
	float X1[customersNumber];
	float T1[customersNumber];
	float W2[customersNumber];
	float X2[customersNumber];
	float T2[customersNumber];

    float *W1 = (float*) malloc(sizeof(float));
    float *W2 = (float*) malloc(sizeof(float));

    //People that came out of the system coming from both Queues;
    int out1 = 0; 
    int out2 = 0;

    int voiceArrival = 16;
    int silenceTimeAvg = 560;
}