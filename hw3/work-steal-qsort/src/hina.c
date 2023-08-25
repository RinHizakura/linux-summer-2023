#include "hina.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "deque.h"


#define N_THREADS 24

typedef struct hina {
    pthread_t threads[N_THREADS];
    int tids[N_THREADS];

    atomic_size_t active;
    atomic_bool done;

    struct list_head list;
    pthread_mutex_t list_lock;
} hina_t;

static hina_t hina;
static deque_t *thread_queues;

static void do_work(work_t *work)
{
    if (work) {
        task_t code = work->code;
        dtor_t dtor = work->dtor;
        void *args = work->args;
        code(args);
        dtor(args);
        atomic_fetch_sub_explicit(&hina.active, 1, memory_order_relaxed);
    }
}

static void *thread(void *args)
{
    int id = *(int *) args;
    deque_t *my_queue = &thread_queues[id];

    while (true) {
        work_t *work = deque_take(my_queue);
        if (work != EMPTY) {
            do_work(work);
        } else {
            /* Currently, there is no work present in my own queue */
            work_t *stolen = EMPTY;
            for (int i = 0; i < N_THREADS; ++i) {
                if (i == id)
                    continue;
                stolen = deque_steal(&thread_queues[i]);
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
    pthread_mutex_init(&hina.list_lock, NULL);

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
    work->args = args;
    work->join_count = 0;

    pthread_mutex_lock(&hina.list_lock);
    list_add_tail(&work->node, &hina.list);
    pthread_mutex_unlock(&hina.list_lock);

    atomic_fetch_add_explicit(&hina.active, 1, memory_order_relaxed);

    /* FIXME: Do we need to pick up the availibe queue for load balancing? Or
     * the stealer can do this implicitly? */
    deque_push(&thread_queues[0], work);
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

    pthread_mutex_destroy(&hina.list_lock);

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
