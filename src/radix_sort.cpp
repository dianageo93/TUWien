#include <cstdlib>
#include <list>

using namespace std;

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
