#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

double approx_arctan(double x, int n) {
	double sn = 0;
	for (double i = 1.0; i <= n; i += 1.0) {
		double vi = pow(-1.0, i - 1.0)* pow(x, 2*i - 1.0) / (2*i - 1.0);
		sn += vi;
	}
	return sn;
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("No argument 'n' provided!\n");
		return 1;
	}

	int n = atoi(argv[1]);

	double approx_pi = 4*(4*approx_arctan(1.0/5.0, n) - approx_arctan(1.0/239.0, n));
	printf("%.*g\n", DBL_DECIMAL_DIG, approx_pi);
	
	return 0;
}
