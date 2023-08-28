#include <assert.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "hina.h"

#ifndef ELEM_T
#define ELEM_T uint32_t
#endif

int num_compare(const void *a, const void *b)
{
    return (*(ELEM_T *) a - *(ELEM_T *) b);
}

typedef int cmp_t(const void *, const void *);
static inline char *med3(char *, char *, char *, cmp_t *, void *);
static inline void swapfunc(char *, char *, int, int);

#define min(a, b)           \
    ({                      \
        typeof(a) _a = (a); \
        typeof(b) _b = (b); \
        _a < _b ? _a : _b;  \
    })

/* Qsort routine from Bentley & McIlroy's "Engineering a Sort Function" */
#define swapcode(TYPE, parmi, parmj, n) \
    {                                   \
        long i = (n) / sizeof(TYPE);    \
        TYPE *pi = (TYPE *) (parmi);    \
        TYPE *pj = (TYPE *) (parmj);    \
        do {                            \
            TYPE t = *pi;               \
            *pi++ = *pj;                \
            *pj++ = t;                  \
        } while (--i > 0);              \
    }

static inline void swapfunc(char *a, char *b, int n, int swaptype)
{
    if (swaptype <= 1)
        swapcode(long, a, b, n) else swapcode(char, a, b, n)
}

#define swap(a, b)                         \
    do {                                   \
        if (swaptype == 0) {               \
            long t = *(long *) (a);        \
            *(long *) (a) = *(long *) (b); \
            *(long *) (b) = t;             \
        } else                             \
            swapfunc(a, b, es, swaptype);  \
    } while (0)

#define vecswap(a, b, n)                 \
    do {                                 \
        if ((n) > 0)                     \
            swapfunc(a, b, n, swaptype); \
    } while (0)

#define CMP(t, x, y) (cmp((x), (y)))

static inline char *med3(char *a,
                         char *b,
                         char *c,
                         cmp_t *cmp,
                         __attribute__((unused)) void *thunk)
{
    return CMP(thunk, a, b) < 0
               ? (CMP(thunk, b, c) < 0 ? b : (CMP(thunk, a, c) < 0 ? c : a))
               : (CMP(thunk, b, c) > 0 ? b : (CMP(thunk, a, c) < 0 ? a : c));
}

static void *xmalloc(size_t s)
{
    void *p;

    if ((p = malloc(s)) == NULL) {
        perror("malloc");
        exit(1);
    }
    return (p);
}

struct common {
    int swaptype; /* Code to use for swapping */
    size_t es;    /* Element size. */
    cmp_t *cmp;   /* Comparison function */
};
static struct common *qsort_common = NULL;

struct qsort {
    struct common *common;
    void *a;
    size_t n;
};

#define thunk NULL

static void qsort_algo(void *args);
static void qsort_dtor(void *args);

static void qsort_spawn(ELEM_T *int_elem, size_t nelem)
{
    assert(qsort_common);

    struct qsort *q = xmalloc(sizeof(struct qsort));
    q->a = int_elem;
    q->n = nelem;
    q->common = qsort_common;
    hina_spawn(qsort_algo, qsort_dtor, q);
}

