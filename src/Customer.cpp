#include "Customer.h"

// Sets the total of customers to zero
int Customer::total_customers = 0;

/*
// Defines a Customer without identification
Customer::Customer(customer_type type, double arrival_time) {
	this->id = total_customers++;
	this->type = type;
	if (type == DATA) this->size = data_package_size() * 8;
	else if (type == VOICE) this->size = 512;
	this->service_time = (double) this->size / 2097152;
	this->time_in_queue = 0;
	this->remaining_time = this->service_time;
	this->arrival_time = arrival_time;
}
*/

Customer::Customer() {
	this->id = -99999;
	this->round = -99999;
	this->type = NONE;
	this->size = 0;
	this->service_time = 0;
	this->time_in_queue = 0;
	this->time_in_service = 0;
	this->arrival_time = 0;
	this->checkpoint_time = 0;
}

// Defines a Customer with identification
Customer::Customer(int id, customer_type type, double arrival_time, int channel_id) {
	this->id = id;
	this->round = -99999;
	this->type = type;
	if (type == DATA) this->size = data_package_size() * 8;
	else if (type == VOICE) this->size = 512;
	this->service_time = (double) this->size / (0.002 * 1024 * 1024);
	this->time_in_queue = 0;
	this->time_in_service = 0;
	this->arrival_time = arrival_time;
	this->checkpoint_time = arrival_time;
	this->channel_id = channel_id;
}

// Function that determines the size of a data package in bytes (actual function)
int data_package_size() {
	long double seed = rand1();
	if (seed < .3) return 64;
	else if (seed < .3 + .1) return 512;
	else if (seed < .3 + .1 + .3) return 1500;
	else {
		seed = (seed - .3 - .1 - .3) / (1 - .3 - .1 - .3);
		return seed * (1500 - 64) + 64;
	}
}