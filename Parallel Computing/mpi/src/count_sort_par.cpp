#include <cstdlib>
#include <iostream>
#include <mpi.h>

#include "count_sort_par.h"

namespace count {
using namespace std;

void partition (int *vector, int *localVector, int *buckets, int blockSize, int range) {
    MPI_Scatter (vector, blockSize, MPI_INT, localVector, blockSize, MPI_INT, 0, MPI_COMM_WORLD);
    int *localBuckets = new int[range+1]();
    // for Saturn
    //int tmp[range+1] = {0};
    for (int i = 0; i < blockSize; i++) {
        ++localBuckets[localVector[i]];
    }
    MPI_Allreduce (localBuckets, buckets, range + 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD); 
}

void sortBucket(int *v, int *bucketContainer, int *prefixSum, int index, int end) {
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
    
    partition (vector, localVector, buckets, block, range);
    if (rank == 0) {
        if (size % procs != 0) {
            for (int i = block * procs; i < size; i++) {
                ++buckets[vector[i]];
            }
        }
        for (int i = 0; i <= range; i++) {
            cout << buckets[i] << " ";
        }
        cout << endl;
    }
}
};
