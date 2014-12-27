#include <cstdlib>
#include <iostream>
#include <mpi.h>

#include "count_sort_par.h"

namespace count {
using namespace std;

void partition (int *vector, int *localVector, int *buckets, int blockSize, 
        int range, int size) {
    MPI_Scatter (vector, blockSize, MPI_INT, localVector, blockSize, MPI_INT,
            0, MPI_COMM_WORLD);
    int num_buckets = range + 1;
    int *localBuckets = new int[num_buckets]();
    for (int i = 0; i < blockSize; i++) {
        ++localBuckets[localVector[i]];
    }
    int procs, rank;

    MPI_Comm_size(MPI_COMM_WORLD, &procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0) {
        if (size % procs != 0) {
            for (int i = blockSize * procs; i < size; i++) {
                ++localBuckets[vector[i]];
            }
        }
    }

    MPI_Allreduce (localBuckets, buckets, num_buckets, MPI_INT, MPI_SUM, MPI_COMM_WORLD); 

    int prefixSum[num_buckets + 1];
    prefixSum[0] = 0;
    for (int i = 0; i < num_buckets; i++) {
        prefixSum[i+1] = prefixSum[i] + buckets[i];
    }
    
    blockSize = num_buckets / procs;
    blockSize = blockSize == 0 ? 1 : blockSize;

    int *local_sort;
    int start = rank * blockSize;
    int end;
    int local_size = 0;
    if (start < num_buckets) {
        end = start + blockSize;
        for (int i = start; i < end; i++) {
            local_size  += buckets[i];
        }
        local_sort = new int[local_size];
        int index = 0;
        for (int i = start; i < end; i++) {
            for (int j = 0; j < buckets[i]; j++) {
                local_sort[index++] = i;
            }
        }
    }

    int offsets[procs];
    int sizes[procs];
    for (int i = 0; i < procs; i++) {
        offsets[i] = 0;
        if (i < num_buckets) {
            offsets[i] = prefixSum[i * blockSize];
        }
        start = i * blockSize;
        end = start + blockSize;
        sizes[i] = 0;
        if (start < num_buckets) {
            for (int j = start; j < end; j++) {
                sizes[i] += buckets[j];
            }
        }
    }

    MPI_Gatherv (local_sort, local_size, MPI_INT, vector, sizes, offsets,
            MPI_INT, 0, MPI_COMM_WORLD);

    if (num_buckets  > procs && num_buckets % procs != 0  && rank == 0) {
        start = procs * blockSize;
        end = num_buckets;
        for (int i = start; i < end; i++) {
            int offset = prefixSum[i];
            for (int j = 0; j < buckets[i]; j++) {
                vector[offset + j] = i;
            }
        }
    }
}

void countSort_par (int* vector, int size, int range) {
    struct timespec begin, end;
    double elapsed;
    int procs, rank;

    MPI_Comm_size(MPI_COMM_WORLD, &procs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int block = size / procs;
    int *localVector = new int[block];
    int *buckets = new int[range+1];
    
    partition (vector, localVector, buckets, block, range, size);
    //for (int i = 0; i <= range; i++) {
    //    cout << buckets[i] << " ";
    //}
    //cout << endl;
}
};
