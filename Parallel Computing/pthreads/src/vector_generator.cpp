#include <cstdlib>
#include <ctime>

#include "vector_generator.h"

int compare_asc (const void* a, const void* b);
int compare_desc (const void* a, const void* b);

int* generate_random_vector (int size, int range) {
    int* vector = (int*) malloc (size * sizeof(int));
    srand(time(NULL));
	for (int i = 0; i < size; i++) {
		vector[i] = rand() % range + 1;
    }
    return vector;
}

int* generate_asc_sorted_vector (int size, int range) {
    int* vector = generate_random_vector (size, range);
    qsort (vector, size, sizeof (int), compare_asc);
    return vector;
}

int* generate_desc_sorted_vector (int size, int range) {
    int* vector = generate_random_vector (size, range);
    qsort (vector, size, sizeof (int), compare_desc);
    return vector;
}

int* generate_same_value_vector (int size, int range) {
    int* vector = (int*) malloc (size * sizeof(int));
    srand(time(NULL));
    int value = rand() % range + 1;
	for (int i = 0; i < size; i++) {
        vector[i] = value;
    }
    return vector;
}

int compare_asc (const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

int compare_desc (const void* a, const void* b) {
    return (*(int*)b - *(int*)a);
}
