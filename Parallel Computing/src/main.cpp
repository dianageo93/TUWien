#include <cstdlib>
#include <ctime>
#include <iostream>

#include "sort_seq.h"
#include "bucket_sort_par.h"
#include "count_sort_par.h"
#include "test_sort.h"
#include "vector_generator.h"
#include "time.h"

using namespace std;

int main (int argc, char** argv) {
    if (argc < 4) {
        cout << "Invalid number of parameters. Usage:" << endl;
        cout << "./seq #NUM_OF_ELEMS #RANGE #NO_OF_BUCKETS [#NUM_THREADS]"
             << endl;
        cout << "where #RANGE is an unsigned int and is the upper bound that "
             << "an element in the input vector can take. #NUM_THREADS is " 
             << "optional (use only when running parallel algorithm."
             << endl;
        exit (-1);
    }

    int num_of_elems = atoi (argv[1]); 
    int range = atoi (argv[2]);
    int num_of_buckets = atoi (argv[3]);
    int num_of_threads;
    if (argc == 5) {
        num_of_threads = atoi (argv[4]);
    }
    int* vector = generate_random_vector (num_of_elems, range);

    struct timespec begin, end;
    double elapsed;
    clock_gettime (CLOCK_MONOTONIC, &begin);
    cout << "Sequential sort\n";
    countSort_seq(vector, num_of_elems, range);
    //bucketSort_seq(vector, num_of_elems, range, num_of_buckets);
    clock_gettime (CLOCK_MONOTONIC, &end);
    elapsed = end.tv_sec - begin.tv_sec;
    elapsed += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;
    cout << elapsed << endl;

    clock_gettime (CLOCK_MONOTONIC, &begin);
    cout << "Parallel sort\n";
    count::countSort_par(vector, num_of_elems, range, num_of_threads);
    //bucket::bucketSort_par(vector, num_of_elems, range, num_of_buckets, num_of_threads);
    clock_gettime (CLOCK_MONOTONIC, &end);
    elapsed = end.tv_sec - begin.tv_sec;
    elapsed += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;
    cout << elapsed << endl;


    if (test_is_sorted_asc (vector, num_of_elems)) {
        cout << "Test passed: sorted asc." << endl;
    } else {
        cout << "Test failed: sorted asc." << endl;
    }

    return 0;
}
