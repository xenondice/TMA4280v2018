#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <string.h>
#include <mpi.h>

int nprocs, rank;

double arctan_vi(double x, double i) {

	// Calculate one element in the arctan approximation
	return pow(-1.0, i - 1.0) * pow(x, 2*i - 1.0) / (2*i - 1.0);
}

double approx_pi(int n) {

	// Create a 2D vector to hold the partial sums
	double partial_sn[2] = {0, 0};

	// Get how many values this process should process
	int proc_part_size = n/nprocs;

	// If n is less than the number of processes,
	// set this process as inactive if rank is bigger than or equal n
	// considering there aren't enough elements for all the processes
	// to be working
	int active = 1;
	if (n < nprocs) {
		if (rank >= n) {
			printf("Process %i not used!\n", rank);
			active = 0;
		} else {
			// Every active process should then process
			// one element each
			proc_part_size = 1;
		}
	}

	// Skip processing if inactive
	if (active) {

		// Use the rank to find which part of the vector
		// to process
		int proc_start_i = proc_part_size*rank+1;
		int proc_end_i;
		if (rank == nprocs - 1) // If the last process, do the rest of the elements
			proc_end_i = n;
		else
			proc_end_i = proc_start_i + proc_part_size - 1;
		
		// Iterate over each of this process' elements and compute
		// both the arctan approcimation sums we will need
		// and add to their respective sum components
		for (double i = proc_start_i; i <= proc_end_i; i++) {
			partial_sn[0] += arctan_vi(1.0/5.0, i);
			partial_sn[1] += arctan_vi(1.0/239.0, i);
		}
	}

	// Now sum all the partial sums into two sums at process 0
	// process 0 can now solve for π, the rest returns -1
	double result[2];
	MPI_Reduce(partial_sn, result, 2, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	if (rank == 0)
		// result[0] equals arctan(1/5) while result[1] equals arctan(1/239)
		// we have π/4 = 4*arctan(1/5) - arctan(1/239)
		return 4*(4*result[0] - result[1]);
	else
		return -1;
}

void unit_test_n3() {

	// Get the correct value from serial program
	double expected = 3.1416210293250346;

	// Approximate with n = 3
	double pi = approx_pi(3);

	// Only process 0 gets to print result
	if (rank != 0) return;

	// Check if correct and print
	char *ans;
	if (expected == pi)
		ans = "success";
	else
	        ans = "failure";
	printf("%s\n", ans);
}

void verification_test() {

	// Iterate through the 24 powers of 2 for
	// a exponential distribution
	for (int k = 1; k <= 24; k++) {

		// Get n for this power
		double n = pow(2,k);

		// Approximate for this n
		double pi = approx_pi(n);

		// Only print result if process 0
		if (rank != 0) continue;

		// Print error for this n
		double error = fabs(M_PI - pi);
		printf("Error n = 2^%i: %.*g\n", k, DBL_DECIMAL_DIG, error);
	}
}

int is_not_power_of_2(int val) {

	// Magic bit-wise operation, if 0 then power of two
	return (val & (val - 1));
}

int main(int argc, char *argv[]) {

	// Setup mpi
	MPI_Init(&argc, &argv);

	// Get rank and number of processes
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	// Quit if no arguments are provided,
	// but only print help text once
	if (argc < 2) {
		if (rank == 0)
			printf("No argument 'n' provided!\n");
		return 1;
	}

	// Quit if the number of processes is not power of two
	if (is_not_power_of_2(nprocs)) {
		if (rank == 0)
			printf("Number of processes not power of 2!\n");
		return 1;
	
	// Enter different modes depending on the given arguments
	} else if (strcmp(argv[1], "utest") == 0) {
		unit_test_n3();
	} else if (strcmp(argv[1], "vtest") == 0) {
		verification_test();
	} else {

		// Let process 0 get the time
		double time_start = 0;
		if (rank == 0) {
			time_start = MPI_Wtime();
		}

		// Convert argument to integer
		int n = atoi(argv[1]);

		// Run parallel calculation
		// Only process 0 gets the correct value
		double pi = approx_pi(n);

		// Let only process 0 print the full value, time used and error
		if (rank == 0)
			printf("PI = %.*g\nTime used: %gs\nError: %g\n",
					DBL_DECIMAL_DIG,
					pi,
					MPI_Wtime() - time_start,
					fabs(M_PI - pi));
	}
	
	// Exit MPI
	MPI_Finalize();
	return 0;
}
