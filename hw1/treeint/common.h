#ifndef COMMON_H
#define COMMON_H

#include <stddef.h> /* offsetof */
#include <stdio.h>
#include <stdlib.h>

#define container_of(ptr, type, member) \
    ((type *) ((char *) (ptr) - (offsetof(type, member))))

static inline int throw_err(const char *str)
{
    fprintf(stderr, "%s\n", str);
    exit(-1);
}

#define todo() throw_err("Function not done yet :(")

//#define PRINT_DEBUG
#ifdef PRINT_DEBUG
#define pr_debug(...)                \
    do {                             \
        fprintf(stderr, __VA_ARGS__) \
    } while (0)
#else
#define pr_debug(...) \
    do {              \
    } while (0)
#endif

#define unlikely(x) __builtin_expect(!!(x), 0)

#endif
