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

    double vec_gen_time;
    int *vector, *copy;
    if (rank == 0) {
        cout << "Start vector generating\n";
        double start = MPI_Wtime();
        vector = generate_random_vector (num_of_elems, range);
        copy = new int[num_of_elems];
        for (int i = 0; i < num_of_elems; i++) {
            copy[i] = vector[i];
        }
        double end = MPI_Wtime();
        vec_gen_time = end -start;
        cout << "Done vector generating: " << end - start << endl;
    }

    if (rank == 0) {
        cout << "Start parallel sort\n";
    }
    double start = MPI_Wtime();
    count::countSort_par (copy, num_of_elems, range);
    double end = MPI_Wtime();
    double elapsed = end - start;
    double max_time;
    MPI_Reduce (&elapsed, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if (rank == 0) {
        cout << "Parallel time: " << end - start << endl;
        cout << "Parallel max time: " << max_time - vec_gen_time << endl;
    }

    if (rank == 0) {
        double start_seq = MPI_Wtime();
        countSort_seq (vector, num_of_elems, range);
        double end_seq = MPI_Wtime();
        cout << "Sequential time: " << end_seq - start_seq << endl;
    }

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
