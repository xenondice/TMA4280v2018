#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <string.h>

double approx_arctan(double x, int n) {
	double sn = 0;
	for (double i = 1.0; i <= n; i += 1.0) {
		double vi = pow(-1.0, i - 1.0)* pow(x, 2*i - 1.0) / (2*i - 1.0);
		sn += vi;
	}
	return sn;
}

double approx_pi(int n) {
	return 4*(4*approx_arctan(1.0/5.0, n) - approx_arctan(1.0/239.0, n));
}

void unit_test_n3() {
	double expected = 3.1416210293250346;
	double pi = approx_pi(3);
	char *ans;
	if (expected == pi)
		ans = "success";
	else
		ans = "failure";
	printf("%s\n", ans);
}

void verification_test() {
	for (int k = 1; k <= 24; k++) {
		double n = pow(2,k);
		double pi = approx_pi(n);
		double error = fabs(M_PI - pi);
		printf("Error n = 2^%i: %.*g\n", k, DBL_DECIMAL_DIG, error);
	}
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("No argument 'n' provided!\n");
		return 1;
	} else if (strcmp(argv[1], "utest") == 0) {
		unit_test_n3();
		return 0;
	} else if (strcmp(argv[1], "vtest") == 0) {
		verification_test();
		return 0;
	}

	int n = atoi(argv[1]);
	double pi = approx_pi(n);

	printf("%.*g\n", DBL_DECIMAL_DIG, pi);
	
	return 0;
}
