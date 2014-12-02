#include <cstdlib>
#include <vector>
#include <algorithm>
#include "bucket_sort_seq.h"

using namespace std;
using std::vector;
using std::sort;

void bucketSort (int* vector, int size, int range, int num_of_buckets) {
    std::vector<std::vector<int> > buckets(num_of_buckets, std::vector<int>());
    int min = vector[0];
    int max = vector[0];
    for (int i = 0; i < size; i++) {
        min = vector[i] < min ? vector[i] : min;
        max = vector[i] > max ? vector[i] : max;
    }
    float delta = max - min;

    // place values in buckets
    for (int i = 0; i < size; i++) {
        int bucket = (vector[i] - min)/delta * (num_of_buckets - 1);
        buckets[bucket].push_back(vector[i]);
    }
    int k = 0;
    // sort buckets and place the values in the initial vector
    for (int i = 0; i < num_of_buckets; i++) {
        if (buckets[i].size() > 0) {
            sort(buckets[i].begin(), buckets[i].end());
            for (unsigned int j = 0; j < buckets[i].size(); j++) {
                vector[k++] = buckets[i][j];
            }
        }
    }
}
