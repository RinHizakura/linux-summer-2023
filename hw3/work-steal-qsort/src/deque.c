#include "deque.h"
#include <stdlib.h>

void deque_init(deque_t *q, int size_hint)
{
    /* FIXME: These are initialized to one but not zero because
     * the wrapping behavior of unsigned integer may lead to
     * error. */
    atomic_init(&q->top, 1);
    atomic_init(&q->bottom, 1);
    array_t *a = malloc(sizeof(array_t) + sizeof(work_t *) * size_hint);
    atomic_init(&a->size, size_hint);
    atomic_init(&q->array, a);

    pthread_mutex_init(&q->lock, NULL);
}

static void deque_resize(deque_t *q)
{
    array_t *a = atomic_load_explicit(&q->array, memory_order_relaxed);
    size_t old_size = a->size;
    size_t new_size = old_size * 2;
    array_t *new = malloc(sizeof(array_t) + sizeof(work_t *) * new_size);
    atomic_init(&new->size, new_size);
    size_t t = atomic_load_explicit(&q->top, memory_order_relaxed);
    size_t b = atomic_load_explicit(&q->bottom, memory_order_relaxed);
    for (size_t i = t; i < b; i++) {
        work_t *old = atomic_load_explicit(&a->buffer[i % old_size],
                                           memory_order_relaxed);
        atomic_store_explicit(&new->buffer[i % new_size], old,
                              memory_order_relaxed);
    }

    atomic_thread_fence(memory_order_seq_cst);
    atomic_store_explicit(&q->array, new, memory_order_relaxed);
    /* The question arises as to the appropriate timing for releasing memory
     * associated with the previous array denoted by *a. In the original Chase
     * and Lev paper, this task was undertaken by the garbage collector, which
     * presumably possessed knowledge about ongoing steal operations by other
     * threads that might attempt to access data within the array.
     *
     * In our context, the responsible deallocation of *a cannot occur at this
     * point, as another thread could potentially be in the process of reading
     * from it. Thus, we opt to abstain from freeing *a in this context,
     * resulting in memory leakage. It is worth noting that our expansion
     * strategy for these queues involves consistent doubling of their size;
     * this design choice ensures that any leaked memory remains bounded by the
     * memory actively employed by the functional queues.
     */
}

work_t *deque_take(deque_t *q)
{
    pthread_mutex_lock(&q->lock);
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
            if (!atomic_compare_exchange_strong_explicit(&q->top, &t, t + 1,
                                                         memory_order_seq_cst,
                                                         memory_order_relaxed))
                /* Failed race */
                x = EMPTY;
            atomic_store_explicit(&q->bottom, b + 1, memory_order_relaxed);
        }
    } else { /* Empty queue */
        x = EMPTY;
        atomic_store_explicit(&q->bottom, b + 1, memory_order_relaxed);
    }
    pthread_mutex_unlock(&q->lock);
    return x;
}

void deque_push(deque_t *q, work_t *w)
{
    pthread_mutex_lock(&q->lock);
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
    pthread_mutex_unlock(&q->lock);
}

work_t *deque_steal(deque_t *q)
{
    pthread_mutex_lock(&q->lock);
    size_t t = atomic_load_explicit(&q->top, memory_order_acquire);
    atomic_thread_fence(memory_order_seq_cst);
    size_t b = atomic_load_explicit(&q->bottom, memory_order_acquire);
    work_t *x = EMPTY;
    if (t < b) {
        /* Non-empty queue */
        array_t *a = atomic_load_explicit(&q->array, memory_order_consume);
        x = atomic_load_explicit(&a->buffer[t % a->size], memory_order_relaxed);
        if (!atomic_compare_exchange_strong_explicit(
                &q->top, &t, t + 1, memory_order_seq_cst, memory_order_relaxed))
            /* Failed race */
            x = ABORT;
    }
    pthread_mutex_unlock(&q->lock);
    return x;
}

/* This is expected to run at the end of program, so
 * assuming no synchronization is required here. */
void deque_free(deque_t *q)
{
    pthread_mutex_destroy(&q->lock);

    /* FIXME: if resize happened, we should also
     * take care of those unused array buffer. */
    free(q->array);
}
