#include <list>

#include "vector_generator.h"

using namespace std;

void radix (int* vector, int size, int range, int num_of_buckets);

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

    for (int i = 0; i < num_of_elems; i++) {
        cout << vector[i] << " ";
    }
    cout << endl;

    return 0;
}

void radix (int* vector, int size, int range, int num_of_buckets) {
	list<int> bucket[num_of_buckets];
	int i;
	// iterate through each radix until n>range
	for (int n = 1; range >= n; n *= num_of_buckets) {
		// sort list of numbers into buckets
		for (i = 0; i < size; i++)
			bucket[(vector[i] / n) % num_of_buckets].push_back(vector[i]);
 
		// merge buckets back to list
		for (int k = i = 0; i < num_of_buckets; bucket[i++].clear())
			for (list<int>::iterator j = bucket[i].begin();
                    j != bucket[i].end(); vector[k++] = *(j++));
	}
}
