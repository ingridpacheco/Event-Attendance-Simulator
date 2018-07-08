#include "statistics.cpp"
#ifndef STDIO
#include <stdio.h>
#define STDIO
#endif
#ifndef STDLIB
#include <stdlib.h>
#define STDLIB
#endif
#ifndef MATH
#include <math.h>
#define MATH
#endif

// Function that determines the size of a data package (actual function)
int data_package_size() {
	long double seed = rand1();
	//printf("Seed is %Lf\n", seed);
	if (seed < .3) return 64;
	else if (seed < .3 + .1) return 512;
	else if (seed < .3 + .1 + .3) return 1500;
	else {
		seed = (seed - .3 - .1 - .3) / (1 - .3 - .1 - .3);
		return seed * (1500 - 64) + 64;
	}
}

// Function that determines the number of voice packages (actual function)
int voice_package_number() {
	long double seed = (long double) rand1();
	if (seed == 1) seed = .999999; // seed cannot be 1 or there would be a log(0), which is infinite, so... we make it a little smaller
	return (int) ceil(log(1.0 - seed) / log(1 - 1.0L / 22.0));
}

// This function only exists to make sure that the percentages are correct
void test_sample_mean(int (*function)()) {
	int current_sample;
	int n64 = 0;
	int n512 = 0;
	int n1500 = 0;
	for (int i = 0; i < BIGNUM; i++) {
		current_sample = (*function)();
		if (current_sample == 64) n64++;
		else if (current_sample == 512) n512++;
		else if (current_sample == 1500) n1500++;
	}
	printf("\t\t64: %lf%%\n\t\t512: %lf%%\n\t\t1500: %lf%%\n", (double)n64/BIGNUM * 100, (double)n512/BIGNUM * 100, (double)n1500/BIGNUM * 100);
}

int main() {
	unsigned int random;
	printf("Random Seed: ");
	scanf("%int", &random);
	srand(random);
	
	putchar('\n');
	
	printf("Data Package Size\n");
	printf("\tSample Mean: %lf\n", sample_mean(&data_package_size));
	test_sample_mean(&data_package_size);
	
	putchar('\n');
	
	printf("Voice Package Quantity\n");
	printf("\tSample Mean: %lf\n", sample_mean(&voice_package_number));
}