#include <assert.h>
#include <stddef.h> /* offsetof */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "s_tree.h"

//#define PRINT_DEBUG

#define container_of(ptr, type, member) \
    ((type *) ((char *) (ptr) - (offsetof(type, member))))

#define treeint_entry(ptr) container_of(ptr, struct treeint, st_n)

#ifdef PRINT_DEBUG
#define pr_debug(...) fprintf(stderr, __VA_ARGS__)
#else
#define pr_debug(...)
#endif

struct treeint {
    int value;
    struct st_node st_n;
};

static struct st_tree *tree;

static int treeint_cmp(struct st_node *node, void *key)
{
    struct treeint *n = container_of(node, struct treeint, st_n);
    int value = *(int *) key;

    return n->value - value;
}

static struct st_node *treeint_node_create(void *key)
{
    int value = *(int *) key;
    struct treeint *i = calloc(sizeof(struct treeint), 1);
    assert(i);

    i->value = value;
    // return the st_node reference of the new node
    return &i->st_n;
}

static void treeint_node_destroy(struct st_node *n)
{
    struct treeint *i = treeint_entry(n);
    free(i);
}

int treeint_init()
{
    tree = st_create(treeint_cmp, treeint_node_create, treeint_node_destroy);
    assert(tree);
    return 0;
}

int treeint_destroy()
{
    assert(tree);
    st_destroy(tree);
    return 0;
}

int treeint_insert(int a)
{
    return st_insert(tree, (void *) &a);
}

struct st_node *treeint_find(int a)
{
    return st_find(tree, (void *) &a);
}

int treeint_remove(int a)
{
    return st_remove(tree, (void *) &a);
}

/* ascending order */
#ifdef PRINT_DEBUG
static void __treeint_dump(struct st_node *n, int depth)
{
    if (!n)
        return;

    __treeint_dump(st_left(n), depth + 1);  // FFFF

    struct treeint *v = treeint_entry(n);
    pr_debug("%d\n", v->value);

    __treeint_dump(st_right(n), depth + 1);  // GGGG
}
#else
static void __treeint_dump(__attribute__((unused)) struct st_node *n,
                           __attribute__((unused)) int depth)
{
    // do nothing
}
#endif

void treeint_dump()
{
    __treeint_dump(st_root(tree), 0);
}

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

    if (strcmp(argv[1], "baseline") != 0) {
        // TODO: Let's support algorithm other than baseline?
        printf("Invalid algorithm %s\n", argv[1]);
        return -3;
    }

    size_t tree_size = 0;
    if (!sscanf(argv[2], "%ld", &tree_size)) {
        printf("Invalid tree size %s\n", argv[2]);
        return -3;
    }

    srand(time(0));

    treeint_init();

    long long insert_time = bench(for (size_t i = 0; i < tree_size; ++i)
                                      treeint_insert(rand_key(tree_size)););

    pr_debug("[ After insertions ]\n");
    treeint_dump();

    long long find_time = bench(for (size_t i = 0; i < tree_size; ++i)
                                    treeint_find(rand_key(tree_size)););

    pr_debug("Removing...\n");
    long long remove_time = bench(for (size_t i = 0; i < tree_size; ++i) {
        int v = rand_key(tree_size);
        pr_debug("%2d  ", v);
        if ((i + 1) % 10 == 0) {
            pr_debug("\n");
        }
        treeint_remove(v);
    });
    pr_debug("\n");

    pr_debug("[ After removals ]\n");
    treeint_dump();

    treeint_destroy();

    printf("%lld, %lld, %lld\n", insert_time / tree_size, find_time / tree_size,
           remove_time / tree_size);

    return 0;
}
