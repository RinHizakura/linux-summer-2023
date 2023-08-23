#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "hina.h"

#ifndef ELEM_T
#define ELEM_T uint32_t
#endif

static void *xmalloc(size_t s)
{
    void *p;

    if ((p = malloc(s)) == NULL) {
        perror("malloc");
        exit(1);
    }
    return (p);
}

struct qsort {
    void *a;
    size_t n;
};

static void qsort_dtor(void *args)
{
    free(args);
}

static void qsort_algo(void *args)
{
    struct qsort *q = (struct qsort *) args;
    int n = q->n;

    printf("qsort_algo start %d\n", n);
    /* TODO */
}

int main()
{
    size_t nelem = 10000000;
    ELEM_T *int_elem = xmalloc(nelem * sizeof(ELEM_T));
    for (size_t i = 0; i < nelem; i++)
        int_elem[i] = rand() % nelem;

    hina_init();

    struct qsort *q = xmalloc(sizeof(struct qsort));
    q->a = int_elem;
    q->n = nelem;
    hina_spawn(qsort_algo, qsort_dtor, q);

    hina_exit();

    free(int_elem);
    return 0;
}
