#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <mpi.h>
#include "julia.h"

double x_start_val=-2.01;
double x_end_val=1;
double yupper;
double ylower;

double ycenter=1e-6;
double step;

int *image_part;

// MPI information
int world_size;
int world_rank;

// I suggest you implement these, however you can do fine without them if you'd rather operate
// on your complex number directly.
complex_t square_complex(complex_t c, complex_t t){
	t.real = c.real*c.real - c.imag*c.imag;
	t.imag = 2*c.real*c.imag;
	return t;
}

complex_t add_complex(complex_t a, complex_t b){
	a.real += b.real;
	a.imag += b.imag;
	return a;
}

complex_t add_real(complex_t a, int b){
	a.real += b;
	return a;
}



// add julia_c input arg here?
void calculate(complex_t julia_C, int x_start, int x_end, int y_start, int y_end) {
	for(int i=x_start;i<=x_end;i++) {
		for(int j=y_start;j<=y_end;j++) {

			/* Calculate the number of iterations until divergence for each pixel.
			   If divergence never happens, return MAXITER */
			complex_t c;
      complex_t z;
      complex_t temp;
			int iter=0;

      // find our starting complex number c
			c.real = (x_start_val + step*i);
			c.imag = (ylower + step*j);

      // our starting z is c
			z = c;

      // iterate until we escape
			while(z.real*z.real + z.imag*z.imag < 4) {
        // Each pixel in a julia set is calculated using z_n = (z_n-1)² + C
        // C is provided as user input, so we need to square z and add C until we
        // escape, or until we've reached MAXITER
				z = add_complex(square_complex(z, temp), julia_C);
        // z = z squared + C

				if(++iter==MAXITER) break;
			}
			image_part[PIXEL(i,j - y_start)] = iter;
		}
	}
}


int main(int argc,char **argv) {
	
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	double time_start = MPI_Wtime();
	if(argc!=3) {
		puts("Usage: JULIA\n");
		puts("Input real and imaginary part. ex: ./julia 0.0 -0.8");
		MPI_Finalize();
		return 0;
	}

	char id[7];
	sprintf(id, "%i/%i: ", world_rank, world_size - 1);

	/* Calculate the range in the y-axis such that we preserve the
	   aspect ratio */
	step=(x_end_val-x_start_val)/XSIZE;
	yupper=ycenter+(step*YSIZE)/2;
	ylower=ycenter-(step*YSIZE)/2;

  // Unlike the mandelbrot set where C is the coordinate being iterated, the
  // julia C is the same for all points and can be chosed arbitrarily
  complex_t julia_C;

  // Get the command line args
  julia_C.real = strtod(argv[1], NULL);
  julia_C.imag = strtod(argv[2], NULL);

  int delta_y = YSIZE / world_size;
  int y_start = world_rank * delta_y;
  int y_end;
  if (world_rank == world_size-1) {
	  y_end = YSIZE - 1;
  } else {
	  y_end = y_start + delta_y - 1;
  }
  int x_start = 0;
  int x_end = XSIZE-1;
  
  int image_part_length = delta_y*XSIZE; // We assume the y dimension of the image is divisable by the number of processors
  image_part = malloc(image_part_length*sizeof(int));

  calculate(julia_C, x_start, x_end, y_start, y_end);
  printf("%sI processed X=%i->%i and Y=%i->%i\n", id, x_start, x_end, y_start, y_end);
  
  /* create nice image from iteration counts. take care to create it upside
     down (bmp format) */
  
  int *image;
  if (world_rank == 0) {
	image = malloc(image_part_length*world_size*sizeof(int));
  	printf("%sGathering data...\n", id);
  } else {
  	printf("%sSending data...\n", id);
  }

  MPI_Gather(image_part, image_part_length, MPI_INT, image, image_part_length, MPI_INT, 0, MPI_COMM_WORLD);

  if (world_rank == 0) {
	printf("%sComposing image...\n", id);
	unsigned char *composed_image = calloc(XSIZE*YSIZE*3, 1);
	for (int i = 0; i < XSIZE; i++) {
		for (int j = 0; j < YSIZE; j++) {
			int p = ((YSIZE-j-1)*XSIZE+i)*3;
			fancycolour(composed_image+p, image[PIXEL(i,j)]);
		}
	}
	free(image);
	/* write image to disk */
	printf("%sSaving image...\n", id);
  	savebmp("julia.bmp",composed_image,XSIZE,YSIZE);
	free(composed_image);
  }

  free(image_part);

  double delta_time = MPI_Wtime() - time_start;
  printf("%sProcessing took %lf seconds\n", id, delta_time);
	
  MPI_Finalize();

  return 0;
}
