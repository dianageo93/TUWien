#include <cilk/cilk.h>
#include <cilk/reducer_opadd.h>
#include <cmath>
#include <cstdlib>
#include <iostream>

#include "count_sort_par.h"

namespace count {
using namespace std;

typedef cilk::reducer< cilk::op_add<int> > add_reducer;

void partition (int *v, add_reducer *bucketContainer, int range, int start, int end) {
    int *tmp = new int[range+1]();
    // for Saturn
    //int tmp[range+1] = {0};
    #pragma simd
    for (int i = start; i < end; i++) {
        ++tmp[v[i]];
    }
    #pragma simd
    for (int i = 0; i < range + 1; i++) {
        *bucketContainer[i] += tmp[i];
    }
}

void sortBucket(int *v, int *bucketContainer, int *prefixSum, int index, int end) {
    for (int j = index; j < end; j++) {
        int start = prefixSum[j];
        int end = start + bucketContainer[j];
        //v[start:end:1] = index;
        #pragma simd
        for (int i = start; i < end; i++) {
            v[i] = index;
        }
    }
}

void countSort_par (int* vector, int size, int range, int num_cores) {
    struct timespec begin, end;
    double elapsed;

    int num_buckets = range + 1;
    add_reducer *bucketContainer = new add_reducer[num_buckets];
    int block = size / num_cores;


    clock_gettime (CLOCK_MONOTONIC, &begin);
    for (int i = 0; i < num_cores; i++) {
        cilk_spawn partition(vector, bucketContainer, range, i * block, (i+1) * block);
    }

    if (size % num_cores != 0) {
        partition(vector, bucketContainer, range, num_cores * block, size);
    }

    cilk_sync;

    clock_gettime (CLOCK_MONOTONIC, &end);
    elapsed = end.tv_sec - begin.tv_sec;
    elapsed += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;
    cout << "Partition time: " << elapsed << endl;

    // Sort the buckets and place the values in the initial vector.

    clock_gettime (CLOCK_MONOTONIC, &begin);
    int prefixSum[num_buckets + 1];
    int bucketContInt[num_buckets];
    prefixSum[0] = 0;
    for (int i = 0; i < num_buckets; i++) {
        bucketContInt[i] = bucketContainer[i].get_value();
        prefixSum[i + 1] = bucketContInt[i] + prefixSum[i];
    }

    clock_gettime (CLOCK_MONOTONIC, &end);
    elapsed = end.tv_sec - begin.tv_sec;
    elapsed += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;
    cout << "Prefixsum time: " << elapsed << endl;

    clock_gettime (CLOCK_MONOTONIC, &begin);
    int sort_block = num_buckets / num_cores;
    for (int i = 0; i < num_cores; i++) {
        cilk_spawn sortBucket (vector, bucketContInt, prefixSum, i * sort_block, (i + 1) * sort_block);
    }
    cilk_sync;
    if (num_buckets % num_cores != 0) {
        sortBucket (vector, bucketContInt, prefixSum, num_cores * sort_block, num_buckets);
    }

    clock_gettime (CLOCK_MONOTONIC, &end);
    elapsed = end.tv_sec - begin.tv_sec;
    elapsed += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;
    cout << "Sort time: " << elapsed << endl;

}
};
