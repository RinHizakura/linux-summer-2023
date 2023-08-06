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

int treeint_remove(int a)
{
    return st_remove(tree, (void *) &a);
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
