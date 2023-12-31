#ifndef DEQUE_H
#define DEQUE_H

#include <pthread.h>
#include <stdatomic.h>
#include "work.h"

typedef struct {
    atomic_size_t size;
    work_t *buffer[];
} array_t;

typedef struct {
    _Atomic(array_t *) array;
} handle_t;

typedef struct {
    /* Assume that they never overflow */
    atomic_size_t top, bottom;
    _Atomic(array_t *) array;
    /* This is an old array which should be released when making sure
     * that no thread needs it. We simply assume that when a new array
     * have to be created, the old one was released in time. So instead
     * of maintain a list of old array we just maintain one. */
    _Atomic(array_t *) old_array;

    handle_t *handle;
    int nr_threads;
} deque_t;

void deque_init(deque_t *q, int size_hint, int nr_threads);
work_t *deque_take(deque_t *q);
void deque_push(deque_t *q, work_t *w);
work_t *deque_steal(deque_t *q);
void deque_free(deque_t *q);

#endif
