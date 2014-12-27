#include <cstdlib>
#include <ctime>
#include <iostream>
#include <mpi.h>

#include "count_sort_par.h"
#include "sort_seq.h"
#include "test_sort.h"
#include "vector_generator.h"
#include "time.h"

using namespace std;

int main (int argc, char** argv) {
    if (argc < 2) {
        cout << "Invalid number of parameters. Usage:" << endl;
        cout << "./seq #NUM_OF_ELEMS #RANGE"
             << endl;
        cout << "where #RANGE is an unsigned int and is the upper bound that "
             << "an element in the input vector can take." 
             << endl;
        exit (-1);
    }

    int size, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int num_of_elems = atoi (argv[1]); 
    int range = atoi (argv[2]);

    int *vector, *copy;
    if (rank == 0) {
        vector = generate_random_vector (num_of_elems, range);
        copy = new int[num_of_elems];
        for (int i = 0; i < num_of_elems; i++) {
            copy[i] = vector[i];
        }
    }

    count::countSort_par (copy, num_of_elems, range);
    if (rank == 0) {
        countSort_seq (vector, num_of_elems, range);
    }

    struct timespec begin, end;
    double elapsed;

    //clock_gettime (CLOCK_MONOTONIC, &begin);
    //cout << "Sequential sort\n";
    //countSort_seq(vector, num_of_elems, range);
    //clock_gettime (CLOCK_MONOTONIC, &end);
    //elapsed = end.tv_sec - begin.tv_sec;
    //elapsed += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;
    //cout << elapsed << endl;


    if (rank == 0) {
        if (test_is_sorted_asc (copy, num_of_elems)) {
            cout << "Test passed: sorted asc." << endl;
        } else {
            cout << "Test failed: sorted asc." << endl;
        }
    }

    MPI_Finalize();
    return 0;
}
