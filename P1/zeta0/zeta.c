#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("No argument 'n' provided!\n");
		return 1;
	}

	int n = atoi(argv[1]);
	double sn = 0;
	for (double i = 1.0; i <= n; i += 1.0) {
		double vi = 1.0/(i*i);
		sn += vi;
	}

	double approx_pi = sqrt(sn*6);
	printf("%.*g\n", DBL_DECIMAL_DIG, approx_pi);
	
	return 0;
}
