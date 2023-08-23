#ifndef WORK_H
#define WORK_H

#include "task.h"

struct work_internal;
typedef struct work_internal {
    task_t code;
    atomic_int join_count;
    void *args[];
} work_t;

#endif
