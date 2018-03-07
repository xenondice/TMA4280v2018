#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <string.h>

double approx_pi(int n) {	
	double sn = 0;
	for (int i = 1; i <= n; i++)
		sn += 1.0/(i*i);
	return sqrt(sn*6);
}

void unit_test_n3() {
	double expected = 2.8577380332470415;
	double pi = approx_pi(3);
	char *ans;
	if (expected == pi)
		ans = "success";
	else
	        ans = "failure";
	printf("%s\n", ans);
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("No argument 'n' provided!\n");
		return 1;
	} else if (strcmp(argv[1], "utest") == 0) {
		unit_test_n3();
		return 0;
	}

	int n = atoi(argv[1]);
	double pi = approx_pi(n);
	printf("%.*g\n", DBL_DECIMAL_DIG, pi);
	
	return 0;
}
