bool test_is_sorted_asc (int* vector, int size) {
    for (int i = 0; i < size - 1; i++) {
        if (vector[i] > vector[i + 1]) {
            return false;
        }
    }
    return true;
}

bool test_is_sorted_desc (int* vector, int size) {
    for (int i = 0; i < size - 1; i++) {
        if (vector[i] < vector[i + 1]) {
            return false;
        }
    }
    return true;
}
