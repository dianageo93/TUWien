#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <vector>

#include "count_sort_par.h"

#define POOL_MAX_SIZE 10000
#define MAX_NO_THREADS 1000

namespace count {
using namespace std;

int R;

struct buckets_t {
    int size;
    int *buckets;
    pthread_mutex_t *mutex;

    buckets_t (int size) : size(size) {
        buckets = new int[size]();
        mutex = new pthread_mutex_t[size];
        for (int i = 0; i < size; i++) {
            pthread_mutex_init(&mutex[i], NULL);
        }
    }
};

struct work_t {
    void *(*routine)(void*);
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

tpool_t* pool_init (int num_threads, int max_size) {
    tpool_t *tpl = new tpool_t;
    tpl->num_threads = num_threads;
    tpl->max_size = max_size;
    tpl->num_tasks = 0;
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

void tpool_insert (tpool_t *tpl, void* (*routine)(void*), void *arg) {
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
    int *v, start, end;
    buckets_t *bucketContainer;

    partition_t (int *v, int start, int end, buckets_t *bucketContainer) : v(v),
    start(start), end(end), bucketContainer(bucketContainer)
    {}
};

struct sort_t {
    int *v, *prefix, index;
    buckets_t *bucketContainer;

    sort_t (int *v, int *prefix, int index, buckets_t *bucketContainer) : v(v),
        prefix(prefix), index(index), bucketContainer(bucketContainer)
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
        __sync_add_and_fetch(&p->bucketContainer->buckets[i], tmp[i]);
    }
    return NULL;
}

void *sortBucket(void *s_void) {
    sort_t *s = (sort_t*) s_void;
    int offset = s->prefix[s->index];
    for (int i = 0; i < s->bucketContainer->buckets[s->index]; i++) {
        s->v[offset + i] = s->index;
    }
    return NULL;
}

void countSort_par (int* vector, int size, int range, int num_cores) {
    R = range;
    int num_buckets = range + 1;
    buckets_t *bucketContainer = new buckets_t (num_buckets);
    int block = size / num_cores;

    tpool_t *pool = pool_init (MAX_NO_THREADS, POOL_MAX_SIZE);

    // NO POOL
    pthread_t threads[num_cores + 1];
    int active_threads = num_cores;
    for (int i = 0; i < num_cores; i++) {
        partition_t *p = new partition_t (vector, i * block, (i+1) * block,
                bucketContainer);
        pthread_create (&threads[i], NULL, partition, (void*)p);
    }

    if (size % num_cores != 0) {
        partition_t *p = new partition_t (vector, num_cores * block, size,
                bucketContainer);
        pthread_create (&threads[num_cores], NULL, partition, (void*)p);
        ++active_threads;
    }
    for (int i = 0; i < active_threads; i++) {
        pthread_join (threads[i], NULL);
    }

    // Sort the buckets and place the values in the initial vector.
    int prefixSum[num_buckets + 1];
    prefixSum[0] = 0;
    int num_tasks = 0;
    for (int i = 0; i < num_buckets; i++) {
        if (bucketContainer->buckets[i] > 0) {
            ++num_tasks;
        }
        prefixSum[i + 1] = bucketContainer->buckets[i] + prefixSum[i];
    }

    // POOL
    pool->num_tasks = num_tasks;

    for (int i = 0; i < num_buckets; i++) {
        if (bucketContainer->buckets[i] > 0) {
            sort_t *s = new sort_t (vector, prefixSum, i, bucketContainer);
            tpool_insert (pool, sortBucket, (void*)s);
        }
    }

    pthread_mutex_lock (&(pool->lock));
    while (pool->num_tasks > 0) {
        pthread_cond_wait (&(pool->done), &(pool->lock));
    }
    for (int i = 0; i < MAX_NO_THREADS; i++) {
        pthread_cancel (pool->threads[i]);
    }
    pthread_mutex_unlock (&(pool->lock));
}
};
