#include "Customer.h"

int Customer::totalCustomers = 0;

Customer::Customer(customer_type type, double arrival_time) {
	this->id = totalCustomers++;
	this->type = type;
	if (type == DATA) this->size = data_package_size() * 8;
	else if (type == VOICE) this->size = 512;
	this->service_time = (double) this->size / 2097152;
	this->queue_time = 0;
	this->remaining_time = this->service_time;
	this->arrival_time = arrival_time;
}

Customer::Customer(int id, customer_type type, double arrival_time) {
	this->id = id;
	this->type = type;
	if (type == DATA) this->size = data_package_size() * 8;
	else if (type == VOICE) this->size = 512;
	this->service_time = (double) this->size / 2097152;
	this->queue_time = 0;
	this->remaining_time = this->service_time;
	this->arrival_time = arrival_time;
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