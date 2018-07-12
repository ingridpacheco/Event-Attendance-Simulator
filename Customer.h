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
		double arrival_time; // when this customer joined the queue
		customer_type type;
		static int totalCustomers; // used for giving the next customer a new id
		double service_time; // X1
		double remaining_time; // X1r
		double queue_time; // W1
		int size; // in bits
		// Customer(customer_type type, double arrival_time);
		Customer(int id, customer_type type, double arrival_time);
};

// Function that determines the size of a data package in bytes (actual function)
int data_package_size();

#endif