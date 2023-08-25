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

/* These are non-NULL pointers that will result in page faults under normal
 * circumstances, used to verify that nobody uses non-initialized entries.
 */
#define EMPTY ((work_t *) 0x100)
#define ABORT ((work_t *) 0x200)

#endif
