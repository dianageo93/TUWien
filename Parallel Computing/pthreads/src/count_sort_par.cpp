#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <vector>

#include "count_sort_par.h"

namespace count {
using namespace std;

int R; 

//pthread_create (&(tpl->threads[i]), NULL, tpool_thread, (void*) tpl);

struct partition_t {
    int *v, start, end;
    int *bucketContainer;

    partition_t (int *v, int start, int end, int *bucketContainer) : v(v),
    start(start), end(end), bucketContainer(bucketContainer)
    {}
};

struct sort_t {
    int *v, *prefix, index, end;
    int *bucketContainer;

    sort_t (int *v, int *prefix, int index, int end, int *bucketContainer) : v(v),
        prefix(prefix), index(index), end(end), bucketContainer(bucketContainer)
    {}
};

void *partition (void *p_void) {
    partition_t *p = (partition_t*) p_void;
    int *tmp = new int[R+1]();
    // for Saturn
    //int tmp[R+1] = {0};
    for (int i = p->start; i < p->end; i++) {
        ++tmp[p->v[i]];
    }
    for (int i = 0; i < R+1; i++) {
        __sync_add_and_fetch(&p->bucketContainer[i], tmp[i]);
    }
    return NULL;
}

void *sortBucket(void *s_void) {
    sort_t *s = (sort_t*) s_void;
    for (int j = s->index; j < s->end; j++) {
        int offset = s->prefix[j];
        for (int i = 0; i < s->bucketContainer[j]; i++) {
            s->v[offset + i] = j;
        }
    }
    return NULL;
}

void countSort_par (int* vector, int size, int range, int num_cores) {
    R = range;
    int num_buckets = range + 1;
    int *bucketContainer = new int[num_buckets];
    int max_threads = num_cores * 1;
    int block = size / max_threads;

    pthread_t threads[max_threads]; 

    // Partition the input vector into buckets.
    for (int i = 0; i < max_threads; i++) {
        partition_t *p = new partition_t (vector, i * block, (i+1) * block,
                bucketContainer);
        pthread_create (&threads[i], NULL, partition, (void*)p);
    }

    if (size % max_threads != 0) {
        partition_t *p = new partition_t (vector, max_threads * block, size,
                bucketContainer);
        partition ((void*)p);
    }

    for (int i = 0; i < max_threads; i++) {
        pthread_join (threads[i], NULL);
    }

    // Sort the buckets and place the values in the initial vector.
    int prefixSum[num_buckets + 1];
    prefixSum[0] = 0;
    for (int i = 0; i < num_buckets; i++) {
        prefixSum[i + 1] = bucketContainer[i] + prefixSum[i];
    }

    int prefix_block = num_buckets / max_threads;
    for (int i = 0; i < max_threads; i++) {
        sort_t *s = new sort_t (vector, prefixSum, i * prefix_block,
                (i+1) * prefix_block, bucketContainer);
        pthread_create (&threads[i], NULL, sortBucket, (void*)s);
    }

    if (num_buckets % max_threads != 0) {
        sort_t *s = new sort_t (vector, prefixSum, max_threads * prefix_block,
                num_buckets, bucketContainer);
        sortBucket ((void*)s);
    }

    for (int i = 0; i < max_threads; i++) {
        pthread_join (threads[i], NULL);
    }
}
};
