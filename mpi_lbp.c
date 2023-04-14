#include <mpi.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lbp.c"

int block_low(int rank, int nprocs, int nelems) {
    return rank * nelems / nprocs;
}

int block_high(int rank, int nprocs, int nelems) {
    return block_low(rank + 1, nprocs, nelems) - 1;
}

int block_size(int rank, int nprocs, int nelems) {
    return block_low(rank + 1, nprocs, nelems) - 
    	   block_low(rank, nprocs, nelems);
}

int block_owner(int index, int nprocs, int nelems) {
    return (nprocs * (index + 1) - 1) / nelems;
}

int main(int argc, char *argv[]) {
	int rank;
	int nprocs;

	int root = 0;
	MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    argc--;
    argv++;
    int width, height, bpp;
    unsigned char* data = stbi_load(*argv, &width, &height, &bpp, GRAY);

    argv++;
    int NTHREADS = *argv ? atoi(*argv) : omp_get_num_procs();
    omp_set_num_threads(NTHREADS);

	int radius = 1;
    int points = 8;
	Point_s *window = coordinates(radius, points);
    
    int *offset = malloc(nprocs * sizeof(int));
    int *counts = malloc(nprocs * sizeof(int));

    // calculate send counts and displacements
    for (int i = 0; i < nprocs; i++) {
    	counts[i] = block_size(i, nprocs, width) * height;
        offset[i] = block_low(i, nprocs, width) * height;
    }

 	unsigned char *rec_buf;
 	for (int i = 0; i < nprocs; ++i) {
 		rec_buf = malloc(counts[i] * sizeof(unsigned char));
 	}
    
    int sztable = (int) pow(2, points);
	int *histogram = calloc(sztable, sizeof(int));
    int *lcl_histogram = calloc(sztable, sizeof(int));

    MPI_Scatterv(data, counts, offset, MPI_UNSIGNED_CHAR, 
    			 rec_buf, counts[rank], MPI_UNSIGNED_CHAR, 
    			 root, MPI_COMM_WORLD);
    
    MPI_Barrier(MPI_COMM_WORLD);
	double time_feature = -MPI_Wtime();
    #pragma omp parallel for schedule(dynamic, height)
    for (int i = 0; i < counts[rank]; ++i) {
    	int lcl_width = counts[rank] / height;
        int row = i / lcl_width;
        int col = i % lcl_width;
        if ((row < radius) || (row >= height - radius) || 
            (col < radius) || (col >= lcl_width - radius))
            continue;
        int *idxs = neighbor_idxs(window, points, lcl_width, i);
        int value = lbp_value(rec_buf, idxs, points, i);
        free(idxs);
        #pragma omp atomic //update
        lcl_histogram[value]++;
    }
    MPI_Reduce(lcl_histogram, histogram, sztable, 
    		   MPI_INT, MPI_SUM, root, MPI_COMM_WORLD);
	
	MPI_Barrier(MPI_COMM_WORLD);
    time_feature += MPI_Wtime();
    if (rank == root) {
        printf("%g", time_feature);
    }

    free(lcl_histogram);
    free(histogram);
    free(data);
    free(rec_buf);
    free(counts);
    free(offset);
	MPI_Finalize();
}
