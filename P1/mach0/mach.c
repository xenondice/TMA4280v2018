#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <string.h>

double approx_arctan(double x, int n) {

	// Create a variable for holding the sum
	double sn = 0;

	// Iterate over each vi element
	// number of elements specified by 'n'
	for (double i = 1.0; i <= n; i++) {

		// We are interested in the sum of the elements,
		// so we add the new element directly to the total sum
		sn += pow(-1.0, i - 1.0) * pow(x, 2*i - 1.0) / (2*i - 1.0);
	}

	// This sum equals the arctan(x) function, so we can just return it
	return sn;
}

double approx_pi(int n) {

	// We have π/4 = 4*arctan(1/5)-arctan(1/239)
	// To get π, we need to approximate arctan with n elements
	// and multiply both sides by four
	return 4*(4*approx_arctan(1.0/5.0, n) - approx_arctan(1.0/239.0, n));
}

void unit_test_n3() {

	// We get a reference value by executing the program once it works correctly
	// n is set to 3
	double expected = 3.1416210293250346;

	// We then execute the approximation
	double pi = approx_pi(3);

	// Now we check whether these values are the same
	// we have variable to hold the result
	char *ans;
	if (expected == pi)
		ans = "success";
	else
	    ans = "failure";

	// We then print the result of the test
	printf("%s\n", ans);
}

void verification_test() {

	// We want to run a exponential test with 24 steps
	for (int k = 1; k <= 24; k++) {

		// We calculate the n value as 2^k
		double n = pow(2,k);

		// We approximate pi with the chosen method and n
		double pi = approx_pi(n);

		// We calculate the error by using math.h's pi,
		// which is 100% accurate for double precision,
		// by getting the absolute difference
		double error = fabs(M_PI - pi);

		// We print every decimal of the resulting error
		// The makefile can then store the output to a file
		printf("Error n = 2^%i: %.*g\n", k, DBL_DECIMAL_DIG, error);
	}
}

int main(int argc, char *argv[]) {

	// Stop if no arguments are provided
	if (argc < 2) {
		printf("No argument 'n' provided!\n");
		return 1;

	// Enter a special mode if argument 'vtest' or 'utest' is provided
	} else if (strcmp(argv[1], "utest") == 0) {
		unit_test_n3();
		return 0;
	} else if (strcmp(argv[1], "vtest") == 0) {
		verification_test();
		return 0;
	}

	// Convert argument to an integer
	int n = atoi(argv[1]);

	// Execute the approximation function
	double pi = approx_pi(n);

	// Display the entire double precision approximation
	printf("%.*g\n", DBL_DECIMAL_DIG, pi);
	
	return 0;
}
