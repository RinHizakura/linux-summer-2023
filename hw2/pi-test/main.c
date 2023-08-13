#include <errno.h>
#include <pthread.h>
#include <stdio.h>

#include "mutex.h"

/* NOTE: This macro is memory-wasted because it will create
 * not must required static string for debugging purpose. */
#define TRY(f)                                                  \
    do {                                                        \
        int __r;                                                \
        if ((__r = (f != 0))) {                                 \
            fprintf(stderr, "Run function %s = %d\n", #f, __r); \
            return __r;                                         \
        }                                                       \
    } while (0)

struct ctx {
    mutex_t m0;
    mutex_t m1;
};

static void ctx_init(struct ctx *ctx)
{
    mutex_init(&ctx->m0);
    mutex_init(&ctx->m1);
}

static int pthread_create_with_prio(pthread_t *thread,
                                    pthread_attr_t *attr,
                                    void *(*start_routine)(void *),
                                    void *arg,
                                    int prio)
{
    struct sched_param param;
    param.sched_priority = prio;

    TRY(pthread_attr_setschedparam(attr, &param));
    TRY(pthread_create(thread, attr, start_routine, arg));

    return 0;
}

static void *thread_low(void *arg)
{
    /* TODO */
    return NULL;
}

static void *thread_mid(void *arg)
{
    /* TODO */
    return NULL;
}

static void *thread_high(void *arg)
{
    /* TODO */
    return NULL;
}

int main()
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    TRY(pthread_attr_setschedpolicy(&attr, SCHED_FIFO));
    /* This enable the new created thread follow the attribute
     * which is provided in pthread_create */
    TRY(pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED));

    struct ctx ctx;
    ctx_init(&ctx);

    pthread_t low_t, mid_t, high_t;
    /* Create the low priority thread */
    TRY(pthread_create_with_prio(&low_t, &attr, thread_low, &ctx, 1));
    /* Create the middle priority thread */
    TRY(pthread_create_with_prio(&mid_t, &attr, thread_mid, &ctx, 2));
    /* Create the high priority thread */
    TRY(pthread_create_with_prio(&high_t, &attr, thread_high, &ctx, 3));

    TRY(pthread_join(low_t, NULL));
    TRY(pthread_join(mid_t, NULL));
    TRY(pthread_join(high_t, NULL));

    printf("Test Done\n");

    return 0;
}
