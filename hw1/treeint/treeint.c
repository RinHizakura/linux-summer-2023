#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "common.h"
#include "treeint_rb.h"
#include "treeint_st.h"

struct treeint_ops {
    void *(*init)();
    int (*destroy)(void *);
    int (*insert)(void *, int);
    void *(*find)(void *, int);
    int (*remove)(void *, int);
    void (*dump)(void *ctx, enum dump_mode);
};

static struct treeint_ops *ops;

static struct treeint_ops st_ops = {
    .init = treeint_st_init,
    .destroy = treeint_st_destroy,
    .insert = treeint_st_insert,
    .find = treeint_st_find,
    .remove = treeint_st_remove,
    .dump = treeint_st_dump,
};

static struct treeint_ops rbtree_ops = {
    .init = treeint_rb_init,
    .destroy = treeint_rb_destroy,
    .insert = treeint_rb_insert,
    .find = treeint_rb_find,
    .remove = treeint_rb_remove,
    .dump = treeint_rb_dump,
};

#define rand_key(sz) rand() % ((sz) -1)

/* Just a naive implementation to benchmark a code block. */
#define bench(statement)                                                  \
    ({                                                                    \
        struct timespec _tt1, _tt2;                                       \
        clock_gettime(CLOCK_MONOTONIC, &_tt1);                            \
        statement;                                                        \
        clock_gettime(CLOCK_MONOTONIC, &_tt2);                            \
        long long time = (long long) (_tt2.tv_sec * 1e9 + _tt2.tv_nsec) - \
                         (long long) (_tt1.tv_sec * 1e9 + _tt1.tv_nsec);  \
        time;                                                             \
    })

int main(int argc, char *argv[])
{
    if (argc < 4) {
        printf("usage: treeint <algo> <tree size> <seed>\n");
        return -1;
    }

    if (strcmp(argv[1], "s-tree") == 0) {
        ops = &st_ops;
    } else if (strcmp(argv[1], "rbtree") == 0) {
        ops = &rbtree_ops;
    } else {
        printf("Invalid algorithm %s\n", argv[1]);
        return -2;
    }

    size_t tree_size = 0;
    if (!sscanf(argv[2], "%ld", &tree_size)) {
        printf("Invalid tree size %s\n", argv[2]);
        return -3;
    }

    /* Note: seed 0 is reserved as special value, it will
     * perform linear operatoion. */
    size_t seed = 0;
    if (!sscanf(argv[3], "%ld", &seed)) {
        printf("Invalid seed %s\n", argv[3]);
        return -3;
    }

    srand(seed);

    void *ctx = ops->init();

    for (size_t i = 0; i < tree_size; ++i) {
        int v = seed ? rand_key(tree_size) : i;
        long long insert_time = bench(ops->insert(ctx, v));
        printf("%lld,", insert_time);
    }
    printf("\n");

    pr_debug("[ After insertions ]\n");
    ops->dump(ctx, LEVEL_ORDER);

    for (size_t i = 0; i < tree_size; ++i) {
        int v = seed ? rand_key(tree_size) : i;
        long long find_time = bench(ops->find(ctx, v));
        printf("%lld,", find_time);
    }
    printf("\n");

    pr_debug("Removing...\n");
    for (size_t i = 0; i < tree_size; ++i) {
        int v = seed ? rand_key(tree_size) : i;
        long long remove_time = bench(ops->remove(ctx, v));
        printf("%lld,", remove_time);
    }
    printf("\n");

    pr_debug("[ After removals ]\n");
    ops->dump(ctx, LEVEL_ORDER);

    ops->destroy(ctx);

    return 0;
}
