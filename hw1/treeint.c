#include <assert.h>
#include <stddef.h> /* offsetof */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "s_tree.h"

#define container_of(ptr, type, member) \
    ((type *) ((char *) (ptr) - (offsetof(type, member))))

#define treeint_entry(ptr) container_of(ptr, struct treeint, st_n)

struct treeint {
    int value;
    struct st_node st_n;
};

static struct st_tree *tree;

static int treeint_cmp(const void *a, const void *b)
{
    return *(int *) a - *(int *) b;
}

static struct st_node *treeint_node_create()
{
    // TODO
    return NULL;
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

struct treeint *treeint_insert(int a)
{
    struct st_node *p = NULL;
    enum st_dir d;
    for (struct st_node *n = st_root(tree); n;) {
        struct treeint *t = container_of(n, struct treeint, st_n);
        if (a == t->value)
            return t;

        p = n;

        if (a < t->value) {
            n = st_left(n);
            d = LEFT;
        } else if (a > t->value) {
            n = st_right(n);
            d = RIGHT;
        }
    }

    struct treeint *i = calloc(sizeof(struct treeint), 1);
    if (st_root(tree))
        st_insert(&st_root(tree), p, &i->st_n, d);
    else
        st_root(tree) = &i->st_n;

    i->value = a;
    return i;
}

struct treeint *treeint_find(int a)
{
    struct st_node *n = st_root(tree);
    while (n) {
        struct treeint *t = treeint_entry(n);
        if (a == t->value)
            return t;

        if (a < t->value)
            n = st_left(n);
        else if (a > t->value)
            n = st_right(n);
    }

    return 0;
}

int treeint_remove(int a)
{
    struct treeint *n = treeint_find(a);
    if (!n)
        return -1;

    st_remove(&st_root(tree), &n->st_n);
    free(n);
    return 0;
}

/* ascending order */
static void __treeint_dump(struct st_node *n, int depth)
{
    if (!n)
        return;

    __treeint_dump(st_left(n), depth + 1);  // FFFF

    struct treeint *v = treeint_entry(n);
    printf("%d\n", v->value);

    __treeint_dump(st_right(n), depth + 1);  // GGGG
}

void treeint_dump()
{
    __treeint_dump(st_root(tree), 0);
}

int main()
{
    srand(time(0));

    treeint_init();

    for (int i = 0; i < 100; ++i)
        treeint_insert(rand() % 99);

    printf("[ After insertions ]\n");
    treeint_dump();

    printf("Removing...\n");
    for (int i = 0; i < 100; ++i) {
        int v = rand() % 99;
        printf("%2d  ", v);
        if ((i + 1) % 10 == 0)
            printf("\n");
        treeint_remove(v);
    }
    printf("\n");

    printf("[ After removals ]\n");
    treeint_dump();

    treeint_destroy();

    return 0;
}