static void qsort_algo(void *args)
{
    struct qsort *qs = (struct qsort *) args;

    char *pa, *pb, *pc, *pd, *pl, *pm, *pn;
    int d, r, swaptype, swap_cnt;
    void *a;      /* Array of elements. */
    size_t n, es; /* Number of elements; size. */
    cmp_t *cmp;
    size_t nl, nr;
    struct common *c;

    /* Initialize qsort arguments. */
    c = qs->common;
    es = c->es;
    cmp = c->cmp;
    swaptype = c->swaptype;
    a = qs->a;
    n = qs->n;
top:
    /* From here on qsort(3) business as usual. */
    swap_cnt = 0;
    if (n < 7) {
        for (pm = (char *) a + es; pm < (char *) a + n * es; pm += es)
            for (pl = pm; pl > (char *) a && CMP(thunk, pl - es, pl) > 0;
                 pl -= es)
                swap(pl, pl - es);
        return;
    }
    pm = (char *) a + (n / 2) * es;
    if (n > 7) {
        pl = (char *) a;
        pn = (char *) a + (n - 1) * es;
        if (n > 40) {
            d = (n / 8) * es;
            pl = med3(pl, pl + d, pl + 2 * d, cmp, thunk);
            pm = med3(pm - d, pm, pm + d, cmp, thunk);
            pn = med3(pn - 2 * d, pn - d, pn, cmp, thunk);
        }
        pm = med3(pl, pm, pn, cmp, thunk);
    }
    swap(a, pm);
    pa = pb = (char *) a + es;

    pc = pd = (char *) a + (n - 1) * es;
    for (;;) {
        while (pb <= pc && (r = CMP(thunk, pb, a)) <= 0) {
            if (r == 0) {
                swap_cnt = 1;
                swap(pa, pb);
                pa += es;
            }
            pb += es;
        }
        while (pb <= pc && (r = CMP(thunk, pc, a)) >= 0) {
            if (r == 0) {
                swap_cnt = 1;
                swap(pc, pd);
                pd -= es;
            }
            pc -= es;
        }
        if (pb > pc)
            break;
        swap(pb, pc);
        swap_cnt = 1;
        pb += es;
        pc -= es;
    }

    pn = (char *) a + n * es;
    r = min(pa - (char *) a, pb - pa);
    vecswap(a, pb - r, r);
    r = min(pd - pc, pn - pd - (long) es);
    vecswap(pb, pn - r, r);

    if (swap_cnt == 0) { /* Switch to insertion sort */
        r = 1 + n / 4;   /* n >= 7, so r >= 2 */
        for (pm = (char *) a + es; pm < (char *) a + n * es; pm += es)
            for (pl = pm; pl > (char *) a && CMP(thunk, pl - es, pl) > 0;
                 pl -= es) {
                swap(pl, pl - es);
                if (++swap_cnt > r)
                    goto nevermind;
            }
        return;
    }

nevermind:
    nl = (pb - pa) / es;
    nr = (pd - pc) / es;

    if (nl > 100 && nr > 100) {
        qsort_spawn(a, nl);
    } else if (nl > 0) {
        qs->a = a;
        qs->n = nl;
        qsort_algo(qs);
    }

    if (nr > 0) {
        a = pn - nr * es;
        n = nr;
        goto top;
    }
}

static void qsort_dtor(void *args)
{
    free(args);
}

int main(int argc, char *argv[])
{
    int nr_threads = 16;
    size_t nelem = 100000;
    size_t es = sizeof(ELEM_T);

    int ch;
    char *ep;
    while ((ch = getopt(argc, argv, "f:h:ln:stv")) != -1) {
        switch (ch) {
        case 'n':
            nelem = (size_t) strtol(optarg, &ep, 10);
            if (nelem == 0 || *ep != '\0') {
                warnx("illegal number, -n argument -- %s", optarg);
            }
            break;
        case 'h':
            nr_threads = (int) strtol(optarg, &ep, 10);
            if (nr_threads < 0 || *ep != '\0') {
                warnx("illegal number, -h argument -- %s", optarg);
            }
            break;
        default:
            break;
        }
    }

    ELEM_T *int_elem = xmalloc(nelem * sizeof(ELEM_T));
    for (size_t i = 0; i < nelem; i++)
        int_elem[i] = rand() % nelem;

    qsort_common = xmalloc(sizeof(struct common));
    qsort_common->swaptype =
        ((char *) int_elem - (char *) 0) % sizeof(long) || es % sizeof(long) ? 2
        : es == sizeof(long)                                                 ? 0
                             : 1;
    qsort_common->es = es;
    qsort_common->cmp = num_compare;

    hina_init(nr_threads);
    hina_run();
    qsort_spawn(int_elem, nelem);
    hina_exit();

    /* Verify the result of sorting */
    for (size_t i = 1; i < nelem; i++) {
        if (num_compare(&int_elem[i], &int_elem[i - 1]) < 0) {
            printf("Sorting failed!\n");
            break;
        }
    }

    printf("Done\n");

    free(qsort_common);
    free(int_elem);
    return 0;
}
