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
    /* Assume that they never overflow */
    atomic_size_t top, bottom;
    _Atomic(array_t *) array;
} deque_t;

void deque_init(deque_t *q, int size_hint);
work_t *deque_take(deque_t *q);
void deque_push(deque_t *q, work_t *w);
work_t *deque_steal(deque_t *q);
void deque_free(deque_t *q);

#endif
