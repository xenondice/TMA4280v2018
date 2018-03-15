#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include <mpi.h>

int nprocs, rank;

double approx_pi(int n) {
	double partial_sn = 0;
	int proc_part_size = n/nprocs;
	int active = 1;
	if (n < nprocs) {
		if (rank >= n) {
			printf("Process %i not used!\n", rank);
			active = 0;
		} else {
			proc_part_size = 1;
		}
	}

	if (active) {
		int proc_start_i = proc_part_size*rank+1;
		int proc_end_i;
		if (rank == nprocs - 1)
			proc_end_i = n;
		else
			proc_end_i = proc_start_i + proc_part_size - 1;
		for (double i = proc_start_i; i <= proc_end_i; i++)
			partial_sn += 1.0/(i*i);
	}

	double result;
	MPI_Reduce(&partial_sn, &result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	if (rank == 0)
		return sqrt(result*6);
	else
		return -1;
}

void unit_test_n3() {
	double expected = 2.8577380332470415;
	double pi = approx_pi(3);
	if (rank != 0) return;
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
		if (rank != 0) continue;
		double error = fabs(M_PI - pi);
		printf("Error n = 2^%i: %.*g\n", k, DBL_DECIMAL_DIG, error);
	}
}

int is_not_power_of_2(int val) {
	return (val & (val - 1));
}

int main(int argc, char *argv[]) {
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	if (argc < 2) {
		if (rank == 0)
			printf("No argument 'n' provided!\n");
		return 1;
	}

	if (is_not_power_of_2(nprocs)) {
		if (rank == 0)
			printf("Number of processes not power of 2!\n");
		return 1;
	} else if (strcmp(argv[1], "utest") == 0) {
		unit_test_n3();
	} else if (strcmp(argv[1], "vtest") == 0) {
		verification_test();
	} else {
		double time_start = 0;
		if (rank == 0) {
			time_start = MPI_Wtime();
		}
		int n = atoi(argv[1]);	
		double pi = approx_pi(n);
		if (rank == 0)
			printf("PI = %.*g\nTime used: %gs\nError: %g\n",
					DBL_DECIMAL_DIG,
					pi,
					MPI_Wtime() - time_start,
					fabs(M_PI - pi));
	}
	
	MPI_Finalize();
	return 0;
}
