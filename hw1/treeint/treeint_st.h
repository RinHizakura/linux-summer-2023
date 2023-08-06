#ifndef TREEINT_ST_H
#define TREEINT_ST_H

#include "common.h"
#include "s_tree.h"

#define treeint_st_entry(ptr) container_of(ptr, struct treeint_st, st_n)

struct treeint_st {
    int value;
    struct st_node st_n;
};

static int treeint_st_cmp(struct st_node *node, void *key)
{
    struct treeint_st *n = treeint_st_entry(node);
    int value = *(int *) key;

    return n->value - value;
}

static struct st_node *treeint_st_node_create(void *key)
{
    int value = *(int *) key;
    struct treeint_st *i = calloc(sizeof(struct treeint_st), 1);
    assert(i);

    i->value = value;
    // return the st_node reference of the new node
    return &i->st_n;
}

static void treeint_st_node_destroy(struct st_node *n)
{
    struct treeint_st *i = treeint_st_entry(n);
    free(i);
}

static void *treeint_st_init()
{
    struct st_tree *tree;
    tree = st_create(treeint_st_cmp, treeint_st_node_create,
                     treeint_st_node_destroy);
    assert(tree);
    return tree;
}

static int treeint_st_destroy(void *ctx)
{
    struct st_tree *tree = (struct st_tree *) ctx;

    assert(tree);
    st_destroy(tree);
    return 0;
}

static int treeint_st_insert(void *ctx, int a)
{
    struct st_tree *tree = (struct st_tree *) ctx;
    return st_insert(tree, (void *) &a);
}

static void *treeint_st_find(void *ctx, int a)
{
    struct st_tree *tree = (struct st_tree *) ctx;
    return st_find(tree, (void *) &a);
}

static int treeint_st_remove(void *ctx, int a)
{
    struct st_tree *tree = (struct st_tree *) ctx;
    return st_remove(tree, (void *) &a);
}

static void __treeint_st_dump(struct st_node *n, int depth)
{
    if (!n)
        return;

    __treeint_st_dump(st_left(n), depth + 1);  // FFFF

    struct treeint_st *v = treeint_st_entry(n);
    pr_debug("%d\n", v->value);

    __treeint_st_dump(st_right(n), depth + 1);  // GGGG
}

static void treeint_st_dump(void *ctx)
{
    struct st_tree *tree = (struct st_tree *) ctx;
    __treeint_st_dump(st_root(tree), 0);
}

#endif
