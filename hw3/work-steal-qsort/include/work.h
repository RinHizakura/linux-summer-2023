#ifndef WORK_H
#define WORK_H

struct work_internal;
typedef struct work_internal *(*task_t)(struct work_internal *);
typedef struct work_internal {
    task_t *code;
    atomic_int join_count;
    void *args[];
} work_t;

#endif
