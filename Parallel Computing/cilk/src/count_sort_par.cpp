#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <vector>

#include "count_sort_par.h"

namespace count {
using namespace std;

void partition (int *v, int *bucketContainer, int range, int start, int end) {
    int *tmp = new int[range+1]();
    // for Saturn
    //int tmp[range+1] = {0};
    for (int i = start; i < end; i++) {
        ++tmp[v[i]];
    }
    for (int i = 0; i < R+1; i++) {
        // add
    }
}

void sortBucket(int *v, int *bucketContainer, int *prefixSum, int index) {
    int start = prefixSum[index];
    int end = start + bucketContainer[index];
    //v[start:end:1] = index;
}

void countSort_par (int* vector, int size, int range, int num_cores) {
    int num_buckets = range + 1;
    int *bucketContainer = new int[num_buckets];
    int block = size / num_cores;

    cilk_for (int i = 0; i < num_cores; i++) {
        partition(vector, bucketContainer, range, i * block, (i+1) * block);
    }

    if (size % num_cores != 0) {
        partition(vector, bucketContainer, range, num_cores * block, size);
    }

    // sync 

    // Sort the buckets and place the values in the initial vector.
    int prefixSum[num_buckets + 1];
    prefixSum[0] = 0;
    for (int i = 0; i < num_buckets; i++) {
        prefixSum[i + 1] = bucketContainer->buckets[i] + prefixSum[i];
    }

    cilk_for (int i = 0; i < num_buckets; i++) {
        if (bucketContainer[i] != 0) {
            sort (vector, bucketContainer, prefixSum, i);
        }
    }
    //sync
}
};
