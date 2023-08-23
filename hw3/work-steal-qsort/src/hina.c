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

    atomic_size_t active;
    atomic_bool done;

    struct list_head list;
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

static void do_work(work_t *work)
{
    if (work) {
        (work->code)(work->args);
        (work->dtor)(work->args);
        atomic_fetch_sub_explicit(&hina.active, 1, memory_order_relaxed);
    }
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
                if (atomic_load(&hina.done))
                    break;
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

    INIT_LIST_HEAD(&hina.list);

    for (int i = 0; i < N_THREADS; ++i) {
        deque_init(&thread_queues[i], 8);
        hina.tids[i] = i;
    }
}

void hina_spawn(task_t task, dtor_t dtor, void *args)
{
    work_t *work = malloc(sizeof(work_t));
    work->code = task;
    work->dtor = dtor;
    work->join_count = 0;
    work->args = args;
    list_add_tail(&work->node, &hina.list);

    atomic_fetch_add_explicit(&hina.active, 1, memory_order_relaxed);

    /* FIXME: Do we need to pick up the availibe queue for load balancing? Or
     * the stealer can do this implicitly? */
    push(&thread_queues[0], work);
}

void hina_run()
{
    for (int i = 0; i < N_THREADS; ++i) {
        if (pthread_create(&hina.threads[i], NULL, thread, &hina.tids[i]) !=
            0) {
            perror("Failed to start the thread");
            exit(EXIT_FAILURE);
        }
    }
}

void hina_exit()
{
    /* FIXME: busy waiting until there're no active job */
    while (atomic_load_explicit(&hina.active, memory_order_relaxed) != 0)
        ;

    atomic_store(&hina.done, true);

    for (int i = 0; i < N_THREADS; ++i) {
        if (pthread_join(hina.threads[i], NULL) != 0) {
            perror("Failed to join the thread");
            exit(EXIT_FAILURE);
        }
    }

    work_t *work, *tmp;
    list_for_each_entry_safe (work, tmp, &hina.list, node) {
        list_del(&work->node);
        free(work);
    }

    for (int i = 0; i < N_THREADS; ++i) {
        deque_free(&thread_queues[i]);
    }
    free(thread_queues);
}
