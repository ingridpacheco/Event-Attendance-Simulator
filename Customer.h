#ifndef CUSTOMER_H
#define CUSTOMER_H

#ifndef STATISTICS_H
#include "statistics.cpp"
#endif

// Defines the type of the customer
typedef enum {
	NONE, // "phantom" customer, returned when dequeueing an empty queue
	DATA,
	VOICE
} customer_type;

class Customer {
	public:
		int id;
		int round; // the round in which this customer joined the queue
		double arrival_time; // when this customer joined the queue
		double checkpoint_time; // used to know when was the last time this customer was sent to the queue/server
		customer_type type;
		static int totalCustomers; // used for giving the next customer a new id
		double service_time; // how long it takes the server to serve this customer
		double time_in_service; // X1
		double time_in_queue; // W1
		int size; // in bits
		int channel_id; // the id of the voice channel related to the customer
		// Customer(customer_type type, double arrival_time);
		Customer();
		Customer(int id, int round, customer_type type, double arrival_time, int channel_id = -1);
};

// Function that determines the size of a data package in bytes (actual function)
int data_package_size();

#endif