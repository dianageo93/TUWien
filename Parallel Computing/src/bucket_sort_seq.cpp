#include <algorithm>
#include <cstdlib>
#include <vector>

#include "bucket_sort_seq.h"

using std::vector;
using std::sort;

int getMinVector (int* vector, int size) {
    int min = vector[0];
    for (int i = 0; i < size; i++) {
        min = vector[i] < min ? vector[i] : min;
    }
    return min;
}

int getMaxVector (int* vector, int size) {
    int max = vector[0];
    for (int i = 0; i < size; i++) {
        max = vector[i] > max ? vector[i] : max;
    }
    return max;
}

void bucketSort (int* vector, int size, int range, int num_of_buckets) {
    std::vector<std::vector<int> > buckets(num_of_buckets, std::vector<int>());
    int min = getMinVector (vector, size);
    int max = getMaxVector (vector, size);
    float delta = max - min;

    // Place values in their buckets.
    for (int i = 0; i < size; i++) {
        int bucket = (vector[i] - min) / delta * (num_of_buckets - 1);
        buckets[bucket].push_back (vector[i]);
    }

    // Sort buckets and replace the values in the initial vector.
    int k = 0;
    for (int i = 0; i < num_of_buckets; i++) {
        if (buckets[i].size() > 0) {
            sort(buckets[i].begin(), buckets[i].end());
            for (unsigned int j = 0; j < buckets[i].size(); j++) {
                vector[k++] = buckets[i][j];
            }
        }
    }
}
