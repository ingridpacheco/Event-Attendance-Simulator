#include "statistics.h"
#ifndef STDLIB
#include <stdlib.h>
#define STDLIB
#endif
#ifndef MATH
#include <math.h>
#define MATH
#endif

// Number used for large sample sizes
#define BIGNUM 2000000

// Function that generates a random number between 0 and 1
long double rand1() {
	return (long double) rand() / (long double) RAND_MAX;
}

// Sample Mean function:
// Gives the mean of a sample of BIGNUM random values generated by the given function.

// Discrete variable with no parameter
double sample_mean(int (*function)()) {
	double ret = 0;
	int current_sample;
	for (int i = 0; i < BIGNUM; i++) {
		current_sample = (*function)();
		ret += current_sample;
	}
	return ret / BIGNUM;
}

// Discrete variable with integer parameter
double sample_mean(int (*function)(int), int i) {
	double ret = 0;
	int current_sample;
	for (int i = 0; i < BIGNUM; i++) {
		current_sample = (*function)(i);
		ret += current_sample;
	}
	return ret / BIGNUM;
}

// Continuous variable with no parameter
double sample_mean(double (*function)()) {
	double ret = 0;
	double current_sample;
	for (int i = 0; i < BIGNUM; i++) {
		current_sample = (*function)();
		ret += current_sample;
	}
	return ret / BIGNUM;
}

// Continuous variable with floating point parameter
long double sample_mean(double (*function)(double), double d) {
	long double ret = 0;
	double current_sample;
	for (int i = 0; i < BIGNUM; i++) {
		current_sample = (*function)(d);
		ret += current_sample;
	}
	return ret / BIGNUM;
}

// Generates a random number with an exponential distribution
double exponential(double rate) {
	long double seed = rand1();
	if (seed < .000001) seed = .000001; // seed cannot be 0 or there would be a log(0), which is infinite, so... we make it a little smaller
	return -log(seed) / rate;
}