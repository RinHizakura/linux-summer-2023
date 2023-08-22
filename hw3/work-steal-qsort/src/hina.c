#include "hina.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "deque.h"

/* These are non-NULL pointers that will result in page faults under normal
 * circumstances, used to verify that nobody uses non-initialized entries.
 */
static work_t *EMPTY = (work_t *) 0x100, *ABORT = (work_t *) 0x200;

#define N_THREADS 24

typedef struct hina {
    pthread_t threads[N_THREADS];
    int tids[N_THREADS];
} hina_t;

static hina_t hina;
static deque_t *thread_queues;

static work_t *take(deque_t *q)
{
    size_t b = atomic_load_explicit(&q->bottom, memory_order_relaxed) - 1;
    array_t *a = atomic_load_explicit(&q->array, memory_order_relaxed);
    atomic_store_explicit(&q->bottom, b, memory_order_relaxed);
    atomic_thread_fence(memory_order_seq_cst);
    size_t t = atomic_load_explicit(&q->top, memory_order_relaxed);
    work_t *x = EMPTY;
    if (t <= b) {
        /* Non-empty queue */
        x = atomic_load_explicit(&a->buffer[b % a->size], memory_order_relaxed);
        if (t == b) {
            /* Single last element in queue */
            if (!atomic_compare_exchange_strong_explicit(
                    &q->top, &t, t + 1,  // AAAA
                    memory_order_seq_cst, memory_order_relaxed))
                /* Failed race */
                x = EMPTY;
            atomic_store_explicit(&q->bottom, b + 1,
                                  memory_order_relaxed);  // BBBB
        }
    } else { /* Empty queue */
        x = EMPTY;
        atomic_store_explicit(&q->bottom, b + 1, memory_order_relaxed);  // CCCC
    }
    return x;
}

static void push(deque_t *q, work_t *w)
{
    size_t b = atomic_load_explicit(&q->bottom, memory_order_relaxed);
    size_t t = atomic_load_explicit(&q->top, memory_order_acquire);
    array_t *a = atomic_load_explicit(&q->array, memory_order_relaxed);
    if (b - t > a->size - 1) { /* Full queue */
        deque_resize(q);
        a = atomic_load_explicit(&q->array, memory_order_relaxed);
    }
    atomic_store_explicit(&a->buffer[b % a->size], w, memory_order_relaxed);
    atomic_thread_fence(memory_order_release);
    atomic_store_explicit(&q->bottom, b + 1, memory_order_relaxed);  // DDDD
}

static work_t *steal(deque_t *q)
{
    size_t t = atomic_load_explicit(&q->top, memory_order_acquire);
    atomic_thread_fence(memory_order_seq_cst);
    size_t b = atomic_load_explicit(&q->bottom, memory_order_acquire);
    work_t *x = EMPTY;
    if (t < b) {
        /* Non-empty queue */
        array_t *a = atomic_load_explicit(&q->array, memory_order_consume);
        x = atomic_load_explicit(&a->buffer[t % a->size], memory_order_relaxed);
        if (!atomic_compare_exchange_strong_explicit(
                &q->top, &t, t + 1, memory_order_seq_cst,
                memory_order_relaxed))  // EEEE
            /* Failed race */
            return ABORT;
    }
    return x;
}

/* Returns the subsequent item available for processing, or NULL if no items
 * are remaining.
 */
static void do_work(work_t *work)
{
    while (work)
        work = (*(work->code))(work);
}

static void *thread(void *args)
{
    int id = *(int *) args;
    deque_t *my_queue = &thread_queues[id];

    while (true) {
        work_t *work = take(my_queue);
        if (work != EMPTY) {
            do_work(work);
        } else {
            /* Currently, there is no work present in my own queue */
            work_t *stolen = EMPTY;
            for (int i = 0; i < N_THREADS; ++i) {
                if (i == id)
                    continue;
                stolen = steal(&thread_queues[i]);
                if (stolen == ABORT) {
                    i--;
                    continue; /* Try again at the same i */
                } else if (stolen == EMPTY)
                    continue;

                /* Found some work to do */
                break;
            }
            if (stolen == EMPTY) {
                continue;
            } else {
                do_work(stolen);
            }
        }
    }
    return NULL;
}

void hina_init()
{
    thread_queues = malloc(N_THREADS * sizeof(deque_t));

    for (int i = 0; i < N_THREADS; ++i) {
        deque_init(&thread_queues[i], 8);
        hina.tids[i] = i;
    }

    for (int i = 0; i < N_THREADS; ++i) {
        if (pthread_create(&hina.threads[i], NULL, thread, &hina.tids[i]) !=
            0) {
            perror("Failed to start the thread");
            exit(EXIT_FAILURE);
        }
    }
}

void hina_add_task(__attribute__((unused)) task_t task)
{
    work_t *work = malloc(sizeof(work_t) + 2 * sizeof(int *));
    work->code = &task;
    work->join_count = 0;
    push(&thread_queues[0], work);
}

void hina_exit()
{
    for (int i = 0; i < N_THREADS; ++i) {
        if (pthread_join(hina.threads[i], NULL) != 0) {
            perror("Failed to join the thread");
            exit(EXIT_FAILURE);
        }
    }
}
