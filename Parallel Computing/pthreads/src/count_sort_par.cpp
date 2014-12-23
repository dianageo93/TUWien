#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <vector>
#include <time.h>

#include "count_sort_par.h"

#define POOL_MAX_SIZE 10000
#define MAX_NO_THREADS 200

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
    int *v, index, offset, count, start_pos;
    buckets_t *bucketContainer;

    sort_t (int *v, int index, int offset, int count, int start_pos,
            buckets_t *bucketContainer) : v(v), index(index), offset(offset),
    count(count), start_pos(start_pos), bucketContainer(bucketContainer)
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
    //int *v, index, offset, count, start_pos;

    int written_elems = 0;
    int index = s->index;
    int offset = s->offset;
    while (written_elems < s->count) {
        while (written_elems < s->count && offset < s->bucketContainer->buckets[index]) {
            s->v[written_elems + s->start_pos] = index;
            ++offset;
            ++written_elems;
        }
        ++index;
        offset = 0;
    }
    return NULL;
}

void countSort_par (int* vector, int size, int range, int num_cores) {
    R = range;
    int num_buckets = range + 1;
    buckets_t *bucketContainer = new buckets_t (num_buckets);

    int max_threads = num_cores * 5;
    int block = size / max_threads;

    struct timespec begin, end;
    double elapsed;
    clock_gettime (CLOCK_MONOTONIC, &begin);

    tpool_t *pool = pool_init (max_threads + 1, POOL_MAX_SIZE);

    int num_tasks = max_threads;
    if (size % num_cores != 0) {
        ++num_tasks;
    }

    pool->num_tasks = num_tasks;
    // Partition the input vector into buckets.
    for (int i = 0; i < max_threads; i++) {
        partition_t *p = new partition_t (vector, i * block, (i+1) * block,
                bucketContainer);
        tpool_insert (pool, partition, (void*)p);
    }

    if (size % num_cores != 0) {
        partition_t *p = new partition_t (vector, max_threads * block, size,
                bucketContainer);
        tpool_insert (pool, partition, (void*)p);
    }

    pthread_mutex_lock (&(pool->lock));
    while (pool->num_tasks > 0) {
        pthread_cond_wait (&(pool->done), &(pool->lock));
    }
    pthread_mutex_unlock (&(pool->lock));

    clock_gettime (CLOCK_MONOTONIC, &end);
    elapsed = end.tv_sec - begin.tv_sec;
    elapsed += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;
    cout << "Partition time: " << elapsed << endl;

    // Sort the buckets and place the values in the initial vector.
    clock_gettime (CLOCK_MONOTONIC, &begin);

    pool->num_tasks = max_threads;
    if (size % max_threads != 0) {
        ++pool->num_tasks;
    }

    int index = 0;
    int taken_from_index = 0;
    for (int i = 0; i < max_threads;) {
        int count = 0; 
        int offset = taken_from_index;
        int start_index = index;
        while (count < block) {
            if (count + bucketContainer->buckets[index] - taken_from_index >= block) {
                taken_from_index += block - count;
                count += block;
                sort_t *s = new sort_t (vector, start_index, offset, block,
                        i * block, bucketContainer);
                tpool_insert (pool, sortBucket, (void*)s);
            }
            else {
                count += bucketContainer->buckets[index] - taken_from_index;
                taken_from_index = 0;
                ++index;
            }
        }
        i++;
    }
    if (size % max_threads != 0) {
        sort_t *s = new sort_t (vector, index, taken_from_index, size % max_threads,
                max_threads * block, bucketContainer);
        tpool_insert (pool, sortBucket, (void*)s);
    }

    pthread_mutex_lock (&(pool->lock));
    while (pool->num_tasks > 0) {
        pthread_cond_wait (&(pool->done), &(pool->lock));
    }
    pthread_mutex_unlock (&(pool->lock));

    clock_gettime (CLOCK_MONOTONIC, &end);
    elapsed = end.tv_sec - begin.tv_sec;
    elapsed += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;
    cout << "Value placement time: " << elapsed << endl;

    for (int i = 0; i < max_threads; i++) {
        pthread_cancel (pool->threads[i]);
    }
}
};
