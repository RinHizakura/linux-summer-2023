#ifndef WORK_H
#define WORK_H

#include "list.h"
#include "type.h"

struct work_internal;
typedef struct work_internal {
    task_t code;
    dtor_t dtor;
    atomic_int join_count;
    struct list_head node;
    void *args;
} work_t;

#endif
