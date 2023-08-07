#ifndef TREEINT_RBTREE_H
#define TREEINT_RBTREE_H

#include "common.h"
#include "rbtree.h"

#define treeint_rbtree_entry(ptr) \
    container_of(ptr, struct treeint_rbtree, rbtree_n)

struct treeint_rbtree {
    int value;
    struct rb_node rbtree_n;
};

static void *treeint_rbtree_init()
{
    struct rb_root *root;
    root = calloc(sizeof(struct rb_root), 1);
    *root = RB_ROOT;
    return root;
}

static void __treeint_rbtree_destroy(struct rb_node *n)
{
    if (rb_left(n))
        __treeint_rbtree_destroy(rb_left(n));

    if (rb_right(n))
        __treeint_rbtree_destroy(rb_right(n));

    struct treeint_rbtree *entry = treeint_rbtree_entry(n);
    free(entry);
}

static int treeint_rbtree_destroy(void *ctx)
{
    struct rb_root *root = (struct rb_root *) ctx;
    if (rb_root(root))
        __treeint_rbtree_destroy(rb_root(root));

    free(root);
    return 0;
}

static struct treeint_rbtree *__treeint_rbtree_insert(void *ctx, int a)
{
    struct rb_root *root = (struct rb_root *) ctx;

    struct rb_node **n = &rb_root(root);
    struct rb_node *p = NULL;
    struct treeint_rbtree *entry;

    while (*n) {
        p = *n;
        entry = treeint_rbtree_entry(p);
        if (a == entry->value)
            return NULL;

        if (a < entry->value)
            n = &(*n)->rb_left;
        else if (a > entry->value)
            n = &(*n)->rb_right;
    }

    struct treeint_rbtree *i = calloc(sizeof(struct treeint_rbtree), 1);
    assert(i);
    i->value = a;
    rb_link_node(&i->rbtree_n, p, n);
    return i;
}

static int treeint_rbtree_insert(void *ctx, int a)
{
    struct treeint_rbtree *node = __treeint_rbtree_insert(ctx, a);
    if (node == NULL)
        return -1;

    struct rb_root *root = (struct rb_root *) ctx;
    rb_insert_color(&node->rbtree_n, root);
    return 0;
}

static void *treeint_rbtree_find(void *ctx, int a)
{
    struct rb_root *root = (struct rb_root *) ctx;
    struct rb_node *n = rb_root(root);
    struct treeint_rbtree *entry;

    while (n) {
        entry = treeint_rbtree_entry(n);
        if (a == entry->value)
            return entry;

        if (a < entry->value)
            n = n->rb_left;
        else if (a > entry->value)
            n = n->rb_right;
    }

    return NULL;
}

static int treeint_rbtree_remove(void *ctx, int a)
{
    struct rb_root *root = (struct rb_root *) ctx;
    struct treeint_rbtree *entry = treeint_rbtree_find(ctx, a);

    if (!entry)
        return -1;

    rb_erase(&entry->rbtree_n, root);
    free(entry);
    return 0;
}

static void __treeint_rbtree_dump(struct rb_node *n, int depth)
{
    if (!n)
        return;

    __treeint_rbtree_dump(rb_left(n), depth + 1);

    struct treeint_rbtree *v = treeint_rbtree_entry(n);
    pr_debug("%d\n", v->value);

    __treeint_rbtree_dump(rb_right(n), depth + 1);
}

static void treeint_rbtree_dump(void *ctx)
{
    struct rb_root *root = (struct rb_root *) ctx;
    __treeint_rbtree_dump(rb_root(root), 0);
}

#endif
