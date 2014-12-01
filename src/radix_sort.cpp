#include "vector_generator.h"

using namespace std;

int main (int argc, char** argv) {
    if (argc < 3) {
        cout << "Invalid number of parameters. Usage:" << endl;
        cout << "./seq #NUM_OF_ELEMS #RANGE" << endl;
        cout << "where #RANGE is an unsigned int and is the upper bound that "
             << "an element in the input vector can take." << endl;
        exit (-1);
    }

    int num_of_elems = atoi (argv[1]); 
    int range = atoi (argv[2]);
    int* vector = generate_same_value_vector (num_of_elems, range);

    for (int i = 0; i < num_of_elems; i++) {
        cout << vector[i] << " ";
    }
    cout << endl;

    return 0;
}
