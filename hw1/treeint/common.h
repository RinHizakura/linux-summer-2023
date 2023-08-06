#ifndef COMMON_H
#define COMMON_H

#include <stddef.h> /* offsetof */

#define container_of(ptr, type, member) \
    ((type *) ((char *) (ptr) - (offsetof(type, member))))

#define todo() panic("Function not done yet :(\n")

//#define PRINT_DEBUG
#ifdef PRINT_DEBUG
#define pr_debug(...) fprintf(stderr, __VA_ARGS__)
#else
#define pr_debug(...)
#endif

#endif
