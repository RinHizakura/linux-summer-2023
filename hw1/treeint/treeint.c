#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "treeint_st.h"
#include "treeint_rb.h"

struct treeint_ops {
    void *(*init)();
    int (*destroy)(void *);
    int (*insert)(void *, int);
    void *(*find)(void *, int);
    int (*remove)(void *, int);
    void (*dump)(void *ctx);
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
    .init = treeint_rbtree_init,
    .destroy = treeint_rbtree_destroy,
    .insert = treeint_rbtree_insert,
    .find = treeint_rbtree_find,
    .remove = treeint_rbtree_remove,
    .dump = treeint_rbtree_dump,
};

#define rand_key(sz) rand() % ((sz) -1)

/* Just a naive implementation to benchmark a code block. */
#define bench(statement)                                                  \
    ({                                                                    \
        struct timespec _tt1, _tt2;                                       \
        clock_gettime(CLOCK_MONOTONIC, &_tt1);                            \
        statement clock_gettime(CLOCK_MONOTONIC, &_tt2);                  \
        long long time = (long long) (_tt2.tv_sec * 1e9 + _tt2.tv_nsec) - \
                         (long long) (_tt1.tv_sec * 1e9 + _tt1.tv_nsec);  \
        time;                                                             \
    })

int main(int argc, char *argv[])
{
    if (argc < 3) {
        printf("usage: treeint <algo> <tree size>\n");
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

    srand(time(0));

    void *ctx = ops->init();

    long long insert_time = bench(for (size_t i = 0; i < tree_size; ++i)
                                      ops->insert(ctx, rand_key(tree_size)););

    pr_debug("[ After insertions ]\n");
    ops->dump(ctx);

    long long find_time = bench(for (size_t i = 0; i < tree_size; ++i)
                                    ops->find(ctx, rand_key(tree_size)););

    pr_debug("Removing...\n");
    long long remove_time = bench(for (size_t i = 0; i < tree_size; ++i) {
        int v = rand_key(tree_size);
        pr_debug("%2d  ", v);
        if ((i + 1) % 10 == 0) {
            pr_debug("\n");
        }
        ops->remove(ctx, v);
    });
    pr_debug("\n");

    pr_debug("[ After removals ]\n");
    ops->dump(ctx);

    ops->destroy(ctx);

    printf("%lld, %lld, %lld\n", insert_time / tree_size, find_time / tree_size,
           remove_time / tree_size);

    return 0;
}
