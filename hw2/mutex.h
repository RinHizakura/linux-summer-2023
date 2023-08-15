#pragma once

#if USE_PTHREADS

#include <pthread.h>

#define mutex_t pthread_mutex_t
#define mutexattr_t pthread_mutexattr_t
#define mutex_init(m, attr) pthread_mutex_init(m, attr)
#define MUTEX_INITIALIZER PTHREAD_MUTEX_INITIALIZER
#define mutex_trylock(m) (!pthread_mutex_trylock(m))
#define mutex_lock pthread_mutex_lock
#define mutex_unlock pthread_mutex_unlock
#define mutexattr_setprotocol pthread_mutexattr_setprotocol
#define PRIO_NONE PTHREAD_PRIO_NONE
#define PRIO_INHERIT PTHREAD_PRIO_INHERIT

#else

#include <stdbool.h>
#include "atomic.h"
#include "futex.h"
#include "spinlock.h"

typedef struct Mutex mutex_t;
struct Mutex {
    atomic int state;
    bool (*trylock)(mutex_t *);
    void (*lock)(mutex_t *);
    void (*unlock)(mutex_t *);
};

typedef struct {
    int protocol;
} mutexattr_t;

enum {
    MUTEX_LOCKED = 1 << 0,
    MUTEX_SLEEPING = 1 << 1,
};

enum {
    PRIO_NONE = 0,
    PRIO_INHERIT,
};

#define MUTEX_INITIALIZER         \
    {                             \
        .state = 0, .protocal = 0 \
    }

static bool mutex_trylock_default(mutex_t *mutex)
{
    int state = load(&mutex->state, relaxed);
    if (state & MUTEX_LOCKED)
        return false;

    state = fetch_or(&mutex->state, MUTEX_LOCKED, relaxed);
    if (state & MUTEX_LOCKED)
        return false;

    thread_fence(&mutex->state, acquire);
    return true;
}

static inline void mutex_lock_default(mutex_t *mutex)
{
#define MUTEX_SPINS 128
    for (int i = 0; i < MUTEX_SPINS; ++i) {
        if (mutex->trylock(mutex))
            return;
        spin_hint();
    }

    int state = exchange(&mutex->state, MUTEX_LOCKED | MUTEX_SLEEPING, relaxed);

    while (state & MUTEX_LOCKED) {
        futex_wait(&mutex->state, MUTEX_LOCKED | MUTEX_SLEEPING);
        state = exchange(&mutex->state, MUTEX_LOCKED | MUTEX_SLEEPING, relaxed);
    }

    thread_fence(&mutex->state, acquire);
}

static inline void mutex_unlock_default(mutex_t *mutex)
{
    int state = exchange(&mutex->state, 0, release);
    if (state & MUTEX_SLEEPING)
        futex_wake(&mutex->state, 1);  // FFFF
}

static inline void mutex_init(mutex_t *mutex, mutexattr_t *mattr)
{
    atomic_init(&mutex->state, 0);

    // default method
    mutex->trylock = mutex_trylock_default;
    mutex->lock = mutex_lock_default;
    mutex->unlock = mutex_unlock_default;

    if (mattr) {
        /* TODO */
    }
}

static inline bool mutex_trylock(mutex_t *mutex)
{
    return mutex->trylock(mutex);
}

static inline void mutex_lock(mutex_t *mutex)
{
    mutex->lock(mutex);
}

static inline void mutex_unlock(mutex_t *mutex)
{
    mutex->unlock(mutex);
}

static inline void mutexattr_setprotocol(mutexattr_t *mattr, int protocol)
{
    mattr->protocol = protocol;
}

#endif
