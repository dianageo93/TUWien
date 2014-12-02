#include <cstdlib>
#include <iostream>

#include "radix_sort.h"
#include "test_sort.h"
#include "vector_generator.h"

using namespace std;

int main (int argc, char** argv) {
    if (argc < 4) {
        cout << "Invalid number of parameters. Usage:" << endl;
        cout << "./seq #NUM_OF_ELEMS #RANGE #NO_OF_BUCKETS" << endl;
        cout << "where #RANGE is an unsigned int and is the upper bound that "
             << "an element in the input vector can take." << endl;
        exit (-1);
    }

    int num_of_elems = atoi (argv[1]); 
    int range = atoi (argv[2]);
    int num_of_buckets = atoi (argv[3]);
    int* vector = generate_random_vector (num_of_elems, range);
    radix (vector, num_of_elems, range, num_of_buckets);

    if (test_is_sorted_asc (vector, num_of_elems)) {
        cout << "Test passed: sorted asc." << endl;
    } else {
        cout << "Test failed: sorted asc." << endl;
    }

    return 0;
}