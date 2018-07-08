#include "statistics.cpp"
#ifndef STDIO
#include <stdio.h>
#define STDIO
#endif
#ifndef STDLIB
#include <stdlib.h>
#define STDLIB
#endif

// Function that determines the size of a data package (actual function)
int data_transfer() {
	double seed = (double) rand() / RAND_MAX;
	if (seed < .3) return 64;
	else if (seed < .3 + .1) return 512;
	else if (seed < .3 + .1 + .3) return 1500;
	else {
		seed = (seed - .3 - .1 - .3) / (1 - .3 - .1 - .3);
		return seed * (1500 - 64) + 64;
	}
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
	printf("64: %lf%%\n512: %lf%%\n1500: %lf%%\n", (double)n64/BIGNUM, (double)n512/BIGNUM, (double)n1500/BIGNUM);
}

int main() {
	unsigned int random;
	printf("Random Seed: ");
	scanf("%int", &random);
	srand(random);
	
	printf("\nSample Mean: ");
	
	printf("%lf\n", sample_mean(&data_transfer));
	
	test_sample_mean(&data_transfer);
}