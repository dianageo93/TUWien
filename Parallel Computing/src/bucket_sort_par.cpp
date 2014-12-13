#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <vector>

#include "bucket_sort_par.h"

#define POOL_MAX_SIZE 10000

namespace bucket {
using namespace std;

struct buckets_t {
    int size;
    vector<vector<int> > buckets;
    pthread_mutex_t *mutex;

    buckets_t (int size) : size(size) {
        buckets.resize(size);
        mutex = new pthread_mutex_t[size];
        for (int i = 0; i < size; i++) {
            pthread_mutex_init(&mutex[i], NULL);
        }
    }
};

struct work_t {
    void (*routine)(void*);
    void *arg;
    work_t *next;
};

struct tpool_t {
    int num_threads, max_size, curr_size, num_tasks;
    pthread_t *threads;
    work_t *head, *tail;
    pthread_mutex_t lock;
    pthread_cond_t not_empty, not_full, done;
};

void* tpool_thread (void* arg);

tpool_t* pool_init (int num_threads, int max_size, int num_tasks) {
    tpool_t *tpl = new tpool_t;
    tpl->num_threads = num_threads;
    tpl->max_size = max_size;
    tpl->num_tasks = num_tasks;
    tpl->curr_size = 0;
    tpl->head = tpl->tail = NULL;

    pthread_mutex_init (&(tpl->lock), NULL);
    pthread_cond_init (&(tpl->not_empty), NULL);
    pthread_cond_init (&(tpl->not_full), NULL);
    pthread_cond_init (&(tpl->done), NULL);
    tpl->threads = new pthread_t[num_threads];
    for (int i = 0; i < num_threads; i++) {
        pthread_create (&(tpl->threads[i]), NULL, tpool_thread, (void*) tpl);
    }
    return tpl;
}

void* tpool_thread (void* arg) {
    work_t *work;
    tpool_t *tpl = (tpool_t*) arg;

    while (true) {
        pthread_mutex_lock (&(tpl->lock));
        while (tpl->curr_size == 0) {
            pthread_cond_wait (&(tpl->not_empty), &(tpl->lock));
        }
        work = tpl->head;
        tpl->curr_size--;
        if (tpl->curr_size == 0) {
            tpl->head = tpl->tail = NULL;
        }
        else {
            tpl->head = work->next;
        }
        if (tpl->curr_size <= tpl->max_size - 1) {
            pthread_cond_broadcast (&(tpl->not_full));
        }
        pthread_mutex_unlock (&(tpl->lock));

        (*(work->routine))(work->arg);
        pthread_mutex_lock (&(tpl->lock));
        tpl->num_tasks--;
        if (tpl->num_tasks == 0) {
            pthread_cond_signal (&(tpl->done));
        }
        pthread_mutex_unlock (&(tpl->lock));
        delete work;
    }
    return NULL;
}

void tpool_insert (tpool_t *tpl, void (*routine)(void*), void *arg) {
    work_t *work;

    pthread_mutex_lock (&(tpl->lock));
    while (tpl->curr_size == tpl->max_size) {
        pthread_cond_wait (&(tpl->not_full), &(tpl->lock));
    }
    work = new work_t;
    work->routine = routine;
    work->arg = arg;
    work->next = NULL;
    if (tpl->curr_size == 0) {
        tpl->head = tpl->tail = work;
        pthread_cond_signal (&(tpl->not_empty));
    }
    else {
        tpl->tail->next = work;
        tpl->tail = work;
    }
    tpl->curr_size++;
    pthread_mutex_unlock (&(tpl->lock));
}

struct partition_t {
    int *v, start, end, min;
    float delta;
    buckets_t *bucketContainer;

    partition_t (int *v, int start, int end, int min, float delta,
            buckets_t *bucketContainer) : v(v), start(start), end(end),
            min(min), delta(delta), bucketContainer(bucketContainer)
    {}
};

struct sort_t {
    int *v, *prefix, index;
    buckets_t *bucketContainer;

    sort_t (int *v, int *prefix, int index, buckets_t *bucketContainer) : v(v),
        prefix(prefix), index(index), bucketContainer(bucketContainer)
    {}
};

void sortBucket(void *s_void) {
    sort_t *s = (sort_t*) s_void;
    sort(s->bucketContainer->buckets[s->index].begin(),
         s->bucketContainer->buckets[s->index].end());
    int offset = s->prefix[s->index];
    for (unsigned int i = 0; i < s->bucketContainer->buckets[s->index].size(); i++) {
        s->v[offset + i] = s->bucketContainer->buckets[s->index][i];
    }
}

void partition (void *p_void) {
    partition_t *p = (partition_t*) p_void;
    for (int i = p->start; i < p->end; i++) {
        int bucket = ((p->v[i] - p->min) / p->delta) * (p->bucketContainer->size - 1); 
        pthread_mutex_lock (&(p->bucketContainer->mutex[bucket]));
        p->bucketContainer->buckets[bucket].push_back(p->v[i]);
        pthread_mutex_unlock (&(p->bucketContainer->mutex[bucket]));
    }
}

void bucketSort_par (int* vector, int size, int range, int num_of_buckets,
        int num_threads) {
    buckets_t *bucketContainer = new buckets_t (num_of_buckets);
    int min = vector[0];
    int max = vector[0];
    for (int i = 0; i < size; i++) {
        min = vector[i] < min ? vector[i] : min;
        max = vector[i] > max ? vector[i] : max;
    }
    float delta = max - min;
    int block = size / num_threads;

    tpool_t *pool;
    if (size % num_threads != 0) {
        pool = pool_init (num_threads, POOL_MAX_SIZE, num_threads + 1);
    } else {
        pool = pool_init (num_threads, POOL_MAX_SIZE, num_threads);
    }

    // Partition the input vector into buckets.
    for (int i = 0; i < num_threads; i++) {
        partition_t *p = new partition_t (vector, i * block, (i+1) * block, min,
                delta, bucketContainer);
        tpool_insert (pool, partition, (void*)p);
    }

    if (size % num_threads != 0) {
        partition_t *p = new partition_t (vector, num_threads * block, size,
                min, delta, bucketContainer);
        tpool_insert (pool, partition, (void*)p);
    }

    pthread_mutex_lock (&(pool->lock));
    while (pool->num_tasks > 0) {
        pthread_cond_wait (&(pool->done), &(pool->lock));
    }
    pthread_mutex_unlock (&(pool->lock));

    // Sort the buckets and place the values in the initial vector.
    int prefixSum[num_of_buckets + 1];
    prefixSum[0] = 0;
    int num_tasks = 0;
    for (int i = 0; i < num_of_buckets; i++) {
        if (bucketContainer->buckets[i].size() > 0) {
            num_tasks++;
        }
        prefixSum[i + 1] = bucketContainer->buckets[i].size() +
            prefixSum[i];
    }

    pthread_mutex_lock (&(pool->lock));
    pool->num_tasks = num_tasks;
    pthread_mutex_unlock (&(pool->lock));

    for (int i = 0; i < num_of_buckets; i++) {
        if (bucketContainer->buckets[i].size() > 0) {
            sort_t *s = new sort_t (vector, prefixSum, i, bucketContainer);
            tpool_insert (pool, sortBucket, (void*)s);
        }
    }

    pthread_mutex_lock (&(pool->lock));
    while (pool->num_tasks > 0) {
        pthread_cond_wait (&(pool->done), &(pool->lock));
    }
    for (int i = 0; i < num_threads; i++) {
        pthread_cancel (pool->threads[i]);
    }
    pthread_mutex_unlock (&(pool->lock));
}
};
