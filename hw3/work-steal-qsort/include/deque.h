#ifndef DEQUE_H
#define DEQUE_H

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
void deque_resize(deque_t *q);

#endif
